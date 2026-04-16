require linux-custom.inc

LINUX_VERSION = "7.0"
KERNEL_BRANCH = "master"
SRCREV_kernel = "028ef9c96e96197026887c0f092424679298aae8"

META_BRANCH = "master"
SRCREV_yocto-meta = "513c13df4a72b50e4b1e93c011c3932da06b7605"

SRC_URI:append = " \
	git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"