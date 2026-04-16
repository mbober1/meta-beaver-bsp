FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
	file://env.cfg \
	file://decompression.cfg \
	file://quickstart.cfg \
"

SRC_URI:append:rock-5b-plus = " file://bootcmd_emmc.cfg"
SRC_URI:append:radxa-zero-3e-custom = " file://bootcmd_sdcard.cfg"

BRANCH:rk3506 = "rk3506"
SRC_URI:rk3506 = "git://github.com/Kwiboo/u-boot-rockchip.git;protocol=https;branch=${BRANCH}"
SRCREV:rk3506 = "c6782a37feee4a8aee08d321c01b4ca0b0562f0b"
ROCKCHIP_TEE:closed-tpl:rk3506 = "${DEPLOY_DIR_IMAGE}/tee-rk3506.bin"
EXTRA_OEMAKE:append:closed-tpl:rk3506 = " TEE=${ROCKCHIP_TEE}"
