CC = arm-rockchip830-linux-uclibcgnueabihf-gcc
CFLAGS = -I../../sysdrv/source/buildroot/buildroot-2023.02.6/output/staging/usr/include
LDFLAGS = -L../../sysdrv/source/buildroot/buildroot-2023.02.6/output/staging/usr/lib -lgpiod

morse_code: morse_code.c
	$(CC) $(CFLAGS) -o morse_code morse_code.c $(LDFLAGS)
