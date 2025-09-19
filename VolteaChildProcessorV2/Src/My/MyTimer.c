#include "MyTimer.h"
#include "ProjectConstants.h"
#include "ProjectVariable.h"

struct timer MyClock;

void InitMyTimer(void)
{	//INITIALIZE TIMER DEFAULT VALUE
	MyClock.Hr = 0;
	MyClock.Min = 0;
	MyClock.Sec = 0;
	MyClock.InterruptCounter = 0;
	MyClock.TimeStamp =0;
	//START TIMER
	HAL_TIM_Base_Start_IT(&htim14);
	MyClock.SyncDoneFlag = MY_FALSE;
	//INITIALIZE VARIABLE DEPENDENT ON TIMER
	
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
	if(htim->Instance == TIM14)
	{	//MyClock.TimeStamp += 1;//with 48MHz clock 1 interrupt = 1ms but this is hampering watch dog timer
		MyClock.TimeStamp += 4;//with 12MHz clock 1 interrupt = 4ms
		MyClock.TimeStamp %= 86400000;//in 1 day there are 86400000 mS. 1 interrupt per 1mS
		MyClock.InterruptCounter = MyClock.TimeStamp % 1000;
		MyClock.Sec = (MyClock.TimeStamp/1000) % 60;
		MyClock.Min = (MyClock.TimeStamp/60000) % 60;
		MyClock.Hr = (MyClock.TimeStamp/3600000);
		//---------------------------------------------------------------------------------//
		//if counting time slot is decreased than 1 sec then the result is less fluctuation but less sensitivity
		//for example 200ms then 5 pulse per sec will have 1 pulse every time slot, hence only 5 pulse per sec changed 
		//is sensed by the system.
		//if we increased the time slot then refresh rate is hampered.
		//if we take median technology mixed reaction is achieved.
		
		/* logic 1 */
		if(MyClock.InterruptCounter == 0)//calculate once in sec
		{	CurrentFlow = CurrentFlowIntrCounter;
			CurrentFlowIntrCounter = 0;
		}
		/* logic 2 *
		static uint16_t CurrentFlowIntrCounterArray[5];
		uint8_t i,j;
		
		if((MyClock.InterruptCounter % 200) == 0)//calculate once in 100 milli sec
		{	CurrentFlowIntrCounterArray[MyClock.InterruptCounter/200] = CurrentFlowIntrCounter;
			CurrentFlowIntrCounter = 0;
			if(MyClock.InterruptCounter == 0)//calculate once in sec
			{	//arrang the value in assending order
				for(i=0;i<5;i++)
				{	for(j=i+1;j<5;j++)
					{	if(CurrentFlowIntrCounterArray[i] > CurrentFlowIntrCounterArray[j])
						{	//swap
							CurrentFlowIntrCounterArray[i] += CurrentFlowIntrCounterArray[j];
							CurrentFlowIntrCounterArray[j] = CurrentFlowIntrCounterArray[i] - CurrentFlowIntrCounterArray[j];
							CurrentFlowIntrCounterArray[i] = CurrentFlowIntrCounterArray[i] - CurrentFlowIntrCounterArray[j];
						}
					}
				}
				CurrentFlow = ((float)CurrentFlowIntrCounterArray[2]*300/(float)FlowKvalue);
			}
		}*/
		/* logic 3 *
		static uint16_t CurrentFlowIntrCounterArray[3];
		static uint8_t current_slot = 0;
		uint8_t i,j;
		if((MyClock.InterruptCounter % 1000) == 0)//calculate once in 1 sec
		{	CurrentFlowIntrCounterArray[current_slot] = CurrentFlowIntrCounter;
			current_slot++;
			CurrentFlowIntrCounter = 0;
			if(current_slot == 3)//calculate once in sec
			{	//arrang the value in assending order
				for(i=0;i<3;i++)
				{	for(j=i+1;j<3;j++)
					{	if(CurrentFlowIntrCounterArray[i] > CurrentFlowIntrCounterArray[j])
						{	//swap
							CurrentFlowIntrCounterArray[i] += CurrentFlowIntrCounterArray[j];
							CurrentFlowIntrCounterArray[j] = CurrentFlowIntrCounterArray[i] - CurrentFlowIntrCounterArray[j];
							CurrentFlowIntrCounterArray[i] = CurrentFlowIntrCounterArray[i] - CurrentFlowIntrCounterArray[j];
						}
					}
				}
				CurrentFlow = ((float)CurrentFlowIntrCounterArray[1]*60/(float)FlowKvalue);
				current_slot = 0;
			}
		}*/
	}
}
