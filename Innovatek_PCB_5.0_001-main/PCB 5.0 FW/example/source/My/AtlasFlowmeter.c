#include "AtlasFlowmeter.h"
#include "Delay.h"
#include "Variable.h"
#include "myfun.h"
#include <string.h>
#include <stdio.h>
#include "SerialHmi.h"
#include "constants.c"
#include <ppg.h>
#include <stdlib.h>

//just for testing
#include "MyUsb.h"
#include "gpio.h"



//variable local to the file
char AtlasFlowmeterBuff[21];
uc AtlasFlowmeterReceiveByte;
uc AtlasFlowmeterReceiveCounter = 0;
uc FlowmeterCommTimeOutSec = 0;
boolean_t AtlasFlowmeterReceivePacketCompleteFlag = FALSE;
boolean_t AtlasFlowmeterEchoFlag = FALSE;
boolean_t AtlasFlowmeterStartCodeFlag = FALSE;
boolean_t FlowMeterDebugFlag = FALSE;
boolean_t AtlasFlowmeterByteReceiveFlag = FALSE;
struct atlasflowmeter AtlasFlowMeter;
///////////////////////////////////////////////////////////////////////////////
//function local to the file
void InitUartAtlasFlowmeter();
void PutCharAtlasFlowmeter(uint8_t u8Char);
void PutStringAtlasFlowmeter(char *pu8Buffer);
void SendFlowMeterCommand(char *command);

struct flowmeterdatacollection
{	int TotalFlow;//to convert it in float divide it by 100
	int CurrentFlow;
};
////////////////////////////////////////////////////////////////////////////////
void ClearAllFlow()
{   //PutStringAtlasFlowmeter("CLEAR\r");
    PutStringAtlasFlowmeter("[302,0]");
    DelayInSec(2);
    //DelayInMicroSec(2000000);
    AtlasFlowMeter.TotalFlowAfterRestart = 0;
    AtlasFlowMeter.TotalFlowB4Restart = 0;
    TotalPureVolume = 0;
    TotalPureVolumeB4CycleStart = 0;
}
void AtlasReplacementCommand(char *cmd)
{   SendFlowMeterCommand(cmd);
    DelayInMilliSec(100);
	while(AtlasFlowmeterByteReceiveFlag == TRUE)
	{	AtlasFlowmeterByteReceiveFlag = FALSE;
		DelayInMilliSec(10);
	}
	if(FlowMeterDebugFlag == TRUE)
	{	PutStringUsb("\r\nFM->");
		PutStringUsb(AtlasFlowmeterBuff);
	}
}
void GetFlowMeterReading()
{	static uc flow_read_val_counter = 0;
	static struct flowmeterdatacollection FlowMeterReadSampleData[MAX_NO_OF_SAMPLE_COLLECTION];
	static uc OptimizedOutputVoltageHiCode=192;
	static uc OptimizedOutputVoltageLoCode=0;
	
	//SendFlowMeterCommand("R");
	SendFlowMeterCommand("[300,?]");
    DelayInMilliSec(1000);
	while(AtlasFlowmeterByteReceiveFlag == TRUE)
	{	AtlasFlowmeterByteReceiveFlag = FALSE;
		DelayInMilliSec(10);
	}
	if(FlowMeterDebugFlag == TRUE)
	{	PutStringUsb("\r\nFM->");
		PutStringUsb(AtlasFlowmeterBuff);
	}
    
	if(strlen(AtlasFlowmeterBuff) > 0){
		FlowmeterCommTimeOutSec = 0;
        RegisterAlarm(FLOWMETER_COMM_ALARM_FLAG, ALARM_RESOLVED);
        
        for(ucTemp=0;AtlasFlowmeterBuff[ucTemp] < '0' || AtlasFlowmeterBuff[ucTemp] > '9'; ucTemp++);
		
		for(uiTemp=0;AtlasFlowmeterBuff[ucTemp] != '.'; ucTemp++)
		{	uiTemp *= 10;
			uiTemp += AtlasFlowmeterBuff[ucTemp] - 48;
		}
		ucTemp++;
		for(;AtlasFlowmeterBuff[ucTemp] != ','; ucTemp++)
		{	uiTemp *= 10;
			uiTemp += AtlasFlowmeterBuff[ucTemp] - 48;
		}
		FlowMeterReadSampleData[flow_read_val_counter].TotalFlow = uiTemp;
			
		ucTemp++;
		for(uiTemp=0;AtlasFlowmeterBuff[ucTemp] != '.'; ucTemp++)
		{	uiTemp *= 10;
			uiTemp += AtlasFlowmeterBuff[ucTemp] - 48;
		}
		ucTemp++;
		for(;AtlasFlowmeterBuff[ucTemp] != '\r'; ucTemp++)
		{	uiTemp *= 10;
			uiTemp += AtlasFlowmeterBuff[ucTemp] - 48;
		}
		//AtlasFlowMeter.CurrentFlow = uiTemp;
		FlowMeterReadSampleData[flow_read_val_counter].CurrentFlow = uiTemp;
		
		if(++flow_read_val_counter == MAX_NO_OF_SAMPLE_COLLECTION){
			flow_read_val_counter = 0;
			//arrange the sample into assending order
			for(cTemp=0;cTemp<MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)
			{	for(ucTemp=cTemp+1;ucTemp<MAX_NO_OF_SAMPLE_COLLECTION;ucTemp++)
				{	if(FlowMeterReadSampleData[cTemp].TotalFlow > FlowMeterReadSampleData[ucTemp].TotalFlow)
					{	FlowMeterReadSampleData[cTemp].TotalFlow += FlowMeterReadSampleData[ucTemp].TotalFlow;
						FlowMeterReadSampleData[ucTemp].TotalFlow = FlowMeterReadSampleData[cTemp].TotalFlow - FlowMeterReadSampleData[ucTemp].TotalFlow;
						FlowMeterReadSampleData[cTemp].TotalFlow -= FlowMeterReadSampleData[ucTemp].TotalFlow;
					}
					if(FlowMeterReadSampleData[cTemp].CurrentFlow > FlowMeterReadSampleData[ucTemp].CurrentFlow)
					{	FlowMeterReadSampleData[cTemp].CurrentFlow += FlowMeterReadSampleData[ucTemp].CurrentFlow;
						FlowMeterReadSampleData[ucTemp].CurrentFlow = FlowMeterReadSampleData[cTemp].CurrentFlow - FlowMeterReadSampleData[ucTemp].CurrentFlow;
						FlowMeterReadSampleData[cTemp].CurrentFlow -= FlowMeterReadSampleData[ucTemp].CurrentFlow;
					}
				}
			}
			//testing code		
			//for(cTemp=0;cTemp<MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)//testing code
			//{	sprintf(CopyBuff,"\r\FlowMeterReadSampleData[%d]=%d",cTemp,FlowMeterReadSampleData[cTemp]);//testing code
			//	PutStringUsb(CopyBuff);//testing code
			//}//testing code
					
			AtlasFlowMeter.TotalFlowAfterRestart = FlowMeterReadSampleData[MAX_NO_OF_SAMPLE_COLLECTION/2].TotalFlow;//GET THE MEDIAN VALUE
			AtlasFlowMeter.CurrentFlow = FlowMeterReadSampleData[MAX_NO_OF_SAMPLE_COLLECTION/2].CurrentFlow;
			
			if(Mode == MODE_AUTO && CurrentProcessStep == PURIFY){
				//flow calculation for DAC output
				fTemp = ((float)AtlasFlowMeter.CurrentFlow) / 100;
				
				//Pure optimized output voltage via formula Vnew = P*(FlowSet-FlowCurrent)*Vout
				//u.f[0] = AnaOutProportionalFactor * (TargetPureFlowLperM/10 - fTemp) + (LastCycleOptimizedOutputVoltageCode * 25);
				//if(u.f[0] > 4800)
				//	u.f[0] = 4800;
				//u.f[0] /= 25;
				//LastCycleOptimizedOutputVoltageCode = (int)u.f[0];
				
				
				
				//sprintf(CopyBuff,"\r\n-----Current Flow=%.2f, AnaOut=%dmV----",fTemp,LastCycleOptimizedOutputVoltageCode*25);
				//PutStringUsb(CopyBuff);
				
				//my logic
				if(abs(AtlasFlowMeter.CurrentFlow-(TargetPureFlowLperM*10)) > 10)
				{	if(fTemp < ((float)TargetPureFlowLperM)/10)
					{	if((OptimizedOutputVoltageHiCode-OptimizedOutputVoltageLoCode) < 5){
							if(OptimizedOutputVoltageHiCode < 179)
								OptimizedOutputVoltageHiCode += 5;
						}
						OptimizedOutputVoltageLoCode = LastCycleOptimizedOutputVoltageCode;
					}
					else
					{	if((OptimizedOutputVoltageHiCode-OptimizedOutputVoltageLoCode) < 5){
							if(OptimizedOutputVoltageLoCode > 9)
								OptimizedOutputVoltageLoCode -= 5;
						}
						OptimizedOutputVoltageHiCode = LastCycleOptimizedOutputVoltageCode;
					}
					LastCycleOptimizedOutputVoltageCode = (OptimizedOutputVoltageHiCode + OptimizedOutputVoltageLoCode)/2;
				}
				Ppg_SetDuty(5,LastCycleOptimizedOutputVoltageCode);
				//TESTING CODE	
				//sprintf(CopyBuff,"\r\nTarget=%f, Cur=%f, Diff=%d, OutCode=%d HiCode=%d LoCode=%d",
				//		(float)(TargetPureFlowLperM)/10,
				//		fTemp,
				//		abs(AtlasFlowMeter.CurrentFlow-(TargetPureFlowLperM*10)),
				//		LastCycleOptimizedOutputVoltageCode,
				//		OptimizedOutputVoltageHiCode,
				//		OptimizedOutputVoltageLoCode);
				//PutStringUsb(CopyBuff);
			}
			   
			//if(AverageFlowFlag == TRUE){
				//calculate the average flow
				AverageFlowAccumulator += AtlasFlowMeter.CurrentFlow;
				AverageFlowCounter++;
				//if(AverageFlowCounter > 3)
					AverageFlow = AverageFlowAccumulator / AverageFlowCounter;
				if(AverageFlowCounter >= 250)
				{	AverageFlowCounter = 0;
					AverageFlowAccumulator = 0;
				}
				//sprintf(CopyBuff,"\r\nAverage Flow = %d",AverageFlow);
				//PutStringUsb(CopyBuff);
				if(AverageFlow > LowFlowWasteAlarmSetpoint)
					RegisterAlarm(LOW_PRESSURE_ALARM_FLAG, ALARM_RESOLVED);
		}
	}
    else    //no response from flow meter
    {   if(FlowmeterCommTimeOutSec > 30)
        {   RegisterAlarm(FLOWMETER_COMM_ALARM_FLAG, ALARM_OCCURED);   
        }
    }
}
void ResetAverageFlow()
{   AverageFlow = 0;
	AverageFlowAccumulator = 0;
	AverageFlowCounter = 0;
}
///////////////////////////////////////////////////////////////////////////////
void SendFlowMeterCommand(char *command)
{	if(FlowMeterDebugFlag == TRUE)
	{	PutStringUsb("\r\n---\r\n");
		PutStringUsb(command);
	}
	PutStringAtlasFlowmeter(command);
	AtlasFlowmeterReceiveCounter = 0;
	AtlasFlowmeterBuff[AtlasFlowmeterReceiveCounter] = '\0';
	PutCharAtlasFlowmeter(']');
}	
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void ATLAS_FLOWMETER_RX_IRQHandler(void)
{	//MAIN_PROG_LED_OFF;
	if ((ATLAS_FLOWMETER_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	ATLAS_FLOWMETER_UART->SSR |= (1 << 7);      // Clear error flags 
    	AtlasFlowmeterReceiveByte = ATLAS_FLOWMETER_UART->RDR;         // Flush RDR
  	}
  	else if (ATLAS_FLOWMETER_UART->SSR & 0x04)    // RDRF?
  	{	AtlasFlowmeterReceiveByte = ATLAS_FLOWMETER_UART->RDR;
		AtlasFlowmeterByteReceiveFlag = TRUE;
    	if(AtlasFlowmeterReceivePacketCompleteFlag == FALSE)
        {   switch(AtlasFlowmeterReceiveByte)
			{ 	//case '[': 	AtlasFlowmeterReceiveCounter = 0; AtlasFlowmeterBuff[0]='\0';break;
				//case ']': 	AtlasFlowmeterBuff[AtlasFlowmeterReceiveCounter]='\0';AtlasFlowmeterResponseFlag = TRUE; break;
				//case 8:		if(AtlasFlowmeterReceiveCounter>0) AtlasFlowmeterReceiveCounter--;break;//backspace
				/*case '*':	if(AtlasFlowmeterStartCodeFlag == FALSE)
							{	AtlasFlowmeterReceiveCounter = 0; 
								AtlasFlowmeterBuff[0]='\0';
								AtlasFlowmeterStartCodeFlag = TRUE;
							}
							else
							{	AtlasFlowmeterBuff[AtlasFlowmeterReceiveCounter%20]=AtlasFlowmeterReceiveByte;
								AtlasFlowmeterReceiveCounter++;
							}
							break;
				case '\r':	//if(AtlasFlowmeterStartCodeFlag == TRUE)
							{	AtlasFlowmeterBuff[AtlasFlowmeterReceiveCounter%20]='\0';
								AtlasFlowmeterReceivePacketCompleteFlag = TRUE;
								AtlasFlowmeterStartCodeFlag = FALSE;//FOR NEXT PACKET
							}
							break;*/
				default:  	AtlasFlowmeterBuff[AtlasFlowmeterReceiveCounter%20] = AtlasFlowmeterReceiveByte; 
							AtlasFlowmeterReceiveCounter++;
							AtlasFlowmeterBuff[AtlasFlowmeterReceiveCounter%20] = '\0';
							AtlasFlowmeterBuff[20] = '\0';//confirming no overflow while printing
							break;
			}
			//PutCharUsb(AtlasFlowmeterReceiveByte);
			if(AtlasFlowmeterEchoFlag == TRUE)
				PutCharAtlasFlowmeter(AtlasFlowmeterReceiveByte);
		}
  	}
}
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
char CheckAtlasFlowmeterResponse(char* response,unsigned int gstate)
{	//char i;
    AtlasFlowmeterReceiveByte = FALSE;
    for(;gstate>0;gstate--)
    {   FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
		FM3_SWWDT->WDOGINTCLR = 0x11111111;
		FM3_SWWDT->WDOGLOCK = 0;
		
		DelayInMilliSec(1);
        if(AtlasFlowmeterByteReceiveFlag == TRUE || AtlasFlowmeterReceiveCounter>0)
        {   while(AtlasFlowmeterReceiveByte == TRUE)
            {   AtlasFlowmeterByteReceiveFlag = FALSE;
                DelayInMilliSec(1);
            }
            if(SearchString(AtlasFlowmeterBuff,response) != -1)
			  	return TRUE;//FOUND
        }
    }
    return FALSE; //NOT FOUND			
}
///////////////////////////////////////////////////////////////////////////////
void InitAtlasFlowmeter()
{	bTemp = TRUE; //FLAG TO INDICATE ISSUE
	InitUartAtlasFlowmeter();
	DelayInMilliSec(1);
	//strcpy(LcdLine1,"Initializing    ");//testing code
	//strcpy(LcdLine2,"Flowmeter.......");//testing code
	//SendSerialHmiPacket();//testing code
	SendFlowMeterCommand("RESPONSE,1");//ENABLE OK RESPONSE
	DelayInMilliSec(10);
	for(ucTemp = 0;ucTemp<100;ucTemp++)
	{	SendFlowMeterCommand("C,?");//DISABLE continous reading mode
		if(CheckAtlasFlowmeterResponse("*OK",10)==TRUE)
		{	bTemp = FALSE;
			break;
		}
	}
	SendFlowMeterCommand("RESPONSE,0");//DISABLE OK RESPONSE
	if(CheckAtlasFlowmeterResponse("*ER",10)==TRUE)
	{	bTemp = TRUE;
	}
	SendFlowMeterCommand("P,10");//pull up set to 10K
	if(CheckAtlasFlowmeterResponse("*ER",10)==TRUE)
	{	bTemp = TRUE;
	}
	SendFlowMeterCommand("TK,M");//time base in minute
	if(CheckAtlasFlowmeterResponse("*ER",10)==TRUE)
	{	bTemp = TRUE;
	}
	SendFlowMeterCommand("K,clear");//clear all k values stored inside
	if(CheckAtlasFlowmeterResponse("*ER",10)==TRUE)
	{	bTemp = TRUE;
	}
	sprintf(CopyBuff,"K,1,%d",FlowmeterKfactor);
	SendFlowMeterCommand(CopyBuff);//volume per pulse i.e 1 liter per 720 pulse
	if(CheckAtlasFlowmeterResponse("*ER",10)==TRUE)
	{	bTemp = TRUE;
	}
	if(bTemp == TRUE)
	{	//strcpy(LcdLine1,"Initializing    ");//testing code
		//strcpy(LcdLine2,"Flowmeter ERROR.");//testing code
		//SendSerialHmiPacket();//testing code
		//PutStringUsb("\r\nFlowmeter Initialization Error");
		//DelayInSec(2);
		BOOTLOADER_LED_ON;
		DelayInMilliSec(100);
		BOOTLOADER_LED_OFF;
	}
}
void InitUartAtlasFlowmeter()
{	// Initialize ports for UART
  	ATLAS_FLOWMETER_UART_PFR;
	ATLAS_FLOWMETER_UART_EPFR;  
  	
  	// Initialize UART asynchronous mode
  	ATLAS_FLOWMETER_UART->SCR =  0x80;   // UART reset
  	ATLAS_FLOWMETER_UART->SMR =  0x01;   // serial data output enable, operation mode 0
  	ATLAS_FLOWMETER_UART->SSR =  0x00;   // 0x80 = clear receive error flag
  	ATLAS_FLOWMETER_UART->ESCR = 0x00;          
 	
  	ATLAS_FLOWMETER_UART->BGR = ((40000000UL / (ATLAS_FLOWMETER_UART_BAUDRATE -1))/2);
  	
	ATLAS_FLOWMETER_UART->SCR |= 0x03;   // RX, TX enable  
  	//FM3_MFS0_UART->FBYTE = 0X0003;//INTERRUPT ON FIVE BYTE RECEIVE TESTED
  
  	// UART interrupt settings
  	NVIC_ClearPendingIRQ(ATLAS_FLOWMETER_RX_IRQn);
  	NVIC_EnableIRQ(ATLAS_FLOWMETER_RX_IRQn);
  	NVIC_SetPriority(ATLAS_FLOWMETER_RX_IRQn, 3);
  	
  	ATLAS_FLOWMETER_UART->SCR |= 0x10;//set RIE serial receive interrupt enable bit
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharAtlasFlowmeter(uint8_t u8Char)
{  	
#ifdef FLOWMETER_FUNCTION_ENABLE
	while (!(ATLAS_FLOWMETER_UART->SSR & (1 << 1))); // wait for transmit buffer empty    
  	if(u8Char == '\r')
	{	AtlasFlowmeterReceiveCounter = 0;
		AtlasFlowmeterBuff[0] = '\0';
		AtlasFlowmeterReceivePacketCompleteFlag = FALSE;
		AtlasFlowmeterStartCodeFlag = FALSE;
	}
	ATLAS_FLOWMETER_UART->TDR = u8Char;                // put u8Char into buffer
  	//UsbDeviceCdcCom_SendByte(u8Char);
	//AtlasFlowmeterCrcCodeOut += u8Char;
  	Delay(500);
#endif
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *  RETURNS:        None
 *****************************************************************************/
void PutStringAtlasFlowmeter(char *pu8Buffer)              
{	char* buff;
	buff = (char*)pu8Buffer;
  	while (*buff != '\0')
  	{ 	PutCharAtlasFlowmeter(*buff++);	// send every char of string
    	Delay(65000);Delay(65000); //must for long replay from gprs
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
