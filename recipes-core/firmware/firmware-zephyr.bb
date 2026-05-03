SUMMARY = "Firmware for coprocesor built with ZephyrOS"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

include recipes-kernel/zephyr-kernel/zephyr-image.inc

SRCREV_firmware = "950d0edbefde81df25120d6bf4fc5efd7ec39a7f"

SRC_URI += " \
  git://github.com/mbober1/STM32MP1_ZephyrOS.git;protocol=https;branch=master;name=firmware;destsuffix=git/firmware \
"

ZEPHYR_SRC_DIR = "${S}/firmware/app"
