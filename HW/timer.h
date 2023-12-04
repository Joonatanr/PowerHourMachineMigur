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

#endif /* HW_TIMER_H_ */
