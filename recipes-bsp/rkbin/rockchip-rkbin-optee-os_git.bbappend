do_deploy:rk3506() {
	# Prebuilt OPTEE-OS
	install -m 644 ${S}/bin/rk35/rk3506_tee_v*.bin ${DEPLOYDIR}/tee-rk3506.bin
}

COMPATIBLE_MACHINE:rk3506 = "rk3506"