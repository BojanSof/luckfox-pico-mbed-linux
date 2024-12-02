#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>

int main() {
  const char *device = "/dev/video11";
  // open camera device
  int fd = open(device, O_RDWR);
  if (fd < 0) {
    perror("Failed to open device");
    return 1;
  }

  // check if opened device is compatible with v4l2 specification
  struct v4l2_capability capability;
  if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
    perror("Failed to get device capabilities");
    return 2;
  }

  // configure camera capture format
  struct v4l2_format fmt = {};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  fmt.fmt.pix_mp.width = 640;
  fmt.fmt.pix_mp.height = 480;
  fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12;
  fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
  if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
    perror("Failed to set device image format");
    return 3;
  }

  // request buffers from the device
  struct v4l2_requestbuffers request_buffer = {};
  request_buffer.count = 1;
  request_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  request_buffer.memory = V4L2_MEMORY_MMAP;
  if (ioctl(fd, VIDIOC_REQBUFS, &request_buffer) < 0) {
    perror("Could not request buffer from device");
    return 4;
  }

  // query the status of the request buffer
  struct v4l2_buffer query_buffer = {};
  struct v4l2_plane plane;
  query_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  query_buffer.memory = V4L2_MEMORY_MMAP;
  query_buffer.index = 0;
  query_buffer.length = fmt.fmt.pix_mp.num_planes;
  query_buffer.m.planes = &plane;
  if (ioctl(fd, VIDIOC_QUERYBUF, &query_buffer) < 0) {
    perror("Failed to query buffer status");
    return 5;
  }

  // memory map the device buffers into memory
  char *buffer = (char *)mmap(NULL, query_buffer.m.planes[0].length,
                              PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                              query_buffer.m.planes[0].m.mem_offset);
  memset(buffer, 0, query_buffer.length);

  // queue buffers in which to store frames (prevents swapping of memory pages)
  struct v4l2_buffer bufd = {};
  memset(&bufd, 0, sizeof(bufd));
  bufd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  bufd.memory = V4L2_MEMORY_MMAP;
  bufd.index = 0;
  bufd.length = fmt.fmt.pix_mp.num_planes;
  bufd.m.planes = &plane;
  if (ioctl(fd, VIDIOC_QBUF, &bufd) < 0) {
    perror("Failed to queue buffer");
    return 7;
  }

  // activate streaming
  int type = bufd.type;
  if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
    perror("Could not start streaming from device");
    return 6;
  }

  // dequeue the buffer, to read frame
  if (ioctl(fd, VIDIOC_DQBUF, &bufd) < 0) {
    perror("Could not dequeue the buffer, VIDIOC_DQBUF");
    return 8;
  }
  printf("Captured buffer has %.2f kB of data\n",
         (float)bufd.m.planes[0].bytesused / 1024);
  FILE *file = fopen("frame.raw", "wb");
  if (file) {
    fwrite(buffer, bufd.m.planes[0].bytesused, 1, file);
    fclose(file);
    printf("Frame saved to frame.raw\n");
  }

  // end streaming
  if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
    perror("Could not end streaming, VIDIOC_STREAMOFF");
    return 9;
  }

  close(fd);

  return 0;
}
