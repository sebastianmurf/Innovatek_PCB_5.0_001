#include "PC_Communication.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "SerialCommunication.h"
#include "MyFunc.h"
#include "MyAnalog.h"
#include "InternalFlashMMU.h"

int result;
	
void CheckCommand()
{	if(CommFlag.Command == MY_TRUE)
	{	CommFlag.Command = MY_FALSE;
		result = SearchString((char*)RxBuff,"123=");
		if(result > 0)
		{	//strcpy(CopyBuff,"{123=12345,0,50,25}");//flow is pulse counted through interrupt
			sprintf(CopyBuff,"{123=%d,0,%d,%d}",(unsigned int)TemperatureFilteredDval,(ui)(FlowIntrCounterCurrent-FlowIntrCounterLastSent),CurrentFlow);
			FlowIntrCounterLastSent = FlowIntrCounterCurrent;
			SendString(CopyBuff);
			
		}
		else if((result = (SearchString((char*)RxBuff,"000=0"))) > 0)
		{	while(1);//hang for reboot test
		}
		/*else if((result = (SearchString((char*)RxBuff,"304=0"))) > 0)
		{	FlowIntrCounterCurrent = 0;
			FlowIntrCounterLastSent = 0;
			CurrentFlowIntrCounter = 0;
			TotalVolume = 0;
			CurrentFlow = 0;
		}
		else if((result = (SearchString((char*)RxBuff,"201=?"))) > 0)
		{	sprintf(CopyBuff,"{201=%d}",(unsigned int)TemperatureFilteredDval);
			SendString(CopyBuff);
		}
		else if((result = (SearchString((char*)RxBuff,"100=1"))) > 0)//need to test 
		{	//reset adc
			if(HAL_I2C_Mem_Write(&hi2c1 , MCP_I2C_ADD , 0x06 , I2C_MEMADD_SIZE_8BIT , 0, 0, 100) == HAL_OK)
			{
			}
		}*/
	}
}
