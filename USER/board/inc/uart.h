#ifndef __UART_H__
#define __UART_H__

#include "stm32f0xx.h"
#include "LCD.h"
#include <stdio.h>
#include <string.h>
 
#define BUFFER_SIZE   10

#define USART1_RDR_Address    0x40013824
#define USART1_TDR_Address    0x40013828
 
#define USART2_RDR_Address    0x40004424
#define USART2_TDR_Address    0x40004428

#define countof(a)   (sizeof(a) / sizeof(*(a)))


void User_USART_Init(void);
void USART1_String(uint8_t *Data);
void USART2_String(uint8_t *Data);
void USART2_SendData(uint8_t *Command,uint8_t length);
void USART1_SendData(uint8_t *Data,uint8_t length);
uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void USART1_RECV_Handle(void);
#endif
