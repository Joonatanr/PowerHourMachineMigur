/*
 * BitmapHandler.c
 *
 *  Created on: 6. dets 2023
 *      Author: JRE
 */

#include "BitmapHandler.h"
#include "display.h"
#include "MSPIO.h"
#include "fatfs\ff.h"
#include "systimer.h"


typedef enum
{
    BITMAP_HANDLER_IDLE,
    BITMAP_HANDLER_CYCLIC_LOADING,
    BITMAP_HANDLER_COMPLETE,

    NUMBER_OF_BITMAP_HANDLER_STATES
} BitmapHandlerState;

#pragma pack(push)  // save the original data alignment
#pragma pack(1)     // Set data alignment to 1 byte boundary
typedef struct {
    uint16_t type;              // Magic identifier: 0x4d42
    uint32_t size;              // File size in bytes
    uint16_t reserved1;         // Not used
    uint16_t reserved2;         // Not used
    uint32_t offset;            // Offset to image data in bytes from beginning of file
    uint32_t dib_header_size;   // DIB Header size in bytes
    int32_t  width_px;          // Width of the image
    int32_t  height_px;         // Height of image
    uint16_t num_planes;        // Number of color planes
    uint16_t bits_per_pixel;    // Bits per pixel
    uint32_t compression;       // Compression type
    uint32_t image_size_bytes;  // Image size in bytes
    int32_t  x_resolution_ppm;  // Pixels per meter
    int32_t  y_resolution_ppm;  // Pixels per meter
    uint32_t num_colors;        // Number of colors
    uint32_t important_colors;  // Important colors
} BMPHeader;
#pragma pack(pop)  // restore the previous pack setting

//#define BMP_DEBUG_PRINT

#define MAX_BMP_LINE_LENGTH 162u
Private U8 priv_bmp_line_buffer[(MAX_BMP_LINE_LENGTH * 3) + 4u];

volatile U32 debug_begin_timestamp = 0u;
volatile U32 debug_end_timestamp = 0u;
volatile U32 debug_period = 0u;


Private BitmapHandlerState priv_state = BITMAP_HANDLER_IDLE;
Private FIL priv_f_obj;
Private BMPHeader priv_bmp_header;
Private U16 priv_current_line;
Private U16 * priv_dest_ptr;
Private BitmapLoaderCallback priv_load_complete_callback;

#define BITMAP_LOADER_TIME_SLOT 50u /* We need to finish our operations in 50 milliseconds time. */

/***************************** Private function forward declarations *******************************/

Private Boolean setupBitmapLoad(const char * path);
Private Boolean resumeBitmapRead(void);


/***************************** Public functions  **************************************************/

/* This functions loads a bitmap, but it locks up the whole CPU. This can be a problem for realtime application. */
Public Boolean LoadBitmap(const char * path, U16 * dest)
{
    /* Lets try implementing a general bitmap loader that loads a 24-bit BMP file into a provided memory buffer. */
    FRESULT file_res;
    Boolean res = FALSE;
    UINT bytes_read;
    U16 * dest_ptr = dest;
    U32 x, y;

    U16 line_stride;
    U16 line_px_data_len;

    if (priv_state != BITMAP_HANDLER_IDLE)
    {
        return FALSE;
    }

    debug_begin_timestamp = systimer_getTimestamp();

    file_res = f_open(&priv_f_obj, path, FA_READ);

    if (file_res == FR_OK)
    {
        /* Lets first try reading out the bitmap header. */
        file_res = f_read(&priv_f_obj, &priv_bmp_header, sizeof(BMPHeader), &bytes_read);
#ifdef BMP_DEBUG_PRINT
        MSPrintf(EUSCI_A0_BASE, "Size of header is %i\n", sizeof(BMPHeader));
#endif
        if (bytes_read == sizeof(BMPHeader))
        {
#ifdef BMP_DEBUG_PRINT
            MSPrintf(EUSCI_A0_BASE, "Type : %i, Offset : %i width_px : %i, height_px : %i\n",
                     BitmapHeader.type,
                     BitmapHeader.offset,
                     BitmapHeader.width_px,
                     BitmapHeader.height_px);
#endif
            /* Now lets try and read RGB data... */
            if (priv_bmp_header.width_px <= MAX_BMP_LINE_LENGTH)
            {
                /* Take padding into account... */
                line_px_data_len = priv_bmp_header.width_px * 3u;
                line_stride = (line_px_data_len + 3u) & ~0x03;

                for(y = 0u; y < priv_bmp_header.height_px; y++)
                {
                    file_res = f_lseek(&priv_f_obj, ((priv_bmp_header.height_px - (y + 1)) * line_stride) + priv_bmp_header.offset);
                    file_res = f_read(&priv_f_obj, priv_bmp_line_buffer, line_stride, &bytes_read);

                    if (file_res == FR_OK)
                    {
#ifdef BMP_DEBUG_PRINT
                        //MSPrintf(EUSCI_A0_BASE, "Read line %i\n", y);
#endif
                    }
                    else
                    {
#ifdef BMP_DEBUG_PRINT
                        MSPrintf(EUSCI_A0_BASE, "Failed to read line %i\n", y);
#endif
                        break;
                    }

                    for (x = 0u; x < line_px_data_len; x+=3u )
                    {
                        *dest_ptr++ = CONVERT_888RGB_TO_565BGR(priv_bmp_line_buffer[x+2], priv_bmp_line_buffer[x+1], priv_bmp_line_buffer[x]);
                    }
                }

                res = TRUE;
#ifdef BMP_DEBUG_PRINT
                MSPrintf(EUSCI_A0_BASE, "Successfully loaded BMP file %s\n", path);
#endif
            }
            else
            {
#ifdef BMP_DEBUG_PRINT
                MSPrintf(EUSCI_A0_BASE, "BMP file too large for buffer %s\n", path);
#endif
            }
        }
        else
        {
#ifdef BMP_DEBUG_PRINT
            MSPrintf(EUSCI_A0_BASE, "Failed to read BMP file header for %s\n", path);
#endif
        }
    }
    else
    {
#ifdef BMP_DEBUG_PRINT
        MSPrintf(EUSCI_A0_BASE, "Failed to open BMP file %s\n", path);
#endif
    }

    debug_end_timestamp = systimer_getTimestamp();
    debug_period = debug_end_timestamp - debug_begin_timestamp;
    return res;
}


Public Boolean StartCyclicBitmapLoad(const char * path, U16 * dest, BitmapLoaderCallback cb)
{
    Boolean res = FALSE;

    if (priv_state != BITMAP_HANDLER_IDLE)
    {
        return FALSE;
    }

    if (setupBitmapLoad(path) == TRUE)
    {
        priv_current_line = 0u;
        priv_dest_ptr = dest;
        priv_load_complete_callback = cb;
        priv_state = BITMAP_HANDLER_CYCLIC_LOADING;
        res = TRUE;
    }

    return res;
}


Public void BitmapLoaderCyclic100ms(void)
{
    switch (priv_state)
    {
        case (BITMAP_HANDLER_IDLE):
            /* Nothing to do here... */
            break;
        case (BITMAP_HANDLER_CYCLIC_LOADING):
            if (resumeBitmapRead() == TRUE)
            {
                priv_state = BITMAP_HANDLER_COMPLETE;
            }
            break;
        case (BITMAP_HANDLER_COMPLETE):
            if (priv_load_complete_callback != NULL)
            {
                priv_load_complete_callback();
            }
            priv_state = BITMAP_HANDLER_IDLE;
            break;
        default:
            break;
    }
}


/***************************** Private function definitions *******************************/
Private Boolean setupBitmapLoad(const char * path)
{
    FRESULT file_res;
    Boolean res = FALSE;
    UINT bytes_read;

    file_res = f_open(&priv_f_obj, path, FA_READ);

    if (file_res == FR_OK)
    {
        /* Lets first try reading out the bitmap header. */
        file_res = f_read(&priv_f_obj, &priv_bmp_header, sizeof(BMPHeader), &bytes_read);

        if (bytes_read == sizeof(BMPHeader))
        {
            /* TODO : Consider, if we should always just ignore a larger bitmap?? Initially lets just get the BMP reading to work properly and then we can start looking at special cases like this. */
            if (priv_bmp_header.width_px <= MAX_BMP_LINE_LENGTH)
            {
                res = TRUE;
            }
        }
    }

    return res;
}


Private Boolean resumeBitmapRead(void)
{
    U16 line_stride;
    U16 line_px_data_len;
    U32 begin_time;
    UINT bytes_read;
    FRESULT file_res;
    U32 x;

    Boolean res = FALSE; /* Return true if finished reading. */

    begin_time = systimer_getTimestamp();

    do
    {
        /* Now lets try and read RGB data... */
        /* Take padding into account... */
        line_px_data_len = priv_bmp_header.width_px * 3u;
        line_stride = (line_px_data_len + 3u) & ~0x03;

        file_res = f_lseek(&priv_f_obj, ((priv_bmp_header.height_px - (priv_current_line + 1)) * line_stride) + priv_bmp_header.offset);
        file_res = f_read(&priv_f_obj, priv_bmp_line_buffer, line_stride, &bytes_read);
        priv_current_line++;

        if (file_res == FR_OK)
        {
            for (x = 0u; x < line_px_data_len; x+=3u )
            {
                *priv_dest_ptr++ = CONVERT_888RGB_TO_565BGR(priv_bmp_line_buffer[x+2], priv_bmp_line_buffer[x+1], priv_bmp_line_buffer[x]);
            }
        }

        if (priv_current_line >= priv_bmp_header.height_px)
        {
            res = TRUE;
            break;
        }
    }
    while(systimer_getPeriod(begin_time) < BITMAP_LOADER_TIME_SLOT);

    return res;
}
