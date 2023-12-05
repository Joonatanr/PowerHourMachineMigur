/*
 * spi.c
 *
 *  Created on: Aug 13, 2017
 *      Author: Joonatan
 */

#include <driverlib.h>
#include "spidrv.h"
#include "ports.h"

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig spiMasterConfigLCD =
{
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        24000000,                                  // SMCLK = DCO = 24MHZ
        8000000,                                   // SPICLK = 3MHz
        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,    // Phase
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, // High polarity
        EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
};


/* SPI Configuration Parameter. These are the configuration parameters to
 * make the eUSCI B SPI module to operate with a 500KHz clock.*/
const eUSCI_SPI_MasterConfig spiMasterConfigSdCard =
{
     EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
     24000000,
     500000,
     EUSCI_B_SPI_MSB_FIRST,
     EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
     EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
     EUSCI_B_SPI_3PIN
};

Public void spidrv_init(void)
{
    /* Set the main display to work on EUSCI_B0 */
    /* Selecting P1.5 and P1.6 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 , GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfigLCD);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);

    /* Set the SD card to work on EUSCI_B2 */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_SPI_initMaster(EUSCI_B2_BASE, &spiMasterConfigSdCard);
    MAP_SPI_enableModule(EUSCI_B2_BASE);
}

/* This function is used to transmit the frame buffer, so we need to be pretty efficient... */
Public void spidrv_transmitU16(U16 * data, U32 data_len)
{
    U8 * data_ptr = (U8*)data;
    U32 buf_len = data_len *2u;

    SPI_disableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);

    while (buf_len > 0)
    {
        while(BITBAND_PERI(EUSCI_B_CMSIS(EUSCI_B0_BASE)->STATW, EUSCI_B_STATW_BBUSY_OFS));
        __delay_cycles(10);
        EUSCI_B_CMSIS(EUSCI_B0_BASE)->TXBUF = *data_ptr++;
        buf_len--;
    }

    SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
}


Public void SPI_Write_Byte(uint32_t SPI, U8 byte)
{
    //Transmit data to slave.
    //while(BITBAND_PERI(EUSCI_B_CMSIS(EUSCI_B0_BASE)->STATW, EUSCI_B_STATW_BBUSY_OFS));
    MAP_SPI_transmitData(SPI, byte);
}


void SPI_Write(uint32_t SPI, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    for(i = 0; i < Size; i++)
    {
        MAP_SPI_transmitData(SPI, Data[i]);
    }
}

void SPI_Read(uint32_t SPI, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    for(i = 0; i < Size; i++)
    {
        Data[i] = MAP_SPI_receiveData(SPI);
    }
}

