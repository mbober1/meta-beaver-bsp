// SPDX-License-Identifier: GPL-2.0
// Driver for RK3506 Cortex-M0 remoteproc
//
// Copyright (c) Marcin Bober <mbober1@gmail.com>


#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/remoteproc.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/firmware.h>
#include <linux/io.h>
#include <linux/arm-smccc.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/of_reserved_mem.h>

// Exported by the remoteproc core but only declared in its internal header
int rproc_elf_sanity_check(struct rproc *rproc, const struct firmware *fw);
u64 rproc_elf_get_boot_addr(struct rproc * rproc, const struct firmware * fw);
int rproc_elf_load_segments(struct rproc * rproc, const struct firmware * fw);
int rproc_elf_load_rsc_table(struct rproc * rproc, const struct firmware * fw);
struct resource_table *rproc_elf_find_loaded_rsc_table(struct rproc * rproc, const struct firmware * fw);
irqreturn_t rproc_vq_interrupt(struct rproc *rproc, int vq_id);

// Rockchip vendor TF-A SiP service
#define SIP_MCU_CFG                             0x82000028
#define ROCKCHIP_SIP_CONFIG_BUSMCU_0_ID         0x00
#define ROCKCHIP_SIP_CONFIG_MCU_CODE_START_ADDR 0x01

#define RK3506_MCU_TCM_ADDR                 0xFFF84000
#define RK3506_MCU_TCM_SIZE                     0x8000

#define RK3506_MCU_SHMEM_ADDR               0x03C00000
#define RK3506_MCU_SHMEM_SIZE                 0x100000

#define RK3506_PMU_BASE   0xFF900000
#define RK3506_CRU_BASE   0xFF9A0000
#define RK3506_GRF_BASE   0xFF288000

// RK3506 mailbox (rev 0x200, one channel per instance). Linux is the A side, the M0 the B side.
#define RK3506_MBOX_A2B_STATUS   0x04
#define RK3506_MBOX_A2B_CMD      0x08
#define RK3506_MBOX_A2B_DATA     0x0C
#define RK3506_MBOX_B2A_INTEN    0x10
#define RK3506_MBOX_B2A_STATUS   0x14
#define RK3506_MBOX_INTEN_WMSK   BIT(16)

typedef struct
{
  struct rproc *            rproc;

	struct clk_bulk_data *    clks;
	int                       num_clks;

  struct reset_control *    rst_h_m0;
  struct reset_control *    rst_m0_jtag;
  struct reset_control *    rst_hresetn_m0_ac;

  uint8_t *                 tcm_virt;
  uint8_t *                 regs_PMU;
  uint8_t *                 regs_CRU;
  uint8_t *                 regs_GRF;
  uint8_t *                 shmem_virt;

  struct platform_device *  pdev;

  void __iomem *            mbox_base;  // NULL: RPMsg disabled
  int                       mbox_irq;
} rk3506_mcu_t;

// PMU_INT_MASK_CON (0x00C): mcu_rst_dis_cfg releases the M0 reset, glb_int_mask_mcu masks its IRQs
static void rk3506_rproc_mcu_run(rk3506_mcu_t * mcu, bool arun)
{
  if (arun)
    writel(0x00060004, mcu->regs_PMU + 0x00C);
  else
    writel(0x00060002, mcu->regs_PMU + 0x00C);
}

static int rk3506_rproc_mem_alloc(struct rproc * rproc, struct rproc_mem_entry * mem)
{
  void __iomem * va = ioremap_wc(mem->dma, mem->len);

  if (!va)
  {
    dev_err(&rproc->dev, "Unable to map memory region: %pad+%zx", &mem->dma, mem->len);
    return -ENOMEM;
  }
  mem->va = (__force void *)va;
  mem->is_iomem = true;
  return 0;
}

static int rk3506_rproc_mem_release(struct rproc * rproc, struct rproc_mem_entry * mem)
{
  iounmap((__force void __iomem *)mem->va);
  return 0;
}

// Register the vrings and the vdev buffer pool from the "memory-region" list (SRAM, same address on both cores)
static int rk3506_rproc_add_carveouts(struct rproc * rproc)
{
  rk3506_mcu_t *            mcu = rproc->priv;
  struct device *           dev = &mcu->pdev->dev;
  struct of_phandle_iterator it;
  struct rproc_mem_entry *  mem;
  struct reserved_mem *     rmem;
  int                       index = 0;

  of_phandle_iterator_init(&it, dev->of_node, "memory-region", NULL, 0);
  while (of_phandle_iterator_next(&it) == 0)
  {
    rmem = of_reserved_mem_lookup(it.node);
    if (!rmem)
    {
      dev_err(dev, "unable to acquire memory-region %s", it.node->name);
      of_node_put(it.node);
      return -EINVAL;
    }

    if (!strcmp(it.node->name, "vdev0buffer"))
    {
      mem = rproc_of_resm_mem_entry_init(dev, index, rmem->size, rmem->base, it.node->name);
    }
    else
    {
      mem = rproc_mem_entry_init(dev, NULL, (dma_addr_t)rmem->base, rmem->size, rmem->base,
                                 rk3506_rproc_mem_alloc, rk3506_rproc_mem_release,
                                 it.node->name);
    }

    if (!mem)
    {
      of_node_put(it.node);
      return -ENOMEM;
    }
    rproc_add_carveout(rproc, mem);
    index++;
  }
  return 0;
}

// In TCM mode AXI writes are dropped; the M0 firmware must clear SGRF_PMU mcu_tcm_sel at boot
static int rk3506_rproc_prepare(struct rproc * rproc)
{
  rk3506_mcu_t *  mcu = rproc->priv;

  rk3506_rproc_mcu_run(mcu, false);

  writel(0x3506A55A, mcu->tcm_virt);
  writel(0xC0DE5AA5, mcu->tcm_virt + 4);
  dsb(sy);
  if ((readl(mcu->tcm_virt) != 0x3506A55A) || (readl(mcu->tcm_virt + 4) != 0xC0DE5AA5))
  {
    dev_err(&rproc->dev, "M0 SRAM is still in TCM mode, cannot load firmware (reboot required)");
    return -EACCES;
  }
  return rk3506_rproc_add_carveouts(rproc);
}

// The M0 firmware leaves TCM mode in its early init. Until then Linux writes to the
// loaded resource table (vring addresses, virtio status) would be dropped.
static int rk3506_rproc_wait_sram_writable(struct rproc * rproc)
{
  struct resource_table * table = rproc->table_ptr;
  int                     i;

  if (!table)
    return 0;

  for (i = 0; i < 100; i++)
  {
    writel(0x3506A55A, &table->reserved[0]);
    if (readl(&table->reserved[0]) == 0x3506A55A)
    {
      writel(0, &table->reserved[0]);
      return 0;
    }
    usleep_range(1000, 2000);
  }
  dev_err(&rproc->dev, "M0 did not leave TCM mode, resource table is not writable");
  return -ETIMEDOUT;
}

static irqreturn_t rk3506_rproc_mbox_irq(int irq, void * data)
{
  rk3506_mcu_t * mcu = data;

  if (!(readl(mcu->mbox_base + RK3506_MBOX_B2A_STATUS) & 1))
    return IRQ_NONE;

  writel(1, mcu->mbox_base + RK3506_MBOX_B2A_STATUS);
  return IRQ_WAKE_THREAD;
}

// rpmsg callbacks may sleep, so the vrings are processed in the IRQ thread
static irqreturn_t rk3506_rproc_mbox_thread(int irq, void * data)
{
  rk3506_mcu_t *  mcu = data;
  int             id;

  for (id = 0; id <= mcu->rproc->max_notifyid; id++)
    rproc_vq_interrupt(mcu->rproc, id);
  return IRQ_HANDLED;
}

static void rk3506_rproc_mbox_enable(rk3506_mcu_t * mcu, bool aenable)
{
  if (!mcu->mbox_base)
    return;

  if (aenable)
  {
    writel(1, mcu->mbox_base + RK3506_MBOX_B2A_STATUS);
    writel(RK3506_MBOX_INTEN_WMSK | 1, mcu->mbox_base + RK3506_MBOX_B2A_INTEN);
  }
  else
  {
    writel(RK3506_MBOX_INTEN_WMSK, mcu->mbox_base + RK3506_MBOX_B2A_INTEN);
    writel(1, mcu->mbox_base + RK3506_MBOX_B2A_STATUS);
    synchronize_irq(mcu->mbox_irq);
  }
}

static void rk3506_rproc_kick(struct rproc * rproc, int vqid)
{
  rk3506_mcu_t *  mcu = rproc->priv;

  if (!mcu->mbox_base)
    return;

  // The M0 scans every virtqueue on each notification, so a pending one covers this kick
  if (readl(mcu->mbox_base + RK3506_MBOX_A2B_STATUS) & 1)
    return;

  writel(vqid, mcu->mbox_base + RK3506_MBOX_A2B_CMD);
  writel(0, mcu->mbox_base + RK3506_MBOX_A2B_DATA);
}

static int rk3506_rproc_parse_fw(struct rproc * rproc, const struct firmware * fw)
{
  if (rproc_elf_load_rsc_table(rproc, fw))
    dev_info(&rproc->dev, "No resource table in the firmware, RPMsg disabled");
  return 0;
}

static int rk3506_rproc_start(struct rproc * rproc)
{
  rk3506_mcu_t *        mcu = rproc->priv;
  struct arm_smccc_res  res;
  uint32_t              mcu_entry = RK3506_MCU_TCM_ADDR;  // M0 vector table

  dev_info(&rproc->dev, "Starting M0 MCU at 0x%08X...", mcu_entry);

  // Maps M0 address 0 to this SRAM in TCM mode, which hides it from Linux until the M0 fw undoes it
  arm_smccc_smc(SIP_MCU_CFG, ROCKCHIP_SIP_CONFIG_BUSMCU_0_ID,
                ROCKCHIP_SIP_CONFIG_MCU_CODE_START_ADDR,
                mcu_entry,
                0, 0, 0, 0, &res);
  if (res.a0)
  {
    dev_err(&rproc->dev, "SMCCC CODE START call error: %i", (int)res.a0);
    return -1;
  }

  reset_control_deassert(mcu->rst_m0_jtag);
  reset_control_deassert(mcu->rst_h_m0);
  reset_control_deassert(mcu->rst_hresetn_m0_ac);

  rk3506_rproc_mbox_enable(mcu, true);
  rk3506_rproc_mcu_run(mcu, true);
  return rk3506_rproc_wait_sram_writable(rproc);
}

static int rk3506_rproc_stop(struct rproc * rproc)
{
  rk3506_mcu_t *        mcu = rproc->priv;

  dev_info(&rproc->dev, "Stopping M0 MCU");

  rk3506_rproc_mcu_run(mcu, false);
  rk3506_rproc_mbox_enable(mcu, false);

  return 0;
}

static void * my_da_to_va(struct rproc * rproc, u64 da, size_t len, bool * is_iomem)
{
  rk3506_mcu_t *  mcu = rproc->priv;
  void __iomem * va;

  if (da + len <= RK3506_MCU_TCM_SIZE)  // M0 address 0 aliases the TCM
  {
    va = mcu->tcm_virt + da;
  }
  else if (mcu->shmem_virt && (da >= RK3506_MCU_SHMEM_ADDR) &&
           (da + len <= RK3506_MCU_SHMEM_ADDR + RK3506_MCU_SHMEM_SIZE))
  {
    va = mcu->shmem_virt + (da - RK3506_MCU_SHMEM_ADDR);
  }
  else
  {
    va = NULL;  // the remoteproc core then looks up the registered carveouts
  }

  return va;
}

static const struct rproc_ops rk3506_rproc_ops =
{
  .start = rk3506_rproc_start,
  .stop = rk3506_rproc_stop,
  .prepare = rk3506_rproc_prepare,
  .da_to_va = my_da_to_va,
  .kick = rk3506_rproc_kick,
	.load = rproc_elf_load_segments,
	.parse_fw = rk3506_rproc_parse_fw,
	.find_loaded_rsc_table = rproc_elf_find_loaded_rsc_table,
	.sanity_check = rproc_elf_sanity_check,
	.get_boot_addr = rproc_elf_get_boot_addr,
};

static const char * rk3506_rproc_get_firmware(struct platform_device * pdev)
{
	const char * fw_name;
	int ret;

	ret = of_property_read_string(pdev->dev.of_node, "firmware-name",
				      &fw_name);
	if (ret)
		return ERR_PTR(ret);

	return fw_name;
}

static void rk3506_rproc_clk_put(void * data)
{
  struct clk * clk = data;

  clk_disable_unprepare(clk);
  clk_put(clk);
}

// The mainline rockchip-mailbox driver does not support the RK3506 layout, so drive the registers here
static int rk3506_rproc_mbox_init(rk3506_mcu_t * mcu)
{
  struct device *         dev = &mcu->pdev->dev;
  struct of_phandle_args  args;
  void __iomem *          base;
  struct clk *            clk;
  int                     idx, irq, ret;

  idx = of_property_match_string(dev->of_node, "mbox-names", "rpmsg");
  if (idx < 0)
  {
    dev_warn(dev, "no \"rpmsg\" mailbox, RPMsg disabled");
    return 0;
  }

  ret = of_parse_phandle_with_args(dev->of_node, "mboxes", "#mbox-cells", idx, &args);
  if (ret)
    return dev_err_probe(dev, ret, "invalid \"rpmsg\" mailbox phandle");

  base = devm_of_iomap(dev, args.np, 0, NULL);
  if (IS_ERR(base))
  {
    ret = dev_err_probe(dev, PTR_ERR(base), "cannot map the mailbox (bound to another driver?)");
    goto out;
  }

  irq = of_irq_get(args.np, 0);
  if (irq <= 0)
  {
    ret = dev_err_probe(dev, irq ? irq : -EINVAL, "cannot get the mailbox IRQ");
    goto out;
  }

  clk = of_clk_get(args.np, 0);
  if (IS_ERR(clk))
  {
    ret = dev_err_probe(dev, PTR_ERR(clk), "cannot get the mailbox clock");
    goto out;
  }
  ret = clk_prepare_enable(clk);
  if (ret)
  {
    clk_put(clk);
    goto out;
  }
  ret = devm_add_action_or_reset(dev, rk3506_rproc_clk_put, clk);
  if (ret)
    goto out;

  writel(RK3506_MBOX_INTEN_WMSK, base + RK3506_MBOX_B2A_INTEN);
  mcu->mbox_base = base;
  mcu->mbox_irq = irq;
  ret = devm_request_threaded_irq(dev, irq, rk3506_rproc_mbox_irq, rk3506_rproc_mbox_thread,
                                  IRQF_ONESHOT, dev_name(dev), mcu);
  if (ret)
    dev_err_probe(dev, ret, "cannot request the mailbox IRQ");

out:
  of_node_put(args.np);
  return ret;
}

static int rk3506_rproc_probe(struct platform_device * pdev)
{
  struct device *     dev = &pdev->dev;
  struct rproc *      rproc;
  rk3506_mcu_t *      mcu;
  int                 ret;
  const char *        firmware;

  firmware = rk3506_rproc_get_firmware(pdev);
  if (IS_ERR(firmware))
    return dev_err_probe(dev, PTR_ERR(firmware), "error getting firmware-name from the device-tree");

  rproc = devm_rproc_alloc(dev, dev_name(dev), &rk3506_rproc_ops, firmware, sizeof(rk3506_mcu_t));
  if (!rproc)
    return -ENOMEM;

  mcu = rproc->priv;
  mcu->rproc = rproc;
  mcu->pdev = pdev;

  mcu->tcm_virt = devm_ioremap(dev, RK3506_MCU_TCM_ADDR, RK3506_MCU_TCM_SIZE);
  mcu->regs_PMU = devm_ioremap(dev, RK3506_PMU_BASE, 4096);
  mcu->regs_CRU = devm_ioremap(dev, RK3506_CRU_BASE, 4096);
  mcu->regs_GRF = devm_ioremap(dev, RK3506_GRF_BASE, 4096);
  if (!mcu->tcm_virt || !mcu->regs_PMU || !mcu->regs_CRU || !mcu->regs_GRF)
    return -ENOMEM;

  // fails if the DT does not reserve it (no-map)
  mcu->shmem_virt = devm_ioremap(dev, RK3506_MCU_SHMEM_ADDR, RK3506_MCU_SHMEM_SIZE);
  if (!mcu->shmem_virt)
    dev_warn(dev, "cannot map the shared memory at 0x%08X", RK3506_MCU_SHMEM_ADDR);

  rk3506_rproc_mcu_run(mcu, false);

  mcu->num_clks = devm_clk_bulk_get_all_enabled(dev, &mcu->clks);
  if (mcu->num_clks < 0)
    return dev_err_probe(dev, mcu->num_clks, "error getting or enabling the clocks");

  // CRU_GATE_CON5: ungate hclk_m0 + swclktck_m0
  writel(0x0c000000, mcu->regs_CRU + 0x814);

  // GRF_SOC_CON36: M0 SysTick calibration, value from Rockchip U-Boot
  writel(0xbcd3d80, mcu->regs_GRF + 0x090);

  mcu->rst_h_m0 = devm_reset_control_get(dev, "h_m0");
  if (IS_ERR(mcu->rst_h_m0))
    return dev_err_probe(dev, PTR_ERR(mcu->rst_h_m0), "error getting reset: h_m0");

  mcu->rst_m0_jtag = devm_reset_control_get(dev, "m0_jtag");
  if (IS_ERR(mcu->rst_m0_jtag))
    return dev_err_probe(dev, PTR_ERR(mcu->rst_m0_jtag), "error getting reset: m0_jtag");

  mcu->rst_hresetn_m0_ac = devm_reset_control_get(dev, "hresetn_m0_ac");
  if (IS_ERR(mcu->rst_hresetn_m0_ac))
    return dev_err_probe(dev, PTR_ERR(mcu->rst_hresetn_m0_ac), "error getting reset: hresetn_m0_ac");

  platform_set_drvdata(pdev, rproc);

  ret = rk3506_rproc_mbox_init(mcu);
  if (ret)
    return ret;

  return rproc_add(rproc);
}

static void rk3506_rproc_shutdown(struct platform_device * pdev)
{
  struct rproc *   rproc = platform_get_drvdata(pdev);

  rk3506_rproc_stop(rproc);
}

static void rk3506_rproc_remove(struct platform_device * pdev)
{
  struct rproc *   rproc = platform_get_drvdata(pdev);
  rk3506_mcu_t *   mcu = rproc->priv;

  // rproc_del() calls stop(), so it must run before the devm unmaps
  rproc_del(rproc);
  rk3506_rproc_mcu_run(mcu, false);
  rk3506_rproc_mbox_enable(mcu, false);
}

static const struct of_device_id rk3506_rproc_match[] =
{
  { .compatible = "rockchip,rk3506-mcu" },
  {}
};
MODULE_DEVICE_TABLE(of, rk3506_rproc_match);

static struct platform_driver rk3506_rproc_driver =
{
  .probe = rk3506_rproc_probe,
  .remove = rk3506_rproc_remove,
  .shutdown = rk3506_rproc_shutdown,
  .driver = {
    .name = "rk3506_mcu_rproc",
    .of_match_table = rk3506_rproc_match,
  },
};

module_platform_driver(rk3506_rproc_driver);

MODULE_AUTHOR("mbober");
MODULE_DESCRIPTION("RK3506 Cortex-M0 Remote Processor Driver");
MODULE_LICENSE("GPL v2");
