MACHINEOVERRIDES =. "k3r5:"

SOC_FAMILY = "k3:k3r5"

require conf/machine/include/soc-family.inc
include conf/machine/include/arm/armv7a/tune-cortexa8.inc

DEFAULTTUNE = "armv7athf"

PREFERRED_PROVIDER_virtual/kernel = "linux-dummy"

UBOOT_SUFFIX = "bin"
UBOOT_BINARY = "tiboot3.bin"
UBOOT_ENTRYPOINT = "0x82000000"
UBOOT_LOADADDRESS = "0x82000000"

UBOOT_INITIAL_ENV = ""
