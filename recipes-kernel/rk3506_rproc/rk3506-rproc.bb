DESCRIPTION = "RK3506 Cortex-M0 remoteproc driver"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = " \
  file://Makefile \
  file://rk3506_rproc.c \
"

PV = "1.0"

UNPACKDIR = "${WORKDIR}/sources"
S = "${UNPACKDIR}"
