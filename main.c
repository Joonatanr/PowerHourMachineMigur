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
#include "systimer.h"
#include "pot.h"
#include "Configuration.h"
#include "misc.h"

#include "Menus/Menu.h"
#include "PowerHourMain.h"
#include "SpecialTasks.h"

/**
 * main.c
 */

//#define DISPLAY_TEST
//#define POT_TEST
//#define STR_TEST
//#define STR_LENGTH_TEST

Private U8 priv_50msec_flag = 0u;
Private U16 priv_msec_counter = 0u;
Private const char priv_version_string[] = "Machine 4.2";

#ifdef STR_TEST
Private void ReplaceStringEscapeChars(const char * str, char * dest);
#endif

#ifdef DISPLAY_TEST
Private void display_test(void);
#endif

#ifdef POT_TEST
Private void pot_test(void);
#endif

#ifdef STR_TEST
Private void str_test(void);
#endif

Private void timer_50msec_callback(void);
Private void showStartScreen(void);
Private void showDedicationText(void);
Private void startGameHandler(void);


#ifdef STR_TEST
Private char priv_test_buf[32];
#endif

/* Settings Menu Items */
Private const MenuItem ColorMenuItemArray[] =
{
     { .text = "Scheme 1",    .Action = MENU_ACTION_SELECT  , .ActionArg.function_set_u16_ptr = setSelectedColorSchemeIndex  },
     { .text = "Scheme 2",    .Action = MENU_ACTION_SELECT  , .ActionArg.function_set_u16_ptr = setSelectedColorSchemeIndex  },
     { .text = "Scheme 3",    .Action = MENU_ACTION_SELECT  , .ActionArg.function_set_u16_ptr = setSelectedColorSchemeIndex  },
     { .text = "Scheme 4",    .Action = MENU_ACTION_SELECT  , .ActionArg.function_set_u16_ptr = setSelectedColorSchemeIndex  },
};

Private const MenuItem BuzzerMenuItemArray[] =
{
     { .text = "Disabled",    .Action = MENU_ACTION_SELECT  , .ActionArg.function_set_u16_ptr = setBuzzerEnable  },
     { .text = "Enabled",   .Action = MENU_ACTION_SELECT    , .ActionArg.function_set_u16_ptr = setBuzzerEnable  },
};

Private SelectionMenu ColorMenu =
{
     .items = ColorMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(ColorMenuItemArray),
     .selected_item = 0u,
     .initial_select_func = getSelectedColorScheme,
     .isCheckedMenu = TRUE,
     .isTransparentMenu = FALSE,
};

Private SelectionMenu BuzzerMenu =
{
     .items = BuzzerMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(BuzzerMenuItemArray),
     .selected_item = 0u,
     .initial_select_func = isBuzzerEnabled,
     .isCheckedMenu = TRUE,
     .isTransparentMenu = FALSE,
};

Private const MenuItem SettingsMenuItemArray[] =
{
   { .text = "Brightness",    .Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &BRIGHTNESS_BARGRAPH        },
   { .text = "Task frequency",.Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &TASK_FREQUENCY_BARGRAPH    },
   { .text = "Color scheme"  ,.Action = MENU_ACTION_SUBMENU , .ActionArg.subMenu_ptr =  &ColorMenu                  },
   { .text = "Buzzer"        ,.Action = MENU_ACTION_SUBMENU , .ActionArg.subMenu_ptr =  &BuzzerMenu                 }
};

Private SelectionMenu SettingsMenu =
{
     .items = SettingsMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(SettingsMenuItemArray),
     .selected_item = 0u,
     .initial_select_func = NULL,
     .isCheckedMenu = FALSE,
     .isTransparentMenu = FALSE,
};


/** Start Menu Items.*/
Private const MenuItem StartMenuItemArray[] =
{
   { .text = "Start Game",  .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   startGameHandler        },
   { .text = "Settings",    .Action = MENU_ACTION_SUBMENU     , .ActionArg.subMenu_ptr  =   &SettingsMenu           },
   { .text = "About",       .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   &showDedicationText     },
};

Private SelectionMenu StartMenu =
{
     .items = StartMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(StartMenuItemArray),
     .selected_item = 0u,
     .initial_select_func = NULL,
     .isCheckedMenu = FALSE,
     .isTransparentMenu = TRUE, /* Just for experimenting, probably will not really use this. */
};

/** End of Start Menu Items. */


void main(void)
{
    /* Set up all low level peripherals. */
    hwmain_init();

	/* Setup the TFT display. */
	display_init();

	timer_delay_msec(1000u);
    /* Initialize the SD Card reader*/
    SdCardHandlerInit();

    //Set up the configuration
    configuration_start();
    ColorScheme_start();
    backlight_set_level(configuration_getItem(CONFIG_ITEM_BRIGHTNESS));

    /* Initialize the main scheduler. */
    Scheduler_initTasks();

    timer_delay_msec(100);

    //Start all scheduler task
    Scheduler_StartTasks();
    timer_delay_msec(100);



#ifdef POT_TEST
    timer_delay_msec(200u);
    pot_test();
#endif

#ifdef STR_TEST
    str_test();
    timer_delay_msec(10000);
#endif

#ifdef STR_LENGTH_TEST
    SpecialTask_StringLengthSanityTest();
    timer_delay_msec(5000);
#endif

    //We show the initial start screen for a while.
    showStartScreen();

    /* We pass control over to the menu handler. */
    menu_enterMenu(&StartMenu, TRUE);

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
    menu_enterMenu(&StartMenu, FALSE);
}

Public void timer_1msec_callback(void)
{
    disk_timerproc();

    priv_msec_counter++;
    if (priv_msec_counter >= 50u)
    {
        priv_msec_counter = 0u;
        priv_50msec_flag = 1u;
        Scheduler_50msec_timer();
    }
}

Private void timer_50msec_callback(void)
{
    Scheduler_cyclic();
}

Private void showStartScreen(void)
{
    /* Load a bitmap and display it on the screen. */
#ifdef DISPLAY_TEST
    display_test();
    timer_delay_msec(6000u);
#endif

    display_fill(disp_background_color);

    BitmapHandler_LoadBitmap("/Migurid/Migurid1.bmp", display_get_frame_buffer());
    display_flushBuffer(0u, 0u, 162u, 132u);
    timer_delay_msec(3000u);

    LcdWriter_drawStringCenter("Power Hour", (DISPLAY_WIDTH / 2u) + 4u, 40u, FONT_ARIAL_16_BOLD, disp_text_color, disp_background_color);
    LcdWriter_drawStringCenter(priv_version_string, (DISPLAY_WIDTH / 2u) + 4u, 60u, FONT_ARIAL_16_BOLD, disp_text_color, disp_background_color);
    LcdWriter_drawStringCenter("Migur Edition", (DISPLAY_WIDTH / 2u) + 4u, 80u, FONT_ARIAL_14_BOLD, disp_text_color, disp_background_color);

    timer_delay_msec(3000);
    display_flushBuffer(0u, 0u, 162u, 132u);
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

#ifdef POT_TEST
Private void pot_test(void)
{
    char str[64u];

    sprintf(str, "Pot 1: %d", pot_getSelectedRange(POTENTIOMETER_MIGUR2));
    LcdWriter_drawString(str, 5, 10, FONT_SMALL_FONT_10);
    sprintf(str, "Pot 2: %d", pot_getSelectedRange(POTENTIOMETER_NUDE_LEVEL));
    LcdWriter_drawString(str, 5, 30, FONT_SMALL_FONT_10);
    sprintf(str, "Pot 3: %d", pot_getSelectedRange(POTENTIOMETER_SEXY_LEVEL));
    LcdWriter_drawString(str, 5, 50, FONT_SMALL_FONT_10);
    sprintf(str, "Pot 4: %d", pot_getSelectedRange(POTENTIOMETER_MIGUR1));
    LcdWriter_drawString(str, 5, 70, FONT_SMALL_FONT_10);
    sprintf(str, "Pot 5: %d", pot_getSelectedRange(POTENTIOMETER_GIRLS));
    LcdWriter_drawString(str, 5, 90, FONT_SMALL_FONT_10);
    sprintf(str, "Pot 6: %d", pot_getSelectedRange(POTENTIOMETER_GUYS));
    LcdWriter_drawString(str, 5, 110, FONT_SMALL_FONT_10);

    while(1u)
    {
       timer_delay_msec(1000u);

       sprintf(str, "Pot 1: %d", pot_getSelectedRange(POTENTIOMETER_MIGUR2));
       LcdWriter_drawString(str, 5, 10, FONT_SMALL_FONT_10);
       sprintf(str, "Pot 2: %d", pot_getSelectedRange(POTENTIOMETER_NUDE_LEVEL));
       LcdWriter_drawString(str, 5, 30, FONT_SMALL_FONT_10);
       sprintf(str, "Pot 3: %d", pot_getSelectedRange(POTENTIOMETER_SEXY_LEVEL));
       LcdWriter_drawString(str, 5, 50, FONT_SMALL_FONT_10);
       sprintf(str, "Pot 4: %d", pot_getSelectedRange(POTENTIOMETER_MIGUR1));
       LcdWriter_drawString(str, 5, 70, FONT_SMALL_FONT_10);
       sprintf(str, "Pot 5: %d", pot_getSelectedRange(POTENTIOMETER_GIRLS));
       LcdWriter_drawString(str, 5, 90, FONT_SMALL_FONT_10);
       sprintf(str, "Pot 6: %d", pot_getSelectedRange(POTENTIOMETER_GUYS));
       LcdWriter_drawString(str, 5, 110, FONT_SMALL_FONT_10);
    }
}
#endif


/* Starts the main Power Hour game. */
Private void startGameHandler(void)
{
    Scheduler_SetActiveApplication(APPLICATION_POWER_HOUR);
}


Private void showDedicationText(void)
{
    /* Turns out we can't do this without making this into a dummy application for some reason. */
    Scheduler_SetActiveApplication(APPLICATION_DEDICATION);
}


#ifdef STR_TEST

Private void str_test(void)
{
    strcpy(priv_test_buf, "##############################");
    ReplaceStringEscapeChars("{$} must take", priv_test_buf);
    //ReplaceStringEscapeChars("Yolotron", priv_test_buf);
    display_drawStringCenter(priv_test_buf, DISPLAY_CENTER, 20u, FONT_COURIER_14, FALSE);
}

Private void ReplaceStringEscapeChars(const char * str, char * dest)
{
    const char * ps = str;

    if (ps == NULL)
    {
        return;
    }

    while(*ps)
    {
        if(*ps == '{')
        {
            if (*(ps+1) == '$' && *(ps+2) == '}' )
            {
                ps+=3;
                dest += addstr(dest, "Diana");
            }
            else
            {
                *dest = *ps;
                ps++;
                dest++;
                *dest = 0;
            }
        }
        else
        {
            *dest = *ps;
            ps++;
            dest++;
            *dest = 0;
        }
    }
}
#endif
