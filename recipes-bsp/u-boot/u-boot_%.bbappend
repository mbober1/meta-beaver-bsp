FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
	file://env.cfg \
	file://decompression.cfg \
	file://quickstart.cfg \
	file://bootcmd_emmc.cfg \
"

SRC_URI:append:radxa-zero-3e-custom = " file://bootcmd_sdcard.cfg"

BRANCH = "rockchip"
SRC_URI = "git://gitlab.collabora.com/hardware-enablement/rockchip-3588/u-boot.git;protocol=https;branch=${BRANCH}"
SRCREV = "80ed675263b0a85420a3bd9ac45410fa9803bbf4"
DEPENDS += "gnutls-native"

BRANCH:luckfox-lyra-plus = "rk3506"
SRC_URI:luckfox-lyra-plus = "git://github.com/Kwiboo/u-boot-rockchip.git;protocol=https;branch=${BRANCH}"
SRCREV:luckfox-lyra-plus = "c6782a37feee4a8aee08d321c01b4ca0b0562f0b"
ROCKCHIP_TEE:closed-tpl:rk3506 = "${DEPLOY_DIR_IMAGE}/tee-rk3506.bin"
EXTRA_OEMAKE:append:closed-tpl:rk3506 = " TEE=${ROCKCHIP_TEE}"
