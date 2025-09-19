#ifndef __PROJECT_VARIABLE_H__
#define __PROJECT_VARIABLE_H__

#include<stm32f0xx_hal.h>
#include "ProjectConstants.h"

#define MASTER_BUFF_SIZE 500

union temp{
	uint8_t c[8];
	uint16_t i[4];
	uint32_t l[2];
	uint64_t ld;	
};

extern char CopyBuff[100];		//stores data from the memory temporarily
extern char cTemp;
extern int iTemp;
extern char MasterBuff[MASTER_BUFF_SIZE];
extern char PasteBuff[100];		//stores data to be stored into memory temporarily
extern sli sliTemp;
extern union temp u;
extern uc ucTemp;
extern ui uiTemp;
extern float fTemp;
extern uli uliTemp;

extern uint16_t FlowKvalue;
extern uint64_t FlowIntrCounterCurrent, FlowIntrCounterLastSent;
extern uint16_t CurrentFlowIntrCounter;
extern float TotalVolume;
extern ui CurrentFlow;

extern uint32_t TemperatureDVal;

#endif
