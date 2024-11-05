# Preparing development environment

Luckfox has SDK, which contains all the necessary tools and software for building customized Linux images and applications targeting the Luckfox Pico board.
The SDK is available on [GitHub](https://github.com/LuckfoxTECH/luckfox-pico).
The toolchain (compiler, linker, etc.) is only available on Linux, so Ubuntu system, or WSL on Windows is required to build images and applications for the board.

We are going to create customized images, which will include some libraries that will be needed in the next chapters.

[Luckfox Wiki pages](https://wiki.luckfox.com/Luckfox-Pico/Luckfox-Pico-SDK/) have information how to use the SDK.

## Toolchain

The toolchain is included under `tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf` directory.
Add the `tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin` directory to the environment variable `PATH`, i.e.

```
export PATH=$PATH:/home/user/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin
```

## Building image

To build buildroot based image, simply use the `build.sh` script provided in the SDK.

1. Execute `./build.sh lunch` and select:
  1. `RV1103_Luckfox_Pico`, the option 0
  2. `SD_CARD`, the option 0
  3. `Buildroot(Support Rockchip official features)`, the option 0
2. Execute `./build.sh` to build the image

The generated `img` files can be found in `output/image` subdirectory in the SDK root.
To burn the image on the SD card, use the same steps as in the first guide of this project.

The new image should behave same as the prebuilt image downloaded from Luckfox.
