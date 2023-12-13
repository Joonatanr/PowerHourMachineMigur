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

#define CLOCK_X_OFFSET 10u
#define CLOCK_Y_OFFSET 20u

Private U16 priv_curr_minute;
Private U16 priv_curr_second;

Private void drawClock(void);
Private void incrementTimer(void);


Public void powerHour_init(void)
{
    priv_curr_minute = 0u;
    priv_curr_second = 0u;
}

Public void powerHour_start(void)
{
    display_clear();
    drawClock();
}

Public void powerHour_cyclic1000msec(void)
{
    if (priv_curr_second == 59u)
    {
        buzzer_playBeeps(3u);
    }

    incrementTimer();
    drawClock();
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


Private void drawClock(void)
{
    U8 x_offset = CLOCK_X_OFFSET;

    LcdWriter_drawCharColored('0' + (priv_curr_minute / 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += 30u;
    LcdWriter_drawCharColored('0' + (priv_curr_minute % 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += 30u;
    LcdWriter_drawCharColored(':' , x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += 30u;
    LcdWriter_drawCharColored('0' + (priv_curr_second / 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
    x_offset += 30u;
    LcdWriter_drawCharColored('0' + (priv_curr_second % 10u), x_offset, CLOCK_Y_OFFSET, FONT_TNR_HUGE_NUMBERS, disp_text_color, disp_background_color);
}
