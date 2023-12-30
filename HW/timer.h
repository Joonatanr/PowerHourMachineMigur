/*
 * timer.h
 *
 *  Created on: 4 Dec 2023
 *      Author: Joonatan
 */

#ifndef HW_TIMER_H_
#define HW_TIMER_H_

#include "typedefs.h"

extern void timer_init(void);
extern void timer_delay_msec(U32 msec);

extern void timer_1msec_callback(void);
extern void timer_1sec_callback(void);

extern U16 generate_random_number_rng(U16 min, U16 max);
extern U16 generate_random_number(U16 max);
extern void regenerate_random_number_seed(void);

#endif /* HW_TIMER_H_ */
