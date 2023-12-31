/*
 * hwmain.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#include "hwmain.h"
#include "driverlib.h"
#include "clocks.h"
#include "ports.h"
#include "timer.h"
#include "pot.h"
#include "buttons.h"
#include "spidrv.h"
#include "UART_Driver.h"
#include "backlight.h"
#include "systimer.h"

//#define MAIN_CLK_TEST

eUSCI_UART_Config UART0Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK, /* 24MHz */
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};


Public void hwmain_init(void)
{

    /* Stop WDT  */
    MAP_WDT_A_holdTimer();
    Interrupt_disableMaster();

    clocks_init();

    systimer_init();

    ports_init();

    ports_setOutput(0u, PORTS_LED_ONE);
    ports_setOutput(0u, PORTS_LED_TWO_BLUE);
    ports_setOutput(0u, PORTS_LED_TWO_GREEN);
    ports_setOutput(0u, PORTS_LED_TWO_RED);

    backlight_init();

    pot_init();

    timer_init();

    spidrv_init();

    UART_Init(EUSCI_A0_BASE, UART0Config);

#ifdef MAIN_CLK_TEST
    setBL(0);
    __delay_cycles(250000);
    setBL(1);
#endif

    //Enable interrupts in general.
    Interrupt_enableMaster();
}
