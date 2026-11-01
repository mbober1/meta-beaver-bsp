SUMMARY = "Firmware for RK3506 coprocessor"

require firmware.inc

SRC_URI = "file://zephyr.elf"

FIRMWARE_FILE ??= "${UNPACKDIR}/zephyr.elf"
FIRMWARE_OUTPUT_FILE ??= "rk3506-m0.elf"
