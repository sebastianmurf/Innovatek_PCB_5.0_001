/*******************************************************************************
FILE NAME			: 	MCP3424.h (4 Channel 18bit Sigma Delta ADC)
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	01/06/2020
MOTHER VERSION NO	:	1.0
LAST MODIFIED DATE	:	01/06/2020
CURRENT VERSION NO	:	1.0
*******************************************************************************/
#ifndef __MCP3424_H__
#define __MCP3424_H__

#include<stm32f0xx_hal.h>
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

#define MCP_I2C_ADD 0xD0

extern uint32_t ReadADC(uint8_t channel, uint8_t bit, uint8_t gain);

#endif
