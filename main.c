#include "msp.h"
#include "typedefs.h"
#include "hwmain.h"
#include "ports.h"
#include "driverlib.h"
#include "timer.h"

/**
 * main.c
 */

Private U8 priv_1sec_flag = 0u;
Private U16 priv_msec_counter = 0u;

void main(void)
{
	hwmain_init();

    /* Sleeping when not in use */

	for(;;)
	{
	    /* Trap CPU... */
	    if (priv_1sec_flag == 1u)
	    {
	        priv_1sec_flag = 0u;
	        timer_1sec_callback();
	    }
	}
}


Public void timer_10msec_callback(void)
{
    ports_set_led_one(ports_isBtnOne());
    ports_set_led_two_green(ports_isBtnTwo());

    priv_msec_counter++;
    if (priv_msec_counter >= 100u)
    {
        priv_msec_counter = 0u;
        priv_1sec_flag = 1u;
    }
}

Public void timer_1sec_callback(void)
{
    static U8 toggle = 0u;
    toggle = !toggle;
    ports_set_led_two_blue(toggle);
}
