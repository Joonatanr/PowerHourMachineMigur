#include "Fonts.h"

Private const tFont * priv_fonts[NUMBER_OF_FONTS] =
{
     &PowerHourFont, /* FONT_COURIER_14 */
     &Courier16Bold, /* FONT_COURIER_16_BOLD */
};


Public const tFont * font_get_font_ptr(FontType_t font)
{
    if (font < NUMBER_OF_FONTS)
    {
        return priv_fonts[font];
    }
    else
    {
        return NULL;
    }
}

Public U8 font_getCharWidth(char asc, FontType_t font)
{
    const tFont * font_info_ptr = priv_fonts[font];
    char c;

    U8 res = 0u;

    if ((asc >= 0x20u) && (asc <= (0x20u + font_info_ptr->length)))
    {
        c = asc - 0x20u;
        res = font_info_ptr->chars[c].image->width;
    }

    return res;
}
