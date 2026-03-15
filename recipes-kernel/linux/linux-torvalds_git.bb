require linux-custom.inc

LINUX_VERSION = "6.19.8"
KERNEL_BRANCH = "linux-6.19.y"
SRCREV_kernel = "86818b2e7d9c22225b15f2ae91d3f35c4a07dfd9"

META_BRANCH = "yocto-6.18"
SRCREV_yocto-meta = "bc293057a14f94dd50838a399bd8758a6766b877"

SRC_URI:append = " \
	git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"