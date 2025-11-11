#include "i2c.h"
#include "delay.h"
#include "gpio.h"

#define HIGH 1
#define LOW 0
/*###########################################################################################################*/
/********* VARIABLE LOCAL TO THIS FILE DECLERATION ***********/
boolean_t I2C_ErrorFlag;
uc I2C_ComErrorCounter = 0;
uc loop;
boolean_t I2C_SDA_CurStat, I2C_SCL_CurStat;
/*************************************************************/
/*###########################################################################################################*/
/********* VARIABLE EXTERNAL TO THIS FILE DECLERATION ***********/

/*************************************************************/
/*###########################################################################################################*/
/**********************FUNCTION LOCAL TO THE FILE*************/
/**** FUNCTIONS RELATED TO I2C PROTOCOL ****/
void I2C_Clock();
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Nack(void);
void I2C_Ack(void);
void I2C_Send(char);
int I2C_Get(void);
/*###########################################################################################################*/
/*EXTERNAL FUNCTIONS REQUIRED FOR TESTING PURPOSE*/

/*************************************************************/
/*###########################################################################################################*/

/*###########################################################################################################*/
void I2C_Start(void)
{	I2C_SCL_INIT_AS_OUTPUT_HIGH;
	I2C_SDA_INIT_AS_OUTPUT_HIGH;
	//SDA = HIGH;
	//SCL = HIGH;
	DelayInMicroSec(4);
	I2C_SCL_INIT_AS_INPUT;
	I2C_SDA_INIT_AS_INPUT;
	
	if(CHECK_I2C_SCL == 0 || CHECK_I2C_SDA == 0/*SDA == 0 || SCL == 0*/)
	{	I2C_ErrorFlag = TRUE;
	}
	else
	{	I2C_ErrorFlag = FALSE;
		DelayInMicroSec(4);
		I2C_SDA_INIT_AS_OUTPUT_LOW;//SDA = LOW;
		DelayInMicroSec(4);
		I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
		DelayInMicroSec(4);
	}
}
/*###########################################################################################################*/
void I2C_Send(char a)
{	static uc counter = 0;	// Code Used For Testing
	//xdata uc i;
	counter++; 				// Code Used For Testing
	
	for(loop=0x80;loop>0x00;loop>>=1)
	{	if((a&loop)!=0x00)
		{	I2C_SDA_INIT_AS_OUTPUT_HIGH;//I2C_SDA_HIGH;//SDA = HIGH;
		}
		else
		{	I2C_SDA_INIT_AS_OUTPUT_LOW;//I2C_SDA_LOW;//SDA = LOW;
		}
		I2C_Clock();
	}
	//I2C_Clock();
	I2C_SDA_HIGH;//SDA = HIGH;
	I2C_SCL_HIGH;//SCL = HIGH;
	I2C_SCL_INIT_AS_INPUT;
	I2C_SDA_INIT_AS_INPUT;
	DelayInMicroSec(4);
	while(!CHECK_I2C_SCL/*SCL*/);
	if(CHECK_I2C_SDA > 0 /*SDA ==HIGH*/)
	{	I2C_ErrorFlag = TRUE;
		 //Code Used For Testing
		//for(i=0;counter>0;counter/=10,i++)
		//	OutCharXY(15-i,1,(counter%10)+48);
	}
	else
	{	I2C_ErrorFlag = FALSE;
	}
	I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
	DelayInMicroSec(4);
}
/*###########################################################################################################*/
int I2C_Get(void)
{	char a = 0;
	I2C_SDA_INIT_AS_OUTPUT_HIGH;//SDA = HIGH;
	for(loop = 0x08;loop>0x00;loop--)
	{	DelayInMicroSec(2);
		I2C_SCL_INIT_AS_OUTPUT_HIGH;//SCL = HIGH;
		I2C_SCL_INIT_AS_INPUT;
		while(!CHECK_I2C_SCL);
		a = a << 0x01;
		I2C_SDA_INIT_AS_INPUT;
		if(CHECK_I2C_SDA > 0)
			a = a | 0x01;
		else
			a = a |0x00;
		I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
		DelayInMicroSec(2);
	}
	return (a);
}
/*###########################################################################################################*/
void I2C_Stop(void)
{	I2C_SDA_INIT_AS_OUTPUT_LOW;//SDA = LOW;
	I2C_SCL_INIT_AS_OUTPUT_HIGH;//SCL = HIGH;
	I2C_SCL_INIT_AS_INPUT;
	while(!CHECK_I2C_SCL);
	
	I2C_SDA_HIGH;//SDA = HIGH;
}
/*###########################################################################################################*/
void I2C_Ack(void)
{	I2C_SDA_INIT_AS_OUTPUT_LOW;//SDA = LOW;
	I2C_Clock();
}
/*###########################################################################################################*/
void I2C_Nack(void)
{	I2C_SDA_INIT_AS_OUTPUT_HIGH;//SDA = HIGH;
	I2C_Clock();
}
/*###########################################################################################################*/
void I2C_Clock()
{	DelayInMicroSec(2);	
	I2C_SCL_INIT_AS_OUTPUT_HIGH;//SCL = HIGH;
	I2C_SCL_INIT_AS_INPUT;
	while(!CHECK_I2C_SCL);
	DelayInMicroSec(4);	
	I2C_SCL_INIT_AS_OUTPUT_LOW;//SCL = LOW;
	DelayInMicroSec(2);
}
/*###########################################################################################################*/
