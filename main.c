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
#include "BitmapHandler.h"
#include "LcdWriter.h"

/**
 * main.c
 */

Private U8 priv_1sec_flag = 0u;
Private U16 priv_msec_counter = 0u;

Private void display_test(void);

void main(void)
{
    hwmain_init();

	/* Test sequence for TFT display. */
	display_init();

    /* Initialize the SD Card reader*/
    SdCardHandlerInit();

    /* Load a bitmap and display it on the screen. */
    display_test();

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


Private void display_test(void)
{
    U16 * disp_buffer_ptr = display_get_frame_buffer();

    /*
    if (LoadBitmap("/color_test.bmp", disp_buffer_ptr) == TRUE)
    {
        display_flushBuffer(0u, 0u, 162u, 132u);
    }
    */
    timer_delay_msec(1000u);

    if (LoadBitmap("/Lena.bmp", disp_buffer_ptr) == TRUE)
    {
        display_flushBuffer(0u, 0u, 162u, 132u);
    }

    /*
    timer_delay_msec(3000u);
    if (LoadBitmap("/laudur.bmp", disp_buffer_ptr) == TRUE)
    {
        display_flushBuffer(10u, 20u, 30u, 101u);
    }
    */
    timer_delay_msec(1000u);
    //LcdWriter_drawChar('A', 10, 10, FONT_COURIER_14);
    //LcdWriter_drawChar('B', 20, 10, FONT_COURIER_14);
    //LcdWriter_drawChar('C', 30, 10, FONT_COURIER_14);
    LcdWriter_drawString("Hello World!", 10, 10, FONT_COURIER_14);
}
