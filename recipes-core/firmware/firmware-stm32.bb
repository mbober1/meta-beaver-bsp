SUMMARY = "Firmware for STM32 coprocessor"

require firmware.inc

FIRMWARE_FILE ??= "${TOPDIR}/tmp/deploy/images/stm32mp157c-dk2/firmware-zephyr.elf"
FIRMWARE_OUTPUT_FILE ??= "rproc-m4-fw"
