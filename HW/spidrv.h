/*
 * spi.h
 *
 *  Created on: Aug 13, 2017
 *      Author: Joonatan
 */

#ifndef HW_SPI_DRV_H_
#define HW_SPI_DRV_H_


#include "typedefs.h"

Public void spidrv_init(void);
//Public void spidrv_transmit(U8 * data, U16 data_len);
Public void SPI_Write_Byte(uint32_t SPI, U8 byte);
Public void spidrv_transmitU16(const U16 * data, U32 data_len);
Public void spidrv_transmitU16constValue(const U16 data, U32 data_len);


Public void SPI_Write(uint32_t SPI, uint8_t *Data, uint32_t Size);
Public void SPI_Read(uint32_t SPI, uint8_t *Data, uint32_t Size);

#endif /* HW_SPI_DRV_H_ */
