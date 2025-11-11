#include <time.h>
#include "DualTimer.h"
#include "board.h"
#include "mcu.h"
#include "gprs.h"
#include "variable.h"
#include "GprsUart.h"
#include"gpio.h"
#include"GpsUart.h"
#include"constants.c"
#include<stdio.h>
#include"1wire.h"
#include"DS18B20.h"
#include "PAE800.h"
#include "Atlas_T.h"
#include "AtlasReplacement.h"

const char day[7][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

time_t _ttemp;
struct tm *TimeStampPtr;
ui FreeRunSec=0;
//ui ServerAckTimeStamp = 0;
ui SecAfterTimeSync=0;
boolean_t SecondChangeFlag = FALSE;

void DT_Handler(void)
{   FM3_DTIM->TIMER1LOAD = TIMER_RELOAD_VAL;
	SecondChangeFlag = TRUE;
	PAEcommandFlag = TRUE;
	FreeRunSec++;
    StopTimer++;
    ZeroEcSecondCounter++;
    WaitTimeB4BypassAlarmInSecCounter++;
    AtlasReplacementCommTimeOutSec++;
    SecAfterTimeSync++;
	ProcessStepRunSec++;
	//if(FreeRunSec%900 == 0)//15 min
	//	GetMobileRtcTimeFlag = TRUE;
	//GprsPowerWaitSec++;
	//------------------
	MobileStageWaitSec++;
	MobileResponseWaitSec++;
    if(FlowmeterCommTimeOutSec < 250)
        FlowmeterCommTimeOutSec++;
	//------------------
	//if((FreeRunSec % 10) == 0)
	//  SmsCheckFlag = TRUE;
	if((FreeRunSec % 2) == 0)
		GetTemperatureFlag = TRUE;
	if(CIP_PulsedTotalFlowTimerFlag == TRUE)
		CIP_PulseRunTimeInSec++;
	if(CIP_DosingTotalFlowTimerFlag == TRUE)
		CIP_DosingRunTimeInSec++;
    if((CurrentProcessStep == PREPURIFY) || (CurrentProcessStep == PURIFY))
      PureStepRunSec++;
	else if(CurrentProcessStep == CIP_DOSING_ON)
		CIP_PumpRunTimeInSec++;
	if(((AlarmRegister24Bit & PAE_ALARMS) > 0 ) || PaeAlarm > 0){
		PaeErrorSecCounter++;
	}
	else
		PaeErrorSecCounter = 0;

    #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
    Bypass2AlarmSecCounter++;
    BypassOperationStopCounter++;
    BypassOperationStartCounter++;
    #endif
    /*
    if((FreeRunSec - ServerAckTimeStamp) > 300)
    {   if(FreeRunSec % 2 == 0)
           DO9_RELAY_ON;
        else
          DO9_RELAY_OFF;
        
    }
    */
    /*  
    //if(((Device.TimeStamp.tm_hour + (SecAfterTimeSync/3600))%24) == BalanceChkHour)
	if(((Device.TimeStamp.tm_hour + (SecAfterTimeSync/3600))%24) ==0)
	{  	if(BalanceCheckFlag == FALSE)
		{	BalanceCheckFlag = TRUE;
		}
	}
    */
	if(AtlasTemperatureErrorFlag == TRUE)
        AtlasTemperatureContinuousErrorSec++;
	//TimerInterruptCounter++;
    //TOGGLE LED ON CHARACTER RECEIVE
        /******* Method 1 using register*****************
        if((FM3_GPIO->PDOR3&LED2)==LED2) 
          FM3_GPIO->PDOR3 &= ~(LED2);
        else
          FM3_GPIO->PDOR3 |= (LED2);
        **************************************************/
    FM3_DTIM->TIMER1INTCLR = 0xFFFFFFFF;				/* Interrupt flg clear */
}
void InitDualTimer()
{   /*Set Timer 1 Load Value*/
    
 	FM3_DTIM->TIMER1INTCLR  = 0xFFFFFFFF;				/* Interrupt flg clear */
	FM3_DTIM->TIMER1CONTROL = 0x00000023;				/* bit7:TimerEn=0b0 DualTimer1 disable */
												/* bit5:IntEnable=0b1 count underflow interrupt */
												/* bit3,2:TimerPre=0b00 clock prescale(PCLK/1) */
												/* bit1:TimerSize=0b1 32bit */
												/* bit0:OneShot=0b1 */
	FM3_DTIM->TIMER1LOAD     = TIMER_RELOAD_VAL;
	FM3_DTIM->TIMER1CONTROL |= 0x00000080;				/* dual timer1 enable */
    /*Enable Dual Timer Interrup*/
	NVIC_SetPriority(DTIM_QDU_IRQn,0x00);
    NVIC_EnableIRQ(DTIM_QDU_IRQn);
}

time_t TimeStamp2Sec(struct tm t)
{	t.tm_year = 2000+t.tm_year-1900;
	t.tm_mon = t.tm_mon-1;
	t.tm_mday = t.tm_mday;
	t.tm_hour = t.tm_hour;
	t.tm_min = t.tm_min;
	t.tm_sec = t.tm_sec;
  	//change last valid gmt time to sec after 1970
  	return mktime(&t);
}

char* Gmt2LocalTimeStamp(struct tm t)
{	time_t totalsec;
	char buff[25];
	char *buffptr=buff;
	//change last valid gmt time to sec after 1970
	totalsec = TimeStamp2Sec(t);
	totalsec += SecAfterTimeSync;
  	//add timezone
	totalsec += Device.TimeZone;
	TimeStampPtr = localtime(&totalsec);
	sprintf(buff,"%s-%d/%d/%d %d:%d:%d",
			day[TimeStampPtr->tm_wday],
			(TimeStampPtr->tm_year) + 1900,
			TimeStampPtr->tm_mon + 1,
			TimeStampPtr->tm_mday,
			TimeStampPtr->tm_hour,
			TimeStampPtr->tm_min,
			TimeStampPtr->tm_sec);
	return buffptr;
}
