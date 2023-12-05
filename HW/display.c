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

//#define CONVERT_888RGB_TO_565BGR(r, g, b) ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11))
/* The problem here is that we shall be accessing memory via DMA, 1 byte at a time. So we also need to switch the MSB and LSB of the resulting U16 here. */
#define CONVERT_888RGB_TO_565BGR(r, g, b) ((r >> 3) << 8 | ((g >> 5) << 13) | ((b >> 3) << 3) | ((g >> 2) & 0x0005u))

#define COLOR_BLUE (U16)(CONVERT_888RGB_TO_565BGR(0x00u, 0x00u, 0xFFu))
#define COLOR_RED (U16)(CONVERT_888RGB_TO_565BGR(0xFFu, 0x00u, 0x00u))
#define COLOR_GREEN (U16)(CONVERT_888RGB_TO_565BGR(0x00u, 0xFFu, 0x00u))


//Private U16 priv_frame_buf[132][162];
Private U16 priv_frame_buf[162][132];
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
void LCD_DrawBuffer(void);

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

    //LCD_Data(0x01);
    //LCD_Data(0x2C);
    //LCD_Data(0x2D);

    LCD_Command(0xB2);
    //LCD_Data(0x01);
    //LCD_Data(0x2C);
    //LCD_Data(0x2D);
    U8 fr_data_2[3] = {0x01u, 0x2Cu, 0x2Du};
    LCD_Data( fr_data_2, 3u);

    LCD_Command(0xB3);
    U8 fr_data_3[6] = {0x01u, 0x2Cu, 0x2Du, 0x01u, 0x2Cu, 0x2Du};
    LCD_Data( fr_data_3 , 6u);
    //LCD_Data(0x01);
    //LCD_Data(0x2C);
    //LCD_Data(0x2D);
    //LCD_Data(0x01);
    //LCD_Data(0x2C);
    //LCD_Data(0x2D);
    //------------------------------------End ST7735R Frame Rate-----------------------------------------//
    LCD_Command(0xB4);//Column inversion
    LCD_Data_Byte(0x07u);
    //------------------------------------ST7735R Power Sequence-----------------------------------------//
    LCD_Command(0xC0);
    U8 power_cmd1[3] = {0xA2, 0x02, 0x84};
    LCD_Data( power_cmd1, 3u);
    //LCD_Data(0xA2);
    //LCD_Data(0x02);
    //LCD_Data(0x84);
    LCD_Command(0xC1);
    LCD_Data_Byte(0xC5);
    LCD_Command(0xC2);
    U8 power_cmd2[2] = {0x0A, 0x00};
    LCD_Data(power_cmd2 , 2u);
   // LCD_Data(0x0A);
   //LCD_Data(0x00);
    LCD_Command(0xC3);
    U8 power_cmd3[2] = {0x8A, 0x2A};
    LCD_Data(power_cmd3 , 2u);
    //LCD_Data(0x8A);
    //LCD_Data(0x2A);
    LCD_Command(0xC4);
    U8 power_cmd4[2] = {0x8A, 0xEE};
    LCD_Data( power_cmd4, 2u);
    //LCD_Data(0x8A);
    //LCD_Data(0xEE);
    //---------------------------------End ST7735R Power Sequence-------------------------------------//
    LCD_Command(0xC5);//VCOM
    LCD_Data_Byte(0x0E);
    LCD_Command(0x36);//MX, MY, RGB mode
    LCD_Data_Byte(0x88);
    //------------------------------------ST7735R Gamma Sequence-----------------------------------------//
    LCD_Command(0xe0);
    U8 gamma_cmd1[16] = {0x02u, 0x1cu, 0x07u, 0x12u, 0x37u, 0x32u, 0x29u, 0x2du, 0x29u, 0x25u, 0x2bu, 0x39u, 0x00u, 0x01u, 0x03u, 0x10u};
    LCD_Data(gamma_cmd1, 16u);
    //LCD_Data(0x02);
    //LCD_Data(0x1c);
    //LCD_Data(0x07);
    //LCD_Data(0x12);
    //LCD_Data(0x37);
    //LCD_Data(0x32);
    //LCD_Data(0x29);
    //LCD_Data(0x2d);
    //LCD_Data(0x29);
    //LCD_Data(0x25);
    //LCD_Data(0x2b);
    //LCD_Data(0x39);
    //LCD_Data(0x00);
    //LCD_Data(0x01);
    //LCD_Data(0x03);
    //LCD_Data(0x10);

    LCD_Command(0xe1);
    U8 gamma_cmd2[16] = {0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29u, 0x2du,0x2eu, 0x2eu, 0x37u, 0x3fu, 0x00u, 0x00u, 0x02u, 0x10u};
    LCD_Data(gamma_cmd2, 16u);

    //LCD_Data(0x03);
    //LCD_Data(0x1d);
    //LCD_Data(0x07);
    //LCD_Data(0x06);
    //LCD_Data(0x2e);
    //LCD_Data(0x2c);
    //LCD_Data(0x29);
    //LCD_Data(0x2d);
    //LCD_Data(0x2e);
    //LCD_Data(0x2e);
    //LCD_Data(0x37);
    //LCD_Data(0x3f);
    //LCD_Data(0x00);
    //LCD_Data(0x00);
    //LCD_Data(0x02);
    //LCD_Data(0x10);

    LCD_Command(0x2A);
    U8 gamma_cmd3[4] = {0x00, 0x02, 0x00, 0x81};
    LCD_Data(gamma_cmd3, 4u);
   //LCD_Data(0x00);
   //LCD_Data(0x02);
   //LCD_Data(0x00);
   //LCD_Data(0x81);

   LCD_Command(0x2B);
   U8 gamma_cmd4[4] = {0x00, 0x01, 0x00, 0xA0};
   LCD_Data(gamma_cmd4, 4u);
   //LCD_Data(0x00);
   //LCD_Data(0x01);
   //LCD_Data(0x00);
   //LCD_Data(0xA0);
    //------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

    LCD_Command(0x3A); //65k mode
    LCD_Data_Byte(0x05);

    LCD_Command(0x2C);//Display on
    LCD_Rectangle(0,0,132,162,0x0000u); // black it out
    LCD_Command(0x29);//Display on

    //LCD_Rectangle(10,10,80,80,COLOR_RED);
    //LCD_Rectangle(10,10,80,80,COLOR_GREEN);
    //LCD_RectangleRainbow(10,10,80,100);


    //Test Drawing a frame buffer.
    memset(priv_frame_buf, 0x00u, sizeof(U16) * 162u * 132u);


    //Lets set some lines to a different color.
    int x;
    for (x = 0; x < 162; x++)
    {
        priv_frame_buf[x][5] = COLOR_RED;
        priv_frame_buf[x][6] = COLOR_RED;
        priv_frame_buf[x][7] = COLOR_RED;
        priv_frame_buf[x][8] = COLOR_RED;
        priv_frame_buf[x][9]= COLOR_GREEN;
        priv_frame_buf[x][10] = COLOR_GREEN;
    }

    priv_frame_buf[50][50] = COLOR_GREEN;

    LCD_DrawBuffer();

    setDisplayCS(1);
    setBL(1u);
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

    //setRS(1);
    //setCS(0);

    for (y=y1; y <= y2; y++)
    {
        for (x=x1; x <= x2; x++)
        {
            LCD_Data_Byte(colour >> 8);
            LCD_Data_Byte(colour & 0xFF);
        }
    }

    //setCS(1);
}


void LCD_RectangleRainbow(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    int x, y;
    U16 colour;
    U8 colour_index = 0u;

    U16 colors[6] = {COLOR_BLUE,COLOR_BLUE, COLOR_GREEN,COLOR_GREEN, COLOR_RED,COLOR_RED};

    LCD_SetArea(x1,y1,x2,y2);
    LCD_Command(0x2C);

    //setRS(1);
    //setCS(0);

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
    //setCS(1);
}


void LCD_DrawBuffer(void)
{
    LCD_SetArea(0,0,132u,162u);
    LCD_Command(0x2C);

    setRS(1);
    //setCS(0);

    spidrv_transmitU16(&priv_frame_buf[0][0], 132u * 162u);

    //setCS(1);
}
