/*
 * display.h
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#ifndef HW_DISPLAY_H_
#define HW_DISPLAY_H_

#include "typedefs.h"
#include "Bitmaps/Bitmaps.h"

#define CONVERT_888RGB_TO_565BGR(r, g, b) ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11))
/* The problem here is that we shall be accessing memory via DMA, 1 byte at a time. So we also need to switch the MSB and LSB of the resulting U16 here. */
//#define CONVERT_888RGB_TO_565BGR(r, g, b) ((r >> 3) << 8 | ((g >> 5) << 13) | ((b >> 3) << 3) | ((g >> 2) & 0x0007u))

//#define COLOR_BLUE (U16)(CONVERT_888RGB_TO_565BGR(0x00u, 0x00u, 0xFFu))
//#define COLOR_RED (U16)(CONVERT_888RGB_TO_565BGR(0xFFu, 0x00u, 0x00u))
//#define COLOR_GREEN (U16)(CONVERT_888RGB_TO_565BGR(0x00u, 0xFFu, 0x00u))

#define COLOR_BLACK    CONVERT_888RGB_TO_565BGR(0,  0,  0   )
#define COLOR_BLUE     CONVERT_888RGB_TO_565BGR(0,  0,  255 )
#define COLOR_RED      CONVERT_888RGB_TO_565BGR(255,0,  0   )
#define COLOR_GREEN    CONVERT_888RGB_TO_565BGR(0,  255,0   )
#define COLOR_CYAN     CONVERT_888RGB_TO_565BGR(0,  255,255 )
#define COLOR_MAGENTA  CONVERT_888RGB_TO_565BGR(255,  0,255 )
#define COLOR_YELLOW   CONVERT_888RGB_TO_565BGR(255,255,0   )
#define COLOR_WHITE    CONVERT_888RGB_TO_565BGR(255,255,255 )


#define DISPLAY_WIDTH 162u
#define DISPLAY_HEIGHT 132u
#define DISPLAY_CENTER ((DISPLAY_WIDTH / 2u) + 4u)


extern void display_init(void);

extern U16 * display_get_frame_buffer(void);
extern void display_flushBuffer(U8 x, U8 y, U8 width, U8 height);
extern void display_drawBitmapCenter(const U16 * src_ptr, U16 centerPoint, U16 y, U8 width, U8 height);
extern void display_drawTimageCenter(const tImage * image_ptr, U16 centerPoint, U16 y);
extern void display_drawImage(U8 x, U8 y, U8 width, U8 height, const U16 * src_ptr);
extern void display_fill(U16 color);
extern void display_fillRectangle(U8 x, U8 y, U8 width, U8 height, U16 color);
extern void display_clear(void);



#endif /* HW_DISPLAY_H_ */
