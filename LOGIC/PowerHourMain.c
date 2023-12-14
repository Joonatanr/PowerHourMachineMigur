/*
 * PowerHourMain.c
 *
 *  Created on: 13 Dec 2023
 *      Author: Joonatan
 */

#include "PowerHourMain.h"
#include "display.h"
#include "LcdWriter.h"
#include "Buzzer.h"
#include "Bitmaps/Bitmaps.h"
#include "systimer.h"

#define DISABLE_BUZZER_FOR_TESTING

#define CLOCK_X_OFFSET 3u
#define CLOCK_Y_OFFSET 15u
#define CLOCK_LETTER_SPACING 23u

#define BEERSHOT_IMAGE_X_OFFSET 3u
#define BEERSHOT_IMAGE_Y_OFFSET 15u

#define TEXT_X_OFFSET 3u
#define TEXT_Y_OFFSET 3u
#define TEXT_LINE_DISTANCE 18u

#define BORDER_WIDTH 2u

/* CLOCK area dimensions */
#define CLOCK_AREA_X_BEGIN  0u
#define CLOCK_AREA_Y_BEGIN  0u
#define CLOCK_AREA_X_END    115u
#define CLOCK_AREA_Y_END    86u

/* BEERSHOT area dimensions */
#define BEERSHOT_AREA_X_BEGIN (CLOCK_AREA_X_END + BORDER_WIDTH)
#define BEERSHOT_AREA_Y_BEGIN 0u
#define BEERSHOT_AREA_X_END DISPLAY_WIDTH - 1u
#define BEERSHOT_AREA_Y_END CLOCK_AREA_Y_END

/* Text area dimensions */
#define TEXT_AREA_X_BEGIN 0u
#define TEXT_AREA_X_END   (DISPLAY_WIDTH - 1u)
#define TEXT_AREA_Y_BEGIN (CLOCK_AREA_Y_END + BORDER_WIDTH)
#define TEXT_AREA_Y_END DISPLAY_HEIGHT - 1u


volatile U32 maximum_cyclic_period = 0u;



Private U16 priv_curr_minute;
Private U16 priv_curr_second;

Private void drawClock(void);
Private void incrementTimer(void);
Private void drawBorders(void);
Private void drawBeerShot(U8 level);
Private void drawTextOnLine(char * text, int line);

#define NUMBER_OF_BEERSHOT_IMAGES 8U
Private const tImage * priv_beershot_images[NUMBER_OF_BEERSHOT_IMAGES] =
{
     &BeerShot_1,
     &BeerShot_2,
     &BeerShot_3,
     &BeerShot_4,
     &BeerShot_5,
     &BeerShot_6,
     &BeerShot_7,
     &BeerShot_8,
};

Private U8 priv_beershot_counter = 0u;

Public void powerHour_init(void)
{
    priv_curr_minute = 0u;
    priv_curr_second = 0u;

    priv_beershot_counter = 0u;
}

Public void powerHour_start(void)
{
    display_clear();
    drawBorders();

    drawClock();
    drawBeerShot(priv_beershot_counter);

    /* Test drawing a string */
    drawTextOnLine("At the end of this round,", 0u);
    drawTextOnLine("there is a task for all the girls", 1u);
}

Private U32 cyclic_begin_timestamp;
Private U32 cyclic_end_timestamp;

Public void powerHour_cyclic1000msec(void)
{
    cyclic_begin_timestamp = systimer_getTimestamp();

    if (priv_curr_second == 59u)
    {
#ifndef DISABLE_BUZZER_FOR_TESTING
        buzzer_playBeeps(3u);
#endif
    }

    incrementTimer();
    drawClock();

    /* Just for testing the beershot animation : TODO, Remove this. */
    priv_beershot_counter++;
    if (priv_beershot_counter >= NUMBER_OF_BEERSHOT_IMAGES)
    {
        priv_beershot_counter = 0u;
    }
    drawBeerShot(priv_beershot_counter);

    cyclic_end_timestamp = systimer_getTimestamp();
    maximum_cyclic_period = MAX(maximum_cyclic_period, cyclic_end_timestamp - cyclic_begin_timestamp);
}

Public void powerHour_stop(void)
{

}


Private void incrementTimer(void)
{
    priv_curr_second++;
    if (priv_curr_second >= 60u)
    {
        priv_curr_second = 0u;
        priv_curr_minute++;
    }
}


Private void drawBorders(void)
{
    display_fillRectangle(CLOCK_AREA_X_BEGIN, CLOCK_AREA_Y_END, DISPLAY_WIDTH, BORDER_WIDTH, disp_text_color);
    display_fillRectangle(CLOCK_AREA_X_END, CLOCK_AREA_Y_BEGIN, BORDER_WIDTH, CLOCK_AREA_Y_END, disp_text_color);
}


Private void drawBeerShot(U8 level)
{
    /* Lets begin with some very initial testing. */
    if(level < NUMBER_OF_BEERSHOT_IMAGES)
    {
        drawBitmap(BEERSHOT_AREA_X_BEGIN + BEERSHOT_IMAGE_X_OFFSET, BEERSHOT_AREA_Y_BEGIN + BEERSHOT_IMAGE_Y_OFFSET, priv_beershot_images[level]);
    }
}


Private void drawClock(void)
{
    U8 x_offset = CLOCK_X_OFFSET;

    LcdWriter_drawCharColored('0' + (priv_curr_minute / 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING;
    LcdWriter_drawCharColored('0' + (priv_curr_minute % 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING - 1u;
    LcdWriter_drawCharColored(':' , x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += 14u;
    LcdWriter_drawCharColored('0' + (priv_curr_second / 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING;
    LcdWriter_drawCharColored('0' + (priv_curr_second % 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
}


Private void drawTextOnLine(char * text, int line)
{
    /* We should have enough space for two lines of text...*/
    if (line <= 1u)
    {
        LcdWriter_drawColoredString(text, TEXT_X_OFFSET, TEXT_AREA_Y_BEGIN + TEXT_Y_OFFSET + (line * TEXT_LINE_DISTANCE), FONT_SMALL_FONT_10, disp_ph_prompt_text_color, disp_background_color);
    }
}
