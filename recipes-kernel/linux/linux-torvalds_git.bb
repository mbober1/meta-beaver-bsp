require linux-custom.inc

LINUX_VERSION = "7.3"
KERNEL_BRANCH = "master"
SRCREV_kernel = "091b06388b117fefdcb63336ac09956a4914b961"

META_BRANCH = "master"
SRCREV_yocto-meta = "513c13df4a72b50e4b1e93c011c3932da06b7605"

SRC_URI:append = " \
	git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git;name=kernel;protocol=https;branch=${KERNEL_BRANCH} \
"

SRC_URI:append:luckfox-lyra-plus = " \
	file://rk3506/add-luckfox-lyra-plus-support.patch \
	file://rk3506/stmmac-rx-init-gfp.patch \
	file://rk3506/lyra-remote.cfg \
	file://rk3506/rk3506.cfg \
"

INSANE_SKIP:${PN}-src += "buildpaths"
