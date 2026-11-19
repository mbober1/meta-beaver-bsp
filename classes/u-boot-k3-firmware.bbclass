DEPENDS:append:k3 = " \
	ti-linux-firmware \
	python3-pyyaml-native \
	python3-jsonschema-native \
	python3-yamllint-native \
"
DEPENDS:append:beagley-ai = " trusted-firmware-a optee-os"

K3_FIRMWARE_INPUTS = "${WORKDIR}/binman-indirs"
K3_TRUSTED_FIRMWARE = ""
K3_TRUSTED_FIRMWARE:beagley-ai = "1"

do_configure:prepend:k3() {
	install -d ${K3_FIRMWARE_INPUTS}
	cp -r ${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware/ti-sysfw ${K3_FIRMWARE_INPUTS}/
	cp -r ${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware/ti-dm ${K3_FIRMWARE_INPUTS}/
	if [ -n "${K3_TRUSTED_FIRMWARE}" ]; then
		install -m 644 ${STAGING_DIR_HOST}/firmware/trusted-firmware-a/bl31.bin ${K3_FIRMWARE_INPUTS}/
		install -m 644 ${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware/tee-raw.bin ${K3_FIRMWARE_INPUTS}/
	fi
}

EXTRA_OEMAKE:append:k3 = " \
	BINMAN_INDIRS=${K3_FIRMWARE_INPUTS} \
	BL31=${K3_FIRMWARE_INPUTS}/bl31.bin \
	TEE=${K3_FIRMWARE_INPUTS}/tee-raw.bin \
"

do_deploy:append:beagley-ai() {
	install -m 644 ${B}/tispl.bin ${DEPLOYDIR}/tispl.bin-${MACHINE}-${UBOOT_VERSION}
	ln -sf tispl.bin-${MACHINE}-${UBOOT_VERSION} ${DEPLOYDIR}/tispl.bin
}
