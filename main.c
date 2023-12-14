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

#include "Menus/Menu.h"

/**
 * main.c
 */

//#define DISPLAY_TEST

Private U8 priv_50msec_flag = 0u;
Private U16 priv_msec_counter = 0u;
Private const char priv_version_string[] = "Machine 4.0";

#ifdef DISPLAY_TEST
Private void display_test(void);
#endif

Private void timer_50msec_callback(void);
Private void showStartScreen(void);
Private void showDedicationText(void);
Private void startGameHandler(void);
Private void startSnakeGame(void);

/* TODO : These neeed to be accessed elsewhere as well.*/
U16 disp_background_color = COLOR_BLACK;
U16 disp_text_color = COLOR_GREEN;
U16 disp_highlight_color = COLOR_MAGENTA;
U16 disp_ph_prompt_text_color = COLOR_RED;


/* Settings Menu Items */
Private const MenuItem SettingsMenuItemArray[] =
{
   { .text = "Brightness",    .Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &BRIGHTNESS_BARGRAPH        },
   { .text = "Snake speed",   .Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &SNAKE_SPEED_BARGRAPH      },
   { .text = "Task frequency",.Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &TASK_FREQUENCY_BARGRAPH    }
};

Private SelectionMenu SettingsMenu =
{
     .items = SettingsMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(SettingsMenuItemArray),
     .selected_item = 0u
};


/** Start Menu Items.*/
Private const MenuItem StartMenuItemArray[] =
{
   { .text = "Start Game",  .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   startGameHandler        },
   { .text = "Play Snake",  .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   startSnakeGame          },
   { .text = "Settings",    .Action = MENU_ACTION_SUBMENU     , .ActionArg.subMenu_ptr  =   &SettingsMenu           },
   { .text = "About",       .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   &showDedicationText     },
};

Private SelectionMenu StartMenu =
{
     .items = StartMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(StartMenuItemArray),
     .selected_item = 0u,
};

/** End of Start Menu Items. */


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

    /* We pass control over to the menu handler. */
    menu_enterMenu(&StartMenu);

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

Public void returnToMain(void)
{
    Scheduler_StopActiveApplication();
    menu_enterMenu(&StartMenu);
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
#ifdef DISPLAY_TEST
    display_test();
    timer_delay_msec(6000u);
#endif

    display_fill(disp_background_color);

    LcdWriter_drawStringCenter("Power Hour", (DISPLAY_WIDTH / 2u) + 4u, 30u, FONT_COURIER_16_BOLD, disp_text_color, disp_background_color);
    LcdWriter_drawStringCenter(priv_version_string, (DISPLAY_WIDTH / 2u) + 4u, 50u, FONT_COURIER_16_BOLD, disp_text_color, disp_background_color);
    LcdWriter_drawStringCenter("Migur Edition", (DISPLAY_WIDTH / 2u) + 4u, 70u, FONT_COURIER_14, disp_text_color, disp_background_color);
}

#ifdef DISPLAY_TEST
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
#endif


/** Placeholders **/
Private U16 dummy_task_frequency = 3u;
Private U16 dummy_snake_speed = 4u;

Public void clockDisplay_setTaskFrequency(U16 value)
{
    dummy_task_frequency = value;
}

Public U16 clockDisplay_getTaskFrequency(void)
{
    return dummy_task_frequency;
}

Public void snake_setSpeed(U16 value)
{
    dummy_snake_speed = value;
}

/* Starts the main Power Hour game. */
Private void startGameHandler(void)
{
    Scheduler_SetActiveApplication(APPLICATION_POWER_HOUR);
}

/* Starts the snake game. */
Private void startSnakeGame(void)
{
    //Scheduler_SetActiveApplication(APPLICATION_SNAKE);
}

/* TODO */
Private void showDedicationText(void)
{
    /* Turns out we can't do this without making this into a dummy application for some reason. */
    Scheduler_SetActiveApplication(APPLICATION_DEDICATION);
}
