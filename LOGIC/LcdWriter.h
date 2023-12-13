/*
 * LcdWriter.h
 *
 *  Created on: 7. dets 2023
 *      Author: JRE
 */

#ifndef LOGIC_LCDWRITER_H_
#define LOGIC_LCDWRITER_H_


#include "typedefs.h"
#include "Fonts/Fonts.h"

extern U8 LcdWriter_drawChar(char c, int x, int y, FontType_t font);
extern void LcdWriter_drawString(char * str, int x, int y, FontType_t font);
extern U8 LcdWriter_drawCharColored(char c, int x, int y, FontType_t font, U16 foreground, U16 background);
extern void LcdWriter_drawColoredString(const char * str, int x, int y, FontType_t font, U16 foreground, U16 background);
extern void LcdWriter_drawStringCenter(const char * str, U8 centerPoint, U8 yloc, FontType_t font, U16 foreground, U16 background);

#endif /* LOGIC_LCDWRITER_H_ */
