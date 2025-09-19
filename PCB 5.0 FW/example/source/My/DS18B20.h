#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "VARIABLE.h"
#include "1wire.h"

extern void GetTemperature();
extern void ReadTemperature(void);	  //Read temperature

union temperature
{	unsigned char c[4];
	int i;
	float f;
};
extern union temperature Temperature,LastLogTemperature;
extern boolean_t GetTemperatureFlag;


#endif