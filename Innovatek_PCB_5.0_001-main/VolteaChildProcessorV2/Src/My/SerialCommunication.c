#include "SerialCommunication.h"
#include "ProjectConstants.h"
#include "ProjectVariable.h"
#include <string.h>

extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;

struct comm_flags CommFlag;
//struct mp3_comm_flags MP3_Flag;

char TxBuff[COMM_BUFF_SIZE];//not necessary
uint8_t RxBuff[COMM_BUFF_SIZE];
uint8_t RxByte;
uint8_t NtkComStage=0;
//uint8_t MP3_TxBuff[BUFF_SIZE];
//uint8_t MP3_RxBuff[BUFF_SIZE];
//uint8_t MP3_RxByte;

void ReInitUart(UART_HandleTypeDef *huart)
{	HAL_UART_DeInit(huart);
	HAL_Delay(2000);
	
	if(huart == &huart1)
	{	huart1.Instance = USART1;
		huart1.Init.BaudRate = 119200; //MyPcBaudRate;
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		huart1.Init.StopBits = UART_STOPBITS_1;
		huart1.Init.Parity = UART_PARITY_NONE;
		huart1.Init.Mode = UART_MODE_TX_RX;
		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart1.Init.OverSampling = UART_OVERSAMPLING_16;
		huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart1) != HAL_OK)
		{
			//Error_Handler(__FILE__, __LINE__);
		}
	}
	/*
	if(huart == &huart2)
	{	huart2.Instance = USART2;
		huart2.Init.BaudRate = 9600; //MyPcBaudRate;
		huart2.Init.WordLength = UART_WORDLENGTH_8B;
		huart2.Init.StopBits = UART_STOPBITS_1;
		huart2.Init.Parity = UART_PARITY_NONE;
		huart2.Init.Mode = UART_MODE_TX_RX;
		huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart2.Init.OverSampling = UART_OVERSAMPLING_16;
		huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart2) != HAL_OK)
		{
			//Error_Handler(__FILE__, __LINE__);
		}
	}*/
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	static char RxCounter = 0;
	//static char MP3_RxCounter = 0;
	if(huart == &huart1){
		if(CommFlag.Command == MY_FALSE){
			switch(RxByte){
				case '{': 	RxCounter = 0;
									RxBuff[0]='\0'; 
									break;
				case '}': 	CommFlag.Command = MY_TRUE; break; 
				default: 	RxBuff[RxCounter % COMM_BUFF_SIZE] = RxByte;
									RxCounter++;
									RxBuff[RxCounter % COMM_BUFF_SIZE]='\0';
									break;
			}
		}
		HAL_UART_Receive_IT(&huart1,&RxByte,1);
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart){
	if(huart == &huart1)
		CommFlag.CanTransmitAgain = MY_TRUE;
}


void SendString(const uint8_t* msg){
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)msg,strlen((const char*)msg));
}
