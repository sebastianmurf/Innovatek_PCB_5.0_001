#include "SerialHmi.h"
#include"Delay.h"
#include"Variable.h"
#include"constants.c"
#include"command.h"
#include"myfun.h"
#include"AT45DB161.h"
#include"gprs.h"
#include"gpio.h"
#include<string.h>
#include"DualTimer.h"
#include"DS18B20.h"
#include "AtlasReplacement.h"
#include"i2c.h"
#include"PAE800.h"
#include "Atlas_TC.h"
//#include "Atlas_T.h"
#include "GprsUart.h"
#include "ProjectFunc.h"
#include "PAE800.h"
#include <ppg.h>
//just for testing
#include"MyUsb.h"

//variable local to the file
char LcdLine1[LCD_CHAR_PER_LINE_WITH_NULL];
char LcdLine2[LCD_CHAR_PER_LINE_WITH_NULL];
ui DisplayPage = LOGOUT_0;
char SerialHmiBuff[SERIAL_HMI_BUFFER_SIZE];
char SerialHmiReceiveByte;
char SerialHmiCrcCodeOut=0;
uc Key;
uc KeyPressCounter;
boolean_t KeyPressFlag;
uc SerialHmiDebugTarget = 0; //0=none, 1=USB, 2=SERVER
uc CurPos;
boolean_t CursorActiveFlag = FALSE;  
boolean_t SerialHmiReceivePacketCompleteFlag = FALSE;
boolean_t SerialHmiEchoFlag = FALSE;
char SerialHmiPassword[33];
char SerialHmiTemp;
//ui KeyTimeOut;
uint8_t wifi_test_fail_cntr;//if fail more than 3 times, declare as fail
///////////////////////////////////////////////////////////////////////////////
//function local to the file
void DisplayMenu();
void SendSerialHmiPacket();
void InitUartSerialHmi();
void PutCharSerialHmi(uint8_t u8Char);
void PutStringSerialHmi(char *pu8Buffer);
ui UserInputToUnsignedInt();
void VariableUnsignedInt2UserInput(ui var);
////////////////////////////////////////////////////////////////////////////////
void CheckSerialHmiKeyInput()
{	if(SerialHmiReceivePacketCompleteFlag == TRUE)
    {   if(SearchString(SerialHmiBuff,"ENTER")>0)
		{	KeyPressFlag = TRUE;
			Key = KEY_ENTER;
		}
		else if(SearchString(SerialHmiBuff,"UP")>0)
		{	KeyPressFlag = TRUE;
			Key = KEY_UP;
		}
		else if(SearchString(SerialHmiBuff,"DOWN")>0)
		{	KeyPressFlag = TRUE;
			Key = KEY_DOWN;
		}
		else if(SearchString(SerialHmiBuff,"ESC")>0)
		{	KeyPressFlag = TRUE;
			Key = KEY_EXIT;
		}
		SerialHmiReceivePacketCompleteFlag = FALSE;
		if(SerialHmiDebugTarget > 0)
		{	PutStringUsb("\r\nHMI -> ");
			PutStringUsb(SerialHmiBuff);
		}
	}
}

void DisplayAlarmStatus(uc alarm_no)
{	switch(alarm_no)
	{	case LOW_FLOW_PURIFY_ALARM_FLAG:	        strcpy(LcdLine1,"LOW PURIFY  : No");break;
		case LOW_FLOW_WASTE_ALARM_FLAG:		        strcpy(LcdLine1,"LOW WASTE   : No");break;
		case BYPASS_ALARM_FLAG:				strcpy(LcdLine1,"BYPASS ALARM: No");break;
		case CABINET_TEMP_ALARM_FLAG:		        strcpy(LcdLine1,"CAB.TEMP    : No");break;
		case LEAKAGE_ALARM_FLAG:			strcpy(LcdLine1,"LEAKAGE     : No");break;
		case WASTE_VALUE_ALARM_FLAG:		        strcpy(LcdLine1,"WASTE VALVE : No");break;
		case CIP_LOW_LEVEL_ALARM_FLAG:		        strcpy(LcdLine1,"CIP L.LEVEL : No");
											if(CIP_PumpRunTimeInSec==0){
												LcdLine1[13] = 'N';
												LcdLine1[14] = '.';
												LcdLine1[15] = 'E';
											}
											break;
		case PAE_COMM_ERROR_FLAG:			strcpy(LcdLine1,"PAE COMM ERR: No");break;
		case PAE_OVP_SHUTDOWN_FLAG:			strcpy(LcdLine1,"PAE OVP SHUT: No");break;
		case PAE_OLP_SHUTDOWN_FLAG:			strcpy(LcdLine1,"PAE OLP SHUT: No");break;
		case PAE_OTP_SHUTDOWN_FLAG:			strcpy(LcdLine1,"PAE OTP SHUT: No");break;
		case PAE_FAN_FAIL_FLAG:				strcpy(LcdLine1,"PAE FAN FAIL: No");break;
		case PAE_AUX_OR_SMPS_FAIL_FLAG:		        strcpy(LcdLine1,"PAE SMPS ERR: No");break;
		case PAE_HI_TEMP_FLAG:				strcpy(LcdLine1,"PAE HI TEMP : No");break;
		case PAE_AC_POWER_DOWN_FLAG:		        strcpy(LcdLine1,"PAE AC PDOWN: No");break;
		case PAE_AC_INPUT_FAIL_FLAG:		        strcpy(LcdLine1,"PAE AC IP FL: No");break;
		case LOW_PRESSURE_ALARM_FLAG:		        strcpy(LcdLine1,"LOW PRESSURE: No");break;
		case ATLAS_I2C_COMM_ERROR_FLAG:		        strcpy(LcdLine1,"ATLAS I2C ER: No");break;
		case ATLAS_CONDUCTIVITY_ZERO_READ_FLAG:		strcpy(LcdLine1,"ZERO EC ERR : No");break;
                case FLOWMETER_COMM_ALARM_FLAG:                 strcpy(LcdLine1,"FLOW COMM ER: No");break;
    }
											
	if((AlarmRegister24Bit & (1<<alarm_no)) > 0)
	{	LcdLine1[13] = 'Y';
		LcdLine1[14] = 'e';
		LcdLine1[15] = 's';
	}
	DirectMemRead(ALARM_TABLE_START_ADDRESS + alarm_no * ALARM_TABLE_RECORD_SIZE, PasteBuff, ALARM_TABLE_RECORD_SIZE);
	sprintf(CopyBuff,"%d/%d/%d %d:%d",
			PasteBuff[0],//YEAR
			PasteBuff[1],//MONTH
			PasteBuff[2],//DATE
			PasteBuff[3],//HOUR
			PasteBuff[4]);//MIN
	CopyBuff[16] = '\0';
	JustifyCenterForLcdLine(CopyBuff,LcdLine2);
	if(alarm_no > 0)
		LcdLine2[0] = 0x7F;
	if(alarm_no < (MAX_NO_OF_ALARMS-1))
		LcdLine2[15] = 0x7E;
	CursorActiveFlag = FALSE;
}
void DisplayBothArrowAndNoCursor()
{	LcdLine2[0] = 0x7F;
	LcdLine2[15] = 0x7E;
	CursorActiveFlag = FALSE; 
}
void DisplayIllegalValuMsg()
{	strcpy(LcdLine2," Illegal Value! ");
	CursorActiveFlag = FALSE;
}
void DisplayMenu()
{	switch(DisplayPage){
		case LOGOUT_0:			
                                #if(MANUFACTURER==MANUFACTURER_VOLTEA)
                                    strcpy(LcdLine1,"     VOLTEA     ");
                                    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        strcpy(LcdLine2,"     DiUse      "); 
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                        strcpy(LcdLine2,"    DiEntry     "); 
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                        strcpy(LcdLine2,"    DiSeries    "); 
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_CFO)
                                        strcpy(LcdLine2,"      DiCFO     "); 
                                    #endif
                                #else
                                      strcpy(LcdLine1,"     Electra    ");
                                      #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        strcpy(LcdLine2,"    Flex Mini   "); 
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                        strcpy(LcdLine2,"    Flex F1     "); 
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                        strcpy(LcdLine2,"    DiSeries    "); 
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_CFO)
                                        strcpy(LcdLine2,"      DiCFO     "); 
                                    #endif
                                #endif
                                
                                //LcdLine2[0] = 0x7F;
                                  LcdLine2[15] = 0x7E;
                                  CursorActiveFlag = FALSE; 
                                  break;
		case LOGOUT_1:			
                                #if(MANUFACTURER==MANUFACTURER_VOLTEA)
                                    strcpy(LcdLine1,"     VOLTEA     ");
                                #else
                                    strcpy(LcdLine1,"     Electra    ");
                                #endif
                                strcpy(LcdLine2,"   Device Info  ");
								DisplayBothArrowAndNoCursor();
								break;
		case LOGOUT_2:			
                                #if(MANUFACTURER==MANUFACTURER_VOLTEA)
                                    strcpy(LcdLine1,"     VOLTEA     ");
                                #else
                                    strcpy(LcdLine1,"     Electra    ");
                                #endif
                                strcpy(LcdLine2,"     System     ");
								DisplayBothArrowAndNoCursor();
								break;
		case LOGOUT_3:			
                                #if(MANUFACTURER==MANUFACTURER_VOLTEA)
                                    strcpy(LcdLine1,"     VOLTEA     ");
                                #else
                                    strcpy(LcdLine1,"     Electra    ");
                                #endif
                                strcpy(LcdLine2," User Functions ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		/*case PROCESS:			strcpy(LcdLine1,"     VOLTEA     ");
								strcpy(LcdLine2," Start Process  ");
								DisplayBothArrowAndNoCursor();
								break;
		*/
		case PASSWORD_0:		strcpy(LcdLine1,"    PASSWORD    ");
								strcpy(LcdLine2,"      ----      ");
								//LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								switch(KeyPressCounter){
										case 0:	CurPos = 206; LcdLine2[6]= SerialHmiPassword[0]; break;
										case 1: CurPos = 207; LcdLine2[6]='x';LcdLine2[7]=SerialHmiPassword[1];break;
										case 2: CurPos = 208; LcdLine2[6]='x';LcdLine2[7]='x';LcdLine2[8]=SerialHmiPassword[2];break;
										case 3: CurPos = 209; LcdLine2[6]='x';LcdLine2[7]='x';LcdLine2[8]='x';LcdLine2[9]=SerialHmiPassword[3];break;
								}
								CursorActiveFlag = TRUE; 
								break;
		case PASSWORD_ERR:		strcpy(LcdLine1,"    PASSWORD    ");
								strcpy(LcdLine2,"      ERROR     ");
								CursorActiveFlag = FALSE; 
								break;
		case DEV_INFO_0:		strcpy(LcdLine1,"   DEVICE INFO  ");
								strcpy(LcdLine2,"  Product Name  ");
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case DEV_INFO_1:		strcpy(LcdLine1,"   DEVICE INFO  ");
								strcpy(LcdLine2,"  Product Model ");
								DisplayBothArrowAndNoCursor();
								break;
		case DEV_INFO_2:		strcpy(LcdLine1,"   DEVICE INFO  ");
								strcpy(LcdLine2," Version Number ");
								DisplayBothArrowAndNoCursor();
								break;
		case DEV_INFO_3:		strcpy(LcdLine1,"   DEVICE INFO  ");
								strcpy(LcdLine2,"  Serial Number ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
        /*
		case DEV_INFO_4:		strcpy(LcdLine1,"   DEVICE INFO  ");
								strcpy(LcdLine2,"   IMEI Number  ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                                */
		case DEV_INFO_0_1:		strcpy(LcdLine1,"  PRODUCT NAME  ");
								strcpy(LcdLine2,PRODUCT_NAME);
								//LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
                                                                break;
		case DEV_INFO_1_1:		strcpy(LcdLine1,"  PRODUCT MODEL ");
								strcpy(LcdLine2,MODEL_NO);
								//LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case DEV_INFO_2_1:		strcpy(LcdLine1," VERSION NUMBER ");
								strcpy(LcdLine2,DEVICE_VERSION);
								//LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case DEV_INFO_3_1:		strcpy(LcdLine1,"  SERIAL NUMBER ");
								/*ReadSerialInfoInCopyBuff();
								u.c[0] = CopyBuff[1];
								u.c[1] = CopyBuff[2];
								u.c[2] = CopyBuff[3];
								u.c[3] = CopyBuff[4];
								sprintf(CopyBuff,"%d",u.i[0]);
								*/
                                                                sprintf(CopyBuff,"%d",Device.SerialNo);
								JustifyCenterForLcdLine(CopyBuff,LcdLine2);
								//LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
        /*                        
		case DEV_INFO_4_1:		strcpy(LcdLine1,"   IMEI NUMBER  ");
								sprintf(CopyBuff,"%lld",GprsPacket.Imei);
								JustifyCenterForLcdLine(CopyBuff,LcdLine2);
								//LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		*/
		case MENU_1:			strcpy(LcdLine1,"      MENU      ");
								strcpy(LcdLine2,"    Setpoints   ");   
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		/*case MENU_2:			strcpy(LcdLine1,"      MENU      ");
								strcpy(LcdLine2," System Setting ");
								LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                                */                
		case MENU_3:			strcpy(LcdLine1,"      MENU      ");
								strcpy(LcdLine2," Hardware Test  ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                
		case PARA_SET_1:		//Outlet Conductivity
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Pure EC Target ");
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case PARA_SET_2:		// Prepurify Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," PrePurify Time ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_3:		// Purify Time	
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"  Purify Time   ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_4:		//Waste Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"   Waste Time   ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_5:		//HF Waste Time
								strcpy(LcdLine1,"   SETPOINTS    ");
                                                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"  HF Waste Time ");
                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2,"   Check Time   ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2,"   Check Time   ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_6:		//CIP Dosing On Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"  CIP Step 1A   ");
                                                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2,"    CIP Dose    ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2,"    CIP Dose    ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_7:		//CIP Dosing Off Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"  CIP Step 1B   "); 
                                                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2,"  CIP Dose Rec  ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2,"  CIP Dose Rec  ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_8:		//CIP Dosing Total Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"   CIP Step 1   ");
                                                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2," CIP Dose Total ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2," CIP Dose Total ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_9:		//CIP Flow Pulse Total Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"   CIP Step 2   ");
                                                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2," CIP Flow Total ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2," CIP Flow Total ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;						
		case PARA_SET_10:		//CIP Flow Pulse On Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"  CIP Step 2A   ");
                                                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2," CIP Flow Flush ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2," CIP Flow Flush ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_11:		//CIP Flow Pulse Off Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                                strcpy(LcdLine2,"  CIP Step 2B   ");
                                                                                                #endif
                                                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                                strcpy(LcdLine2,"  CIP Flow Rec  ");
                                                                #endif
                                                                                                #if(PRODUCT_TYPE==PRODUCT_DISERIES)
                                                                strcpy(LcdLine2,"  CIP Flow Rec  ");
                                                                #endif
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_12:		//CIP Flush Time	
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," CIP Flush Time ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_13:		//WV Check Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"  WV Check Time ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_14:		//Wait HT Time
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"  Wait HT Time  "); 
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_15:		//Pure Target Flow
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," P.Flow Target  ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_16:		//Low Flow Purify Alarm
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Low Flo P.Alrm ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_17:		//Low Flow waste Alarm
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Low Flo W.Alrm ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_18:		//CIP Cycles
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Max CIP Cycles ");
								DisplayBothArrowAndNoCursor();
								break;
		case PARA_SET_19:		//Temperature alarm High Setpoint
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"   Temp Alarm   ");
								DisplayBothArrowAndNoCursor();
								break;	        
		case PARA_SET_20:		//Max CIP Pump Runtime
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," MAX CIP P.R.T  ");
								DisplayBothArrowAndNoCursor();
								break;	
		case PARA_SET_21:		//Pump P-factor
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Pump P-factor  ");
								DisplayBothArrowAndNoCursor();
								break;	
		case PARA_SET_22:		//Dynamic P-factor
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Dyna P-factor  ");
								DisplayBothArrowAndNoCursor();
								break;	
		/*************************** Added by Arun Amatya Nov 14 *************************/
                case PARA_SET_23:		// Voltage during the pure cycle
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"   P.Max Volt   ");
								DisplayBothArrowAndNoCursor();
								break;	
		case PARA_SET_24:		// Voltage during the waste cycle
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"   W.Max Volt   ");
								DisplayBothArrowAndNoCursor();
								break;	
                case PARA_SET_25:		// CURRENT DURING WASTE (i.e w_value)
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"    w_value     ");
								DisplayBothArrowAndNoCursor();
								break;	
		case PARA_SET_26:		// Flowmeter K factor
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Flow K Factor  ");
								DisplayBothArrowAndNoCursor();
								break;
                case PARA_SET_27:		//Reset Pure Volume
								strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2," Clear Pure Vol ");
								DisplayBothArrowAndNoCursor();
								break;
                case PARA_SET_28:              //SELECT VOLUME UNIT
                                                                strcpy(LcdLine1,"   SETPOINTS    ");
								strcpy(LcdLine2,"  Volume Unit   ");
								DisplayBothArrowAndNoCursor();
								break;
                #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                case PARA_SET_29:       //bypass option
                                        strcpy(LcdLine1,"   SETPOINTS    ");
                                                                        strcpy(LcdLine2," BYPASS Options ");
                                                                        DisplayBothArrowAndNoCursor();
                                                                        break;
                #endif
                case PARA_SET_30:                      //start pressure
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," Start Pressure ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_31:                     //stop pressure
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," Stop  Pressure ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_32:                    //bypass pressure
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," BypassPressure ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_33:                   //cip pressure
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," CIP   Pressure ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_34:		    //time before CIP starts
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," CIP Wait Time  ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_35:		   //time before CIP starts
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," BYP Alarm Wait ");
                                                          #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)
                                                          DisplayBothArrowAndNoCursor();
                                                          #else
                                                          LcdLine2[0] = 0x7F;
                                                          //LcdLine2[15] = 0x7E;
                                                          #endif
                                                          break;
                #if(CVS_CIP==CVS_CIP_NO)
                case PARA_SET_36:		//time before CIP starts
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," Min CIP Cycles ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_37:		//time before CIP starts
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2,"  CIP Set Time  ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_38:		//time before CIP starts
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," CIP Grace Time ");
                                                          LcdLine2[0] = 0x7F;
                                                          //LcdLine2[15] = 0x7E;
                                                          DisplayBothArrowAndNoCursor();
                                                          CursorActiveFlag = FALSE; 
                                                          break;
                #endif
                case PARA_SET_39:		// Current during pre-pure cycle
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," Dynamic CapDI ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;	                                                                        
                case PARA_SET_40:               //Current during pure cycle
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2," P-Pure Current");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_41:               //Current during waste cycle
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2,"  Pure Current  ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_42:               //Current during waste cycle
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2,"  Waste Current ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_43:               //Current during waste cycle
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2,"  Pre-CHG SHNT  ");
                                                          DisplayBothArrowAndNoCursor();
                                                          break;
                case PARA_SET_44:               //Current during waste cycle
                                                          strcpy(LcdLine1,"   SETPOINTS    ");
                                                          strcpy(LcdLine2,"  Post-CHG SHNT ");
                                                          LcdLine2[0] = 0x7F;                                                         
                                                          //DisplayBothArrowAndNoCursor();
                                                          break;
                /*********************************************************************************/								
                case PARA_SET_1_0:		strcpy(LcdLine1," PURE EC TARGET ");
                                                                //strcpy(LcdLine2," 0000 ???? 2000 ");
                                                                sprintf(LcdLine2," %04d ???? %04d ",PURE_EC_TARGET_MIN_VAL, PURE_EC_TARGET_MAX_VAL);
                                                                LcdLine2[6]= SerialHmiPassword[0];
                                                                LcdLine2[7]=SerialHmiPassword[1];
                                                                LcdLine2[8]=SerialHmiPassword[2];
                                                                LcdLine2[9]=SerialHmiPassword[3];
                                                                switch(KeyPressCounter){
                                                                                case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
                                                                                case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
                                                                                case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
                                                                                case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
                                                                                default: CursorActiveFlag = FALSE;break; 
                                                                }
                                                                break;
		case PARA_SET_2_0:		strcpy(LcdLine1," PREPURIFY TIME ");
								//strcpy(LcdLine2," 0002 ???? 0030 ");
								sprintf(LcdLine2," %04d ???? %04d ",PREPURIFY_TIME_MIN_VAL, PREPURIFY_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								
								break;
		case PARA_SET_3_0:		strcpy(LcdLine1,"   PURIFY TIME  ");
								//strcpy(LcdLine2," 0010 ???? 0500 ");
								sprintf(LcdLine2," %04d ???? %04d ",PURIFY_TIME_MIN_VAL, PURIFY_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_4_0:		strcpy(LcdLine1,"   WASTE TIME   ");
                                                                //strcpy(LcdLine2," 0010 ???? 0500 ");
                                                                sprintf(LcdLine2," %04d ???? %04d ",WASTE_TIME_MIN_VAL, WASTE_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_5_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                strcpy(LcdLine1,"  HF WASTE TIME ");
                                #endif
                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                strcpy(LcdLine1,"   CHECK TIME   ");
                                #endif
								//strcpy(LcdLine2," 0000 ???? 0500 ");
								sprintf(LcdLine2," %04d ???? %04d ",HF_WASTE_TIME_MIN_VAL, HF_WASTE_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_6_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                strcpy(LcdLine1,"  CIP STEP 1A   ");
                                #endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                strcpy(LcdLine1,"    CIP DOSE    ");
                                #endif
								//strcpy(LcdLine2," 0001 ???? 0200 ");
                                sprintf(LcdLine2," %04d ???? %04d ",CIP_DOSE_ON_TIME_MIN_VAL, CIP_DOSE_ON_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_7_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                strcpy(LcdLine1,"  CIP STEP 1B   ");
								//strcpy(LcdLine2," 0001 ???? 0200 ");
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                strcpy(LcdLine1,"  CIP DOSE REC  ");
                                //strcpy(LcdLine2," 0001 ???? 2000 ");
								#endif
								sprintf(LcdLine2," %04d ???? %04d ",CIP_DOSE_OFF_TIME_MIN_VAL, CIP_DOSE_OFF_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_8_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                strcpy(LcdLine1,"   CIP STEP 1   ");
								//strcpy(LcdLine2," 0001 ???? 0600 ");
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                strcpy(LcdLine1," CIP DOSE TOTAL ");
                                //strcpy(LcdLine2," 0001 ???? 2000 ");
								#endif
								sprintf(LcdLine2," %04d ???? %04d ",CIP_DOSE_TOTAL_TIME_MIN_VAL, CIP_DOSE_TOTAL_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_9_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                strcpy(LcdLine1,"   CIP STEP 2   ");
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                strcpy(LcdLine1," CIP FLOW TOTAL ");
                                #endif
								//strcpy(LcdLine2," 0000 ???? 7200 ");
								sprintf(LcdLine2," %04d ???? %04d ",CIP_PULSE_TOTAL_TIME_MIN_VAL, CIP_PULSE_TOTAL_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_10_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                strcpy(LcdLine1,"  CIP STEP 2A   ");
								//strcpy(LcdLine2," 0001 ???? 0020 ");
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                strcpy(LcdLine1," CIP FLOW FLUSH ");
                                //strcpy(LcdLine2," 0000 ???? 0240 ");
								#endif
								sprintf(LcdLine2," %04d ???? %04d ",CIP_PULSE_ON_TIME_MIN_VAL, CIP_PULSE_ON_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_11_0:		
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                    strcpy(LcdLine1,"  CIP STEP 2B   ");
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                    strcpy(LcdLine1,"  CIP FLOW REC  ");
                                #endif
								//strcpy(LcdLine2," 0000 ???? 0060 ");
								sprintf(LcdLine2," %04d ???? %04d ",CIP_PULSE_OFF_TIME_MIN_VAL, CIP_PULSE_OFF_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_12_0:		strcpy(LcdLine1," CIP FLUSH TIME ");
								//strcpy(LcdLine2," 0050 ???? 3600 ");
								sprintf(LcdLine2," %04d ???? %04d ",CIP_FLUSH_TIME_MIN_VAL, CIP_FLUSH_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_13_0:		strcpy(LcdLine1,"  WV CHECK TIME ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)      
                                //strcpy(LcdLine2," 0010 ???? 0060 ");
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)      
                                //strcpy(LcdLine2," 0005 ???? 0080 ");
								#endif
								sprintf(LcdLine2," %04d ???? %04d ",WV_CHECK_TIME_MIN_VAL, WV_CHECK_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_14_0:		strcpy(LcdLine1,"  WAIT HT TIME  ");
								//strcpy(LcdLine2," 0030 ???? 3600 ");
								sprintf(LcdLine2," %04d ???? %04d ",WAIT_HT_TIME_MIN_VAL, WAIT_HT_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_15_0:		strcpy(LcdLine1," P.FLOW TARGET  ");
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                strcpy(LcdLine2," 00.0 ??.? 10.0 ");  
                                #endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                strcpy(LcdLine2," 00.0 ??.? 90.0 ");  
                                #endif
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										//case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_16_0:		strcpy(LcdLine1," LOW FLO P.ALRM ");
								strcpy(LcdLine2," 00.0 ??.? 10.0 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										//case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_17_0:		strcpy(LcdLine1," LOW FLO W.ALRM ");
								strcpy(LcdLine2," 00.0 ??.? 10.0 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										//case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_18_0:		strcpy(LcdLine1,"   CIP CYCLES   ");
                                sprintf(LcdLine2," %04d ???? %04d ",MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL, MAX_CYCLE_COUNT_FOR_CIP_MAX_VAL);
								//strcpy(LcdLine2," 0005 ???? 5000 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_19_0:		strcpy(LcdLine1,"   TEMP ALARM   ");
								//strcpy(LcdLine2," 0020 ???? 0060 "); 
								sprintf(LcdLine2," %04d ???? %04d ",CABINET_HIGH_TEMP_MIN_VAL, CABINET_HIGH_TEMP_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_20_0:		strcpy(LcdLine1,"  MAX CIP P.R.T ");
								#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                //strcpy(LcdLine2," 0005 ???? 2500 "); 
								#endif
								#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                //strcpy(LcdLine2," 0005 ???? 4000 "); 
								#endif
								sprintf(LcdLine2," %04d ???? %04d ",MAX_CIP_PUMP_RUN_TIME_MIN_VAL, MAX_CIP_PUMP_RUN_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_21_0:		strcpy(LcdLine1,"  PUMP P-FACTOR ");
								strcpy(LcdLine2," 0.25 ?.?? 2.00 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_22_0:		strcpy(LcdLine1,"  DYNA P-FACTOR ");
								strcpy(LcdLine2," 0.00 ?.?? 9.99 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		/**************************** ADDED BY ARUN NOV 14 **********************************/
                case PARA_SET_23_0:		strcpy(LcdLine1,"   P.MAX VOLT   ");
								strcpy(LcdLine2," 00.0 ??.? 12.0 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										//case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_24_0:		strcpy(LcdLine1,"   W.MAX VOLT   ");
								strcpy(LcdLine2," 00.0 ??.? 12.0 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										//case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_25_0:		strcpy(LcdLine1,"     W_VALUE    ");
								strcpy(LcdLine2," 0.50 ?.?? 2.00 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
		case PARA_SET_26_0:		strcpy(LcdLine1," Flow K Factor  ");
								//strcpy(LcdLine2," 0010 ???? 2000 "); 
								sprintf(LcdLine2," %04d ???? %04d ",FLOWMETER_K_FACTOR_MIN_VAL, FLOWMETER_K_FACTOR_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_27_0:		strcpy(LcdLine1," Press Enter To ");
								strcpy(LcdLine2,"Clear PureVolume"); 
								break;
                case PARA_SET_28_0:     //SELECT VOLUME UNIT
                                                strcpy(LcdLine1,"  VOLUME UNIT?  ");
								strcpy(LcdLine2,"   =in Litre    ");
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                case PARA_SET_28_1:     //SELECT VOLUME UNIT
                                                strcpy(LcdLine1,"  VOLUME UNIT?  ");
								strcpy(LcdLine2,"   =in Gallon   ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                case PARA_SET_29_0:     //SELECT BYPASS OPTIONS
                                strcpy(LcdLine1," BYPASS OPTIONS ");
								strcpy(LcdLine2,"   = INTERNAL   ");
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case PARA_SET_29_1:     //SELECT BYPASS OPTIONS
                                strcpy(LcdLine1," BYPASS OPTIONS ");
								strcpy(LcdLine2,"  = EXTERNAL NC ");
								LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
		                        break;
		case PARA_SET_29_2:     //SELECT BYPASS OPTIONS
                                strcpy(LcdLine1," BYPASS OPTIONS ");
								strcpy(LcdLine2,"  = EXTERNAL NO ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                #endif
                case PARA_SET_30_0:     //START PRESSURE
                                                strcpy(LcdLine1," START PRESSURE ");
								strcpy(LcdLine2," 0.50 ?.?? 5.90 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_31_0:     //START PRESSURE
                                                strcpy(LcdLine1," STOP  PRESSURE ");
								strcpy(LcdLine2," 0.50 ?.?? 5.90 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_32_0:     //START PRESSURE
                                                strcpy(LcdLine1,"BYPASS PRESSURE ");
								strcpy(LcdLine2," 0.00 ?.?? 5.90 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_33_0:     //START PRESSURE
                                                strcpy(LcdLine1," CIP   PRESSURE ");
								strcpy(LcdLine2," 0.50 ?.?? 5.90 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										//case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_34_0:		strcpy(LcdLine1,"WAIT TIME B4 CIP");
                                                //strcpy(LcdLine2," 0000 ???? 9999 ");
								sprintf(LcdLine2," %04d ???? %04d",CIP_WAIT_TIME_MIN_VAL, CIP_WAIT_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_35_0:		strcpy(LcdLine1,"WAIT B4 BY ALARM");
                                                //strcpy(LcdLine2," 0000 ???? 9999 ");
								sprintf(LcdLine2," %04d ???? %04d",WAIT_TIME_B4_BYPASS_ALARM_MIN_VAL, WAIT_TIME_B4_BYPASS_ALARM_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                #if(CVS_CIP==CVS_CIP_NO)
                case PARA_SET_36_0:		strcpy(LcdLine1," MIN CIP CYCLES ");
                                                //strcpy(LcdLine2," 0050 ???? 1000 ");
								sprintf(LcdLine2," %04d ???? %04d",CIP_LOWER_LIMIT_CYCLE_MIN_VAL, CIP_LOWER_LIMIT_CYCLE_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_37_0:		strcpy(LcdLine1," CIP SET TIME  ");
                                                //strcpy(LcdLine2," 0000 ???? 2359 ");
								sprintf(LcdLine2," %04d ???? %04d",CIP_FAVORABLE_TIME_MIN_VAL, CIP_FAVORABLE_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_38_0:		strcpy(LcdLine1," CIP GRACE TIME ");
                                                //strcpy(LcdLine2," 0005 ???? 0030 ");
								sprintf(LcdLine2," %04d ???? %04d",CIP_FAVORABLE_TIME_GRACE_MINUTES_MIN_VAL, CIP_FAVORABLE_TIME_GRACE_MINUTES_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;               
                #endif    
                case PARA_SET_39_0:     //SELECT DYNAMIC MODE ON
                                                strcpy(LcdLine1,"  DYNAMIC CAPDI ");
								strcpy(LcdLine2,"       ON       ");
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
                                                                //DisplayBothArrowAndNoCursor();
								CursorActiveFlag = FALSE; 
								break;
                case PARA_SET_39_1:     //SELECT DYNAMIC MODE OFF
                                                strcpy(LcdLine1,"  DYNAMIC CAPDI ");
								strcpy(LcdLine2,"       OFF      ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                case PARA_SET_40_0:		strcpy(LcdLine1," P-PURE CURRENT ");
								strcpy(LcdLine2," 005  ????  125 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;  
                case PARA_SET_41_0:		strcpy(LcdLine1,"   PURE CURRENT ");
								strcpy(LcdLine2," 005  ????  125 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;  
                case PARA_SET_42_0:		strcpy(LcdLine1,"  WASTE CURRENT ");
								strcpy(LcdLine2," 005  ????  125 "); 
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]= SerialHmiPassword[1];;
								LcdLine2[8]= SerialHmiPassword[2];
								LcdLine2[9]= SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								break;
                case PARA_SET_43_0:		strcpy(LcdLine1,"  PRE-CHG TIME  ");
								//strcpy(LcdLine2," 0002 ???? 0030 ");
								sprintf(LcdLine2," %04d ???? %04d ",PREPURIFY_TIME_MIN_VAL, PREPURIFY_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								
								break;
                case PARA_SET_44_0:		strcpy(LcdLine1,"  POST-CHG TIME ");
								//strcpy(LcdLine2," 0002 ???? 0030 ");
								sprintf(LcdLine2," %04d ???? %04d ",PREPURIFY_TIME_MIN_VAL, PREPURIFY_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								
								break;
		/*******************************************************************************************/
		case PARA_SET_1_1:		strcpy(LcdLine1," PURE EC TARGET ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_2_1:		strcpy(LcdLine1," PREPURIFY TIME ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_3_1:		strcpy(LcdLine1,"   PURIFY TIME  ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_4_1:		strcpy(LcdLine1,"   WASTE TIME   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_5_1:		strcpy(LcdLine1,"  HF WASTE TIME ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_6_1:		strcpy(LcdLine1,"  CIP STEP 1A   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_7_1:		strcpy(LcdLine1,"  CIP STEP 1B   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_8_1:		strcpy(LcdLine1,"   CIP STEP 1   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_9_1:		strcpy(LcdLine1,"   CIP STEP 2   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_10_1:		strcpy(LcdLine1,"  CIP STEP 2A   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_11_1:		strcpy(LcdLine1,"  CIP STEP 2B   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_12_1:		strcpy(LcdLine1," CIP FLUSH TIME ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_13_1:		strcpy(LcdLine1,"  WV CHECK TIME ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_14_1:		strcpy(LcdLine1,"  WAIT HT TIME  ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_15_1:		strcpy(LcdLine1,"  P.FLOW TARGET ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_16_1:		strcpy(LcdLine1," LOW FLO P.ALRM ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_17_1:		strcpy(LcdLine1," LOW FLO W.ALRM ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_18_1:		strcpy(LcdLine1," MAX CIP CYCLES ");
                                                                DisplayIllegalValuMsg();
								break;
		case PARA_SET_19_1:		strcpy(LcdLine1,"   TEMP ALARM   ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_20_1:		strcpy(LcdLine1,"  MAX CIP P.R.T ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_21_1:		strcpy(LcdLine1,"  PUMP P-FACTOR ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_22_1:		strcpy(LcdLine1,"  DYNA P-FACTOR ");
								DisplayIllegalValuMsg();
								break;
		/************************** ADDED BY ARUN NOV 14 *********************************/
		case PARA_SET_23_1:		strcpy(LcdLine1,"  P.CYCLE VOLT  ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_24_1:		strcpy(LcdLine1,"  W.CYCLE VOLT  ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_25_1:		strcpy(LcdLine1,"     W_VALUE    ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_30_1:		strcpy(LcdLine1," START PRESSURE ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_31_1:		strcpy(LcdLine1," STOP  PRESSURE ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_32_1:		strcpy(LcdLine1,"BYPASS PRESSURE ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_33_1:		strcpy(LcdLine1," CIP   PRESSURE ");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_34_1:		strcpy(LcdLine1,"WAIT TIME B4 CIP");
								DisplayIllegalValuMsg();
								break;
		case PARA_SET_35_1:		strcpy(LcdLine1,"WAIT B4 BY ALARM");
                                                                DisplayIllegalValuMsg();
								break;
		#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)
                case PARA_SET_36_1:		strcpy(LcdLine1," MIN CIP CYCLES ");
                                                DisplayIllegalValuMsg();
                                                                        break;
                case PARA_SET_37_1:		strcpy(LcdLine1," CIP SET TIME  ");
                                                DisplayIllegalValuMsg();
                                                                        break;
                case PARA_SET_38_1:		strcpy(LcdLine1," CIP GRACE TIME ");
                                                DisplayIllegalValuMsg();
                                                                        break;
                #endif
                                                                        
                case PARA_SET_40_1:		strcpy(LcdLine1," P-PURE CURRENT ");
                                                                DisplayIllegalValuMsg();
								break;
                case PARA_SET_41_1:		strcpy(LcdLine1,"  PURE CURRENT  ");
                                                                DisplayIllegalValuMsg();
								break;
		case PARA_SET_42_1:		strcpy(LcdLine1,"  WASTE CURRENT ");
                                                                DisplayIllegalValuMsg();
								break;
                case PARA_SET_43_1:		strcpy(LcdLine1,"  PRE-CHG TIME  ");
                                                                DisplayIllegalValuMsg();
								break;  
                case PARA_SET_44_1:		strcpy(LcdLine1,"  POST-CHG TIME ");
                                                                DisplayIllegalValuMsg();
								break; 
                ////////////////////////////////////////////////////////////////////////////////////////////////////
		/*case SYS_SETTING_0:             strcpy(LcdLine1," SYSTEM SETTING ");
										strcpy(LcdLine2," Reporting Time ");
										LcdLine2[15] = 0x7E;
										CursorActiveFlag = FALSE;
										break;
		*/
		/*case SYS_SETTING_0_0:           strcpy(LcdLine1," REPORTING TIME ");
										strcpy(LcdLine2,"                ");
										CursorActiveFlag = TRUE;
										LcdLine2[5] = SerialHmiPassword[0];
										LcdLine2[6] = SerialHmiPassword[1];
										LcdLine2[7] = ':';
										LcdLine2[8] = SerialHmiPassword[2];
										LcdLine2[9] = SerialHmiPassword[3];
										//strcpy(LcdLine2,ReportingTime.tm_min);
										switch(KeyPressCounter){
												case 0:	CurPos = 205; LcdLine2[5]=SerialHmiPassword[0]; break;
												case 1: CurPos = 206; LcdLine2[6]=SerialHmiPassword[1];break;
												case 2: CurPos = 208; LcdLine2[8]=SerialHmiPassword[2];break;
												case 3: CurPos = 209; LcdLine2[9]=SerialHmiPassword[3];break;
										}
										break;*/
		case HW_TEST_1:		        strcpy(LcdLine1," HARDWARE TEST  ");
									strcpy(LcdLine2,"   Input Read   ");
									CursorActiveFlag = FALSE;
									//LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case HW_TEST_2:         	strcpy(LcdLine1," HARDWARE TEST  ");
									strcpy(LcdLine2,"   Relay Test   ");
									DisplayBothArrowAndNoCursor();
									break;
		case HW_TEST_3:                 strcpy(LcdLine1," HARDWARE TEST  ");
									strcpy(LcdLine2,"   Sensor Read  ");
									CursorActiveFlag = FALSE;
									LcdLine2[0] = 0x7F;
									//LcdLine2[15] = 0x7E;
									break;
		case INPUT_TEST_1:			strcpy(LcdLine1,"   INPUT READ   ");
									strcpy(LcdLine2," 1L 2L 3L 4L 5L ");
									CursorActiveFlag = FALSE;
									if((CurDigitalInput & IP_CIP_LVL)>0)
									{	LcdLine2[2] = 'H';
									}
									if((CurDigitalInput & IP_BYPASS_SIG)>0)
									{	LcdLine2[5] = 'H'; 
									}
									if((CurDigitalInput & IP_LEAKAGE_SIG)>0)
									{	LcdLine2[8] = 'H'; 
									}
									if((CurDigitalInput & IP_START_SWT)>0)
									{	LcdLine2[11] = 'H'; 
									}
                                                                        if((CurDigitalInput & IP_SPARE_SWT)>0)
									{	LcdLine2[14] = 'H'; 
									}
									break;
		case OUTPUT_TEST_1:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
                                                                        sprintf(CopyBuff,"MIV : %s",((DigitalOutput & 0x0001)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									//LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_2:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"BYPASS : %s",((DigitalOutput & 0x0002)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_3:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"POV:%s",((DigitalOutput & 0x0004)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_4:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"WOV:%s",((DigitalOutput & 0x0008)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_5:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"CIP:%s",((DigitalOutput & 0x0010)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_6:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"WASTE COIL:%s",((DigitalOutput & 0x0020)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_7:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"PURE COIL:%s",((DigitalOutput & 0x0040)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_8:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"RECIRC.V:%s",((DigitalOutput & 0x0080)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_9:			strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"BUZZER:%s",((DigitalOutput & 0x0100)>0)? "OFF":"ON");
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
		case OUTPUT_TEST_10:		        strcpy(LcdLine1,"   RELAY TEST   ");
									CursorActiveFlag = FALSE;
									sprintf(CopyBuff,"Man. Pump:%s",(PumpManualMode == 0 ) ? "OFF":"ON");                                                                        
                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
									LcdLine2[0] = 0x7F;
									LcdLine2[15] = 0x7E;
									break;
                
                case OUTPUT_TEST_11:                    strcpy(LcdLine1,"   RELAY TEST   ");
                                                                        strcpy(LcdLine2,"   Pump Speed   ");
                                                                        DisplayBothArrowAndNoCursor();
                                                                        break;

                case OUTPUT_TEST_11_0:			strcpy(LcdLine1,"   Pump Speed   ");								
                                                                        sprintf(LcdLine2," %04d ???? %04d ",PUMP_MANUAL_SPEED_MIN_VAL, PUMP_MANUAL_SPEED_MAX_VAL);
                                                                        LcdLine2[6]= SerialHmiPassword[0];
                                                                        LcdLine2[7]= SerialHmiPassword[1];
                                                                        LcdLine2[8]= SerialHmiPassword[2];
                                                                        LcdLine2[9]= SerialHmiPassword[3];
                                                                        switch(KeyPressCounter){
                                                                                        case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
                                                                                        case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
                                                                                        case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
                                                                                        case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
                                                                                        default: CursorActiveFlag = FALSE;break; 
                                                                        }
                                                                        break;                         	

                case OUTPUT_TEST_12:		        strcpy(LcdLine1,"   RELAY TEST   ");
                                                                EnablePAEoutput(PAE_PrepurifyStepVoltageSetpoint*10, OptimizedPAE_CurrentForPrepureStep);
								sprintf(LcdLine2," %04d ???? %04d ",PREPURIFY_TIME_MIN_VAL, PREPURIFY_TIME_MAX_VAL);
								LcdLine2[6]= SerialHmiPassword[0];
								LcdLine2[7]=SerialHmiPassword[1];
								LcdLine2[8]=SerialHmiPassword[2];
								LcdLine2[9]=SerialHmiPassword[3];
								switch(KeyPressCounter){
										case 0:	CurPos = 206; CursorActiveFlag = TRUE;break;
										case 1: CurPos = 207; CursorActiveFlag = TRUE;break;
										case 2: CurPos = 208; CursorActiveFlag = TRUE;break;
										case 3: CurPos = 209; CursorActiveFlag = TRUE;break;
										default: CursorActiveFlag = FALSE;break; 
								}
								
								break;                                                                        
                
               case SENSOR_READ_1:             strcpy(LcdLine1,"   SENSOR READ  ");
										CursorActiveFlag = FALSE;
										strcpy(LcdLine2,"      Flow      ");
										//LcdLine2[0] = 0x7F;
										LcdLine2[15] = 0x7E;
										break;
		case SENSOR_READ_2:             strcpy(LcdLine1,"   SENSOR READ  ");
										strcpy(LcdLine2,"  Conductivity  ");
										DisplayBothArrowAndNoCursor();
										break;
		case SENSOR_READ_3:             strcpy(LcdLine1,"   SENSOR READ  ");
										strcpy(LcdLine2,"   Water Temp   ") ;
										DisplayBothArrowAndNoCursor();
										break;
		case SENSOR_READ_4:             strcpy(LcdLine1,"   SENSOR READ  ");
										CursorActiveFlag = FALSE;
										strcpy(LcdLine2,"  Cabinet Temp  ");
										DisplayBothArrowAndNoCursor();
										break;
		case SENSOR_READ_5:             strcpy(LcdLine1,"   SENSOR READ  ");
										CursorActiveFlag = FALSE;
										strcpy(LcdLine2,"    Pressure    ");
										LcdLine2[0] = 0x7F;
										//LcdLine2[15] = 0x7E;
										break;
										
		case SENSOR_READ_1_1:           /*strcpy(LcdLine1,"      FLOW      ");
										CursorActiveFlag = FALSE;
										//GetAtlasReplacementFlowMeterReading();
                                                fTemp = (float)FlowMeter.CurrentFlow / 100;
                                                                                        sprintf(CopyBuff,"%.2f L/min",fTemp);
                                                                                        JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                                                                        break;*/
                                                strcpy(LcdLine1,"      FLOW      ");
                                                                                        CursorActiveFlag = FALSE;
                                                                                        if(VolumeUnit == VOLUME_UNIT_GALLON)
                                                    sprintf(CopyBuff,"%.2f gal/min",FlowMeter.CurrentFlow * LITER_TO_GALLON);
                                                else
                                                    sprintf(CopyBuff,"%.1f L/min",FlowMeter.CurrentFlow);
                                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                                break;
                                        
		case SENSOR_READ_2_1:           strcpy(LcdLine1,"  CONDUCTIVITY  ");
										CursorActiveFlag = FALSE;
                                        /*
                                        //GetAtlasReplacementReading();
                                        u.i[1] = RawWaterTemperature/10;
                                        u.c[0] = u.c[7];
                                        u.c[1] = u.c[6];
                                        u.c[2] = u.c[5];
                                        u.c[3] = u.c[4];
                                        WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,16,u.c,4);
                                        
                                        ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
										if(I2C_ErrorFlag == FALSE)
										{	if(u.c[0] == 0)
											{	u.c[0] = 1; //Activate Conductivity Probe sensor
												WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
											}
											ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,24,u.c,4);
											if(I2C_ErrorFlag == FALSE)
											{	//switch MSB <--> LSB. First byte received is MSB
												u.c[4] = u.c[3];
												u.c[5] = u.c[2];
												u.c[6] = u.c[1];
												u.c[7] = u.c[0];
												AtlasConductivity = u.i[1];
											}
											else
												AtlasConductivity = -1;
										}
										else
											AtlasConductivity = -1;
										*/
                                        sprintf(CopyBuff,"%d uS/cm",AtlasConductivity/100);
										JustifyCenterForLcdLine(CopyBuff,LcdLine2);
										break;
		case SENSOR_READ_3_1:           strcpy(LcdLine1,"   WATER TEMP   ");
										CursorActiveFlag = FALSE;
										//GetAtlasReplacementReading();
                                        //GetAtlasReplacementTemperatureReading();
                                        //RawWaterTemperature = (int)(((float)RawWaterTemperature - 777.1)/1.7824);//voltea formula editing after my formula
                                        //AtlasTemperature = RawWaterTemperature;
										sprintf(CopyBuff,"%.2f DegC",(float)AtlasTemperature/1000);
										JustifyCenterForLcdLine(CopyBuff,LcdLine2);
										break;
		case SENSOR_READ_4_1:           strcpy(LcdLine1,"  CABINET TEMP  ");
										CursorActiveFlag = FALSE;
										sprintf(CopyBuff,"%.1f DegC",(float)Temperature.i/10);
										JustifyCenterForLcdLine(CopyBuff,LcdLine2);
										break;
                case SENSOR_READ_5_1:           strcpy(LcdLine1,"    PRESSURE    ");
										CursorActiveFlag = FALSE;
										sprintf(CopyBuff,"%.2f bar",AnaPressureAval);
										JustifyCenterForLcdLine(CopyBuff,LcdLine2);
										break;
		case USER_FUNCTIONS_0:	strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"  Flush Module  ");
								//LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case USER_FUNCTIONS_1:	strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"   Start CIP    ");
								DisplayBothArrowAndNoCursor();
								break;
		case USER_FUNCTIONS_2:	strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"  Current Time  ");
								DisplayBothArrowAndNoCursor();
								break;
		case USER_FUNCTIONS_3:	strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"  Current Date  ");
								LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                case USER_FUNCTIONS_4:  strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"  WiFi Network  ");
								LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case USER_FUNCTIONS_5:  strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2," WiFi Password  ");
								LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
		case USER_FUNCTIONS_6:  strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"   Test WiFi    ");
								LcdLine2[0] = 0x7F;
								LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                case USER_FUNCTIONS_7:  strcpy(LcdLine1," USER FUNCTIONS ");
								strcpy(LcdLine2,"   Dry System   ");
								LcdLine2[0] = 0x7F;
								//LcdLine2[15] = 0x7E;
								CursorActiveFlag = FALSE; 
								break;
                                
		case USER_FUNCTIONS_0_0:DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
								DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
								DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV ON
								DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
								DisplayPage = USER_FUNCTIONS_0_1;
								break;
                case USER_FUNCTIONS_0_1:strcpy(LcdLine1," System Running ");
								strcpy(LcdLine2,"  Flush Module  ");
                                                                CursorActiveFlag = FALSE; 
                                                                break;
                case USER_FUNCTIONS_2_0:       strcpy(LcdLine1,"CURRENT UTC TIME");
										strcpy(LcdLine2,"       :        "); 
										CursorActiveFlag = TRUE;
										LcdLine2[5] = SerialHmiPassword[0];
										LcdLine2[6] = SerialHmiPassword[1];
										LcdLine2[7] = ':';
										LcdLine2[8] = SerialHmiPassword[2];
										LcdLine2[9] = SerialHmiPassword[3];
										switch(KeyPressCounter){
												case 0:	CurPos = 205; LcdLine2[5]= SerialHmiPassword[0]; break;
												case 1: CurPos = 206; LcdLine2[6]=SerialHmiPassword[1];break;
												case 2: CurPos = 208; LcdLine2[8]=SerialHmiPassword[2];break;
												case 3: CurPos = 209; LcdLine2[9]=SerialHmiPassword[3];break;
										}
										break;
		 case USER_FUNCTIONS_3_0:       strcpy(LcdLine1,"CURRENT UTC DATE");
										strcpy(LcdLine2,"   20           ");
										//LcdLine2[3] = SerialHmiPassword[0];
										//LcdLine2[4] = SerialHmiPassword[1];
										LcdLine2[5] = SerialHmiPassword[0];
										LcdLine2[6] = SerialHmiPassword[1];
										LcdLine2[7] = '-';
										LcdLine2[8] = SerialHmiPassword[2];
										LcdLine2[9] = SerialHmiPassword[3];
										LcdLine2[10] = '-';
										LcdLine2[11] = SerialHmiPassword[4];
										LcdLine2[12] = SerialHmiPassword[5];			  
										CursorActiveFlag = TRUE;
										switch(KeyPressCounter){
												case 0:	CurPos = 205; break;   //      Y
												case 1: CurPos = 206; break;   //      Y
												case 2: CurPos = 208; break;   //      M
												case 3: CurPos = 209; break;   //      M
												case 4: CurPos = 211; break;   //      D
												case 5: CurPos = 212; break;   //      D
												//case 6: CurPos = 211; LcdLine2[11]=SerialHmiPassword[6];break;   //       
												//case 7: CurPos = 212; LcdLine2[12]=SerialHmiPassword[7];break;   //       
										}
										break;
		/*case SYS_SETTING_0_0_ERROR:	strcpy(LcdLine1," REPORTING TIME ");
										DisplayIllegalValuMsg();
										break;*/
	  	case USER_FUNCTIONS_2_0_ERROR:	strcpy(LcdLine1,"  CURRENT TIME  ");
										DisplayIllegalValuMsg();
										break;
	  	case USER_FUNCTIONS_3_0_ERROR:	strcpy(LcdLine1,"  CURRENT DATE  ");
										DisplayIllegalValuMsg();
										break;
                case USER_FUNCTIONS_4_0:        strcpy(LcdLine1,"  WiFi NETWORK  ");
                                                strcpy(LcdLine2,"     VIEW       ");
                                                //LcdLine2[0] = 0x7F;
                                                LcdLine2[15] = 0x7E;
                                                CursorActiveFlag = FALSE; 
		                                break;
                case USER_FUNCTIONS_4_1:        strcpy(LcdLine1,"  WiFi NETWORK  ");
                                                strcpy(LcdLine2,"     CHANGE     ");
                                                LcdLine2[0] = 0x7F;
                                                //LcdLine2[15] = 0x7E;
                                                CursorActiveFlag = FALSE; 
		                                break;
          
		case USER_FUNCTIONS_5_0:        strcpy(LcdLine1," WiFi PASSWORD  ");
                                        strcpy(LcdLine2,"     VIEW       ");
                                        //LcdLine2[0] = 0x7F;
                                        LcdLine2[15] = 0x7E;
                                        CursorActiveFlag = FALSE; 
		                                break;
                case USER_FUNCTIONS_5_1:        strcpy(LcdLine1," WiFi PASSWORD  ");
                                                strcpy(LcdLine2,"     CHANGE     ");
                                                LcdLine2[0] = 0x7F;
                                                //LcdLine2[15] = 0x7E;
                                                CursorActiveFlag = FALSE; 
		                                break;
        case USER_FUNCTIONS_4_0_0:      //VIEW WIFI NETWORK
                                        DirectMemRead(WIFI_SSID_NAMEa,CopyBuff,WIFI_SSID_NAMEn);
                                        CopyBuff[WIFI_SSID_NAMEn] = '\0';
                                        for(ucTemp = 0; ucTemp < WIFI_SSID_PASSWORDn && CopyBuff[ucTemp]!='\0'; ucTemp++)
                                        {   if(ucTemp<16)
                                            {   LcdLine1[ucTemp] = CopyBuff[ucTemp];
                                                LcdLine1[ucTemp+1] = '\0';
                                            }
                                            else
                                            {   LcdLine2[ucTemp-16] = CopyBuff[ucTemp];
                                                LcdLine2[ucTemp-15] = '\0';
                                            }
                                        }
                                        for(; ucTemp < WIFI_SSID_PASSWORDn; ucTemp++)
                                        {   if(ucTemp<16)
                                            {   LcdLine1[ucTemp] = ' ';
                                                LcdLine1[ucTemp+1] = '\0';
                                            }
                                            else
                                            {   LcdLine2[ucTemp-16] = ' ';
                                                LcdLine2[ucTemp-15] = '\0';
                                            }
                                        }
                                        break;
        case USER_FUNCTIONS_4_1_0:      for(ucTemp=0;ucTemp<KeyPressCounter;ucTemp++)
                                        {   if(KeyPressCounter < 16)
                                            {   LcdLine1[KeyPressCounter] = SerialHmiPassword[KeyPressCounter];
                                                CurPos = 100+KeyPressCounter; 
                                            }
                                            else
                                            {   LcdLine2[KeyPressCounter-16] = SerialHmiPassword[KeyPressCounter];
                                                CurPos = 200 + KeyPressCounter-16;
                                            }
                                        }
                                        if(KeyPressCounter < 16)
                                            LcdLine1[KeyPressCounter] = SerialHmiTemp;
                                        else
                                            LcdLine2[KeyPressCounter-16] = SerialHmiTemp;
                                        
                                        CursorActiveFlag = TRUE;
                                        break;
        case USER_FUNCTIONS_5_0_0:      //VIEW WIFI password
                                        DirectMemRead(WIFI_SSID_PASSWORDa,CopyBuff,WIFI_SSID_PASSWORDn);
                                        CopyBuff[WIFI_SSID_PASSWORDn] = '\0';
                                        for(ucTemp = 0; ucTemp < WIFI_SSID_PASSWORDn && CopyBuff[ucTemp]!='\0'; ucTemp++)
                                        {   if(ucTemp<16)
                                            {   LcdLine1[ucTemp] = CopyBuff[ucTemp];
                                                LcdLine1[ucTemp+1] = '\0';
                                            }
                                            else
                                            {   LcdLine2[ucTemp-16] = CopyBuff[ucTemp];
                                                LcdLine2[ucTemp-15] = '\0';
                                            }
                                        }
                                        for(; ucTemp < WIFI_SSID_PASSWORDn; ucTemp++)
                                        {   if(ucTemp<16)
                                            {   LcdLine1[ucTemp] = ' ';
                                                LcdLine1[ucTemp+1] = '\0';
                                            }
                                            else
                                            {   LcdLine2[ucTemp-16] = ' ';
                                                LcdLine2[ucTemp-15] = '\0';
                                            }
                                        }
                                        break;
        case USER_FUNCTIONS_5_1_0:      for(ucTemp=0;ucTemp<KeyPressCounter;ucTemp++)
                                        {   if(KeyPressCounter < 16)
                                            {   LcdLine1[KeyPressCounter] = SerialHmiPassword[KeyPressCounter];
                                                CurPos = 100+KeyPressCounter; 
                                            }
                                            else
                                            {   LcdLine2[KeyPressCounter-16] = SerialHmiPassword[KeyPressCounter];
                                                CurPos = 200 + KeyPressCounter-16;
                                            }
                                        }
                                        if(KeyPressCounter < 16)
                                            LcdLine1[KeyPressCounter] = SerialHmiTemp;
                                        else
                                            LcdLine2[KeyPressCounter-16] = SerialHmiTemp;
                                        
                                        CursorActiveFlag = TRUE;
                                        break;
        case USER_FUNCTIONS_6_0:    switch(MobileStage)
                                    {   case MOBILE_STAGE_SWITCH_TO_WIFI:
                                        case MOBILE_STAGE_SWITCHING_WAIT:
                                        case MOBILE_STAGE_WIFI_AT_COMMAND:
                                        case MOBILE_STAGE_WIFI_ROUTER_SSID_AND_PASSWORD:
                                        case MOBILE_STAGE_WIFI_NETWORK_QUERY:
                                                strcpy(LcdLine1,"   TEST WiFi    ");
                                                strcpy(LcdLine2,"Stat:In Progress");
                                        break;
                                        case MOBILE_STAGE_WIFI_ENABLE_AUTO_RECEIVE:
                                        case MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT:
                                        case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING:
                                            DisplayPage = USER_FUNCTIONS_6_1;
                                        break;
                                        default: 
                                                wifi_test_fail_cntr++;
                                                if(wifi_test_fail_cntr >=1 )
                                                    DisplayPage = USER_FUNCTIONS_6_2;
                                                else//retest
                                                {   MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                                                }
                                        break;
                                    }
                                    break;
        case USER_FUNCTIONS_6_1:    strcpy(LcdLine1,"   TEST WiFi    ");
                                    strcpy(LcdLine2,"Stat:SUCCESSFUL.");
                                    break;
	case USER_FUNCTIONS_6_2:    strcpy(LcdLine1,"   TEST WiFi    ");
                                    strcpy(LcdLine2,"Stat: FAILED    ");
                                    break;
	case USER_FUNCTIONS_7_0:    strcpy(LcdLine1,"     DRYING     ");
                                    strcpy(LcdLine2,"     SYSTEM     ");
                                    #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                        DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                                        DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_BYPASS ON
                                        DigitalOutput &= ~(1<<OUTPUT_POV);	//OUTPUT_POV ON
                                        DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_CFO)
                                        DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                                        DigitalOutput &= ~(1<<OUTPUT_WOV2);	//OUTPUT_WOV2 ON
                                        DigitalOutput &= ~(1<<OUTPUT_POV1);	//OUTPUT_POV1 ON
                                        DigitalOutput &= ~(1<<OUTPUT_WOV1);	//OUTPUT_WOV1 ON
                                    #endif
                                    #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                        DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                                        DigitalOutput &= ~(1<<OUTPUT_POV);	//OUTPUT_POV ON
                                        DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                        DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE ON
                                    #endif
                                    break;
                                    
        case AUTO_0:            CursorActiveFlag = FALSE;
				switch(CurrentProcessStep){
                                    case PREPURIFY:
                                    case PURIFY:
                                    case SHUNT_POST_PURE:  
                                    case WASTE:
                                    case HF_WASTE:
                                    case SHUNT_POST_WASTE:
                                    //case FLOW_CHECK:    
                                                    strcpy(CopyBuff,"OPERATION"); break;
                                    case CIP_DOSING_ON:	
                                    case CIP_DOSING_OFF:
                                    case CIP_PULSE_ON:
                                    case CIP_PULSE_OFF:	
                                    case CIP_FLUSH:
                                    //case WV_CHECK:  
                                                    strcpy(CopyBuff,"CLEANING"); break;
                                    case IDLE:      strcpy(CopyBuff,"IDLE/ENTER"); break;
                                    
                                    case WAIT_BEFORE_CIP_START:
                                    default:        strcpy(CopyBuff,"STAND BY"); break;
                                }
                                JustifyCenterForLcdLine(CopyBuff,LcdLine1);
                                /*for(u.c[0]=0,u.c[1]=0,uiTemp=0;u.c[0]<5;u.c[0]++)
                                {   if(LastFiveCyclePureAverageConductivitySamples[u.c[0]]>0)
                                    {   uiTemp+=LastFiveCyclePureAverageConductivitySamples[u.c[0]];
                                        u.c[1]++;
                                    }
                                }
                                uiTemp/=u.c[1];//average of avaliable max 5 sample*/
                                sprintf(CopyBuff,"%d uS/cm",LastFiveCyclePureAverageConductivityAverage/100);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                if(AlarmRegister24Bit > 0)
                                    LcdLine2[15] = '!';
                                break;
        case AUTO_1:		strcpy(LcdLine1,"  Exit Process  ");
                                strcpy(LcdLine2,"     N / Y ?    ");
                                CursorActiveFlag = FALSE;
                                break;
                                                                
        case AUTO_0_1:	        CursorActiveFlag = FALSE;
				switch(CurrentProcessStep){
                                    case FREE_RUN_SEC:{
                                        u.i[0] = FreeRunSec;
                                        break;
                                    }
                                    case IDLE:	{
                                        strcpy(CopyBuff,"IDLE");
										u.i[0]=ProcessStepRunSec;//idle for this much sec
                                        break;
									}
									case PREPURIFY:{
                                        strcpy(CopyBuff,"PREPURIFY");
										u.i[0] = PrepurifyTimeInSec;
										break;
									}
									case PURIFY:{
                                        strcpy(CopyBuff,"PURIFY");
										u.i[0] = PurifyTimeInSec;
										break;
                                    }
                                                                        case SHUNT_POST_PURE:{
                                        strcpy(CopyBuff,"SHUNT POST PURE");
										u.i[0] = ShuntPostPureStepTimeInSec;
										break;
                                    }
									case WASTE:{
                                        strcpy(CopyBuff,"WASTE");
                                        if(FirstWasteCycle == FALSE)
                                        {   u.i[0] = PureStepRunSec * WasteTimeInSec/(PrepurifyTimeInSec + PurifyTimeInSec);
                                            /*if(u.i[0] > WasteTimeInSec)
                                               u.i[0] = WasteTimeInSec;*/
                                        }
                                        else
                                        {   //strcpy(CopyBuff,"FIRST WASTE");
                                            u.i[0] = WasteTimeInSec/2;
										}
                                        break;
									}
									case HF_WASTE:{
                                        strcpy(CopyBuff,"HIGH FLOW WASTE");
										u.i[0] = HF_WasteTimeInSec;
										break;
									}
									case SHUNT_POST_WASTE:{
                                        strcpy(CopyBuff,"SHUNT POST WASTE");
										u.i[0] = ShuntPostWasteStepTimeInSec;
										break;
                                    }
									case WAIT:{
                                        strcpy(CopyBuff,"STAND BY");
										u.i[0] = ProcessStepRunSec;//waited for this much sec till now
										break;
									}
									case CIP_DOSING_ON:{
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            strcpy(CopyBuff,"CIP DOSING");
                                        #else
                                            strcpy(CopyBuff,"CIP DOSING ON");
                                        #endif
                                        u.i[0] = CIP_DosingOnTimeInSec;
										break;
									}
									case CIP_DOSING_OFF:{
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            strcpy(CopyBuff,"CIP RECIRCULATE");
                                        #else
                                            strcpy(CopyBuff,"CIP DOSING OFF");
                                        #endif
                                        u.i[0] = CIP_DosingOffTimeInSec;
										break;
									}  
									#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                    case CIP_PULSE_ON:{
                                        strcpy(CopyBuff,"CIP PULSE ON");
										u.i[0] = CIP_PulseOnTimeInSec;
										break;
									}
									case CIP_PULSE_OFF:{
                                        strcpy(CopyBuff,"CIP PULSE OFF");
                                        u.i[0] = CIP_PulseOffTimeInSec;
                                        break;
                                    }
                                    #endif
                                    case CIP_FLUSH:{
                                        strcpy(CopyBuff,"CIP FLUSH");
                                        u.i[0] = CIP_FlushTimeInSec;
                                        break;
                                    }
                                    //case WV_CHECK:{
                                    //    strcpy(CopyBuff,"WASTE VALVE CHK ");
                                    //    u.i[0] = WV_CheckTimeInSec;
                                    //    break;
                                    //}
                                    case HIGH_TEMP:{
                                        strcpy(CopyBuff,"HIGH TEMPERATURE");
                                        u.i[0] = ProcessStepRunSec;//waited for this much sec till now
                                        break;
                                    }
                                    case WAIT_HT:{
                                        strcpy(CopyBuff,"HIGH TEMP. WAIT");
                                        u.i[0] = WaitHT_TimeInSec;
                                        break;
                                    }
                                    /*
                                    case FLOW_CHECK:{
                                        strcpy(CopyBuff,"FLOW CHECK");
                                        u.i[0] = FLOW_CHECK_TIME_IN_SEC;
                                        break;
                                    }
                                    */
                                    case SHUNT:{
                                        strcpy(CopyBuff,"FLUSH+SHUNT");
                                        u.i[0] = ShuntSec;
                                        break;
                                    }
                                    case WAIT_BEFORE_CIP_START:{
                                        strcpy(CopyBuff,"PRE CIP WAIT");
                                        u.i[0] = CIP_WaitTimeInSec;
                                        break;
                                    }
								}
                                JustifyCenterForLcdLine(CopyBuff,LcdLine1);
                                if(CurrentProcessStep == IDLE){
                                    strcpy(CopyBuff,"ENTER");
                                }	
                                else if(CurrentProcessStep == WAIT){
                                    strcpy(CopyBuff,"START");
                                }	
                                else{
                                    iTemp = u.i[0]-ProcessStepRunSec;
                                    
                                    if(iTemp < -10)//something went wrong and not resolved for 10 sec so restart the system
                                    {   /*PutStringUsb("\r\nRESTART DUE TO ProcessStepRunSec OVERRUN");//RESET TEST CODE
                                        SoftwareResetProcessor();*/
                                        iTemp = u.i[0];
                                    }
                                    sprintf(CopyBuff,"%d",iTemp);
                                }
                                sprintf(PasteBuff,"/%d",u.i[0]);
                                strcat(CopyBuff,PasteBuff);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                				DisplayBothArrowAndNoCursor();
                                break;
                                
        case AUTO_0_2:		strcpy(LcdLine1," MAX CIP CYCLES ");
                                iTemp = MaxCycleCountForCIP-CycleCounter;
                                if(iTemp < 0)
                                    iTemp = 0;
                                sprintf(CopyBuff,"%d/%d",iTemp,MaxCycleCountForCIP);      
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                				DisplayBothArrowAndNoCursor();
                                break;
        case AUTO_0_3:          strcpy(LcdLine1,"TOT.PURE VOLUME ");
                                if(VolumeUnit == VOLUME_UNIT_GALLON)
                                    sprintf(CopyBuff,"%.2f gal",TotalPureVolume * LITER_TO_GALLON);
                                else
                                    sprintf(CopyBuff,"%d L",(ui)TotalPureVolume);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                DisplayBothArrowAndNoCursor();
                                break;
        case AUTO_0_4:          //Display % recovery
                                strcpy(LcdLine1,"    RECOVERY    ");
                                for(u.c[0]=0,u.c[1]=0,uiTemp=0;u.c[0]<5;u.c[0]++)
                                {   if(PercentageRecovery[u.c[0]]>0)
                                    {   uiTemp+=PercentageRecovery[u.c[0]];
                                        u.c[1]++;
                                    }
                                }
                                uiTemp/=u.c[1];//average of avaliable max 5 sample
                                sprintf(CopyBuff,"%d%%",uiTemp);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                DisplayBothArrowAndNoCursor();
                                break;
        case AUTO_0_5:          strcpy(LcdLine1,"  INSTANT FLOW  ");
                                if(VolumeUnit == VOLUME_UNIT_GALLON)
                                    sprintf(CopyBuff,"%.2f gal/min",FlowMeter.CurrentFlow * LITER_TO_GALLON);
                                else
                                    sprintf(CopyBuff,"%.1f L/min",FlowMeter.CurrentFlow);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                DisplayBothArrowAndNoCursor();
                                break;
                                
        case AUTO_0_6:          strcpy(LcdLine1,"  Live Voltage  "); 
                                sprintf(CopyBuff,"%.2f",(float)CurrentPAEvolt/100);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                LcdLine2[0] = 0x7F;
                                LcdLine2[15] = 0x7E;
                                CursorActiveFlag = FALSE; 
                                break;
        
        case AUTO_0_7:          strcpy(LcdLine1,"  Live Current  "); 
                                sprintf(CopyBuff,"%.2f",(float)CurrentPAEcurrent/100);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                LcdLine2[0] = 0x7F;
                                LcdLine2[15] = 0x7E;
                                CursorActiveFlag = FALSE; 
                                break;
        case AUTO_0_8:          strcpy(LcdLine1,"  Live Pressure "); 
                                sprintf(CopyBuff,"%.2f bar",AnaPressureAval);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                LcdLine2[0] = 0x7F;
                                LcdLine2[15] = 0x7E;
                                CursorActiveFlag = FALSE; 
                                break;
                                
                                //alarm will always be the last page.
        case AUTO_0_9:          strcpy(LcdLine1,"     ALARMS     ");
                                sprintf(CopyBuff,"%s",(AlarmRegister24Bit>0)? "YES":"NO");
                                //sprintf(CopyBuff,"Code = 0x%X",AlarmRegister24Bit);
                                JustifyCenterForLcdLine(CopyBuff,LcdLine2);
                                LcdLine2[0] = 0x7F;
                                //LcdLine2[15] = 0x7E;
                                CursorActiveFlag = FALSE; 
                                break;

        case AUTO_0_9_0:		DisplayAlarmStatus(LOW_FLOW_PURIFY_ALARM_FLAG);	break;
        case AUTO_0_9_1:		DisplayAlarmStatus(LOW_FLOW_WASTE_ALARM_FLAG);	break;
        case AUTO_0_9_2:		DisplayAlarmStatus(BYPASS_ALARM_FLAG);			break;
        case AUTO_0_9_3:		DisplayAlarmStatus(CABINET_TEMP_ALARM_FLAG);	break;
        case AUTO_0_9_4:		DisplayAlarmStatus(LEAKAGE_ALARM_FLAG);			break;
        case AUTO_0_9_5:		DisplayAlarmStatus(WASTE_VALUE_ALARM_FLAG);		break;
        case AUTO_0_9_6:		DisplayAlarmStatus(CIP_LOW_LEVEL_ALARM_FLAG);	break;
        case AUTO_0_9_7:		DisplayAlarmStatus(PAE_COMM_ERROR_FLAG);		break;
        case AUTO_0_9_8:		DisplayAlarmStatus(PAE_OVP_SHUTDOWN_FLAG);		break;
        case AUTO_0_9_9:		DisplayAlarmStatus(PAE_OLP_SHUTDOWN_FLAG);		break;
        case AUTO_0_9_10:		DisplayAlarmStatus(PAE_OTP_SHUTDOWN_FLAG);		break;
        case AUTO_0_9_11:		DisplayAlarmStatus(PAE_FAN_FAIL_FLAG);			break;
        case AUTO_0_9_12:		DisplayAlarmStatus(PAE_AUX_OR_SMPS_FAIL_FLAG);	break;
        case AUTO_0_9_13:		DisplayAlarmStatus(PAE_HI_TEMP_FLAG);			break;
        case AUTO_0_9_14:		DisplayAlarmStatus(PAE_AC_POWER_DOWN_FLAG);		break;
        case AUTO_0_9_15:		DisplayAlarmStatus(PAE_AC_INPUT_FAIL_FLAG);		break;
        case AUTO_0_9_16:		DisplayAlarmStatus(LOW_PRESSURE_ALARM_FLAG);	break;
        case AUTO_0_9_17:		DisplayAlarmStatus(ATLAS_I2C_COMM_ERROR_FLAG);	break;
        case AUTO_0_9_18:		DisplayAlarmStatus(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG);	break;
        case AUTO_0_9_19:               DisplayAlarmStatus(FLOWMETER_COMM_ALARM_FLAG);break;
	}
	SendSerialHmiPacket();
}
//------------------------------------------------------------------------------
void CheckKeyPress()
{	CheckSerialHmiKeyInput();
	if(KeyPressFlag == TRUE)
	{	KeyPressFlag = FALSE;
		switch(Key)
		{	case KEY_UP:
				switch(DisplayPage)
				{	//case LOGOUT_0:			break;
					case LOGOUT_1:			DisplayPage=LOGOUT_0;break;
					case LOGOUT_2:			DisplayPage=LOGOUT_1;break;
                                        case LOGOUT_3:			DisplayPage=LOGOUT_2;break; // User functions                 
					case USER_FUNCTIONS_4_1_0:
                                        case USER_FUNCTIONS_5_1_0: 
                                            if(SerialHmiTemp < 122)//end of readable char in ascii table
                                            {   SerialHmiTemp++;
                                                if(SerialHmiTemp == 92)//'\' character not allowed in password
                                                {   SerialHmiTemp++;
                                                }
                                            }
                                            else
                                            {   if(SerialHmiTemp == 122)
                                                    SerialHmiTemp = 127;
                                                else
                                                    SerialHmiTemp = 32;
                                            }
                                            break;
                                        //case SYS_SETTING_0_0:
					case USER_FUNCTIONS_2_0:
					case USER_FUNCTIONS_3_0:
					case PASSWORD_0:
					case PARA_SET_1_0:		
					case PARA_SET_2_0:		
					case PARA_SET_3_0:		
					case PARA_SET_4_0:		
					case PARA_SET_5_0:		
					case PARA_SET_6_0:		
					case PARA_SET_7_0:		
					case PARA_SET_8_0:		
					case PARA_SET_9_0:		
					case PARA_SET_10_0:		
					case PARA_SET_11_0:		
					case PARA_SET_12_0:		
					case PARA_SET_13_0:		
					case PARA_SET_14_0:		
					case PARA_SET_15_0:		
					case PARA_SET_16_0:		
					case PARA_SET_17_0:		
					case PARA_SET_18_0:		
					case PARA_SET_19_0:
					case PARA_SET_20_0:
					case PARA_SET_21_0:
					case PARA_SET_22_0:
					case PARA_SET_23_0:
					case PARA_SET_24_0:
					case PARA_SET_25_0:
					case PARA_SET_26_0:
					case PARA_SET_30_0:
					case PARA_SET_31_0:
					case PARA_SET_32_0:
					case PARA_SET_33_0:		
					case PARA_SET_34_0:		
					case PARA_SET_35_0:		
                                        #if(CVS_CIP==CVS_CIP_NO)
                                        case PARA_SET_36_0:
                                        case PARA_SET_37_0:
                                        case PARA_SET_38_0:
                                        #endif	
					case PARA_SET_40_0:
                                        case PARA_SET_41_0:		
					case PARA_SET_42_0:
                                        case PARA_SET_43_0:		
					case PARA_SET_44_0:
                                        case OUTPUT_TEST_11_0:
                                          if(SerialHmiPassword[KeyPressCounter] < '9')
                                          SerialHmiPassword[KeyPressCounter]++;
                                        break;
                                        case PARA_SET_39_1:     DisplayPage = PARA_SET_39_0; break;
                                        case PARA_SET_28_1:     DisplayPage = PARA_SET_28_0; break;                                        
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_29_1:     DisplayPage = PARA_SET_29_0; break;
                                        case PARA_SET_29_2:     DisplayPage = PARA_SET_29_1; break;
                                        #endif
					//case PASSWORD_ERR:		break;
					//case DEV_INFO_0:		break;
					case DEV_INFO_1:		DisplayPage=DEV_INFO_0;break;
					case DEV_INFO_2:		DisplayPage=DEV_INFO_1;break;
					case DEV_INFO_3:		DisplayPage=DEV_INFO_2;break;
					//case DEV_INFO_4:		DisplayPage=DEV_INFO_3;break;
					//case DEV_INFO_0_1:		break;
					//case DEV_INFO_1_1:		break;
					//case DEV_INFO_2_1:		break;
					//case DEV_INFO_3_1:		break;
					//case DEV_INFO_4_1:		break;
					
					//case MENU_0:			break;
					case MENU_1:			DisplayPage=MENU_0;break;
					//case MENU_2:			DisplayPage=MENU_1;break;
					case MENU_3:			DisplayPage=MENU_1;break;
					
					case PARA_SET_2:		DisplayPage=PARA_SET_1;break;
					case PARA_SET_3:		DisplayPage=PARA_SET_2;break;
					case PARA_SET_4:		DisplayPage=PARA_SET_3;break;
					case PARA_SET_5:		DisplayPage=PARA_SET_4;break;
					case PARA_SET_6:		DisplayPage=PARA_SET_5;break;
					case PARA_SET_7:		DisplayPage=PARA_SET_6;break;
					case PARA_SET_8:		DisplayPage=PARA_SET_7;break;
					case PARA_SET_9:		DisplayPage=PARA_SET_8;break;
					case PARA_SET_10:		DisplayPage=PARA_SET_9;break;
					case PARA_SET_11:		DisplayPage=PARA_SET_10;break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        case PARA_SET_12:		DisplayPage=PARA_SET_11;break;
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_12:		DisplayPage=PARA_SET_8;break;
                                        #endif
                                        case PARA_SET_13:		DisplayPage=PARA_SET_12;break;
					case PARA_SET_14:		DisplayPage=PARA_SET_13;break;
					case PARA_SET_15:		DisplayPage=PARA_SET_14;break;
					case PARA_SET_16:		DisplayPage=PARA_SET_15;break;
					case PARA_SET_17:		DisplayPage=PARA_SET_16;break;
					case PARA_SET_18:		DisplayPage=PARA_SET_17;break;
					case PARA_SET_19:		DisplayPage=PARA_SET_18;break;
					case PARA_SET_20:		DisplayPage=PARA_SET_19;break;
					case PARA_SET_21:		DisplayPage=PARA_SET_20;break;
					case PARA_SET_22:		DisplayPage=PARA_SET_21;break;
                                        case PARA_SET_23:		DisplayPage=PARA_SET_22;break;
                                        case PARA_SET_24:		DisplayPage=PARA_SET_23;break;
                                        case PARA_SET_25:		DisplayPage=PARA_SET_24;break;
                                        case PARA_SET_26:		DisplayPage=PARA_SET_25;break;
                                        case PARA_SET_27:		DisplayPage=PARA_SET_26;break;
                                        case PARA_SET_28:               DisplayPage=PARA_SET_27;break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_29:               DisplayPage=PARA_SET_28;break;
                                        case PARA_SET_30:               DisplayPage=PARA_SET_29; break;
                                        #else
                                        case PARA_SET_30:               DisplayPage=PARA_SET_28; break;
                                        #endif
                                        case PARA_SET_31:               DisplayPage=PARA_SET_30; break;
                                        case PARA_SET_32:               DisplayPage=PARA_SET_31; break;
                                        case PARA_SET_33:               DisplayPage=PARA_SET_32; break;
                                        case PARA_SET_34:               DisplayPage=PARA_SET_33; break;
                                        case PARA_SET_35:               DisplayPage=PARA_SET_34; break;
                                        #if(CVS_CIP==CVS_CIP_NO)
                                        case PARA_SET_36:               DisplayPage=PARA_SET_35; break;
                                        case PARA_SET_37:               DisplayPage=PARA_SET_36; break;
                                        case PARA_SET_38:               DisplayPage=PARA_SET_37; break;                                                                             
                                        case PARA_SET_39:               DisplayPage=PARA_SET_38; break;                                        
                                        #else
                                        case PARA_SET_39:               DisplayPage=PARA_SET_35; break;                                        
                                        #endif
                                        case PARA_SET_40:               DisplayPage=PARA_SET_39; break;
                                        case PARA_SET_41:               DisplayPage=PARA_SET_40; break;
                                        case PARA_SET_42:               DisplayPage=PARA_SET_41; break;
                                        #if(DynamicCapDIMode == DYNAMIC_CAPDI_MODE_ON)
                                        case PARA_SET_43:               DisplayPage=PARA_SET_42; break;
                                        #else
                                        case PARA_SET_43:               DisplayPage=PARA_SET_39; break;
                                        #endif
                                        case PARA_SET_44:               DisplayPage=PARA_SET_43; break;
                                        
					//case HW_TEST_1:					break;
					case HW_TEST_2:			DisplayPage=HW_TEST_1;break;
					case HW_TEST_3:                 DisplayPage=HW_TEST_2;break;
					
					//case SENSOR_READ_1:             break;
					case SENSOR_READ_2:             DisplayPage=SENSOR_READ_1;break;
					case SENSOR_READ_3:             DisplayPage=SENSOR_READ_2;break;
					case SENSOR_READ_4:             DisplayPage=SENSOR_READ_3;break;
					case SENSOR_READ_5:             DisplayPage=SENSOR_READ_4;break;
					
					//case USER_FUNCTIONS_0:	break;
					case USER_FUNCTIONS_1:	        DisplayPage=USER_FUNCTIONS_0;break;
					case USER_FUNCTIONS_2:	        DisplayPage=USER_FUNCTIONS_1;break;
					case USER_FUNCTIONS_3:	        DisplayPage=USER_FUNCTIONS_2;break;
					case USER_FUNCTIONS_4:	        DisplayPage=USER_FUNCTIONS_3;break;
					case USER_FUNCTIONS_5:	        DisplayPage=USER_FUNCTIONS_4;break;
					case USER_FUNCTIONS_6:	        DisplayPage=USER_FUNCTIONS_5;break;
					case USER_FUNCTIONS_7:	        DisplayPage=USER_FUNCTIONS_6;break;
					
                                        case USER_FUNCTIONS_4_1:        DisplayPage=USER_FUNCTIONS_4_0;break;
					case USER_FUNCTIONS_5_1:        DisplayPage=USER_FUNCTIONS_5_0;break;
					
					//case OUTPUT_TEST_1:				break;
					case OUTPUT_TEST_2:	        DisplayPage=OUTPUT_TEST_1;break;
					case OUTPUT_TEST_3:		DisplayPage=OUTPUT_TEST_2;break;
					case OUTPUT_TEST_4:		DisplayPage=OUTPUT_TEST_3;break;
					case OUTPUT_TEST_5:		DisplayPage=OUTPUT_TEST_4;break;
					case OUTPUT_TEST_6: 	        DisplayPage=OUTPUT_TEST_5;break;
					case OUTPUT_TEST_7: 	        DisplayPage=OUTPUT_TEST_6;break;
					case OUTPUT_TEST_8:		DisplayPage=OUTPUT_TEST_7;break;
					case OUTPUT_TEST_9:		DisplayPage=OUTPUT_TEST_8;break;
					case OUTPUT_TEST_10:		DisplayPage=OUTPUT_TEST_9;break;
                                        case OUTPUT_TEST_11:		DisplayPage=OUTPUT_TEST_10;break;
                                        
					//case AUTO_0:					break;
					case AUTO_0_1:                  DisplayPage = AUTO_0; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_2:                  DisplayPage = AUTO_0_1; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_3:                  DisplayPage = AUTO_0_2; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_4:                  DisplayPage = AUTO_0_3; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_5:                  DisplayPage = AUTO_0_4; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_6:                  DisplayPage = AUTO_0_5; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_7:                  DisplayPage = AUTO_0_6; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_8:                  DisplayPage = AUTO_0_7; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_9:                  DisplayPage = AUTO_0_8; /*KeyTimeOut = FreeRunSec;*/ break;
					
                                        case AUTO_0_9_1:		DisplayPage = AUTO_0_9_0; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_2:		DisplayPage = AUTO_0_9_1; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_3:		DisplayPage = AUTO_0_9_2; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_4:		DisplayPage = AUTO_0_9_3; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_5:		DisplayPage = AUTO_0_9_4; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_6:		DisplayPage = AUTO_0_9_4; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_7:		DisplayPage = AUTO_0_9_6; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_8:		DisplayPage = AUTO_0_9_7; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_9:		DisplayPage = AUTO_0_9_8; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_10:		DisplayPage = AUTO_0_9_9; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_11:		DisplayPage = AUTO_0_9_10; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_12:		DisplayPage = AUTO_0_9_11; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_13:		DisplayPage = AUTO_0_9_12; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_14:		DisplayPage = AUTO_0_9_13; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_15:		DisplayPage = AUTO_0_9_14; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_16:		DisplayPage = AUTO_0_9_15; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_17:		DisplayPage = AUTO_0_9_16; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_18:		DisplayPage = AUTO_0_9_17; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_19:		DisplayPage = AUTO_0_9_18; /*KeyTimeOut = FreeRunSec;*/break;
				}
			break;
			case KEY_DOWN:
				switch(DisplayPage)
				{	case LOGOUT_0:			DisplayPage=LOGOUT_1;break; // Start Process
					case LOGOUT_1:			DisplayPage=LOGOUT_2;break; // Device Info
					case LOGOUT_2:			DisplayPage=LOGOUT_3;break; // User functions
					case USER_FUNCTIONS_4_1_0:
                                        case USER_FUNCTIONS_5_1_0:
                                            if(SerialHmiTemp > 32)// ascii value of space
                                            {   if(SerialHmiTemp == 127)
                                                   SerialHmiTemp = 122;
                                                else
                                                {   SerialHmiTemp--;
                                                    if(SerialHmiTemp == 92)//'\' character not allowed in password
                                                    {   SerialHmiTemp--;
                                                    }
                                                }
                                            }
                                            else 
                                              SerialHmiTemp = 127;
                                            break;
                    
                                        //case SYS_SETTING_0_0:
					case USER_FUNCTIONS_2_0:
					case USER_FUNCTIONS_3_0:
					  
					//case PROCESS:
					case PASSWORD_0:
					case PARA_SET_1_0:		
					case PARA_SET_2_0:		
					case PARA_SET_3_0:		
					case PARA_SET_4_0:		
					case PARA_SET_5_0:		
					case PARA_SET_6_0:		
					case PARA_SET_7_0:		
					case PARA_SET_8_0:		
					case PARA_SET_9_0:		
					case PARA_SET_10_0:		
					case PARA_SET_11_0:		
					case PARA_SET_12_0:		
					case PARA_SET_13_0:		
					case PARA_SET_14_0:		
					case PARA_SET_15_0:		
					case PARA_SET_16_0:		
					case PARA_SET_17_0:		
					case PARA_SET_18_0:		
					case PARA_SET_19_0:	
					case PARA_SET_20_0:	
					case PARA_SET_21_0:	
					case PARA_SET_22_0:
					case PARA_SET_23_0:	
					case PARA_SET_24_0:
					case PARA_SET_25_0:
					case PARA_SET_26_0:	
                                        case PARA_SET_30_0:
					case PARA_SET_31_0:
					case PARA_SET_32_0:
					case PARA_SET_33_0:
					case PARA_SET_34_0:
					case PARA_SET_35_0:
					#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)
                                        case PARA_SET_36_0:
                                        case PARA_SET_37_0:
                                        case PARA_SET_38_0:
                                        #endif
					case PARA_SET_40_0:
					case PARA_SET_41_0:
					case PARA_SET_42_0:
                                        case PARA_SET_43_0:
					case PARA_SET_44_0:
                                        case OUTPUT_TEST_11_0:
                                        if(SerialHmiPassword[KeyPressCounter] > '0')
                                        SerialHmiPassword[KeyPressCounter]--;
                                        break;
                                        case PARA_SET_28_0:     DisplayPage = PARA_SET_28_1; break;
                                        case PARA_SET_39_0:     DisplayPage = PARA_SET_39_1; break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_29_0:     DisplayPage = PARA_SET_29_1; break;
                                        case PARA_SET_29_1:     DisplayPage = PARA_SET_29_2; break;
                                        #endif
                                        ////////////////////////////////////////////////////////////////////////////////////////
					//case SYS_SETTING_2:   DisplayPage=SYS_SETTING_3;break;
                                        ///////////////////////////////////////////////////////////////////////////////////////
					//case PASSWORD_ERR:		break;
					case DEV_INFO_0:		DisplayPage=DEV_INFO_1;break;
					case DEV_INFO_1:		DisplayPage=DEV_INFO_2;break;
					case DEV_INFO_2:		DisplayPage=DEV_INFO_3; break;
					//case DEV_INFO_3:		DisplayPage=DEV_INFO_4; break;
					//case DEV_INFO_4:		break;
					//case DEV_INFO_0_1:		break;
					//case DEV_INFO_1_1:		break;
					//case DEV_INFO_2_1:		break;
					//case DEV_INFO_3_1:		break;
					//case DEV_INFO_4_1:		break;
					
					case MENU_0:			DisplayPage=MENU_1; break;
					case MENU_1:			DisplayPage=MENU_3; break;
					//case MENU_2:			DisplayPage=MENU_3; break;
					//case MENU_3:			break;
					
					case PARA_SET_1:		DisplayPage=PARA_SET_2; break;
					case PARA_SET_2:		DisplayPage=PARA_SET_3; break;
					case PARA_SET_3:		DisplayPage=PARA_SET_4; break;
					case PARA_SET_4:		DisplayPage=PARA_SET_5; break;
					case PARA_SET_5:		DisplayPage=PARA_SET_6; break;
					case PARA_SET_6:		DisplayPage=PARA_SET_7; break;
					case PARA_SET_7:		DisplayPage=PARA_SET_8; break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        case PARA_SET_8:		DisplayPage=PARA_SET_9; break;
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_8:		DisplayPage=PARA_SET_12; break;
                                        #endif
                                        case PARA_SET_9:		DisplayPage=PARA_SET_10; break;
					case PARA_SET_10:		DisplayPage=PARA_SET_11; break;
					case PARA_SET_11:		DisplayPage=PARA_SET_12; break;
					case PARA_SET_12:		DisplayPage=PARA_SET_13; break;
					case PARA_SET_13:		DisplayPage=PARA_SET_14; break;
					case PARA_SET_14:		DisplayPage=PARA_SET_15; break;
					case PARA_SET_15:		DisplayPage=PARA_SET_16; break;
					case PARA_SET_16:		DisplayPage=PARA_SET_17; break;
					case PARA_SET_17:		DisplayPage=PARA_SET_18; break;
					case PARA_SET_18:		DisplayPage=PARA_SET_19; break;
					case PARA_SET_19:		DisplayPage=PARA_SET_20; break;
					case PARA_SET_20:		DisplayPage=PARA_SET_21; break;
					case PARA_SET_21:		DisplayPage=PARA_SET_22; break;
					case PARA_SET_22:		DisplayPage=PARA_SET_23; break;
					case PARA_SET_23:		DisplayPage=PARA_SET_24; break;
					case PARA_SET_24:		DisplayPage=PARA_SET_25; break;
                                        case PARA_SET_25:		DisplayPage=PARA_SET_26; break;
					case PARA_SET_26:		DisplayPage=PARA_SET_27; break;
                                        case PARA_SET_27:               DisplayPage=PARA_SET_28; break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_28:               DisplayPage=PARA_SET_29; break;
                                        case PARA_SET_29:               DisplayPage=PARA_SET_30; break;
                                        #else
                                        case PARA_SET_28:               DisplayPage=PARA_SET_30; break;
                                        #endif
                                        case PARA_SET_30:               DisplayPage=PARA_SET_31; break;
                                        case PARA_SET_31:               DisplayPage=PARA_SET_32; break;
                                        case PARA_SET_32:               DisplayPage=PARA_SET_33; break;
                                        case PARA_SET_33:               DisplayPage=PARA_SET_34; break;
                                        case PARA_SET_34:               DisplayPage=PARA_SET_35; break;
                                        #if(CVS_CIP==CVS_CIP_NO)
                                        case PARA_SET_35:               DisplayPage=PARA_SET_36; break;
                                        case PARA_SET_36:               DisplayPage=PARA_SET_37; break;
                                        case PARA_SET_37:               DisplayPage=PARA_SET_38; break;
                                        case PARA_SET_38:               DisplayPage=PARA_SET_39; break;
                                        #endif      
                                        case PARA_SET_39:               DisplayPage=PARA_SET_40; break;
                                        case PARA_SET_40:               DisplayPage=PARA_SET_41; break;
                                        case PARA_SET_41:               DisplayPage=PARA_SET_42; break;
                                        case PARA_SET_42:               DisplayPage=PARA_SET_43; break;
                                        case PARA_SET_43:               DisplayPage=PARA_SET_44; break;
                                        
                                        case HW_TEST_1:			DisplayPage=HW_TEST_2; break;
					case HW_TEST_2:			DisplayPage=HW_TEST_3; break;
                                        
                                        case SENSOR_READ_1:             DisplayPage=SENSOR_READ_2; break;
					case SENSOR_READ_2:             DisplayPage=SENSOR_READ_3; break;
					case SENSOR_READ_3:             DisplayPage=SENSOR_READ_4; break;
					case SENSOR_READ_4:             DisplayPage=SENSOR_READ_5; break;
					
					case OUTPUT_TEST_1:		DisplayPage=OUTPUT_TEST_2; break;
					case OUTPUT_TEST_2:		DisplayPage=OUTPUT_TEST_3; break;
					case OUTPUT_TEST_3:		DisplayPage=OUTPUT_TEST_4; break;
					case OUTPUT_TEST_4:		DisplayPage=OUTPUT_TEST_5; break;
					case OUTPUT_TEST_5:		DisplayPage=OUTPUT_TEST_6; break;
					case OUTPUT_TEST_6: 	        DisplayPage=OUTPUT_TEST_7; break;
					case OUTPUT_TEST_7: 	        DisplayPage=OUTPUT_TEST_8; break;
					case OUTPUT_TEST_8:		DisplayPage=OUTPUT_TEST_9; break;
                                        case OUTPUT_TEST_9:		DisplayPage=OUTPUT_TEST_10; break;
                                        case OUTPUT_TEST_10:		DisplayPage=OUTPUT_TEST_11; break;
					//case OUTPUT_TEST_9:   break;
                    
					case USER_FUNCTIONS_0:	        DisplayPage=USER_FUNCTIONS_1; break;
					case USER_FUNCTIONS_1:	        DisplayPage=USER_FUNCTIONS_2; break;
					case USER_FUNCTIONS_2:	        DisplayPage=USER_FUNCTIONS_3; break;
					case USER_FUNCTIONS_3:	        DisplayPage=USER_FUNCTIONS_4; break;
					case USER_FUNCTIONS_4:	        DisplayPage=USER_FUNCTIONS_5; break;
					case USER_FUNCTIONS_5:	        DisplayPage=USER_FUNCTIONS_6; break;
					case USER_FUNCTIONS_6:	        DisplayPage=USER_FUNCTIONS_7; break;
                                        
                                        case USER_FUNCTIONS_4_0:        DisplayPage=USER_FUNCTIONS_4_1;break;
					case USER_FUNCTIONS_5_0:        DisplayPage=USER_FUNCTIONS_5_1;break;
					
					case AUTO_0:	                DisplayPage = AUTO_0_1; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_1:                  DisplayPage = AUTO_0_2; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_2:                  DisplayPage = AUTO_0_3; /*KeyTimeOut = FreeRunSec;*/ break;
                                        case AUTO_0_3:                  DisplayPage = AUTO_0_4; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_4:                  DisplayPage = AUTO_0_5; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_5:                  DisplayPage = AUTO_0_6; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_6:                  DisplayPage = AUTO_0_7; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_7:                  DisplayPage = AUTO_0_8; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_8:                  DisplayPage = AUTO_0_9; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_9_0:		DisplayPage = AUTO_0_9_1; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_1:		DisplayPage = AUTO_0_9_2; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_9_2:		DisplayPage = AUTO_0_9_3; /*KeyTimeOut = FreeRunSec;*/ break;
					case AUTO_0_9_3:		DisplayPage = AUTO_0_9_4; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_4:		DisplayPage = AUTO_0_9_6; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_5:		DisplayPage = AUTO_0_9_6; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_6:		DisplayPage = AUTO_0_9_7; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_7:		DisplayPage = AUTO_0_9_8; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_8:		DisplayPage = AUTO_0_9_9; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_9:		DisplayPage = AUTO_0_9_10; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_10:		DisplayPage = AUTO_0_9_11; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_11:		DisplayPage = AUTO_0_9_12; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_12:		DisplayPage = AUTO_0_9_13; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_13:		DisplayPage = AUTO_0_9_14; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_14:		DisplayPage = AUTO_0_9_15; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_15:		DisplayPage = AUTO_0_9_16; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_16:		DisplayPage = AUTO_0_9_17; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_17:		DisplayPage = AUTO_0_9_18; /*KeyTimeOut = FreeRunSec;*/break;
					case AUTO_0_9_18:		DisplayPage = AUTO_0_9_19; /*KeyTimeOut = FreeRunSec;*/break;
					
				}
			break;
			case KEY_ENTER:
				switch(DisplayPage)
				{	case LOGOUT_0:			SwitchFromLogoutToAutoMode(0); break;
					case LOGOUT_1:			DisplayPage=DEV_INFO_0; break;
					case LOGOUT_2:			DisplayPage=PASSWORD_0;KeyPressCounter=0;strcpy(SerialHmiPassword,"0000");break;
					case LOGOUT_3:			DisplayPage=USER_FUNCTIONS_0; break;
					case PASSWORD_0:		if(KeyPressCounter < 3)
                                            {	KeyPressCounter++;
                                            }
											else
											{	//check password
													//sprintf(PasteBuff,"\r\nInput Pass = %s, Dev.Pass=%s\r\n",SerialHmiPassword,Device.Password);
													//PutStringUsb(PasteBuff);
													if(strcmp(SerialHmiPassword,Device.Password)==0)
															DisplayPage=MENU_1;//PutStringUsb("\r\nPassword Matched");
													else
															DisplayPage=PASSWORD_ERR;
											}
											break;
					//case PASSWORD_ERR:		break;
					
					case DEV_INFO_0:		DisplayPage=DEV_INFO_0_1;break;
					case DEV_INFO_1:		DisplayPage=DEV_INFO_1_1;break;
					case DEV_INFO_2:		DisplayPage=DEV_INFO_2_1;break;
					case DEV_INFO_3:		DisplayPage=DEV_INFO_3_1;break;
					//case DEV_INFO_4:		DisplayPage=DEV_INFO_4_1;break;
					//case DEV_INFO_0_1:		break;
					//case DEV_INFO_1_1:		break;
					//case DEV_INFO_2_1:		break;
					//case DEV_INFO_3_1:		break;
					//case DEV_INFO_4_1:		break;
					
					//case MENU_0:			DisplayPage=AUTO_0;Mode = MODE_AUTO;CurrentProcessStep = IDLE;break;
					case MENU_1:			DisplayPage = PARA_SET_1; break;
					//case MENU_2:			DisplayPage=SYS_SETTING_0; break;
					case MENU_3:			DisplayPage=HW_TEST_1; break;
					
					case PARA_SET_2:		VariableUnsignedInt2UserInput(PrepurifyTimeInSec);
											DisplayPage=PARA_SET_2_0;break;
											
					case PARA_SET_3:		VariableUnsignedInt2UserInput(PurifyTimeInSec);
											DisplayPage=PARA_SET_3_0;break;
											
					case PARA_SET_4:		VariableUnsignedInt2UserInput(WasteTimeInSec);
											DisplayPage=PARA_SET_4_0;break;
											
					case PARA_SET_5:		VariableUnsignedInt2UserInput(HF_WasteTimeInSec);
											DisplayPage=PARA_SET_5_0;break;
					
					case PARA_SET_6:		VariableUnsignedInt2UserInput(CIP_DosingOnTimeInSec);
											DisplayPage=PARA_SET_6_0;break;
					
					case PARA_SET_7:		VariableUnsignedInt2UserInput(CIP_DosingOffTimeInSec);
											DisplayPage=PARA_SET_7_0;break;
					#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        case PARA_SET_8:		VariableUnsignedInt2UserInput(CIP_DosingTotalTimeInSec);
											DisplayPage=PARA_SET_8_0;break;
					
					case PARA_SET_9:		VariableUnsignedInt2UserInput(CIP_PulseTotalTimeInSec);
											DisplayPage=PARA_SET_9_0;break;
					
					case PARA_SET_10:		VariableUnsignedInt2UserInput(CIP_PulseOnTimeInSec);
											DisplayPage=PARA_SET_10_0;break;
					
					case PARA_SET_11:		VariableUnsignedInt2UserInput(CIP_PulseOffTimeInSec);
											DisplayPage=PARA_SET_11_0;break;
                                        #endif
					case PARA_SET_12:		VariableUnsignedInt2UserInput(CIP_FlushTimeInSec);
											DisplayPage=PARA_SET_12_0;break;
					
					case PARA_SET_13:		VariableUnsignedInt2UserInput(WV_CheckTimeInSec);
											DisplayPage=PARA_SET_13_0;break;
					
					case PARA_SET_14:		VariableUnsignedInt2UserInput(WaitHT_TimeInSec);
											DisplayPage=PARA_SET_14_0;break;
					
					case PARA_SET_15:		SerialHmiPassword[3] = TargetPureFlowLperM%10+48;
											SerialHmiPassword[2] = '.';
											SerialHmiPassword[1] = (TargetPureFlowLperM/10)%10+48;
											SerialHmiPassword[0] = (TargetPureFlowLperM/100)%10+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_15_0;break;
					
					case PARA_SET_1:		VariableUnsignedInt2UserInput(TargetOutletConductivity);
											DisplayPage=PARA_SET_1_0;
                                                                                        break;
					
					
					case PARA_SET_16:		SerialHmiPassword[3] = LowFlowPurifyAlarmSetpoint%10+48; /*****************ARUN******/
											SerialHmiPassword[2] = '.';
											SerialHmiPassword[1] = LowFlowPurifyAlarmSetpoint/10+48;
											SerialHmiPassword[0] = LowFlowPurifyAlarmSetpoint/100+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_16_0;break;
					
					case PARA_SET_17:		SerialHmiPassword[3] = LowFlowWasteAlarmSetpoint%10+48; /*********ARUN*********/
											SerialHmiPassword[2] = '.';
											SerialHmiPassword[1] = LowFlowWasteAlarmSetpoint/10+48;
											SerialHmiPassword[0] = LowFlowWasteAlarmSetpoint/100+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_17_0;break;
					
					case PARA_SET_18:		VariableUnsignedInt2UserInput(MaxCycleCountForCIP);
											DisplayPage=PARA_SET_18_0;break;
					
					case PARA_SET_19:		VariableUnsignedInt2UserInput(CabinetHighTemperatureSetVal);
											DisplayPage=PARA_SET_19_0;break;
					
					case PARA_SET_20:		VariableUnsignedInt2UserInput(MAX_CIP_PumpRunTimeInSec);
											DisplayPage=PARA_SET_20_0;break;
											
					case PARA_SET_21:		SerialHmiPassword[3] = AnaOutProportionalFactor%10+48;
											SerialHmiPassword[2] = (AnaOutProportionalFactor/10)%10+48;
											SerialHmiPassword[1] = '.';
											SerialHmiPassword[0] = AnaOutProportionalFactor/100+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_21_0;break;
											
					case PARA_SET_22:		SerialHmiPassword[3] = DynaProportionalFactor%10+48;
											SerialHmiPassword[2] = (DynaProportionalFactor/10)%10+48;
											SerialHmiPassword[1] = '.';
											SerialHmiPassword[0] = (DynaProportionalFactor/100)%10+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_22_0;break;
                    
                                        case PARA_SET_23:		SerialHmiPassword[3] = PAE_PureStepVoltageSetpoint%10+48;
											SerialHmiPassword[2] = '.';
											SerialHmiPassword[1] = (PAE_PureStepVoltageSetpoint/10)%10+48;
											SerialHmiPassword[0] = (PAE_PureStepVoltageSetpoint/100)%10+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_23_0;break;
											
					case PARA_SET_24:		SerialHmiPassword[3] = PAE_WasteStepVoltageSetpoint%10+48;
											SerialHmiPassword[2] = '.';
											SerialHmiPassword[1] = (PAE_WasteStepVoltageSetpoint/10)%10+48;
											SerialHmiPassword[0] = (PAE_WasteStepVoltageSetpoint/100)%10+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_24_0;break;
                    
                                        case PARA_SET_25:		SerialHmiPassword[3] = CurrentProportionalFactor%10+48;
											SerialHmiPassword[2] = (CurrentProportionalFactor/10)%10+48;
											SerialHmiPassword[1] = '.';
											SerialHmiPassword[0] = (CurrentProportionalFactor/100)%10+48;
											KeyPressCounter=0; DisplayPage=PARA_SET_25_0;break;
											
					case PARA_SET_26:		VariableUnsignedInt2UserInput(FlowMeter.K_Factor);
											DisplayPage=PARA_SET_26_0;break;
					case PARA_SET_27:               DisplayPage = PARA_SET_27_0; break;
                                        case PARA_SET_28:		if(VolumeUnit == VOLUME_UNIT_LITTER)
                                                DisplayPage = PARA_SET_28_0;
                                            else
                                                DisplayPage = PARA_SET_28_1;
                                            break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_29:       switch(BypassOption)
                                                                {   default:
                                                                    case BYPASS_INTERNAL:   DisplayPage = PARA_SET_29_0; break;
                                                                    case BYPASS_EXTERNAL_NC:DisplayPage = PARA_SET_29_1; break;
                                                                    case BYPASS_EXTERNAL_NO:DisplayPage = PARA_SET_29_2; break;
                                                                }
                                            break;
                                        #endif
                                        case PARA_SET_30:       SerialHmiPassword[3] = AnalogPressureStartLevel%10+48;
								SerialHmiPassword[2] = (AnalogPressureStartLevel/10)%10+48;
								SerialHmiPassword[1] = '.';
								SerialHmiPassword[0] = (AnalogPressureStartLevel/100)%10+48;
								KeyPressCounter=0; 
                                                                DisplayPage = PARA_SET_30_0; 
                                                                break;
                                        case PARA_SET_31:       SerialHmiPassword[3] = AnalogPressureStopLevel%10+48;
                                                                SerialHmiPassword[2] = (AnalogPressureStopLevel/10)%10+48;
                                                                SerialHmiPassword[1] = '.';
                                                                SerialHmiPassword[0] = (AnalogPressureStopLevel/100)%10+48;
                                                                KeyPressCounter=0; 
                                                                DisplayPage = PARA_SET_31_0; 
                                                                break;
                                        case PARA_SET_32:       SerialHmiPassword[3] = AnalogPressureBypassLevel%10+48;
                                                                SerialHmiPassword[2] = (AnalogPressureBypassLevel/10)%10+48;
                                                                SerialHmiPassword[1] = '.';
                                                                SerialHmiPassword[0] = (AnalogPressureBypassLevel/100)%10+48;
                                                                KeyPressCounter=0; 
                                                                DisplayPage = PARA_SET_32_0; 
                                                                break;
                                        case PARA_SET_33:       SerialHmiPassword[3] = AnalogPressureCipLevel%10+48;
                                                                SerialHmiPassword[2] = (AnalogPressureCipLevel/10)%10+48;
                                                                SerialHmiPassword[1] = '.';
                                                                SerialHmiPassword[0] = (AnalogPressureCipLevel/100)%10+48;
                                                                KeyPressCounter=0; 
                                                                DisplayPage = PARA_SET_33_0; 
                                                                break;
                                        case PARA_SET_34:		VariableUnsignedInt2UserInput(CIP_WaitTimeInSec);
                                                                                                            DisplayPage=PARA_SET_34_0;break;
                                        case PARA_SET_35:		VariableUnsignedInt2UserInput(WaitTimeB4BypassAlarmInSec);
                                                                                                            DisplayPage=PARA_SET_35_0;break;
                                        #if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_NO))
                                        case PARA_SET_36:		VariableUnsignedInt2UserInput(CIP_LowerLimitCycle);
                                                                                                            DisplayPage=PARA_SET_36_0;break;
                                        case PARA_SET_37:		VariableUnsignedInt2UserInput(CIP_FavorableTime);
                                                                                                            DisplayPage=PARA_SET_37_0;break;
                                        case PARA_SET_38:		VariableUnsignedInt2UserInput(CIP_FavorableTimeGraceMin);
                                                                                                            DisplayPage=PARA_SET_38_0;break;
                                        #endif
                                        case PARA_SET_39:           if(DynamicCapDIMode == DYNAMIC_CAPDI_MODE_ON)
                                                                    DisplayPage = PARA_SET_39_0;
                                                                    else
                                                                    DisplayPage = PARA_SET_39_1;
                                                                    break;
                                        #if(DynamicCapDIMode == DYNAMIC_CAPDI_MODE_ON)
                                          case PARA_SET_40:	VariableUnsignedInt2UserInput(PrePureCurrentSetpoint);
                                                                                        KeyPressCounter = 1;
											DisplayPage=PARA_SET_40_0;break;
                                          case PARA_SET_41:	VariableUnsignedInt2UserInput(PureCurrentSetpoint);
                                                                                        KeyPressCounter = 1;
											DisplayPage=PARA_SET_41_0;break;
                                          case PARA_SET_42:	VariableUnsignedInt2UserInput(WasteCurrentSetpoint);
                                                                                        KeyPressCounter = 1;
											DisplayPage=PARA_SET_42_0;break;
                                                                                        
                                        #endif                           
                                        case PARA_SET_43:	VariableUnsignedInt2UserInput(ShuntPostWasteStepTimeInSec);
											DisplayPage=PARA_SET_43_0;break;
                                        case PARA_SET_44:	VariableUnsignedInt2UserInput(ShuntPostPureStepTimeInSec);
											DisplayPage=PARA_SET_44_0;break;
                                        case OUTPUT_TEST_11:    VariableUnsignedInt2UserInput(PumpManualSpeed);
											DisplayPage=OUTPUT_TEST_11_0;break;                                            
                                        
                                        case PARA_SET_1_0:     if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
                                                #if(PURE_EC_TARGET_MIN_VAL != 0)
                                                if(uiTemp >= PURE_EC_TARGET_MIN_VAL && uiTemp <= PURE_EC_TARGET_MAX_VAL)
                                                #else
												if(uiTemp <= PURE_EC_TARGET_MAX_VAL)
                                                #endif
                                                {	TargetOutletConductivity = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECa,u.c,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_1_1;
												}
                                            }
											break;
					case PARA_SET_2_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= PREPURIFY_TIME_MIN_VAL && uiTemp <= PREPURIFY_TIME_MAX_VAL){
													PrepurifyTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(PREPURIFY_TIME_IN_SECa,u.c,PREPURIFY_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}
												else
												{
													DisplayPage = PARA_SET_2_1;
												}			
											}
											break;
					case PARA_SET_3_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= PURIFY_TIME_MIN_VAL && uiTemp <= PURIFY_TIME_MAX_VAL){
													PurifyTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(PURIFY_TIME_IN_SECa,u.c,PURIFY_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}
												else
												{
													DisplayPage = PARA_SET_3_1;
												}
											}
											break;
					case PARA_SET_4_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp>= WASTE_TIME_MIN_VAL && uiTemp <= WASTE_TIME_MAX_VAL){
													WasteTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(WASTE_TIME_IN_SECa,u.c,WASTE_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_4_1;
												}
											}
											break;
					case PARA_SET_5_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
                                                #if(HF_WASTE_TIME_MIN_VAL != 0)
                                                if(uiTemp >= HF_WASTE_TIME_MIN_VAL && uiTemp <= HF_WASTE_TIME_MAX_VAL)
                                                #else
                                                if(uiTemp <= HF_WASTE_TIME_MAX_VAL)
                                                #endif
                                                {	HF_WasteTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(HF_WASTE_TIME_IN_SECa,u.c,HF_WASTE_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_5_1;
												}
											}
											break;
					case PARA_SET_6_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= CIP_DOSE_ON_TIME_MIN_VAL && uiTemp <= CIP_DOSE_ON_TIME_MAX_VAL){
													CIP_DosingOnTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_DOSING_ON_TIME_IN_SECa,u.c,CIP_DOSING_ON_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_6_1;
												}
											}
											break;
					case PARA_SET_7_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
                                                if(uiTemp >= CIP_DOSE_OFF_TIME_MIN_VAL && uiTemp <= CIP_DOSE_OFF_TIME_MAX_VAL){
													CIP_DosingOffTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_DOSING_OFF_TIME_IN_SECa,u.c,CIP_DOSING_OFF_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_7_1;
												}
                                            }
											break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                            case PARA_SET_8_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= CIP_DOSE_TOTAL_TIME_MIN_VAL && uiTemp <= CIP_DOSE_TOTAL_TIME_MAX_VAL){
													CIP_DosingTotalTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_DOSING_TOTAL_TIME_IN_SECa,u.c,CIP_DOSING_TOTAL_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_8_1;
												}
                                            }
											break;
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        case PARA_SET_9_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= CIP_PULSE_TOTAL_TIME_MIN_VAL && uiTemp <= CIP_PULSE_TOTAL_TIME_MAX_VAL){
													CIP_PulseTotalTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa,u.c,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn);
                                                                        strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_9_1;
												}
											}
											break;
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        case PARA_SET_10_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
                                                if(uiTemp >= CIP_PULSE_ON_TIME_MIN_VAL && uiTemp <= CIP_PULSE_ON_TIME_MAX_VAL){
													CIP_PulseOnTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_FLOW_PULSE_ON_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_ON_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_10_1;
												}
                                            }
											break;
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        case PARA_SET_11_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= CIP_PULSE_OFF_TIME_MIN_VAL && uiTemp <= CIP_PULSE_OFF_TIME_MAX_VAL){
													CIP_PulseOffTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_FLOW_PULSE_OFF_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_OFF_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_11_1;
												}
											}
											break;
                                        #endif
                                        case PARA_SET_12_0:		if(KeyPressCounter < 3) ///////////////////////////////////////////////////////////////////////
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= CIP_FLUSH_TIME_MIN_VAL && uiTemp <= CIP_FLUSH_TIME_MAX_VAL){
													CIP_FlushTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CIP_FLUSH_TIME_IN_SECa,u.c,CIP_FLUSH_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_12_1;
												}
											}
											break;
					case PARA_SET_13_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
                                                                                                if(uiTemp >= WV_CHECK_TIME_MIN_VAL && uiTemp <= WV_CHECK_TIME_MAX_VAL){
													WV_CheckTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(WV_CHECK_TIME_IN_SECa,u.c,WV_CHECK_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_13_1;
												}
                                            }
											break;
					case PARA_SET_14_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= WAIT_HT_TIME_MIN_VAL && uiTemp <= WAIT_HT_TIME_MAX_VAL){
													WaitHT_TimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(WAIT_HT_TIME_IN_SECa,u.c,WAIT_HT_TIME_IN_SECn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_14_1;
												}
											}
											break;
					case PARA_SET_15_0:		if(KeyPressCounter < 3){                    ///////////////////////////ARUN
													KeyPressCounter++;
													if(KeyPressCounter ==2)
														KeyPressCounter++;
											}
											else
											{	uiTemp = SerialHmiPassword[0] - 48;
												uiTemp *= 10;
												uiTemp += SerialHmiPassword[1] - 48;
												uiTemp *= 10;
												uiTemp += SerialHmiPassword[3] - 48;
												if(uiTemp <= TARGET_PURE_FLOW_MAX_VAL){
                                                    TargetPureFlowLperM = uiTemp;
                                                    u.i[0] = uiTemp;
                                                    RandomMemWrite(TARGET_PURE_FLOW_LITER_PER_MINa,u.c,TARGET_PURE_FLOW_LITER_PER_MINn);
                                                    strcpy(LcdLine2,"     SAVED      ");
                                                    SendSerialHmiPacket();
                                                }else
                                                {
                                                    DisplayPage = PARA_SET_15_1;
                                                }
                                            }
											break;
					case PARA_SET_16_0:		if(KeyPressCounter < 3){ ////////////////////////////////////ARUN
												KeyPressCounter++;
												if(KeyPressCounter ==2)
													KeyPressCounter++;
											}
											else
											{	uiTemp = SerialHmiPassword[0] - 48;
												uiTemp *= 10;
												uiTemp += SerialHmiPassword[1] - 48;
												uiTemp *= 10;
												uiTemp += SerialHmiPassword[3] - 48;
												if(uiTemp <= PURIFY_LOW_FLOW_MAX_VAL){ 
													LowFlowPurifyAlarmSetpoint = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(LOW_FLOW_PURIFY_ALARM_SET_POINTa,u.c,LOW_FLOW_PURIFY_ALARM_SET_POINTn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_16_1;
												}
											}
											break;
					case PARA_SET_17_0:		if(KeyPressCounter < 3){        ////////////////////////////////////ARUN
													KeyPressCounter++;
													if(KeyPressCounter ==2)
													  KeyPressCounter++;
											}
											else
											{	uiTemp = SerialHmiPassword[0] - 48;
												uiTemp *= 10;
												uiTemp += SerialHmiPassword[1] - 48;
												uiTemp *= 10;
												uiTemp += SerialHmiPassword[3] - 48;
												if(uiTemp <= WASTE_LOW_FLOW_MAX_VAL){ 
													LowFlowWasteAlarmSetpoint = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(LOW_FLOW_WASTE_ALARM_SET_POINTa,u.c,LOW_FLOW_WASTE_ALARM_SET_POINTn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_17_1;
												}
											}
											break;
					case PARA_SET_18_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL && uiTemp <= MAX_CYCLE_COUNT_FOR_CIP_MAX_VAL){
													MaxCycleCountForCIP = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(MAX_CYCLE_RUNCOUNT_FOR_CIPa,u.c,MAX_CYCLE_RUNCOUNT_FOR_CIPn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_18_1;
												}
											}
											break;
					case PARA_SET_19_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
												if(uiTemp >= CABINET_HIGH_TEMP_MIN_VAL && uiTemp <= CABINET_HIGH_TEMP_MAX_VAL){
													CabinetHighTemperatureSetVal = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(CABINET_HIGH_TEMPERATURE_SET_VALa,u.c,CABINET_HIGH_TEMPERATURE_SET_VALn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_19_1;
												}
											}
											break;
					case PARA_SET_20_0:		if(KeyPressCounter < 3)
													KeyPressCounter++;
											else
											{	uiTemp = UserInputToUnsignedInt();
                                                                                                if(uiTemp >= MAX_CIP_PUMP_RUN_TIME_MIN_VAL && uiTemp <= MAX_CIP_PUMP_RUN_TIME_MAX_VAL){
													MAX_CIP_PumpRunTimeInSec = uiTemp;
													u.i[0] = uiTemp;
													RandomMemWrite(MAX_CIP_PUMP_RUNTIMEa,u.c,MAX_CIP_PUMP_RUNTIMEn);
													strcpy(LcdLine2,"     SAVED      ");
													SendSerialHmiPacket();
												}else
												{
													DisplayPage = PARA_SET_20_1;
												}
                                            }
											break;
                            case PARA_SET_21_0:		if(KeyPressCounter < 3){
                                                                                                          KeyPressCounter++;
                                                                                          if(KeyPressCounter == 1)
                                                                                                          KeyPressCounter =2;
                                                                                          }
                                                                                          else
                                                                                          {	uiTemp = SerialHmiPassword[0] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[2] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[3] - 48;
                                                                                                  if(uiTemp >= ANA_OUT_P_FACTOR_MIN_VAL && uiTemp <= ANA_OUT_P_FACTOR_MAX_VAL){       //in actual 0.25<= P.Factor <= 2.00  ..since we are working as integer 200 is needed
                                                                                                    AnaOutProportionalFactor = uiTemp;
                                                                                                    u.i[0] = uiTemp;
                                                                                                    RandomMemWrite(PUMP_P_FACTORa,u.c,PUMP_P_FACTORn);
                                                                                                    strcpy(LcdLine2,"     SAVED      ");
                                                                                                    SendSerialHmiPacket();
                                                                                                  }else
                                                                                                  {
                                                                                                                  DisplayPage = PARA_SET_21_1;
                                                                                                  }
                                                                                          }
                                                                                          break;
                              case PARA_SET_22_0:		if(KeyPressCounter < 3){
                                                                                                          KeyPressCounter++;
                                                                                                          if(KeyPressCounter == 1)
                                                                                                            KeyPressCounter =2;
                                                                                          }else
                                                                                          {	uiTemp = SerialHmiPassword[0] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[2] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[3] - 48;
                                                                                                  if(uiTemp <= DYNA_P_FACTOR_MAX_VAL){	//its 9.99 in actual..since we are working as integer 999 is needed
                                                                                                                  DynaProportionalFactor = uiTemp;
                                                                                                                  u.i[0] = uiTemp;
                                                                                                                  RandomMemWrite(DYNAMIC_P_FACTORa,u.c,DYNAMIC_P_FACTORn);
                                                                                                                  strcpy(LcdLine2,"     SAVED      ");
                                                                                                                  SendSerialHmiPacket();
                                                                                                  }else
                                                                                                  {
                                                                                                                  DisplayPage = PARA_SET_22_1;
                                                                                                  }
                                                                                          }
                                                                                          break;
                              case PARA_SET_23_0:		if(KeyPressCounter < 3){
                                                                                                          KeyPressCounter++;
                                                                                                          if(KeyPressCounter == 2)
                                                                                                            KeyPressCounter =3;
                                                                                          }else
                                                                                          {	uiTemp = SerialHmiPassword[0] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[1] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[3] - 48;
                                                                                                  if(uiTemp <= PAE_PURE_STEP_VOLT_MAX_VAL){//min val 00.0 max val 12.0 in actual so for actual value divide by 10
                                                                                                    PAE_PureStepVoltageSetpoint = uiTemp;
                                                                                                    u.i[0] = uiTemp;
                                                                                                    RandomMemWrite(PAE_PURE_CYCLE_VOLTAGEa,u.c,PAE_PURE_CYCLE_VOLTAGEn);
                                                                                                    strcpy(LcdLine2,"     SAVED      ");
                                                                                                    SendSerialHmiPacket();
                                                                                                  }else
                                                                                                  {
                                                                                                                  DisplayPage = PARA_SET_23_1;
                                                                                                  }
                                                                                          }
                                                                                          break;
                              case PARA_SET_24_0:		if(KeyPressCounter < 3){
                                                                                                          KeyPressCounter++;
                                                                                                          if(KeyPressCounter == 2)
                                                                                                            KeyPressCounter =3;
                                                                                          }else
                                                                                          {	uiTemp = SerialHmiPassword[0] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[1] - 48;
                                                                                                  uiTemp *= 10;
                                                                                                  uiTemp += SerialHmiPassword[3] - 48;
                                                                                                  if(uiTemp <= PAE_WASTE_STEP_VOLT_MAX_VAL){	//min val 00.0 max val 12.0 in actual so for actual value divide by 10
                                                                                                                  PAE_WasteStepVoltageSetpoint = uiTemp;
                                                                                                                  u.i[0] = uiTemp;
                                                                                                                  RandomMemWrite(PAE_WASTE_CYCLE_VOLTAGEa,u.c,PAE_WASTE_CYCLE_VOLTAGEn);
                                                                                                                  strcpy(LcdLine2,"     SAVED      ");
                                                                                                                  SendSerialHmiPacket();
                                                                                                  }else
                                                                                                  {
                                                                                                                  DisplayPage = PARA_SET_24_1;
                                                                                                  }
                                                                                          }
                                                                                          break;
                              case PARA_SET_25_0:		if(KeyPressCounter < 3){
                                                                                                          KeyPressCounter++;
                                                                                                          if(KeyPressCounter == 1)
                                                                                                            KeyPressCounter =2;
                                                                                          }else
                                                                                          {		  uiTemp = SerialHmiPassword[0] - 48;
                                                                                                          //uiTemp *= 10;
                                                                                                          //uiTemp += SerialHmiPassword[1] - 48;
                                                                                                          uiTemp *= 10;
                                                                                                          uiTemp += SerialHmiPassword[2] - 48;
                                                                                                          uiTemp *= 10;
                                                                                                          uiTemp += SerialHmiPassword[3] - 48;
                                                                                                          if(uiTemp >= CURRENT_P_FACTOR_MIN_VAL && uiTemp <= CURRENT_P_FACTOR_MAX_VAL){
                                                                                                                          CurrentProportionalFactor = uiTemp;
                                                                                                                          u.i[0] = uiTemp;
                                                                                                                          RandomMemWrite(WASTE_VALUE_CURRENTa,u.c,WASTE_VALUE_CURRENTn);
                                                                                                                          strcpy(LcdLine2,"     SAVED      ");
                                                                                                                          SendSerialHmiPacket();
                                                                                                          }else
                                                                                                          {
                                                                                                                          DisplayPage = PARA_SET_25_1;
                                                                                                          }
                                                                                          }
                                                                                          break;
                            case PARA_SET_26_0:		if(KeyPressCounter < 3){
                                                                                                          KeyPressCounter++;
                                                                                                          
                                                                                          }else
                                                                                          {	uiTemp = UserInputToUnsignedInt();
                                                                                                  if(uiTemp >= FLOWMETER_K_FACTOR_MIN_VAL && uiTemp <= FLOWMETER_K_FACTOR_MAX_VAL){
                                                                                                                  FlowMeter.K_Factor = uiTemp;
                                                                                                                  u.i[0] = uiTemp;
                                                                                                                  RandomMemWrite(FLOWMETER_K_FACTORa,u.c,FLOWMETER_K_FACTORn);
                                                                                                                  strcpy(LcdLine2,"     SAVED      ");
                                                                                                                  SendSerialHmiPacket();
                                                                                                  }else
                                                                                                  {
                                                                                                                  DisplayPage = PARA_SET_26_1;
                                                                                                  }
                                                                                          }
                                                                                          break;
                            case PARA_SET_27_0:	    strcpy(LcdLine1,"Clearing Flows  ");
                                                    strcpy(LcdLine2,"Please Wait.....");
                                                    SendSerialHmiPacket();
                                                    ClearAllFlow();
                                                    strcpy(LcdLine2,"      DONE      ");
                                                    SendSerialHmiPacket();
                                                    PeriodicSave();
                                                    DelayInSec(2);
                                                                                                //DelayInMicroSec(2000000);
                                                    DisplayPage = PARA_SET_27;
                                                                                                break;
                            case PARA_SET_28_0:     VolumeUnit = VOLUME_UNIT_LITTER;
                                                    u.c[0] = VolumeUnit;
                                                    RandomMemWrite(VOLUME_UNITa,u.c,VOLUME_UNITn);
                                                    strcpy(LcdLine2,"     SAVED.     ");
                                                    SendSerialHmiPacket();
                                                    DelayInSec(1);
                                                    break;
                            case PARA_SET_28_1:     VolumeUnit = VOLUME_UNIT_GALLON;
                                                    u.c[0] = VolumeUnit;
                                                    RandomMemWrite(VOLUME_UNITa,u.c,VOLUME_UNITn);
                                                    strcpy(LcdLine2,"     SAVED.     ");
                                                    SendSerialHmiPacket();
                                                    DelayInSec(1);
                                                    break;
                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                            case PARA_SET_29_0:     BypassOption = BYPASS_INTERNAL;
                                                    u.c[0] = BypassOption;
                                                    RandomMemWrite(BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
                                                    strcpy(LcdLine2,"     SAVED.     ");
                                                                                                SendSerialHmiPacket();
                                                    DelayInSec(1);
                                                    break;
                            case PARA_SET_29_1:     BypassOption = BYPASS_EXTERNAL_NC;
                                                    u.c[0] = BypassOption;
                                                    RandomMemWrite(BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
                                                    strcpy(LcdLine2,"     SAVED.     ");
                                                                                                SendSerialHmiPacket();
                                                    DelayInSec(1);
                                                    break;
                            case PARA_SET_29_2:     BypassOption = BYPASS_EXTERNAL_NO;
                                                    u.c[0] = BypassOption;
                                                    RandomMemWrite(BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
                                                    strcpy(LcdLine2,"     SAVED.     ");
                                                                                                SendSerialHmiPacket();
                                                    DelayInSec(1);
                                                    break;
                            #endif
                            case PARA_SET_30_0:		if(KeyPressCounter < 3){
                                                                                                                KeyPressCounter++;
                                                                                                                if(KeyPressCounter == 1)
                                                                                                                  KeyPressCounter =2;
                                                                                                }else
                                                                                                {		uiTemp = SerialHmiPassword[0] - 48;
                                                                                                                //uiTemp *= 10;
                                                                                                                //uiTemp += SerialHmiPassword[1] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[2] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[3] - 48;
                                                                                                                if(uiTemp >= START_PRESSURE_MIN_VAL && uiTemp <= START_PRESSURE_MAX_VAL){
                                                                                                                                AnalogPressureStartLevel = uiTemp;
                                                                                                                                u.i[0] = uiTemp;
                                                                                                                                RandomMemWrite(ANALOG_PRESSURE_START_LEVELa,u.c,ANALOG_PRESSURE_START_LEVELn);
                                                                                                                                strcpy(LcdLine2,"     SAVED      ");
                                                                                                                                SendSerialHmiPacket();
                                                                                                                }else
                                                                                                                {
                                                                                                                                DisplayPage = PARA_SET_30_1;
                                                                                                                }
                                                                                                }
                                                                                                break;
                          case PARA_SET_31_0:		if(KeyPressCounter < 3){
                                                                                                                KeyPressCounter++;
                                                                                                                if(KeyPressCounter == 1)
                                                                                                                  KeyPressCounter =2;
                                                                                                }else
                                                                                                {		uiTemp = SerialHmiPassword[0] - 48;
                                                                                                                //uiTemp *= 10;
                                                                                                                //uiTemp += SerialHmiPassword[1] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[2] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[3] - 48;
                                                                                                                if(uiTemp >= STOP_PRESSURE_MIN_VAL && uiTemp <= STOP_PRESSURE_MAX_VAL){
                                                                                                                                AnalogPressureStopLevel = uiTemp;
                                                                                                                                u.i[0] = uiTemp;
                                                                                                                                RandomMemWrite(ANALOG_PRESSURE_STOP_LEVELa,u.c,ANALOG_PRESSURE_STOP_LEVELn);
                                                                                                                                strcpy(LcdLine2,"     SAVED      ");
                                                                                                                                SendSerialHmiPacket();
                                                                                                                }else
                                                                                                                {
                                                                                                                                DisplayPage = PARA_SET_31_1;
                                                                                                                }
                                                                                                }
                                                                                                break;
                          case PARA_SET_32_0:		if(KeyPressCounter < 3){
                                                                                                                KeyPressCounter++;
                                                                                                                if(KeyPressCounter == 1)
                                                                                                                  KeyPressCounter =2;
                                                                                                }else
                                                                                                {		uiTemp = SerialHmiPassword[0] - 48;
                                                                                                                //uiTemp *= 10;
                                                                                                                //uiTemp += SerialHmiPassword[1] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[2] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[3] - 48;
                                                                                                                if(uiTemp <= BYPASS_PRESSURE_MAX_VAL){
                                                                                                                                AnalogPressureBypassLevel = uiTemp;
                                                                                                                                u.i[0] = uiTemp;
                                                                                                                                RandomMemWrite(ANALOG_PRESSURE_BYPASS_LEVELa,u.c,ANALOG_PRESSURE_BYPASS_LEVELn);
                                                                                                                                strcpy(LcdLine2,"     SAVED      ");
                                                                                                                                SendSerialHmiPacket();
                                                                                                                }
                                                                                                                else{
                                                                                                                                DisplayPage = PARA_SET_32_1;
                                                                                                                }
                                                                                                }
                                                                                                break;
                          case PARA_SET_33_0:		if(KeyPressCounter < 3){
                                                                                                                KeyPressCounter++;
                                                                                                                if(KeyPressCounter == 1)
                                                                                                                  KeyPressCounter =2;
                                                                                                }else
                                                                                                {		uiTemp = SerialHmiPassword[0] - 48;
                                                                                                                //uiTemp *= 10;
                                                                                                                //uiTemp += SerialHmiPassword[1] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[2] - 48;
                                                                                                                uiTemp *= 10;
                                                                                                                uiTemp += SerialHmiPassword[3] - 48;
                                                                                                                if(uiTemp >= CIP_PRESSURE_MIN_VAL && uiTemp <= CIP_PRESSURE_MAX_VAL){
                                                                                                                                AnalogPressureCipLevel = uiTemp;
                                                                                                                                u.i[0] = uiTemp;
                                                                                                                                RandomMemWrite(ANALOG_PRESSURE_CIP_LEVELa,u.c,ANALOG_PRESSURE_CIP_LEVELn);
                                                                                                                                strcpy(LcdLine2,"     SAVED      ");
                                                                                                                                SendSerialHmiPacket();
                                                                                                                }else
                                                                                                                {
                                                                                                                                DisplayPage = PARA_SET_33_1;
                                                                                                                }
                                                                                                }
                                                                                                break;
                          case PARA_SET_34_0:		if(KeyPressCounter < 3)
                                                                                                                KeyPressCounter++;
                                                                                                else
                                                                                                {	uiTemp = UserInputToUnsignedInt();
                                                                                                        #if(CIP_WAIT_TIME_MIN_VAL != 0)
                                                                                                        if(uiTemp >= CIP_WAIT_TIME_MIN_VAL && uiTemp <= CIP_WAIT_TIME_MAX_VAL)
                                                                                                        #else
                                                                                                        if(uiTemp <= CIP_WAIT_TIME_MAX_VAL)
                                                                                                        #endif
                                                                                                        {	CIP_WaitTimeInSec = uiTemp;
                                                                                                                u.i[0] = uiTemp;
                                                                                                                RandomMemWrite(CIP_WAIT_TIME_IN_SECa,u.c,CIP_WAIT_TIME_IN_SECn);
                                                                                                                strcpy(LcdLine2,"     SAVED      ");
                                                                                                                SendSerialHmiPacket();
                                                                                                        }else
                                                                                                        {
                                                                                                                DisplayPage = PARA_SET_34_1;
                                                                                                        }
                                                                                                }
                                                                                                break;
                          case PARA_SET_35_0:		if(KeyPressCounter < 3)
                                                                                                                KeyPressCounter++;
                                                                                                else
                                                                                                {	uiTemp = UserInputToUnsignedInt();
                                                        #if(WAIT_TIME_B4_BYPASS_ALARM_MIN_VAL != 0)
                                                        if(uiTemp >= WAIT_TIME_B4_BYPASS_ALARM_MIN_VAL && uiTemp <= WAIT_TIME_B4_BYPASS_ALARM_MAX_VAL)
                                                        #else
                                                        if(uiTemp <= WAIT_TIME_B4_BYPASS_ALARM_MAX_VAL)
                                                        #endif
                                                        {	WaitTimeB4BypassAlarmInSec = uiTemp;
                                                                                                                u.i[0] = uiTemp;
                                                                                                                RandomMemWrite(WAIT_TIME_B4_BYPASS_ALARM_IN_SECa,u.c,WAIT_TIME_B4_BYPASS_ALARM_IN_SECn);
                                                            strcpy(LcdLine2,"     SAVED      ");
                                                                                                                SendSerialHmiPacket();
                                                                                                        }else
                                                                                                        {
                                                                                                                DisplayPage = PARA_SET_35_1;
                                                                                                        }
                                                                                                }
                                                                                                break;
                                                #if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_NO))
                        case PARA_SET_36_0:		if(KeyPressCounter < 3)
                                                                                                                KeyPressCounter++;
                                                                                                else
                                                                                                {	uiTemp = UserInputToUnsignedInt();
                                                                                                        if(uiTemp >= CIP_LOWER_LIMIT_CYCLE_MIN_VAL && uiTemp <= CIP_LOWER_LIMIT_CYCLE_MAX_VAL){
                                                                CIP_LowerLimitCycle = uiTemp;
                                                                                                                u.i[0] = uiTemp;
                                                                                                                RandomMemWrite(CIP_LOWER_LIMIT_CYCLEa,u.c,CIP_LOWER_LIMIT_CYCLEn);
                                                            strcpy(LcdLine2,"     SAVED      ");
                                                                                                                SendSerialHmiPacket();
                                                                                                        }else
                                                                                                        {
                                                                                                                DisplayPage = PARA_SET_36_1;
                                                                                                        }
                                                                                                }
                                                                                                break;
                        case PARA_SET_37_0:		if(KeyPressCounter < 3)
                                                                                                                KeyPressCounter++;
                                                                                                else
                                                                                                {	uiTemp = UserInputToUnsignedInt();
                                                        #if(CIP_FAVORABLE_TIME_MIN_VAL != 0)
                                                        if(uiTemp >= CIP_FAVORABLE_TIME_MIN_VAL && uiTemp <= CIP_FAVORABLE_TIME_MAX_VAL)
                                                        #else
                                                        if(uiTemp <= CIP_FAVORABLE_TIME_MAX_VAL)
                                                        #endif
                                                        {	CIP_FavorableTime = uiTemp;
                                                                                                                u.i[0] = uiTemp;
                                                                                                                RandomMemWrite(CIP_FAVORABLE_TIMEa,u.c,CIP_FAVORABLE_TIMEn);
                                                            strcpy(LcdLine2,"     SAVED      ");
                                                                                                                SendSerialHmiPacket();
                                                                                                        }else
                                                                                                        {
                                                                                                                DisplayPage = PARA_SET_37_1;
                                                                                                        }
                                                                                                }
                                                                                                break;
                        case PARA_SET_38_0:		if(KeyPressCounter < 3)
                                                                                                                KeyPressCounter++;
                                                                                                else
                                                                                                {	uiTemp = UserInputToUnsignedInt();
                                                                                                        if(uiTemp >= CIP_FAVORABLE_TIME_GRACE_MINUTES_MIN_VAL && uiTemp <= CIP_FAVORABLE_TIME_GRACE_MINUTES_MAX_VAL){
                                                                CIP_FavorableTimeGraceMin = uiTemp;
                                                                                                                u.i[0] = uiTemp;
                                                                                                                RandomMemWrite(CIP_FAVORABLE_TIME_GRACE_MINa,u.c,CIP_FAVORABLE_TIME_GRACE_MINn);
                                                            strcpy(LcdLine2,"     SAVED      ");
                                                                                                                SendSerialHmiPacket();
                                                                                                        }else
                                                                                                        {
                                                                                                                DisplayPage = PARA_SET_38_1;
                                                                                                        }
                                                                                                }
                                                                                                break;
                        #endif
                        case PARA_SET_39_0:     DynamicCapDIMode = DYNAMIC_CAPDI_MODE_ON;
                                                u.c[0] = DynamicCapDIMode;
                                                RandomMemWrite(DYNAMIC_CAPDI_MODEa,u.c,DYNAMIC_CAPDI_MODEn);
                                                strcpy(LcdLine2,"     SAVED.     ");
                                                SendSerialHmiPacket();
                                                DelayInSec(1);
                                                break;
                        case PARA_SET_39_1:     DynamicCapDIMode = DYNAMIC_CAPDI_MODE_OFF;
                                                u.c[0] = DynamicCapDIMode;
                                                RandomMemWrite(DYNAMIC_CAPDI_MODEa,u.c,DYNAMIC_CAPDI_MODEn);
                                                strcpy(LcdLine2,"     SAVED.     ");
                                                SendSerialHmiPacket();
                                                DelayInSec(1);
                                                break;  
                       case PARA_SET_40_0:     if(KeyPressCounter < 3){
                                                 KeyPressCounter++;
                                               } else
                                               {	uiTemp = UserInputToUnsignedInt();
                                                        if(uiTemp >= PRE_PURE_CURRENT_MIN_VAL && uiTemp <= PRE_PURE_CURRENT_MAX_VAL){
                                                                PrePureCurrentSetpoint = uiTemp;
                                                                u.i[0] = uiTemp;
                                                                RandomMemWrite(PRE_PURE_CURRENTa,u.c,PRE_PURE_CURRENTn);
                                                                strcpy(LcdLine2,"     SAVED      ");
                                                                SendSerialHmiPacket();
                                                        }
                                                        else
                                                        {
                                                                DisplayPage = PARA_SET_40_1;
                                                        }
						}
						break;
                        case PARA_SET_41_0:     if(KeyPressCounter < 3)
						KeyPressCounter++;
                                                else
						{	uiTemp = UserInputToUnsignedInt();
							if(uiTemp >= PURE_CURRENT_MIN_VAL && uiTemp <= PURE_CURRENT_MAX_VAL){
								PureCurrentSetpoint = uiTemp;
								u.i[0] = uiTemp;
								RandomMemWrite(PURE_CURRENTa,u.c,PURE_CURRENTn);
								strcpy(LcdLine2,"     SAVED      ");
								SendSerialHmiPacket();
							}
							else
							{
								DisplayPage = PARA_SET_41_1;
							}
						}
						break;                      
                        case PARA_SET_42_0:     if(KeyPressCounter < 3)
						KeyPressCounter++;
                                                else
						{	uiTemp = UserInputToUnsignedInt();
							if(uiTemp >= WASTE_CURRENT_MIN_VAL && uiTemp <= WASTE_CURRENT_MAX_VAL){
								WasteCurrentSetpoint = uiTemp;
								u.i[0] = uiTemp;
								RandomMemWrite(WASTE_CURRENTa,u.c,WASTE_CURRENTn);
								strcpy(LcdLine2,"     SAVED      ");
								SendSerialHmiPacket();
							}
							else
							{
								DisplayPage = PARA_SET_42_1;
							}
						}
						break;
                        case PARA_SET_43_0:     if(KeyPressCounter < 3)
						KeyPressCounter++;
                                                else
						{	uiTemp = UserInputToUnsignedInt();
							if(uiTemp >= SHUNT_PRE_CHARGE_TIME_MIN_VAL && uiTemp <= SHUNT_PRE_CHARGE_TIME_MAX_VAL){
								ShuntPostWasteStepTimeInSec = uiTemp;
								u.i[0] = uiTemp;
								RandomMemWrite(SHUNT_PRE_CHARGE_TIME_IN_SECa,u.c,SHUNT_PRE_CHARGE_TIME_IN_SECn);
								strcpy(LcdLine2,"     SAVED      ");
								SendSerialHmiPacket();
							}
							else
							{
								DisplayPage = PARA_SET_43_1;
							}
						}
						break;
                        case PARA_SET_44_0:     if(KeyPressCounter < 3)
						KeyPressCounter++;
                                                else
						{	uiTemp = UserInputToUnsignedInt();
							if(uiTemp >= SHUNT_POST_CHARGE_TIME_MIN_VAL && uiTemp <= SHUNT_POST_CHARGE_TIME_MAX_VAL){
								ShuntPostPureStepTimeInSec = uiTemp;
								u.i[0] = uiTemp;
								RandomMemWrite(SHUNT_POST_CHARGE_TIME_IN_SECa,u.c,SHUNT_POST_CHARGE_TIME_IN_SECn);
								strcpy(LcdLine2,"     SAVED      ");
								SendSerialHmiPacket();
							}
							else
							{
								DisplayPage = PARA_SET_44_1;
							}
						}
						break;     
                                                
                        case USER_FUNCTIONS_0:	DisplayPage=USER_FUNCTIONS_0_0;break;
                        case USER_FUNCTIONS_1:	SwitchFromLogoutToManualCIP(); break;
                        case USER_FUNCTIONS_2:  _ttemp = TimeStamp2Sec(Device.TimeStamp);
                                                                            _ttemp += SecAfterTimeSync;
                                                                            TimeStampPtr = localtime(&_ttemp);
                                                                            TimeStampPtr->tm_year -= 100;
                                                                            TimeStampPtr->tm_mon += 1;
                                                                            SerialHmiPassword[0] = TimeStampPtr->tm_hour/10 +48;
                                                                            SerialHmiPassword[1] = TimeStampPtr->tm_hour%10 +48;
                                                                            SerialHmiPassword[2] = TimeStampPtr->tm_min/10 +48;
                                                                            SerialHmiPassword[3] = TimeStampPtr->tm_min%10 +48;
                                                                            KeyPressCounter=0;DisplayPage=USER_FUNCTIONS_2_0;break;
                        case USER_FUNCTIONS_3:  //strcpy(SerialHmiPassword,"20170101");
                                                                            _ttemp = TimeStamp2Sec(Device.TimeStamp);
                                                                            _ttemp += SecAfterTimeSync;
                                                                            TimeStampPtr = localtime(&_ttemp);
                                                                            TimeStampPtr->tm_year -= 100;
                                                                            TimeStampPtr->tm_mon += 1;
                                                                            SerialHmiPassword[0] = TimeStampPtr->tm_year/10 +48;
                                                                            SerialHmiPassword[1] = TimeStampPtr->tm_year%10 +48;
                                                                            SerialHmiPassword[2] = TimeStampPtr->tm_mon/10 +48;
                                                                            SerialHmiPassword[3] = TimeStampPtr->tm_mon%10 +48;
                                                                            SerialHmiPassword[4] = TimeStampPtr->tm_mday/10 +48;
                                                                            SerialHmiPassword[5] = TimeStampPtr->tm_mday%10 +48;
                                                                            KeyPressCounter=0;
                                                    DisplayPage=USER_FUNCTIONS_3_0;
                                                    break;
                        case USER_FUNCTIONS_4:      DisplayPage = USER_FUNCTIONS_4_0; break;
                        case USER_FUNCTIONS_4_0:    DisplayPage = USER_FUNCTIONS_4_0_0; break;
                        
                        case USER_FUNCTIONS_4_1:    strcpy(LcdLine1,"                ");
                                                    strcpy(LcdLine2,"                ");
                                                    strcpy(CopyBuff,"");
                                                    SerialHmiTemp = 0x7F; //ascii value of left arrow to be used as backs
                                                                                    //SerialHmiTemp = 32; //ascii value of space
                                                    CursorActiveFlag = TRUE;
                                                    CurPos = 100; 
                                                                    KeyPressCounter=0;
                                                    DisplayPage = USER_FUNCTIONS_4_1_0;
                                                    break;
                        
                        case USER_FUNCTIONS_5:      DisplayPage = USER_FUNCTIONS_5_0; break;
                        case USER_FUNCTIONS_5_0:    DisplayPage = USER_FUNCTIONS_5_0_0; break;
                        case USER_FUNCTIONS_5_1:    strcpy(LcdLine1,"                ");
                                                    strcpy(LcdLine2,"                ");
                                                    strcpy(CopyBuff,"");
                                                    SerialHmiTemp = 0x7F; //ascii value of left arrow to be used as backspace
                                                                                    //SerialHmiTemp = 32; //ascii value of space
                                                    CursorActiveFlag = TRUE;
                                                    CurPos = 100; 
                                                                    KeyPressCounter=0;
                                                    DisplayPage = USER_FUNCTIONS_5_1_0;
                                                    break;
                        case USER_FUNCTIONS_6:      MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                                                    DisplayPage = USER_FUNCTIONS_6_0;
                                                    wifi_test_fail_cntr = 0;
                                                    break;
                        
                        case USER_FUNCTIONS_7:      DisplayPage = USER_FUNCTIONS_7_0;
                                                    break;
                        
                        case USER_FUNCTIONS_4_1_0:  if(SerialHmiTemp == 0x7F)//left arrow key
                                                    {   if(KeyPressCounter == 1)
                                                        {   strcpy(LcdLine1,"                ");
                                                            strcpy(LcdLine2,"                ");
                                                            strcpy(CopyBuff,"");
                                                            SerialHmiTemp = 0x7F; //ascii value of left arrow to be used as backspace
                                                            //SerialHmiTemp = 32; //ascii value of space
                                                            CursorActiveFlag = TRUE;
                                                            CurPos = 100; 
                                                            KeyPressCounter=0;
                                                        }
                                                        else if(KeyPressCounter > 0)
                                                        {   SerialHmiTemp = 32;//instead of backspace arrow, display space
                                                            DisplayMenu();//this will erase the arrow from screen first
                                                            KeyPressCounter--;
                                                            SerialHmiPassword[KeyPressCounter] = '\0';
                                                            SerialHmiTemp = 0x7F;//display backspace
                                                            //after this DisplayMenu() is call automatically from the loop which will convert the char to be deleted to space.
                                                        }
                                                    }
                                                    else
                                                    {   SerialHmiPassword[KeyPressCounter] = SerialHmiTemp;
                                                        SerialHmiTemp = 0x7F;//by default new char option = backspace 
                                                        if(KeyPressCounter < WIFI_SSID_NAMEn)
                                                        {   KeyPressCounter++;
                                                            SerialHmiPassword[KeyPressCounter] = '\0';
                                                        }
                                                    }
                                                    break;
                        case USER_FUNCTIONS_5_1_0:  if(SerialHmiTemp == 0x7F)//left arrow key
                                                    {   if(KeyPressCounter == 1)
                                                        {   strcpy(LcdLine1,"                ");
                                                            strcpy(LcdLine2,"                ");
                                                            strcpy(CopyBuff,"");
                                                            SerialHmiTemp = 0x7F; //ascii value of left arrow to be used as backspace
                                                            //SerialHmiTemp = 32; //ascii value of space
                                                            CursorActiveFlag = TRUE;
                                                            CurPos = 100; 
                                                            KeyPressCounter=0;
                                                        }
                                                        else if(KeyPressCounter > 0)
                                                        {   SerialHmiTemp = 32;//instead of backspace arrow, display space
                                                            DisplayMenu();//this will erase the arrow from screen first
                                                            KeyPressCounter--;
                                                            SerialHmiPassword[KeyPressCounter] = '\0';
                                                            SerialHmiTemp = 0x7F;//display backspace
                                                            //after this DisplayMenu() is call automatically from the loop which will convert the char to be deleted to space.
                                                        }
                                                    }
                                                    else
                                                    {   SerialHmiPassword[KeyPressCounter] = SerialHmiTemp;
                                                        SerialHmiTemp = 0x7F;//by default new char option = backspace 
                                                        if(KeyPressCounter < WIFI_SSID_PASSWORDn)
                                                        {   KeyPressCounter++;
                                                            SerialHmiPassword[KeyPressCounter] = '\0';
                                                        }
                                                    }
                                                    break;
                                                         
                        case USER_FUNCTIONS_2_0:   
                                                if(KeyPressCounter < 3) // CURRENT TIME HH-MM
                                                                                        KeyPressCounter++;
                                                                        else{
                                                                                                uiTemp = SerialHmiPassword[0] - 48;
                                                                                                uiTemp *= 10;
                                                                                uiTemp += SerialHmiPassword[1] - 48;
                                                                                if(uiTemp<24) // HH checked
                                                                                {
                                                                                                        iTemp = uiTemp;
                                                                                                        uiTemp = SerialHmiPassword[2] - 48;
                                                                                                 uiTemp *= 10;
                                                                                        uiTemp += SerialHmiPassword[3] - 48;
                                                                                        if(uiTemp <60) // HH & MM checked
                                                                                        {
                                                                                                                Device.TimeStamp.tm_hour = iTemp;
                                                                                                                Device.TimeStamp.tm_min = uiTemp;
                                                                                                Device.TimeStamp.tm_sec = 0;
                                                                                                SecAfterTimeSync = 0;
                                                             TimeValidFlag = TRUE;
                                                                                        }
                                                                                        else{	//ILLEGAL MINUTE
                                                                                                                DisplayPage = USER_FUNCTIONS_2_0_ERROR;
                                                                                        }
                                                                                }
                                                                                else{	//ILLEGAL HOUR
                                                                                        DisplayPage = USER_FUNCTIONS_2_0_ERROR;
                                                                                }
                                                                        }
                                                                        break;
                        case USER_FUNCTIONS_3_0:
                                                 if(KeyPressCounter < 6) // CURRENT DATE YYYY-MM-DD
                                                                                KeyPressCounter++;
                                                                        else{
                                                                                                uiTemp = SerialHmiPassword[0]-48;
                                                                                uiTemp *=10;
                                                                                uiTemp += SerialHmiPassword[1]-48;
                                                                                if(uiTemp >= 17)
                                                                                {
                                                                                                        if(ValidateDate((SerialHmiPassword[4]-48)*10+(SerialHmiPassword[5]-48),(SerialHmiPassword[2]-48)*10+(SerialHmiPassword[3]-48),uiTemp)== TRUE)
                                                                                        {
                                                                                                                Device.TimeStamp.tm_year = uiTemp;
                                                                                                Device.TimeStamp.tm_mon = (SerialHmiPassword[2]-48)*10+(SerialHmiPassword[3]-48);
                                                                                                Device.TimeStamp.tm_mday = (SerialHmiPassword[4]-48)*10+(SerialHmiPassword[5]-48);
                                                             DateValidFlag = TRUE;
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                                        DisplayPage= USER_FUNCTIONS_3_0_ERROR;
                                                                                        }
                                                                                }
                                                                                else
                                                                                {
                                                                                                        DisplayPage = USER_FUNCTIONS_3_0_ERROR;
                                                                                }		  				
                                                                        }
                                                                        break;
                                        
					case HW_TEST_1:			DisplayPage=INPUT_TEST_1;break;
					case HW_TEST_2:			DisplayPage=OUTPUT_TEST_1;break;
                                        case HW_TEST_3:                 DisplayPage=SENSOR_READ_1;break;   
					
					//case INPUT_TEST_1:		break;
					
					case OUTPUT_TEST_1:		if((DigitalOutput & 0x0001) == 0)
													DigitalOutput |= 0x0001;
											else
													DigitalOutput &= ~0x0001;
											break;
					case OUTPUT_TEST_2:		if((DigitalOutput & 0x0002) == 0)
													DigitalOutput |= 0x0002;
											else
													DigitalOutput &= ~0x0002;
											break;
					case OUTPUT_TEST_3:		if((DigitalOutput & 0x0004) == 0)
													DigitalOutput |= 0x0004;
											else
													DigitalOutput &= ~0x0004;
											break;
					case OUTPUT_TEST_4:		if((DigitalOutput & 0x0008) == 0)
													DigitalOutput |= 0x0008;
											else
													DigitalOutput &= ~0x0008;
											break;
					case OUTPUT_TEST_5:		if((DigitalOutput & 0x0010) == 0)
													DigitalOutput |= 0x0010;
											else
													DigitalOutput &= ~0x0010;
											break;
					case OUTPUT_TEST_6:		if((DigitalOutput & 0x0020) == 0)
													DigitalOutput |= 0x0020;
											else
													DigitalOutput &= ~0x0020;
											break;
					case OUTPUT_TEST_7:		if((DigitalOutput & 0x0040) == 0)
													DigitalOutput |= 0x0040;
											else
													DigitalOutput &= ~0x0040;
											break;
					case OUTPUT_TEST_8:		if((DigitalOutput & 0x0080) == 0)
													DigitalOutput |= 0x0080;
											else
													DigitalOutput &= ~0x0080;
											break;
                                        case OUTPUT_TEST_9:		if((DigitalOutput & 0x0100) == 0)
													DigitalOutput |= 0x0100;
											else
													DigitalOutput &= ~0x0100;
											break;
                                        case OUTPUT_TEST_10:      if (PumpManualMode == 0)
                                                                      PumpManualMode = PUMP_MANUAL_MODE_ON;
                                                                  else
                                                                      PumpManualMode = PUMP_MANUAL_MODE_OFF;

                                                                  break;
                                        case OUTPUT_TEST_11_0:    if(KeyPressCounter < 3)
                                                                  KeyPressCounter++;
                                                                  else
                                                                  {	  uiTemp = UserInputToUnsignedInt();
                                                                          if(uiTemp >= PUMP_MANUAL_SPEED_MIN_VAL && uiTemp <= PUMP_MANUAL_SPEED_MAX_VAL){
                                                                                  PumpManualSpeed = uiTemp;
                                                                                  u.i[0] = uiTemp;
                                                                                  RandomMemWrite(PUMP_MANUAL_SPEEDa,u.c,PUMP_MANUAL_SPEEDn);
                                                                                  strcpy(LcdLine2,"     SAVED      ");
                                                                                  SendSerialHmiPacket();
                                                                                  KeyPressCounter = 0;
                                                                          }
                                                                          else
                                                                          {
                                                                                  DisplayPage = PARA_SET_41_1;
                                                                          }
                                                                  }
                                                                  break; 					
                                                                                
                                        case AUTO_0:
                                        case AUTO_0_1:          if(CurrentProcessStep == IDLE)
								{	CurrentProcessStep = WAIT;
                                                                        ProcessStepRunSec = 0;
                                                                        RegisterAlarm(LEAKAGE_ALARM_FLAG, ALARM_RESOLVED);//no problem even if no leakage alarm
                                                                        RegisterAlarm(LOW_FLOW_PURIFY_ALARM_FLAG, ALARM_RESOLVED);//no problem even if no leakage alarm
                                                                        RegisterAlarm(LOW_FLOW_WASTE_ALARM_FLAG, ALARM_RESOLVED);//no problem even if no leakage alarm
                                                                        RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_RESOLVED);//no problem even if no existing EC alarm
                                                                        //reset all PAE alarms if any
                                                                for(ucTemp=0;ucTemp<8;ucTemp++)
                                                                  {	if((PaeAlarm & (1<<ucTemp)) > 0)
                                                                          RegisterAlarm(8+ucTemp, ALARM_RESOLVED);	
                                                                  }
                                                                }
											break;
					case AUTO_1:			SwitchingFromAutoToLogout();
                                            break;
                                                                        
					case AUTO_0_9:			DisplayPage=AUTO_0_9_0;break;
					case AUTO_0_9_0:		RegisterAlarm(LOW_FLOW_PURIFY_ALARM_FLAG, ALARM_RESOLVED);	break;//clear alarm
					case AUTO_0_9_1:		RegisterAlarm(LOW_FLOW_WASTE_ALARM_FLAG, ALARM_RESOLVED);	break;//clear alarm
					case AUTO_0_9_2:		RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED);
                                            #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                                WaitTimeB4BypassAlarmInSecCounter = 0;
                                            #endif
                                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                                BypassOperationStopCounter = 0;
                                                BypassOperationStartCounter = 0;
                                                BypassOperation2AlarmSecCounterFlag = FALSE;
                                                Bypass2AlarmSecCounter =0;
                                            #endif
                                            break;//clear alarm
					case AUTO_0_9_3:		RegisterAlarm(CABINET_TEMP_ALARM_FLAG, ALARM_RESOLVED);		break;//clear alarm
					case AUTO_0_9_4:		RegisterAlarm(LEAKAGE_ALARM_FLAG, ALARM_RESOLVED);			break;//clear alarm
					case AUTO_0_9_5:		RegisterAlarm(WASTE_VALUE_ALARM_FLAG, ALARM_RESOLVED);		break;//clear alarm
					case AUTO_0_9_6:		RegisterAlarm(CIP_LOW_LEVEL_ALARM_FLAG, ALARM_RESOLVED);CIP_PumpRunTimeInSec = 0;PeriodicSave();break;//clear alarm
                                        
                                        case AUTO_0_9_8:		RegisterAlarm(PAE_OVP_SHUTDOWN_FLAG, ALARM_RESOLVED);		break;
                                        case AUTO_0_9_9:		RegisterAlarm(PAE_OLP_SHUTDOWN_FLAG, ALARM_RESOLVED);		break;
                                        case AUTO_0_9_10:		RegisterAlarm(PAE_OTP_SHUTDOWN_FLAG, ALARM_RESOLVED);		break;
                                        case AUTO_0_9_11:		RegisterAlarm(PAE_FAN_FAIL_FLAG, ALARM_RESOLVED);			break;
                                        case AUTO_0_9_12:		RegisterAlarm(PAE_AUX_OR_SMPS_FAIL_FLAG, ALARM_RESOLVED);	break;
                                        case AUTO_0_9_13:		RegisterAlarm(PAE_HI_TEMP_FLAG, ALARM_RESOLVED);			break;
                                        case AUTO_0_9_14:		RegisterAlarm(PAE_AC_POWER_DOWN_FLAG, ALARM_RESOLVED);		break;
                                        case AUTO_0_9_15:		RegisterAlarm(PAE_AC_INPUT_FAIL_FLAG, ALARM_RESOLVED);		break;
                            
                                        case SENSOR_READ_1:             DisplayPage=SENSOR_READ_1_1;break;
                                        case SENSOR_READ_2:             u.c[0] = 1; //Activate Conductivity Probe sensor
                                                                        //WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
                                                                        DisplayPage=SENSOR_READ_2_1;break;
                                        case SENSOR_READ_3:             u.c[0] = 1; //Activate Temperature Probe sensor
                                                                        //WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
                                                                        DisplayPage=SENSOR_READ_3_1;break;
                                        case SENSOR_READ_4:             DisplayPage=SENSOR_READ_4_1;break;
                                        case SENSOR_READ_5:             DisplayPage=SENSOR_READ_5_1;break;
                                                          
                                        }
                                        break;
			case KEY_EXIT:
				switch(DisplayPage)
				{	//case LOGOUT_0:			break;
					//case LOGOUT_1:			break;
					//case LOGOUT_2:			break;
					
					//case PROCESS:           break;
                                        
					case LOGOUT_1:			
					case LOGOUT_2:			
					case LOGOUT_3:
					case PASSWORD_0:
					case PASSWORD_ERR:		DisplayPage=LOGOUT_0;break;
					
					case DEV_INFO_0:		
					case DEV_INFO_1:		
					case DEV_INFO_2:		
					case DEV_INFO_3:		
					//case DEV_INFO_4:
                                                                        DisplayPage=LOGOUT_1;break;
					
					case DEV_INFO_0_1:		DisplayPage=DEV_INFO_0;break;
					case DEV_INFO_1_1:		DisplayPage=DEV_INFO_1;break;
					case DEV_INFO_2_1:		DisplayPage=DEV_INFO_2;break;
					case DEV_INFO_3_1:		DisplayPage=DEV_INFO_3;break;
					//case DEV_INFO_4_1:		DisplayPage=DEV_INFO_4;break;
					
					case MENU_0:			
					case MENU_1:			
					//case MENU_2:			
					case MENU_3:			DisplayPage=LOGOUT_0;break;
					
					case PARA_SET_2:
					case PARA_SET_3:
					case PARA_SET_4:
					case PARA_SET_5:
					case PARA_SET_6:
					case PARA_SET_7:
					case PARA_SET_8:
					case PARA_SET_9:
					case PARA_SET_10:
					case PARA_SET_11:
					case PARA_SET_12:
					case PARA_SET_13:
					case PARA_SET_14:
					case PARA_SET_15:
					case PARA_SET_1:
					case PARA_SET_16:
					case PARA_SET_17:
					case PARA_SET_18:
					case PARA_SET_19:
					case PARA_SET_20:
					case PARA_SET_21:
					case PARA_SET_22:
					case PARA_SET_23:
					case PARA_SET_24:
                                        case PARA_SET_25:
					case PARA_SET_26:
					case PARA_SET_27:
                                        case PARA_SET_28:
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                        case PARA_SET_29:
                                        #endif
                                        case PARA_SET_30:
                                        case PARA_SET_31:
                                        case PARA_SET_32:
                                        case PARA_SET_33:
                                        case PARA_SET_34:
                                        case PARA_SET_35:
                                        #if(CVS_CIP==CVS_CIP_NO)
                                        case PARA_SET_36:
                                        case PARA_SET_37:
                                        case PARA_SET_38:
                                        #endif                                       
                                        case PARA_SET_39:
                                        case PARA_SET_40:
                                        case PARA_SET_41:
                                        case PARA_SET_42:
                                        case PARA_SET_43:
                                        case PARA_SET_44:
                                                                        DisplayPage=MENU_1;
                                                                        break;
											
					case PARA_SET_2_1:
					case PARA_SET_2_0:		DisplayPage = PARA_SET_2;break;
					case PARA_SET_3_1:
					case PARA_SET_3_0:		DisplayPage = PARA_SET_3;break;
					case PARA_SET_4_1:
					case PARA_SET_4_0:		DisplayPage = PARA_SET_4;break;
					case PARA_SET_5_1:
					case PARA_SET_5_0:		DisplayPage = PARA_SET_5;break;
					case PARA_SET_6_1:
					case PARA_SET_6_0:		DisplayPage = PARA_SET_6;break;
					case PARA_SET_7_1:
					case PARA_SET_7_0:		DisplayPage = PARA_SET_7;break;
					case PARA_SET_8_1:
					case PARA_SET_8_0:		DisplayPage = PARA_SET_8;break;
					case PARA_SET_9_1:
					case PARA_SET_9_0:		DisplayPage = PARA_SET_9;break;
					case PARA_SET_10_1:
					case PARA_SET_10_0:		DisplayPage = PARA_SET_10;break;
					case PARA_SET_11_1:
					case PARA_SET_11_0:		DisplayPage = PARA_SET_11;break;
					case PARA_SET_12_1:
					case PARA_SET_12_0:		DisplayPage = PARA_SET_12;break;
					case PARA_SET_13_1:
					case PARA_SET_13_0:		DisplayPage = PARA_SET_13;break;
					case PARA_SET_14_1:
					case PARA_SET_14_0:		DisplayPage = PARA_SET_14;break;
					case PARA_SET_15_1:
					case PARA_SET_15_0:		DisplayPage = PARA_SET_15;break;
					case PARA_SET_1_1:
					case PARA_SET_1_0:		DisplayPage = PARA_SET_1;break;
					case PARA_SET_16_1:
					case PARA_SET_16_0:		DisplayPage = PARA_SET_16;break;
					case PARA_SET_17_1:
					case PARA_SET_17_0:		DisplayPage = PARA_SET_17;break;
					case PARA_SET_18_1:
					case PARA_SET_18_0:		DisplayPage = PARA_SET_18;break;
					case PARA_SET_19_1:
					case PARA_SET_19_0:		DisplayPage = PARA_SET_19;break;
					case PARA_SET_20_1:
					case PARA_SET_20_0:		DisplayPage = PARA_SET_20;break;
					case PARA_SET_21_1:
					case PARA_SET_21_0:		DisplayPage = PARA_SET_21;break;
					case PARA_SET_22_1:
					case PARA_SET_22_0:		DisplayPage = PARA_SET_22;break;
                                        case PARA_SET_23_1:
					case PARA_SET_23_0:		DisplayPage = PARA_SET_23;break;
					case PARA_SET_24_1:
					case PARA_SET_24_0:		DisplayPage = PARA_SET_24;break;
                                        case PARA_SET_25_1:
					case PARA_SET_25_0:		DisplayPage = PARA_SET_25;break;
					case PARA_SET_26_1:
					case PARA_SET_26_0:		DisplayPage = PARA_SET_26;break;
					case PARA_SET_27_0:		DisplayPage = PARA_SET_27;break;
                    
                                        case PARA_SET_28_0:     VolumeUnit = VOLUME_UNIT_LITTER;
                                                                u.c[0] = VolumeUnit;
                                                                RandomMemWrite(VOLUME_UNITa,u.c,VOLUME_UNITn);
                                                                DisplayPage = PARA_SET_28;
                                                                break;
                                        case PARA_SET_28_1:     VolumeUnit = VOLUME_UNIT_GALLON;
                                                                u.c[0] = VolumeUnit;
                                                                RandomMemWrite(VOLUME_UNITa,u.c,VOLUME_UNITn);
                                                                DisplayPage = PARA_SET_28;
                                                                break;
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                          case PARA_SET_29_0:     
                                          case PARA_SET_29_1:     
                                          case PARA_SET_29_2:     DisplayPage = PARA_SET_29;  break;
                                        #endif
                                        case PARA_SET_30_1:
					case PARA_SET_30_0:     DisplayPage = PARA_SET_30;  break;
                                        case PARA_SET_31_1:
					case PARA_SET_31_0:     DisplayPage = PARA_SET_31;  break;
                                        case PARA_SET_32_1:
					case PARA_SET_32_0:     DisplayPage = PARA_SET_32;  break;
                                        case PARA_SET_33_1:
					case PARA_SET_33_0:     DisplayPage = PARA_SET_33;  break;
                                        case PARA_SET_34_1:
					case PARA_SET_34_0:     DisplayPage = PARA_SET_34;  break;
                                        case PARA_SET_35_1:
					case PARA_SET_35_0:     DisplayPage = PARA_SET_35;  break;
                                        #if(CVS_CIP==CVS_CIP_NO)
                                        case PARA_SET_36_1:
					case PARA_SET_36_0:     DisplayPage = PARA_SET_36;  break;
                                        case PARA_SET_37_1:
					case PARA_SET_37_0:     DisplayPage = PARA_SET_37;  break;
                                        case PARA_SET_38_1:
					case PARA_SET_38_0:     DisplayPage = PARA_SET_38;  break;
                                        #endif
                                        case PARA_SET_39_1:
                                        case PARA_SET_39_0:     DisplayPage = PARA_SET_39;  break;
                                        case PARA_SET_40_1:
                                        case PARA_SET_40_0:     DisplayPage = PARA_SET_40;  break;
                                        case PARA_SET_41_1:        
                                        case PARA_SET_41_0:     DisplayPage = PARA_SET_41;  break;
                                        case PARA_SET_42_1:        
                                        case PARA_SET_42_0:     DisplayPage = PARA_SET_42;  break;
                                        case PARA_SET_43_1:        
                                        case PARA_SET_43_0:     DisplayPage = PARA_SET_43;  break;
                                        case PARA_SET_44_1:        
                                        case PARA_SET_44_0:     DisplayPage = PARA_SET_44;  break;
                    //case SYS_SETTING_0:     DisplayPage = MENU_2;break;          
                                        
                    //case SYS_SETTING_0_0:   DisplayPage=SYS_SETTING_0;break;
					case USER_FUNCTIONS_2_0:   DisplayPage=USER_FUNCTIONS_2;break;
					case USER_FUNCTIONS_3_0:   DisplayPage=USER_FUNCTIONS_3;break;
                                        case USER_FUNCTIONS_4_0:
                                        case USER_FUNCTIONS_4_1:    DisplayPage=USER_FUNCTIONS_4;break;
                                        case USER_FUNCTIONS_5_0:
                                        case USER_FUNCTIONS_5_1:    DisplayPage=USER_FUNCTIONS_5;break;
                    
                                        case USER_FUNCTIONS_4_0_0:  DisplayPage=USER_FUNCTIONS_4_0;break;
                                        case USER_FUNCTIONS_4_1_0:  RemoveCharFromEndOfString(' ',SerialHmiPassword);
                                                RandomMemWrite(WIFI_SSID_NAMEa,SerialHmiPassword,WIFI_SSID_NAMEn);
                                                DisplayPage=USER_FUNCTIONS_4;
                                                strcpy(LcdLine1,"  WiFi NETWORK  ");
                                                strcpy(LcdLine2,"SAVE SUCCESSFUL.");
                                                CursorActiveFlag = FALSE;
                                                SendSerialHmiPacket();
                                                DelayInSec(3);
                                                break;
                                        case USER_FUNCTIONS_5_0_0:  DisplayPage=USER_FUNCTIONS_5_0;break;
                                        case USER_FUNCTIONS_5_1_0:  RemoveCharFromEndOfString(' ',SerialHmiPassword);
                                                RandomMemWrite(WIFI_SSID_PASSWORDa,SerialHmiPassword,WIFI_SSID_PASSWORDn);
                                                DisplayPage=USER_FUNCTIONS_5;
                                                strcpy(LcdLine1," WiFi PASSWORD  ");
                                                strcpy(LcdLine2,"SAVE SUCCESSFUL.");
                                                CursorActiveFlag = FALSE;
                                                SendSerialHmiPacket();
                                                DelayInSec(3);
                                                break;
					
                                        case USER_FUNCTIONS_6_0:
                                        case USER_FUNCTIONS_6_1:
                                        case USER_FUNCTIONS_6_2:    DisplayPage=USER_FUNCTIONS_6;break;
                    
                                        case USER_FUNCTIONS_7_0:    DigitalOutput = 0xFFFF; DisplayPage = USER_FUNCTIONS_7;break;
                    
                                        /*
					case SYS_SETTING_0_0_ERROR:	SerialHmiPassword[0] = ReportingTime.tm_hour/10+48;
												SerialHmiPassword[1] = ReportingTime.tm_hour%10+48;
												SerialHmiPassword[2] = ReportingTime.tm_min/10+48;
												SerialHmiPassword[3] = ReportingTime.tm_min%10+48;
												KeyPressCounter = 0;DisplayPage=SYS_SETTING_0_0;
												break;*/
					case USER_FUNCTIONS_2_0_ERROR:	KeyPressCounter = 0;DisplayPage=USER_FUNCTIONS_2_0;break;
					case USER_FUNCTIONS_3_0_ERROR:	KeyPressCounter = 0;DisplayPage=USER_FUNCTIONS_3_0;break;
					
					
					case HW_TEST_1:			
					case HW_TEST_2:			
                                        case HW_TEST_3:                 DisplayPage=MENU_3;break;
                                                            
					case INPUT_TEST_1:		DisplayPage=HW_TEST_1;break;
					case OUTPUT_TEST_1:
					case OUTPUT_TEST_2:
					case OUTPUT_TEST_3:
					case OUTPUT_TEST_4:
					case OUTPUT_TEST_5:
					case OUTPUT_TEST_6: 
					case OUTPUT_TEST_7: 
					case OUTPUT_TEST_8:	
                                        case OUTPUT_TEST_9:             //DigitalOutput = 0xFFFF;//Switch off all output
                                        case OUTPUT_TEST_10:
                                        case OUTPUT_TEST_11:
                                                                        DisplayPage=HW_TEST_2;
									break;
                                        case OUTPUT_TEST_11_0:          DisplayPage = OUTPUT_TEST_11;  break;
					case USER_FUNCTIONS_0:
					case USER_FUNCTIONS_1:
					case USER_FUNCTIONS_2:
					case USER_FUNCTIONS_3:
                                        case USER_FUNCTIONS_4:
                                        case USER_FUNCTIONS_5:
                                        case USER_FUNCTIONS_6:
                                        case USER_FUNCTIONS_7:          DisplayPage = LOGOUT_3;break;
					
					case USER_FUNCTIONS_0_0:
                                        case USER_FUNCTIONS_0_1:        DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
												DisplayPage = USER_FUNCTIONS_0; 
												break;
					case USER_FUNCTIONS_1_0:	DisplayPage = USER_FUNCTIONS_1;	break;
								
					case AUTO_0:			/*KeyTimeOut = FreeRunSec;*/DisplayPage=AUTO_1;break;
					case AUTO_1:			DisplayPage=AUTO_0;break;
                                        
					case AUTO_0_1:			
					case AUTO_0_2:
					case AUTO_0_3:			
					case AUTO_0_4:
                                        case AUTO_0_5:			
					case AUTO_0_6:			
					case AUTO_0_7:			
					case AUTO_0_8:			
					case AUTO_0_9:		        DisplayPage=AUTO_0;break;
					
					case AUTO_0_9_0:
					case AUTO_0_9_1:
					case AUTO_0_9_2:
					case AUTO_0_9_3:
					case AUTO_0_9_4:
					case AUTO_0_9_5:
					case AUTO_0_9_6:
					case AUTO_0_9_7:
					case AUTO_0_9_8:
					case AUTO_0_9_9:
					case AUTO_0_9_10:
					case AUTO_0_9_11:
					case AUTO_0_9_12:
					case AUTO_0_9_13:
					case AUTO_0_9_14:
					case AUTO_0_9_15:
					case AUTO_0_9_16:
					case AUTO_0_9_17:
					case AUTO_0_9_18:
					case AUTO_0_9_19:		DisplayPage = AUTO_0_9; break;
                    
                                        case SENSOR_READ_1:
					case SENSOR_READ_2:
					case SENSOR_READ_3:
					case SENSOR_READ_4:
					case SENSOR_READ_5:             DisplayPage=HW_TEST_3;break;
					case SENSOR_READ_1_1:           DisplayPage = SENSOR_READ_1;break;
					case SENSOR_READ_2_1:           u.c[0] = 0; //Hybernate Conductivity Probe sensor
													//WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
									DisplayPage = SENSOR_READ_2;break;
					case SENSOR_READ_3_1:           u.c[0] = 0; //Hybernate Temperature Probe sensor
									//WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
									DisplayPage = SENSOR_READ_3;break;
					case SENSOR_READ_4_1:           DisplayPage = SENSOR_READ_4;break;
                                        case SENSOR_READ_5_1:           DisplayPage = SENSOR_READ_5;break;          
				}
			break;
		}
		DisplayMenu();
	}
}
//------------------------------------------------------------------------------
void SendSerialHmiPacket()
{	if(Fota.Status == FIRMWARE_UPGRADE_COMMAND_RECEIVED)
	{	strcpy(LcdLine1,"CONNECTING FOTA ");
		strcpy(LcdLine2,"SERVER..........");     
	}
	else if(Fota.Status == FIRMWARE_DOWNLOADING)
	{	strcpy(LcdLine1,"FIRMWARE UPGRADE");
		sprintf(CopyBuff,"IN PROGRESS:%d",Fota.PageNo);
		JustifyCenterForLcdLine(CopyBuff,LcdLine2);
	}
	PutCharSerialHmi('{');
	SerialHmiCrcCodeOut = 0;
	PutStringSerialHmi("00=00\r\n----------------\r\n");
	PutStringSerialHmi(LcdLine1);
	PutStringSerialHmi("\r\n");
	PutStringSerialHmi(LcdLine2);
	PutStringSerialHmi("\r\n");
	PutStringSerialHmi("----------------\r\n");
	PutCharSerialHmi(CurPos/100+48);        // at location 79
	PutCharSerialHmi((CurPos/10)%10+48);    // 80
	PutCharSerialHmi(CurPos%10+48);         // 81
	if(CursorActiveFlag == TRUE)
		PutCharSerialHmi('A');	 							//at location 82
	else
		PutCharSerialHmi('I');								//at location 82
	//PutStringSerialHmi("\r\n----------------\r\n*");
    PutStringSerialHmi("ABCDEFGH\r\n*");
	sprintf(CopyBuff,"%d}",SerialHmiCrcCodeOut);
	PutStringSerialHmi(CopyBuff);
    /*DelayInMilliSec(1);
	PutCharSerialHmi('}');	
	DelayInMilliSec(1);
	PutCharSerialHmi('}');	
	DelayInMilliSec(1);
	PutStringSerialHmi("}}");
    */
    if(SerialHmiDebugTarget == 1)//USB
	{	sprintf(CopyBuff,"\r\n%s\r\n%s\r\n%d%c",
                LcdLine1,
                LcdLine2,
                CurPos,
                (CursorActiveFlag==TRUE)? 'A':'I');
        PutStringUsb(CopyBuff);
    }
    if(SerialHmiDebugTarget == 2)//SERVER
	{	sprintf(CopyBuff,"x,DBG,901.1=%s,901.2=%s,901.3=%d,901.4=%c",
                LcdLine1,
                LcdLine2,
                CurPos,
                (CursorActiveFlag==TRUE)? 'A':'I');
        PutStringGprs(CopyBuff);
    }
}
void SERIAL_LCD_RX_IRQHandler(void)
{	static boolean_t SerialPacketStartCodeFlag = FALSE;
	static uc SerialHmiReceiveCounter = 0;
  	//LED_TOG(LED_BLUE);
  	if ((SERIAL_LCD_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	//LED_ON(LED_BLUE);
  		SERIAL_LCD_UART->SSR |= (1 << 7);      // Clear error flags 
    	SerialHmiReceiveByte = SERIAL_LCD_UART->RDR;         // Flush RDR
  	}
  	else if (SERIAL_LCD_UART->SSR & 0x04)    // RDRF?
  	{	SerialHmiReceiveByte = SERIAL_LCD_UART->RDR;
    	if(SerialHmiReceivePacketCompleteFlag == FALSE)
        {   switch(SerialHmiReceiveByte)
			{ 	//case '[': 	RfidReceiveCounter = 0; RfidBuff[0]='\0';break;
				//case ']': 	RfidBuff[RfidReceiveCounter]='\0';RfidResponseFlag = TRUE; break;
				//case 8:		if(RfidReceiveCounter>0) RfidReceiveCounter--;break;//backspace
				case '{':	if(SerialPacketStartCodeFlag == FALSE)
							{	SerialHmiReceiveCounter = 0; 
								SerialHmiBuff[0]='\0';
								SerialPacketStartCodeFlag = TRUE;
							}
							else
							{	SerialHmiBuff[SerialHmiReceiveCounter%SERIAL_HMI_BUFFER_SIZE]=SerialHmiReceiveByte;
								SerialHmiReceiveCounter++;
								SerialHmiReceiveCounter %= SERIAL_HMI_BUFFER_SIZE;
							}
							break;
				case '}':	if(SerialPacketStartCodeFlag == TRUE)
							{	SerialHmiBuff[SerialHmiReceiveCounter%SERIAL_HMI_BUFFER_SIZE]='\0';
								SerialHmiReceivePacketCompleteFlag = TRUE;
								SerialPacketStartCodeFlag = FALSE;//FOR NEXT PACKET
							}
							break;
				default:  	SerialHmiBuff[SerialHmiReceiveCounter%SERIAL_HMI_BUFFER_SIZE] = SerialHmiReceiveByte; 
							SerialHmiReceiveCounter++;
							SerialHmiReceiveCounter %= SERIAL_HMI_BUFFER_SIZE;
							break;
			}
			if(SerialHmiEchoFlag == TRUE)
				PutCharSerialHmi(SerialHmiReceiveByte);
		}
  	}
}
//------------------------------------------------------------------------------
void InitUartSerialHmi()
{	// Initialize ports for UART
  	SERIAL_LCD_UART_PFR;
	SERIAL_LCD_UART_EPFR;  
  
  	// Initialize UART asynchronous mode
  	SERIAL_LCD_UART->SCR =  0x80;   // UART reset
  	SERIAL_LCD_UART->SMR =  0x01;   // serial data output enable, operation mode 0
  	SERIAL_LCD_UART->SSR =  0x00;   // 0x80 = clear receive error flag
  	SERIAL_LCD_UART->ESCR = 0x00;          
 
  	SERIAL_LCD_UART->BGR = ((40000000UL / (SERIAL_LCD_UART_BAUDRATE -1))/2);
  
	SERIAL_LCD_UART->SCR |= 0x03;   // RX, TX enable  
  	//FM3_MFS0_UART->FBYTE = 0X0003;//INTERRUPT ON FIVE BYTE RECEIVE TESTED
  
  	// UART interrupt settings
  	NVIC_ClearPendingIRQ(SERIAL_LCD_RX_IRQn);
  	NVIC_EnableIRQ(SERIAL_LCD_RX_IRQn);
  	NVIC_SetPriority(SERIAL_LCD_RX_IRQn, 2);
  
  	SERIAL_LCD_UART->SCR |= 0x10;//set RIE serial receive interrupt enable bit
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharSerialHmi(uint8_t u8Char)
{  	while (!(SERIAL_LCD_UART->SSR & (1 << 1))); // wait for transmit buffer empty    
  	SERIAL_LCD_UART->TDR = u8Char;                // put u8Char into buffer
  	//UsbDeviceCdcCom_SendByte(u8Char);
	SerialHmiCrcCodeOut += u8Char;
  	Delay(500);
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *  RETURNS:        None
 *****************************************************************************/
void PutStringSerialHmi(char *pu8Buffer)              
{	char* buff;
	buff = (char*)pu8Buffer;
  	while (*buff != '\0')
  	{ 	PutCharSerialHmi(*buff++);	// send every char of string
    	//Delay(65000);Delay(65000); //must for long replay from gprs
  	}
//	PutStringUsb(pu8Buffer);
  
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    returns incoming character, if received
 *  PARAMETERS:     None
 *  RETURNS:        Character or 0xFF (Error) or 0 (Nothing)
 *****************************************************************************/
/*uint8_t GetCharRfid(void)   
{	uint8_t u8Char;
  	if ((RFID_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	RFID_UART->SSR |= (1 << 7);      // Clear error flags 
    	u8Char = RFID_UART->RDR;         // Flush RDR
    	return 0xFF;
  	}
  	else if (RFID_UART->SSR & 0x04)    // RDRF?
  	{	u8Char = RFID_UART->RDR;
    	if ('\r' == u8Char)
    	{	u8Char = '\n';
    	}
    	return (u8Char); 
  	}
  	else
  	{	return 0; 
  	}
}
*/
//------------------------------------------------------------------------------
ui UserInputToUnsignedInt()
{ return (((SerialHmiPassword[0] - 48)*10 + (SerialHmiPassword[1] - 48)) * 10 + (SerialHmiPassword[2] - 48)) * 10 + (SerialHmiPassword[3] - 48);
}
void VariableUnsignedInt2UserInput(ui var)
{	SerialHmiPassword[3] = var%10+48;
	SerialHmiPassword[2] = (var/10)%10+48;
	SerialHmiPassword[1] = (var/100)%10+48;
	SerialHmiPassword[0] = (var/1000)%10+48;
	KeyPressCounter=0;
}