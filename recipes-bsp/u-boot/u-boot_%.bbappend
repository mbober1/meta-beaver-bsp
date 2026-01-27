FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
	file://env.cfg \
	file://decompression.cfg \
	file://quickstart.cfg \
	file://rock-5b-plus-nvme.patch \
"

SRC_URI:append:rock-5b-plus = "${@bb.utils.contains('MACHINE_FEATURES', 'nvme_boot', ' file://bootcmd_nvme.cfg', ' file://bootcmd_emmc.cfg', d)}"
SRC_URI:append:radxa-zero-3e-custom = " file://bootcmd_sdcard.cfg"

BRANCH = "rockchip"
SRC_URI = "git://gitlab.collabora.com/hardware-enablement/rockchip-3588/u-boot.git;protocol=https;branch=${BRANCH}"
SRCREV = "a81c91446a19bb106467703f6336fbe366271e26"
DEPENDS += "gnutls-native"

BRANCH:luckfox-lyra = "rk3506"
SRC_URI:luckfox-lyra = "git://source.denx.de/u-boot/contributors/kwiboo/u-boot.git;protocol=https;branch=${BRANCH}"
SRCREV:luckfox-lyra = "c6782a37feee4a8aee08d321c01b4ca0b0562f0b"
