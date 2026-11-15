SUMMARY = "Rockchip MPP VEPU2 H.264/HEVC encoder kernel driver"
DESCRIPTION = "Out-of-tree backport of the vendor Rockchip MPP encoder driver \
(mpp_service + mpp_iommu + mpp_vepu2) for the RK3566 VEPU540. Mainline Linux \
only exposes the VEPU121 JPEG encoder through Hantro, so this module provides \
the /dev/mpp_service interface used by librockchip_mpp and gstreamer-rockchip."
HOMEPAGE = "https://github.com/rockchip-linux/kernel"
SECTION = "kernel/modules"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit module

COMPATIBLE_MACHINE = "radxa-zero-3e-custom"

SRCREV = "d5732eb88608640ed2700b81750c9153bb7a8b63"
SRC_URI = " \
    git://github.com/mbober1/rockchip_mpp.git;protocol=https;branch=master \
"

S = "${UNPACKDIR}/mpp"
UNPACKDIR = "${WORKDIR}/sources"

PV = "1.0"
