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

#define BMP_DEBUG_PRINT

#define MAX_BMP_LINE_LENGTH 162u
/* TODO : Padding should be fixed. */
U8 priv_bmp_line_buffer[(MAX_BMP_LINE_LENGTH * 3) + 4u];



Public Boolean LoadBitmap(const char * path, U16 * dest)
{
    /* Lets try implementing a general bitmap loader that loads a 24-bit BMP file into a provided memory buffer. */
    FIL f_obj;
    FRESULT file_res;
    Boolean res = FALSE;
    BMPHeader BitmapHeader;
    UINT bytes_read;
    U16 * dest_ptr = dest;
    U32 x, y;

    U16 line_stride;
    U16 line_px_data_len;

    file_res = f_open(&f_obj, path, FA_READ);

    if (file_res == FR_OK)
    {
        /* Lets first try reading out the bitmap header. */
        file_res = f_read(&f_obj, &BitmapHeader, sizeof(BMPHeader), &bytes_read);
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
            if (BitmapHeader.width_px <= MAX_BMP_LINE_LENGTH)
            {
                /* Take padding into account... */
                line_px_data_len = BitmapHeader.width_px * 3u;
                line_stride = (line_px_data_len + 3u) & ~0x03;

                for(y = 0u; y < BitmapHeader.height_px; y++)
                {
                    file_res = f_read(&f_obj, priv_bmp_line_buffer, line_stride, &bytes_read);

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
                MSPrintf(EUSCI_A0_BASE, "BMP file too large for buffer %s\n", path);
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

    return res;
}



