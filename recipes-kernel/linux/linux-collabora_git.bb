FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "Linux Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

LINUX_VERSION = "6.19"
KBRANCH = "rockchip-release"
SRCREV_kernel = "d0829c6423d74e7e20a8f547709671fdf355335a"
SRCREV_yocto-meta = "bc293057a14f94dd50838a399bd8758a6766b877"
PV = "${LINUX_VERSION}+git"
KMETA_AUDIT = ""

SRC_URI = " \
	git://gitlab.collabora.com/hardware-enablement/rockchip-3588/linux.git;name=kernel;protocol=https;branch=${KBRANCH} \
	git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=yocto-meta;branch=yocto-6.18;destsuffix=kernel-meta;protocol=https \
"

DEPENDS += "lzop-native"
FIT_KERNEL_COMP_ALG ?= "lzo"
FIT_KERNEL_COMP_ALG_EXTENSION ?= ".lzo"

inherit kernel
inherit kernel-yocto
require recipes-kernel/linux/linux-yocto.inc
