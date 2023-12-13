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



FATFS FS;
DIR DI;
FILINFO FI;

Public void SdCardHandlerInit(void)
{
    FRESULT r;
    U8 ix;

    for(ix = 0u; ix < 3u; ix++)
    {
        /*First we should mount the SD Card into the Fatfs file system*/
        r = f_mount(&FS, "0", 1);
        if (r == FR_OK)
        {
            break;
        }
    }

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
}
