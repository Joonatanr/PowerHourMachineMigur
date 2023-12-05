/*
 * SdCardHandler.c
 *
 *  Created on: 5. dets 2023
 *      Author: JRE
 */

#include "SdCardHandler.h"
#include "fatfs/ff.h"
#include "MSPIO.h"

/* Lets try with EUSCI_B2 alternative outputs:
 *
 * MISO = 3.7
 * MOSI = 3.6
 * SCK  = 3.5
 * CS   = 5.1
 * */


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

        if (strncmp(FI.fname, "METSA", 5) == 0)
        {
            MSPrintf(EUSCI_A0_BASE, "Leidsin!");
            /* Lets try reading the contents of the file. */
        }



    }while(FI.fname[0]);

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

    /* TODO : Replace test sequence with something reasonable. */
}



