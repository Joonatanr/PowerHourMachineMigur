/*
 * Fonts.h
 *
 *  Created on: 6. dets 2023
 *      Author: JRE
 */

#ifndef LOGIC_FONTS_FONTS_H_
#define LOGIC_FONTS_FONTS_H_

#include "typedefs.h"

 typedef struct
 {
     const uint16_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
 } tImage;

 typedef struct
 {
     long int code;
     const tImage *image;

 } tChar;

 typedef struct
 {
     int length;
     const tChar *chars;
 } tFont;


typedef enum
{
    FONT_SMALL_FONT_10,
    FONT_COURIER_14, /* TODO : Is this actually necessary? */
    FONT_COURIER_16_BOLD,
    FONT_TNR_HUGE_NUMBERS,
    NUMBER_OF_FONTS
} FontType_t;

/* Legacy conversion */
#define FONT_MEDIUM_FONT FONT_COURIER_14

extern const tFont PowerHourFont;
extern const tFont Courier16Bold;
extern const tFont ClockFont;
extern const tFont SmallFont;


extern U8 font_getCharWidth(char asc, FontType_t font);
extern U8 font_getFontHeight(FontType_t font);
extern const tFont * font_get_font_ptr(FontType_t font);


#endif /* LOGIC_FONTS_FONTS_H_ */
