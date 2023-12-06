/*
 * Fonts.h
 *
 *  Created on: 6. dets 2023
 *      Author: JRE
 */

#ifndef LOGIC_FONTS_FONTS_H_
#define LOGIC_FONTS_FONTS_H_

#include "typedefs.h"

 typedef struct {
     const uint16_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
     } tImage;
 typedef struct {
     long int code;
     const tImage *image;
     } tChar;
 typedef struct {
     int length;
     const tChar *chars;
     } tFont;

extern const tFont PowerHourFont;

#endif /* LOGIC_FONTS_FONTS_H_ */
