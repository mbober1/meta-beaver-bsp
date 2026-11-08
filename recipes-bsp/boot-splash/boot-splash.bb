SUMMARY = "Boot splash bitmap shown by U-Boot on the DSI panel"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://logo.bmp"

do_install() {
    install -Dm 0644 ${UNPACKDIR}/logo.bmp ${D}/boot/logo.bmp
}

FILES:${PN} = "/boot/logo.bmp"
