# Machine meta layer for Yocto Project

## Supported hardware
- [x] Radxa Rock 5B+
- [ ] Radxa Zero 3E


## Dependencies

  Layer: OECORE
  URI: https://github.com/openembedded/openembedded-core.git

  Layer: BITBAKE
  URI: https://github.com/openembedded/bitbake.git

  Layer: ARM
  URI: https://git.yoctoproject.org/meta-arm

  Layer: ROCKCHIP
  URI: https://git.yoctoproject.org/meta-rockchip


## Flashing eMMC
1. Hold maskrom button
2. Connect USB-C cable
3. Check if device is connected
4. Build and install rkdeveloptool or use Docker image with it

### Flash all at once

`/bin/rkdeveloptool wl 0 image.wic`

You can use Docker image with rkdeveloptool:

`docker run --rm -it -v .:/home/ubuntu/workspace --privileged -u root radxa-lab /bin/rkdeveloptool wl 0 lab-image-minimal.rootfs.wic`


### Flash in manually
#### Initialize DDR
/bin/rkdeveloptool db rk3588_spl_loader_v1.15.113.bin

#### Flash SPL
/bin/rkdeveloptool wl 64 idbloader.img

#### Flash U-boot
/bin/rkdeveloptool wl 16384 u-boot.itb
