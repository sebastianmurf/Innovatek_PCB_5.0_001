#include "MyDelay.h"
#include "MyConstants.h"
#include "stm32f0xx_hal.h"

#ifdef WATCH_DOG
	extern WWDG_HandleTypeDef hwwdg;
#endif
void DelayMachineCycle(unsigned int t)
{
	for(;t>0;t--)
	{	
		#ifdef WATCH_DOG
			HAL_WWDG_Refresh(&hwwdg);
		#endif
	}
}
void MyDelayMS(unsigned int t)
{	for(;t>0;t--)
	{	HAL_Delay(1);
		#ifdef WATCH_DOG
			HAL_WWDG_Refresh(&hwwdg);
		#endif
	}
}
