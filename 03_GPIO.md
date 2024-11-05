# Interfacing with GPIOs

GPIO (General Purpose Input/Output) pins are the most basic interfaces to other components in an embedded system.
In the most basic form, they allow to set specific pins on logic zero or one, which translates to voltage, for example 0 and 3.3 Volts.
They also allow to read the state of a given pin, based on the voltage present on that pin.

## `sysfs` GPIO interfacing

The Linux kernel provides pseudo-filesystem, called [`sysfs`](https://en.wikipedia.org/wiki/Sysfs), which has virtual files that can be used to configure specific hardware devices.

The GPIO also has such virtual files in the `/sys/class/gpio` directory and the [Luckfox wiki page](https://wiki.luckfox.com/Luckfox-Pico/Luckfox-Pico-GPIO/) contains info how to configure and interface with the GPIOs using these virtual files.
In summary, to control the GPIOs in this way, the following steps should be performed in shell environment:
1. Export the specific GPIO pin to user-space
```
echo <pin_number> > /sys/class/gpio/export
```
2. Configure the pin as input or output:
- input:
```
echo in > /sys/class/gpio<pin_number>/direction
cat /sys/class/gpio<pin_number>/value
```
- output:
```
echo out > /sys/class/gpio<pin_number>/direction
echo 0 > /sys/class/gpio<pin_number>/value
echo 1 > /sys/class/gpio<pin_number>/value
```
3. Unexport the GPIO pin from user-space after usage
```
echo <pin_number> > /sys/class/gpio/unexport
```

For the Luckfox pico board, the GPIOs are described in the following way: `GPIO{bank}_{group}{X}` (ex. `GPIO1_C7`) and the pin number can be found by utilizing the following formula:
```
pin_number = bank x 32 + (group x 8 + X)
```
The same thing can be done in C program, by utilizing file operations.

## `libgpiod`: New and better way for interfacing GPIOs

However, the `sysfs` way of interfacing GPIO is deprecated as stated in the Linux kernel [documentation](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt) and there is new alternative using the `libgpiod` library, which provides APIs for easy interfacing with the GPIOs.

By default, the buildroot image does not include this library and the utility programs.
For this reason, we need to enable it via buildroot configuration.
We can achieve this by following these steps, inside the root directory of the SDK:
1. Execute `./build.sh buildrootconfig` and the buildroot configuration menuconfig should appear
2. Go to `Target Packages`
3. Go to `Libraries`
4. Go to `Hardware handling`
5. Select `libgpiod`, along with `install tools` under it. The tools include some CLI programs that allow to interface the GPIOs.
After enabling `libgpiod` in buildroot config, build the image and burn it on the SD card.
Now, after connecting to the luckfox pico board, there are few new programs that can be run on the board that utilize `libgpiod`:

- `gpioinfo`, which shows the current states of all GPIOs;
- `gpioset` to set the state of a given GPIO: `gpioset gpiochip1 23=1`;
- `gpioget` to get the state of a given GPIO: `gpioget gpiochip1 23`;

Note that the GPIOs when using `libgpiod` built with the steps above are split in banks, but not in groups.
That means that for example the pin labeled `GPIO1_C7` is the 23rd pin in `gpiochip1`.

### C example program

Finally, we will provide sample C program that utilizes `libgpiod` interface and provide steps to cross-compile the program and transfer it to our board.
For this program, we connect red LED via 470 Ohms series resistor on the pin annotated `GPIO1_C7`.
The example will create morse code on the LED from the input text given in the terminal.

First, create directory called `user_apps` in the SDK root.
This directory will be used for all other examples.

In the `user_apps` subdirectory create new directory, `morse_code`, that will contain the source code and makefile for our example program.

We need to provide the include directories for `libgpiod` and link with it when building the example.
The cleanest way to do this is to create new buildroot package, however, to simplify the process for the examples, this will be done manually.

The directory where the `libgpiod` headers are stored is found in `sysdrv/source/buildroot/buildroot-2023.02.6/output/staging/usr/include` in the SDK root, while the library itself is built in `sysdrv/source/buildroot/buildroot-2023.02.6/output/staging/usr/lib`.

Refer to the example for more deatils.
