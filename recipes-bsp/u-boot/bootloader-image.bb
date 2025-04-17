DESCRIPTION = "Image which build only bootloader"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

WKS_FILE = "only_bootloader.wks.in"

inherit image

IMAGE_FEATURES = ""
EXTRA_IMAGE_FEATURES = ""
RRECOMMENDS = ""

deltask do_image_qa
do_rootfs[depends] = "${LDCONFIGDEPEND}"
