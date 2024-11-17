/*
 * ssd1306.c
 *
 *  Created on: Jan 17, 2022
 *      Author: bojan
 */

#include "ssd1306.h"

/***********************************************
 * Interface functions
 ***********************************************/

/***********************************************
 * Initialize SSD1306 and structure data
 *
 * Refer to p. 5 in the Application Note
 * for SSD1306
 ***********************************************/
int SSD1306_Init(SSD1306 *dev, const Font *font, SSD1306_i2cWrite i2cWrite) {
  unsigned char errNum = 0; // the number of error transactions
  int status;               // the status of each transaction

  dev->i2cWrite = i2cWrite;  // set the function for i2c write operation
  dev->curX = dev->curY = 0; // set the cursor to the top left corner
  dev->font = font;          // font to use for text

  // clear the screen buffer
  for (unsigned short i = 0; i < SSD1306_SCREEN_BUFFER_SIZE; i++) {
    dev->screenBuffer[i] = 0;
  }

  // send commands for initialization
  // 1. Turn off the display
  status = SSD1306_WriteCommand(dev, SSD1306_DISPLAY_OFF);
  errNum += (status < 0);
  // 2. Set MUX Ratio to 64
  status = SSD1306_WriteCommand(dev, SSD1306_SET_MUX_RATIO);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(
      dev, 0x3F); // mux ratio is n + 1, write 63 for mux ratio 64
  errNum += (status < 0);
  // 3. Set page addressing mode
  status = SSD1306_WriteCommand(dev, SSD1306_SET_MEM_ADDR_MODE);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, SSD1306_PAGE_ADDR_MODE);
  errNum += (status < 0);
  // 4. Set the column start address to zero
  // first, the four less significant bits
  status = SSD1306_WriteCommand(dev, SSD1306_SET_LOWER_COL_ADDR | 0x00);
  errNum += (status < 0);
  // then, the four more significant bits
  status = SSD1306_WriteCommand(dev, SSD1306_SET_HIGHER_COL_ADDR | 0x00);
  errNum += (status < 0);
  // 5. Set GDDRAM page start address
  status = SSD1306_WriteCommand(dev, SSD1306_SET_PAGE_START_ADDR | 0x00);
  errNum += (status < 0);
  // 6. Set display offset to 0
  status = SSD1306_WriteCommand(dev, SSD1306_SET_DISPLAY_OFFSET);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, 0x00);
  errNum += (status < 0);
  // 7. Set display start line to 0
  status = SSD1306_WriteCommand(dev, SSD1306_SET_DISPLAY_START_LINE | 0x00);
  errNum += (status < 0);
  // 8. Segment re-map (should be changed if screen is horizontally mirrored)
  status = SSD1306_WriteCommand(dev, SSD1306_SET_SEGMENT_REMAP);
  errNum += (status < 0);
  // 9. COM output scan direction (should be changed if screen is vertically
  // mirrored)
  status = SSD1306_WriteCommand(dev, SSD1306_SET_COM_REMAP);
  errNum += (status < 0);
  // 10. Set COM pins hardware configuration (p. 40, table 10-3 in datasheet)
  // Alternative COM pin configuration and Disable COM Left/Right re-map
  status = SSD1306_WriteCommand(dev, SSD1306_SET_COM_PINS_HW_CONFIG);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, 0x12);
  errNum += (status < 0);
  // 11. Set contrast control
  status = SSD1306_WriteCommand(dev, SSD1306_SET_CONTRAST);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, 0xFF);
  errNum += (status < 0);
  // 12. Disable entire display on (output follows RAM content)
  status = SSD1306_WriteCommand(dev, SSD1306_ENTIRE_DISPLAY_OFF);
  // 13. Set normal display mode
  status = SSD1306_WriteCommand(dev, SSD1306_SET_NORMAL_DISPLAY);
  errNum += (status < 0);
  // 14. Set Osc Frequency
  status = SSD1306_WriteCommand(dev, SSD1306_SET_DISPLAY_CLOCK_DIVIDE);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, 0xF0);
  errNum += (status < 0);
  // 15. Set pre-charge period
  status = SSD1306_WriteCommand(dev, SSD1306_SET_PRECHARGE_PERIOD);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, 0x22);
  errNum += (status < 0);
  // 16. Set Vcomh level to 0.77xVcc
  status = SSD1306_WriteCommand(dev, SSD1306_SET_VCOMH_DESELECT_LEVEL);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, 0x20);
  errNum += (status < 0);
  // 17. Enable charge pump regulator
  status = SSD1306_WriteCommand(dev, SSD1306_CHARGE_PUMP_SETTING);
  errNum += (status < 0);
  status = SSD1306_WriteCommand(dev, SSD1306_CHARGE_PUMP_ENABLED);
  errNum += (status < 0);
  // 18. Turn on the display
  status = SSD1306_WriteCommand(dev, SSD1306_DISPLAY_ON);
  errNum += (status < 0);

  return errNum;
}

/***********************************************
 * Update the screen with the buffer content
 ***********************************************/
int SSD1306_UpdateScreen(SSD1306 *dev) {
  unsigned char errNum = 0; // the number of error transactions
  int status;               // the status of each transaction

  // write the data for the RAM page by page
  for (unsigned char i = 0; i < SSD1306_NUMBER_PAGES; i++) {
    // set page start address
    status = SSD1306_WriteCommand(dev, SSD1306_SET_PAGE_START_ADDR | i);
    errNum += (status < 0);
    // write the page data
    status =
        SSD1306_WriteData(dev, dev->screenBuffer + i * SSD1306_SCREEN_WIDTH,
                          SSD1306_SCREEN_WIDTH);
    errNum += (status < 0);
  }

  return errNum;
}

/***********************************************
 * Set cursor for the display buffer
 ***********************************************/
void SSD1306_SetCursor(SSD1306 *dev, unsigned char x, unsigned char y) {
  if (x >= SSD1306_SCREEN_WIDTH || y >= SSD1306_SCREEN_HEIGHT) {
    return;
  }
  dev->curX = x;
  dev->curY = y;
}

/***********************************************
 * Get cursor for the display buffer
 ***********************************************/
void SSD1306_GetCursor(SSD1306 *dev, unsigned char *x, unsigned char *y) {
  *x = dev->curX;
  *y = dev->curY;
}

/***********************************************
 * Clear the display buffer
 ***********************************************/
void SSD1306_ClearBuffer(SSD1306 *dev) {
  for (unsigned short i = 0; i < SSD1306_SCREEN_BUFFER_SIZE; i++) {
    dev->screenBuffer[i] = 0;
  }
}

/***********************************************
 * Set pixel state in the buffer
 ***********************************************/
void SSD1306_SetPixelState(SSD1306 *dev, unsigned char x, unsigned char y,
                           unsigned char state) {
  if (x >= SSD1306_SCREEN_WIDTH || y >= SSD1306_SCREEN_HEIGHT) {
    return;
  }
  if (state == 0) {
    dev->screenBuffer[x + (y / 8) * SSD1306_SCREEN_WIDTH] &= ~(1 << (y % 8));
  } else {
    dev->screenBuffer[x + (y / 8) * SSD1306_SCREEN_WIDTH] |= (1 << (y % 8));
  }
}

/******************************************
 * Write character in the buffer
 * Note: in this moment the function only
 * works for font size 6x8 pixels
 ******************************************/
void SSD1306_WriteChar(SSD1306 *dev, char c) {
  unsigned short ci, row, col, state;
  // check if there is enough space to print
  // the character; if not, do nothing
  if (dev->curX + dev->font->width >= SSD1306_SCREEN_WIDTH ||
      dev->curY + dev->font->height >= SSD1306_SCREEN_HEIGHT) {
    return;
  }
  // determine character starting index in fontset
  ci = (c - 32) * (dev->font->width);
  // draw the pixels
  for (col = 0; col < dev->font->width; col++) {
    for (row = 0; row < dev->font->height; row++) {
      state = ((dev->font->fontset[ci + col]) & (0x1 << row)) != 0;
      SSD1306_SetPixelState(dev, dev->curX + col, dev->curY + row, state);
    }
  }
  // update the cursor position
  dev->curX += dev->font->width;
}

/******************************************
 * Write string in the buffer
 ******************************************/
void SSD1306_WriteString(SSD1306 *dev, const char *str) {
  while (*str != 0) {
    SSD1306_WriteChar(dev, *str++);
  }
}

/*******************************************
 * Change the state of all pixels in
 * a rectangular region, with top left
 * corner located at (x, y), with the given
 * width and height
 *******************************************/
void SSD1306_FillRegion(SSD1306 *dev, unsigned char x, unsigned char y,
                        unsigned char width, unsigned char height,
                        unsigned char state) {
  unsigned char row, col;
  // do nothing if region is out of screen bounds
  if (x >= SSD1306_SCREEN_WIDTH || y >= SSD1306_SCREEN_HEIGHT ||
      x + width > SSD1306_SCREEN_WIDTH || y + height > SSD1306_SCREEN_HEIGHT) {
    return;
  }

  for (col = x; col < x + width; col++) {
    for (row = y; row < y + height; row++) {
      SSD1306_SetPixelState(dev, col, row, state);
    }
  }
}

/*******************************************
 * Invert the state of all pixels in
 * a rectangular region, with top left
 * corner located at (x, y), with the given
 * width and height
 *******************************************/
void SSD1306_InvertRegion(SSD1306 *dev, unsigned char x, unsigned char y,
                          unsigned char width, unsigned char height) {
  unsigned char row, col, state;
  // do nothing if region is out of screen bounds
  if (x >= SSD1306_SCREEN_WIDTH || y >= SSD1306_SCREEN_HEIGHT ||
      x + width > SSD1306_SCREEN_WIDTH || y + height > SSD1306_SCREEN_HEIGHT) {
    return;
  }

  for (col = x; col < x + width; col++) {
    for (row = y; row < y + height; row++) {
      // get the current state
      state = dev->screenBuffer[col + (row / 8) * SSD1306_SCREEN_WIDTH] &
              (1 << (row % 8));
      // invert the state
      state = (state > 0) ? 0 : 1;
      // set the new state
      SSD1306_SetPixelState(dev, col, row, state);
    }
  }
}

/***********************************************
 * Low-Level functions
 ***********************************************/

/************************************************
 * Write command to ssd1306
 * First, send control byte with c0 = 1
 * Then, send the command byte
 ************************************************/
int SSD1306_WriteCommand(SSD1306 *dev, unsigned char cmd) {
  const unsigned char payloadType = SSD1306_CONTROL_CMD_SINGLE;
  const unsigned char buf[] = {payloadType, cmd};
  int ret = 0;
  ret = dev->i2cWrite(buf, sizeof(buf));
  return ret;
}

/************************************************
 * Write data to ssd1306
 * First, send control byte with c0 = 0 (don't
 * resend control bytes)
 * Then, send the stream of data bytes
 ************************************************/
int SSD1306_WriteData(SSD1306 *dev, unsigned char *data, unsigned char size) {
  const unsigned char payloadType = SSD1306_CONTROL_DATA_CONT;
  int ret = 0;
  if (size + 1 > SSD1306_WORK_BUFFER_SIZE) {
    return -1;
  }
  dev->workBuffer[0] = payloadType;
  for (unsigned char i = 0; i < size; ++i) {
    dev->workBuffer[i + 1] = data[i];
  }
  ret = dev->i2cWrite(dev->workBuffer, size + 1);
  return ret;
}
