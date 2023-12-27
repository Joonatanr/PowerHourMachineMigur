/*
 * pot.h
 *
 *  Created on: 16. dets 2019
 *      Author: JRE
 */

#ifndef HW_POT_H_
#define HW_POT_H_

#include "typedefs.h"


typedef enum
{
    POTENTIOMETER_NUDE_LEVEL,       /* Stripi mõõdik                        */
    POTENTIOMETER_SEXY_LEVEL,       /* Nilbuste mõõdik                      */
    POTENTIOMETER_MIGUR1,           /* Diana/Mirtel                         */
    POTENTIOMETER_MIGUR2,           /* Mirtel/Diana                         */
    POTENTIOMETER_GIRLS,            /* How brave/intense the girls are      */
    POTENTIOMETER_GUYS,             /* How brave/intense the guys are       */

    NUMBER_OF_DEFINED_POTENTIOMETERS
} potentiometer_T;


extern void pot_init(void);
extern void pot_cyclic_10ms(void);
extern int  pot_getSelectedRange(potentiometer_T pot);


#endif /* HW_POT_H_ */
