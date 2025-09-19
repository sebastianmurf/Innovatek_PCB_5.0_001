#include "PAE_I2C.h"
#include "delay.h"
#include "gpio.h"

#define HIGH 1
#define LOW 0
/*###########################################################################################################*/
/********* VARIABLE LOCAL TO THIS FILE DECLERATION ***********/
boolean_t PAE_I2C_ErrorFlag;
uc PAE_loop;
boolean_t PAE_I2C_SDA_CurStat, PAE_I2C_SCL_CurStat;
/*************************************************************/
/*###########################################################################################################*/
/********* VARIABLE EXTERNAL TO THIS FILE DECLERATION ***********/

/*************************************************************/
/*###########################################################################################################*/
/**********************FUNCTION LOCAL TO THE FILE*************/
/**** FUNCTIONS RELATED TO I2C PROTOCOL ****/
void PAE_I2C_Clock();
void PAE_I2C_Start(void);
void PAE_I2C_Stop(void);
void PAE_I2C_Nack(void);
void PAE_I2C_Ack(void);
void PAE_I2C_Send(char);
int PAE_I2C_Get(void);
/*###########################################################################################################*/
/*EXTERNAL FUNCTIONS REQUIRED FOR TESTING PURPOSE*/

/*************************************************************/
/*###########################################################################################################*/
/*###########################################################################################################*/
void PAE_I2C_Start(void)
{	PAE_I2C_SCL_INIT_AS_OUTPUT_HIGH;
	PAE_I2C_SDA_INIT_AS_OUTPUT_HIGH;
	//SDA = HIGH;
	//SCL = HIGH;
	DelayInMicroSec(32);
	PAE_I2C_SCL_INIT_AS_INPUT;
	PAE_I2C_SDA_INIT_AS_INPUT;
	
	if(CHECK_PAE_I2C_SCL == 0 || CHECK_PAE_I2C_SDA == 0/*SDA == 0 || SCL == 0*/)
	{	PAE_I2C_ErrorFlag = TRUE;
	}
	else
	{	PAE_I2C_ErrorFlag = FALSE;
		//DelayInMicroSec(16);
		PAE_I2C_SDA_INIT_AS_OUTPUT_LOW;//SDA = LOW;
		DelayInMicroSec(32);
		PAE_I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
		DelayInMicroSec(32);
	}
}
/*###########################################################################################################*/
void PAE_I2C_Send(char a)
{	static uc counter = 0;	// Code Used For Testing
	//xdata uc i;
	counter++; 				// Code Used For Testing
	
	for(PAE_loop=0x80;PAE_loop>0x00;PAE_loop>>=1)
	{	if((a&PAE_loop)!=0x00)
		{	PAE_I2C_SDA_INIT_AS_OUTPUT_HIGH;//I2C_SDA_HIGH;//SDA = HIGH;
		}
		else
		{	PAE_I2C_SDA_INIT_AS_OUTPUT_LOW;//I2C_SDA_LOW;//SDA = LOW;
		}
		PAE_I2C_Clock();
	}
	//PAE_I2C_Clock();
	PAE_I2C_SDA_HIGH;//SDA = HIGH;
	PAE_I2C_SCL_HIGH;//SCL = HIGH;
	PAE_I2C_SCL_INIT_AS_INPUT;
	PAE_I2C_SDA_INIT_AS_INPUT;
	DelayInMicroSec(16);
	while(!CHECK_PAE_I2C_SCL/*SCL*/);
	DelayInMicroSec(16);
	if(CHECK_PAE_I2C_SDA > 0 /*SDA ==HIGH*/)
	{	PAE_I2C_ErrorFlag = TRUE;
		 //Code Used For Testing
		//for(i=0;counter>0;counter/=10,i++)
		//	OutCharXY(15-i,1,(counter%10)+48);
	}
	else
	{	PAE_I2C_ErrorFlag = FALSE;
	}
	PAE_I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
	DelayInMicroSec(32);
}
/*###########################################################################################################*/
int PAE_I2C_Get(void)
{	char a = 0;
	PAE_I2C_SDA_INIT_AS_OUTPUT_HIGH;//SDA = HIGH;
	for(PAE_loop = 0x08;PAE_loop>0x00;PAE_loop--)
	{	DelayInMicroSec(16);
		PAE_I2C_SCL_INIT_AS_OUTPUT_HIGH;//SCL = HIGH;
		PAE_I2C_SCL_INIT_AS_INPUT;
		DelayInMicroSec(8);
        while(!CHECK_PAE_I2C_SCL);
		DelayInMicroSec(8);
        a = a << 0x01;
		PAE_I2C_SDA_INIT_AS_INPUT;
		if(CHECK_PAE_I2C_SDA > 0)
			a = a | 0x01;
		else
			a = a |0x00;
		PAE_I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
		DelayInMicroSec(8);
	}
	return (a);
}
/*###########################################################################################################*/
void PAE_I2C_Stop(void)
{	PAE_I2C_SDA_INIT_AS_OUTPUT_LOW;//SDA = LOW;
	PAE_I2C_SCL_INIT_AS_OUTPUT_HIGH;//SCL = HIGH;
	PAE_I2C_SCL_INIT_AS_INPUT;
	DelayInMicroSec(8);
	while(!CHECK_PAE_I2C_SCL);
	DelayInMicroSec(8);
	PAE_I2C_SDA_HIGH;//SDA = HIGH;
}
/*###########################################################################################################*/
void PAE_I2C_Ack(void)
{	PAE_I2C_SDA_INIT_AS_OUTPUT_LOW;//SDA = LOW;
	PAE_I2C_Clock();
}
/*###########################################################################################################*/
void PAE_I2C_Nack(void)
{	PAE_I2C_SDA_INIT_AS_OUTPUT_HIGH;//SDA = HIGH;
	PAE_I2C_Clock();
}
/*###########################################################################################################*/
void PAE_I2C_Clock()
{	DelayInMicroSec(16);	
	PAE_I2C_SCL_INIT_AS_OUTPUT_HIGH;//SCL = HIGH;
	PAE_I2C_SCL_INIT_AS_INPUT;
	DelayInMicroSec(16);	
	while(!CHECK_PAE_I2C_SCL);
	DelayInMicroSec(16);	
	PAE_I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
	DelayInMicroSec(16);
}
/*###########################################################################################################*/
