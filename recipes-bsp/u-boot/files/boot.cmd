part list mmc 0 -bootable bootpart
load mmc 0:${bootpart} $loadaddr /boot/fitImage
load mmc 0:${bootpart} $fdt_addr_r /boot/rk3588-rock-5b-plus.dtb
load mmc 0:${bootpart} $fdtoverlay_addr_r /boot/overlays/rk3588-spi1-m1-cs0-spidev.dtbo
fdt addr $fdt_addr_r
fdt resize 8192
fdt apply $fdtoverlay_addr_r
setenv bootargs root=/dev/mmcblk0p${bootpart} rootwait rw rootfstype=ext4 earlycon console=tty1 console=ttyS2,1500000n8
bootm $loadaddr - $fdt_addr_r



FDTOVERLAYS /boot/overlays/rk3588-spi1-m1-cs0-spidev.dtbo

part list mmc 0 -bootable bootpart
setenv bootargs root=/dev/mmcblk0p${bootpart} rootwait rw rootfstype=ext4 earlycon console=tty1 console=ttyS2,1500000n8
load mmc 0:${bootpart} $loadaddr /boot/fitImage
bootm ${loadaddr}#conf-rk3588-rock-5b-plus.dtb#conf-rk3588-spi1-m1-cs0-spidev.dtbo
