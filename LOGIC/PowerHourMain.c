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

#define DISABLE_BUZZER_FOR_TESTING

#define CLOCK_X_OFFSET 10u
#define CLOCK_Y_OFFSET 10u
#define CLOCK_LETTER_SPACING 25u

#define BORDER_WIDTH 2u

/* CLOCK area dimensions */
#define CLOCK_AREA_X_BEGIN  0u
#define CLOCK_AREA_Y_BEGIN  0u
#define CLOCK_AREA_X_END    DISPLAY_WIDTH - 1u
#define CLOCK_AREA_Y_END    65u

/* Text area dimensions */
#define TEXT_AREA_X_BEGIN 0u
#define TEXT_AREA_X_END   110u
#define TEXT_AREA_Y_BEGIN (CLOCK_AREA_Y_END + BORDER_WIDTH)
#define TEXT_AREA_Y_END DISPLAY_HEIGHT - 1u

/* BEERSHOT area dimensions */
#define BEERSHOT_AREA_X_BEGIN (TEXT_AREA_X_END + BORDER_WIDTH)
#define BEERSHOT_AREA_Y_BEGIN (CLOCK_AREA_Y_END + BORDER_WIDTH)
#define BEERSHOT_AREA_X_END DISPLAY_WIDTH - 1u
#define BEERSHOT_AREA_Y_END DISPLAY_HEIGHT - 1u




Private U16 priv_curr_minute;
Private U16 priv_curr_second;

Private void drawClock(void);
Private void incrementTimer(void);
Private void drawBorders(void);
Private void drawBeerShot(U8 level);

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
}

Public void powerHour_cyclic1000msec(void)
{
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
    display_fillRectangle(TEXT_AREA_X_END, TEXT_AREA_Y_BEGIN, BORDER_WIDTH, TEXT_AREA_Y_END, disp_text_color);
}


Private void drawBeerShot(U8 level)
{
    /* Lets begin with some very initial testing. */
    if(level < NUMBER_OF_BEERSHOT_IMAGES)
    {
        drawBitmap(BEERSHOT_AREA_X_BEGIN + 5u, BEERSHOT_AREA_Y_BEGIN + 5u, priv_beershot_images[level]);
    }
}


Private void drawClock(void)
{
    U8 x_offset = CLOCK_X_OFFSET;

    LcdWriter_drawCharColored('0' + (priv_curr_minute / 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING;
    LcdWriter_drawCharColored('0' + (priv_curr_minute % 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING;
    LcdWriter_drawCharColored(':' , x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING;
    LcdWriter_drawCharColored('0' + (priv_curr_second / 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += CLOCK_LETTER_SPACING;
    LcdWriter_drawCharColored('0' + (priv_curr_second % 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
}
