#include "Atlas_C.h"
#include "Atlas_T.h"
#include "Atlas_TC.h"
#include "Variable.h"
#include "delay.h"
#include "MyUsb.h"
#include <stdio.h>
#include <stdlib.h>
#include "DualTimer.h"
#include "AtlasReplacement.h"
#include "constants.c"
#include "AT45DB161.h"
#include "gpio.h"

//#define MY_COMPENSATED_CONDUCTIVITY_LOGIC
#define ATLAS_COMPENSATED_CONDUCTIVITY_LOGIC
#ifdef MY_COMPENSATED_CONDUCTIVITY_LOGIC
    int AtlasUncompensatedConductivity = -1;
#endif
float Conductivity, Salinity, TDS;
int AtlasConductivity = -1;
long long AverageConductivityAccumulator = 0;
uc AverageConductivityCounter = 0;
	
uc CheckAtlasConductivityNewReadingAvailability()
{	ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,7,u.c,1);
	return u.c[0];
}
/*
void SetTemperatureCompensationToAtlasConductivity(float temperature)
{	u.f[0] = temperature;
	u.f[0] *= 100;
	u.i[1] = (int)u.f[0];
	//converting little-indian to big-indian format. Atlas send MSB byte first
	u.c[3] = u.c[4];
	u.c[2] = u.c[5];
	u.c[1] = u.c[6];
	u.c[0] = u.c[7];
	if(u.i[0] > 0 && u.i[0] < 20001)	//valid from 0.01 to 200.00 deg centigrade
		WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,16,u.c,4);
}
*/

void ReadAtlasConductivity()
{	static uc read_conductivity_sec = 0;
    
    if(FreeRunSec%10 != read_conductivity_sec)
    {   read_conductivity_sec = FreeRunSec%10;
    
        #ifdef MY_COMPENSATED_CONDUCTIVITY_LOGIC
            static uc my_ec_debug_flag = FALSE;
            if(FreeRunSec%5==0)
            {   my_ec_debug_flag = TRUE;
            }
        #endif
        
        #ifdef ATLAS_COMPENSATED_CONDUCTIVITY_LOGIC
            //GetAtlasReplacementReading();
            //RawWaterTemperature = -1500;
            //AtlasTemperature = RawWaterTemperature;//only for display on debug log else not required
            
            /*sprintf(PasteBuff,"\r\nTemp for compensation conductivity f= %3f, i=%d",(float)RawWaterTemperature/1000,RawWaterTemperature);
            PutStringUsb(PasteBuff);
            DelayInSec(1);*/
            u.i[1] = AtlasTemperature/10;
            //arrange the 4 byte integer value to send in MSB first 
            u.c[0] = u.c[7];
            u.c[1] = u.c[6];
            u.c[2] = u.c[5];
            u.c[3] = u.c[4];
            WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,16,u.c,4);//Feeding current temperature to atlas conductivity module
            
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
            
            //if(AverageConductivityFlag == TRUE){
                //calculate the average conductivity
                AverageConductivityAccumulator += AtlasConductivity;
                AverageConductivityCounter ++;
                //if(AverageConductivityCounter > 10)
                    AverageConductivity = AverageConductivityAccumulator / AverageConductivityCounter;
                if(AverageConductivityCounter >= 250)
                {	AverageConductivityCounter = 1;
                    AverageConductivityAccumulator = AverageConductivity;
                }
                //sprintf(CopyBuff,"\r\nAcc=%lld, Count=%d, AvgEC=%d",AverageConductivityAccumulator,AverageConductivityCounter,AverageConductivity);
                //PutStringUsb(CopyBuff);
                //Permanent code
                if(AverageConductivity < 500)
                {   //ALARM SHOULD OCCUR ONLY IN AUTO MODE
                    if(Mode == MODE_AUTO && CurrentProcessStep < WAIT)
                    {   DirectMemRead(EC_ZERO_CYCLE_COUNTa,u.c,EC_ZERO_CYCLE_COUNTn);//read zero ec cycle count
                        ucTemp = u.c[0];
                        DirectMemRead(SP_EC_ZERO_CYCLEa,u.c,SP_EC_ZERO_CYCLEn);//read zero ec max cycle limit
                        if(ucTemp >= u.c[0])
                        {   RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_OCCURED);
                            //CurrentProcessStep = IDLE;
                            //below code only if system going to IDLE
                            //DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
                        }
                    }
                    else
                    {   ZeroEcSecondCounter = 0;
                    }
                }
                else
                {   ZeroEcSecondCounter = 0;
                    RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_RESOLVED);
                }
            //}
        #endif
            
        #ifdef MY_COMPENSATED_CONDUCTIVITY_LOGIC
            static uc conductivity_read_val_counter = 0;
            static ui ConductivityReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION];
            /* NABEEN 27-01-2021 16:09
            #ifdef TEMPERATURE_COMPENSATED_CONDUCTIVITY
                static ui TemperatureReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION];
            #endif
            */
            ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,24,u.c,4);
            if(I2C_ErrorFlag == FALSE)
            {	//switch MSB <--> LSB. First byte received is MSB
                u.c[4] = u.c[3];
                u.c[5] = u.c[2];
                u.c[6] = u.c[1];
                u.c[7] = u.c[0];
                ConductivityReadSampleData[conductivity_read_val_counter] = u.i[1];
                //testing code starts
                //sprintf(CopyBuff,"\r\nConductivityReadSampleData[%d]=%d",conductivity_read_val_counter,u.i[1]);
                //PutStringUsb(CopyBuff);
                //testing code ends
                #ifdef TEMPERATURE_COMPENSATED_CONDUCTIVITY
                    //GetAtlasReplacementReading();
                    /* NABEEN 27-01-2021 16:09
                     TemperatureReadSampleData[conductivity_read_val_counter] = RawWaterTemperature;
                     */   
                    if(++conductivity_read_val_counter == CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION){
                        conductivity_read_val_counter = 0;
                        //arrange the sample into assending order
                        for(cTemp=0;cTemp<CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)
                        {	for(ucTemp=cTemp+1;ucTemp<CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION;ucTemp++)
                            {	if(ConductivityReadSampleData[cTemp] > ConductivityReadSampleData[ucTemp])
                                {	ConductivityReadSampleData[cTemp] += ConductivityReadSampleData[ucTemp];
                                    ConductivityReadSampleData[ucTemp] = ConductivityReadSampleData[cTemp] - ConductivityReadSampleData[ucTemp];
                                    ConductivityReadSampleData[cTemp] -= ConductivityReadSampleData[ucTemp];
                                }
                                /* NABEEN 27-01-2021 16:09
                                if(TemperatureReadSampleData[cTemp] > TemperatureReadSampleData[ucTemp])
                                {	TemperatureReadSampleData[cTemp] += TemperatureReadSampleData[ucTemp];
                                    TemperatureReadSampleData[ucTemp] = TemperatureReadSampleData[cTemp] - TemperatureReadSampleData[ucTemp];
                                    TemperatureReadSampleData[cTemp] -= TemperatureReadSampleData[ucTemp];
                                }*/
                            }
                        }
                                
                        //for(cTemp=0;cTemp<CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)//testing code
                        //{	sprintf(CopyBuff,"\r\nConductivityReadSampleData[%d]=%d",cTemp,ConductivityReadSampleData[cTemp]);//testing code
                        //    PutStringUsb(CopyBuff);//testing code
                        //}//testing code
                        /* NABEEN 27-01-2021 16:09
                        if(TemperatureReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION/2] > 0 && TemperatureReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION/2] < 100000)
                        {   AtlasTemperature = TemperatureReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION/2];//GET THE MEDIAN VALUE
                            AtlasTemperatureErrorFlag = FALSE;
                            AtlasTemperatureContinuousErrorSec = 0;
                        }
                        else
                        {    AtlasTemperatureErrorFlag = TRUE;
                        }*/
                        //UNCOMPENSATED
                        AtlasUncompensatedConductivity = ConductivityReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION/2];//GET THE MEDIAN VALUE
                        //COMPENSATED DEBUG_EC_CALC
                        /*
                        #ifdef DEBUG_EC_CALC
                            if(FreeRunSec%5 > 0 && my_ec_debug_flag == TRUE)
                            {   PutStringUsb("\r\n----------------- ATLAS TEMPERATURE COMPENSATED CONDUCTIVITY -------------------------------\r\n");
                                sprintf(CopyBuff,"LatestGoodTemp = %.2f\r\nAct_Cur_Temp = %.2f\r\nTempAtWhichECWasCali = %.2f",
                                        (float)AtlasTemperature/1000,
                                        (float)TemperatureReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION/2]/1000,
                                        (float)TemperatureAtWhichConductivityWasCalibrated/1000);
                                PutStringUsb(CopyBuff);
                            }
                        #endif
                        */
                        fTemp = (float)(((float)AtlasTemperature-(float)TemperatureAtWhichConductivityWasCalibrated)/1000);
                        #ifdef DEBUG_EC_CALC
                            if(FreeRunSec%5 > 0 && my_ec_debug_flag == TRUE)
                            {   PutStringUsb("\r\n--- ATLAS TEMPERATURE COMPENSATED CONDUCTIVITY ---");
                                sprintf(CopyBuff,"\r\nLatestGoodTemp = %.2f\r\nTempAtWhichECWasCali = %.2f\r\nDelta T (t-tc) = %.2f",(float)AtlasTemperature/1000,(float)TemperatureAtWhichConductivityWasCalibrated/1000,fTemp);
                                PutStringUsb(CopyBuff);
                            }
                        #endif
                        fTemp *=0.02;
                        #ifdef DEBUG_EC_CALC
                            if(FreeRunSec%5 > 0 && my_ec_debug_flag == TRUE)
                            {   sprintf(CopyBuff,"\r\na(t-tc) = %.2f",fTemp);
                                PutStringUsb(CopyBuff);
                            }
                        #endif
                        fTemp += 1;
                        #ifdef DEBUG_EC_CALC
                            if(FreeRunSec%5 > 0 && my_ec_debug_flag == TRUE)
                            {   sprintf(CopyBuff,"\r\n1+a(t-tc) = %.2f",fTemp);
                                PutStringUsb(CopyBuff);
                            }
                        #endif
                        AtlasConductivity = (int)((float)AtlasUncompensatedConductivity/fTemp);
                        #ifdef DEBUG_EC_CALC
                            if(FreeRunSec%5 > 0 && my_ec_debug_flag == TRUE)
                            {   sprintf(CopyBuff,"\r\nUC.EC = %d\r\nC.EC = %d",AtlasUncompensatedConductivity,AtlasConductivity);
                                PutStringUsb(CopyBuff);
                            }
                            my_ec_debug_flag = FALSE;
                        #endif
                        
                        //if(AverageConductivityFlag == TRUE){
                            //calculate the average conductivity
                            AverageConductivityAccumulator += AtlasConductivity;
                            AverageConductivityCounter ++;
                            //if(AverageConductivityCounter > 10)
                                AverageConductivity = AverageConductivityAccumulator / AverageConductivityCounter;
                            if(AverageConductivityCounter >= 250)
                            {	AverageConductivityCounter = 1;
                                AverageConductivityAccumulator = AverageConductivity;
                            }
                            //sprintf(CopyBuff,"\r\nAcc=%lld, Count=%d, AvgEC=%d",AverageConductivityAccumulator,AverageConductivityCounter,AverageConductivity);
                            //PutStringUsb(CopyBuff);
                            //Permanent code
                            if(AverageConductivity < 500)
                            {   if(ZeroEcSecondCounter > 30)
                                    RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_OCCURED);
                            }
                            else
                            {   ZeroEcSecondCounter = 0;
                                RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_RESOLVED);
                            }
                        //}
                    }
                #else
                    if(++conductivity_read_val_counter == CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION){
                        conductivity_read_val_counter = 0;
                        //arrange the sample into assending order
                        for(cTemp=0;cTemp<CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)
                        {	for(ucTemp=cTemp+1;ucTemp<CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION;ucTemp++)
                            {	if(ConductivityReadSampleData[cTemp] > ConductivityReadSampleData[ucTemp])
                                {	ConductivityReadSampleData[cTemp] += ConductivityReadSampleData[ucTemp];
                                    ConductivityReadSampleData[ucTemp] = ConductivityReadSampleData[cTemp] - ConductivityReadSampleData[ucTemp];
                                    ConductivityReadSampleData[cTemp] -= ConductivityReadSampleData[ucTemp];
                                }
                            }
                        }
                                
                        //for(cTemp=0;cTemp<MAX_NO_OF_SAMPLE_COLLECTION;cTemp++)//testing code
                        //{	sprintf(CopyBuff,"\r\ConductivityReadSampleData[%d]=%d",cTemp,ConductivityReadSampleData[cTemp]);//testing code
                        //    PutStringUsb(CopyBuff);//testing code
                        //}//testing code
                                
                        AtlasConductivity = ConductivityReadSampleData[CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION/2];//GET THE MEDIAN VALUE
                        //if(AverageConductivityFlag == TRUE){
                            //calculate the average conductivity
                            AverageConductivityAccumulator += AtlasConductivity;
                            AverageConductivityCounter ++;
                            //if(AverageConductivityCounter > 10)
                                AverageConductivity = AverageConductivityAccumulator / AverageConductivityCounter;
                            if(AverageConductivityCounter >= 250)
                            {	AverageConductivityCounter = 1;
                                AverageConductivityAccumulator = AverageConductivity;
                            }
                            //Permanent code
                            if(AverageConductivity < 500)
                            {   if(ZeroEcSecondCounter > 30)
                                    RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_OCCURED);
                            }
                            else
                            {   ZeroEcSecondCounter = 0;
                                RegisterAlarm(ATLAS_CONDUCTIVITY_ZERO_READ_FLAG, ALARM_RESOLVED);
                            }
                    }
                #endif
            }
        #endif
    }
}
/*
void AtlasConductivityModuleReadWrite(boolean_t read_write,uc address,char* data,uc no_of_bytes)
{	if(read_write == READ)
		ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,address,data,no_of_bytes);
	else
		WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,address,data,no_of_bytes);
}
*/
