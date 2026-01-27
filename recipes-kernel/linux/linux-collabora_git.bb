FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "Linux Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

LINUX_VERSION = "6.17"
KBRANCH = "rockchip-release"
SRCREV_kernel = "0fa4464135a05564d3f690dc6714979f7ed2fac5"
SRCREV_yocto-meta = "d84c38d95b4e042c0df0f6cdd14ee36909f86ea3"
PV = "${LINUX_VERSION}+git"

SRC_URI = " \
	git://gitlab.collabora.com/hardware-enablement/rockchip-3588/linux.git;name=kernel;protocol=https;branch=${KBRANCH} \
	git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=yocto-meta;branch=yocto-6.17;destsuffix=kernel-meta;protocol=https \
"

DEPENDS += "lzop-native"
FIT_KERNEL_COMP_ALG ?= "lzo"
FIT_KERNEL_COMP_ALG_EXTENSION ?= ".lzo"
LINUX_KERNEL_TYPE = "preempt-rt"

COMPATIBLE_MACHINE:rock-5b-plus = "rock-5b-plus"


inherit kernel
inherit kernel-yocto
require recipes-kernel/linux/linux-yocto.inc

#SRC_URI += "file://add-spi0m2_cs0-spidev.patch"