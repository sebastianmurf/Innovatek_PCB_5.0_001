#ifndef __ATLAS_REPLACEMENT_H__
#define __ATLAS_REPLACEMENT_H__

#include "common.h"

//---------------------------------------------------------------------------------
/******************* ATLAS_FLOWMETER CONSTANTS CONFIGURATION STARTS ***************************/
#define ATLAS_BUFF_SIZE 250

#define TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION 5

/******************* ATLAS_FLOWMETER CONSTANTS CONFIGURATION ENDS *****************************/
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/******************* ATLAS_FLOWMETER COMMUNICATION VARIABLE LOCAL TO THE FILE STARTS **********/
extern uc AtlasReplacementCommTimeOutSec;
extern uc AtlasReplacementReceiveCounter;
extern char AtlasReplacementBuff[ATLAS_BUFF_SIZE];
extern boolean_t AtlasReplacementReceivePacketCompleteFlag;
extern boolean_t AtlasReplacementDebugFlag;

extern int AtlasTemperature;
extern int RawWaterTemperature;
extern int TemperatureOffsetRegister;

extern int RawWaterConductivity;
//related to flow
extern uc FlowmeterCommTimeOutSec;
struct struct_flowmeter
{	float TotalFlowAfterRestart;
	float CurrentFlow;
	ui K_Factor;
};
extern struct struct_flowmeter FlowMeter;

/******************* ATLAS_FLOWMETER COMMUNICATION VARIABLE LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/******************* ATLAS_FLOWMETER COMMUNICATION FUNCTIONS LOCAL TO THE FILE STARTS **********/
/******************* ATLAS_FLOWMETER COMMUNICATION FUNCTIONS LOCAL TO THE FILE ENDS ************/
extern void ClearAllFlow();
extern void GetAtlasReplacementReading();
extern void GetAtlasReplacementTemperatureReading();
extern void GetAtlasReplacementConductivityReading();
extern void GetAtlasReplacementFlowMeterReading();
extern void InitUartAtlasReplacement();
extern void PutCharAtlasReplacement(uint8_t u8Char);
extern void PutStringAtlasReplacement(char *pu8Buffer);
extern void ResetAverageFlow();
extern void InitAtlasReplacement();
extern void SendAtlasReplacementCommand(char *cmd);
extern void PureAnalogVoltageOut();   
extern void FlowAverageCalc();
	
extern void RecalibrateTemperatureIfNeeded();
//extern void ReadAtlasConductivity();
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
/******************* ATLAS_FLOWMETER HARDWARE CONFIGURATION STARTS ****************************/
#define ATLAS_REPLACEMENT_UART3
#define ATLAS_REPLACEMENT_UART_BAUDRATE 	115200
		
#ifdef ATLAS_REPLACEMENT_UART0
	#define ATLAS_REPLACEMENT_UART			FM3_MFS0_UART
	#define ATLAS_REPLACEMENT_RX_IRQn		MFS0RX_IRQn
	#define ATLAS_REPLACEMENT_UART_PFR		FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02)   // pin 48 P21->SIN0_0, pin 47 P22->SOT0_0
	#define ATLAS_REPLACEMENT_UART_EPFR		FM3_GPIO->EPFR07 |= (1 << 6)        // Use SIN0_0, SOT0_0
	#define ATLAS_REPLACEMENT_RX_IRQHandler	MFS0RX_IRQHandler
#else 
	#ifdef ATLAS_REPLACEMENT_UART1
		#define ATLAS_REPLACEMENT_UART			FM3_MFS1_UART
		#define ATLAS_REPLACEMENT_RX_IRQn		MFS1RX_IRQn
		#define ATLAS_REPLACEMENT_UART_PFR		FM3_GPIO->PFR1 |= (1 << 0x01) | (1 << 0x02)   // pin 35 P11->SIN1_1, pin 36 P12->SOT1_1
		#define ATLAS_REPLACEMENT_UART_EPFR		FM3_GPIO->EPFR07 |= (1 << 11) | (1 << 13)     // Use SIN1_1, SOT1_1
		#define ATLAS_REPLACEMENT_RX_IRQHandler	MFS1RX_IRQHandler
	#else 
		#ifdef ATLAS_REPLACEMENT_UART3
			#define ATLAS_REPLACEMENT_UART			FM3_MFS3_UART
			#define ATLAS_REPLACEMENT_RX_IRQn		MFS3RX_IRQn
			#define ATLAS_REPLACEMENT_UART_PFR		FM3_GPIO->PFR5 |= (1 << 0x00) | (1 << 0x01)	// pin 2 P50->SIN3_1, pin 3 P51->SOT3_1
			#define ATLAS_REPLACEMENT_UART_EPFR		FM3_GPIO->EPFR07 |= (10<<22)				// Use SIN3_1, SOT3_1
			#define ATLAS_REPLACEMENT_RX_IRQHandler	MFS3RX_IRQHandler
		#else
			#ifdef ATLAS_REPLACEMENT_UART4
				#define ATLAS_REPLACEMENT_UART			FM3_MFS4_UART
				#define ATLAS_REPLACEMENT_RX_IRQn		MFS4RX_IRQn
				#define ATLAS_REPLACEMENT_UART_PFR		FM3_GPIO->PFR0 |= (1 << 0x0a) | (1 << 0x0b)   // pin 54 P0A->SIN4_0, pin 55 P0B->SOT4_0
				#define ATLAS_REPLACEMENT_UART_EPFR 	FM3_GPIO->EPFR08 |= (1 << 6)        // Use SIN4_0, SOT4_0
				#define ATLAS_REPLACEMENT_RX_IRQHandler	MFS4RX_IRQHandler
			#else
				#ifdef ATLAS_REPLACEMENT_UART5
					#define ATLAS_REPLACEMENT_UART			FM3_MFS5_UART
					#define ATLAS_REPLACEMENT_RX_IRQn		MFS5RX_IRQn
					#define ATLAS_REPLACEMENT_UART_PFR		FM3_GPIO->PFR6 |= (1 << 0x00) | (1 << 0x01)   //SDK pin 44 P60->SIN5_0, pin 43 P61->SOT5_0
					#define ATLAS_REPLACEMENT_UART_EPFR 	FM3_GPIO->EPFR08 |= (1 << 12)        // Use SIN5_0, SOT5_0
					#define ATLAS_REPLACEMENT_RX_IRQHandler	MFS5RX_IRQHandler
				#endif
			#endif
		#endif
	#endif
#endif

/******************* ATLAS_FLOWMETER HARDWARE CONFIGURATION ENDS ******************************/
//---------------------------------------------------------------------------------


#endif
