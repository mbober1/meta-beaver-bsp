FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "Linux Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_VERSION_SANITY_SKIP="1"
LINUX_VERSION = "6.13+"
BRANCH = "rockchip-release"
SRCREV_kernel = "0fa4464135a05564d3f690dc6714979f7ed2fac5"
SRCREV_yocto-meta = "5a1b16216628f2edb6eacc7d25b9f0dc3e50fbac"

SRC_URI = " \
	git://gitlab.collabora.com/hardware-enablement/rockchip-3588/linux.git;name=kernel;protocol=https;branch=${BRANCH} \
	git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=yocto-meta;branch=master;destsuffix=kernel-meta;protocol=https \
"

DEPENDS += "lzop-native"
FIT_KERNEL_COMP_ALG ?= "lzo"
FIT_KERNEL_COMP_ALG_EXTENSION ?= ".lzo"
LINUX_KERNEL_TYPE = "preempt-rt"

COMPATIBLE_MACHINE:rock-5b-plus = "rock-5b-plus"


inherit kernel
inherit kernel-yocto
require recipes-kernel/linux/linux-yocto.inc
