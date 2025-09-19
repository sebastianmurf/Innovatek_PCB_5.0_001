#include"DS18B20.h"
#include"DualTimer.h"
#include"constants.c"
//for testing
#include"variable.h"
#include"MyUsb.h"
#include<stdio.h>
#include<string.h>

void GetTemperature();
void  ReadTemperature(void);	  //Read temperature
char Init_DS18B20(void);
void WriteOneChar(unsigned char dat);
unsigned char ReadOneChar(void);

union temperature Temperature,LastLogTemperature;
boolean_t GetTemperatureFlag = FALSE;

void GetTemperature()
{	if(GetTemperatureFlag == TRUE)
	{	GetTemperatureFlag = FALSE;
		ReadTemperature();
	}    
}

void ReadTemperature(void)	  //Read temperature
{   
    #define ONE_WIRE_TEMP_SAMPLE_COUNT  5
	static uc one_wire_sample_counter=0;
	static float OneWireSample[ONE_WIRE_TEMP_SAMPLE_COUNT];
	
  	OneWireData.ll=0;
	bTemp = FALSE;
  	OneWireReset();
  	if(OneWirePresence()==0)
  		bTemp = TRUE;
	else{
	  	OneWireReset();
        if(OneWirePresence()==0)
            bTemp = TRUE;
		else{
		  	OneWireReset();
            if(OneWirePresence()==0)
                bTemp = TRUE;
        }
    }
				
    if(bTemp == TRUE)
    {   DelayInMilliSec(10);
		OneWireWriteByte(0xCC); // Skip ROM
		OneWireWriteByte(0x44); // Start temperature conversion
    	OneWireReset();
        if(OneWirePresence()==0)
        {	OneWireWriteByte(0xCC); //Skip ROM
    		OneWireWriteByte(0xBE); //Read the temperature register
    		OneWireData.c[4]=OneWireReadByte();			  // for LSB
    		OneWireData.c[5]=OneWireReadByte();			  // for MSB
			if(OneWireData.i[1] > 0x07D0)//padding sign bit if the value is -ve
			{	OneWireData.i[1] |= 0xfffff800;
			}
			OneWireData.f[0] = (float)OneWireData.i[1]/16;
			#ifdef DEBUG_ONE_WIRE
				sprintf(PasteBuff,"\r\nTemperature = %f",OneWireData.f[0]);	//testing code
			#endif
			OneWireSample[one_wire_sample_counter] = OneWireData.f[0];
			one_wire_sample_counter++;
			if(one_wire_sample_counter == ONE_WIRE_TEMP_SAMPLE_COUNT){
				one_wire_sample_counter = 0;//for next lot of sampling
				//arrange the data into assending order
			  for(cTemp=0;cTemp<ONE_WIRE_TEMP_SAMPLE_COUNT;cTemp++){
					for(ucTemp=cTemp+1;ucTemp<ONE_WIRE_TEMP_SAMPLE_COUNT;ucTemp++){
				  		if(OneWireSample[cTemp]>OneWireSample[ucTemp]){
							OneWireSample[cTemp] += OneWireSample[ucTemp];
							OneWireSample[ucTemp] = OneWireSample[cTemp] - OneWireSample[ucTemp];
							OneWireSample[cTemp] -= OneWireSample[ucTemp];
				  		}
					}
				}
			  	Temperature.i = (int)(OneWireSample[ONE_WIRE_TEMP_SAMPLE_COUNT/2]*10);//GET THE MEDIAN VALUE in int format including upto one decimal value
				#ifdef DEBUG_ONE_WIRE
			  		sprintf(CopyBuff,"\r\nRecorded Temperature = %d.%d",Temperature.i/10,Temperature.i%10);	//testing code
					strcat(PasteBuff,CopyBuff);//testing code
				#endif
			}

		}
		#ifdef DEBUG_ONE_WIRE
		else
		  	sprintf(PasteBuff,"\r\nReset after conversion ERROR");
		PutStringUsb(PasteBuff);
		#endif
    }
}
/*###########################################################################################################*/
