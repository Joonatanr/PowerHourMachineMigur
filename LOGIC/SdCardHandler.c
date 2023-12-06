/*
 * SdCardHandler.c
 *
 *  Created on: 5. dets 2023
 *      Author: JRE
 */

#include "SdCardHandler.h"
#include "display.h"
#include "fatfs/ff.h"
#include "MSPIO.h"

/* Lets try with EUSCI_B2 alternative outputs:
 *
 * MISO = 3.7
 * MOSI = 3.6
 * SCK  = 3.5
 * CS   = 5.1
 * */

/* Lets try with EUSCI_B1 alternative outputs, after the EUSCI_B2 pin burned down... :
 *
 * MISO = 6.5
 * MOSI = 6.4
 * SCK  = 6.3
 * CS   = 5.1
 * */

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

U8 myBuf[(162 * 3) + 4u];

FATFS FS;
DIR DI;
FILINFO FI;

Public void SdCardHandlerInit(void)
{
    FRESULT r;

    /*First we should mount the SD Card into the Fatfs file system*/
    r = f_mount(&FS, "0", 1);
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK)
    {
        MSPrintf(EUSCI_A0_BASE, "Error mounting SD Card, check your connections\r\n");
        while(1);
    }

    /*Let's try to open the root directory on the SD Card*/
    r = f_opendir(&DI, "/");
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK)
    {
        MSPrintf(EUSCI_A0_BASE, "Could not open root directory\r\n");
        while(1);
    }

    /*Read everything inside the root directory*/
    do
    {
        /*Read a directory/file*/
        r = f_readdir(&DI, &FI);
        /*Check for errors. Trap MSP432 if there is an error*/
        if(r != FR_OK)
        {
           MSPrintf(EUSCI_A0_BASE, "Error reading file/directory\r\n");
           while(1);
        }

        /*Print the file to the serial terminal*/
        MSPrintf(EUSCI_A0_BASE, "%c%c%c%c%c %s\r\n",
               (FI.fattrib & AM_DIR) ? 'D' : '-',
               (FI.fattrib & AM_RDO) ? 'R' : '-',
               (FI.fattrib & AM_HID) ? 'H' : '-',
               (FI.fattrib & AM_SYS) ? 'S' : '-',
               (FI.fattrib & AM_ARC) ? 'A' : '-',
               ((char*)FI.fname));

        if (strncmp(FI.fname, "Lena", 4) == 0)
        {
            MSPrintf(EUSCI_A0_BASE, "Leidsin!");
            /* Lets try reading the contents of the file. */
        }



    }while(FI.fname[0]);

#if 0
    { /* Try to open and read one of the files. */
        FIL f_obj;
        FRESULT res;
        char myBuf[65];
        UINT bytes_read = 64;

        res = f_open(&f_obj, "/Metsavennad.txt", FA_READ);

        if (res == FR_OK)
        {
            MSPrintf(EUSCI_A0_BASE, "Begin\n");
            while(bytes_read == 64u)
            {
                res = f_read(&f_obj, myBuf, 64, &bytes_read);
                myBuf[bytes_read] = 0;

                MSPrintf(EUSCI_A0_BASE, "%s", myBuf);
            }
            MSPrintf(EUSCI_A0_BASE, "End\n");
        }
    }
#endif
    { /* Try to open and read one of the files. */
        FIL f_obj;
        FRESULT res;

        UINT bytes_read = 64;
        U16 x, y;
        U16 * disp_buffer_ptr;

        res = f_open(&f_obj, "/Lena.bmp", FA_READ);
        //res = f_open(&f_obj, "/color_test.bmp", FA_READ);

        if (res == FR_OK)
        {
            MSPrintf(EUSCI_A0_BASE, "Begin\n");

            /* Lets try reading out the bitmap header. */
            BMPHeader LenaHeader;

            res = f_read(&f_obj, &LenaHeader, sizeof(BMPHeader), &bytes_read);
            MSPrintf(EUSCI_A0_BASE, "Size of header is %i\n", sizeof(BMPHeader));

            if (bytes_read == sizeof(BMPHeader))
            {
                MSPrintf(EUSCI_A0_BASE, "Type : %i, Offset : %i width_px : %i, height_px : %i\n",
                         LenaHeader.type,
                         LenaHeader.offset,
                         LenaHeader.width_px,
                         LenaHeader.height_px);

                /* Now lets try and read RGB data... */
                if (LenaHeader.width_px <= 162u)
                {
                    disp_buffer_ptr = display_get_frame_buffer();

                    for(y = 0u; y < LenaHeader.height_px; y++)
                    {
                        /* TODO : Padding is currently hardcoded, this needs to be set up properly .*/
                        res = f_read(&f_obj, myBuf, (LenaHeader.width_px * 3u) + 2u, &bytes_read);

                        if (res == FR_OK)
                        {
                            MSPrintf(EUSCI_A0_BASE, "Read line %i\n", y);
                        }
                        else
                        {
                            MSPrintf(EUSCI_A0_BASE, "Failed to read line %i\n", y);
                        }

                        for (x = 0u; x < (bytes_read - 3u); x+=3u )
                        {
                            /* TODO : Obviously we need to optimise this. Initial implementation just for proof of concept can be like this. */
                            //*disp_buffer_ptr++ = CONVERT_888RGB_TO_565BGR(myBuf[x], myBuf[x+1], myBuf[x+2]);
                            *disp_buffer_ptr++ = CONVERT_888RGB_TO_565BGR(myBuf[x+2], myBuf[x+1], myBuf[x]);
                        }
                    }

                    display_flushBuffer();
                }
            }
            else
            {
                MSPrintf(EUSCI_A0_BASE, "Something went wrong:(\n");
            }

            MSPrintf(EUSCI_A0_BASE, "End\n");
        }
        else
        {
            MSPrintf(EUSCI_A0_BASE, "Could not open Lena.bmp\n");
        }
    }

    /* TODO : Replace test sequence with something reasonable. */
}



