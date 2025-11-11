#include "MCP3424.h"

uint32_t ReadADC(uint8_t channel, uint8_t bit, uint8_t gain)
{	uint8_t AdcConfigVal;
	uint8_t val[3];
	
	switch(channel)
	{	default:
		case 1: AdcConfigVal = 0x90;break;
		case 2: AdcConfigVal = 0xB0;break;
		case 3: AdcConfigVal = 0xD0;break;
		case 4: AdcConfigVal = 0xF0;break;
	}
	switch(bit)
	{	default:
		case 12: AdcConfigVal |= 0x00;break;
		case 14: AdcConfigVal |= 0x04;break;
		case 16: AdcConfigVal |= 0x08;break;
		case 18: AdcConfigVal |= 0x0C;break;
	}
	switch(gain)
	{	default:
		case 1: AdcConfigVal |= 0x00;break;
		case 2: AdcConfigVal |= 0x01;break;
		case 4: AdcConfigVal |= 0x02;break;
		case 8: AdcConfigVal |= 0x03;break;
	}
	
	if(HAL_I2C_Mem_Read(&hi2c1 , MCP_I2C_ADD , AdcConfigVal , I2C_MEMADD_SIZE_8BIT , val , 3, 100) == HAL_OK)
	{ 	switch(bit)
		{	default:
			case 12: return val[0]*256+val[1];
			case 14: return val[0]*256+val[1];
			case 16: return val[0]*256+val[1];
			case 18: return val[0]*65536+val[1]*256+val[2];
		}
	}		
	else
		return 0x7FFFFFFF;//MAX POSITIVE VALUE	
}
