require linux-custom.inc

LINUX_VERSION = "6.19"
KERNEL_BRANCH = "rockchip-release"
SRCREV_kernel = "d0829c6423d74e7e20a8f547709671fdf355335a"

META_BRANCH = "yocto-6.18"
SRCREV_yocto-meta = "bc293057a14f94dd50838a399bd8758a6766b877"

SRC_URI:append = " \
	git://gitlab.collabora.com/hardware-enablement/rockchip-3588/linux.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"