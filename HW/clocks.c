/*
 * clocks.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#include "clocks.h"
#include "driverlib.h"

Public void clocks_init(void)
{
    WDT_A_holdTimer();

    /* This should be set in case of higher frequency. */
    //MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1, also required for 48MHz */
    //MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    //MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    //Lets configure the DCO to 12MHz
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* Initializing the clock source as follows:
     *      MCLK = MODOSC/2 = 12MHz
     *      ACLK = REFO/2 = 16kHz   --- TODO Not used, should remove.
     *      HSMCLK = DCO/2 = 6Mhz
     *      SMCLK = DCO =  12MHz
     *      BCLK  = REFO = 32kHz    --- TODO Not used, should remove.
     */
    MAP_CS_initClockSignal(CS_MCLK,     CS_MODOSC_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_ACLK,     CS_REFOCLK_SELECT,  CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_HSMCLK,   CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_SMCLK,    CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_BCLK,     CS_REFOCLK_SELECT,  CS_CLOCK_DIVIDER_1);
}
