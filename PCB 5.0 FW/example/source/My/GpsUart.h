#ifndef __GPSUART_H__
#define __GPSUART_H__

#include <time.h>

#include "common.h"
//---------------------------------------------------------------------------------
/******************* GPS HARDWARE CONFIGURATION STARTS ****************************/
#define GPS_UART0
#ifdef GPS_UART0
	#define GPS_UART		FM3_MFS0_UART
	#define GPS_RX_IRQn		MFS0RX_IRQn
	#define GPS_UART_PFR	FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02)   // pin 36 P21->SIN0_0, pin 35 P22->SOT0_0
	#define GPS_UART_EPFR 	FM3_GPIO->EPFR07 |= (1 << 6)        // Use SIN0_0, SOT0_0
	#define GPS_BAUDRATE 	9600 	
	#define GPS_RX_IRQHandler	MFS0RX_IRQHandler
#else 
	#ifdef GPS_UART4
		#define GPS_UART			FM3_MFS4_UART
		#define GPS_RX_IRQn			MFS4RX_IRQn
		#define GPS_UART_PFR		FM3_GPIO->PFR0 |= (1 << 0x0a) | (1 << 0x0b)   // pin 54 P0A->SIN4_0, pin 55 P0B->SOT4_0
		#define GPS_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 6)        // Use SIN4_0, SOT4_0
		#define GPS_BAUDRATE 		9600
		#define GPS_RX_IRQHandler	MFS4RX_IRQHandler
	#endif
	#ifdef GPS_UART5
		#define GPS_UART			FM3_MFS5_UART
		#define GPS_RX_IRQn			MFS5RX_IRQn
		#define GPS_UART_PFR		FM3_GPIO->PFR6 |= (1 << 0x00) | (1 << 0x01)   //SDK pin 44 P60->SIN5_0, pin 43 P61->SOT5_0
		#define GPS_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 12)        // Use SIN5_0, SOT5_0
		#define GPS_BAUDRATE 		115200
		#define GPS_RX_IRQHandler	MFS5RX_IRQHandler
	#endif
#endif
//GPS GPIO
#define GPS_RESET       (1<<10)	//P3A
#define GPS_EN     		(1<<14)	//P3E

#define GPS_MASK    (GPS_RESET  | GPS_EN )

#define GPS_GPIO_INIT	FM3_GPIO->PFR3 &=~GPS_MASK; FM3_GPIO->PDOR3 &= ~GPS_MASK; FM3_GPIO->DDR3 |= GPS_MASK;

#define GPS_GPIO_HIGH(x)	FM3_GPIO->PDOR3 |= x;
#define GPS_GPIO_LOW(x) 	FM3_GPIO->PDOR3 &= ~x;
#define GPS_GPIO_TOG(x)		FM3_GPIO->PDOR3 ^= x

/******************* GPS HARDWARE CONFIGURATION ENDS ******************************/
//---------------------------------------------------------------------------------
/******************* GPS CONSTANTS CONFIGURATION STARTS ***************************/
#define GPS_MAX_PKT 3
#define GPS_PKT_BUFF_SIZE 200
#define GPS_VALID_TIMEOUT_SEC 1800
/******************* GPS CONSTANTS CONFIGURATION ENDS *****************************/
//---------------------------------------------------------------------------------
/******************* GPS COMMUNICATION VARIABLE LOCAL TO THE FILE STARTS **********/
struct gpselement	//raw data
{	struct tm TimeStamp;
  	float Lat;		//current in degree & GpsLastLogData in radian
	float Lng;		//current in degree & GpsLastLogData in radian
	ui DistanceFromLastLoc;	//distance from last gps loc to current loc
	ui Altitude;	//in centimeter
	ui Speed;		//in CENTI METER PER SECOND or can be interpreted as m/s with decimal at second last pos
	ui Angle;		//in degree
	uc NoS;			//in nos
	uc AntStat;		//OK= , short= , open= 
};
extern struct gpselement GpsCurData,GpsLastValidData,LastLogGpsData;
extern union union_temp GpsCurDataInRadian;
extern char GpsBuff[4][GPS_PKT_BUFF_SIZE];//GpsBuff packet should be one more then GPS_MAX_PKT else creating problem by sending unknown sms to unknown number while sending command
extern uc GpsPktCntr;
extern boolean_t GpsCommandFlag;
extern boolean_t GpsInterruptRecvDebugFlag;
extern boolean_t GpsPacketValidFlag;
//extern boolean_t GpsRawDataDebugFlag;
extern ui GpsValidTimeOutSec;	//if gps valid is not received within GPS_VALID_TIMEOUT_SEC then gps will restart without any condition.
/******************* GPS COMMUNICATION VARIABLE LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
/******************* GPS COMMUNICATION FUNCTIONS LOCAL TO THE FILE STARTS **********/
extern void ContinueLastLogData();
extern void CheckGpsPacket();
extern void GetLocation(char whichone);
extern void InitGps();
extern void InitUartGps(void);
extern void PutCharGps(uc u8Char);
extern void PutStringGps(char *pu8Buffer);
extern uc GetCharGps(void);
/******************* GPS COMMUNICATION FUNCTIONS LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------

#endif
