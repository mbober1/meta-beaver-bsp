SUMMARY = "TI K3 system firmware and device manager firmware"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE.ti;md5=b5aebf0668bdf95621259288c4a46d76"

SRCREV = "9ad14a1a97699561e45da56e1ec4e59476ca94b6"
BRANCH = "ti-linux-firmware"

SRC_URI = "git://git.ti.com/git/processor-firmware/ti-linux-firmware.git;protocol=https;branch=${BRANCH}"

PACKAGE_ARCH = "${MACHINE_ARCH}"

INHIBIT_DEFAULT_DEPS = "1"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP = "1"

INSANE_SKIP:${PN} += "arch"

SYSROOT_DIRS += "${nonarch_base_libdir}/firmware"

FILES:${PN} = "${nonarch_base_libdir}/firmware"

do_install() {
    install -d ${D}${nonarch_base_libdir}/firmware/ti-sysfw
    install -m 644 ${S}/ti-sysfw/ti-fs-firmware-j722s-* ${D}${nonarch_base_libdir}/firmware/ti-sysfw
    install -d ${D}${nonarch_base_libdir}/firmware/ti-dm/j722s
    install -m 644 ${S}/ti-dm/j722s/* ${D}${nonarch_base_libdir}/firmware/ti-dm/j722s
}
