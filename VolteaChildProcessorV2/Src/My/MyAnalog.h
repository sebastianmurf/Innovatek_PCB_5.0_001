/*******************************************************************************
FILE NAME			: 	MyAnalog.h
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	01/11/2020
MOTHER VERSION NO	:	1.0
LAST MODIFIED DATE	:	01/11/2020
CURRENT VERSION NO	:	1.0
*******************************************************************************/
#ifndef __MY_ANALOG_H__
#define __MY_ANALOG_H__

#include "MCP3424.h"

#define MAX_NO_OF_SAMPLE_FOR_MEDIAN 5
#define MAX_NO_OF_SAMPLE_FOR_AVERAGE 2

extern uint64_t TemperatureFilteredDval;
extern float TemperatureFilteredTemperature;

extern void ReadTemperature(void);

#endif
