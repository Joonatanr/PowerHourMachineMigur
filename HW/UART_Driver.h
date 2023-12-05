#ifndef HARDWARE_UART_DRIVER_H_
#define HARDWARE_UART_DRIVER_H_

#include "driverlib\MSP432P4xx\rom.h"
#include "driverlib\MSP432P4xx\rom_map.h"
#include "driverlib\MSP432P4xx\interrupt.h"
#include "driverlib\MSP432P4xx\uart.h"
#include "driverlib\MSP432P4xx\gpio.h"

#define UARTA0_BUFFERSIZE 128
#define UARTA2_BUFFERSIZE 128

void UART_Init(uint32_t UART, eUSCI_UART_Config UARTConfig);
void UART_Write(uint32_t UART, uint8_t *Data, uint32_t Size);
uint32_t UART_Read(uint32_t UART, uint8_t *Data, uint32_t Size);

#endif /* HARDWARE_UART_DRIVER_H_ */
