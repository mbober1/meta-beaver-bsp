MACHINEOVERRIDES =. "am67a:j722s:k3:"

SOC_FAMILY = "k3:j722s"

require conf/machine/include/soc-family.inc
include conf/machine/include/arm/armv8a/tune-cortexa53.inc

DEFAULTTUNE = "cortexa53"

PREFERRED_PROVIDER_virtual/kernel = "linux-torvalds"
PREFERRED_PROVIDER_virtual/bootloader = "u-boot"
PREFERRED_PROVIDER_u-boot = "u-boot"

UBOOT_SUFFIX = "img"
UBOOT_ENTRYPOINT = "0x82000000"
UBOOT_LOADADDRESS = "0x82000000"

KERNEL_IMAGETYPE = "Image"
KBUILD_DEFCONFIG ?= "defconfig"
KCONFIG_MODE ?= "alldefconfig"

SERIAL_CONSOLES = "115200;ttyS2"

TFA_PLATFORM = "k3"
TFA_BOARD = "lite"
TFA_SPD = "opteed"
TFA_INSTALL_TARGET = "bl31"

OPTEEMACHINE = "k3-am62x"

UBOOT_EXTLINUX ?= "1"
UBOOT_EXTLINUX_ROOT ?= "root=/dev/mmcblk1p${bootpart}"
UBOOT_EXTLINUX_FDTDIR ?= "/boot"
UBOOT_EXTLINUX_FDT ?= "/boot/k3-am67a-beagley-ai.dtb"
UBOOT_EXTLINUX_CONSOLE ?= "earlycon console=ttyS2,115200n8"
UBOOT_EXTLINUX_KERNEL_ARGS ?= "rootwait rw rootfstype=ext4"
UBOOT_EXTLINUX_KERNEL_IMAGE ?= "/boot/${KERNEL_IMAGETYPE}"
UBOOT_EXTLINUX_LABELS ?= "default"
UBOOT_EXTLINUX_MENU_DESCRIPTION:default ?= "${MACHINE}"

MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "u-boot-extlinux"

BBMULTICONFIG += "k3r5"
do_image[mcdepends] += "mc::k3r5:virtual/bootloader:do_deploy"
do_image_wic[mcdepends] += "mc::k3r5:virtual/bootloader:do_deploy"

WKS_FILE_DEPENDS ?= " \
    virtual/bootloader \
    e2fsprogs-native \
"
