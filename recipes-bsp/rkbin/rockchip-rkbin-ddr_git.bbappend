DDRBIN_VERS:rk3506 ?= "v1.02"
DDRBIN_FILE:rk3506 ?= "rk3506_ddr_750MHz_${DDRBIN_VERS}.bin"

do_deploy:rk3506() {
	# Prebuilt U-Boot TPL (DDR init)
	install -m 644 ${S}/bin/rk35/${DDRBIN_FILE} ${DEPLOYDIR}/ddr-rk3506.bin
	install -m 644 ${S}/bin/rk35/rk3506_tee_v1.23.bin ${DEPLOYDIR}/tee-rk3506.bin
}

COMPATIBLE_MACHINE:rk3506 = "rk3506"