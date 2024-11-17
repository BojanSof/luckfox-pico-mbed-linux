#include <linux/init.h>
#include <linux/module.h>

#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include "ssd1306.h"
#include "ssd1306_font.h"

#define DRIVER_NAME "ssd1306_disp"
#define DRIVER_CLASS "ssd1306"
#define CLIENT_NAME "SSD1306_DISP"
#define SSD1306_I2C_BUS 3

static SSD1306 ssd1306;

static struct i2c_adapter *ssd1306_i2cAdapter = NULL;
static struct i2c_client *ssd1306_i2cClient = NULL;

static dev_t ssd1306_dev_id;
static struct cdev ssd1306_cdev;
static struct class *ssd1306_class = NULL;

#define CHAR_DEV_BUFFER_SIZE 256
static unsigned char ssd1306_char_dev_buffer[CHAR_DEV_BUFFER_SIZE];

static int ssd1306_i2cWrite(const unsigned char *data,
                            const unsigned char size);
static int ssd1306_probe(struct i2c_client *client,
                         const struct i2c_device_id *id);
static int ssd1306_remove(struct i2c_client *client);

static int ssd1306_char_open(struct inode *inode, struct file *file);
static ssize_t ssd1306_char_write(struct file *file, const char __user *buffer,
                                  size_t count, loff_t *ppos);
static int ssd1306_char_release(struct inode *inode, struct file *file);

static int ssd1306_i2cWrite(const unsigned char *data,
                            const unsigned char size) {
  int ret = i2c_master_send(ssd1306_i2cClient, data, size);
  return ret;
}

static int ssd1306_probe(struct i2c_client *client,
                         const struct i2c_device_id *id) {
  int ret;
  msleep(100);
  ret = SSD1306_Init(&ssd1306, &SSD1306_FONT6x8, ssd1306_i2cWrite);
  if (ret < 0) {
    pr_err("Failed to initialize SSD1306 driver\n");
    return ret;
  }
  SSD1306_SetCursor(&ssd1306, 0, 0);
	SSD1306_WriteString(&ssd1306, "SSD1306 OLED");
	SSD1306_FillRegion(&ssd1306, 0, 8, SSD1306_SCREEN_WIDTH, 8, 1);
  SSD1306_SetCursor(&ssd1306, 0, 16);
  ret = SSD1306_UpdateScreen(&ssd1306);
  if (ret < 0) {
    pr_err("Failed to init SSD1306 display content\n");
    return ret;
  }
  pr_info("Initialized SSD1306 I2C driver\n");
  return 0;
}

static int ssd1306_remove(struct i2c_client *client) {
  int ret = SSD1306_WriteCommand(&ssd1306, SSD1306_DISPLAY_OFF);
  if (ret < 0) {
    pr_err("Failed to turn off display\n");
  }
  return ret;
}

static int ssd1306_char_open(struct inode *inode, struct file *file) {
  pr_info("Open SSD1306 char dev\n");
  return 0;
}

static ssize_t ssd1306_char_write(struct file *file, const char __user *buffer,
                                  size_t count, loff_t *ppos) {
  int n_bytes = 0;
  unsigned char curX, curY;
  n_bytes = count - copy_from_user(ssd1306_char_dev_buffer, buffer, count);
  if(count == 1 && ssd1306_char_dev_buffer[0] == '\0') {
    SSD1306_FillRegion(&ssd1306, 0, 16, SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT - 16, 0);
    SSD1306_UpdateScreen(&ssd1306);
    SSD1306_SetCursor(&ssd1306, 0, 16);
  } else {
    ssd1306_char_dev_buffer[count] = '\0';
    SSD1306_WriteString(&ssd1306, (const char*)ssd1306_char_dev_buffer);
    SSD1306_UpdateScreen(&ssd1306);
    SSD1306_GetCursor(&ssd1306, &curX, &curY);
    if(curY == SSD1306_SCREEN_HEIGHT) {
      curY = 16;
    } else {
      curY += 8;
    }
    curX = 0;
    SSD1306_SetCursor(&ssd1306, curX, curY);
  }
  return n_bytes;
}

static int ssd1306_char_release(struct inode *inode, struct file *file) {
  pr_info("Close SSD1306 char dev\n");
  return 0;
}

static const struct file_operations ssd1306_char_fops = {
    .owner = THIS_MODULE,
    .open = ssd1306_char_open,
    .release = ssd1306_char_release,
    .read = NULL,
    .write = ssd1306_char_write,
};

static struct i2c_board_info ssd1306_i2cBoardInfo = {
    I2C_BOARD_INFO(CLIENT_NAME, SSD1306_I2C_ADDR)};

static struct i2c_device_id ssd1306_idtable[] = {{CLIENT_NAME, 0}, {}};

static struct i2c_driver ssd1306_i2c_driver = {
    .driver = {.name = CLIENT_NAME, .owner = THIS_MODULE},
    .id_table = ssd1306_idtable,
    .probe = ssd1306_probe,
    .remove = ssd1306_remove,
};


static int __init ssd1306_driver_init(void) {
  int ret = 0;

  // allocate region for char device
  ret = alloc_chrdev_region(&ssd1306_dev_id, 0, 1, DRIVER_NAME);
  if (ret < 0) {
    pr_err("Failed to allocated SSD1306 char dev region\n");
    goto alloc_chrdev_error;
  }
  // create and register device in sysfs
  ssd1306_class = class_create(THIS_MODULE, DRIVER_CLASS);
  if (ssd1306_class == NULL) {
    pr_err("Failed to create class for SSD1306 device\n");
    goto class_error;
  }
  if (device_create(ssd1306_class, NULL, ssd1306_dev_id, NULL, DRIVER_NAME) ==
      NULL) {
    pr_err("Failed to create device file for SSD1306\n");
    goto device_create_error;
  }

  // char device init and add to system
  cdev_init(&ssd1306_cdev, &ssd1306_char_fops);
  ret = cdev_add(&ssd1306_cdev, ssd1306_dev_id, 1);
  if (ret < 0) {
    pr_err("Failed to add SSD1306 device to system\n");
    goto cdev_add_error;
  }

  // i2c dev init
  ssd1306_i2cAdapter = i2c_get_adapter(SSD1306_I2C_BUS);
  if (ssd1306_i2cAdapter == NULL) {
    pr_err("Failed to get I2C adapter for %d\n", SSD1306_I2C_BUS);
    goto i2c_adapter_error;
  }
  ssd1306_i2cClient =
      i2c_new_client_device(ssd1306_i2cAdapter, &ssd1306_i2cBoardInfo);
  if (ssd1306_i2cClient == NULL) {
    pr_err("Failed to create I2C client for SSD1306\n");
    goto i2c_client_error;
  }
  ret = i2c_add_driver(&ssd1306_i2c_driver);
  if (ret < 0) {
    pr_err("Failed to add I2C driver for SSD1306\n");
    goto i2c_add_error;
  }
  i2c_put_adapter(ssd1306_i2cAdapter);

  pr_info("SSD1306 driver added\n");
  return 0;
i2c_add_error:
  i2c_unregister_device(ssd1306_i2cClient);
i2c_client_error:
  i2c_put_adapter(ssd1306_i2cAdapter);
i2c_adapter_error:
cdev_add_error:
  device_destroy(ssd1306_class, ssd1306_dev_id);
device_create_error:
  class_destroy(ssd1306_class);
class_error:
  unregister_chrdev_region(ssd1306_dev_id, 1);
alloc_chrdev_error:
  return -1;
}

static void __exit ssd1306_driver_exit(void) {
  i2c_unregister_device(ssd1306_i2cClient);
  i2c_del_driver(&ssd1306_i2c_driver);
  cdev_del(&ssd1306_cdev);
  device_destroy(ssd1306_class, ssd1306_dev_id);
  class_destroy(ssd1306_class);
  unregister_chrdev_region(ssd1306_dev_id, 1);
  pr_info("SSD1306 driver removed\n");
}

module_init(ssd1306_driver_init);
module_exit(ssd1306_driver_exit);

MODULE_AUTHOR("Bojan");
MODULE_DESCRIPTION("SSD1306 Driver");
MODULE_LICENSE("GPL");
