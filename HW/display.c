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
    // Clear CS pin
    setCS(0);
    spi_transmit_byte(cmd, TRUE);
    // Set CS pin
    setCS(1);
}

Private void LCD_Data(U8 * data_ptr, U16 len)
{
    // Clear CS pin
    setCS(0);
    spidrv_transmit(data_ptr, len);
    // Set CS pin
    setCS(1);
}

Private void LCD_Data_Byte(U8 data)
{
    // Clear CS pin
    setCS(0);
    spi_transmit_byte(data, FALSE);
    // Set CS pin
    setCS(1);
}


Private void LCD_Init(void)
{
    setReset(0);
    DelayMs(500);
    setReset(1);
    DelayMs(500);

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
    LCD_Data_Byte(0xC8);
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

    LCD_Command(0x3A);
    LCD_Data_Byte(0x05);
    //LCD_Command(0x3A);//65k mode
    //LCD_Data(0x05);
    LCD_Command(0x2C);//Display on
    LCD_Rectangle(0,0,128,160,0); // black it out
    LCD_Command(0x29);//Display on

    LCD_Rectangle(10,10,60,40,0x00EF);
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
    setCS(0);

    for (x=x1; x<x2; x++)
        for (y=y1; y<y2; y++)
        {
            LCD_Data_Byte(colour >> 8);
            LCD_Data_Byte(colour & 0xFF);
        }

    setCS(1);
}
