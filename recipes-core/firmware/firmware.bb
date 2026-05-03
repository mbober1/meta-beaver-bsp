SUMMARY = "Firmware for coprocesor"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FIRMWARE_FILE ??= "${TOPDIR}/tmp/deploy/images/stm32mp157c-dk2/firmware-zephyr.elf"
FIRMWARE_DIR ??= "${libdir}/firmware"

do_install() {
  install -d ${D}${FIRMWARE_DIR}
  install -Dm 0644 ${FIRMWARE_FILE} ${D}${FIRMWARE_DIR}/rproc-m4-fw
}

do_install[mcdepends] = "mc::coproc:firmware-zephyr:do_deploy"

FILES:${PN} = "${FIRMWARE_DIR}/rproc-m4-fw"
INSANE_SKIP:${PN} += "buildpaths"
