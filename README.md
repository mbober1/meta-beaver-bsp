# Machine meta layer for Yocto Project

## Supported hardware
- Radxa Rock 5B+
- Radxa Zero 3E
- Luckfox Lyra Plus
- STM32MP157C-DK2


## Dependencies

  Layer: OECORE
  URI: https://github.com/openembedded/openembedded-core.git

  Layer: BITBAKE
  URI: https://github.com/openembedded/bitbake.git

  Layer: ARM
  URI: https://git.yoctoproject.org/meta-arm

  Layer: ROCKCHIP
  URI: https://git.yoctoproject.org/meta-rockchip


## Flashing eMMC (Rockchip based devices)
1. Hold maskrom button
2. Connect USB-C cable
3. Check if device is connected
4. Initialize DDR
`/bin/rkdeveloptool db rk3588_spl_loader_v1.15.113.bin`
5. Flash image
`/bin/rkdeveloptool wl 0 image.wic`

You can use Docker image with rkdeveloptool:

`docker run --rm -it -v .:/home/ubuntu/workspace --privileged -u root radxa-lab /bin/rkdeveloptool wl 0 image.wic`

#### Flashing only SPL
`/bin/rkdeveloptool wl 64 idbloader.img`

#### Flashing only U-boot
`/bin/rkdeveloptool wl 16384 u-boot.itb`
