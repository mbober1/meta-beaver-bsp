FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
	file://env.cfg \
	file://decompression.cfg \
	file://quickstart.cfg \
	file://rock-5b-plus-nvme.patch \
"

SRC_URI:append:rock-5b-plus = "${@bb.utils.contains('MACHINE_FEATURES', 'nvme_boot', ' file://bootcmd_nvme.cfg', ' file://bootcmd_emmc.cfg', d)}"
SRC_URI:append:radxa-zero-3e-custom = " file://bootcmd_sdcard.cfg"

BRANCH = "rk3588"
SRC_URI = "git://gitlab.collabora.com/hardware-enablement/rockchip-3588/u-boot.git;protocol=https;branch=${BRANCH}"
SRCREV = "cbc9673f77851953e294845549d418ffe2190ef9"
