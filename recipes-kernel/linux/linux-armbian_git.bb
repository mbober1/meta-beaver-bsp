require linux-custom.inc

LINUX_VERSION = "6.1.115"
KERNEL_BRANCH = "rk-6.1-rkr5.1"
SRCREV_kernel = "e40822f070f56bd3e5b3a94285c61c26abd394cb"

META_BRANCH = "yocto-6.1"
SRCREV_yocto-meta = "7545dc5bc77560f4b1b6efc47b56e953dc86572d"

SRC_URI:append = " \
	git://github.com/armbian/linux-rockchip.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"

INSANE_SKIP:${PN}-src += "buildpaths"