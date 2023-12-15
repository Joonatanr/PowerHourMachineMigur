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


typedef enum
{
    FILES_MEN,
    FILES_WOMEN,
    FILES_EVERYBODY,
    FILES_KAISA,

    NUMBER_OF_FILE_CATEGORIES
} FileCategory_t;

extern Boolean LoadBitmap(const char * path, U16 * dest);
extern Boolean StartCyclicBitmapLoad(const char * path, U16 * dest, BitmapLoaderCallback cb);
extern void BitmapHandler_getRandomBitmapForCategory(FileCategory_t type, char *dest);

extern void BitmapHandler_init(void);
extern void BitmapHandler_start(void);
extern void BitmapLoaderCyclic100ms(void);

#endif /* LOGIC_BITMAPHANDLER_H_ */
