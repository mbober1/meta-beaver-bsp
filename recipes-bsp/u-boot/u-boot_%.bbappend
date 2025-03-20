FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
	file://env.cfg \
	file://bootcmd.cfg \
	file://decompression.cfg \
	file://quickstart.cfg \
"

BRANCH = "rk3588"
SRC_URI = "git://gitlab.collabora.com/hardware-enablement/rockchip-3588/u-boot.git;protocol=https;branch=${BRANCH}"
SRCREV = "${AUTOREV}"
