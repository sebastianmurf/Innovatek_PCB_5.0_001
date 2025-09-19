#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__

#include<stm32f0xx_hal.h>

extern TIM_HandleTypeDef htim14;

struct timer
{	_Bool SecondChangeFlag;
	_Bool MinChangeFlag;
	_Bool HrChangeFlag;
	_Bool SyncDoneFlag;
	uint16_t InterruptCounter;
	uint8_t Sec;
	uint8_t Min;
	uint8_t Hr;
	uint32_t TimeStamp;//in milli sec
};

extern struct timer MyClock;

//static void MX_TIM6_Init(void);

extern void InitMyTimer(void);


#endif
