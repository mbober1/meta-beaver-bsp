FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "Linux Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_VERSION_SANITY_SKIP="1"
LINUX_VERSION = "6.13+"
BRANCH = "linux-6.13.y"
SRCREV = "${AUTOREV}"
SRCPV = "${@bb.fetch2.get_srcrev(d)}"

SRC_URI = " \
	git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;protocol=https;branch=${BRANCH} \
	file://beaver-kmeta;type=kmeta;name=beaver-kmeta;destsuffix=beaver-kmeta \
	git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=master;destsuffix=kernel-meta;protocol=https \
	file://arm64-dts-rockchip-Add-Rock-5B-plus.patch \
	${@bb.utils.contains('DISTRO_FEATURES', 'can', 'file://0001-Enable-MCP2515-on-SPI3-M1-CS0.patch', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'spi', 'file://0001-radxa-zero-3E-Enable-SPI1-spidev.patch', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'gps', 'file://0001-radxa-zero-3E-Enable-UART3.patch', '', d)} \
"

DEPENDS += "lzop-native"
FIT_KERNEL_COMP_ALG ?= "lzo"
FIT_KERNEL_COMP_ALG_EXTENSION ?= ".lzo"

COMPATIBLE_MACHINE:rock-5b-plus = "rock-5b-plus"
COMPATIBLE_MACHINE:luckfox-lyra = "luckfox-lyra"

#KERNEL_DTC_FLAGS += "-@ -H epapr"

inherit kernel
inherit kernel-yocto
require recipes-kernel/linux/linux-yocto.inc