#include "gpio.h"
#include "mcu.h"
#include "mmu.h"
#include <stdio.h>
#include "variable.h"
#include "constants.c"
#include"AT45DB161.h"
#include"MyUsb.h"
#include <string.h>
#include "adc.h"
#include "delay.h"
#include "PAE800.h"
#include "AtlasReplacement.h"
#include "Atlas_C.h"

uc CurDigitalInput=0,PrevDigitalInput=0;
uint16_t DigitalOutput=0, PrevDigitalOutput=0;
boolean_t DigitalOutputSaveFlag = FALSE;
boolean_t DisableIgnitionFlag = FALSE;
struct input Input[MAX_NO_OF_INPUT];

void GpioInit(void)
{ 	//OUTPUT INTIALIZATION
  	P0_INIT;
	FM3_GPIO->ADE = 0X0000;
	P1_INIT;
	P2_INIT;
	P3_INIT;
	FM3_GPIO->SPSR &= 0XFE;//USE P46/X0A & P47/X1A as GPIO instad of sub clock (oscillation).
	P4_INIT;
	P5_INIT;
	P6_INIT;
	P8_INIT;
	
	//INPUT INITIALIZATION
	DIN3_PFR &= ~DIN3_MASK;	//Select CPIO function for DIN 
  	DIN3_PCR |= DIN3_MASK;	//Set DIN to HIGH
  	DIN3_DDR &= ~DIN3_MASK;	//Make DIN pins as input
	
	DIN5_PFR &= ~DIN5_MASK;	//Select CPIO function for DIN 
  	DIN5_PCR |= DIN5_MASK;	//Set DIN to HIGH
  	DIN5_DDR &= ~DIN5_MASK;	//Make DIN pins as input
	
}

void ScanInput()
{	/*static boolean_t vib_val = 0;//VIBRATION SENSOR IS BISTABLE TYPE SO REQUIRED TWO STATE IDENTIFICATION
	*/
    #define ANA_PRESSURE_MAX_NO_OF_AVG_SAMPLE 11
  
	static uc DI_ScanCounter = 0;
        static uc ScanDigitalInput[10];
        static uc DAC_FB_ScanCounter = 0;
	static ui DAC_FB_DCumVal = 0;
        static uc AnaPressureAvgScanCounter = 0;
        static uc AnaPressureMedianSampleCount = 0;
	static ui AnaPressureDCumVal = 0;
        static ui AnaPressureDvalArrayForMedian[ANA_PRESSURE_MAX_NO_OF_AVG_SAMPLE];
        ui AnaPressureFilteredDval;
    
	//PutStringUsb("\r\n%Inside ScanInput()\r\n");
    //below logic all input are mitual inclusive. if any one input is fluctuating then it will hamper all other input
    if(DI_ScanCounter%10==0)//once in ten scan
    {   ScanDigitalInput[DI_ScanCounter/10] = 0;
        if((DIN5_PDIR & DIGITAL_INPUT_1)== 0)
            ScanDigitalInput[DI_ScanCounter/10] |= IP_CIP_LVL;
        
        if((DIN3_PDIR & DIGITAL_INPUT_2) == 0)
            ScanDigitalInput[DI_ScanCounter/10] |= IP_BYPASS_SIG;
        
        if((DIN3_PDIR & DIGITAL_INPUT_3) == 0)
            ScanDigitalInput[DI_ScanCounter/10] |= IP_LEAKAGE_SIG;
        
        if((DIN3_PDIR & DIGITAL_INPUT_4) == 0)
            ScanDigitalInput[DI_ScanCounter/10] |= IP_START_SWT;
        
        if((DIN3_PDIR & DIGITAL_INPUT_5) == 0)
            ScanDigitalInput[DI_ScanCounter/10] |= IP_SPARE_SWT;
    }
    DI_ScanCounter++;
    if(DI_ScanCounter >= 100)//after each input stored for 10 times
    {   for(DI_ScanCounter=1;DI_ScanCounter<10;DI_ScanCounter++)
        {   if(ScanDigitalInput[0] != ScanDigitalInput[DI_ScanCounter])
                break;
        }
        if(DI_ScanCounter == 10)
        {   PrevDigitalInput = CurDigitalInput;
            CurDigitalInput = ScanDigitalInput[0];
            if(PrevDigitalInput != CurDigitalInput)
            {	NeedToLogFlag = TRUE;
                //sprintf(CopyBuff,"\r\n\r\nPrev=%d Cur=%d\r\n\r\n",PrevDigitalInput,CurDigitalInput);
                //PutStringUsb(CopyBuff);
            }
        }
        DI_ScanCounter = 0;
    }
    /*
    PrevDigitalInput = CurDigitalInput;
	CurDigitalInput = 0;
  	
	if((DIN5_PDIR & DIGITAL_INPUT_1)== 0)
	  	CurDigitalInput |= IP_CIP_LVL;
	
	if((DIN3_PDIR & DIGITAL_INPUT_2) == 0)
	  	CurDigitalInput |= IP_BYPASS_SIG;
	
	if((DIN3_PDIR & DIGITAL_INPUT_3) == 0)
	  	CurDigitalInput |= IP_LEAKAGE_SIG;
	
	if((DIN3_PDIR & DIGITAL_INPUT_4) == 0)
	  	CurDigitalInput |= IP_START_SWT;
    
	if((DIN3_PDIR & DIGITAL_INPUT_5) == 0)
	  	CurDigitalInput |= IP_SPARE_SWT;
    
	if(PrevDigitalInput != CurDigitalInput)
	{	NeedToLogFlag = TRUE;
        //sprintf(CopyBuff,"\r\n\r\nPrev=%d Cur=%d\r\n\r\n",PrevDigitalInput,CurDigitalInput);
		//PutStringUsb(CopyBuff);
	}
	*/

	//analog output as feedback input
	DAC_FB_Dval = Adc_Read(4);
	//sprintf(CopyBuff,"\r\n# %d samples = %d\r\n",DAC_FB_ScanCounter,DAC_FB_Dval);
	//PutStringUsb(CopyBuff);
	DAC_FB_DCumVal += DAC_FB_Dval;
	if(++DAC_FB_ScanCounter > 99)
	{	DAC_FB_Dval = DAC_FB_DCumVal/DAC_FB_ScanCounter;
        DAC_FB_Aval = (ui)(((float)DAC_FB_Dval)*.067);
		//sprintf(CopyBuff,"\r\n# 100 samples = %d\r\n",DAC_FB_Aval);
		//PutStringUsb(CopyBuff);
	  	DAC_FB_ScanCounter = 0;
		DAC_FB_DCumVal = 0;
	}		
    
    //pressure analog input
    AnaPressureDvalArrayForMedian[AnaPressureMedianSampleCount] = Adc_Read(9);
    AnaPressureMedianSampleCount++;
    if(AnaPressureMedianSampleCount == ANA_PRESSURE_MAX_NO_OF_AVG_SAMPLE)
    {   AnaPressureMedianSampleCount = 0;
        //arrange the sample into assending order
        for(cTemp=0;cTemp<ANA_PRESSURE_MAX_NO_OF_AVG_SAMPLE;cTemp++)
        {	for(ucTemp=cTemp+1;ucTemp<ANA_PRESSURE_MAX_NO_OF_AVG_SAMPLE;ucTemp++)
            {	if(AnaPressureDvalArrayForMedian[cTemp] > AnaPressureDvalArrayForMedian[ucTemp])
                {	AnaPressureDvalArrayForMedian[cTemp] += AnaPressureDvalArrayForMedian[ucTemp];
                        AnaPressureDvalArrayForMedian[ucTemp] = AnaPressureDvalArrayForMedian[cTemp] - AnaPressureDvalArrayForMedian[ucTemp];
                        AnaPressureDvalArrayForMedian[cTemp] -= AnaPressureDvalArrayForMedian[ucTemp];
                }
            }
        }
        AnaPressureDCumVal += AnaPressureDvalArrayForMedian[ANA_PRESSURE_MAX_NO_OF_AVG_SAMPLE/2];
        AnaPressureAvgScanCounter++;
        if(AnaPressureAvgScanCounter > 35)
        {   AnaPressureFilteredDval = AnaPressureDCumVal/AnaPressureAvgScanCounter;
            //for 6 bar sensor
            //AnaPressureAval = (float)(((float)((float)AnaPressureFilteredDval - 1500))/1000); //x=(y-C)/m
            //for 10 bar sensor
            //AnaPressureAval = ((float)AnaPressureFilteredDval - 1486)/601.6;
            //for universal converting mA to Physical value defined in GPIO.h file
            fTemp = (AnaPressureFilteredDval - 0.323)/377.6;//converted to mA as per my test instrument
            //Conversion from a physical value (Pv) to a current (I) formula:
            //Pv=((Pvhigh-Pvlow)/(Ihigh-Ilow))*(I-Ilow) + Pvlow
            AnaPressureAval = PHYSICAL_VALUE_HIGH - PHYSICAL_VALUE_LOW;
            AnaPressureAval /= 16;
            AnaPressureAval *= (fTemp - 4);
            AnaPressureAval += PHYSICAL_VALUE_LOW;
            AnaPressureAval += PressureOffset;
            //For universal converting ends here *
            
            // for formulation data recording in docklight*
            //sprintf(CopyBuff,"\r\nAnaPressureFilteredDval=%d, mA=%f, AnaPressureAval=%f\r\n",AnaPressureFilteredDval, fTemp, AnaPressureAval);
            //PutStringUsb(CopyBuff);
            AnaPressureAvgScanCounter = 0;
            AnaPressureDCumVal = 0;
        }
    }
    
	/*
	//if(CarBatteryAV < (Device.InputBatVoltCategory*0.5))//less than 50% TO PREVENT DATA LOSS
	if(CarBatteryAV < 4200)//(Device.InputBatVoltCategory*0.5))//less than 50% TO PREVENT DATA LOSS
	{	if(PeriodicMemValueChangeFlag == TRUE)
		{	PeriodicSave();
			PeriodicMemValueChangeFlag = FALSE;
		}
		CurDigitalInput |= EXTERNAL_SUPPLY;
	}
	*/
    GetAtlasReplacementReading();
    ReadAtlasConductivity();
}
////////////////////////////////////////////////////////////////////////////////
boolean_t CheckOutputStatus(char outputid)
{	if((DigitalOutput & (1<<outputid)) == 0)
		return ON;
	else
		return OFF;
}
////////////////////////////////////////////////////////////////////////////////
void SwitchOutput(char outputid, boolean_t on_off)
{	if(on_off == OFF)
		DigitalOutput |= (1<<outputid);
	else
		DigitalOutput &= (1<<outputid);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
char CallAction2Constant(char *str)
{	RemoveCharFromString(' ',str);
  	String2Upper(str);
	if(*str == NULL)
  		return -1;
	else if(strlen(str)==0)
	  	return -1;
	else if(strcmp(str,CALL_ACTION_STR_NONE)==0)
	  	return CALL_ACTION_NONE;
	else if(strcmp(str,CALL_ACTION_STR_TOGGLE_DO1)==0)
	  	return CALL_ACTION_TOGGLE_DO1;
	else if(strcmp(str,CALL_ACTION_STR_TOGGLE_DO2)==0)
	  	return CALL_ACTION_TOGGLE_DO2;
	else if(strcmp(str,CALL_ACTION_STR_TOGGLE_BOTH)==0)
	  	return CALL_ACTION_TOGGLE_BOTH;
	else if(strcmp(str,CALL_ACTION_STR_SMS_LOCATION)==0)
	  	return CALL_ACTION_SMS_LOCATION;
	else
	  	return -1;
}
////////////////////////////////////////////////////////////////////////////////
char* strcatCallAction(uc output_alloc_id)
{	switch(output_alloc_id)
	{	case CALL_ACTION_NONE: 				return CALL_ACTION_STR_NONE;
		case CALL_ACTION_TOGGLE_DO1:		return CALL_ACTION_STR_TOGGLE_DO1;
		case CALL_ACTION_TOGGLE_DO2:		return CALL_ACTION_STR_TOGGLE_DO2;
		case CALL_ACTION_TOGGLE_BOTH:		return CALL_ACTION_STR_TOGGLE_BOTH;
		case CALL_ACTION_SMS_LOCATION:		return CALL_ACTION_STR_SMS_LOCATION;
		default:							return "UNKNOWN";
	}
}
////////////////////////////////////////////////////////////////////////////////
/*
void UpdateOutputWithDelay()
{   static uint8_t outputno=0;
    do
    {   
        if(outputno == OUTPUT_POLARITY)
        {   if((PrevDigitalOutput & (1<<OUTPUT_POLARITY)) != (DigitalOutput & (1<<OUTPUT_POLARITY)))	//NEED TO SWITCH THE POLARITY //if(outputno == OUTPUT_POLARITY)	//NEED TO SWITCH THE POLARITY
            {   EnablePAEoutput(0, 0);//DISABLE OUTPUT BEFORE SWITCHING BY SHUNTING FOR DISCHARGING THE MODULE
                if(PAE_I2C_ErrorFlag == FALSE)
                {	
                    #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                        //OPERATE THE SHUNT RELAY
                        DO6_RELAY_ON;
                        DelayInMilliSec(250);
                        DO6_RELAY_OFF;
                        DigitalOutput |= (1<<OUTPUT_SHUNT);	
                    #endif  
                }
            }
        }
        
        if((DigitalOutput & (1<<outputno))==0)
        {	switch(outputno)
            {	case 0: DO1_RELAY_ON;break;
                case 1: 
                        #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                            DO2_RELAY_ON;
                        #else
                            switch(BypassOption)
                            {   case BYPASS_INTERNAL:
                                case BYPASS_EXTERNAL_NO:
                                    DO2_RELAY_ON;
                                break;
                                case BYPASS_EXTERNAL_NC:
                                    DO2_RELAY_OFF;
                                break;
                            }
                        #endif
                        if(WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec)
                        {   RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_OCCURED);
                        }
                        break;
                case 2: DO3_RELAY_ON;break;
                case 3: DO4_RELAY_ON;break;
                case 4: DO5_RELAY_ON;break;
                case 5: DO6_RELAY_ON;break;
                case 6: DO7_RELAY_ON;break;
                case 7: DO8_RELAY_ON;break;//THIS RELAY DECISION WILL BE TAKEN BY PAE800 FILE PAE_OperationAsPerModeAndProcessStep() FUNCTION
                case 8: DO9_RELAY_ON;break;
            }
        }
        else
        {	switch(outputno)
            {	case 0: DO1_RELAY_OFF;break;
                case 1: WaitTimeB4BypassAlarmInSecCounter = 0;
                        #if(PRODUCT_TYPE==PRODUCT_DIUSE)
                            DO2_RELAY_OFF;
                        #else
                            switch(BypassOption)
                            {   case BYPASS_INTERNAL:
                                case BYPASS_EXTERNAL_NO:
                                    DO2_RELAY_OFF;
                                break;
                                case BYPASS_EXTERNAL_NC:
                                    DO2_RELAY_ON;
                                break;
                            }
                        #endif
                        break;
                case 2: DO3_RELAY_OFF;break;
                case 3: DO4_RELAY_OFF;break;
                case 4: DO5_RELAY_OFF;break;
                case 5: DO6_RELAY_OFF;break;
                case 6: DO7_RELAY_OFF;break;
                case 7: DO8_RELAY_OFF;break;//THIS RELAY DECISION WILL BE TAKEN BY PAE800 FILE PAE_OperationAsPerModeAndProcessStep() FUNCTION
                case 8: DO9_RELAY_OFF;break;
            }
        }
        if((PrevDigitalOutput & (1<<outputno)) != (DigitalOutput & (1<<outputno)))	//NEED TO SWITCH
        {   //Update the Previous status to current status
            if((DigitalOutput & (1<<outputno)) == 0)	
                PrevDigitalOutput &= ~(1<<outputno);
            else
                PrevDigitalOutput |= (1<<outputno);
            DelayInMilliSec(500);//MINIMUM DELAY
		    break;
        }
        else
        {  outputno++;
            if(outputno >= MAX_NO_OF_OUTPUT)
            {   outputno = 0;
                break;
            }
        }
    }while(1);
}
*/
////////////////////////////////////////////////////////////////////////////////
void UpdateOutput()
{	/*
	if((PrevDigitalOutput ^ DigitalOutput) > 0){
		//StepsBeforeSwitchingPolarity();
	}
	*/
	if((PrevDigitalOutput & (1<<OUTPUT_POLARITY)) != (DigitalOutput & (1<<OUTPUT_POLARITY))){	//NEED TO SWITCH THE POLARITY
		PutStringUsb("\r\nPOLARITY SWITCH");
                EnablePAEoutput(0, 0);//DISABLE OUTPUT BEFORE SWITCHING BY SHUNTING FOR DISCHARGING THE MODULE
		if(PAE_I2C_ErrorFlag == FALSE)
		{	
                #if(CONTACTOR_TYPE==OLD_CONTACTOR)
                    //OPERATE THE SHUNT RELAY
                    DO6_RELAY_ON;
                    DelayInMilliSec(1000);
                    DO6_RELAY_OFF;
                    DigitalOutput |= (1<<OUTPUT_SHUNT);	
                #else
                    DO6_RELAY_OFF;
                    DO7_RELAY_OFF;
    //                //Deactivated for version T3.7
    //                #if(PRODUCT_TYPE!=PRODUCT_DIUSE)    //for DiUse 1 sec delay is not required
    //                    DelayInMilliSec(1000);
    //                #endif
                #endif 
                }
	}
	for(ucTemp=0;ucTemp<MAX_NO_OF_OUTPUT;ucTemp++)
	{	//DelayInMilliSec(1000);
		if((DigitalOutput & (1<<ucTemp))==0)
		{	switch(ucTemp)
			{	case 0: DO1_RELAY_ON;break;
				case 1: 
                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                            DO2_RELAY_ON;
                        #else
                            switch(BypassOption)
                            {   case BYPASS_INTERNAL:
                                case BYPASS_EXTERNAL_NO:
                                    DO2_RELAY_ON;
                                break;
                                case BYPASS_EXTERNAL_NC:
                                    DO2_RELAY_OFF;
                                break;
                            }
                            WaitTimeB4BypassAlarmInSecCounter = 0;
                        #endif
                        break;
				case 2: DO3_RELAY_ON;break;
				case 3: DO4_RELAY_ON;break;
				case 4: DO5_RELAY_ON;break;
				case 5: DO6_RELAY_ON;break;
				case 6: DO7_RELAY_ON;break;
				case 7: DO8_RELAY_ON;break;//THIS RELAY DECISION WILL BE TAKEN BY PAE800 FILE PAE_OperationAsPerModeAndProcessStep() FUNCTION
                case 8: DO9_RELAY_ON;break;
			}
		}
		else
		{	switch(ucTemp)
			{	case 0: DO1_RELAY_OFF;break;
				case 1: 
                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                            DO2_RELAY_OFF;
                        #else
                            switch(BypassOption)
                            {   case BYPASS_INTERNAL:
                                case BYPASS_EXTERNAL_NO:
                                    DO2_RELAY_OFF;
                                break;
                                case BYPASS_EXTERNAL_NC:
                                    DO2_RELAY_ON;
                                break;
                            }
                            if(AnalogPressureBypassLevel > 0 && WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec)
                            {   RegisterAlarm(BYPASS_ALARM_FLAG, ALARM_OCCURED);
                            }
                        #endif
                        
                        break;
				case 2: DO3_RELAY_OFF;break;
				case 3: DO4_RELAY_OFF;break;
				case 4: DO5_RELAY_OFF;break;
				case 5: DO6_RELAY_OFF;break;
				case 6: DO7_RELAY_OFF;break;
				case 7: DO8_RELAY_OFF;break;//THIS RELAY DECISION WILL BE TAKEN BY PAE800 FILE PAE_OperationAsPerModeAndProcessStep() FUNCTION
                case 8: DO9_RELAY_OFF;break;
            }
		}
	}
	
	if((PrevDigitalOutput ^ DigitalOutput) > 0){
		//DelayInMilliSec(100);//don't do anything for this time period whenever relay state change
        NeedToLogFlag = TRUE;
	}
	
	PrevDigitalOutput = DigitalOutput;
}
////////////////////////////////////////////////////////////////////////////////
