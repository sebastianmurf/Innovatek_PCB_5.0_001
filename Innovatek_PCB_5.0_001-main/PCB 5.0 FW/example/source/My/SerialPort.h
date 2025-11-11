#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <time.h>
#include "common.h"

#define SERIAL_PORT_BUFF_SIZE 20
//---------------------------------------------------------------------------------
/******************* SERIAL_PORT CONSTANTS CONFIGURATION STARTS ***************************/
/******************* SERIAL_PORT CONSTANTS CONFIGURATION ENDS *****************************/
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/******************* SERIAL_PORT COMMUNICATION VARIABLE LOCAL TO THE FILE STARTS **********/
extern boolean_t SerialPortCommandFlag;
extern char SerialPortBuff[21];
/******************* SERIAL_PORT COMMUNICATION VARIABLE LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/******************* SERIAL_PORT COMMUNICATION FUNCTIONS LOCAL TO THE FILE STARTS **********/
extern void InitUartSerialPort(void);
extern void PutCharSerialPort(uc u8Char);
extern void PutStringSerialPort(const char *pu8Buffer);              
extern uc GetCharSeialPort(void);
/******************* SERIAL_LCD COMMUNICATION FUNCTIONS LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
/******************* GPS HARDWARE CONFIGURATION STARTS ****************************/
#define SERIAL_PORT_UART2
#define SERIAL_PORT_UART_BAUDRATE 	115200

#ifdef SERIAL_PORT_UART2
			#define SERIAL_PORT_UART			FM3_MFS2_UART
			#define SERIAL_PORT_RX_IRQn			MFS2RX_IRQn
			#define SERIAL_PORT_UART_PFR		FM3_GPIO->PFR1 |= (1 << 0x07) | (1 << 0x08)	// pin 40 P17->SIN2_2,  pin 44 P18->SOT2_2 
			#define SERIAL_PORT_UART_EPFR		FM3_GPIO->EPFR07 |= (3<<16) | (3<<18) // Uses SIN2_2 at the input pin of MFS-ch.2-SIN. & Uses SOT2_2 at the input pin of MFS-ch.2-SOT.
			#define SERIAL_PORT_RX_IRQHandler	MFS2RX_IRQHandler
#endif
/*		
#ifdef SERIAL_PORT_UART0
	#define SERIAL_PORT_UART			FM3_MFS0_UART
	#define SERIAL_PORT_RX_IRQn			MFS0RX_IRQn
	#define SERIAL_PORT_UART_PFR		FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02)   // pin 48 P21->SIN0_0, pin 47 P22->SOT0_0
	#define SERIAL_PORT_UART_EPFR		FM3_GPIO->EPFR07 |= (1 << 6)        // Use SIN0_0, SOT0_0
	#define SERIAL_PORT_RX_IRQHandler	MFS0RX_IRQHandler
#else 
	#ifdef SERIAL_PORT_UART1
		#define SERIAL_PORT_UART			FM3_MFS1_UART
		#define SERIAL_PORT_RX_IRQn			MFS1RX_IRQn
		#define SERIAL_PORT_UART_PFR		FM3_GPIO->PFR1 |= (1 << 0x01) | (1 << 0x02)   // pin 35 P11->SIN1_1, pin 36 P12->SOT1_1
		#define SERIAL_PORT_UART_EPFR		FM3_GPIO->EPFR07 |= (1 << 11) | (1 << 13)     // Use SIN1_1, SOT1_1
		#define SERIAL_PORT_RX_IRQHandler	MFS1RX_IRQHandler
	#else 
		#ifdef SERIAL_PORT_UART3
			#define SERIAL_PORT_UART			FM3_MFS3_UART
			#define SERIAL_PORT_RX_IRQn			MFS3RX_IRQn
			#define SERIAL_PORT_UART_PFR		FM3_GPIO->PFR5 |= (1 << 0x00) | (1 << 0x01)	// pin 2 P50->SIN3_1, pin 3 P51->SOT3_1
			#define SERIAL_PORT_UART_EPFR		FM3_GPIO->EPFR07 |= (10<<22)				// Use SIN3_1, SOT3_1
			#define SERIAL_PORT_RX_IRQHandler	MFS3RX_IRQHandler
		#else
			#ifdef SERIAL_PORT_UART4
				#define SERIAL_PORT_UART			FM3_MFS4_UART
				#define SERIAL_PORT_RX_IRQn			MFS4RX_IRQn
				#define SERIAL_PORT_UART_PFR		FM3_GPIO->PFR0 |= (1 << 0x0a) | (1 << 0x0b)   // pin 54 P0A->SIN4_0, pin 55 P0B->SOT4_0
				#define SERIAL_PORT_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 6)        // Use SIN4_0, SOT4_0
				#define SERIAL_PORT_RX_IRQHandler	MFS4RX_IRQHandler
			#else
				#ifdef SERIAL_PORT_UART5
					#define SERIAL_PORT_UART			FM3_MFS5_UART
					#define SERIAL_PORT_RX_IRQn			MFS5RX_IRQn
					#define SERIAL_PORT_UART_PFR		FM3_GPIO->PFR6 |= (1 << 0x00) | (1 << 0x01)   //SDK pin 44 P60->SIN5_0, pin 43 P61->SOT5_0
					#define SERIAL_PORT_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 12)        // Use SIN5_0, SOT5_0
					#define SERIAL_PORT_RX_IRQHandler	MFS5RX_IRQHandler
				#endif
			#endif
		#endif
	#endif
#endif
*/
/******************* SERIAL_LCD HARDWARE CONFIGURATION ENDS ******************************/
//---------------------------------------------------------------------------------

#endif