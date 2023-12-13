/*
 * timer.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#include "timer.h"
#include "driverlib.h"
#include "ports.h"
#include "pot.h"
#include "buttons.h"

/*****************************************************************************************************
 *
 * Function Prototypes
 *
 *****************************************************************************************************/

Private void TA0_0_IRQHandler(void);
Private void TA1_0_IRQHandler(void);

/*****************************************************************************************************
 *
 * Private variables.
 *
 *****************************************************************************************************/

Public volatile U32 priv_delay_counter = 0u;

//Hi priority timer runs at 1msec interval (might need to be faster)
Private const Timer_A_UpModeConfig hi_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8,
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     .timerPeriod = 3000u /* 24MHz / 8 / 3000 = 1msec */
};

//Lo priority timer runs at 10msec interval
Private const Timer_A_UpModeConfig lo_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8,
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     .timerPeriod = 30000u /* 24MHz / 8 / 30000 = 10msec */
};


Public void timer_init(void)
{
    //Set up timer for high priority interrupts.
    Timer_A_configureUpMode(TIMER_A0_BASE, &hi_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A0_BASE, TIMER_A_CCR0_INTERRUPT, TA0_0_IRQHandler);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    Timer_A_configureUpMode(TIMER_A1_BASE, &lo_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A1_BASE, TIMER_A_CCR0_INTERRUPT, TA1_0_IRQHandler);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    //Enable this interrupt in NVIC.
    Interrupt_setPriority(INT_TA0_0, 4u); /* TODO : 4U has been chosen quite randomly... */
    Interrupt_enableInterrupt(INT_TA0_0);

    Interrupt_setPriority(INT_TA1_0, 5u); /* TODO : 4U has been chosen quite randomly... */
    Interrupt_enableInterrupt(INT_TA1_0);
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


//Hi priority interrupt handler.
Private void TA1_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    pot_cyclic_10ms();

    buttons_cyclic10msec();

    //ports_set_led_one(ports_isBtnOne());
    //ports_set_led_two_green(ports_isBtnTwo());
    ports_set_led_one(isButton(BLACK_BUTTON));
    ports_set_led_two_green(isButton(GREEN_BUTTON));
    ports_set_led_two_blue(isButton(BLUE_BUTTON));
    ports_set_led_two_red(isButton(RED_BUTTON));

}

