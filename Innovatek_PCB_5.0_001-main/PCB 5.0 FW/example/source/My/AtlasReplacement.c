#include "AtlasReplacement.h"
#include "Delay.h"
#include "Variable.h"
#include "myfun.h"
#include <string.h>
#include <stdio.h>
#include "SerialHmi.h"
#include "constants.c"
#include <ppg.h>
#include <stdlib.h>
#include <math.h>
#include "DualTimer.h"

//just for testing
#include "MyUsb.h"
#include "gpio.h"

//variable local to the file
char AtlasReplacementBuff[ATLAS_BUFF_SIZE];
uc AtlasReplacementReceiveByte;
uc AtlasReplacementReceiveCounter = 0;
uc AtlasReplacementCommTimeOutSec = 0;
boolean_t AtlasReplacementReceivePacketCompleteFlag = FALSE;
boolean_t AtlasReplacementEchoFlag = FALSE;
boolean_t AtlasReplacementResponseFlag = FALSE;
boolean_t AtlasReplacementStartCodeFlag = FALSE;
boolean_t AtlasReplacementDebugFlag = FALSE;
boolean_t AtlasReplacementByteReceiveFlag = FALSE;
//variable related to temperature
#define RECALIBRATION_SEC 5
boolean_t AtlasTemperatureErrorFlag = FALSE;
int AtlasTemperature = 0;
ui AtlasTemperatureContinuousErrorSec = 0;
int RawWaterTemperature;
int TemperatureOffsetRegister;

//variable related to conductivity
//int RawWaterConductivity;
//int AtlasUncompensatedConductivity = -1;
//long long AverageConductivityAccumulator = 0;
//uc AverageConductivityCounter = 0;

//related to flow
#define MAX_NO_OF_SAMPLE_COLLECTION 5
uc FlowmeterCommTimeOutSec = 0;
struct struct_flowmeter FlowMeter;

///////////////////////////////////////////////////////////////////////////////
//function local to the file
void InitUartAtlasReplacement();
void PutCharAtlasReplacement(uint8_t u8Char);
void PutStringAtlasReplacement(char *pu8Buffer);
void SendAtlasReplacementCommand(char *command);

///////////////////////////////////////////////////////////////////////////////
void ClearAllFlow()
{   FlowMeter.TotalFlowAfterRestart = 0;
    TotalPureVolume = 0;
    TotalPureVolumeB4CycleStart = 0;
    StartVolume = 0;
}
///////////////////////////////////////////////////////////////////////////////
void RecalibrateTemperatureIfNeeded()
{   /*boolean_t result = FALSE;
    if(AtlasTemperatureErrorFlag == TRUE)
    {   if(AtlasTemperatureContinuousErrorSec % RECALIBRATION_SEC == 0)
        {   AtlasTemperatureErrorFlag = FALSE;
            //AtlasTemperatureContinuousErrorSec = 0;
            //Set Atlas Temp to ACTIVE state
            u.c[0] = 1;
            WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
			if(I2C_ErrorFlag == FALSE)
			{   //Clear Atlas Temp^ Mod Calibration
                u.c[0] = 1;
                WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,12,u.c,1);
                if(I2C_ErrorFlag == FALSE)
                {   //Set Atlas Temp^ Mod Calibration
                    u.i[1] = AtlasTemperature;
                    //arrange the 4 byte integer value to send in MSB first 
                    u.c[0] = u.c[7];
                    u.c[1] = u.c[6];
                    u.c[2] = u.c[5];
                    u.c[3] = u.c[4];
                    WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,8,u.c,4);//USE MSB FIRST VARIABLE
                    if(I2C_ErrorFlag == FALSE)
                    {    //Set Atlas Temp^ Mod Calibration Request-Single Point
                        u.c[0] = 2;
                        WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,12,u.c,1);
                        if(I2C_ErrorFlag == FALSE)
                        {   //Get Atlas Temp^ Mod Calibration Confirmation
                            ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,13,u.c,1);
                            if(I2C_ErrorFlag == FALSE && u.c[0] == 1)
                            {    result = TRUE;
                            }
                        }
                    }
                }
            }
                
            if(result == FALSE)
            {   PutStringUsb("\r\nATLAS Temperature-> Auto Calibration Error");
            }
        }
    }*/
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAtlasReplacementReading()//all three READING EVERY ONE SEC
{   static uc read_child_processor_sec = 0;
    
    //#ifdef MY_COMPENSATED_CONDUCTIVITY_LOGIC
     //   #ifdef TEMPERATURE_COMPENSATED_CONDUCTIVITY
            static ui TemperatureReadSampleData[TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION];
            static uc temperature_read_val_counter = 0;
    //    #endif
    //#endif        
    if(FreeRunSec%10 != read_child_processor_sec)//READING EVERY ONE SEC
    {   read_child_processor_sec = FreeRunSec%10;
    
        SendAtlasReplacementCommand("{123=?");
        while(AtlasReplacementResponseFlag == FALSE)
        {   DelayInMilliSec(1);//this delay is must
            if(AtlasReplacementCommTimeOutSec > 1)
            {   //action for comm error
                break;
            }
        }
        if(AtlasReplacementDebugFlag == TRUE)
        {	PutStringUsb("\r\nAR -> ");
                PutStringUsb(AtlasReplacementBuff);
        }
        //FlowMeter.CurrentFlow = 200;
        
        if(AtlasReplacementResponseFlag == TRUE)//123=13342,0,23.21,0.00
        {   sscanf(AtlasReplacementBuff,"%d=%d,%d,%d,%d",&uiTemp,&RawWaterTemperature,&iTemp,&u.i[0],&u.i[1]);
            //sprintf(CopyBuff,"\r\nAFTER:%d=%d,%d,%f,%f",uiTemp,RawWaterTemperature,iTemp,u.f[0],u.f[1]);
            //PutStringUsb(CopyBuff);
            if(uiTemp == 123)
            {   FlowMeter.TotalFlowAfterRestart += ((float)(u.i[0]))/FlowMeter.K_Factor;
                FlowMeter.CurrentFlow = ((float)u.i[1]*60)/FlowMeter.K_Factor;
                //sprintf(CopyBuff,"\r\nCuC=%d=%d,%d,%d,%d,%.2f,%.2f,%d",uiTemp,RawWaterTemperature,iTemp,u.i[0],u.i[1],FlowMeter.TotalFlowAfterRestart,FlowMeter.CurrentFlow,FlowMeter.K_Factor);
                //PutStringUsb(CopyBuff);
                
                //u.f[0] = (float)RawWaterTemperature;
                //RawWaterTemperature = (int)(((u.f[0] - 777.1)/1.7824) * 1000); //VOLTEA FORMULA FOR 12BIT MODE
                //ADC_MeasuredVoltage = (float)ADC_MeasuredTemperatureDigitalValue * 0.0000625; 
                //CalculatedResistance = ((330.0 * ADC_MeasuredVoltage)/(3.3 - ADC_MeasuredVoltage));
                //FilteredTemperature = ((CalculatedResistance/100.0)-1)/0.003851;
                /*if(RawWaterTemperature < 1 || RawWaterTemperature > 60000)
                {   PutStringUsb("\r\nREBOOTING PCB DUE TO TEMPERATURE ERROR");
                    DelayInSec(5);
                    RESET_DEVICE;
                }*/		
                u.f[0] = (float)RawWaterTemperature * 0.0000625;
                u.f[1] = ((330.0 * u.f[0])/(3.3 - u.f[0]));
                RawWaterTemperature = (int)(((u.f[1]/100)-1)/0.000003851);//must have been this
                //RawWaterTemperature = (int)(((u.f[1]/100)-1)/0.000004);
                //RawWaterTemperature += TemperatureOffsetRegister;
                
                TemperatureReadSampleData[temperature_read_val_counter] = RawWaterTemperature + TemperatureOffsetRegister;
                /*testing code start *
                sprintf(CopyBuff,"\r\nTemperature(%d) = RawWaterTemperature(%d) + TemperatureOffsetRegister(%d)",
                        TemperatureReadSampleData[temperature_read_val_counter],
                        RawWaterTemperature,
                        TemperatureOffsetRegister);
                PutStringUsb(CopyBuff); 
                //testing code end */
                if(++temperature_read_val_counter == TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION){
                    temperature_read_val_counter = 0;
                    //arrange the sample into assending order
                    for(cTemp=0;cTemp<TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)
                    {	for(ucTemp=cTemp+1;ucTemp<TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION;ucTemp++)
                        {	if(TemperatureReadSampleData[cTemp] > TemperatureReadSampleData[ucTemp])
                            {	TemperatureReadSampleData[cTemp] += TemperatureReadSampleData[ucTemp];
                                TemperatureReadSampleData[ucTemp] = TemperatureReadSampleData[cTemp] - TemperatureReadSampleData[ucTemp];
                                TemperatureReadSampleData[cTemp] -= TemperatureReadSampleData[ucTemp];
                            }
                        }
                    }
                            
                    //for(cTemp=0;cTemp<CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)//testing code
                    //{	sprintf(CopyBuff,"\r\nConductivityReadSampleData[%d]=%d",cTemp,ConductivityReadSampleData[cTemp]);//testing code
                    //    PutStringUsb(CopyBuff);//testing code
                    //}//testing code
                    
                    if(TemperatureReadSampleData[TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION/2] > 0 && TemperatureReadSampleData[TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION/2] < 100000)
                    {   AtlasTemperature = TemperatureReadSampleData[TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION/2];//GET THE MEDIAN VALUE
                        AtlasTemperatureErrorFlag = FALSE;
                        AtlasTemperatureContinuousErrorSec = 0;
                         
                    }
                    else
                    {    AtlasTemperatureErrorFlag = TRUE;
                    }
                    /*
                    //testing code start
                    sprintf(CopyBuff,"\r\nCURRENT TEMPERATURE = %.2f",
                            (float)TemperatureReadSampleData[TEMPERATURE_MAX_NO_OF_SAMPLE_COLLECTION/2]/1000);
                    PutStringUsb(CopyBuff);
                    //testing code end
                    */
                }
                
                
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////
void PureAnalogVoltageOut()
{   static uc OptimizedOutputVoltageHiCode=192;
	static uc OptimizedOutputVoltageLoCode=0;
	static uc PureAnalogVoltageTimeStamp = 0;

    if(FreeRunSec%10 != PureAnalogVoltageTimeStamp)
    {   PureAnalogVoltageTimeStamp = FreeRunSec%10;
        
        if(Mode == MODE_AUTO)
        {   //flow calculation for DAC output
            fTemp = FlowMeter.CurrentFlow;;
                    
            //Pure optimized output voltage via formula Vnew = P*(FlowSet-FlowCurrent)*Vout
            //u.f[0] = AnaOutProportionalFactor * (TargetPureFlowLperM/10 - fTemp) + (LastCycleOptimizedOutputVoltageCode * 25);
            //if(u.f[0] > 4800)
            //	u.f[0] = 4800;
            //u.f[0] /= 25;
            //LastCycleOptimizedOutputVoltageCode = (int)u.f[0];
            //sprintf(CopyBuff,"\r\n-----Current Flow=%.2f, AnaOut=%dmV----",fTemp,LastCycleOptimizedOutputVoltageCode*25);
            //PutStringUsb(CopyBuff);
                    
            //my logic
            // max value that OptimizedOutputVoltageHiCode can have is 201
            // max value that LastCycleOptimizedOutputVoltageCode  can have is 200
            if(abs((ui)(FlowMeter.CurrentFlow*100)-(TargetPureFlowLperM*10)) > 10)
            {	if(fTemp < ((float)TargetPureFlowLperM)/10)
                {	if((OptimizedOutputVoltageHiCode-OptimizedOutputVoltageLoCode) < 5){
                        if(OptimizedOutputVoltageHiCode <= 195)
                            OptimizedOutputVoltageHiCode += 5;
                        else if (OptimizedOutputVoltageHiCode <= 200)
                          OptimizedOutputVoltageHiCode++;
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
    }
}
void FlowAverageCalc()
{   		   
    //if(AverageFlowFlag == TRUE){
        //calculate the average flow
        AverageFlowAccumulator += FlowMeter.CurrentFlow;
        AverageFlowCounter++;
        //if(AverageFlowCounter > 3)
            AverageFlow = AverageFlowAccumulator / AverageFlowCounter;
        if(AverageFlowCounter >= 250)
        {	AverageFlowCounter = 0;
            AverageFlowAccumulator = 0;
        }
        //sprintf(CopyBuff,"\r\nAverage Flow = %d",AverageFlow);
        //PutStringUsb(CopyBuff);
        if(AverageFlow > LowFlowWasteAlarmSetpoint/10)
            RegisterAlarm(LOW_PRESSURE_ALARM_FLAG, ALARM_RESOLVED);
    //}
}
///////////////////////////////////////////////////////////////////////////////
void ResetAverageFlow()
{   AverageFlow = 0;
	AverageFlowAccumulator = 0;
	AverageFlowCounter = 0;
}
///////////////////////////////////////////////////////////////////////////////
void InitAtlasReplacement()
{	InitUartAtlasReplacement();
}
///////////////////////////////////////////////////////////////////////////////
void SendAtlasReplacementCommand(char *command)
{	if(AtlasReplacementDebugFlag == TRUE)
	{	PutStringUsb("\r\n---\r\n");
		PutStringUsb(command);
	}
	PutStringAtlasReplacement(command);
	AtlasReplacementReceiveCounter = 0;
	AtlasReplacementBuff[AtlasReplacementReceiveCounter] = '\0';
    AtlasReplacementResponseFlag = FALSE;
	PutCharAtlasReplacement('}');
    //PutStringAtlasReplacement("}");
    AtlasReplacementCommTimeOutSec = 0;
}	
///////////////////////////////////////////////////////////////////////////////

void InitUartAtlasReplacement()
{	// Initialize ports for UART
  	ATLAS_REPLACEMENT_UART_PFR;
	ATLAS_REPLACEMENT_UART_EPFR;  
  	
  	// Initialize UART asynchronous mode
  	ATLAS_REPLACEMENT_UART->SCR =  0x80;   // UART reset
  	ATLAS_REPLACEMENT_UART->SMR =  0x01;   // serial data output enable, operation mode 0
  	ATLAS_REPLACEMENT_UART->SSR =  0x00;   // 0x80 = clear receive error flag
  	ATLAS_REPLACEMENT_UART->ESCR = 0x00;          
 	
  	ATLAS_REPLACEMENT_UART->BGR = ((40000000UL / (ATLAS_REPLACEMENT_UART_BAUDRATE -1))/2);
  	
	ATLAS_REPLACEMENT_UART->SCR |= 0x03;   // RX, TX enable  
  	//FM3_MFS0_UART->FBYTE = 0X0003;//INTERRUPT ON FIVE BYTE RECEIVE TESTED
  
  	// UART interrupt settings
  	NVIC_ClearPendingIRQ(ATLAS_REPLACEMENT_RX_IRQn);
  	NVIC_EnableIRQ(ATLAS_REPLACEMENT_RX_IRQn);
  	NVIC_SetPriority(ATLAS_REPLACEMENT_RX_IRQn, 3);
  	
  	ATLAS_REPLACEMENT_UART->SCR |= 0x10;//set RIE serial receive interrupt enable bit
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharAtlasReplacement(uint8_t u8Char)
{  	
#ifdef ATLAS_REPLACEMENT_FUNCTION_ENABLE
	while (!(ATLAS_REPLACEMENT_UART->SSR & (1 << 1))); // wait for transmit buffer empty    
  	if(u8Char == '\r')
	{	AtlasReplacementReceiveCounter = 0;
		AtlasReplacementBuff[0] = '\0';
		AtlasReplacementReceivePacketCompleteFlag = FALSE;
		AtlasReplacementStartCodeFlag = FALSE;
	}
	ATLAS_REPLACEMENT_UART->TDR = u8Char;                // put u8Char into buffer
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
void PutStringAtlasReplacement(char *pu8Buffer)              
{	char* buff;
	buff = (char*)pu8Buffer;
  	while (*buff != '\0')
  	{ 	PutCharAtlasReplacement(*buff++);	// send every char of string
    	Delay(65000);Delay(65000); //must for long replay from gprs
  	}
//	PutStringUsb(pu8Buffer);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ATLAS_REPLACEMENT_RX_IRQHandler(void)
{	//MAIN_PROG_LED_ON;
    //BOOTLOADER_LED_ON;
	if ((ATLAS_REPLACEMENT_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	ATLAS_REPLACEMENT_UART->SSR |= (1 << 7);      // Clear error flags 
    	AtlasReplacementReceiveByte = ATLAS_REPLACEMENT_UART->RDR;         // Flush RDR
  	}
  	else if (ATLAS_REPLACEMENT_UART->SSR & 0x04)    // RDRF?
  	{	AtlasReplacementReceiveByte = ATLAS_REPLACEMENT_UART->RDR;
        //CopyBuff[AtlasReplacementReceiveCounter]=AtlasReplacementReceiveByte;
        //CopyBuff[AtlasReplacementReceiveCounter+1] = '\0';
		AtlasReplacementByteReceiveFlag = TRUE;
    	if(AtlasReplacementReceivePacketCompleteFlag == FALSE)
        {   switch(AtlasReplacementReceiveByte)
            { 	case '{': 	AtlasReplacementReceiveCounter = 0; AtlasReplacementBuff[0]='\0';break;
                case '}': 	AtlasReplacementBuff[AtlasReplacementReceiveCounter]='\0';AtlasReplacementResponseFlag = TRUE; break;
				//case 8:		if(AtlasReplacementReceiveCounter>0) AtlasReplacementReceiveCounter--;break;//backspace
				/*case '*':	if(AtlasReplacementStartCodeFlag == FALSE)
							{	AtlasReplacementReceiveCounter = 0; 
								AtlasReplacementBuff[0]='\0';
								AtlasReplacementStartCodeFlag = TRUE;
							}
							else
							{	AtlasReplacementBuff[AtlasReplacementReceiveCounter%20]=AtlasReplacementReceiveByte;
								AtlasReplacementReceiveCounter++;
							}
							break;
				case '\r':	//if(AtlasReplacementStartCodeFlag == TRUE)
							{	AtlasReplacementBuff[AtlasReplacementReceiveCounter%20]='\0';
								AtlasReplacementReceivePacketCompleteFlag = TRUE;
								AtlasReplacementStartCodeFlag = FALSE;//FOR NEXT PACKET
							}
							break;*/
				default:  	AtlasReplacementBuff[AtlasReplacementReceiveCounter % ATLAS_BUFF_SIZE] = AtlasReplacementReceiveByte; 
							AtlasReplacementReceiveCounter++;
							AtlasReplacementBuff[AtlasReplacementReceiveCounter % ATLAS_BUFF_SIZE] = '\0';
							AtlasReplacementBuff[ATLAS_BUFF_SIZE-1] = '\0';//confirming no overflow while printing
							break;
			}
			//PutCharUsb(AtlasReplacementReceiveByte);
			if(AtlasReplacementEchoFlag == TRUE)
				PutCharAtlasReplacement(AtlasReplacementReceiveByte);
		}
  	}
}
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
