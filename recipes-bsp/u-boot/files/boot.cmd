part list mmc 1 -bootable bootpart
load mmc 1:${bootpart} $loadaddr /boot/fitImage
load mmc 1:${bootpart} $fdt_addr_r /boot/rk3566-radxa-zero-3e.dtb
setenv bootargs root=/dev/mmcblk0p${bootpart} rootwait rw rootfstype=ext4 earlycon console=tty1 console=ttyS2,1500000n8
bootm $loadaddr - $fdt_addr_r
