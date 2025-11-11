#ifndef __DUALTIMER_H__
#define __DUALTIMER_H__

#include "common.h"
#include<time.h>

extern time_t _ttemp;
extern struct tm *TimeStampPtr;
extern ui FreeRunSec;
//extern ui ServerAckTimeStamp;
extern ui SecAfterTimeSync;
extern boolean_t SecondChangeFlag;
extern const char day[7][4];
//---------------------------------------------------------------------------------

/******************* DUAL TIMER CONSTANTS CONFIGURATION STARTS ***************************/
//#define TIMER_RELOAD_VAL 0x02625A00	//1SEC * 40MHz Ideally Should be this 
//#define TIMER_RELOAD_VAL 0x02625A01	//to make little slow than gps clock sec
#define TIMER_RELOAD_VAL 0x01312D00		//
/******************* DUAL TIMER CONSTANTS CONFIGURATION ENDS ***************************/

//---------------------------------------------------------------------------------
#define RESET_WATCH_DOG_TIMER {FM3_SWWDT->WDOGLOCK = 0x1ACCE551; FM3_SWWDT->WDOGINTCLR = 0x11111111; FM3_SWWDT->WDOGLOCK = 0;}

/******************* DUAL TIMER FUNCTIONS LOCAL TO THE FILE STARTS **********/
extern void InitDualTimer();
extern time_t TimeStamp2Sec(struct tm t);
extern char* Gmt2LocalTimeStamp(struct tm t);

/******************* DUAL TIMER FUNCTIONS LOCAL TO THE FILE ENDS **********/

//---------------------------------------------------------------------------------

#endif
