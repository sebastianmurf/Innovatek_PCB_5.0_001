#include"mcu.h"
#include"GprsUart.h"
#include"myfun.h"
#include"common.h"
#include"board.h"
#include"gprs.h"
#include"MyUsb.h"
#include"delay.h"

//----------------
#include<ctype.h>
#include"string.h"
#include"math.h"
//----------------
/******************* GPS COMMUNICATION VARIABLE INITIALIZATION STARTS **********/
char GprsBuff[GPRS_BUFF_SIZE];
boolean_t GprsByteRecvFlag = FALSE;
boolean_t MobileDebugFlag = FALSE;
ui GprsCrcCodeOut;
ui GprsReceiveCounter;
//boolean_t GprsReceiveFlag = FALSE;
//unsigned char GprsReceiveByte;
boolean_t GprsCommandFlag = FALSE;
//boolean_t GprsInterruptRecvDebugFlag = FALSE;
/******************* GPS COMMUNICATION VARIABLE INITIALIZATION ENDS ************/

//------------------------------------------------------------------------------
void GPRS_RX_IRQHandler(void)
{	//static unsigned char GprsRecvCounter = 0;
	uc GprsRecvByte;

  	if ((GPRS_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	GPRS_UART->SSR |= (1 << 7);      // Clear error flags 
    	GprsRecvByte = GPRS_UART->RDR;         // Flush RDR
  	}
  	else if (GPRS_UART->SSR & 0x04)    // RDRF?
  	{	GprsRecvByte = GPRS_UART->RDR;
    	GprsByteRecvFlag = TRUE;
		if(GprsCommandFlag == FALSE)
		{	switch(GprsRecvByte)
			{ 	case '[': 	GprsReceiveCounter = 0; GprsBuff[0]='\0';break;
				case ']': 	GprsBuff[GprsReceiveCounter%GPRS_BUFF_SIZE]='\0';GprsCommandFlag = TRUE; break;
				//case 8:		if(GprsReceiveCounter>0) GprsReceiveCounter--;break;//backspace
				default:  	GprsBuff[GprsReceiveCounter%GPRS_BUFF_SIZE]=GprsRecvByte; 
							GprsReceiveCounter++;
							GprsBuff[GprsReceiveCounter%GPRS_BUFF_SIZE] = '\0';
                            break;
			}
		}
		//PutCharGprs(GprsRecvByte);	//for debug on same port
		//if(GprsInterruptRecvDebugFlag == TRUE) //for debug on DEBUG port
		//	PutCharUsb(GprsRecvByte);
  	}
}
//------------------------------------------------------------------------------
void InitUartGprs()
{	//Initialize GPSR GPIO pins
  	//GPRS_GPIO_INIT;
	// Initialize ports for UART
  	GPRS_UART_PFR;
	GPRS_UART_EPFR;  
  
  	// Initialize UART asynchronous mode
  	GPRS_UART->SCR =  0x80;   // UART reset
  	GPRS_UART->SMR =  0x01;   // serial data output enable, operation mode 0
  	GPRS_UART->SSR =  0x00;   // 0x80 = clear receive error flag
  	GPRS_UART->ESCR = 0x00;          
 
  	GPRS_UART->BGR = ((40000000UL / (GPRS_BAUDRATE -1))/2);
  
	GPRS_UART->SCR |= 0x03;   // RX, TX enable  
  	// UART interrupt settings
  	NVIC_ClearPendingIRQ(GPRS_RX_IRQn);
  	NVIC_EnableIRQ(GPRS_RX_IRQn);
  	NVIC_SetPriority(GPRS_RX_IRQn, 3);
  
  	GPRS_UART->SCR |= 0x10;//set RIE serial receive interrupt enable bit
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharGprs(uint8_t u8Char)
{	/*if(GPRS_DTR_PDOR & GPRS_DTR)	//IF DTR IS HIGH
	{	//MODULE IS IN SLEEP MODE
		GPRS_MODULE_WAKEUP;
		//MIN 20MS DELAY
		DelayInMilliSec(100);
	}*/
	while (!(GPRS_UART->SSR & (1 << 1))); // wait for transmit buffer empty    
	GPRS_UART->TDR = u8Char;                // put u8Char into buffer
	
	GprsCrcCodeOut += u8Char;
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *  RETURNS:        None
 *****************************************************************************/
void PutStringGprs(const char *pu8Buffer)              
{	while (*pu8Buffer != '\0')
  	{ 	PutCharGprs(*pu8Buffer++);	// send every char of string
    	Delay(65000);Delay(65000); //must for long replay from gprs
  	}
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    returns incoming character, if received
 *  PARAMETERS:     None
 *  RETURNS:        Character or 0xFF (Error) or 0 (Nothing)
 *****************************************************************************/
uint8_t GetCharGprs(void)   
{	uint8_t u8Char;
  	if ((GPRS_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	GPRS_UART->SSR |= (1 << 7);      // Clear error flags 
    	u8Char = GPRS_UART->RDR;         // Flush RDR
    	return 0xFF;
  	}
  	else if (GPRS_UART->SSR & 0x04)    // RDRF?
  	{	u8Char = GPRS_UART->RDR;
    	if ('\r' == u8Char)
    	{	u8Char = '\n';
    	}
    	return (u8Char); 
  	}
  	else
  	{	return 0; 
  	}
}
//------------------------------------------------------------------------------
