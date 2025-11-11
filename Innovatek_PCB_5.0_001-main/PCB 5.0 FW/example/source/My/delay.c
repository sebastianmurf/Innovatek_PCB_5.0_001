#include "delay.h"
#include "common.h"

#include"constants.c"   //FOR WATCH DOG APPLICATION
#include"DualTimer.h"   //FOR WATCH DOG APPLICATION 

//------------------------------------------------------------------------------
void Delay(ui t)
{ for(;t>0;t--);
}
//------------------------------------------------------------------------------
void DelayInSec(uc sec)
{	ui i;
  	for(;sec>0;sec--){
		#ifdef WATCH_DOG_TIMER_ENABLED
            RESET_WATCH_DOG_TIMER;
        #endif
		for(i=0x009CFFFF;i>0;i--){
		//__no_operation();
		}
	}
}
//------------------------------------------------------------------------------
void DelayInMilliSec(ui millisec)
{	ui i;
  	for(;millisec>0;millisec--){
		#ifdef WATCH_DOG_TIMER_ENABLED
            RESET_WATCH_DOG_TIMER;
        #endif
		for(i=10289;i>0;i--){
		//__no_operation();
		}
	}
}
//------------------------------------------------------------------------------
void DelayInMicroSec(ui micro_sec)
{	ui i;
  	for(;micro_sec>0;micro_sec--){
		for(i=11;i>0;i--){
		//__no_operation();
		}
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
