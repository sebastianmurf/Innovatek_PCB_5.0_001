#ifndef __GPRSUART_H__
#define __GPRSUART_H__

#include "common.h"
//---------------------------------------------------------------------------------
/******************* GPRS HARDWARE CONFIGURATION STARTS ****************************/
#define GPRS_UART1
#define GPRS_BAUDRATE 	115200 	
#ifdef GPRS_UART0
	#define GPRS_UART		FM3_MFS0_UART
	#define GPRS_RX_IRQn	MFS0RX_IRQn
	#define GPRS_UART_PFR	FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02)   // pin 48 P21->SIN0_0, pin 47 P22->SOT0_0
	#define GPRS_UART_EPFR 	FM3_GPIO->EPFR07 |= (1 << 6)        // Use SIN0_0, SOT0_0
	#define GPRS_RX_IRQHandler	MFS0RX_IRQHandler
#endif 
#ifdef GPRS_UART1
	#define GPRS_UART		FM3_MFS1_UART
	#define GPRS_RX_IRQn	MFS1RX_IRQn
	#define GPRS_UART_PFR	FM3_GPIO->PFR1 |= (1 << 0x01) | (1 << 0x02)   // pin 35 P11->SIN1_1, pin 36 P12->SOT1_1
	#define GPRS_UART_EPFR 	FM3_GPIO->EPFR07 |= (1 << 11) | (1 << 13)     // Use SIN1_1, SOT1_1
	#define GPRS_RX_IRQHandler	MFS1RX_IRQHandler
#endif 
		
#ifdef GPRS_UART4
	#define GPRS_UART			FM3_MFS4_UART
	#define GPRS_RX_IRQn		MFS4RX_IRQn
	#define GPRS_UART_PFR		FM3_GPIO->PFR0 |= (1 << 0x0a) | (1 << 0x0b)   // pin 54 P0A->SIN4_0, pin 55 P0B->SOT4_0
	#define GPRS_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 6)        // Use SIN4_0, SOT4_0
	#define GPRS_RX_IRQHandler	MFS4RX_IRQHandler
#endif
#ifdef GPRS_UART5
	#define GPRS_UART			FM3_MFS5_UART
	#define GPRS_RX_IRQn		MFS5RX_IRQn
	#define GPRS_UART_PFR		FM3_GPIO->PFR6 |= (1 << 0x00) | (1 << 0x01)   //SDK pin 44 P60->SIN5_0, pin 43 P61->SOT5_0
	#define GPRS_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 12)        // Use SIN5_0, SOT5_0
	#define GPRS_RX_IRQHandler	MFS5RX_IRQHandler
#endif

/*************** GPRS GPIO *****************************************************/
//POWER KEY
#define GPRS_PWR_KEY	(1<<12)	//P0C = PORT 0 BIT 12
#define GPRS_PWR_KEY_PFR	FM3_GPIO->PFR0	//PORT FUNCTION REGISTER
#define GPRS_PWR_KEY_PDOR	FM3_GPIO->PDOR0	//PORT DIRECTION OUTPUT REGISTER
#define GPRS_PWR_KEY_DDR	FM3_GPIO->DDR0	//DATA DIRECTION REGISTER
//DTR
#define GPRS_DTR     	(1<<0)	//P00
#define GPRS_DTR_PFR	FM3_GPIO->PFR0
#define GPRS_DTR_PDOR	FM3_GPIO->PDOR0
#define GPRS_DTR_DDR	FM3_GPIO->DDR0

#define GPRS_GPIO_INIT     GPRS_PWR_KEY_PFR &=~GPRS_PWR_KEY; GPRS_PWR_KEY_PDOR |= GPRS_PWR_KEY; GPRS_PWR_KEY_DDR |= GPRS_PWR_KEY;GPRS_DTR_PFR &=~GPRS_DTR; GPRS_DTR_PDOR &= ~GPRS_DTR; GPRS_DTR_DDR |= GPRS_DTR;

#define GPRS_PWR_KEY_HIGH	GPRS_PWR_KEY_PDOR |= GPRS_PWR_KEY
#define GPRS_PWR_KEY_LOW 	GPRS_PWR_KEY_PDOR &= ~GPRS_PWR_KEY
#define GPRS_PWR_KEY_TOG	GPRS_PWR_KEY_PDOR ^= GPRS_PWR_KEY
#define GPRS_DTR_HIGH		GPRS_DTR_PDOR |= GPRS_DTR
#define GPRS_DTR_LOW 		GPRS_DTR_PDOR &= ~GPRS_DTR
#define GPRS_DTR_TOG		GPRS_DTR_PDOR ^= GPRS_DTR

#define GPRS_MODULE_SLEEP	GPRS_DTR_HIGH
#define GPRS_MODULE_WAKEUP	GPRS_DTR_LOW
/******************* GPS HARDWARE CONFIGURATION ENDS ******************************/
//---------------------------------------------------------------------------------
/******************* GPS CONSTANTS CONFIGURATION STARTS ***************************/
#define GPRS_BUFF_SIZE 600
/******************* GPS CONSTANTS CONFIGURATION ENDS *****************************/
//---------------------------------------------------------------------------------
/******************* GPS COMMUNICATION VARIABLE LOCAL TO THE FILE STARTS **********/
extern char GprsBuff[GPRS_BUFF_SIZE];
extern boolean_t GprsByteRecvFlag;
extern boolean_t MobileDebugFlag;
extern ui GprsCrcCodeOut;
extern ui GprsReceiveCounter;
//extern uc GprsReceiveByte;
extern boolean_t GprsCommandFlag;
//extern boolean_t GprsInterruptRecvDebugFlag;
/******************* GPS COMMUNICATION VARIABLE LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
/******************* GPS COMMUNICATION FUNCTIONS LOCAL TO THE FILE STARTS **********/
//extern char ExeCommand();
extern void InitUartGprs(void);
extern void PutCharGprs(uc u8Char);
extern void PutStringGprs(const char *pu8Buffer);
extern uc GetCharGprs(void);
/******************* GPS COMMUNICATION FUNCTIONS LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------

#endif
