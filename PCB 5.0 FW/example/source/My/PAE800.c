#include "PAE800.h"
#include "PAE_I2C.h"
#include "Delay.h"
#include "gpio.h"
#include "variable.h"
#include "MyUsb.h"
#include "myfun.h"
#include "mmu.h"
#include "constants.c"
#include "DualTimer.h"
#include <stdio.h>

int CurrentPAEvolt = 0;
int CurrentPAEcurrent = 0;
int SwitchPAEvoltWaste = 0;
int SwitchPAEvoltPure = 0;
uc PaeErrorSecCounter = 0;
uc PaeAlarm = 0;
uc TempPaeError;
boolean_t PAEcommandFlag;
uc PAE_I2C_ErrorCounter = 0;

void DisablePAEoutput()
{	u.c[0] = 128;//disable output = 128
	WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x7C, u.c, 1);
}
void EnablePAEoutput(ui volt_multiplied_by_100, ui current_multiplied_by_100)
{	u.i[0] = volt_multiplied_by_100;
	WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x70, u.c, 2);
	DelayInMilliSec(10);
	u.i[0] = current_multiplied_by_100;
	WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x72, u.c, 2);
	DelayInMilliSec(10);
	u.c[0] = 133;//enable output = 133
	WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x7C, u.c, 1);	
}

void InitPAE800()
{	PAE_I2C_SCL_INIT_AS_OUTPUT_HIGH;
	PAE_I2C_SDA_INIT_AS_OUTPUT_HIGH;
//	DisablePAEoutput();
}
void PAE_OnOffRelayControl()
{ 
    //static boolean_t NeedPaeInitFlag = TRUE;
    static uc PaeFailCounter = 0;
    
	if(Mode == MODE_AUTO){
        switch(CurrentProcessStep)	
        {	case HF_WASTE:      //without error if the step goes to hf_waste then it can be consider that even if there was pae error in prev cycle that can be consider as resolved after flush+shunt
                                PaeErrorSecCounter = 0;
                                PaeFailCounter = 0;
                                break;
            case PREPURIFY:	
            case PURIFY:
            case WASTE:
                                #define PAE_ALARMS 0x00ff80
                                //if((AlarmRegister24Bit & PAE_ALARMS) > 0 ){
                                if(PaeAlarm > 0 ){
                                    if(PaeErrorSecCounter > 5) 
                                    {   DisablePAEoutput();
                                        PaeFailCounter++;
                                        if(PaeFailCounter > 3)
                                        {   CurrentProcessStep = IDLE;
                                            for(ucTemp=0;ucTemp<8;ucTemp++)
                                            {	if((PaeAlarm & (1<<ucTemp)) > 0)
                                                    RegisterAlarm(8+ucTemp, ALARM_OCCURED);	
                                                //else
                                                //    RegisterAlarm(8+ucTemp, ALARM_RESOLVED);
                                            }
                                            //below code only if system going to IDLE
                                            DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
                                        }
                                        else
                                        {   FlushShuntPendingFlag = TRUE; //CurrentProcessStep = SHUNT;
                                            //PutStringUsb("\r\n\nSHUNTING DUE TO PAE ERROR\r\n\n");
                                        }
                                        //ProcessStepRunSec = 0;
                                    }
                                }
                                /*else{
                                    
                                    DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                                    //DO8_RELAY_ON;
                                    if(NeedPaeInitFlag == TRUE){
                                        InitPAE800();
                                        NeedPaeInitFlag = FALSE;
                                    }
                                }*/
            break;
            case IDLE:          PaeErrorSecCounter = 0;
                                PaeFailCounter = 0;
                                break;
            /*
            case SHUNT:
                                 //below code may be executing only after SHUNT process is over and come back to prepurify
                                if((PaeErrorSecCounter%30) > 15)//reset pae
                                {	DigitalOutput |= (1<<OUTPUT_PAE);	//OUTPUT_PAE Off
                                    //DO8_RELAY_OFF;
                                    NeedPaeInitFlag = TRUE;
                                }
                                else
                                {	
                                    if(PaeErrorSecCounter > 75)
                                    {   //PutStringUsb("\r\nRESTART DUE TO PAE");//RESET TEST CODE
                                        SoftwareResetProcessor();
                                    }
                                    else{
                                        DigitalOutput &= ~(1<<OUTPUT_PAE);	//OUTPUT_PAE On
                                        //DO8_RELAY_ON;
                                        if(NeedPaeInitFlag == TRUE){
                                            InitPAE800();
                                            NeedPaeInitFlag = FALSE;
                                        }
                                    }
                                }
            break;*/
            /*
            default:	        DigitalOutput |= (1<<OUTPUT_PAE);	//OUTPUT_PAE Off
                                //DO8_RELAY_OFF;
                                NeedPaeInitFlag = TRUE;
            break;
            */
        }
    }
    /*
    else{
        DigitalOutput |= (1<<OUTPUT_PAE);	//OUTPUT_PAE Off
        //DO8_RELAY_OFF;
        NeedPaeInitFlag = TRUE;
    }
    */
}
void PAE_OperationAsPerModeAndProcessStep()
{	
	//PAE FUNCTIONALITY AS PER PROCESS STEP
	if(PowerOnResetActionFlag == TRUE || PAEcommandFlag == TRUE)
	{	PAEcommandFlag = FALSE;
        if(PowerOnResetActionFlag == TRUE)
        {   PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
            PAE_ForceI2CStop();
        }
		if(Mode == MODE_AUTO){
			switch(CurrentProcessStep)	
			{	case PREPURIFY:	    EnablePAEoutput(PAE_PrepurifyStepVoltageSetpoint*10, OptimizedPAE_CurrentForPrepureStep); break;
				case PURIFY:        EnablePAEoutput(PAE_PureStepVoltageSetpoint*10, OptimizedPAE_CurrentForPureStep);   break;
				case WASTE:         EnablePAEoutput(PAE_WasteStepVoltageSetpoint*10, OptimizedPAE_CurrentForWasteStep);
                                    break;
				case HF_WASTE:      
                                    #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
                                        EnablePAEoutput(PAE_WasteStepVoltageSetpoint*10, OptimizedPAE_CurrentForWasteStep);
                                    #else
                                        DisablePAEoutput();
                                    #endif
				break;
                //case SHUNT:         DisablePAEoutput();break;
				default:			DisablePAEoutput();break;
			}
		}
		else
			DisablePAEoutput();
	}
}
void StepsBeforeSwitchingPolarity()
{	
	#ifdef DEBUG_PAE
		ui start_time;
	#endif
	EnablePAEoutput(0, 0);
	#ifdef DEBUG_PAE
		start_time = FreeRunSec;
	#endif
	do
	{	
        #ifdef WATCH_DOG_TIMER_ENABLED
            RESET_WATCH_DOG_TIMER;
        #endif
		u.i[0] =0;
		ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x60, u.c,2);
		if(PAE_I2C_ErrorFlag == FALSE)
		{	
			#ifdef DEBUG_PAE
				sprintf(PasteBuff,"\r\nPAE Volt=%.2fV",(float)u.i[0]/100);
				PutStringUsb(PasteBuff);
			#endif
			u.i[1] = u.i[0];
		}
		u.i[0] =0;
		ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x62, u.c,2);
		if(PAE_I2C_ErrorFlag == FALSE)
		{	
			#ifdef DEBUG_PAE
				sprintf(PasteBuff,"\r\nPAE Current=%.2fA",(float)u.i[0]/100);
				PutStringUsb(PasteBuff);
			#endif
		}
	}while(u.i[0]>100 || u.i[1]>50);
	#ifdef DEBUG_PAE
		sprintf(PasteBuff,"\r\nSettling Time<=%dSec",FreeRunSec-start_time);
		PutStringUsb(PasteBuff);
	#endif
}
void ReadPowerSolveModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_read)
{	
#ifdef PAE_FUNCTION_ENABLE
	//start
	/*#ifdef DEBUG_I2C_ERROR
		PutStringUsb("\r\nPAE < Sending Read Command to PAE");
	#endif*/
	PAE_I2C_Start();
	
	if(PAE_I2C_ErrorFlag == TRUE)
	{	PAE_I2C_Stop();
		no_of_byte_to_read = 0;
		//Code Required For Testing
		//OutTextXY(0,1,"R0 ERROR");	 
		//while(1);
	}
	else
	{	PAE_I2C_Send(device_address);				//DEVICE ADDRESS
		if(PAE_I2C_ErrorFlag == TRUE)
		{	PAE_I2C_Stop();
			no_of_byte_to_read = 0;
			//Code Required For Testing
			//OutTextXY(0,1,"R1 ERROR");	 
			//while(1);
		}
		else
		{	PAE_I2C_Send(mem_start_add);
			if(PAE_I2C_ErrorFlag == TRUE)
			{	PAE_I2C_Stop();
				no_of_byte_to_read = 0;
				//Code Required For Testing
				//OutTextXY(0,1,"R3 ERROR");	
				//while(1);
			}
			else
			{	PAE_I2C_Stop();
				PAE_I2C_Start();
				if(PAE_I2C_ErrorFlag == TRUE)
				{	PAE_I2C_Stop();
					no_of_byte_to_read = 0;
					//Code Required For Testing
					//OutTextXY(0,1,"R4 ERROR");	
					//while(1);
				}
				else
				{	PAE_I2C_Send(device_address|0X01);				//DEVICE ADDRESS WITH READ COMMAND
					if(PAE_I2C_ErrorFlag == TRUE)
					{	PAE_I2C_Stop();
						no_of_byte_to_read = 0;
						// Code Required For Testing
						//OutTextXY(0,1,"R5 ERROR");	
						//while(1);
					}
					else
					{	while(no_of_byte_to_read)
						{	*buff = PAE_I2C_Get();
							buff++;
							mem_start_add++;
							if(no_of_byte_to_read == 1 || mem_start_add%131072 == 0)
							{	PAE_I2C_Nack();
								PAE_I2C_Stop();
								if(no_of_byte_to_read == 1)
									no_of_byte_to_read = 0;
							}
							else
							{	PAE_I2C_Ack();
								no_of_byte_to_read--;
							}
						}
					}
				}		
			}
		}
	}
	if(PAE_I2C_ErrorFlag == TRUE){
		PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_Stop(); 
        DelayInMilliSec(10);
        PAE_I2C_ErrorCounter++;
		if(PAE_I2C_ErrorCounter > 250){
			PAE_I2C_ErrorCounter = 0;
			RegisterAlarm(PAE_COMM_ERROR_FLAG, ALARM_OCCURED);
            //RESET_DEVICE;
		}
		#ifdef DEBUG_I2C_ERROR
			PutStringUsb("\r\nPAE > COMM. READ ERROR");
        #endif
	}
	else{
		RegisterAlarm(PAE_COMM_ERROR_FLAG, ALARM_RESOLVED);
		PAE_I2C_ErrorCounter = 0;
	}
#endif
}
/*###########################################################################################################*/
#define MAX_PAE_STATUS_REG_VAL_BUFFER 10
void ReadPowerSolveModuleOutputVoltage()
{	static int pae_volt_read_counter = 0;
	static uc pae_status_reg[MAX_PAE_STATUS_REG_VAL_BUFFER];
	static uc pae_status_reg_val_counter = 0;
	static int PAE_ReadSampleData[PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION];
	
	//READ STATUS REGISTER AND SHUTDOWN ALARM
	ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x6C, u.c,1);
    //u.c[0] = NabeenTestValue;//for testing purpose
    if(PAE_I2C_ErrorFlag == FALSE){
		pae_status_reg[pae_status_reg_val_counter % MAX_PAE_STATUS_REG_VAL_BUFFER] = u.c[0];
		pae_status_reg_val_counter++;
		if(pae_status_reg_val_counter>=MAX_PAE_STATUS_REG_VAL_BUFFER){
			for(pae_status_reg_val_counter=0;pae_status_reg_val_counter<MAX_PAE_STATUS_REG_VAL_BUFFER;pae_status_reg_val_counter++){
				if(u.c[0] == pae_status_reg[pae_status_reg_val_counter])
					continue;
				else
					break;
			}
			if(pae_status_reg_val_counter == MAX_PAE_STATUS_REG_VAL_BUFFER){
				PaeAlarm = u.c[0];
                
                for(ucTemp=0;ucTemp<8;ucTemp++)
				{	if((u.c[0] & (1<<ucTemp)) > 0)
                    {	//RegisterAlarm(8+ucTemp, ALARM_OCCURED);	//alarm will occur after 3 attemp of self resolve cycle
                      //PutCharUsb('#');
					}
                    //else  //IT HAS TO BE RESOLVED FROM THE LCD ONLY
					//	RegisterAlarm(8+ucTemp, ALARM_RESOLVED);
                }
                
			}
			pae_status_reg_val_counter = 0;
		}
	}
	//READ OUTPUT VOLTAGE
	u.i[0] = 0;
	ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x60, u.c,2);
	if(PAE_I2C_ErrorFlag == FALSE){
		PAE_ReadSampleData[pae_volt_read_counter] = u.i[0];
		
		if(++pae_volt_read_counter == PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION){
			pae_volt_read_counter = 0;
			//arrange the sample into assending order
			for(cTemp=0;cTemp<PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)
			{	for(ucTemp=cTemp+1;ucTemp<PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION;ucTemp++)
				{	if(PAE_ReadSampleData[cTemp] > PAE_ReadSampleData[ucTemp])
					{	PAE_ReadSampleData[cTemp] += PAE_ReadSampleData[ucTemp];
						PAE_ReadSampleData[ucTemp] = PAE_ReadSampleData[cTemp] - PAE_ReadSampleData[ucTemp];
						PAE_ReadSampleData[cTemp] -= PAE_ReadSampleData[ucTemp];
					}
				}
			}
			/*		
			for(cTemp=0;cTemp<PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)//testing code
			{	sprintf(CopyBuff,"\r\PAE_ReadSampleData[%d]=%d",cTemp,PAE_ReadSampleData[cTemp]);//testing code
				PutStringUsb(CopyBuff);//testing code
			}//testing code
			*/		
            iTemp = PAE_ReadSampleData[PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION/2];//GET THE MEDIAN VALUE. To get actual value, divide by 100
			if(iTemp >= 0 && iTemp <= 1200)
                CurrentPAEvolt = iTemp;
            else
            {   sprintf(CopyBuff,"\r\nPAE volt error = %.2fV",((float)iTemp/100));
                PutStringUsb(CopyBuff);
            }
			if(AverageVoltagePureFlag == TRUE)
			{	AverageVoltagePure += CurrentPAEvolt;
				AverageVoltagePure /= 2;
			}
			if(AverageVoltageWasteFlag == TRUE)
			{	AverageVoltageWaste += CurrentPAEvolt;
				AverageVoltageWaste /= 2;
			}
		}
	}
    //READ OUTPUT CURRENT
	u.i[0] = 0;
	ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x62, u.c,2);
	if(PAE_I2C_ErrorFlag == FALSE)
    {   CurrentPAEcurrent = u.i[0];
    }
}
/*###########################################################################################################*/	
/* NOT TESTED AT 11.0592 MHz 6 CLOCK/CYCLE*/
void WritePowerSolveModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_write)
{	
#ifdef PAE_FUNCTION_ENABLE
	/*#ifdef DEBUG_I2C_ERROR
		PutStringUsb("\r\nPAE < Sending Write Command to PAE");
	#endif*/
	//BlockOrPageChange:
	PAE_I2C_Start();
	if(PAE_I2C_ErrorFlag == TRUE)
	{	PAE_I2C_Stop();
		no_of_byte_to_write = 0;
		//Code Required For Testing
		//OutTextXY(0,1,"W0 ERROR");
		//while(1);					 
	}
	else
	{	PAE_I2C_Send(device_address);			//DEVICE ADDRESS
		if(PAE_I2C_ErrorFlag == TRUE)
		{	PAE_I2C_Stop();
			no_of_byte_to_write = 0;
			//Code Required For Testing
			//OutTextXY(0,1,"W1 ERROR");
			//while(1);					  
		}
		else
		{	PAE_I2C_Send(mem_start_add);
			if(PAE_I2C_ErrorFlag == TRUE)
			{	PAE_I2C_Stop();
				no_of_byte_to_write = 0;
				//Code Required For Testing
				//OutTextXY(0,1,"W3 ERROR");
				//while(1);					  
			}
			else
			{	while(no_of_byte_to_write)
				{	PAE_I2C_Send(*buff);
					if(PAE_I2C_ErrorFlag == TRUE)
					{	PAE_I2C_Stop();
						no_of_byte_to_write = 0;
						//Code Required For Testing
						//OutTextXY(0,1,"W4 ERROR");
						//while(1);					 
					}
					else
					{	no_of_byte_to_write--;
						buff++;
						mem_start_add++;		
						//if((mem_start_add%8)==0)	//Since Write Command has to be issued after each page change
						//{	I2C_Stop();
						//	Delay(2100);
						//	goto BlockOrPageChange;
						//}
						DelayInMilliSec(20);//Write Cycle Time Max 10ms Required	
					}
				}
				PAE_I2C_Stop(); 
				DelayInMilliSec(50);		//This delay is required if Read is initiated soon after write
			}
		}
	}
	#ifdef DEBUG_I2C_ERROR
		if(PAE_I2C_ErrorFlag == TRUE)
		{	PutStringUsb("\r\nPAE > COMM. WRITE ERROR");
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
            PAE_I2C_Stop(); 
            DelayInMilliSec(10);
        }
	#endif
/*	if(PAE_I2C_ErrorFlag == TRUE)
	{	RegisterAlarm(PAE_COMM_ERROR_FLAG, ALARM_OCCURED);
		#ifdef DEBUG_I2C_ERROR
			PutStringUsb("\r\nPAE > COMM. WRITE ERROR");
		#endif
	}
	else
		RegisterAlarm(PAE_COMM_ERROR_FLAG, ALARM_RESOLVED);
	*/
#endif
}
/*###########################################################################################################*/
void PAE_ForceI2CStop()
{	PAE_I2C_Stop(); 
	DelayInMilliSec(100);
}