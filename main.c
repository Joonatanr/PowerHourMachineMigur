#include "msp.h"
#include "typedefs.h"
#include "hwmain.h"
#include "ports.h"
#include "driverlib.h"
#include "timer.h"
#include "display.h"
#include "MSPIO.h"
#include "SdCardHandler.h"
#include "fatfs/diskio.h"

/**
 * main.c
 */

Private U8 priv_1sec_flag = 0u;
Private U16 priv_msec_counter = 0u;

void main(void)
{
    hwmain_init();

    /* TODO : Remove */
    MSPrintf(EUSCI_A0_BASE, "Hello World!\n");

    /* Initialize the SD Card reader*/
    SdCardHandlerInit();

	/* Test sequence for TFT display. */
	display_init();

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


Public void timer_1msec_callback(void)
{
    ports_set_led_one(ports_isBtnOne());
    ports_set_led_two_green(ports_isBtnTwo());
    disk_timerproc();

    priv_msec_counter++;
    if (priv_msec_counter >= 1000u)
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
    if (toggle)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
    }
}
