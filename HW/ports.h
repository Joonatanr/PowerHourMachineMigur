/*
 * ports.h
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#ifndef HW_PORTS_H_
#define HW_PORTS_H_

#include "typedefs.h"

extern void ports_init(void);

extern void ports_set_led_one(U8 state);
extern void ports_set_led_two_red(U8 state);
extern void ports_set_led_two_green(U8 state);
extern void ports_set_led_two_blue(U8 state);

extern void ports_set_disp_reset(U8 state);

extern U8 ports_isBtnOne(void);
extern U8 ports_isBtnTwo(void);

extern void setRS(U8 state);
extern void setCS(U8 state);
extern void setBL(U8 state);

#endif /* HW_PORTS_H_ */
