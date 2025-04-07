LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit devicetree

SRCREV = "b31398007ecc0d7e4c15d429a8dff161ceee45b6"
SRC_URI = "	\
	git://github.com/radxa-pkg/radxa-overlays.git;protocol=https;branch=main \
"

COMPATIBLE_MACHINE:rock-5b-plus = "rock-5b-plus"

PROVIDES = "virtual/dtb"

S = "${WORKDIR}/git"

DT_FILES_PATH = "${WORKDIR}/git/arch/arm64/boot/dts/rockchip/overlays"
DT_FILES = " \
  rk3588-dwc3_1-host.dts \
  rk3588-dwc3_1-peripheral.dts \
  rk3588-fiq-debugger-uart2m2.dts \
  rk3588-fiq-debugger-uart3m1.dts \
  rk3588-fiq-debugger-uart7m1.dts \
  rk3588-i2c0-m1.dts \
  rk3588-i2c3-m1.dts \
  rk3588-i2c8-m4.dts \
  rk3588-i2s2-2ch-m1-hifiberry-dac.dts \
  rk3588-i2s2-2ch-m1.dts \
  rk3588-pwm0-m2.dts \
  rk3588-pwm12-m0.dts \
  rk3588-pwm13-m0.dts \
  rk3588-pwm14-m0.dts \
  rk3588-pwm14-m1.dts \
  rk3588-pwm15-m0.dts \
  rk3588-pwm15-m1.dts \
  rk3588-pwm2-m1.dts \
  rk3588-pwm3-m1.dts \
  rk3588-pwm5-m2.dts \
  rk3588-pwm6-m2.dts \
  rk3588-pwm7-m3.dts \
  rk3588-pwm8-m0.dts \
  rk3588-spi0-m2-cs0-mcp2515-8mhz.dts \
  rk3588-spi0-m2-cs0-spidev.dts \
  rk3588-spi0-m2-cs1-spidev.dts \
  rk3588-spi1-m1-cs0-spidev.dts \
  rk3588-spi1-m1-cs1-mcp2515-8mhz.dts \
  rk3588-spi1-m1-cs1-spidev.dts \
  rk3588-uart2-m0.dts \
  rk3588-uart2-m2.dts \
  rk3588-uart3-m1.dts \
  rk3588-uart4-m2.dts \
  rk3588-uart7-m1-full.dts \
  rk3588-uart7-m1.dts \
  rk3588-uart7-m2.dts \
  rk3588-w1-gpio3-b3.dts \
  rock-5b-plus-cam0-okdo-5mp-camera.dts \
  rock-5b-plus-cam0-radxa-camera-8m-219.dts \
  rock-5b-plus-cam0-rpi-camera-v1p3.dts \
  rock-5b-plus-cam0-rpi-camera-v2.dts \
  rock-5b-plus-cam1-okdo-5mp-camera.dts \
  rock-5b-plus-cam1-radxa-camera-8m-219.dts \
  rock-5b-plus-cam1-rpi-camera-v1p3.dts \
  rock-5b-plus-cam1-rpi-camera-v2.dts \
  rock-5b-plus-radxa-display-10hd.dts \
  rock-5b-plus-radxa-display-8hd.dts \
  rock-5b-radxa-25w-poe.dts \
"

do_install() {
    cd ${B}
    install -d ${D}/boot/overlays
    for DTB_FILE in `ls *.dtbo`; do
        install -Dm 0644 ${B}/${DTB_FILE} ${D}/boot/overlays/${DTB_FILE}
    done
}

FILES:${PN} += "/boot/overlays/*.dtbo"
