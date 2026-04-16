require linux-custom.inc

LINUX_VERSION = "6.1.118"
KERNEL_BRANCH = "rk-6.1-rkr6.1"
SRCREV_kernel = "82c6b3ef1c935064d4aa87f698412fdc37a4435f"

META_BRANCH = "yocto-6.1"
SRCREV_yocto-meta = "7545dc5bc77560f4b1b6efc47b56e953dc86572d"

SRC_URI:append = " \
	git://github.com/armbian/linux-rockchip.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"

INSANE_SKIP:${PN}-src += "buildpaths"
DEPENDS += "lz4-native"