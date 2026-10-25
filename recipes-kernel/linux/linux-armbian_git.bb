require linux-custom.inc

LINUX_VERSION = "6.1.172"
KERNEL_BRANCH = "rk-6.1-rkr7.2"
SRCREV_kernel = "d3d527fca68e0db93aa802692e82b9e3b10cef8d"

META_BRANCH = "yocto-6.1"
SRCREV_yocto-meta = "7545dc5bc77560f4b1b6efc47b56e953dc86572d"

SRC_URI:append = " \
	git://github.com/armbian/linux-rockchip.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
	file://lyra-remote.cfg \
	file://luckfox-lyra-plus.dts \
"

INSANE_SKIP:${PN}-src += "buildpaths"
DEPENDS += "lz4-native"

do_configure:append() {
    install -m 0644 ${WORKDIR}/luckfox-lyra-plus.dts ${S}/arch/arm/boot/dts/luckfox-lyra-plus.dts
    grep -q 'luckfox-lyra-plus.dtb' ${S}/arch/arm/boot/dts/Makefile || \
        echo 'dtb-$(CONFIG_ARCH_ROCKCHIP) += luckfox-lyra-plus.dtb' >> ${S}/arch/arm/boot/dts/Makefile
}