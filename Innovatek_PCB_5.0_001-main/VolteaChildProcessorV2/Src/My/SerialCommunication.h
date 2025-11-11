#ifndef __SERIALCOMMUNICATION_H__
#define __SERIALCOMMUNICATION_H__

#include<stm32f0xx_hal.h>
#include<stm32f0xx_hal_uart.h>

#define COMM_BUFF_SIZE 50

extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;

struct comm_flags{
	uint8_t Command:1;
	uint8_t CanTransmitAgain:1;
};

extern struct comm_flags CommFlag;
//extern struct mp3_comm_flags MP3_Flag;

extern char TxBuff[COMM_BUFF_SIZE];
extern uint8_t RxBuff[COMM_BUFF_SIZE];
extern uint8_t RxByte;

extern void ReInitUart(UART_HandleTypeDef *huart);

#endif
