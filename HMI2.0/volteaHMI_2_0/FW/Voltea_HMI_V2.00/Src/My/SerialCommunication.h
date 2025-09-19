#ifndef __SERIAL_COMMUNICATION_H__
#define __SERIAL_COMMUNICATION_H__

#include<stm32f0xx_hal.h>

extern UART_HandleTypeDef huart1;

#define BUFF_SIZE 500
extern uint8_t SerialRxBuff[BUFF_SIZE];
extern uint8_t SerialRecvByte;

struct flag{
	char CommandReceive:1;
	char CanTransmitAgain:1;
};
extern struct flag CommFlag;
extern unsigned char SendCRC;
extern uint16_t RecvCounter;

extern void SendString(char* str);

#endif
