FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "Linux Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

LINUX_VERSION = "6.1.115"
KBRANCH = "rk-6.1-rkr5.1"
SRCREV_kernel = "b908c7339f51eddcfe8402cd15d1e1f8f4e67c29"
SRCREV_yocto-meta = "7545dc5bc77560f4b1b6efc47b56e953dc86572d"
PV = "${LINUX_VERSION}"

SRC_URI = " \
	git://github.com/armbian/linux-rockchip.git;name=kernel;protocol=https;branch=${KBRANCH} \
	git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=yocto-meta;branch=yocto-6.1;destsuffix=kernel-meta;protocol=https \
"

DEPENDS += "lzop-native"
FIT_KERNEL_COMP_ALG ?= "lzo"
FIT_KERNEL_COMP_ALG_EXTENSION ?= ".lzo"

COMPATIBLE_MACHINE:luckfox-lyra = "luckfox-lyra"

inherit kernel
inherit kernel-yocto
require recipes-kernel/linux/linux-yocto.inc

INSANE_SKIP:${PN}-src += "buildpaths"
