/*
 * ScrollBar.c
 *
 *  Created on: Mar 21, 2018
 *      Author: Joonatan
 */

#include "Bargraph.h"
#include "buttons.h"
#include "backlight.h"
#include "display.h"
#include "LcdWriter.h"
#include "MISC/misc.h"
//#include "LOGIC/PowerHourGame/clockDisplay.h"
//#include <LOGIC/SnakeGame/SnakeMain.h>

#define BARGRAPH_BEGIN_X    14u
#define BARGRAPH_WIDTH     100u
#define BARGRAPH_HEIGHT      4u
#define BARGRAPH_OFFSET_Y   53u

#define UP_ARROW_OFFSET_Y   14u
#define DOWN_ARROW_OFFSET_Y 40u

#define NUMBER_OFFSET_Y     25u
#define NUMBER_BOX_WIDTH    30u
#define NUMBER_BOX_HEIGHT   14u




/******************/

/* Lets define all available scrollbars here as public variables. */
Public Bargraph_T BRIGHTNESS_BARGRAPH =
{
     .max_value = 100u,
     .min_value = 0u,
     .increment = 5u,
     .value = 60u,
     .parent = NULL,
     .text = "Brightness",
     .value_changed = backlight_set_level,
     .value_initial_fptr = NULL,
};

Public Bargraph_T SNAKE_SPEED_BARGRAPH =
{
     .max_value = 5u,
     .min_value = 1u,
     .increment = 1u,
     .value = 2u,
     .parent = NULL,
     .text = "Snake speed",
     .value_changed = snake_setSpeed,
     .value_initial_fptr = NULL
};

Public Bargraph_T TASK_FREQUENCY_BARGRAPH =
{
     .max_value = 10u,
     .min_value = 1u,
     .increment = 1u,
     .value = 2u,
     .parent = NULL,
     .text = "Task Frequency (minutes)",
     .value_changed = clockDisplay_setTaskFrequency,
     .value_initial_fptr = clockDisplay_getTaskFrequency
};

/*******************/

Private Bargraph_T * priv_active_bar;
Private char priv_buf[10];

Private const Rectangle priv_number_box =
{
     .location = { GET_X_FROM_CENTER(64u, NUMBER_BOX_WIDTH), NUMBER_OFFSET_Y   },
     .size =     { NUMBER_BOX_HEIGHT,                        NUMBER_BOX_WIDTH  }
};



Private const U16 priv_up_arrow_bmp[5][9] =
{
     { 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0xffffu, 0x0000u, 0x0000u, 0x0000u, 0x0000u },
     { 0x0000u, 0x0000u, 0x0000u, 0xffffu, 0xffffu, 0xffffu, 0x0000u, 0x0000u, 0x0000u },
     { 0x0000u, 0x0000u, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0x0000u, 0x0000u },
     { 0x0000u, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0x0000u },
     { 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu },
};


Private const U16 priv_down_arrow_bmp[5][9] =
{
     { 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu },
     { 0x0000u, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0x0000u },
     { 0x0000u, 0x0000u, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0xffffu, 0x0000u, 0x0000u },
     { 0x0000u, 0x0000u, 0x0000u, 0xffffu, 0xffffu, 0xffffu, 0x0000u, 0x0000u, 0x0000u },
     { 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0xffffu, 0x0000u, 0x0000u, 0x0000u, 0x0000u },
};



/***************************** Private function forward declarations  ******************************/

Private void drawBarGraph(void);
Private void drawBackGround(void);

Private void handleButtonUp(void);
Private void handleButtonDown(void);
Private void handleButtonAck(void);

Private void updateBargraph(void);


/***************************** Public function definitions  ******************************/


/* Called when a scrollbar becomes active. */
Public void enterBarGraph(Bargraph_T * bar)
{
    priv_active_bar = bar;

    buttons_subscribeListener(UP_BUTTON, handleButtonUp);
    buttons_subscribeListener(DOWN_BUTTON, handleButtonDown);
    buttons_subscribeListener(OK_BUTTON, handleButtonAck);
    buttons_subscribeListener(CANCEL_BUTTON, handleButtonAck);

    //priv_number_box = CreateRectangleAroundCenter((Point){64u, NUMBER_OFFSET_Y }, (Size){ 20u, 20u });

    if (bar->value_initial_fptr != NULL)
    {
        bar->value = bar->value_initial_fptr();
    }

    drawBackGround();
    drawBarGraph();
}


/***************************** Private function definitions  ******************************/

/* Draws whole scrollbar */
Private void drawBarGraph(void)
{
    U16 percentage;
    U8 range = priv_active_bar->max_value - priv_active_bar->min_value;

    percentage = ((priv_active_bar->value - priv_active_bar->min_value) * 100u) / range;

    //Draw the line
    /* We clear it first. */
    //display_fillRectangle(BARGRAPH_BEGIN_X, BARGRAPH_OFFSET_Y , BARGRAPH_HEIGHT, BARGRAPH_WIDTH, PATTERN_WHITE);
    display_fillRectangle(BARGRAPH_BEGIN_X, BARGRAPH_OFFSET_Y , BARGRAPH_HEIGHT, BARGRAPH_WIDTH, disp_background_color);

    /* Then draw the actual line. */
    display_fillRectangle(BARGRAPH_BEGIN_X, BARGRAPH_OFFSET_Y , BARGRAPH_HEIGHT, percentage, disp_highlight_color);

    //Draw the number.
    long2string(priv_active_bar->value, priv_buf);
    display_drawTextBox(&priv_number_box, priv_buf, FONT_MEDIUM_FONT);
}


/* Draws the arrows on the scrollbar sides. */
Private void drawBackGround(void)
{
    display_clear();

    //Draw title
    display_drawStringCenter(priv_active_bar->text, 63u, 1u, FONT_COURIER_14 , FALSE);

    //Draw up arrow.
    //display_drawBitmapCenter(&upArrowBitmap, 64u, UP_ARROW_OFFSET_Y , FALSE);
    display_drawBitmapCenter(&priv_up_arrow_bmp[0][0], (DISPLAY_WIDTH / 2) + 4u, UP_ARROW_OFFSET_Y, 9u, 5u);

    //Draw down arrow.
    display_drawBitmapCenter(&priv_down_arrow_bmp[0][0], (DISPLAY_WIDTH / 2) + 4u, DOWN_ARROW_OFFSET_Y, 9u, 5u);
    //display_drawBitmapCenter(&downArrowBitmap, 64u, DOWN_ARROW_OFFSET_Y, FALSE);
}


Private void handleButtonUp(void)
{
    if (priv_active_bar->value < priv_active_bar->max_value)
    {
        priv_active_bar->value += priv_active_bar->increment;
    }

    //Update the displayed data.
    updateBargraph();
}

Private void handleButtonDown(void)
{
    if (priv_active_bar->value > priv_active_bar->min_value)
    {
        priv_active_bar->value -= priv_active_bar->increment;
    }

    //Update the displayed data.
    updateBargraph();
}


Private void updateBargraph(void)
{
    if (priv_active_bar->value_changed != NULL)
    {
       priv_active_bar->value_changed(priv_active_bar->value);
    }

    drawBarGraph();
}

Private void handleButtonAck(void)
{
    if (priv_active_bar->parent != NULL)
    {
        buttons_unsubscribeAll();
        menu_enterMenu(priv_active_bar->parent);
        priv_active_bar = NULL;
    }
    /* Else, I guess we are stuck here...   */
    /* Maybe should restart, just in case?  */
}
