#include "Atlas_TC.h"
#include "Delay.h"
#include "gpio.h"
#include "constants.c"
#include "MyUsb.h"
#include "myfun.h"
#include "mmu.h"
#include "variable.h"
#include <stdio.h>

void ReadAtlasModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_read)
{	
#ifdef ATLAS_I2C_FUNCTION_ENABLE
	//start
	I2C_Start();
	
	if(I2C_ErrorFlag == TRUE)
	{	I2C_Stop();
		no_of_byte_to_read = 0;
		//Code Required For Testing
		//OutTextXY(0,1,"R0 ERROR");	 
		//while(1);
	}
	else
	{	I2C_Send(device_address);				//DEVICE ADDRESS
		if(I2C_ErrorFlag == TRUE)
		{	I2C_Stop();
			no_of_byte_to_read = 0;
			//Code Required For Testing
			//OutTextXY(0,1,"R1 ERROR");	 
			//while(1);
		}
		else
		{	I2C_Send(mem_start_add);
			if(I2C_ErrorFlag == TRUE)
			{	I2C_Stop();
				no_of_byte_to_read = 0;
				//Code Required For Testing
				//OutTextXY(0,1,"R3 ERROR");	
				//while(1);
			}
			else
			{	I2C_Stop();
				I2C_Start();
				if(I2C_ErrorFlag == TRUE)
				{	I2C_Stop();
					no_of_byte_to_read = 0;
					//Code Required For Testing
					//OutTextXY(0,1,"R4 ERROR");	
					//while(1);
				}
				else
				{	I2C_Send(device_address|0X01);				//DEVICE ADDRESS WITH READ COMMAND
					if(I2C_ErrorFlag == TRUE)
					{	I2C_Stop();
						no_of_byte_to_read = 0;
						// Code Required For Testing
						//OutTextXY(0,1,"R5 ERROR");	
						//while(1);
					}
					else
					{	while(no_of_byte_to_read)
						{	*buff = I2C_Get();
							buff++;
							mem_start_add++;
							if(no_of_byte_to_read == 1 || mem_start_add%131072 == 0)
							{	I2C_Nack();
								I2C_Stop();
								if(no_of_byte_to_read == 1)
									no_of_byte_to_read = 0;
							}
							else
							{	I2C_Ack();
								no_of_byte_to_read--;
							}
						}
					}
				}		
			}
		}
	}
	if(I2C_ErrorFlag == TRUE){
		I2C_ComErrorCounter++;
		if((I2C_ComErrorCounter%200) == 0)
			RegisterAlarm(ATLAS_I2C_COMM_ERROR_FLAG, ALARM_OCCURED);
			
		if(I2C_ComErrorCounter > 250){
			I2C_ComErrorCounter = 0;
			PutStringUsb("\r\nRESTART DUE TO ATLAS I2C ERROR");//RESET TEST CODE
            DelayInMilliSec(2000);
			PeriodicSave();
            RESET_DEVICE;
		}
		#ifdef DEBUG_I2C_ERROR
		    PutStringUsb("\r\nATLAS I2C > COMM. READ ERROR");
			//sprintf(CopyBuff,"\r\nATLAS I2C %d > COMM. READ ERROR",device_address);
            //PutStringUsb(CopyBuff);
        #endif
	}
	else{
		RegisterAlarm(ATLAS_I2C_COMM_ERROR_FLAG, ALARM_RESOLVED);
		I2C_ComErrorCounter = 0;
	}
#endif
}
/*###########################################################################################################*/
/* NOT TESTED AT 11.0592 MHz 6 CLOCK/CYCLE*/

void WriteAtlasModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_write)
{	
#ifdef ATLAS_I2C_FUNCTION_ENABLE
	//BlockOrPageChange:
	I2C_Start();
	if(I2C_ErrorFlag == TRUE)
	{	I2C_Stop();
		no_of_byte_to_write = 0;
		//Code Required For Testing
		//OutTextXY(0,1,"W0 ERROR");
		//while(1);					 
	}
	else
	{	I2C_Send(device_address);			//DEVICE ADDRESS
		if(I2C_ErrorFlag == TRUE)
		{	I2C_Stop();
			no_of_byte_to_write = 0;
			//Code Required For Testing
			//OutTextXY(0,1,"W1 ERROR");
			//while(1);					  
		}
		else
		{	I2C_Send(mem_start_add);
			if(I2C_ErrorFlag == TRUE)
			{	I2C_Stop();
				no_of_byte_to_write = 0;
				//Code Required For Testing
				//OutTextXY(0,1,"W3 ERROR");
				//while(1);					  
			}
			else
			{	while(no_of_byte_to_write)
				{	I2C_Send(*buff);
					if(I2C_ErrorFlag == TRUE)
					{	I2C_Stop();
						no_of_byte_to_write = 0;
						//Code Required For Testing
						//OutTextXY(0,1,"W4 ERROR");
						//while(1);					 
					}
					else
					{	no_of_byte_to_write--;
						buff++;
						mem_start_add++;		
						/*if((mem_start_add%8)==0)	//Since Write Command has to be issued after each page change
						{	I2C_Stop();
							Delay(2100);
							goto BlockOrPageChange;
						}*/
						DelayInMilliSec(10);//Write Cycle Time Max 10ms Required	
					}
				}
				I2C_Stop(); 
				DelayInMilliSec(10);		//This delay is required if Read is initiated soon after write
			}
		}
	}
	#ifdef DEBUG_I2C_ERROR
		if(I2C_ErrorFlag == TRUE)
			PutStringUsb("\r\nATLAS I2C > COMM. WRITE ERROR");
	#endif
#endif
}
/*###########################################################################################################*/
