/*
 * ports.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#include "ports.h"
#include "driverlib.h"


Public void ports_init(void)
{
    /* TODO : Fix the ports module, so we have all the pins in one table and it is only necessary to change them in one place. */
    //First lets set up LED ports as outputs.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    //Lets set up S1 and S2 as input pins.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

    /* Set up the display reset pin */
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN1);

    /* Setup display chip select pin. */
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);

    /* Setup SD card chip select pin. */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);

    /* Set up the display BL pin. */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN7);
}


Public void ports_set_led_one(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}


Public void ports_set_led_two_red(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
}


Public void ports_set_led_two_green(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
}


Public void ports_set_led_two_blue(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
}


Public void ports_set_disp_reset(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN1);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN1);
    }
}


Public U8 ports_isBtnOne(void)
{
    U8 res = 0x00u;
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_LOW)
    {
        res = 0x01u;
    }
    return res;
}


Public U8 ports_isBtnTwo(void)
{
    U8 res = 0x00u;
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW)
    {
        res = 0x01u;
    }
    return res;
}


/* Set function for the RS or A0 pin. */
Public void setRS(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN7);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
    }
}

/* Set function for Chip Select pin. */
Public void setDisplayCS(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN6);
    }
}


Public void setSdCardCS(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
    }
}


Public void setBL(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN7);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN7);
    }
}
