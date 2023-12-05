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
const eUSCI_SPI_MasterConfig spiMasterConfig =
{
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        24000000,                                  // SMCLK = DCO = 24MHZ
        8000000,                                   // SPICLK = 3MHz
        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,    // Phase
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, // High polarity
        EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
};


Public void spidrv_init(void)
{
    /* Selecting P1.5 and P1.6 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 , GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);
}

Public void spidrv_transmit(U8 * data, U16 data_len)
{
    U8 * data_ptr = data;

    while (data_len > 0)
    {
        spi_transmit_byte(*data_ptr);
        data_ptr++;
        data_len--;
    }
}


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


Public void spi_transmit_byte(U8 byte)
{
    //Transmit data to slave.
    while(BITBAND_PERI(EUSCI_B_CMSIS(EUSCI_B0_BASE)->STATW, EUSCI_B_STATW_BBUSY_OFS));
    SPI_transmitData(EUSCI_B0_BASE, byte);
    __delay_cycles(50); /* TODO : Check if this is actually necessary ! */
}

