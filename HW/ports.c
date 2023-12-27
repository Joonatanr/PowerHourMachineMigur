/*
 * ports.c
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#include "ports.h"
#include "driverlib.h"


typedef struct
{
    U8 port;
    U16 pin;
} Gpio_Conf_T;


Private const Gpio_Conf_T priv_output_conf[NUMBER_OF_OUTPUT_PINS] =
{
     {.port = GPIO_PORT_P1, .pin = GPIO_PIN0 }, /*  LED_ONE         */
     {.port = GPIO_PORT_P2, .pin = GPIO_PIN0 },/*  LED_TWO_RED     */
     {.port = GPIO_PORT_P2, .pin = GPIO_PIN1 },/*  LED_TWO_GREEN   */
     {.port = GPIO_PORT_P2, .pin = GPIO_PIN2 },/*  LED_TWO_BLUE    */
     {.port = GPIO_PORT_P6, .pin = GPIO_PIN1 },/*  DISP_RESET      */
     {.port = GPIO_PORT_P1, .pin = GPIO_PIN7 },/*  DISP_RS         */
     {.port = GPIO_PORT_P4, .pin = GPIO_PIN6 },/*  DISP_CS         */
     {.port = GPIO_PORT_P5, .pin = GPIO_PIN1 },/*  SD_CARD_CS      */
};

Private const Gpio_Conf_T priv_input_conf[NUMBER_OF_INPUT_PINS] =
{
     {.port = GPIO_PORT_P1, .pin = GPIO_PIN1 }, /*  LED_ONE         */
     {.port = GPIO_PORT_P1, .pin = GPIO_PIN4 }, /*  LED_TWO_RED     */
};

/***************************** Public functions  **************************************************/
Public void ports_init(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_OUTPUT_PINS; ix++)
    {
        GPIO_setAsOutputPin(priv_output_conf[ix].port, priv_output_conf[ix].pin);
    }

    for (ix = 0u; ix < NUMBER_OF_INPUT_PINS; ix++)
    {
        GPIO_setAsInputPinWithPullUpResistor(priv_input_conf[ix].port, priv_input_conf[ix].pin);
    }

    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
}


Public void ports_setOutput(U8 state, Output_Pin_T output)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(priv_output_conf[output].port, priv_output_conf[output].pin);
    }
    else
    {
        GPIO_setOutputLowOnPin(priv_output_conf[output].port, priv_output_conf[output].pin);
    }
}

Public U8 ports_isInput(Input_Pin_T input)
{
    U8 res = 0u;
    if (GPIO_getInputPinValue(priv_input_conf[input].port, priv_input_conf[input].pin) == GPIO_INPUT_PIN_LOW)
    {
        res = 1u;
    }
    return res;
}
