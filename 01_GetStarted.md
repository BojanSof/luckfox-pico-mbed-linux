# Getting started with Luckfox Pico

There are multiple boards in the Luckfox Pico line, but we are going to use the regular Luckfox Pico board.

The Luckfox Pico is cheap board based on Rockchip RV1103 SoC.
In summary, the SoC includes:
- Cortex A7 1.2 GHz processor,
- 64 MB of DDR2 RAM,
- 24 GPIO pins,
- USB 2.0 Host and Device
- MicroSD card slot, for storage

More details can be found on [Luckfox Wiki pages](https://wiki.luckfox.com/Luckfox-Pico/Luckfox-Pico-quick-start).

The Luckfox wiki contains a lot of info for creating bootable SD card and getting the board running.
This page will give overview of the required steps to get the board up and running.

If you are on Windows, ensure to install the required drivers by following the instructions on [this link](https://wiki.luckfox.com/Luckfox-Pico/Luckfox-Pico-prepare).
Also, on Windows, ensure to have [WSL - Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/), as the SDK for building luckfox pico application is currently easily available for Ubuntu.

## Required tools and software for the projects

- The [`lucfox-pico`](https://github.com/LuckfoxTECH/luckfox-pico) SDK
- Rockchip ARM Toolchain, available as part of the SDK repository, under `tools//toolchain` directory.
- SocToolKit, which is a tool that allows to burn images for the luckfox pico in multiple ways (SD card, SPI NOR flash for boards that support it, etc.).
This tool allows to easily create the bootable SD card for the board.
It is available as part of the SDK repository, under `tools/<OS>/SocToolKit` directory.
- SSH client.
On Windows, one can install OpenSSH client on Windows by following [this guide](https://learn.microsoft.com/en-us/windows/terminal/tutorials/ssh), or use [Putty](https://www.putty.org/).
On Linux, the OpenSSH client comes pre-installed on many distributions.

## Download image

Luckfox provides prebuilt images that can be burned on the SD card.
Luckfox provides the images on [Google drive link](https://drive.google.com/drive/folders/1sFUWjYpDDisf92q9EwP1Ia7lHgp9PaFS?usp=drive_link), which can be found in [their guide](https://wiki.luckfox.com/Luckfox-Pico/Luckfox-Pico-prepare).
We are going to use the images found under [`buildroot/Luckfox_Pico_MicroSD`](https://drive.google.com/drive/folders/19yaqAy5IpGvoiDCfNno8yw2BaHwwiSbt?usp=drive_link).

Note that there is also Ubuntu image available for download, but it doesn't include all the features, for example camera is not working on Ubuntu.
For that reason, buildroot image will be used.

## Burning image on SD card

1. Plug the SD card in your PC (use USB adapter in case you don't have SD card reader)
2. Launch SocToolKit
3. Choose `RV1103` chip
4. Go to the `SDtool` tab
4. Select the SD card reader in the `Usb Disk` dropdown menu.
If the SD card reader is not recognized initially, just replug it.
5. Select `SD Boot` to create bootable SD card
6. Click on `Boot Files...` and select all the `img` files from the previously downloaded image, except for `update.img`.
The files should be: `boot.img`, `env.img`, `idblock.img`, `oem.img`, `rootfs.img`, `uboot.img` and `userdata.img`.
7. Press on `Create SD` and the process of creating the SD card will begin.
Log messages of the creation process are shown in the right pane.

More details can be found on [Luckfox wiki page](https://wiki.luckfox.com/Luckfox-Pico/Luckfox-Pico-SD-Card-burn-image).

## Booting-up the board

After burning the image on the SD card, insert the SD card in the board SD card slot and connect the board to the PC.
The red LED next to the USB-C port on the board will start blinking.
Once the board is booted, the red LED will periodically blink, showing that the board is running, like heart-beat.

## Accessing the board via PC

Once the board is booted, it will show up as network adapter, and we can use OpenSSH client to login.
The board has static IP `172.32.0.93`.
We need to configure the correct subnet for the virtual network card that appears on the PC before accessing the board.
One can give the address `172.32.0.100` for the PC virtual network card.
On Windows, follow the guide on [Luckfox wiki page](https://wiki.luckfox.com/Luckfox-Pico/SSH-Telnet-Login).
On Ubuntu, follow the guide on their [docs page](https://ubuntu.com/server/docs/configuring-networks).

To access the board via ssh, type in `ssh root@172.32.0.93`.
The password is `luckfox`.

With this, we successfully booted-up and gained access to the Luckfox Pico board, which now runs Linux with BusyBox.
