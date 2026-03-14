FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "Linux Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

LINUX_VERSION = "6.18.1"
BRANCH = "linux-6.18.y"
SRCREV_kernel = "25442251cbda7590d87d8203a8dc1ddf2c93de61"
SRCREV_yocto-meta = "f224983ee724c8eb616ae63eb3dc2aac9752c07b"
PV = "${LINUX_VERSION}"
KMETA_AUDIT = ""

SRC_URI = " \
	git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;name=kernel;protocol=https;branch=${BRANCH} \
	git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=yocto-meta;branch=yocto-6.18;destsuffix=kernel-meta;protocol=https \
"

DEPENDS += "lzop-native"
FIT_KERNEL_COMP_ALG ?= "lzo"
FIT_KERNEL_COMP_ALG_EXTENSION ?= ".lzo"
LINUX_KERNEL_TYPE = "preempt-rt"

COMPATIBLE_MACHINE:rock-5b-plus = "rock-5b-plus"


inherit kernel
inherit kernel-yocto
require recipes-kernel/linux/linux-yocto.inc
