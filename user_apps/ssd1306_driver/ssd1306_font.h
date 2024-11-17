/*
 * ssd1306_font.h
 *
 *  Created on: Jan 28, 2022
 *      Author: bojan
 */

#ifndef INC_SSD1306_FONT_H_
#define INC_SSD1306_FONT_H_

/*****************************************
 * Structure for holding font informations
 *****************************************/
typedef struct {
  unsigned char width;          // character width, in pixels
  unsigned char height;         // character height, in pixels
  const unsigned char *fontset; // the actual font data
} Font;

/*****************************************
 * Pre-defined fonts
 *****************************************/
extern const Font SSD1306_FONT6x8;

// Interface Functions
void Font_Init(Font *font, unsigned char width, unsigned char height,
               const unsigned char *fontset);

#endif /* INC_SSD1306_FONT_H_ */
