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
    //First lets set up LED ports as outputs.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    //Lets set up S1 and S2 as input pins.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
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
