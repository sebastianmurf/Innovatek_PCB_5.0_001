#include "MyDelay.h"
#include "stm32f0xx_hal.h"
#include "ProjectConstants.h"

#ifdef WATCH_DOG
	#if(PROJECT_MODE==RELEASED)
		extern WWDG_HandleTypeDef hwwdg;
	#endif
#endif
void DelayMachineCycle(unsigned int t)
{
	for(;t>0;t--)
	{	
		#ifdef WATCH_DOG
			#if(PROJECT_MODE==RELEASED)
				HAL_WWDG_Refresh(&hwwdg);
			#endif
		#endif
	}
}
void MyDelayMS(unsigned int t)
{	for(;t>0;t--)
	{	HAL_Delay(1);
		#ifdef WATCH_DOG
			#if(PROJECT_MODE==RELEASED)
				HAL_WWDG_Refresh(&hwwdg);
			#endif
		#endif
	}
}
