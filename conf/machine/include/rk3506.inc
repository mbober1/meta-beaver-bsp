MACHINEOVERRIDES =. "rk3506:"
SOC_FAMILY = "rk3506"
DEFAULTTUNE = "cortexa7thf-neon-vfpv4"

ROCKCHIP_CLOSED_TPL ?= "1"
RK_KERNEL_FITIMAGE = "False"

require conf/machine/include/soc-family.inc
require conf/machine/include/rockchip-defaults.inc
require conf/machine/include/arm/armv7a/tune-cortexa7.inc
require conf/machine/include/rockchip-wic.inc

KBUILD_DEFCONFIG ?= "rk3506_defconfig"

UBOOT_SUFFIX ?= "itb"
UBOOT_ENTRYPOINT ?= "0x02800000"