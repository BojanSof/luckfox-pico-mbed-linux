ccflags-y += -std=gnu99

obj-m += ssd1306_drv.o
ssd1306_drv-objs := ssd1306_driver.o ssd1306.o ssd1306_font.o

KDIR := ../../sysdrv/source/objs_kernel

all:
	make ARCH=arm CROSS_COMPILE=arm-rockchip830-linux-uclibcgnueabihf- -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.mod *.mod.o *.mod.c *.symvers *.order
