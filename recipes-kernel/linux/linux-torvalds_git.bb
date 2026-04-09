require linux-custom.inc

LINUX_VERSION = "7.0-rc7"
KERNEL_BRANCH = "master"
SRCREV_kernel = "0e5d0a0b5ca6ea4e391d6786266405c5871e0151"

META_BRANCH = "yocto-6.18"
SRCREV_yocto-meta = "bc293057a14f94dd50838a399bd8758a6766b877"

SRC_URI:append = " \
	git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"