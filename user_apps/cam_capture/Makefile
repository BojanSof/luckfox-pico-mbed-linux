CC = arm-rockchip830-linux-uclibcgnueabihf-gcc
CFLAGS = -I../../sysdrv/source/buildroot/buildroot-2023.02.6/output/staging/usr/include
LDFLAGS = -L../../sysdrv/source/buildroot/buildroot-2023.02.6/output/staging/usr/lib -lv4l2 -lv4lconvert

cam_capture: cam_capture.c
	$(CC) $(CFLAGS) -o cam_capture cam_capture.c $(LDFLAGS)
