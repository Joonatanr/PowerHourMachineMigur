/*
 * buttons.h
 *
 *  Created on: Sep 7, 2017
 *      Author: Joonatan
 */

#ifndef HW_BUTTONS_H_
#define HW_BUTTONS_H_

#include "typedefs.h"

typedef void (*buttonListener)(void);

typedef enum
{
    RISING_EDGE,
    FALLING_EDGE,
    NUMBER_OF_BUTTON_MODES
} ButtonMode;


/* Configuration part - Change this to suit the application. */
typedef enum
{
    RED_BUTTON,
    BLUE_BUTTON,
    BLACK_BUTTON,
    GREEN_BUTTON,

    NUMBER_OF_BUTTONS
} ButtonType;

#define UP_BUTTON       GREEN_BUTTON
#define LEFT_BUTTON     RED_BUTTON
#define RIGHT_BUTTON    BLUE_BUTTON
#define DOWN_BUTTON     BLACK_BUTTON

#define OK_BUTTON       LEFT_BUTTON
#define CANCEL_BUTTON   RIGHT_BUTTON

/**************************************************************/


extern void buttons_init(void);
extern void buttons_cyclic10msec(void);
extern void buttons_cyclic100msec(void);
extern void buttons_subscribeListener(ButtonType btn, buttonListener listener);
extern void buttons_setButtonMode(ButtonType btn, ButtonMode mode);
extern void buttons_subscribeHoldDownListener(ButtonType btn, buttonListener listener);
extern void buttons_unsubscribeAll(void);

extern Boolean isButton(ButtonType btn);


#endif /* HW_BUTTONS_H_ */
