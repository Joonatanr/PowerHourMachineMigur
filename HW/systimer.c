/*
 * systimer.c
 *
 *  Created on: 14 Dec 2023
 *      Author: Joonatan
 */

#include "systimer.h"
#include "driverlib.h"


Public void systimer_init(void)
{
    /* Initializing Timer32 in module in 32-bit free-run mode (with max value
     * of 0xFFFFFFFF
     */
    MAP_Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT,
            TIMER32_FREE_RUN_MODE);

    /* Soo.. we will be getting 187500 ticks per second, or 187.5 ticks per millisecond */

    /* Starting the timer */
    MAP_Timer32_startTimer(TIMER32_BASE, true);
}


Public U32 systimer_getTimestamp(void)
{
    U32 curValue = MAP_Timer32_getValue(TIMER32_BASE);
    U32 res;

    /* We translate this roughly to milliseconds. */
    res = (0xFFFFFFFF - curValue) / 187u;
    return res;
}


Public U32 systimer_getPeriod(U32 start_time)
{
    return systimer_getTimestamp() - start_time;
}
