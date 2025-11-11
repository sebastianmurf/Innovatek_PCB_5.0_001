#include "SerialCommunication.h"
#include "MyConstants.h"
#include "MyDelay.h"
#include "main.h"

uint8_t SerialRxBuff[BUFF_SIZE];
uint8_t SerialTxBuff[BUFF_SIZE];
struct flag CommFlag;

uint16_t RecvCounter = 0;
uint8_t SerialRecvByte;
unsigned char SendCRC;

/*################################ FUNCTION LOCAL TO THE FILE ###############################################*/
/*###########################################################################################################*/
/*###########################################################################################################*/
void SendByte(uint8_t byte)
{	while(CommFlag.CanTransmitAgain != MY_TRUE)
		DelayMachineCycle(100);//without this delay, system is getting hang on this while loop
	HAL_UART_Transmit_IT(&huart1,&byte,1);
	CommFlag.CanTransmitAgain = MY_FALSE;
}
/*###########################################################################################################*/
/*###########################################################################################################*/
void SendString(char* str)
{	for(;*str != '\0';str++)
	{	SendByte(*str);
		SendCRC += *str;
	}
}
/*###########################################################################################################*/
/*###########################################################################################################*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	//static uint16_t RecvCounter = 0;
	static uint8_t CommandPacketStartFlag = MY_FALSE;
	char recv_byte;
	if(huart == &huart1){
		recv_byte = SerialRecvByte;
		HAL_UART_Receive_IT(&huart1,&SerialRecvByte,1);
		if(CommFlag.CommandReceive == MY_FALSE)
		{
			switch(recv_byte)
			{	case '{':	RecvCounter = 0;CommandPacketStartFlag = MY_TRUE; 
									//HAL_GPIO_WritePin(STATUS_LED_GPIO_Port,STATUS_LED_Pin,GPIO_PIN_SET);
									break;
				case '}':	if(CommandPacketStartFlag == MY_TRUE)
							{	SerialRxBuff[RecvCounter]='\0';
								CommFlag.CommandReceive = MY_TRUE;
								CommandPacketStartFlag = MY_FALSE;
								//HAL_GPIO_WritePin(STATUS_LED_GPIO_Port,STATUS_LED_Pin,GPIO_PIN_RESET);
							}
							break;
				default	:	if(CommandPacketStartFlag == MY_TRUE)
							{	SerialRxBuff[RecvCounter] = recv_byte;
								RecvCounter++;
								//RecvCounter %= BUFF_SIZE;//not working. getting stuck in waiting for data
							}
							break;
			}
		}
		//HAL_UART_Receive_IT(&huart1,&SerialRecvByte,1);
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart){
	if(huart == &huart1)
		CommFlag.CanTransmitAgain = MY_TRUE;
}
/*###########################################################################################################*/
/*###########################################################################################################*/
/*###########################################################################################################*/
/*###########################################################################################################*/
/*###########################################################################################################*/
/*###########################################################################################################*/

