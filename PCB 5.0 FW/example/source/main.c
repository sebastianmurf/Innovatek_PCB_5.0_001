/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)           */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user's obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FSEU does not warrant that the deliverables do not infringe any      */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FSEU disclaims all */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FSEU's liability  */
/* is restricted to intentional misconduct and gross negligence.        */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.5)                                                               */
/************************************************************************/


//#include "usb.h"
//#include "UsbInit.h"
//#include "bootloader.h"
//#include "bootloaderapi.h"
#include "DualTimer.h"
#include "GpsUart.h"
#include "gprs.h"
#include "GprsUart.h"
#include "constants.c"
#include "gpio.h"
#include "delay.h"
#include "myfun.h"
#include "VARIABLE.h"
#include "command.h"
#include "AT45DB161.h"
#include "mmu.h"
#include "exint.h"
#include "led.h"
#include "board.h"
#include "MyUsb.h"
#include "Atlas_T.h"
#include "Atlas_TC.h"
#include "AtlasReplacement.h"
#include "Atlas_C.h"
//#include "LCD.h"
#include "MyExtInt.h"
//#include "pc.h"
#include <stdio.h>
#include <ctype.h>
#include "mybootloader.h"
#ifdef ONE_WIRE
	#include "DS18B20.h"
#endif
#include "SerialHmi.h"
#include "PAE800.h"
#include "SerialPort.h"
#include "ProjectFunc.h"
//#include "AutoMode.h"

  /**
 ******************************************************************************
 ** \brief  Main function of project for MB9A310 series.
 **
 ** \param  none
 ** \return uint32_t return value, if needed
 ******************************************************************************/
//#define USER_FLASH_START 0x00004000
extern void AutoModeOperations();
extern void BypassOperation();
void PostWasteTask();

int32_t main(void)  
{ //bootloader code setting completed and tested for stage 1
   
  //USER APPLICATION CODE STARTS FROM HERE
    #ifdef WATCH_DOG_TIMER_ENABLED
        //watch dog timer configuration starts
        FM3_SWWDT->WDOGLOAD = 0x007fffFF;
        FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
        //ucTemp = FM3_SWWDT->WDOGLOCK;
        //sprintf(PasteBuff,"\r\nswdt lock = %d", ucTemp);
        //PutStringUsb(PasteBuff);//must read 0
        FM3_SWWDT->WDOGCONTROL = 0x03;
        FM3_SWWDT->WDOGLOCK = 0;
        //ucTemp = FM3_SWWDT->WDOGLOCK;
        //sprintf(PasteBuff,"\r\nswdt lock = %d", ucTemp);
        //PutStringUsb(PasteBuff);//must read 1
        NVIC_EnableIRQ(SWDT_IRQn);
        //watch dog timer configuration ends
    #endif
  	Board_Init();
	BOOTLOADER_LED_OFF;
	MAIN_PROG_LED_ON;
	DO1_RELAY_OFF;
	DO2_RELAY_OFF;
	DO3_RELAY_OFF;
	DO4_RELAY_OFF;
	DO5_RELAY_OFF;
	DO6_RELAY_OFF;
	DO7_RELAY_OFF;
	DO8_RELAY_OFF;
        DO9_RELAY_OFF;
	DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
//	DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
	//ChipErase();
	
    /*  Below code will give delay in process if restart happened to be when USB is conneted
	for(ucTemp=0;ucTemp<20;ucTemp++)
	{	CheckUsbStage();
		if(WaitForConnectionFlag == TRUE)//IF DURING POWER ON USB IS DETECTED
			DelayInMilliSec(1000);
		else
		  	break;
	}
    */	
//	InitPAE800();
	
	Ppg_Init();
	Ppg_SetDuty(5,PwmDuty);
	Ppg_SetCycle(5,PwmCycle);
	
	InitDualTimer();
  
	InitUartSerialHmi();
	InitUartSerialPort();
    
	MobileStage = MOBILE_STAGE_POWER_UP_WAIT;
	ConfigureVariable();
		
	#ifdef ATLAS_REPLACEMENT_FUNCTION_ENABLE
		InitAtlasReplacement();//InitAtlasFlowmeter();//it takes 2 sec to boot itself so initialize it at delay
	#endif
	
	//power off log
	//**************************************************************************
	
	//read last log record
	if(LogPtr > SECOND_LOG_START_ADD)
	  uiTemp = LogPtr-SECOND_LOG_PACKET_SIZE;
	else
	  uiTemp = SECOND_LOG_START_ADD;
	DirectMemRead(uiTemp,PasteBuff,SECOND_LOG_PACKET_SIZE);
	u.c[1] = PasteBuff[LT_SECOND_DATEp+1];
	if(u.c[1] > 0 && u.c[1] < 13)//in case of valid month data
	{	//date
		Device.TimeStamp.tm_year = PasteBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
		Device.TimeStamp.tm_mon = PasteBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
		Device.TimeStamp.tm_mday = PasteBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
		//time
		Device.TimeStamp.tm_hour = PasteBuff[LT_SECOND_TIMEp];// = Device.TimeStamp.tm_hour;
		Device.TimeStamp.tm_min = PasteBuff[LT_SECOND_TIMEp+1];// = Device.TimeStamp.tm_min;
		Device.TimeStamp.tm_sec = PasteBuff[LT_SECOND_TIMEp+2];// = Device.TimeStamp.tm_sec;
        //cycle: recover from periodic save not from log 
        /*NTC:21.09.20 21:24 START: this is read from Periodic Save Page
		u.i[0] = 0;//CycleCounter;
		u.c[0] = PasteBuff[LT_SECOND_CYCLEp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_CYCLEp+1];// = u.c[1];
		CycleCounter = u.i[0];
        NTC:21.09.20 21:24 ENDS */
		/*NTC:21.09.01 21:50 START: this is read from Periodic Save Page
        //step
		//CurrentProcessStep = PasteBuff[LT_SECOND_STEPp];// = CurrentProcessStep; this is read from Periodic Save Page
		//Run Sec
		u.i[0] = 0;//ProcessStepRunSec;
		u.c[0] = PasteBuff[LT_SECOND_RUN_SECp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_RUN_SECp+1];// = u.c[1];
		ProcessStepRunSec = u.i[0];
        NTC:21.09.01 21:50 ENDS*/
		//PAE volt
		u.i[0] = 0;//CurrentPAEvolt;
		u.c[0] = PasteBuff[LT_SECOND_PAE_VOLTp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_PAE_VOLTp+1];// = u.c[1];
		CurrentPAEvolt = u.i[0];
		/*NTC:21.09.20 21:24 START: this is read from Periodic Save Page
		// Total Flow
		u.i[0] = 0;//AtlasReplacement.TotalFlow;
		u.c[0] = PasteBuff[LT_SECOND_T_FLOWp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_T_FLOWp+1];// = u.c[1];
		u.c[2] = PasteBuff[LT_SECOND_T_FLOWp+2];// = u.c[2];
		u.c[3] = PasteBuff[LT_SECOND_T_FLOWp+3];// = u.c[3];
		TotalPureVolume = u.i[0]/100;
        TotalPureVolumeB4CycleStart = TotalPureVolume;
		FlowMeter.TotalFlowAfterRestart = 0;
		StartVolume = 0;
		NTC:21.09.20 21:24 ENDS */
        // Current Flow
		u.i[0] = 0;//AtlasReplacement.CurrentFlow;
		u.c[0] = PasteBuff[LT_SECOND_CURRENT_FLOWp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_CURRENT_FLOWp+1];// = u.c[1];
		FlowMeter.CurrentFlow  = u.i[0];
		// EC
		u.i[0] = 0;//AtlasConductivity;
		u.c[0] = PasteBuff[LT_SECOND_AVERAGE_ECp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_AVERAGE_ECp+1];// = u.c[1];
		u.c[2] = PasteBuff[LT_SECOND_AVERAGE_ECp+2];// = u.c[2];
		AtlasConductivity = u.i[0];
		//Alarm
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_SECOND_ALARMp];// =  AlarmRegister24Bit;
		u.c[1] = PasteBuff[LT_SECOND_ALARMp+1];// =  AlarmRegister24Bit;
		u.c[2] = PasteBuff[LT_SECOND_ALARMp+2];// =  AlarmRegister24Bit;
		AlarmRegister24Bit = u.i[0];
		AlarmRegisterForSMS = AlarmRegister24Bit & ALARM_MASK_FOR_SMS;//so that it does not send sms again and again upon every restart
        ////////////////////////////////////////////////////////////////////////
        // CIP PUMP RUN TIME
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp+1];// = u.c[1];
		CIP_PumpRunTimeInSec  = u.i[0];
		// WATER TEMPERATURE
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_SECOND_WATER_TEMPERATUREp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_WATER_TEMPERATUREp+1];// = u.c[1];
		AtlasTemperature  = u.i[0];
		// CABINET TEMPERATURE
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_SECOND_CABINET_TEMPERATUREp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_CABINET_TEMPERATUREp+1];// = u.c[1];
		Temperature.i  = u.i[0];
		// PRESSURE
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_SECOND_PRESSUREp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_PRESSUREp+1];// = u.c[1];
        if(u.i[0] > 32767) //-ve
        	AnaPressureAval  = ((float)(65535 - u.i[0]))/100.0f;
        else
                AnaPressureAval = u.i[0]/100.0f;
                // ANALOG OUTPUT VOLTAGE MILLI VOLT
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp+1];// = u.c[1];
		DAC_FB_Aval  = u.i[0]/10;
		// DIGITAL INPUT
		CurDigitalInput = PasteBuff[LT_DIGITAL_INPUTp];
		// DIGITAL OUTPUT
		u.i[0] = 0;
		u.c[0] = PasteBuff[LT_DIGITAL_OUTPUTp];// = u.c[0];
		u.c[1] = PasteBuff[LT_DIGITAL_OUTPUTp+1];// = u.c[1];
		PrevDigitalOutput = DigitalOutput = u.i[0];
                // % RECOVERY
		//ucTemp = PasteBuff[LT_PERCENTAGE_RECOVERYp];
		// MODE
		//ucTemp = PasteBuff[LT_MODEp];
		//LIVE EC
                //PAE CURRENT
                //PAE volt
		u.i[0] = 0;//CurrentPAEvolt;
		u.c[0] = PasteBuff[LT_SECOND_PAE_CURRENTp];// = u.c[0];
		u.c[1] = PasteBuff[LT_SECOND_PAE_CURRENTp+1];// = u.c[1];
		CurrentPAEcurrent = u.i[0];
	}
	/*else
	{	TotalPureVolumeB4CycleStart = 0;
	}*/
	//ucTemp = CurrentProcessStep;
    //CurrentProcessStep = 0xFF;
    //DateValidFlag =  TRUE; TimeValidFlag = TRUE; //so that it logs the restart record
    //LogRecord();
    if(CommunicationMediaPrefered > COMM_MEDIA_NONE) //so that in server mode it does not logs till vaild date & time is received.
    {   DateValidFlag =  FALSE; 
        TimeValidFlag = FALSE; 
    }
    //NTC:24.08.2021 22:48
    
    //CurrentProcessStep = ucTemp;// = CurrentProcessStep;
		
  	PowerOnResetActionFlag = TRUE;
    //BELOW CODE IS TO CHECK FOR RESTART. AFTER RESTART STEP GOES TO IDLE
	//CurrentProcessStep = IDLE;
    GPRS_DTR_LOW;
    GPRS_PWR_KEY_HIGH;
	//CommunicationMediaPrefered = COMM_MEDIA_ETHERNET;	
    //NTC:21.09.01 23:36 UPON POWER FAILURE PAE COMM READ/WRITE ERROR POPS UP SO MAY BE DELAY IS REQUIRED.
    DelayInSec(1);
    //NTC: 21.09.01 23:36 ENDS
    while(1)
  	{	//DelayInMilliSec(1000);
    
        #ifdef WATCH_DOG_TIMER_ENABLED
            RESET_WATCH_DOG_TIMER;
        #endif
        
        if(SerialPortCommandFlag == TRUE)
        {   PutStringUsb(SerialPortBuff);
            SerialPortCommandFlag = FALSE;
        }
        
		CheckUsbStage();
		if(UsbCommandFlag == TRUE) {
            UsbCommandFlag = FALSE;
			//PutStringUsb("\r\n{");
			//PutStringUsb(UsbBuff);
			CommandFormat.Sender = SENDER_USB;
			//cFunRetVal = ExecutePcCommand(UsbBuff);
			Command(UsbReceiveBuffer);
            MobileStageWaitSec = 0;
        }
        
		#ifdef GSM_FUNCTION_ENABLE
			MobileRoutine();
		#endif
		//data analysis starts
		if(Fota.Status == FIRMWARE_NOT_AVAILABLE)
		{	BOOTLOADER_LED_OFF;
			//DelayInMilliSec(400);
			if(FreeRunSec % 2 == 0)
				MAIN_PROG_LED_ON;
			else
				MAIN_PROG_LED_OFF;
			
			CheckKeyPress();
			GetTemperature();//board temperature
			ScanInput();//FOR HARDWARE BYPASS FUNTION TO WORK THIS LINE BELOW MUST BE DEACTIVATE
			//GetAtlasReplacementTemperatureReading();
            //GetAtlasReplacementConductivityReading();
            
            if(SerialNoValidFlag == TRUE)
			{	if(ProcessStepRunSec % LOG_REC_SEC == 0 && ProcessStepRunSec > 0 && ActivateNeedToLogFlag == TRUE)
                {	NeedToLogFlag = TRUE;
                    ActivateNeedToLogFlag = FALSE;
                }
                if(ProcessStepRunSec % LOG_REC_SEC == 1)
                   ActivateNeedToLogFlag = TRUE;
                
                ReadPowerSolveModuleOutputVoltage();
                
                if(Mode == MODE_AUTO || Mode == MODE_MANUAL_CIP)
                {	AutoModeOperations();
                    
				}
				else
				{	Ppg_SetDuty(5,0);
                    #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                        //BypassOperationStopCounter = 0;//SHOULD BE DEACTIVE FOR HFIV_BYPASS TO BE OPERATED IN OTHER MODE TOO
                        //BypassOperationStartCounter = 0;//SHOULD BE DEACTIVE FOR HFIV_BYPASS TO BE OPERATED IN OTHER MODE TOO
                        BypassOperation2AlarmSecCounterFlag = FALSE;
                        Bypass2AlarmSecCounter =0;
                    #endif
				}
			}
			else
			{	if(FreeRunSec % 2 == 0)
				{	BOOTLOADER_LED_ON;
					MAIN_PROG_LED_ON;
					ReadSerialInfoInCopyBuff();
					if(CopyBuff[0] == 0)
					{   SerialNoValidFlag = TRUE;
						if(Mode == MODE_AUTO)
							DisplayPage=AUTO_0;
					}
				}
				else
				{	BOOTLOADER_LED_OFF;
					MAIN_PROG_LED_OFF;
				}
			}
			/*WDT TEST 1: SEE README DESCRIPTION */
            CheckAndInsertDailyRecord();//this code should be above LogRecord
			/*
			if(NeedToLogFlag == TRUE)
			{	LogRecord();
                PeriodicSave();
            }
			if(MinRecCounter4PeriodicSave == MIN_REC_COUNTER_4_PERIODIC_SAVE)
			  PeriodicSave();
            */
			/**/
            
            PAE_OnOffRelayControl();
            BypassOperation();//this should be just before UpdateOutput function
        }
		else
		{	DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
            UpdateOutput();
        
			if((FreeRunSec) > 3600)//1 hours
			{	//discard fota and resume to run in prev code with reinitializing data pointers
			  	Fota.Status = FIRMWARE_NOT_AVAILABLE;
				RandomMemWrite(FOTA_STATUSa,&Fota.Status,FOTA_STATUSn);
                LogPtr = SECOND_LOG_START_ADD;
				UploadPtr = LogPtr;
				EmailPtr = LogPtr;
				Server.Sr = SERVER_PROFILE_CONTROL_DISABLE;
				Server.Sr |= SERVER_PROFILE_NOT_RESPONDING;
				RandomMemWrite(SERVER_START_ADDRESS,&Server.Sr,ST_SRn);
					
				PeriodicSave();
			}
			else
			{	if((FreeRunSec % 2) == 0)
				{	BOOTLOADER_LED_ON;
					MAIN_PROG_LED_OFF;
				}
				else
				{	BOOTLOADER_LED_OFF;
					MAIN_PROG_LED_ON;
				}
			}
		}
		
		
        if(SecondChangeFlag == TRUE)
		{	SecondChangeFlag = FALSE;
			DisplayMenu();
            //sprintf(CopyBuff,"\r\WaitTimeB4BypassAlarmInSecCounter=%d",WaitTimeB4BypassAlarmInSecCounter);
            //PutStringUsb(CopyBuff);
            /*
            strcpy(PasteBuff,Gmt2LocalTimeStamp(Device.TimeStamp));
            sprintf(CopyBuff,"\r\nCurrentProcessStep=%d, ProcessStepRunSec=%d, ",CurrentProcessStep,ProcessStepRunSec);
            strcat(CopyBuff,PasteBuff);
            PutStringUsb(CopyBuff);
            */
        }
        
        UpdateOutput();
        PAE_OperationAsPerModeAndProcessStep();
		PowerOnResetActionFlag = FALSE; //ALL JOBS THAT SHOULD BE DONE AFTER POWER ON RESET HAS BEEN DONE
		if(NeedToLogFlag == TRUE)
        {	LogRecord();
            PeriodicSave();
        }
        //if(MinRecCounter4PeriodicSave == MIN_REC_COUNTER_4_PERIODIC_SAVE)
        //  PeriodicSave();
	}
}
void BypassOperation()
{   //Pressure Operation
    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
        if(AnaPressureAval >= -.5)//PRESSURE SENSOR IS PRESENT AND CONNECTED. -0.5 because at high altitude atmospheric will be less than 0
        {   if(AnaPressureAval <= (float)AnalogPressureBypassLevel/100)//<=2.0
            {   if(AnalogPressureBypassLevel > 0 && WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec)
                    RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_OCCURED);
            }
            else
            {   RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED); 
                WaitTimeB4BypassAlarmInSecCounter = 0;
            }
            
            if(AnaPressureAval <= (float)AnalogPressureStartLevel/100)//<=4.0
            {   if(TankFullFlag != FALSE)
                {   TankFullFlag = FALSE;
                    //SAVE IT TO MEMORY
                    u.c[0] = FALSE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
                if(CurrentProcessStep == WAIT)  
                {   if(FirstWasteCycle == FALSE)
                      CurrentProcessStep = PREPURIFY;
                    else
                      CurrentProcessStep = WASTE;
                    ResetAverageFlow();
                    ProcessStepRunSec = 0;
                }
            }
            else if(AnaPressureAval >= (float)AnalogPressureStopLevel/100)//>=4.8
            {   if(TankFullFlag != TRUE)
                {   TankFullFlag = TRUE;
                    //SAVE IT TO MEMORY
                    u.c[0] = TRUE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
                #if(PRODUCT_TYPE==PRODUCT_CFO)
                    if(CurrentProcessStep == PREPURIFY || CurrentProcessStep == PURIFY || CurrentProcessStep == WASTE || CurrentProcessStep == HF_WASTE)
                    {   FlushShuntPendingFlag = TRUE; 
                        CurrentProcessStep = WASTE;//SHUNT;
                        ProcessStepRunSec =0;
                    }
                    Ppg_SetDuty(5,0);
                #else//PRODUCT_TYPE==PRODUCT_DIUSE
                    if(CurrentProcessStep == PURIFY)
                    {   FlushShuntPendingFlag = TRUE; 
                        CurrentProcessStep = WASTE;//SHUNT;
                        ProcessStepRunSec =0;
                        //Ppg_SetDuty(5,0);
                    }
                #endif
            }  
            else
            {   if(TankFullFlag == FALSE)
                {
                }
                else
                {   switch(CurrentProcessStep)
                    {   case PREPURIFY:
                        case PURIFY:
                        //case WASTE:       //sebastian asked to remove the case in dientry so removed from diuse too
                        //case HF_WASTE:    //sebastian asked to remove the case in dientry so removed from diuse too
                        
                                        CurrentProcessStep = WAIT;
                                        ProcessStepRunSec =0;
                                        //Ppg_SetDuty(5,0);//this code moved within WAIT step. NTC 22.08.08 23:45
                                        break;
                    }
                    /*
                    if(CurrentProcessStep != SHUNT && CurrentProcessStep != WAIT && CurrentProcessStep != WAIT_BEFORE_CIP_START)
                    {   CurrentProcessStep = WAIT;
                        ProcessStepRunSec =0;
                        Ppg_SetDuty(5,0);
                    }*/
                }
            }
        }
        else//pressure sensor not installed
        {   /*
            if((CurDigitalInput & IP_BYPASS_SIG) > 0)
            {   if(WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec)
                    RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_OCCURED);
            }
            else
            {   RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED); 
                WaitTimeB4BypassAlarmInSecCounter = 0;
            }
            */
            if((CurDigitalInput & IP_START_SWT) > 0) //input is high
            {   if(TankFullFlag != FALSE)
                {   TankFullFlag = FALSE;
                    //SAVE IT TO MEMORY
                    u.c[0] = FALSE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
                if(CurrentProcessStep == WAIT)  
                {   if(FirstWasteCycle == FALSE)
                      CurrentProcessStep = PREPURIFY;
                    else
                      CurrentProcessStep = WASTE;
                    ResetAverageFlow();
                    ProcessStepRunSec = 0;
                }
            }
            else //input is low
            {   if(TankFullFlag != TRUE)
                {   TankFullFlag = TRUE;
                    //SAVE IT TO MEMORY
                    u.c[0] = TRUE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
                #if(PRODUCT_TYPE==PRODUCT_CFO)
                    if(CurrentProcessStep == PREPURIFY || CurrentProcessStep == PURIFY || CurrentProcessStep == WASTE || CurrentProcessStep == HF_WASTE)
                    {   FlushShuntPendingFlag = TRUE; 
                        CurrentProcessStep = WASTE;//SHUNT;
                        ProcessStepRunSec =0;
                    }
                    Ppg_SetDuty(5,0);
                #else//PRODUCT_TYPE==PRODUCT_DIUSE
                    if(CurrentProcessStep == PURIFY)
                    {   FlushShuntPendingFlag = TRUE; 
                        CurrentProcessStep = WASTE;//SHUNT;
                        ProcessStepRunSec =0;
                        //Ppg_SetDuty(5,0);
                    }
                #endif
            }
            if(TankFullFlag == FALSE)
            {
            }
            else
            {   switch(CurrentProcessStep)
                {   case PREPURIFY:
                    case PURIFY:
                    //case WASTE:       //sebastian asked to remove the case in dientry so removed from diuse too
                    //case HF_WASTE:    //sebastian asked to remove the case in dientry so removed from diuse too
                    
                                    CurrentProcessStep = WAIT;
                                    ProcessStepRunSec =0;
                                    //Ppg_SetDuty(5,0);//this code moved within WAIT step. NTC 22.08.08 23:45
                                    break;
                }
                /*
                if(CurrentProcessStep != SHUNT && CurrentProcessStep != WAIT && CurrentProcessStep != WAIT_BEFORE_CIP_START)
                {   CurrentProcessStep = WAIT;
                    ProcessStepRunSec =0;
                    Ppg_SetDuty(5,0);
                }*/
            }
        }
    #else //for product other than DiUse
        if(AnaPressureAval >= -0.5)//if pressure sensor is installed
        {   //PutStringUsb("\r\n####PRESSURE > -0.5####");
            //DelayInMilliSec(1000);
            if(AnaPressureAval <= (float)AnalogPressureBypassLevel/100)//3.5
            {   //PutStringUsb("\r\n####PRESSURE <= 3.5 BYPASS NC ON, NO OFF ####");
                DigitalOutput |= (1<<OUTPUT_BYPASS);	//OUTPUT_HFIV Off i.e NO TYPE valve will be open
            }
            else
            {   //PutStringUsb("\r\n####PRESSURE > 3.5 BYPASS NC OFF, NO ON ####");
                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV On i.e NO TYPE valve will be close
                RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED);
            }
            
            if(AnaPressureAval <= (float)AnalogPressureStartLevel/100)
            {   //PutStringUsb("\r\n####PRESSURE <= 4.0 ####");
                if(CurrentProcessStep == WAIT || CurrentProcessStep == SHUNT)//During IDLE and SHUNT go directly to PREPURE ->sebastian 28.06.2024
                {   StopFlag = FALSE;
                    if(FirstWasteCycle == FALSE)
                      CurrentProcessStep = PREPURIFY;
                    else
                      CurrentProcessStep = WASTE;
                    ResetAverageFlow();
                    ProcessStepRunSec =0;
                }
                if(TankFullFlag != FALSE)
                {   TankFullFlag = FALSE;
                    //SAVE IT TO MEMORY
                    u.c[0] = FALSE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
            }
            else if(AnaPressureAval >= (float)AnalogPressureStopLevel/100)
            {   //PutStringUsb("\r\n####PRESSURE >= 4.8 ####");
                RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED);
                
                if(TankFullFlag != TRUE)
                {   TankFullFlag = TRUE;
                    //SAVE IT TO MEMORY
                    u.c[0] = TRUE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
                if(CurrentProcessStep == PURIFY)
                {   FlushShuntPendingFlag = TRUE; 
                    CurrentProcessStep = WASTE;//SHUNT;
                    ProcessStepRunSec =0;
                    //Ppg_SetDuty(5,0);
                }
            }
            else
            {   //PutStringUsb("\r\n####3.7 > PRESSURE < 4.8####");
                if(TankFullFlag == FALSE)
                {
                }
                else
                {   switch(CurrentProcessStep)
                    {   case PREPURIFY:
                        case PURIFY:
                        //case WASTE:       //sebastian asked to remove the case
                        //case HF_WASTE:    //sebastian asked to remove the case
                                        CurrentProcessStep = WAIT;
                                        ProcessStepRunSec =0;
                                        //Ppg_SetDuty(5,0);//this code moved within WAIT step. NTC 22.08.08 23:45
                                        break;
                        
                    }
                    /*if(CurrentProcessStep != SHUNT && CurrentProcessStep != WAIT)
                    {   CurrentProcessStep = WAIT;
                        ProcessStepRunSec =0;
                        Ppg_SetDuty(5,0);
                    }*/
                }
            }
        }
        else //else pressure sensor is not installed
        {   if((CurDigitalInput & IP_BYPASS_SIG) > 0)
            {   //PutStringUsb("\r\n####PRESSURE <= 3.5 BYPASS NC ON, NO OFF ####");
                DigitalOutput |= (1<<OUTPUT_BYPASS);	//OUTPUT_HFIV Off i.e NO TYPE valve will be open
            }
            else
            {   //PutStringUsb("\r\n####PRESSURE > 3.5 BYPASS NC OFF, NO ON ####");
                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV On i.e NO TYPE valve will be close
                RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED);
            }
            
            if((CurDigitalInput & IP_START_SWT) > 0) //input is high equivalent to start pressure
            {   //PutStringUsb("\r\n####PRESSURE <= 4.0 ####");
                if(CurrentProcessStep == WAIT || CurrentProcessStep == SHUNT)//During IDLE and SHUNT go directly to PREPURE ->sebastian 28.06.2024
                {   StopFlag = FALSE;
                    if(FirstWasteCycle == FALSE)
                      CurrentProcessStep = PREPURIFY;
                    else
                      CurrentProcessStep = WASTE;
                    ResetAverageFlow();
                    ProcessStepRunSec =0;
                }
                if(TankFullFlag != FALSE)
                {   TankFullFlag = FALSE;
                    //SAVE IT TO MEMORY
                    u.c[0] = FALSE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
            }
            else //input is low
            {   if(TankFullFlag != TRUE)
                {   TankFullFlag = TRUE;
                    //SAVE IT TO MEMORY
                    u.c[0] = TRUE;
                    RandomMemWrite(TANK_FULLa,u.c,TANK_FULLn);
                }
                if(CurrentProcessStep == PURIFY)
                {   FlushShuntPendingFlag = TRUE; 
                    CurrentProcessStep = WASTE;//SHUNT;
                    ProcessStepRunSec =0;
                    //Ppg_SetDuty(5,0);
                }
            }
            if(TankFullFlag == FALSE)
            {
            }
            else
            {   switch(CurrentProcessStep)
                {   case PREPURIFY:
                    case PURIFY:
                    //case WASTE:       //sebastian asked to remove the case
                    //case HF_WASTE:    //sebastian asked to remove the case
                                    CurrentProcessStep = WAIT;
                                    ProcessStepRunSec =0;
                                    //Ppg_SetDuty(5,0);//this code moved within WAIT step. NTC 22.08.08 23:45
                                    break;
                    
                }
                /*if(CurrentProcessStep != SHUNT && CurrentProcessStep != WAIT)
                {   CurrentProcessStep = WAIT;
                    ProcessStepRunSec =0;
                    Ppg_SetDuty(5,0);
                }*/
            }
        }
    #endif
}
              
	
void AutoModeOperations()
{	if((CurDigitalInput & IP_LEAKAGE_SIG) > 0 && CurrentProcessStep != IDLE)
	{	RegisterAlarm(LEAKAGE_ALARM_FLAG, ALARM_OCCURED);
		CurrentProcessStep = IDLE;
                DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
                ProcessStepRunSec = 0;
	}
        FlowAverageCalc();
        /* CIP_LOW_LEVEL_ALARM deactivated NTC 22.12.23 18:40
        if(CIP_PumpRunTimeInSec >= MAX_CIP_PumpRunTimeInSec)
                    RegisterAlarm(CIP_LOW_LEVEL_ALARM_FLAG, ALARM_OCCURED);
            */
	//RAM CORRUPT CHECK STARTS
	if(ProcessStepRunSec % 5 == 0){
		bTemp = FALSE;
		urc.i[0] = 0;
		switch(CurrentProcessStep){
			case PREPURIFY: DirectMemRead(PREPURIFY_TIME_IN_SECa,urc.c,PREPURIFY_TIME_IN_SECn);	urc.i[1] = PrepurifyTimeInSec; break;
			case PURIFY: 	DirectMemRead(PURIFY_TIME_IN_SECa,urc.c,PURIFY_TIME_IN_SECn);	        urc.i[1] = PurifyTimeInSec; 	break;
			case WASTE:	DirectMemRead(WASTE_TIME_IN_SECa,urc.c,WASTE_TIME_IN_SECn);	        urc.i[1] = WasteTimeInSec;	break;
			case HF_WASTE:	DirectMemRead(HF_WASTE_TIME_IN_SECa,urc.c,HF_WASTE_TIME_IN_SECn);	urc.i[1] = HF_WasteTimeInSec;	break;
			case CIP_DOSING_ON:	DirectMemRead(CIP_DOSING_ON_TIME_IN_SECa,urc.c,CIP_DOSING_ON_TIME_IN_SECn);	urc.i[1] = CIP_DosingOnTimeInSec;	break;
			case CIP_DOSING_OFF: DirectMemRead(CIP_DOSING_OFF_TIME_IN_SECa,urc.c,CIP_DOSING_OFF_TIME_IN_SECn);urc.i[1] = CIP_DosingOffTimeInSec;	break;
                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                        case CIP_PULSE_ON:	DirectMemRead(CIP_FLOW_PULSE_ON_TIME_IN_SECa,urc.c,CIP_FLOW_PULSE_ON_TIME_IN_SECn);	urc.i[1] = CIP_PulseOnTimeInSec;	break;
			case CIP_PULSE_OFF:	DirectMemRead(CIP_FLOW_PULSE_OFF_TIME_IN_SECa,urc.c,CIP_FLOW_PULSE_OFF_TIME_IN_SECn);	urc.i[1] = CIP_PulseOffTimeInSec;	break;
                        #endif
                        case CIP_FLUSH:		DirectMemRead(CIP_FLUSH_TIME_IN_SECa,urc.c,CIP_FLUSH_TIME_IN_SECn);	urc.i[1] = CIP_FlushTimeInSec;	break;
			//case WV_CHECK:		DirectMemRead(WV_CHECK_TIME_IN_SECa,urc.c,WV_CHECK_TIME_IN_SECn);	urc.i[1] = WV_CheckTimeInSec;	break;
			case WAIT_HT:		DirectMemRead(WAIT_HT_TIME_IN_SECa,urc.c,WAIT_HT_TIME_IN_SECn);	urc.i[1] = WaitHT_TimeInSec;	break;			
			default: urc.i[1] = urc.i[0];break; //so that it does not load the setting again even if no ram corrupt occurs.
		}
		if(urc.i[0] != urc.i[1]){
			bTemp = TRUE;
			switch(CurrentProcessStep){
				case PREPURIFY: 	PutStringUsb("\r\nRAM Corrupt found PREPURIFY"); 	break;
				case PURIFY: 		PutStringUsb("\r\nRAM Corrupt found PURIFY");	break;
				case WASTE:			PutStringUsb("\r\nRAM Corrupt found WASTE");	break;
				case HF_WASTE:		PutStringUsb("\r\nRAM Corrupt found HF_WASTE");	break;
				case CIP_DOSING_ON:	PutStringUsb("\r\nRAM Corrupt found CIP_DOSING_ON");		break;
				case CIP_DOSING_OFF: PutStringUsb("\r\nRAM Corrupt found CIP_DOSING_OFF");		break;
                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                case CIP_PULSE_ON:	PutStringUsb("\r\nRAM Corrupt found CIP_PULSE_ON");		break;
				case CIP_PULSE_OFF:	PutStringUsb("\r\nRAM Corrupt found CIP_PULSE_OFF");		break;
                #endif
                case CIP_FLUSH:		PutStringUsb("\r\nRAM Corrupt found CIP_FLUSH");		break;
				//case WV_CHECK:		PutStringUsb("\r\nRAM Corrupt found WV_CHECK");	break;
				case WAIT_HT:		PutStringUsb("\r\nRAM Corrupt found WAIT_HT");	break;			
			}
		}
		urc.i[0] = 0;
		switch(CurrentProcessStep){
			#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
            case CIP_DOSING_OFF: DirectMemRead(CIP_DOSING_TOTAL_TIME_IN_SECa,urc.c,CIP_DOSING_TOTAL_TIME_IN_SECn); urc.i[1] = CIP_DosingTotalTimeInSec; break;
            case CIP_PULSE_OFF: DirectMemRead(CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa,urc.c,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn); urc.i[1] = CIP_PulseTotalTimeInSec; break;
            #endif
            default: urc.i[1] = urc.i[0];break; //so that it does not load the setting again even if no ram corrupt occurs.
		}
		if(urc.i[0] != urc.i[1]){
			bTemp = TRUE;
			PutStringUsb("\r\nRAM Corrupt found CIPsection");
		}
		urc.i[0] = 0;
		DirectMemRead(LAST_CYCLE_COUNTER_FOR_CIPa,urc.c,LAST_CYCLE_COUNTER_FOR_CIPn);
		if(urc.i[0] != CycleCounter){
			bTemp = TRUE;
			PutStringUsb("\r\nRAM Corrupt found CycleCounter");
		}
		urc.i[0] = 0;
		DirectMemRead(MAX_CYCLE_RUNCOUNT_FOR_CIPa,urc.c,MAX_CYCLE_RUNCOUNT_FOR_CIPn);
		if(urc.i[0] != MaxCycleCountForCIP){
			bTemp = TRUE;
			PutStringUsb("\r\nRAM Corrupt found MaxCycleCountForCIP");
		}
		if(bTemp == TRUE){
		//	PutStringUsb("\r\nRAM Corrupt found");
		//	SoftwareResetProcessor();
			LoadSetting();
		}
	}		
	//RAM CORRUPT CHECK ENDS******************************************************
            
	DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
							
	switch(CurrentProcessStep)	
	{       default:
		case IDLE:			//DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
						//DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
						//IT WILL STAY IN IDLE UNTILL N UNLESS ENTER KEY IS PRESSED FROM HMI
                            Ppg_SetDuty(5,0);
                            DigitalOutput = PrevDigitalOutput;//for manual control from server while PCB in IDLE step. 
                            if(ProcessStepRunSec % 300 == 0 && BuzzerFlag == FALSE)
                            {   DO9_RELAY_ON;
                                DelayInMilliSec(25);
                                DO9_RELAY_OFF;
                                BuzzerFlag = TRUE;
                            }
                            else
                            {   if(ProcessStepRunSec % 300 > 0)
                                    BuzzerFlag = FALSE;
                            }
                            /*
                            if(ProcessStepRunSec % 300 == 0)
                                DigitalOutput &= ~(1<<OUTPUT_FREE); //relay on
                            else
                                DigitalOutput |= (1<<OUTPUT_FREE); //relay off
                            */
                            break;
		case PREPURIFY:		
                            DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                DigitalOutput &= ~(1<<OUTPUT_WOV2);	//OUTPUT_HFIV ON
                            #endif
                              //DigitalOutput |= (1<<OUTPUT_HFIV);	//OUTPUT_HFIV OFF
                              //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                              DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                              //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                              //DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
                              DigitalOutput &= ~(1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY ON
                              //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                              //RecalibrateTemperatureIfNeeded();
                            
                            if(ProcessStepRunSec >= PrepurifyTimeInSec)
			    {	CycleCounter++;
								//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = PURIFY;
                                CurrentCyclePrepurifyAvgFlow = AverageFlow;//for % recovery calculation
                                ResetAverageFlow();
				ProcessStepRunSec =0;
				AverageVoltagePureFlag = TRUE;
				AverageVoltagePure = CurrentPAEvolt;
								
				StartVolume = FlowMeter.TotalFlowAfterRestart;
				TotalPureVolumeB4CycleStart = TotalPureVolume;
                                #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                    LastCycleOptimizedOutputVoltageCode = 0;//START pump with low voltage/speed
                                #endif
								PeriodicSave();
							}
							break;
		case PURIFY:
                            DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                            #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV ON
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                DigitalOutput &= ~(1<<OUTPUT_WOV2);	//OUTPUT_HFIV ON
                            #endif
                            DigitalOutput &= ~(1<<OUTPUT_POV);	        //OUTPUT_POV ON
                            //DigitalOutput |= (1<<OUTPUT_WOV);	        //OUTPUT_WOV OFF
                            //DigitalOutput |= (1<<OUTPUT_CIP);	        //OUTPUT_CIP OFF
                            //DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
                            DigitalOutput &= ~(1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY On
                            //DigitalOutput |= (1<<OUTPUT_PAE);	        //OUTPUT_PAE Off
							
                            TotalPureVolume = TotalPureVolumeB4CycleStart + FlowMeter.TotalFlowAfterRestart - StartVolume;	
                            PureAnalogVoltageOut();   
                            
                            if((ProcessStepRunSec >= PurifyTimeInSec) || (ProcessStepRunSec > 5 && CurrentPAEvolt > SwitchPAEvoltPure))         {	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                
                                //*********flow related calculations start*********
                                CurrentCyclePurifyAvgFlow = AverageFlow;//for % recovery calculation
                                if(AverageFlow < (LowFlowPurifyAlarmSetpoint/10))
								{	
                                                                  RegisterAlarm(LOW_FLOW_PURIFY_ALARM_FLAG, ALARM_OCCURED);
								}
                                //else  //alarm gets clear only if user press enter button after system enters into idle hence this code is not required
								//	RegisterAlarm(LOW_FLOW_PURIFY_ALARM_FLAG, ALARM_RESOLVED);//clear even if there was an error
								
								//AverageFlowFlag = FALSE;
                                ResetAverageFlow();
                                EndVolume = FlowMeter.TotalFlowAfterRestart;
				PureVolumeOfBatchJustCompleted = (EndVolume - StartVolume);
				StartVolume = EndVolume;//for next step waste volume calculation
                                Ppg_SetDuty(5,0);
								//********flow related calculations ends***********
                                
                                CurrentProcessStep = SHUNT_POST_PURE;//WASTE;
				//WasteBypassHighFlag = TRUE;//before start of waste stag make both wasteBypass High/Low flag as true
				//WasteBypassLowFlag = TRUE;
				ProcessStepRunSec =0;
								
				u.c[0] = LastCycleOptimizedOutputVoltageCode;
				RandomMemWrite(LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEa,u.c,LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEn);
				AverageVoltagePureFlag = FALSE;
				//sprintf(CopyBuff,"\r\nAVERAGE Voltage over Pure part = %.2fV",(float)AverageVoltagePure/100);	//testing code
				//PutStringUsb(CopyBuff);	//testing code
				AverageVoltageWasteFlag = TRUE;
				AverageVoltageWaste = CurrentPAEvolt;
															
				LastCyclePureAverageConductivity = AverageConductivity;
                                AverageConductivityAccumulator = AverageConductivity;
                                AverageConductivityCounter = 1;
                                
				LastFiveCyclePureAverageConductivitySamples[4] = LastFiveCyclePureAverageConductivitySamples[3];
                                LastFiveCyclePureAverageConductivitySamples[3] = LastFiveCyclePureAverageConductivitySamples[2];
                                LastFiveCyclePureAverageConductivitySamples[2] = LastFiveCyclePureAverageConductivitySamples[1];
                                LastFiveCyclePureAverageConductivitySamples[1] = LastFiveCyclePureAverageConductivitySamples[0];
                                LastFiveCyclePureAverageConductivitySamples[0] = LastCyclePureAverageConductivity;
                                for(u.c[0]=0, u.c[1]=0, LastFiveCyclePureAverageConductivityAverage=0; u.c[0]<5; u.c[0]++)
                                {   if(LastFiveCyclePureAverageConductivitySamples[u.c[0]]>0)
                                    {   LastFiveCyclePureAverageConductivityAverage += LastFiveCyclePureAverageConductivitySamples[u.c[0]];
                                        u.c[1]++;
                                    }
                                }
                                LastFiveCyclePureAverageConductivityAverage /= u.c[1];//average of avaliable max 5 sample
								//testing code *
                                sprintf(CopyBuff,"\r\n LastFiveCyclePureAverageConductivitySamples[4] = %.2f\r\n LastFiveCyclePureAverageConductivitySamples[3] = %.2f\r\n LastFiveCyclePureAverageConductivitySamples[2] = %.2f\r\n LastFiveCyclePureAverageConductivitySamples[1] = %.2f\r\n LastFiveCyclePureAverageConductivitySamples[0] = %.2f",
                                        (float)LastFiveCyclePureAverageConductivitySamples[4]/100,	//testing code
                                        (float)LastFiveCyclePureAverageConductivitySamples[3]/100,	//testing code
                                        (float)LastFiveCyclePureAverageConductivitySamples[2]/100,	//testing code
                                        (float)LastFiveCyclePureAverageConductivitySamples[1]/100,	//testing code
                                        (float)LastFiveCyclePureAverageConductivitySamples[0]/100);	//testing code
								PutStringUsb(CopyBuff);	//testing code
								
                                /*
								//Set PAE power supply for Waste Step
								//PAE_WasteStepVoltageSetpoint is itself * 10 so for final sending to pae 8 by 10
								//OptimizedPAE_CurrentForWasteStep is itself * 100
								EnablePAEoutput(PAE_WasteStepVoltageSetpoint*10, OptimizedPAE_CurrentForWasteStep);//temp deactivated
								*/
								PeriodicSave();
								
				}
				break;
                case SHUNT_POST_PURE:
                            DigitalOutput &= ~(1<<OUTPUT_MIV);  //OUTPUT_MIV On
                            DigitalOutput &= ~(1<<OUTPUT_WOV);  //OUTPUT_WOV ON
                            #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                DigitalOutput &= ~(1<<OUTPUT_SHUNT);    //OUTPUT_SHUNT On
                                DO6_RELAY_ON;
                            #endif
                            if(ProcessStepRunSec >= ShuntPostPureStepTimeInSec){
                                CurrentProcessStep = WASTE;
                                ProcessStepRunSec = 0;
                                FlushShuntPendingFlag = FALSE;
                            }
                            break;
        /*
                case TRANSITION:    DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                            if(ProcessStepRunSec >= 1)
							{   CurrentProcessStep = WASTE;
								ProcessStepRunSec =0;
							}
                            break;
        */
                case WASTE: DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                            if(ProcessStepRunSec < 2)//turn other relay only after one sec
                              break;
                            //DigitalOutput |= (1<<OUTPUT_HFIV);//OUTPUT_HFIV OFF
                            //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                            DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                            //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                            DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                            //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY Off
                            //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            /*if(ProcessStepRunSec > WastePreShuntTimeInSec)
                            {   if(FirstWasteCycle == FALSE)
                                {   if(ProcessStepRunSec < (WasteTimeInSec - WastePostShuntTimeInSec))
                                    {   
                                        #if(CONTACTOR_TYPE==NEW_CONTACTOR)
                                            DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                        #endif
                                    }
                                }
                                else //its first waste cycle
                                {   if(ProcessStepRunSec < ((WasteTimeInSec/2) - WastePostShuntTimeInSec))    //half of waste
                                    {
                                        #if(CONTACTOR_TYPE==NEW_CONTACTOR)
                                            DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                        #endif
                                    }
                                }
                            }
                            */      
                            /*
                            if((CurDigitalInput & IP_BYPASS_SIG) > 0)
                                    WasteBypassLowFlag = FALSE; //since during the cycle Bypass Sig at waste stage found high for at least one time so its not low for entire duration
                            else
                                    WasteBypassHighFlag = FALSE; //since during the cycle Bypass Sig at waste stage found low for at least one time so its not high for entire duration
                            */
                                
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                if(ProcessStepRunSec < PrepurifyTimeInSec)
                                {   DigitalOutput &= ~(1<<OUTPUT_WOV2);	//OUTPUT_HFIV ON
                                    PureAnalogVoltageOut();   
                                }
                                else
                                {   DigitalOutput &= ~(1<<OUTPUT_POV2);	//OUTPUT_PAE On
							    }
                            #endif
                            
                            if(FirstWasteCycle == FALSE)
                            {   if((ProcessStepRunSec >= WasteTimeInSec) || 
                                   (ProcessStepRunSec >= PureStepRunSec * WasteTimeInSec/(PrepurifyTimeInSec + PurifyTimeInSec)))
                                {	PostWasteTask();
                                }
                                if((ProcessStepRunSec >= 5) && (CurrentPAEvolt > SwitchPAEvoltWaste))
                                {   //sprintf(CopyBuff,"\r\nSwitching due to PAE Volt Current (%d) > Switch (%d)",CurrentPAEvolt,SwitchPAEvoltWaste);
                                    //PutStringUsb(CopyBuff);
                                    PostWasteTask();
                                }
                            }
                            else
                            {   if((ProcessStepRunSec >= WasteTimeInSec/2) || (CurrentPAEvolt > SwitchPAEvoltWaste))
                                {	PostWasteTask();
                                }
                                if((ProcessStepRunSec >= 5) && (CurrentPAEvolt > SwitchPAEvoltWaste))
                                {   PostWasteTask();
                                }
                            } 
		break;
		case HF_WASTE:		DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
				#if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV ON
                                #if(CONTACTOR_TYPE==NEW_CONTACTOR)
                                    if(ProcessStepRunSec < HF_WasteShuntTimeInSec)
                                        DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                            #endif
                            //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                            DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
							//DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
							
                            //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY Off
							//DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
							//AverageFlowFlag = FALSE;
				if(ProcessStepRunSec >= HF_WasteTimeInSec)
							{	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                if(ZeroEcSecondCounter > 30)
                                {   ZeroEcSecondCounter = 0;
                                    DirectMemRead(EC_ZERO_CYCLE_COUNTa,u.c,EC_ZERO_CYCLE_COUNTn);//read zero ec cycle count
                                    u.c[0]++;
                                    RandomMemWrite(EC_ZERO_CYCLE_COUNTa,u.c,EC_ZERO_CYCLE_COUNTn);//increment and updated
                                    ucTemp = u.c[0];
                                    DirectMemRead(SP_EC_ZERO_CYCLEa,u.c,SP_EC_ZERO_CYCLEn);//read zero ec max cycle limit
                                    if(ucTemp >= u.c[0])//restart PCB
                                    {   CurrentProcessStep = IDLE;//PREPURIFY;
                                        //below code only if system going to IDLE
                                        DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
                                        PeriodicSave();
                                        LogRecord();
                                        //RESET_DEVICE;
                                    }
                                }
                                else
                                {   u.c[0] = 0;
                                    RandomMemWrite(EC_ZERO_CYCLE_COUNTa,u.c,EC_ZERO_CYCLE_COUNTn);//clear and updated
                                }
                                
				if(CabinetTemperature > CabinetHighTemperatureSetVal)
				{	CurrentProcessStep = HIGH_TEMP;
                                    ProcessStepRunSec = 0;
					RegisterAlarm(CABINET_TEMP_ALARM_FLAG, ALARM_OCCURED);//TRUE
				}
                                /*
                                #if(CVS_CIP==CVS_CIP_NO)    //if the system is CVS then cleaning process is decided only if system goes to wait step
								else if(CycleCounter >= MaxCycleCountForCIP + 55)
								{	CurrentProcessStep = CIP_DOSING_ON;
									CIP_DosingRunTimeInSec = 0;
								}
                                #endif*/
                                #if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_NO))
                                else if(CycleCounter >= MaxCycleCountForCIP)
				{   CurrentProcessStep = CIP_DOSING_ON;
					CIP_DosingRunTimeInSec = 0;
                                }
                                #endif
                                else{
				if(CurrentProcessStep == HF_WASTE)  //IF EVERYTHING IS OK THEN
                                    {   CurrentProcessStep = SHUNT_POST_WASTE;//PREPURIFY;
                                        ResetAverageFlow();
                                    }
                                }
								ProcessStepRunSec =0;
								//DisablePAEoutput();
								PeriodicSave();
				}
				break;
                case SHUNT_POST_WASTE:
                            DigitalOutput &= ~(1<<OUTPUT_MIV);  //OUTPUT_MIV On
                            DigitalOutput &= ~(1<<OUTPUT_WOV);  //OUTPUT_WOV ON
                            #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                DigitalOutput &= ~(1<<OUTPUT_SHUNT);    //OUTPUT_SHUNT On
                                DO6_RELAY_ON;
                            #endif
                            if(ProcessStepRunSec >= ShuntPostWasteStepTimeInSec){
                                CurrentProcessStep = PREPURIFY;
                                ProcessStepRunSec = 0;
                                FlushShuntPendingFlag = FALSE;
                            }
                            break;
                            
                case SHUNT:         DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV On
                                    //DigitalOutput &= ~(1<<OUTPUT_HFIV);	//OUTPUT_HFIV On
                                    //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                    DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                    //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                DigitalOutput &= ~(1<<OUTPUT_WOV2);	//OUTPUT_WOV ON
                            #endif
                            #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                DO6_RELAY_ON;
                            #endif
                            //DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
                            //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                            //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            if(ProcessStepRunSec >= ShuntSec){
                              CurrentProcessStep = WAIT;
                              ProcessStepRunSec = 0;
                              FlushShuntPendingFlag = FALSE;
                            }
                            break;
		case WAIT:			//DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
							//DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
							//DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            Ppg_SetDuty(5,0);
                            
                            if(((CurDigitalInput & IP_BYPASS_SIG) > 0) && ((CurDigitalInput & IP_START_SWT) > 0) && AnaPressureAval < -0.5){
                                //if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                StopFlag = FALSE;
                                CurrentProcessStep = WASTE;//PREPURIFY;
                                ResetAverageFlow();
                                ProcessStepRunSec =0;
                                #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                    WaitTimeB4BypassAlarmInSecCounter = 0;
                                #endif
                                /*
                                //Set PAE power supply for Pure Step
                                //PAE_PureStepVoltageSetpoint is itself * 10 so for final sending to pae 8 by 10
                                //OptimizedPAE_CurrentForPureStep is itself * 100
                                EnablePAEoutput(PAE_PureStepVoltageSetpoint*10, OptimizedPAE_CurrentForPureStep);//temp deactivated
                                */
                                PeriodicSave();
                            }
                            //more priority condition is below
                            #if(CVS_CIP==CVS_CIP_YES)
                                if(CycleCounter >= MaxCycleCountForCIP){
                                    CurrentProcessStep = WAIT_BEFORE_CIP_START;
                                    ProcessStepRunSec =0;
                                }
                            #endif
                            #if(CVS_CIP==CVS_CIP_NO)
                                bTemp = FALSE;//CIP SHOULD HAPPEN OR NOT
                                if(CycleCounter >= MaxCycleCountForCIP)
                                    bTemp = TRUE;
                                else if(CycleCounter >= CIP_LowerLimitCycle)
                                {   uiTemp = (CIP_FavorableTime/100)*60 + (CIP_FavorableTime%100);
                                    if(uiTemp > CIP_FavorableTimeGraceMin)
                                        u.i[0] = uiTemp - CIP_FavorableTimeGraceMin;
                                    else
                                        u.i[0] = 1440 + uiTemp - CIP_FavorableTimeGraceMin;
                                    
                                    u.i[1] = uiTemp + CIP_FavorableTimeGraceMin;
                                    if(u.i[1] >= 1440)
                                      u.i[1] %= 1440;
                                    
//                                    if(SecondChangeFlag == TRUE){
//                                        sprintf(PasteBuff,"\r\nu.i[0]=%d FTuiTemp=%d u.i[1]=%d",u.i[0], uiTemp, u.i[1]);
//                                        PutStringUsb(PasteBuff);
//                                    }
                                    _ttemp = TimeStamp2Sec(Device.TimeStamp);
                                    _ttemp += SecAfterTimeSync;
                                    TimeStampPtr = localtime(&_ttemp);
                                    TimeStampPtr->tm_year -= 100;
                                    TimeStampPtr->tm_mon += 1;  
                                    uiTemp = TimeStampPtr->tm_hour*60 + TimeStampPtr->tm_min;
                                    
//                                    if(SecondChangeFlag == TRUE){
//                                        sprintf(PasteBuff,"\r\nu.i[0]=%d uiTemp=%d u.i[1]=%d",u.i[0], uiTemp, u.i[1]);
//                                        PutStringUsb(PasteBuff);
//                                    }
                                    
                                    if(u.i[0] < u.i[1]){
                                        if(uiTemp >= u.i[0] && uiTemp <= u.i[1])
                                        {   bTemp = TRUE;
                                        }
                                    }
                                    else{//date change is there
                                        if(uiTemp >= u.i[0] && uiTemp <= 1440)
                                        {   bTemp = TRUE;
                                        }
                                        else if(uiTemp <= u.i[1])
                                        {   bTemp = TRUE;
                                        }
                                    }
                                }
                                if(bTemp == TRUE){
                                    CurrentProcessStep = CIP_DOSING_ON;
                                    CIP_DosingRunTimeInSec = 0;
                                }
                            #endif
                            break;
                case WAIT_BEFORE_CIP_START:
                            if(ProcessStepRunSec >= CIP_WaitTimeInSec){
                                //if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = CIP_DOSING_ON;
                                CIP_DosingRunTimeInSec = 0;
                                ProcessStepRunSec =0;
                                PeriodicSave();
                            }
                            else{
                                //is there a 3.7 bar start signal
                                if(((CurDigitalInput & IP_BYPASS_SIG) > 0) && ((CurDigitalInput & IP_START_SWT) > 0) && AnaPressureAval < -0.5){
                                	//if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    StopFlag = FALSE;
                                    CurrentProcessStep = PREPURIFY;
                                    ResetAverageFlow();
                                    ProcessStepRunSec =0;
                                    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        WaitTimeB4BypassAlarmInSecCounter = 0;
                                    #endif
                                    PeriodicSave();
                                }
                                else if(AnaPressureAval < (float)(((float)AnalogPressureStartLevel)/100)){
                                    //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    StopFlag = FALSE;
                                    CurrentProcessStep = PREPURIFY;
                                    ResetAverageFlow();
                                    ProcessStepRunSec =0;
                                    PeriodicSave();
                                }
                            }
                            break;
                case CIP_DOSING_ON:	
                            #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                //DigitalOutput |= (1<<OUTPUT_MIV);	//OUTPUT_MIV OFF
                                //DigitalOutput |= (1<<OUTPUT_HFIV);	//OUTPUT_HFIV OFF
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                DigitalOutput &= ~(1<<OUTPUT_CIP);	//OUTPUT_CIP ON
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                                //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                                //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                DigitalOutput &= ~(1<<OUTPUT_CIP);	//OUTPUT_CIP ON
                                DigitalOutput &= ~(1<<OUTPUT_WOV2);	
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                //DigitalOutput |= (1<<OUTPUT_MIV);	//OUTPUT_MIV OFF
                                //DigitalOutput |= (1<<OUTPUT_HFIV_BYPASS);	//OUTPUT_HFIV_BYPASS OFF
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                DigitalOutput &= ~(1<<OUTPUT_CIP);	//OUTPUT_CIP ON
                                //DigitalOutput &= ~(1<<OUTPUT_PVR);	//OUTPUT_PVR ON
                                //DigitalOutput |= (1<<OUTPUT_NVR);	//OUTPUT_NVR OFF
                                //DigitalOutput &= ~(1<<OUTPUT_PUMP);	//OUTPUT_PUMP On
                                //sprintf(CopyBuff,"\r\nCIP RunTime=%d TotalTime=%d",CIP_PumpRunTimeInSec,MAX_CIP_PumpRunTimeInSec);
                                //PutStringUsb(CopyBuff);
                                Ppg_SetDuty(5,0);//0% duty cycle for entire CIP process
				            #endif
                            /*PutStringUsb("\r\n_________________ CIP DOSING ON ________________");
							sprintf(CopyBuff,"\r\nCIP_DosingRunTimeInSec:CIP_DosingTotalTimeInSec=%d:%d, ProcessStepRunSec:CIP_DosingOnTimeInSec=%d:%d, CIP_PumpRunTimeInSec:TotalTime=%d:%d",
                                    CIP_DosingRunTimeInSec,CIP_DosingTotalTimeInSec,
                                    ProcessStepRunSec,CIP_DosingOnTimeInSec,
                                    CIP_PumpRunTimeInSec,MAX_CIP_PumpRunTimeInSec);
							PutStringUsb(CopyBuff);
                            DelayInMilliSec(1000);
                            */
                            
                              //start CIP Pulsed Total flow Timer
                            CIP_DosingTotalFlowTimerFlag = TRUE;
                            #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                if(CIP_DosingRunTimeInSec >=  CIP_DosingTotalTimeInSec){
                                    //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_PULSE_ON;
                                    CIP_DosingTotalFlowTimerFlag = FALSE;
                                    CIP_PulseRunTimeInSec = 0;
                                    ProcessStepRunSec = 0;
                                    PeriodicSave();
                                }
                                else if(ProcessStepRunSec >= CIP_DosingOnTimeInSec )
                                {	//if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_DOSING_OFF;
                                    ProcessStepRunSec =0;
                                    //sprintf(CopyBuff,"\r\n Total=%d, TotalRun=%d",CIP_DosingTotalTimeInSec,CIP_DosingRunTimeInSec);
                                    //PutStringUsb(CopyBuff);
                                    PeriodicSave();
                                }
                            #endif
                            
                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                if(ProcessStepRunSec >= CIP_DosingOnTimeInSec )
                                {	//if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_DOSING_OFF;
                                    ProcessStepRunSec =0;
                                    //sprintf(CopyBuff,"\r\n Total=%d, TotalRun=%d",CIP_DosingTotalTimeInSec,CIP_DosingRunTimeInSec);
                                    //PutStringUsb(CopyBuff);
                                    PeriodicSave();
                                }
                            #endif
                            
                            #if(CVS_CIP==CVS_CIP_YES)
                                if(AnaPressureAval < (float)((float)(AnalogPressureCipLevel)/100))
                                {   //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_FLUSH;
                                    ProcessStepRunSec =0;
                                    PeriodicSave();
                                }
                            #endif
							break;
		case CIP_DOSING_OFF:
                            #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV ON
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                                //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                                //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV ON
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                DigitalOutput &= ~(1<<OUTPUT_WOV2);	
                                //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                //DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                                //DigitalOutput &= ~(1<<OUTPUT_HFIV_BYPASS);	//OUTPUT_HFIV_BYPASS ON
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                //DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                                //DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                //DigitalOutput |= (1<<OUTPUT_NVR);	//OUTPUT_NVR OFF
                                DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                                Ppg_SetDuty(5,100);//50% duty cycle for entire CIP process
                            #endif
							//start CIP Pulsed Total flow Timer
							CIP_DosingTotalFlowTimerFlag = TRUE;
                            
                            #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                if(CIP_DosingRunTimeInSec >=  CIP_DosingTotalTimeInSec){
                                    //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_PULSE_ON;
                                    CIP_DosingTotalFlowTimerFlag = FALSE;
                                    CIP_PulsedTotalFlowTimerFlag = TRUE;
                                    CIP_PulseRunTimeInSec = 0;
                                    ProcessStepRunSec = 0;
                                    PeriodicSave();
                                }
                                else if(ProcessStepRunSec >= CIP_DosingOffTimeInSec )
                                {	//if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_DOSING_ON;
                                    ProcessStepRunSec =0;
                                    //sprintf(CopyBuff,"\r\n Total=%d, TotalRun=%d",CIP_DosingTotalTimeInSec,CIP_DosingRunTimeInSec);
                                    //PutStringUsb(CopyBuff);
                                    PeriodicSave();
                                }
                            #endif
                            
                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                if(ProcessStepRunSec >= CIP_DosingOffTimeInSec )
                                {	//if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_FLUSH;
                                    ProcessStepRunSec =0;
                                    //sprintf(CopyBuff,"\r\n Total=%d, TotalRun=%d",CIP_DosingTotalTimeInSec,CIP_DosingRunTimeInSec);
                                    //PutStringUsb(CopyBuff);
                                    PeriodicSave();
                                }                            
                            #endif
							
                            #if(CVS_CIP==CVS_CIP_YES)
                                if(AnaPressureAval < (float)((float)(AnalogPressureCipLevel)/100))
                                {   //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_FLUSH;
                                    ProcessStepRunSec =0;
                                    PeriodicSave();
                                }
                            #endif
							break;
                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                case CIP_PULSE_ON:	
                            DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV ON
                            DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV ON
                            //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                            DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                            //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                            #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                            #endif
                            //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                            //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            CIP_PulsedTotalFlowTimerFlag = TRUE;
							if(CIP_PulseRunTimeInSec >=CIP_PulseTotalTimeInSec){
								//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = CIP_FLUSH;
								ProcessStepRunSec =0;
								PeriodicSave();
							}
							else if(ProcessStepRunSec >= CIP_PulseOnTimeInSec)
							{	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = CIP_PULSE_OFF;
								ProcessStepRunSec =0;
								//sprintf(CopyBuff,"\r\n Total=%d, TotalRun=%d",CIP_PulseTotalTimeInSec,CIP_PulseRunTimeInSec);
								//PutStringUsb(CopyBuff);
								PeriodicSave();
							}
							#if(CVS_CIP==CVS_CIP_YES)
                                if(AnaPressureAval < (float)((float)(AnalogPressureCipLevel)/100))
                                {   //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_FLUSH;
                                    ProcessStepRunSec =0;
                                    PeriodicSave();
                                }
                            #endif
							break;
		case CIP_PULSE_OFF:	
                            //DigitalOutput |= (1<<OUTPUT_MIV);	//OUTPUT_MIV OFF
                            //DigitalOutput |= (1<<OUTPUT_HFIV);	//OUTPUT_HFIV OFF
                            //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                            DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                            //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                            #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                            #endif
                            //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                            //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            CIP_PulsedTotalFlowTimerFlag = TRUE;
							if(CIP_PulseRunTimeInSec >=CIP_PulseTotalTimeInSec){
								//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = CIP_FLUSH;
								ProcessStepRunSec =0;
								PeriodicSave();
							}
							else if(ProcessStepRunSec >= CIP_PulseOffTimeInSec)
							{	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = CIP_PULSE_ON;
                                ProcessStepRunSec =0;
								//sprintf(CopyBuff,"\r\n Total=%d, TotalRun=%d",CIP_PulseTotalTimeInSec,CIP_PulseRunTimeInSec);
								//PutStringUsb(CopyBuff);
								PeriodicSave();
							}
							#if(CVS_CIP==CVS_CIP_YES)
                                if(AnaPressureAval < (float)((float)(AnalogPressureCipLevel)/100))
                                {   //if end of step status is also to be logged then activate below LogRecord function
                                    //LogRecord();
                                    CurrentProcessStep = CIP_FLUSH;
                                    ProcessStepRunSec =0;
                                    PeriodicSave();
                                }
                            #endif
							break;	
                #endif
                case CIP_FLUSH:		
                            #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                                DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV On
                                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV On
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                                //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                                //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_CFO)
                                DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV On
                                DigitalOutput &= ~(1<<OUTPUT_BYPASS);	//OUTPUT_HFIV On
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                DigitalOutput &= ~(1<<OUTPUT_WOV2);
                                //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                                //DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
                                //DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                            #endif
                            #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV On
                                //DigitalOutput &= ~(1<<OUTPUT_HFIV_BYPASS);	//OUTPUT_HFIV_BYPASS On
                                //DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
                                DigitalOutput &= ~(1<<OUTPUT_WOV);	//OUTPUT_WOV ON
                                //DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
                                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                                    DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On
                                #endif
                                //DigitalOutput |= (1<<OUTPUT_NVR);	//OUTPUT_NVR OFF
                                //DigitalOutput &= ~(1<<OUTPUT_PUMP);	//OUTPUT_PUMP On
                                Ppg_SetDuty(5,0);//0% duty cycle for entire CIP process
                            #endif
							if(ProcessStepRunSec >= CIP_FlushTimeInSec )
							{	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                
                                //CurrentProcessStep = WV_CHECK;
								ResetAverageFlow();
                                
                                /////////// process of waste valve check starts///////////
                                CycleCounter = 0;
                                if(Mode == MODE_MANUAL_CIP)
                                {   Mode = MODE_LOGOUT;
                                    DisplayPage = LOGOUT_0;
                                    CurrentProcessStep = IDLE;
                                    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                        DigitalOutput |= (1<<OUTPUT_MIV);	//OUTPUT_MIV Off
                                        DigitalOutput |= (1<<OUTPUT_BYPASS);	//OUTPUT_HFIV Off
                                        DigitalOutput |= (1<<OUTPUT_WOV);	//OUTPUT_WOV Off
                                        DigitalOutput |= (1<<OUTPUT_SHUNT);	//OUTPUT_PVR Off
                                    #else
                                        DigitalOutput |= (1<<OUTPUT_MIV);	//OUTPUT_MIV Off
                                        DigitalOutput |= (1<<OUTPUT_WOV);	//OUTPUT_WOV OFF
                                        DigitalOutput |= (1<<OUTPUT_SHUNT);	//OUTPUT_PVR OFF                                  
                                    #endif
                                }
                                else if((CurDigitalInput & IP_START_SWT) > 0 && AnaPressureAval < -0.5)
                                {	CurrentProcessStep = PREPURIFY;
                                    ResetAverageFlow();
                                    RegisterAlarm(WASTE_VALUE_ALARM_FLAG, ALARM_RESOLVED);
                                }
                                else
                                {	CurrentProcessStep = WAIT;
                                    RegisterAlarm(WASTE_VALUE_ALARM_FLAG, ALARM_RESOLVED);
                                }
                                ProcessStepRunSec = 0;
								PeriodicSave();
                                /////////// process of waste valve check ends///////////
							}
							break;
        /* this step removed on D3a3
		case WV_CHECK:		DigitalOutput &= ~(1<<OUTPUT_MIV);	//OUTPUT_MIV On
							DigitalOutput &= ~(1<<OUTPUT_HFIV);	//OUTPUT_HFIV On
							//DigitalOutput |= (1<<OUTPUT_POV);	//OUTPUT_POV OFF
							//DigitalOutput |= (1<<OUTPUT_WOV);	//OUTPUT_WOV OFF
							//DigitalOutput |= (1<<OUTPUT_CIP);	//OUTPUT_CIP OFF
							//DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
							//DigitalOutput |= (1<<OUTPUT_POLARITY);	//OUTPUT_POLARITY OFF
							//DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
							//AverageFlowFlag = TRUE;
                            if(ProcessStepRunSec < 8)
                                ResetAverageFlow();
							//sprintf(CopyBuff,"\r\n-----------------------------\r\nCURRENT Average Flow in WV_CHECK step = %d\r\n-----------------------------",AverageFlow);
                            //PutStringUsb(CopyBuff);
							if(ProcessStepRunSec >= WV_CheckTimeInSec)
							{	CycleCounter = 0;
                                //sprintf(CopyBuff,"\r\n-----------------------------\r\nFINAL Average Flow in WV_CHECK step = %d\r\n-----------------------------",AverageFlow);
                                //PutStringUsb(CopyBuff);
								if(AverageFlow > 30)//i.e if AverageFlow is > 0.3 L/M
								{	CurrentProcessStep = IDLE;
									RegisterAlarm(WASTE_VALUE_ALARM_FLAG, ALARM_OCCURED);//TRUE
								}
								else if((CurDigitalInput & IP_START_SWT) > 0)
								{	CurrentProcessStep = PREPURIFY;
                                    ResetAverageFlow();
									RegisterAlarm(WASTE_VALUE_ALARM_FLAG, ALARM_RESOLVED);
								}
								else
								{	CurrentProcessStep = WAIT;
									RegisterAlarm(WASTE_VALUE_ALARM_FLAG, ALARM_RESOLVED);
								}
								ProcessStepRunSec =0;
								PeriodicSave();
                                //AverageFlowFlag = FALSE;
								ResetAverageFlow();
							}
							break;
        */
		case HIGH_TEMP:		//DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
							//DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
							//DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
							Ppg_SetDuty(5,0);
                            if(Temperature.i/10 < CabinetHighTemperatureSetVal){
							  	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                CurrentProcessStep = WAIT_HT;
								RegisterAlarm(CABINET_TEMP_ALARM_FLAG, ALARM_RESOLVED);
							  	ProcessStepRunSec =0;
								PeriodicSave();
							}
							break;	
		case WAIT_HT:		//DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
							//DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT ON
							//DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
							Ppg_SetDuty(5,0);
                            if(ProcessStepRunSec >= WaitHT_TimeInSec)
							{	//if end of step status is also to be logged then activate below LogRecord function
                                //LogRecord();
                                if((CurDigitalInput & IP_START_SWT) > 0)
								{	CurrentProcessStep = PREPURIFY;
                                    ResetAverageFlow();
								}
								else
								{	CurrentProcessStep = WAIT;
								}
								ProcessStepRunSec =0;
								PeriodicSave();
							}
							break;	
	}
}

void PostWasteTask()
{	//if end of step status is also to be logged then activate below LogRecord function
    //LogRecord();
    
    //*********flow related calculations start*********
    //since AverageFlow variable holds CurrentCycleWasteAvgFlow hence AverageFlow can be used for % recovery caluclation
    //CurrentCycleWasteAvgFlow = AverageFlow;//for % recovery calculation
    for(ucTemp=4;ucTemp>0;ucTemp--)
    {   //sprintf(CopyBuff,"\r\nPercentage Recovery cycle %d = %d",ucTemp+1,PercentageRecovery[ucTemp]);
        //PutStringUsb(CopyBuff);
        PercentageRecovery[ucTemp] = PercentageRecovery[ucTemp-1];
    }
    PercentageRecovery[0] = (uc)((CurrentCyclePurifyAvgFlow * PurifyTimeInSec * 100)/(CurrentCyclePrepurifyAvgFlow * PrepurifyTimeInSec + CurrentCyclePurifyAvgFlow * PurifyTimeInSec + AverageFlow * WasteTimeInSec));
    //calculate the average flow
    if( (AverageFlow < (LowFlowWasteAlarmSetpoint/10)) && ((LowFlowWasteAlarmSetpoint*10)>0) )
        RegisterAlarm(LOW_FLOW_WASTE_ALARM_FLAG, ALARM_OCCURED);//TRUE
    //else  //alarm gets clear only if user press enter button after system enters into idle hence this code is not required
    //  RegisterAlarm(LOW_FLOW_WASTE_ALARM_FLAG, ALARM_RESOLVED);//FALSE
    EndVolume = FlowMeter.TotalFlowAfterRestart;
    WasteVolumeOfBatchJustCompleted = (EndVolume - StartVolume);
    //********flow related calculations ends***********
    
    AverageVoltageWasteFlag = FALSE;
    //sprintf(CopyBuff,"\r\nAVERAGE Voltage over waste part = %.2fV",(float)AverageVoltageWaste/100);
    //PutStringUsb(CopyBuff);
    
    /*
    if(WasteBypassHighFlag == TRUE)
        RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_OCCURED);//for entire duration of waste Bypass sig remained High hence clear the alarm flag if present or not	
    if(WasteBypassLowFlag == TRUE)
        RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_RESOLVED);//for entire duration of waste Bypass sig remained low hence clear the alarm flag if present or not
    */
    if((AlarmRegister24Bit & (1<<LOW_FLOW_PURIFY_ALARM_FLAG)) > 0)//if low flow pure alarm is there goto wait
    {   CurrentProcessStep = IDLE;
        //below code only if system going to IDLE
        DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
    }
    else if((AlarmRegister24Bit & (1<<LOW_FLOW_WASTE_ALARM_FLAG)) > 0)//if low flow WASTE alarm is there goto wait
    {   CurrentProcessStep = IDLE;
        //below code only if system going to IDLE
        DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
    }
    else if(StopFlag == TRUE || FlushShuntPendingFlag == TRUE)
         CurrentProcessStep = SHUNT;
    else
    CurrentProcessStep = HF_WASTE;
    FirstWasteCycle = FALSE;
    ProcessStepRunSec = 0;
    PureStepRunSec = 0;
    /*NTC:10.02.2022 15:30 reverted back after guillermo whatsapp msg
    //calculating next cycle PAE current for pure step and waste step
    #if(PRODUCT_TYPE==PRODUCT_CFO)
        //calculate LastCyclePureAverageConductivity by considering pure and waste avg ec
        LastCyclePureAverageConductivity = (AverageConductivity + LastFiveCyclePureAverageConductivitySamples[0])/2;
        AverageConductivityAccumulator = AverageConductivity;
        AverageConductivityCounter = 1;
        LastFiveCyclePureAverageConductivitySamples[4] = LastFiveCyclePureAverageConductivitySamples[3];
        LastFiveCyclePureAverageConductivitySamples[3] = LastFiveCyclePureAverageConductivitySamples[2];
        LastFiveCyclePureAverageConductivitySamples[2] = LastFiveCyclePureAverageConductivitySamples[1];
        LastFiveCyclePureAverageConductivitySamples[1] = LastFiveCyclePureAverageConductivitySamples[0];
        LastFiveCyclePureAverageConductivitySamples[0] = LastCyclePureAverageConductivity;
        for(u.c[0]=0, u.c[1]=0, LastFiveCyclePureAverageConductivityAverage=0; u.c[0]<5; u.c[0]++)
        {   if(LastFiveCyclePureAverageConductivitySamples[u.c[0]]>0)
            {   LastFiveCyclePureAverageConductivityAverage += LastFiveCyclePureAverageConductivitySamples[u.c[0]];
                u.c[1]++;
            }
        }
        LastFiveCyclePureAverageConductivityAverage /= u.c[1];//average of avaliable max 5 sample
        //testing code *
        sprintf(CopyBuff,"\r\n LastFiveCycleAverageConductivitySamples[4] = %.2f\r\n LastFiveCycleAverageConductivitySamples[3] = %.2f\r\n LastFiveCycleAverageConductivitySamples[2] = %.2f\r\n LastFiveCycleAverageConductivitySamples[1] = %.2f\r\n LastFiveCycleWasteAverageConductivitySamples[0] = %.2f",
                (float)LastFiveCyclePureAverageConductivitySamples[4]/100,	//testing code
                (float)LastFiveCyclePureAverageConductivitySamples[3]/100,	//testing code
                (float)LastFiveCyclePureAverageConductivitySamples[2]/100,	//testing code
                (float)LastFiveCyclePureAverageConductivitySamples[1]/100,	//testing code
                (float)LastFiveCyclePureAverageConductivitySamples[0]/100);	//testing code
        PutStringUsb(CopyBuff);	//testing code
    #endif
    */    
    #ifdef DEBUG_PAE
        sprintf(PasteBuff,"\r\nCALCULATION FOR NEXT CYCLE CURRENT\r\n(a)OptimizedPAE_CurrentForPureStep=%d\r\n(b)DynaProportionalFactor=%d\r\n(c)LastCyclePureAverageConductivity=%d\r\n(d)TargetOutletConductivity=%d\r\n(e)CurrentProportionalFactor=%d\r\n(f)PrepurifyTimeInSec=%d\r\n(g)PurifyTimeInSec=%d\r\n(h)WasteTimeInSec=%d\r\n(i)HF_WasteTimeInSec=%d\r\nOptimizedPAE_CurrentForPrepureStep=%d",
                OptimizedPAE_CurrentForPureStep,
                DynaProportionalFactor,
                LastCyclePureAverageConductivity,
                TargetOutletConductivity,
                CurrentProportionalFactor,
                PrepurifyTimeInSec,
                PurifyTimeInSec,
                WasteTimeInSec,
                HF_WasteTimeInSec,
                OptimizedPAE_CurrentForPrepureStep);
        PutStringUsb(PasteBuff);
    #endif
        if (DynamicCapDIMode == DYNAMIC_CAPDI_MODE_ON)      {
          if(LastCyclePureAverageConductivity > 0){
          OptimizedPAE_CurrentForPureStep = (int)(DynaProportionalFactor * ((float)((float)LastCyclePureAverageConductivity/100) - TargetOutletConductivity) + OptimizedPAE_CurrentForPureStep);
          OptimizedPAE_CurrentForWasteStep = WASTE_MAX_CURRENT;
          //OptimizedPAE_CurrentForWasteStep = (int)(((float)(CurrentProportionalFactor * (float)((float)OptimizedPAE_CurrentForPureStep/100) * (PrepurifyTimeInSec + PurifyTimeInSec))) / (WasteTimeInSec + HF_WasteTimeInSec));
          
          /*NTC:10.02.2022 15:30 reverted back after guillermo whatsapp msg
          #if(PRODUCT_TYPE==PRODUCT_CFO)
              OptimizedPAE_CurrentForWasteStep = OptimizedPAE_CurrentForPureStep;
          #endif
          */
          #if(PRODUCT_TYPE==PRODUCT_DISERIES)
              OptimizedPAE_CurrentForPureStep *= 0.27272727272727272727272727272727; //(3/11)
              OptimizedPAE_CurrentForWasteStep *= 0.27272727272727272727272727272727; //(3/11)
          #endif
            if((OptimizedPAE_CurrentForPureStep < 0) || (OptimizedPAE_CurrentForWasteStep < 0)){
                OptimizedPAE_CurrentForPureStep = 0;
                OptimizedPAE_CurrentForWasteStep = 0;
            }
          }      
        }
        else     {
          OptimizedPAE_CurrentForPrepureStep = PrePureCurrentSetpoint;
          
          OptimizedPAE_CurrentForPureStep  = PureCurrentSetpoint;
          OptimizedPAE_CurrentForWasteStep = WasteCurrentSetpoint;
        }
        #ifdef DEBUG_PAE
            #if (PRODUCT_TYPE == PRODUCT_DISERIES)
                sprintf(PasteBuff,"\r\nOptimized Current for Next: (j)Pure=(b*((c/100)-d)+a)*0.273=%.2f, Waste=(e*(j/100)*(f+g)/(h+i))=%.2f",(float)OptimizedPAE_CurrentForPureStep/100, (float)OptimizedPAE_CurrentForWasteStep/100);
                PutStringUsb(PasteBuff);
            #else
                sprintf(PasteBuff,"\r\nOptimized Current for Next: (j)Pure=b*((c/100)-d)+a=%.2f, Waste=(e*(j/100)*(f+g)/(h+i))=%.2f",(float)OptimizedPAE_CurrentForPureStep/100, (float)OptimizedPAE_CurrentForWasteStep/100);
                PutStringUsb(PasteBuff);
            #endif
        #endif
        
        if(OptimizedPAE_CurrentForWasteStep > WASTE_MAX_CURRENT)
        {	OptimizedPAE_CurrentForWasteStep = WASTE_MAX_CURRENT;
            OptimizedPAE_CurrentForPureStep = PURE_MAX_CURRENT;
            #ifdef DEBUG_PAE
                sprintf(PasteBuff,"\r\nWaste Current Exceeded the Limit so Limited to %d Amps hence Pure Current also Limited to %d Amps",WASTE_MAX_CURRENT,PURE_MAX_CURRENT);
                PutStringUsb(PasteBuff);//testing code
            #endif
        }
        else if(OptimizedPAE_CurrentForPureStep > PURE_MAX_CURRENT)
        {	OptimizedPAE_CurrentForPureStep = PURE_MAX_CURRENT;
            #ifdef DEBUG_PAE
                sprintf(PasteBuff,"\r\nPure Current Exceeded the Limit so Limited to %d Amps",PURE_MAX_CURRENT);
                PutStringUsb(PasteBuff);//testing code
            #endif
        }
        //Writing optimized PAE current for pure and waste stage to memory
        PeriodicSave();
}
