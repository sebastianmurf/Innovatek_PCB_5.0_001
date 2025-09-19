#ifndef __SERIAL_LCD_H__
#define __SERIAL_LCD_H__

#include <time.h>
#include "common.h"

//---------------------------------------------------------------------------------
/******************* SERIAL_LCD CONSTANTS CONFIGURATION STARTS ***************************/
#define LCD_CHAR_PER_LINE_WITH_NULL 17

enum DisplayUnitKeys
{	KEY_UP,
	KEY_DOWN,
	KEY_ENTER,
	KEY_EXIT, 
};
enum DisplayPageConstants
{	LOGOUT_0,	
	LOGOUT_1,	
	LOGOUT_2,
	LOGOUT_3,
	PASSWORD_0,	
	PASSWORD_ERR,
	AUTO_0,			//100
	AUTO_1,
	AUTO_0_1,
        AUTO_0_2,
        AUTO_0_3,
        AUTO_0_4,
        AUTO_0_5,
        AUTO_0_6,
        AUTO_0_7,
        AUTO_0_8,
        AUTO_0_9,
        AUTO_0_9_0,
        AUTO_0_9_1,
        AUTO_0_9_2,
        AUTO_0_9_3,
        AUTO_0_9_4,
        AUTO_0_9_5,
        AUTO_0_9_6,
        AUTO_0_9_7,
        AUTO_0_9_8,
        AUTO_0_9_9,
        AUTO_0_9_10,
        AUTO_0_9_11,
        AUTO_0_9_12,
        AUTO_0_9_13,
        AUTO_0_9_14,
        AUTO_0_9_15,
        AUTO_0_9_16,
        AUTO_0_9_17,
        AUTO_0_9_18,
        AUTO_0_9_19,
    
        MENU_0,
	MENU_1,
	//MENU_2,
	MENU_3,
        USER_FUNCTIONS_0,
	USER_FUNCTIONS_0_0,//required to be up coz long jump is not working down below
        USER_FUNCTIONS_0_1,
        USER_FUNCTIONS_1,
	USER_FUNCTIONS_2,
	USER_FUNCTIONS_3,
        USER_FUNCTIONS_4,
        USER_FUNCTIONS_5,
        USER_FUNCTIONS_6,
        USER_FUNCTIONS_7,
        PARA_SET_1,	    //Outlet Conductivity
	PARA_SET_2,		// Prepurify Time
	PARA_SET_3,		// Purify Time	
	PARA_SET_4,		//Waste Time	
	PARA_SET_5,		//HF Waste Time	
	PARA_SET_6,		//CIP Dosing On Time	
	PARA_SET_7,		//CIP Dosing Off Time	
	PARA_SET_8,		//CIP Dosing Total Time	
	PARA_SET_9,		//CIP Flow Pulse Total Time	
	PARA_SET_10,		//CIP Flow Pulse On Time	
	PARA_SET_11, 	//CIP Flow Pulse Off Time		
	PARA_SET_12,	//CIP Flush Time	
	PARA_SET_13,	//WV Check Time
	PARA_SET_14,	//Wait HT Time
	PARA_SET_15,	//Pure Target Flow
	PARA_SET_16,	//Low Flow Purify Alarm
	PARA_SET_17,	//Low Flow waste Alarm
	PARA_SET_18,	//CIP Cycles
	PARA_SET_19,	//Temperature alarm High Setpoint
	PARA_SET_20,	//Max CIP Pump Runtime
	PARA_SET_21,    //Pump P-factor
	PARA_SET_22,    //Dynamic P-factor
        PARA_SET_23,	// Voltage during the pure cycle
	PARA_SET_24,	// Voltage during the waste cycle
        PARA_SET_25, 	// CURRENT DURING WASTE (i.e w_value)
	PARA_SET_26,	//flowmeter k factor
	PARA_SET_27,	//CLEAR flowmeter FLOW RELATED VALUES
        PARA_SET_28,    //SELECT VOLUME UNIT
        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
        PARA_SET_29,    //BYPASS OPTIONS
        #endif
        PARA_SET_30,    //Start Pressure
        PARA_SET_31,    //Stop Pressure 
        PARA_SET_32,    //BYPASS Pressure
        PARA_SET_33,    //Cip Pressure
        PARA_SET_34,    //Time before cip start in sec
        PARA_SET_35,    //Time before bypass alarm in sec
        #if(CVS_CIP==CVS_CIP_NO)
        PARA_SET_36,    //cip lower limit cycle count
        PARA_SET_37,    //Mid time at which CIP is possible
        PARA_SET_38,    //+- MIN of mid time
        #endif
        PARA_SET_39,    //Set manual CapDi
        PARA_SET_40,    //Set manual current pre-pure
        PARA_SET_41,    //Set manual current pure
        PARA_SET_42,    //Set manual current waste
        PARA_SET_43,    //Pre-Charge Shunt
        PARA_SET_44,    //Post-Charge Shunt
        
        PARA_SET_1_0,		
	PARA_SET_2_0,		
	PARA_SET_3_0,		
	PARA_SET_4_0,		
	PARA_SET_5_0,		
	PARA_SET_6_0,		
	PARA_SET_7_0,		
	PARA_SET_8_0,		
	PARA_SET_9_0,		
	PARA_SET_10_0,		
	PARA_SET_11_0,		
	PARA_SET_12_0,		
	PARA_SET_13_0,		
	PARA_SET_14_0,		
	PARA_SET_15_0,		
	PARA_SET_16_0,		
	PARA_SET_17_0,		
	PARA_SET_18_0,		
	PARA_SET_19_0,
	PARA_SET_20_0,
	PARA_SET_21_0,    
	PARA_SET_22_0, 
	PARA_SET_23_0,    
	PARA_SET_24_0, 
        PARA_SET_25_0, 
	PARA_SET_26_0,
        PARA_SET_27_0,
	PARA_SET_28_0,  //VOLUME UNIT = LITTERS
        PARA_SET_28_1,  //VOLUME UNIT = GALLONS
    #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
        PARA_SET_29_0,  //BYPASS = INTERNAL
        PARA_SET_29_1,  //BYPASS = EXTERNAL NC
        PARA_SET_29_2,  //BYPASS = EXTERNAL NO
    #endif
        PARA_SET_30_0,  //start pressure
        PARA_SET_31_0,  //stop  pressure
        PARA_SET_32_0,  //bypassPressure
        PARA_SET_33_0,  //CIP Pressure
        PARA_SET_34_0,  //time before CIP actually starts 
        PARA_SET_35_0,  //time before bypass alarm gets generated 
    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)
        PARA_SET_36_0,    //cip lower limit cycle count
        PARA_SET_37_0,    //Mid time at which CIP is possible
        PARA_SET_38_0,    //+- MIN of mid time
    #endif
        PARA_SET_39_0,
        PARA_SET_40_0,
        PARA_SET_41_0, 
        PARA_SET_42_0,
        PARA_SET_43_0,
        PARA_SET_44_0,
        
        PARA_SET_1_1,		
	PARA_SET_2_1,		
	PARA_SET_3_1,		
	PARA_SET_4_1,		
	PARA_SET_5_1,		
	PARA_SET_6_1,		
	PARA_SET_7_1,		
	PARA_SET_8_1,		
	PARA_SET_9_1,		
	PARA_SET_10_1,		
	PARA_SET_11_1,		
	PARA_SET_12_1,		
	PARA_SET_13_1,		
	PARA_SET_14_1,		
	PARA_SET_15_1,		
	PARA_SET_16_1,		
	PARA_SET_17_1,		
	PARA_SET_18_1,		
	PARA_SET_19_1,
	PARA_SET_20_1,
	PARA_SET_21_1,
	PARA_SET_22_1, 
	PARA_SET_23_1,
	PARA_SET_24_1, 
	PARA_SET_25_1,
	PARA_SET_26_1,
        PARA_SET_30_1, 
	PARA_SET_31_1,
	PARA_SET_32_1,
        PARA_SET_33_1,
        PARA_SET_34_1,  //time before CIP starts 
        PARA_SET_35_1,
    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)
        PARA_SET_36_1,    //cip lower limit cycle count
        PARA_SET_37_1,    //Mid time at which CIP is possible
        PARA_SET_38_1,    //+- MIN of mid time
    #endif
        PARA_SET_39_1,
        PARA_SET_40_1,
        PARA_SET_41_1,
        PARA_SET_42_1,
        PARA_SET_43_1,
        PARA_SET_44_1,
        
        USER_FUNCTIONS_1_0,
        USER_FUNCTIONS_2_0,
	USER_FUNCTIONS_3_0,
	USER_FUNCTIONS_4_0,
	USER_FUNCTIONS_4_1,
	USER_FUNCTIONS_4_0_0,
	USER_FUNCTIONS_4_1_0,
	USER_FUNCTIONS_5_0,
	USER_FUNCTIONS_5_1,
        USER_FUNCTIONS_5_0_0,
	USER_FUNCTIONS_5_1_0,
	USER_FUNCTIONS_6_0,
        USER_FUNCTIONS_6_1,
        USER_FUNCTIONS_6_2,
        USER_FUNCTIONS_7_0,
        USER_FUNCTIONS_2_0_ERROR,
	USER_FUNCTIONS_3_0_ERROR,
        HW_TEST_1,
	HW_TEST_2,
        HW_TEST_3,
	INPUT_TEST_1,
	OUTPUT_TEST_1,
	OUTPUT_TEST_2,
	OUTPUT_TEST_3,
	OUTPUT_TEST_4,
	OUTPUT_TEST_5,
	OUTPUT_TEST_6,
	OUTPUT_TEST_7,
	OUTPUT_TEST_8,
        OUTPUT_TEST_9,
        OUTPUT_TEST_10,         //Pump test
        OUTPUT_TEST_11,         //PAE Enable
        OUTPUT_TEST_12,         //PAE Current
        
	SENSOR_READ_1,
	SENSOR_READ_2,
	SENSOR_READ_3,
	SENSOR_READ_4,
        SENSOR_READ_5,
	SENSOR_READ_1_1,
	SENSOR_READ_2_1,
	SENSOR_READ_3_1,
	SENSOR_READ_4_1,
        SENSOR_READ_5_1,
	DEV_INFO_0,		
	DEV_INFO_1,		
	DEV_INFO_2,		
	DEV_INFO_3,		
	//DEV_INFO_4,		
	DEV_INFO_0_1,		
	DEV_INFO_1_1,		
	DEV_INFO_2_1,		
	DEV_INFO_3_1,		
	//DEV_INFO_4_1,		    
};
/******************* SERIAL_LCD CONSTANTS CONFIGURATION ENDS *****************************/
//---------------------------------------------------------------------------------
#define SERIAL_HMI_BUFFER_SIZE 100
//---------------------------------------------------------------------------------
/******************* SERIAL_LCD COMMUNICATION VARIABLE LOCAL TO THE FILE STARTS **********/
extern char LcdLine1[17];
extern char LcdLine2[17];
extern ui DisplayPage;
extern uc Key;
extern boolean_t KeyPressFlag;
extern char SerialHmiBuff[SERIAL_HMI_BUFFER_SIZE];
extern char SerialHmiReceiveByte;
extern char SerialHmiCrcCodeOut;
extern boolean_t SerialHmiReceivePacketCompleteFlag;
extern boolean_t SerialHmiEchoFlag;
extern uc SerialHmiDebugTarget;
extern char SerialHmiPassword[33];
extern uc CurPos;
extern boolean_t CursorActiveFlag;  


/******************* SERIAL_LCD COMMUNICATION VARIABLE LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/******************* SERIAL_LCD COMMUNICATION FUNCTIONS LOCAL TO THE FILE STARTS **********/
extern void DisplayMenu();
extern void SendSerialHmiPacket();
extern void InitUartSerialHmi(void);
extern void CheckKeyPress();
extern void PutCharSerialHmi(uc u8Char);
extern void PutStringSerialHmi(char *pu8Buffer);
extern uc GetCharSeialLcd(void);
/******************* SERIAL_LCD COMMUNICATION FUNCTIONS LOCAL TO THE FILE ENDS ************/
//---------------------------------------------------------------------------------
/******************* GPS HARDWARE CONFIGURATION STARTS ****************************/
#define SERIAL_LCD_UART0
#define SERIAL_LCD_UART_BAUDRATE 	115200
		
#ifdef SERIAL_LCD_UART0
	#define SERIAL_LCD_UART				FM3_MFS0_UART
	#define SERIAL_LCD_RX_IRQn			MFS0RX_IRQn
	#define SERIAL_LCD_UART_PFR			FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02)   // pin 48 P21->SIN0_0, pin 47 P22->SOT0_0
	#define SERIAL_LCD_UART_EPFR		        FM3_GPIO->EPFR07 |= (1 << 6)        // Use SIN0_0, SOT0_0
	#define SERIAL_LCD_RX_IRQHandler	        MFS0RX_IRQHandler
#else 
	#ifdef SERIAL_LCD_UART1
		#define SERIAL_LCD_UART				FM3_MFS1_UART
		#define SERIAL_LCD_RX_IRQn			MFS1RX_IRQn
		#define SERIAL_LCD_UART_PFR			FM3_GPIO->PFR1 |= (1 << 0x01) | (1 << 0x02)   // pin 35 P11->SIN1_1, pin 36 P12->SOT1_1
		#define SERIAL_LCD_UART_EPFR		FM3_GPIO->EPFR07 |= (1 << 11) | (1 << 13)     // Use SIN1_1, SOT1_1
		#define SERIAL_LCD_RX_IRQHandler	MFS1RX_IRQHandler
	#else 
		#ifdef SERIAL_LCD_UART3
			#define SERIAL_LCD_UART				FM3_MFS3_UART
			#define SERIAL_LCD_RX_IRQn			MFS3RX_IRQn
			#define SERIAL_LCD_UART_PFR			FM3_GPIO->PFR5 |= (1 << 0x00) | (1 << 0x01)	// pin 2 P50->SIN3_1, pin 3 P51->SOT3_1
			#define SERIAL_LCD_UART_EPFR		FM3_GPIO->EPFR07 |= (10<<22)				// Use SIN3_1, SOT3_1
			#define SERIAL_LCD_RX_IRQHandler	MFS3RX_IRQHandler
		#else
			#ifdef SERIAL_LCD_UART4
				#define SERIAL_LCD_UART				FM3_MFS4_UART
				#define SERIAL_LCD_RX_IRQn			MFS4RX_IRQn
				#define SERIAL_LCD_UART_PFR			FM3_GPIO->PFR0 |= (1 << 0x0a) | (1 << 0x0b)   // pin 54 P0A->SIN4_0, pin 55 P0B->SOT4_0
				#define SERIAL_LCD_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 6)        // Use SIN4_0, SOT4_0
				#define SERIAL_LCD_RX_IRQHandler	MFS4RX_IRQHandler
			#else
				#ifdef SERIAL_LCD_UART5
					#define SERIAL_LCD_UART				FM3_MFS5_UART
					#define SERIAL_LCD_RX_IRQn			MFS5RX_IRQn
					#define SERIAL_LCD_UART_PFR			FM3_GPIO->PFR6 |= (1 << 0x00) | (1 << 0x01)   //SDK pin 44 P60->SIN5_0, pin 43 P61->SOT5_0
					#define SERIAL_LCD_UART_EPFR 		FM3_GPIO->EPFR08 |= (1 << 12)        // Use SIN5_0, SOT5_0
					#define SERIAL_LCD_RX_IRQHandler	MFS5RX_IRQHandler
				#endif
			#endif
		#endif
	#endif
#endif

/******************* SERIAL_LCD HARDWARE CONFIGURATION ENDS ******************************/
//---------------------------------------------------------------------------------

#endif
