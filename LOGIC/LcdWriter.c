/*
 * LcdWriter.c
 *
 *  Created on: 7. dets 2023
 *      Author: JRE
 */

#include "LcdWriter.h"
#include "display.h"

Private const tFont * myFonts[NUMBER_OF_FONTS] =
{
     &PowerHourFont, /* FONT_COURIER_14 */
};


Public U8 LcdWriter_drawChar(char c, int x, int y, FontType_t font)
{
    U8 res = 0u;

    if(c >= 0x20u)
    {
        U16 index = c - 0x20;

        if (font < NUMBER_OF_FONTS)
        {
            const tFont * font_ptr = myFonts[font];
            display_drawImage(x, y, font_ptr->chars[index].image->width,
                              font_ptr->chars[index].image->height,
                              font_ptr->chars[index].image->data);

            res = font_ptr->chars[index].image->width;
        }
    }

    return res;
}


Public void LcdWriter_drawString(char * str, int x, int y, FontType_t font)
{
    U16 xCoord = x;
    char * str_ptr = str;

    while(*str_ptr)
    {
        xCoord += LcdWriter_drawChar(*str_ptr, xCoord, y, font);
        str_ptr++;
    }
}
