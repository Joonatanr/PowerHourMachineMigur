/*
 * display.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */


#include "display.h"
#include "ports.h"
#include "spidrv.h"
#include "timer.h"
#include "driverlib.h"


#define DISPLAY_WIDTH 162
#define DISPLAY_HEIGHT 132


Private U16 priv_frame_buf[132][162];
//Private U16 priv_frame_buf[162][132];
/*
 * Current ports :
 * RESET : 6.1
 * RS    : 1.7
 * CS    : 4.3
 *
 * DIN   : 1.6
 * CLK   : 1.5
 * BL    : 5.0 */


Private void LCD_Init(void);
Private void setReset(U8 state);

Private void LCD_Data_Byte(U8 data);
Private void LCD_Command(unsigned char cmd);
Private void LCD_Data(U8 * data_ptr, U16 len);

void LCD_SetArea(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
void LCD_Rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short colour);
void LCD_RectangleRainbow(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
void LCD_DrawFrameBuffer(U8 xOffset, U8 yOffset, U8 width, U8 height);


Public void display_init(void)
{
    LCD_Init();
}

Private inline void setReset(U8 state)
{
    ports_set_disp_reset(state);
}

Private inline void DelayMs(U32 period)
{
    timer_delay_msec(period);
}


Private void LCD_Command(unsigned char cmd)
{
    setRS(0u);
    SPI_Write_Byte(EUSCI_B0_BASE, cmd);
    setRS(1u);
}

Private void LCD_Data(U8 * data_ptr, U16 len)
{
    SPI_Write(EUSCI_B0_BASE ,data_ptr, len);
}

Private void LCD_Data_Byte(U8 data)
{
    SPI_Write_Byte(EUSCI_B0_BASE, data);
}


Private void LCD_Init(void)
{
    setReset(0);
    DelayMs(500);
    setReset(1);
    DelayMs(500);

    setDisplayCS(0);
    LCD_Command(0x11);//Sleep out
    DelayMs(120);
    //ST7735R Frame Rate
    LCD_Command(0xB1);
    U8 fr_data_1[3] = {0x01u, 0x2Cu, 0x2Du};
    LCD_Data( fr_data_1, 3u);

    LCD_Command(0xB2);
    U8 fr_data_2[3] = {0x01u, 0x2Cu, 0x2Du};
    LCD_Data( fr_data_2, 3u);

    LCD_Command(0xB3);
    U8 fr_data_3[6] = {0x01u, 0x2Cu, 0x2Du, 0x01u, 0x2Cu, 0x2Du};
    LCD_Data( fr_data_3 , 6u);

    //------------------------------------End ST7735R Frame Rate-----------------------------------------//
    LCD_Command(0xB4);//Column inversion
    LCD_Data_Byte(0x07u);
    //------------------------------------ST7735R Power Sequence-----------------------------------------//
    LCD_Command(0xC0);
    U8 power_cmd1[3] = {0xA2, 0x02, 0x84};
    LCD_Data( power_cmd1, 3u);

    LCD_Command(0xC1);
    LCD_Data_Byte(0xC5);
    LCD_Command(0xC2);
    U8 power_cmd2[2] = {0x0A, 0x00};
    LCD_Data(power_cmd2 , 2u);

    LCD_Command(0xC3);
    U8 power_cmd3[2] = {0x8A, 0x2A};
    LCD_Data(power_cmd3 , 2u);

    LCD_Command(0xC4);
    U8 power_cmd4[2] = {0x8A, 0xEE};
    LCD_Data( power_cmd4, 2u);

    //---------------------------------End ST7735R Power Sequence-------------------------------------//
    LCD_Command(0xC5);//VCOM
    LCD_Data_Byte(0x0E);
    LCD_Command(0x36);//MX, MY, RGB mode
    LCD_Data_Byte(0xA8);
    //------------------------------------ST7735R Gamma Sequence-----------------------------------------//
    LCD_Command(0xe0);
    U8 gamma_cmd1[16] = {0x02u, 0x1cu, 0x07u, 0x12u, 0x37u, 0x32u, 0x29u, 0x2du, 0x29u, 0x25u, 0x2bu, 0x39u, 0x00u, 0x01u, 0x03u, 0x10u};
    LCD_Data(gamma_cmd1, 16u);


    LCD_Command(0xe1);
    U8 gamma_cmd2[16] = {0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29u, 0x2du,0x2eu, 0x2eu, 0x37u, 0x3fu, 0x00u, 0x00u, 0x02u, 0x10u};
    LCD_Data(gamma_cmd2, 16u);


    LCD_Command(0x2A);
    U8 gamma_cmd3[4] = {0x00, 0x02, 0x00, 0x81};
    LCD_Data(gamma_cmd3, 4u);

   LCD_Command(0x2B);
   U8 gamma_cmd4[4] = {0x00, 0x01, 0x00, 0xA0};
   LCD_Data(gamma_cmd4, 4u);

    //------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

    LCD_Command(0x3A); //65k mode
    LCD_Data_Byte(0x05);

    LCD_Command(0x2C);//Display on
    //LCD_Rectangle(0,0,162,132,COLOR_RED); // black it out
    LCD_Command(0x29);//Display on


    //Test Drawing a frame buffer.
    memset(priv_frame_buf, 0x00u, sizeof(U16) * 162u * 132u);


    //Lets set some lines to a different color.
#if 0
    int x;
    for (x = 0; x < 162; x++)
    {
        priv_frame_buf[5][x] = COLOR_RED;
        priv_frame_buf[6][x] = COLOR_RED;
        priv_frame_buf[7][x] = COLOR_BLUE;
        priv_frame_buf[8][x] = COLOR_BLUE;
        priv_frame_buf[9][x]= COLOR_GREEN;
        priv_frame_buf[10][x] = COLOR_GREEN;
    }

    priv_frame_buf[50][50] = COLOR_GREEN;
#endif
    LCD_DrawFrameBuffer(0u, 0u, 162u, 132u);

    setDisplayCS(1);
}


void LCD_SetArea(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    LCD_Command(0x2A);
    LCD_Data_Byte(x1 >> 8);
    LCD_Data_Byte(x1 & 0xFF);
    LCD_Data_Byte(x2 >> 8);
    LCD_Data_Byte(x2 & 0xFF);

    LCD_Command(0x2B);
    LCD_Data_Byte(y1 >> 8);
    LCD_Data_Byte(y1 & 0xFF);
    LCD_Data_Byte(y2 >> 8);
    LCD_Data_Byte(y2 & 0xFF);
}

void LCD_Rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short colour)
{
    int x, y;

    LCD_SetArea(x1,y1,x2,y2);
    LCD_Command(0x2C);

    for (y=y1; y <= y2; y++)
    {
        for (x=x1; x <= x2; x++)
        {
            LCD_Data_Byte(colour >> 8);
            LCD_Data_Byte(colour & 0xFF);
        }
    }
}


void LCD_RectangleRainbow(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    int x, y;
    U16 colour;
    U8 colour_index = 0u;

    U16 colors[6] = {COLOR_BLUE,COLOR_BLUE, COLOR_GREEN,COLOR_GREEN, COLOR_RED,COLOR_RED};

    LCD_SetArea(x1,y1,x2,y2);
    LCD_Command(0x2C);

    for (y=y1; y <= y2; y++)
    {
        colour = colors[colour_index];
        colour_index++;
        if (colour_index == 6u)
        {
            colour_index = 0u;
        }

        for (x=x1; x <= x2; x++)
        {
            LCD_Data_Byte(colour >> 8);
            LCD_Data_Byte(colour & 0xFF);
        }
    }
}


void LCD_DrawFrameBuffer(U8 xOffset, U8 yOffset, U8 width, U8 height)
{
    display_drawImage(xOffset, yOffset, width, height, &priv_frame_buf[0][0]);
}

/* TODO : This is currently a placeholder API. */
Public U16 * display_get_frame_buffer(void)
{
    return &priv_frame_buf[0][0];
}


Public void display_flushBuffer(U8 x, U8 y, U8 width, U8 height)
{
    LCD_DrawFrameBuffer(x, y, width, height);
}


Public void display_drawImage(U8 x, U8 y, U8 width, U8 height, const U16 * src_ptr)
{
    U16 x2 = x + width - 1u;
    U16 y2 = y + height - 1u;
    U16 drawWidth;
    U16 drawHeight;

    x2 = MIN(x2, DISPLAY_WIDTH);
    y2 = MIN(y2, DISPLAY_HEIGHT);

    drawWidth = x2 - x + 1u;
    drawHeight = y2 - y + 1u;

    setDisplayCS(0);

    LCD_SetArea(x, y , x2, y2);
    LCD_Command(0x2C);

    setRS(1);

    spidrv_transmitU16(src_ptr, drawWidth * drawHeight);
    setDisplayCS(1);
}
