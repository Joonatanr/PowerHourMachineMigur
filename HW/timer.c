/*
 * timer.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#include "timer.h"
#include "driverlib.h"
#include "ports.h"

/*****************************************************************************************************
 *
 * Function Prototypes
 *
 *****************************************************************************************************/

Private void TA0_0_IRQHandler(void);

/*****************************************************************************************************
 *
 * Private variables.
 *
 *****************************************************************************************************/

Public volatile U32 priv_delay_counter = 0u;

//Hi priority timer runs at 10msec interval (might need to be faster)
Private const Timer_A_UpModeConfig hi_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8,
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     .timerPeriod = 3000u /* 24MHz / 8 / 3000 = 1msec */
};


Public void timer_init(void)
{
    //Set up timer for high priority interrupts.
    Timer_A_configureUpMode(TIMER_A0_BASE, &hi_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A0_BASE, TIMER_A_CCR0_INTERRUPT, TA0_0_IRQHandler);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    //Enable this interrupt in NVIC.
    Interrupt_setPriority(INT_TA0_0, 4u); /* TODO : 4U has been chosen quite randomly... */
    Interrupt_enableInterrupt(INT_TA0_0);
}


#pragma FUNCTION_OPTIONS(timer_delay_msec, "--opt_level=off")
Public void timer_delay_msec(U32 msec)
{
    priv_delay_counter = msec;
    while(priv_delay_counter > 0u);
}


/* This should be fired every 1 msec */
//Hi priority interrupt handler.
Private void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    if (priv_delay_counter > 0u)
    {
        priv_delay_counter--;
    }

    timer_1msec_callback();
}

