/*
 * ports.h
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#ifndef HW_PORTS_H_
#define HW_PORTS_H_

#include "typedefs.h"


typedef enum
{
    PORTS_LED_ONE,
    PORTS_LED_TWO_RED,
    PORTS_LED_TWO_GREEN,
    PORTS_LED_TWO_BLUE,
    PORTS_DISP_RESET,
    PORTS_DISP_RS,
    PORTS_DISP_CS,
    PORTS_SD_CARD_CS,

    NUMBER_OF_OUTPUT_PINS
} Output_Pin_T;

typedef enum
{
    BTN_ONE,
    BTN_TWO,

    NUMBER_OF_INPUT_PINS
} Input_Pin_T;

extern void ports_init(void);
extern U8 ports_isInput(Input_Pin_T input);
extern void ports_setOutput(U8 state, Output_Pin_T output);


#endif /* HW_PORTS_H_ */
