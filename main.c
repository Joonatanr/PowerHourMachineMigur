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
#include "backlight.h"
#include "Scheduler.h"

/**
 * main.c
 */

Private U8 priv_50msec_flag = 0u;
Private U16 priv_msec_counter = 0u;

Private void display_test(void);
Private void timer_50msec_callback(void);
Private void showStartScreen(void);

void main(void)
{
    hwmain_init();

	/* Test sequence for TFT display. */
	display_init();

    //Set backlight to 60 percent.
    backlight_set_level(60);

	timer_delay_msec(1000u);
    /* Initialize the SD Card reader*/
    SdCardHandlerInit();

    /* Initialize the main scheduler. */
    Scheduler_initTasks();

    timer_delay_msec(100);

    //Start all scheduler task
    Scheduler_StartTasks();
    timer_delay_msec(100);

    //We show the initial start screen for a while.
    showStartScreen();
    timer_delay_msec(4000);

    /* Sleeping when not in use */
	for(;;)
	{
	    /* Trap CPU... */
	    if (priv_50msec_flag == 1u)
	    {
	        priv_50msec_flag = 0u;
	        timer_50msec_callback();
	    }
	}
}


Public void timer_1msec_callback(void)
{
    disk_timerproc();

    priv_msec_counter++;
    if (priv_msec_counter >= 50u)
    {
        priv_msec_counter = 0u;
        priv_50msec_flag = 1u;
    }
}

Private void timer_50msec_callback(void)
{
    Scheduler_cyclic();
}

Private void showStartScreen(void)
{
    /* TODO : Placeholder. */
    /* Load a bitmap and display it on the screen. */
    display_test();
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
    //LcdWriter_drawString("First\nSecond", 10, 40, FONT_COURIER_14);
    //LcdWriter_drawCharColored('A', 5, 70, FONT_COURIER_14, 0xF000, 0xFFFFu);
    LcdWriter_drawColoredString("First\nSecond", 10, 40, FONT_COURIER_14, COLOR_RED, COLOR_YELLOW);
}
