MACHINEOVERRIDES =. "rk3506:"
SOC_FAMILY = "rk3506"
DEFAULTTUNE = "cortexa7thf-neon-vfpv4"

ROCKCHIP_CLOSED_TPL ?= "1"

require conf/machine/include/soc-family.inc
require conf/machine/include/rockchip-defaults.inc
require conf/machine/include/arm/armv7a/tune-cortexa7.inc
require conf/machine/include/rockchip-wic.inc

PREFERRED_PROVIDER_optee-os = "rockchip-rkbin-optee-os"

KBUILD_DEFCONFIG ?= "rk3506_defconfig"

UBOOT_SUFFIX ?= "itb"
UBOOT_ENTRYPOINT ?= "0x02800000"