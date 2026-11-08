FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
	file://env.cfg \
	file://decompression.cfg \
	file://quickstart.cfg \
"

SRC_URI:append:rock-5b-plus = " file://bootcmd_emmc.cfg"
SRC_URI:append:radxa-zero-3e-custom = " file://bootcmd_sdcard.cfg"
SRC_URI:append:stm32mp15-disco = " file://bootcmd_emmc.cfg"
SRC_URI:append:rk3506 = " file://bootcmd_emmc.cfg"

BRANCH:rk3506 = "main"
SRCREV:rk3506 = "ede4c80c159acfd6038255c88922993fbde16d93"
SRC_URI:rk3506 = "git://source.denx.de/u-boot/u-boot.git;protocol=https;branch=${BRANCH}"
SRC_URI:append:rk3506 = "\
	file://rk3506/0001-HACK-rockchip-dts-Add-work-in-progress-device-tree-f.patch \
	file://rk3506/0002-rockchip-rk3506-Enable-use-of-SPL_OPTEE_IMAGE.patch \
	file://rk3506/0003-board-rockchip-Add-Luckfox-Lyra.patch \
	file://rk3506/0004-rockchip-spl-Allow-use-of-ROCKCHIP_SPL_RESERVE_IRAM-.patch \
	file://rk3506/0005-pinctrl-rockchip-Add-RK3506-RM_IO-support.patch \
	file://rk3506/0006-video-rockchip-Add-RK3506-VOP-lite-and-MIPI-DSI-support.patch \
	file://rk3506/0007-video-simple-panel-Decode-display-timings-via-ofnode.patch \
	file://rk3506/0008-video-simple-panel-Add-Waveshare-10.1-DSI-panel.patch \
	file://rk3506/0009-video-Add-Waveshare-DSI-I2C-backlight-driver.patch \
	file://rk3506/0010-arm64-dts-rockchip-rk3506-Add-display-controller-nodes.patch \
	file://rk3506/0011-arm64-dts-rockchip-luckfox-lyra-plus-Enable-Waveshare-panel.patch \
	file://rk3506/0012-rk3506-Add-splash-screen-environment.patch \
	file://rk3506/0013-video-rockchip-rk3506-Pin-the-scanout-buffer-address.patch \
	file://splash.cfg \
"

ROCKCHIP_TEE:closed-tpl:rk3506 = "${DEPLOY_DIR_IMAGE}/tee-rk3506.bin"
EXTRA_OEMAKE:append:closed-tpl:rk3506 = " TEE=${ROCKCHIP_TEE}"


BRANCH:stmp157-olinuxino-lime2 = "release-20250305"
SRC_URI:stmp157-olinuxino-lime2 = "git://github.com/OLIMEX/u-boot-olinuxino.git;protocol=https;branch=${BRANCH}"
SRCREV:stmp157-olinuxino-lime2 = "23d8f769a94ae230b7a68fa812cc3c4dc0cbf1c9"

