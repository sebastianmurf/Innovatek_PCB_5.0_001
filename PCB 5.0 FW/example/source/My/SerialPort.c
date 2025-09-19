#include "SerialPort.h"
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
/******************* SERIAL PORT COMMUNICATION VARIABLE INITIALIZATION STARTS **********/
char SerialPortBuff[21];
boolean_t SerialPortByteRecvFlag = FALSE;
boolean_t SerialPortDebugFlag = FALSE;
ui SerialPortCrcCodeOut;
ui SerialPortReceiveCounter;
//boolean_t SerialPortReceiveFlag = FALSE;
//unsigned char SerialPortReceiveByte;
boolean_t SerialPortCommandFlag = FALSE;
//boolean_t SerialPortInterruptRecvDebugFlag = FALSE;
/******************* SerialPort COMMUNICATION VARIABLE INITIALIZATION ENDS ************/

//------------------------------------------------------------------------------
void SERIAL_PORT_RX_IRQHandler(void)
{	//static unsigned char SerialPortRecvCounter = 0;
	uc SerialPortRecvByte;

  	if ((SERIAL_PORT_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	SERIAL_PORT_UART->SSR |= (1 << 7);      // Clear error flags 
    	SerialPortRecvByte = SERIAL_PORT_UART->RDR;         // Flush RDR
  	}
  	else if (SERIAL_PORT_UART->SSR & 0x04)    // RDRF?
  	{	SerialPortRecvByte = SERIAL_PORT_UART->RDR;
    	SerialPortByteRecvFlag = TRUE;
		if(SerialPortCommandFlag == FALSE)
		{	switch(SerialPortRecvByte)
			{ 	case '[': 	SerialPortReceiveCounter = 0; SerialPortBuff[0]='\0';break;
				case ']': 	SerialPortBuff[SerialPortReceiveCounter % SERIAL_PORT_BUFF_SIZE]='\0'; SerialPortCommandFlag = TRUE; break;
				//case 8:		if(SerialPortReceiveCounter>0) SerialPortReceiveCounter--;break;//backspace
				default:  	SerialPortBuff[SerialPortReceiveCounter % SERIAL_PORT_BUFF_SIZE]=SerialPortRecvByte; 
							SerialPortReceiveCounter++;
							SerialPortBuff[SerialPortReceiveCounter % SERIAL_PORT_BUFF_SIZE] = '\0';
							break;
			}
		}
		//PutCharSerialPort(SerialPortRecvByte);	//for debug on same port
		//if(SerialPortInterruptRecvDebugFlag == TRUE) //for debug on DEBUG port
		//	PutCharUsb(SerialPortRecvByte);
  	}
}
//------------------------------------------------------------------------------
void InitUartSerialPort()
{	// Initialize ports for UART
  	SERIAL_PORT_UART_PFR;
	SERIAL_PORT_UART_EPFR;  
  
  	// Initialize UART asynchronous mode
  	SERIAL_PORT_UART->SCR =  0x80;   // UART reset
  	SERIAL_PORT_UART->SMR =  0x01;   // serial data output enable, operation mode 0
  	SERIAL_PORT_UART->SSR =  0x00;   // 0x80 = clear receive error flag
  	SERIAL_PORT_UART->ESCR = 0x00;          
 
  	SERIAL_PORT_UART->BGR = ((40000000UL / (SERIAL_PORT_UART_BAUDRATE -1))/2);
  
	SERIAL_PORT_UART->SCR |= 0x03;   // RX, TX enable  
  	// UART interrupt settings
  	NVIC_ClearPendingIRQ(SERIAL_PORT_RX_IRQn);
  	NVIC_EnableIRQ(SERIAL_PORT_RX_IRQn);
  	NVIC_SetPriority(SERIAL_PORT_RX_IRQn, 1);
  
  	SERIAL_PORT_UART->SCR |= 0x10;//set RIE serial receive interrupt enable bit
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharSerialPort(uint8_t u8Char)
{	
	while (!(SERIAL_PORT_UART->SSR & (1 << 1))); // wait for transmit buffer empty    
	SERIAL_PORT_UART->TDR = u8Char;                // put u8Char into buffer
	
	SerialPortCrcCodeOut += u8Char;
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *  RETURNS:        None
 *****************************************************************************/
void PutStringSerialPort(const char *pu8Buffer)              
{	while (*pu8Buffer != '\0')
  	{ 	PutCharSerialPort(*pu8Buffer++);	// send every char of string
    	Delay(65000);Delay(65000); //must for long replay from gprs
  	}
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    returns incoming character, if received
 *  PARAMETERS:     None
 *  RETURNS:        Character or 0xFF (Error) or 0 (Nothing)
 *****************************************************************************/
uint8_t GetCharSerialPort(void)   
{	uint8_t u8Char;
  	if ((SERIAL_PORT_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	SERIAL_PORT_UART->SSR |= (1 << 7);      // Clear error flags 
    	u8Char = SERIAL_PORT_UART->RDR;         // Flush RDR
    	return 0xFF;
  	}
  	else if (SERIAL_PORT_UART->SSR & 0x04)    // RDRF?
  	{	u8Char = SERIAL_PORT_UART->RDR;
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
