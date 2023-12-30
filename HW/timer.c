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
#include <stdlib.h>
#include "MSPIO.h"

//#define RANDOM_SEED_DEBUG

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


Public U16 generate_random_number_rng(U16 min, U16 max)
{
    U16 range = max - min;
    U16 res = 0u;

    if (range > 0u)
    {
        res = generate_random_number(range);
        res += min;
    }

    return res;
}

Private U16 priv_seed = 0u;

Public void regenerate_random_number_seed(void)
{
    priv_seed = TA0R ^ TA1R;
    /* We initialize the pseudo random number generator. */
    srand(priv_seed);
}


Public U16 generate_random_number(U16 max)
{
    U16 res;
    if (priv_seed == 0u)
    {
        regenerate_random_number_seed();
    }

    /* Max might actually be legitimately 0 in some calculations. */
    if (max == 0u)
    {
        res = 0u;
    }
    else
    {
        res =  rand() % (max + 1u);
    }

#ifdef RANDOM_SEED_DEBUG
        MSPrintf(EUSCI_A0_BASE, "Random seed : %d, Random Number : %d, (%d max)\n", priv_seed, res, max);
#endif

    return res;
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

    ports_setOutput(isButton(BLACK_BUTTON), PORTS_LED_ONE);
    ports_setOutput(isButton(GREEN_BUTTON), PORTS_LED_TWO_GREEN);
    ports_setOutput(isButton(BLUE_BUTTON), PORTS_LED_TWO_BLUE);
    ports_setOutput(isButton(RED_BUTTON), PORTS_LED_TWO_RED);
}

