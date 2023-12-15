/*
 * BitmapHandler.h
 *
 *  Created on: 6. dets 2023
 *      Author: JRE
 */

#ifndef LOGIC_BITMAPHANDLER_H_
#define LOGIC_BITMAPHANDLER_H_


#include "typedefs.h"

typedef void(*BitmapLoaderCallback)(void);


extern Boolean LoadBitmap(const char * path, U16 * dest);
extern Boolean StartCyclicBitmapLoad(const char * path, U16 * dest, BitmapLoaderCallback cb);

extern void BitmapHandler_init(void);
extern void BitmapHandler_start(void);
extern void BitmapLoaderCyclic100ms(void);

#endif /* LOGIC_BITMAPHANDLER_H_ */
