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

#endif /* LOGIC_LCDWRITER_H_ */
