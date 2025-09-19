#include"CONSTANTS.C"
#include"mmu.h"
#include"AT45DB161.h"
#include"variable.h"
#include"myfun.h"
#include"command.h"
#include"gpio.h"
#include"gprs.h"
//#include"GpsUart.h"
#include"GprsUart.h"
#include"DualTimer.h"
#include "board.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"math.h"
#include"MyUsb.h"
//#include"MyExtInt.h"
#include"delay.h"
#include"PAE800.h"
#include"AtlasReplacement.h"
#include "Atlas_TC.h"
#include "SerialHmi.h"

#ifdef ONE_WIRE
	#include"1wire.h"
	#include"DS18B20.h"
#endif
#ifdef CTS_RFID
	#include"CTS_RFID.h"
#endif

//#include<time.h>

boolean_t MemDebugFlag = FALSE;
	
struct tm temp_timestamp;
extern void AutoModeOperations();	
void ConfigureVariable();
void InitializeDefaultSetting();
void LoadSetting();

/*
char AddOtherUserMobile(long long mobno,char pid)
{	int loc;
	//search if user & pid exists
	if(SearchUserMobileNumber(mobno,pid)<0)//no user with this pid
	{	loc = SearchUserMobileNumber(mobno,0xFF);//search if user without any pid exists
	  	if(loc>0)//user with no pid exists
		{	RandomMemWrite(USER_TABLE_START_ADDRESS+(loc*USER_TABLE_RECORD_SIZE)+UT_PP_T_IDp,&pid,UT_PP_T_IDn);
			sprintf(PasteBuff,"User %d Edited as %lld for %s.",loc,mobno,strcatPid(pid));
			//PutStringUsb("\r\nUSER EXISTS ADDED PID WITH IT");//TESTING CODE
			return MY_DONE;
		}
		else//search vacent location
		{	loc = SearchUserMobileNumber(0xFFFFFFFFFFFFFFFF,0xFF);//USER TABLE RECORD SIZE IS 8 BYTE
			if(loc == -1)
			{	strcpy(PasteBuff,"User Memory Full.");
			  	return MY_USER_TABLE_FULL;
			}
			else if(loc < MAX_NO_OF_USER)
			{	u.ll = mobno;
				u.c[UT_PP_T_IDp] = pid;
				u.c[CALL_ATTEMPTp] = 0xFF;
				RandomMemWrite(USER_TABLE_START_ADDRESS+(loc*USER_TABLE_RECORD_SIZE),u.c,USER_TABLE_RECORD_SIZE);
				//sprintf(CopyBuff,"\r\nUSER ADDED:%d",loc);//TESTING CODE
				//PutStringUsb(CopyBuff);//TESTING CODE
				return MY_DONE;
			}
		}
	}
	return MY_USER_FOUND;
}*/

void CheckAndInsertDailyRecord()
{
	ui  total_flow = 0, total_cycles = 0, previousCycle=0 , currentCycle=0;
	uli avg_p_volt =0, avg_w_volt =0 , avg_ec=0; 
    ui alarm =0;
	struct tm rec_time_stamp, device_time_stamp;
	boolean_t have2match_time_stamp_flag = FALSE;//To indicate RecordTimestamp has to be matched with TimeStampPtr at the end, //so that it doesn't do same entry multiple times
	ui mem_add;
	ui avg_p_volt_counter = 0;
    ui avg_w_volt_counter = 0;
    ui avg_ec_counter = 0;
	rec_time_stamp = RecordTimeStamp;
	
	_ttemp = TimeStamp2Sec(Device.TimeStamp);
	_ttemp += SecAfterTimeSync;
	TimeStampPtr = localtime(&_ttemp);
	TimeStampPtr->tm_year -= 100;
	TimeStampPtr->tm_mon += 1;
	device_time_stamp = *TimeStampPtr;
	
	if((rec_time_stamp.tm_mday != device_time_stamp.tm_mday) && (rec_time_stamp.tm_mday > 0))//date change
	{	/*
		strcpy(LcdLine1," INSERTING DATA ");
		strcpy(LcdLine2,"Please Wait....."); 
		SendSerialHmiPacket();
		*/
		
		for(mem_add=LogPtr-SECOND_LOG_PACKET_SIZE;mem_add != LogPtr;mem_add-=SECOND_LOG_PACKET_SIZE)
		{	//WATCHDOG
			#ifdef WATCH_DOG_TIMER_ENABLED
                RESET_WATCH_DOG_TIMER;
            #endif
                        
			/* to use below feature, seperate all global project variable to local to file*/
			if(SecondChangeFlag == TRUE){
				SecondChangeFlag = FALSE;
				MAIN_PROG_LED_TOGGLE;
				/*if(Mode == MODE_AUTO){
					CheckKeyPress();
					AutoModeOperations();
					UpdateOutput();
					DisplayMenu();
				}*/
				strcpy(LcdLine1,"Uploading Data!!");
				strcpy(LcdLine2,"Please Wait.....");
				SendSerialHmiPacket();
			}
			if(mem_add < SECOND_LOG_START_ADD)
				mem_add = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
			
			DirectMemRead(mem_add,PasteBuff,SECOND_LOG_PACKET_SIZE);
			
			//date
			u.c[0] = PasteBuff[LT_SECOND_DATEp];
			u.c[1] = PasteBuff[LT_SECOND_DATEp+1];
			u.c[2] = PasteBuff[LT_SECOND_DATEp+2];
			
			temp_timestamp.tm_year = u.c[0];
			temp_timestamp.tm_mon = u.c[1];
			temp_timestamp.tm_mday = u.c[2];
			temp_timestamp.tm_hour = 0;
			temp_timestamp.tm_min = 0;
			temp_timestamp.tm_sec = 0;
			
			_ttemp = TimeStamp2Sec(temp_timestamp);
			u.i[0] = _ttemp;
			
			temp_timestamp.tm_year = rec_time_stamp.tm_year;
			temp_timestamp.tm_mon = rec_time_stamp.tm_mon;
			temp_timestamp.tm_mday = rec_time_stamp.tm_mday;
			temp_timestamp.tm_hour = 0;
			temp_timestamp.tm_min = 0;
			temp_timestamp.tm_sec = 0;
			_ttemp = TimeStamp2Sec(temp_timestamp);
			u.i[1] = _ttemp;
			
			if(u.i[1] - u.i[0] >= 86400)
			{	//PutStringUsb("\r\nRecord Download Complete");
				break;//for loop break;
			}
			
			u.c[0] = PasteBuff[LT_SECOND_DATEp];
			u.c[1] = PasteBuff[LT_SECOND_DATEp+1];
			u.c[2] = PasteBuff[LT_SECOND_DATEp+2];
			
			if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
			{	//PutStringUsb("\r\nRecord Download Complete");
				break;//for loop break;
			}
			
			//cycle
			u.i[0] = 0;//CycleCounter;
			u.c[0] = PasteBuff[LT_SECOND_CYCLEp];// = u.c[0];
			u.c[1] = PasteBuff[LT_SECOND_CYCLEp+1];// = u.c[1];
			currentCycle = u.i[0];
			
			if(previousCycle != currentCycle)
			{	total_cycles++;
				previousCycle = currentCycle;
			}
			//PAE volt
			u.i[0] = 0;//CurrentPAEvolt;
			u.c[0] = PasteBuff[LT_SECOND_PAE_VOLTp];// = u.c[0];
			u.c[1] = PasteBuff[LT_SECOND_PAE_VOLTp+1];// = u.c[1];

			if(PasteBuff[LT_SECOND_STEPp] == PURIFY)
			{	avg_p_volt += u.i[0];
				avg_p_volt_counter++;// /= 2;
                
                        //Avg EC
                        u.i[0] = 0;//AtlasConductivity;
                        u.c[0] = PasteBuff[LT_SECOND_AVERAGE_ECp];// = u.c[0];
                        u.c[1] = PasteBuff[LT_SECOND_AVERAGE_ECp+1];// = u.c[1];
                        u.c[2] = PasteBuff[LT_SECOND_AVERAGE_ECp+2];// = u.c[2];
                        if(u.i[0] > 0) {
                            avg_ec += u.i[0];
                            avg_ec_counter++;// /= 2;
                        }
			}
			else if(PasteBuff[LT_SECOND_STEPp] == WASTE)
			{	avg_w_volt += u.i[0];
				avg_w_volt_counter++;// /= 2;
			}
			
			// Total Flow
			//u.i[0] = AtlasReplacement.TotalFlow;
			u.c[0] = PasteBuff[LT_SECOND_T_FLOWp];// = u.c[0];
			u.c[1] = PasteBuff[LT_SECOND_T_FLOWp+1];// = u.c[1];
			u.c[2] = PasteBuff[LT_SECOND_T_FLOWp+2];// = u.c[2];
			u.c[3] = PasteBuff[LT_SECOND_T_FLOWp+3];// = u.c[3];
			
			if(u.i[0] > total_flow)
			{
				total_flow = u.i[0];
			}
			
                //Alarm
			u.i[0] = 0;
			u.c[0] = PasteBuff[LT_SECOND_ALARMp];// = u.c[0];
			u.c[1] = PasteBuff[LT_SECOND_ALARMp+1];// = u.c[1];
			u.c[2] = PasteBuff[LT_SECOND_ALARMp+2];// = u.c[2];
			alarm |= u.i[0];
		}
		
		//Inserting in DateTable
		PasteBuff[LT_DAILY_DATEp] = rec_time_stamp.tm_year;
		PasteBuff[LT_DAILY_DATEp+1] = rec_time_stamp.tm_mon;
		PasteBuff[LT_DAILY_DATEp+2] = rec_time_stamp.tm_mday;
		
		PasteBuff[LT_DAILY_AVG_P_VOLTp] = avg_p_volt/avg_p_volt_counter;
		
		PasteBuff[LT_DAILY_AVG_W_VOLTp] = avg_w_volt/avg_w_volt_counter;
		
		u.i[0] = total_flow;
		PasteBuff[LT_DAILY_T_FLOWp] = u.c[0];
		PasteBuff[LT_DAILY_T_FLOWp+1] = u.c[1];
		PasteBuff[LT_DAILY_T_FLOWp+2] = u.c[2];
		PasteBuff[LT_DAILY_T_FLOWp+3] = u.c[3];
		
		u.i[0] = avg_ec/avg_ec_counter;
		PasteBuff[LT_DAILY_AVG_ECp] = u.c[0];
		PasteBuff[LT_DAILY_AVG_ECp+1] = u.c[1];
		PasteBuff[LT_DAILY_AVG_ECp+2] = u.c[2];
		
		if(total_cycles > 0)
			total_cycles--;
		u.i[0] = total_cycles;
		PasteBuff[LT_DAILY_T_CYCLEp] = u.c[0];
		PasteBuff[LT_DAILY_T_CYCLEp+1] = u.c[1];
		
        u.i[0] = alarm;
		PasteBuff[LT_DAILY_ALARMp] = u.c[0];
        PasteBuff[LT_DAILY_ALARMp+1] = u.c[1];
        PasteBuff[LT_DAILY_ALARMp+2] = u.c[2];
		
		mem_add = DAILY_LOG_START_ADD+rec_time_stamp.tm_mday*DAILY_LOG_PACKET_SIZE;
		/*
		sprintf(CopyBuff,"\r\nDaily Rec Address = %d & mday=%d",mem_add,rec_time_stamp.tm_mday);
		PutStringUsb(CopyBuff);
		*/
		RandomMemWrite(mem_add,PasteBuff,DAILY_LOG_PACKET_SIZE);  // 0 location will always be vacant
		have2match_time_stamp_flag = TRUE;//To indicate RecordTimestamp has to be matched with TimeStampPtr at the end, //so that it doesn't do same entry multiple times
		DailySummaryReportMailFlag = TRUE;
		sprintf(PasteBuff,"\r\nDaily Report= D:%d/%d/%d, AvgPV=%.2f, AvgWV=%.2f, T.Flow=%.2f, AvgEC=%.2f, T.Cyc=%d, Alarm=%Xh",
				rec_time_stamp.tm_year,
				rec_time_stamp.tm_mon,
				rec_time_stamp.tm_mday,
				(float)avg_p_volt/100,
				(float)avg_w_volt/100,
				(float)total_flow/100,
				(float)avg_ec/100,
				total_cycles,
				alarm);
		PutStringUsb(PasteBuff);
		
	}
	
	/*
	//clear all local variables for date change calculations
	avg_p_volt =0; avg_w_volt =0; total_flow =0; avg_ec=0; total_cycles = 0; previousCycle=0; currentCycle=0;
	alarm =0;
	
	
	if((rec_time_stamp.tm_mon != device_time_stamp.tm_mon) && (rec_time_stamp.tm_mon > 0))//month change
	{	for(mem_add=1;mem_add<32;mem_add++)
		{	//WATCHDOG
			#ifdef WATCH_DOG_TIMER_ENABLED
                RESET_WATCH_DOG_TIMER;
            #endif
                        // to use below feature, seperate all global project variable to local to file
			if(SecondChangeFlag == TRUE){
				SecondChangeFlag = FALSE;
				MAIN_PROG_LED_TOGGLE;
				strcpy(LcdLine1,"Uploading Data!!");
				strcpy(LcdLine2,"Please Wait.....");
				SendSerialHmiPacket();
			}
			DirectMemRead(DAILY_LOG_START_ADD + mem_add*DAILY_LOG_PACKET_SIZE,PasteBuff,DAILY_LOG_PACKET_SIZE);
			
			//date
			u.c[0] = PasteBuff[LT_DAILY_DATEp];
			u.c[1] = PasteBuff[LT_DAILY_DATEp+1];
			u.c[2] = PasteBuff[LT_DAILY_DATEp+2];
			
			if(rec_time_stamp.tm_year != u.c[0] || rec_time_stamp.tm_mon != u.c[1])
				break;
			
			if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
			{	//PutStringUsb("\r\nRecord Download Complete");
				break;//for loop break;
			}
			//cycle
			u.i[0] = 0;//CycleCounter;
			u.c[0] = PasteBuff[LT_DAILY_T_CYCLEp];
			u.c[1] = PasteBuff[LT_DAILY_T_CYCLEp+1];
			total_cycles += u.i[0];
			
			//PAE P volt
			avg_p_volt += PasteBuff[LT_DAILY_AVG_P_VOLTp];
			avg_p_volt /= 2;
			//PAE W volt
			avg_w_volt += PasteBuff[LT_DAILY_AVG_W_VOLTp];
			avg_w_volt /= 2;
			
			// Total Flow
			//u.i[0] = AtlasReplacement.TotalFlow;
			u.c[0] = PasteBuff[LT_DAILY_T_FLOWp];// = u.c[0];
			u.c[1] = PasteBuff[LT_DAILY_T_FLOWp+1];// = u.c[1];
			u.c[2] = PasteBuff[LT_DAILY_T_FLOWp+2];// = u.c[2];
			u.c[3] = PasteBuff[LT_DAILY_T_FLOWp+3];// = u.c[3];
			if(u.i[0] > total_flow)
			{
				total_flow = u.i[0];
			}
			// EC
			u.i[0] = 0;//AtlasConductivity;
			u.c[0] = PasteBuff[LT_DAILY_AVG_ECp];
			u.c[1] = PasteBuff[LT_DAILY_AVG_ECp+1];
			u.c[2] = PasteBuff[LT_DAILY_AVG_ECp+2];
			
			avg_ec += u.i[0];
			avg_ec /= 2;
			//Alarm
			alarm |= PasteBuff[LT_DAILY_ALARMp];
			
		}
		
		//Inserting in MonthTable
		PasteBuff[LT_MONTHLY_DATEp] = rec_time_stamp.tm_year;
		PasteBuff[LT_MONTHLY_DATEp+1] = rec_time_stamp.tm_mon;
		
		PasteBuff[LT_MONTHLY_AVG_P_VOLTp] = avg_p_volt;
		
		PasteBuff[LT_MONTHLY_AVG_W_VOLTp] = avg_w_volt;
		
		u.i[0] = total_flow;
		PasteBuff[LT_MONTHLY_T_FLOWp] = u.c[0];
		PasteBuff[LT_MONTHLY_T_FLOWp+1] = u.c[1];
		PasteBuff[LT_MONTHLY_T_FLOWp+2] = u.c[2];
		PasteBuff[LT_MONTHLY_T_FLOWp+3] = u.c[3];
		
		u.i[0] = avg_ec;
		PasteBuff[LT_MONTHLY_AVG_ECp] = u.c[0];
		PasteBuff[LT_MONTHLY_AVG_ECp+1] = u.c[1];
		PasteBuff[LT_MONTHLY_AVG_ECp+2] = u.c[2];
		
		u.i[0] = total_cycles;
		PasteBuff[LT_MONTHLY_T_CYCLEp] = u.c[0];
		PasteBuff[LT_MONTHLY_T_CYCLEp+1] = u.c[1];
		
		PasteBuff[LT_MONTHLY_ALARMp] = alarm;
		
		mem_add = MONTHLY_LOG_START_ADD+rec_time_stamp.tm_mon*MONTHLY_LOG_PACKET_SIZE;
		RandomMemWrite(MONTHLY_LOG_START_ADD+rec_time_stamp.tm_mon*MONTHLY_LOG_PACKET_SIZE,PasteBuff,MONTHLY_LOG_PACKET_SIZE);  // 0 location will always be vacant
		have2match_time_stamp_flag = TRUE;//To indicate RecordTimestamp has to be matched with TimeStampPtr at the end, //so that it doesn't do same entry multiple times
		MonthlySummaryReportMailFlag = TRUE;
		sprintf(PasteBuff,"\r\nMonthly Report= D:%d/%d, AvgPV=%.2f, AvgWV=%.2f, T.Flow=%.2f, AvgEC=%.2f, T.Cyc=%d, Alarm=%Xh",
				rec_time_stamp.tm_year,
				rec_time_stamp.tm_mon,
				(float)avg_p_volt/100,
				(float)avg_w_volt/100,
				(float)total_flow/100,
				(float)avg_ec/100,
				total_cycles,
				alarm);
		PutStringUsb(PasteBuff);
		
	}
	*/
	/*
	//clear all local variables for date change calculations
	avg_p_volt =0; avg_w_volt =0; total_flow =0; avg_ec=0; total_cycles = 0; previousCycle=0; currentCycle=0;
	alarm =0;
	
	if((rec_time_stamp.tm_year != device_time_stamp.tm_year) && (rec_time_stamp.tm_year > 0))//YEAR  change
	{	for(mem_add=1;mem_add<13;mem_add++)
		{	//WATCHDOG
			#ifdef WATCH_DOG_TIMER_ENABLED
                RESET_WATCH_DOG_TIMER;
            #endif
            // to use below feature, seperate all global project variable to local to file
			if(SecondChangeFlag == TRUE){
				SecondChangeFlag = FALSE;
				MAIN_PROG_LED_TOGGLE;
				strcpy(LcdLine1,"Uploading Data!!");
				strcpy(LcdLine2,"Please Wait.....");
				SendSerialHmiPacket();
			}
			DirectMemRead(MONTHLY_LOG_START_ADD + mem_add*MONTHLY_LOG_PACKET_SIZE,PasteBuff,MONTHLY_LOG_PACKET_SIZE);
			
			//date
			u.c[0] = PasteBuff[LT_MONTHLY_DATEp];
			u.c[1] = PasteBuff[LT_MONTHLY_DATEp+1];
			
			if(rec_time_stamp.tm_year != u.c[0])
				continue;
			
			if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
			{	//PutStringUsb("\r\nRecord Download Complete");
				break;//for loop break;
			}
			//cycle
			u.i[0] = 0;//CycleCounter;
			u.c[0] = PasteBuff[LT_MONTHLY_T_CYCLEp];
			u.c[1] = PasteBuff[LT_MONTHLY_T_CYCLEp+1];
			total_cycles += u.i[0];
			
			//PAE P volt
			avg_p_volt += PasteBuff[LT_MONTHLY_AVG_P_VOLTp];
			avg_p_volt /= 2;
			//PAE W volt
			avg_w_volt += PasteBuff[LT_MONTHLY_AVG_W_VOLTp];
			avg_w_volt /= 2;
			
			// Total Flow
			//u.i[0] = AtlasReplacement.TotalFlow;
			u.c[0] = PasteBuff[LT_MONTHLY_T_FLOWp];// = u.c[0];
			u.c[1] = PasteBuff[LT_MONTHLY_T_FLOWp+1];// = u.c[1];
			u.c[2] = PasteBuff[LT_MONTHLY_T_FLOWp+2];// = u.c[2];
			u.c[3] = PasteBuff[LT_MONTHLY_T_FLOWp+3];// = u.c[3];
			if(u.i[0] > total_flow)
			{
				total_flow = u.i[0];
			}
			// EC
			u.i[0] = 0;//AtlasConductivity;
			u.c[0] = PasteBuff[LT_MONTHLY_AVG_ECp];
			u.c[1] = PasteBuff[LT_MONTHLY_AVG_ECp+1];
			u.c[2] = PasteBuff[LT_MONTHLY_AVG_ECp+2];
			
			avg_ec += u.i[0];
			avg_ec /= 2;
			//Alarm
			alarm |= PasteBuff[LT_MONTHLY_ALARMp];
			
		}
		
		//Inserting in Year Table
		PasteBuff[LT_YEARLYp] = rec_time_stamp.tm_year;
		
		PasteBuff[LT_YEARLY_AVG_P_VOLTp] = avg_p_volt;
		
		PasteBuff[LT_YEARLY_AVG_W_VOLTp] = avg_w_volt;
		
		u.i[0] = total_flow;
		PasteBuff[LT_YEARLY_T_FLOWp] = u.c[0];
		PasteBuff[LT_YEARLY_T_FLOWp+1] = u.c[1];
		PasteBuff[LT_YEARLY_T_FLOWp+2] = u.c[2];
		PasteBuff[LT_YEARLY_T_FLOWp+3] = u.c[3];
		
		u.i[0] = avg_ec;
		PasteBuff[LT_YEARLY_AVG_ECp] = u.c[0];
		PasteBuff[LT_YEARLY_AVG_ECp+1] = u.c[1];
		PasteBuff[LT_YEARLY_AVG_ECp+2] = u.c[2];
		
		u.i[0] = total_cycles;
		PasteBuff[LT_YEARLY_T_CYCLEp] = u.c[0];
		PasteBuff[LT_YEARLY_T_CYCLEp+1] = u.c[1];
		PasteBuff[LT_YEARLY_T_CYCLEp+2] = u.c[2];
		
		PasteBuff[LT_YEARLY_ALARMp] = alarm;
		
		mem_add = YEARLY_LOG_START_ADD+rec_time_stamp.tm_year*YEARLY_LOG_PACKET_SIZE;
		RandomMemWrite(YEARLY_LOG_START_ADD+rec_time_stamp.tm_year*YEARLY_LOG_PACKET_SIZE,PasteBuff,YEARLY_LOG_PACKET_SIZE);  // 0 location will always be vacant
		have2match_time_stamp_flag = TRUE;//To indicate RecordTimestamp has to be matched with TimeStampPtr at the end, //so that it doesn't do same entry multiple times
		YearlySummaryReportMailFlag = TRUE;
		sprintf(PasteBuff,"\r\nYearly Report= D:%d, AvgPV=%.2f, AvgWV=%.2f, T.Flow=%.2f, AvgEC=%.2f, T.Cyc=%d, Alarm=%Xh",
				rec_time_stamp.tm_year,
				(float)avg_p_volt/100,
				(float)avg_w_volt/100,
				(float)total_flow/100,
				(float)avg_ec/100,
				total_cycles,
				alarm);
		PutStringUsb(PasteBuff);
		
	}
	*/
	if(have2match_time_stamp_flag == TRUE)
	{	RecordTimeStamp = device_time_stamp;//so that it doesn't do same entry multiple times
		if(EmailInitializationCompleteFlag == TRUE)
            SendMailFlag = TRUE;
	}
}
void ClearAlarmDate()
{	PageToBuffer(ALARM_TABLE_START_PAGE,PAGE_TO_BUFF1);
	PasteBuff[0] = 1;
	PasteBuff[1] = 1;
	PasteBuff[2] = 1;
	PasteBuff[3] = 0;
	PasteBuff[4] = 0;
	PasteBuff[5] = 0;
	
	for(ucTemp=0;ucTemp<MAX_NO_OF_ALARMS;ucTemp++)
	{	//RandomMemWrite(ALARM_TABLE_START_ADDRESS+ucTemp*ALARM_TABLE_RECORD_SIZE,PasteBuff,ALARM_TABLE_RECORD_SIZE);
		ReadWriteMemBuffer(MEM_WRITE,ALARM_TABLE_START_ADDRESS+ucTemp*ALARM_TABLE_RECORD_SIZE, PasteBuff, ALARM_TABLE_RECORD_SIZE);
		DelayInMilliSec(1);
	}
	BufferToPage(BUFF1_TO_PAGE,ALARM_TABLE_START_PAGE);
}
void ClearDateTable()
{	PasteBuff[LT_DAILY_DATEp]=0;
	PasteBuff[LT_DAILY_DATEp+1]=1;
	PasteBuff[LT_DAILY_DATEp+2] = 0;
	PasteBuff[LT_DAILY_T_CYCLEp] = 0;
	PasteBuff[LT_DAILY_T_CYCLEp+1] = 0;
	PasteBuff[LT_DAILY_AVG_P_VOLTp] = 0;
	PasteBuff[LT_DAILY_AVG_W_VOLTp] = 0;
	PasteBuff[LT_DAILY_T_FLOWp] = 0;
	PasteBuff[LT_DAILY_T_FLOWp+1] = 0;
	PasteBuff[LT_DAILY_T_FLOWp+2] = 0;
	PasteBuff[LT_DAILY_T_FLOWp+3] = 0;
	PasteBuff[LT_DAILY_AVG_ECp] = 0;
	PasteBuff[LT_DAILY_AVG_ECp+1] = 0;
	PasteBuff[LT_DAILY_AVG_ECp+2] = 0;
	PasteBuff[LT_DAILY_ALARMp] = 0;
	for(ucTemp=0;ucTemp<32;ucTemp++)
	{	RandomMemWrite(DAILY_LOG_START_ADD+ucTemp*DAILY_LOG_PACKET_SIZE,PasteBuff,DAILY_LOG_PACKET_SIZE);  
		DelayInMilliSec(10);	
	}
}
/*
void ClearMonthTable()
{	PasteBuff[LT_MONTHLY_DATEp]=0;
	PasteBuff[LT_MONTHLY_DATEp+1]=1;
	PasteBuff[LT_MONTHLY_T_CYCLEp] = 0;
	PasteBuff[LT_MONTHLY_T_CYCLEp+1] = 0;
	PasteBuff[LT_MONTHLY_T_CYCLEp+2] = 0;
	PasteBuff[LT_MONTHLY_AVG_P_VOLTp] = 0;
	PasteBuff[LT_MONTHLY_AVG_W_VOLTp] = 0;
	PasteBuff[LT_MONTHLY_T_FLOWp] = 0;
	PasteBuff[LT_MONTHLY_T_FLOWp+1] = 0;
	PasteBuff[LT_MONTHLY_T_FLOWp+2] = 0;
	PasteBuff[LT_MONTHLY_T_FLOWp+3] = 0;
	PasteBuff[LT_MONTHLY_AVG_ECp] = 0;
	PasteBuff[LT_MONTHLY_AVG_ECp+1] = 0;
	PasteBuff[LT_MONTHLY_AVG_ECp+2] = 0;
	PasteBuff[LT_MONTHLY_ALARMp] = 0;
	for(ucTemp=0;ucTemp<13;ucTemp++)
	{	RandomMemWrite(MONTHLY_LOG_START_ADD+ucTemp*MONTHLY_LOG_PACKET_SIZE,PasteBuff,MONTHLY_LOG_PACKET_SIZE);  
		DelayInMilliSec(10);	
	}
}
void ClearYearTable()
{	PasteBuff[LT_YEARLYp]=0;
	PasteBuff[LT_YEARLY_T_CYCLEp] = 0;
	PasteBuff[LT_YEARLY_T_CYCLEp+1] = 0;
	PasteBuff[LT_YEARLY_T_CYCLEp+2] = 0;
	PasteBuff[LT_YEARLY_AVG_P_VOLTp] = 0;
	PasteBuff[LT_YEARLY_AVG_W_VOLTp] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+1] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+2] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+3] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+4] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+5] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+6] = 0;
	PasteBuff[LT_YEARLY_T_FLOWp+7] = 0;
	PasteBuff[LT_YEARLY_AVG_ECp] = 0;
	PasteBuff[LT_YEARLY_AVG_ECp+1] = 0;
	PasteBuff[LT_YEARLY_AVG_ECp+2] = 0;
	for(ucTemp=0;ucTemp<100;ucTemp++)
	{	RandomMemWrite(YEARLY_LOG_START_ADD+ucTemp*YEARLY_LOG_PACKET_SIZE,PasteBuff,YEARLY_LOG_PACKET_SIZE);  
		DelayInMilliSec(10);	
	}
}
*/
void ConfigureVariable()
{	if(ReadIdentification()!=0)
	{  	PutStringUsb("\r\nMemory Error");
		while(1)
		{  	if(ReadIdentification()!=0)
			{  	PutStringUsb("\r\nMemory Error");
				RESET_DEVICE;
			}
			else
			  break;
		}
	}
	//check configuration
	DirectMemRead(SETTING_STATUS_REGa,u.c,SETTING_STATUS_REGn);
	if(u.c[0] != 0x55)	//configuration available in mem
	{	//configuration is not available in mem, may be its new
		//PutStringUsb("\r\nConfiguration NOT available");
		InitializeDefaultSetting();
		RandomMemWrite(SUPER_ADMIN_EMAILa,"nabeen.amatya@gmail.com",SUPER_ADMIN_EMAILn);
		u.ll = SUPER_ADMIN_DEFAULT_MOBILE_NO;
		RandomMemWrite(USER_SUPER_ADMIN_MOBa,u.c,USER_TABLE_RECORD_SIZE);
		u.ll = 0;
		RandomMemWrite(USER_ADMIN_MOBa,u.c,USER_TABLE_RECORD_SIZE);
	}
	/*DirectMemRead(D_SERIAL_NOa,u.c,D_SERIAL_NOn);
	sprintf(PasteBuff,"\r\nHello Nabeen Lal Amatya\n\rModel: %s\r\nS.No: %d\r\nV: %s",
						MODEL_NO,
						u.i[0],
						DEVICE_VERSION);
	PutStringUsb(PasteBuff);
	*/
	LoadSetting();
	
	//CallPtr = USER_OTHERS_MOBa;
}
////////////////////////////////////////////////////////////////////////////////
void DeleteAllUsers()
{	DirectMemRead(USER_TABLE_START_ADDRESS,PasteBuff,USER_TABLE_RECORD_SIZE*2);//GET SUPER ADMIN AND ADMIN INFO
	for(uiTemp=USER_TABLE_RECORD_SIZE*2;uiTemp<BYTE_PER_PAGE;uiTemp++)//DELETE AFTER 2 USER
	{	PasteBuff[uiTemp] = 0x00;
	}
	RandomMemWrite(USER_TABLE_START_ADDRESS,PasteBuff,BYTE_PER_PAGE);
	
	for(uiTemp=0;uiTemp<BYTE_PER_PAGE;uiTemp++)
	{	PasteBuff[uiTemp] = 0x00;
	}
	RandomMemWrite(USER_TABLE_START_ADDRESS+BYTE_PER_PAGE,PasteBuff,BYTE_PER_PAGE);
	RandomMemWrite(USER_TABLE_START_ADDRESS+BYTE_PER_PAGE*2,PasteBuff,BYTE_PER_PAGE);
}
////////////////////////////////////////////////////////////////////////////////

char DeleteUser(long long mobno,char pid)
{	/*long long userwithpid;
	char deleteflag;
	u.ll = mobno,
	u.c[UT_PP_T_IDp] = pid;
	userwithpid = u.ll;
	cFunRetVal = 0;
  	for(uiTemp=3;uiTemp<MAX_NO_OF_USER;uiTemp++)
	{	deleteflag = FALSE;
		u.ll = 0;
	  	DirectMemRead(USER_TABLE_START_ADDRESS+uiTemp*USER_TABLE_RECORD_SIZE,u.c,USER_TABLE_RECORD_SIZE);
		u.c[CALL_ATTEMPTp] = 0;
		if(mobno==0)	//DELETE ALL USER MATCHING PID
		{	if(pid == u.c[UT_PP_T_IDp])
			{	deleteflag = TRUE;
			}
		}
		else if(pid == 0)//DELETE ALL USER MATCHING MOBILE NO
		{	u.c[UT_PP_T_IDp] = 0;
			if(u.ll == mobno)
			  deleteflag = TRUE;
		}
		else if(u.ll == userwithpid)//SPECIFIC USER WITH SPECIFIC PID
		{	deleteflag = TRUE;
		}
		if(deleteflag == TRUE)
		{	u.ll = 0xFFFFFFFFFFFFFFFF;
			RandomMemWrite(USER_TABLE_START_ADDRESS+uiTemp*USER_TABLE_RECORD_SIZE,u.c,USER_TABLE_RECORD_SIZE);
			cFunRetVal++;
		}
  	}*/
	return cFunRetVal;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
void GetAllUsers()
{				strcpy(PasteBuff,"User Details:");
				for(uiTemp=3;uiTemp<MAX_NO_OF_USER;uiTemp++)
				{	FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
					FM3_SWWDT->WDOGINTCLR = 0x11111111;
					FM3_SWWDT->WDOGLOCK = 0;
					
					u.ll = 0;
					DirectMemRead(USER_TABLE_START_ADDRESS+(uiTemp*USER_TABLE_RECORD_SIZE),u.c,USER_TABLE_RECORD_SIZE);
					cTemp = u.c[UT_PP_T_IDp];
					u.c[UT_PP_T_IDp] = 0;
					u.c[CALL_ATTEMPTp] = 0;
					if(u.ll > 0 && u.ll < 0x0000FFFFFFFFFFFF)	//0xffffffffffffff is always > than 200000000000000
					{	sprintf(CopyBuff,"\r\n+%lld:%s",u.ll,strcatPid(cTemp));
						strcat(PasteBuff,CopyBuff);
					}
					if(strlen(PasteBuff)>500)
					{	PutStringUsb(PasteBuff);
						strcpy(PasteBuff,"");
					}
				}
				PutStringUsb(PasteBuff);
				
}*/
void InitializeDefaultSetting()
{	PageToBuffer(SERVER_PAGE,PAGE_TO_BUFF1);
#if(COUNTRY==NEPAL)
	strcpy(PasteBuff,"ntwap");//BY DEFAULT AIRTEL
#else //BY DEFAULT
	//strcpy(PasteBuff,"airtelgprs.com");//AIRTEL
	//strcpy(PasteBuff,"rcomnet");//RELIANCE
        strcpy(PasteBuff,"");
#endif
	ReadWriteMemBuffer(MEM_WRITE,APNa,PasteBuff,APNn);
	
	u.c[0] = AS_A_CLIENT;
	ReadWriteMemBuffer(MEM_WRITE,SERVER_CLIENTa,u.c,SERVER_CLIENTn);
	
	//MinRecPerConnection = 1;
	u.c[0] = 1;
	ReadWriteMemBuffer(MEM_WRITE,MIN_REC_PER_CONNECTIONa,u.c,MIN_REC_PER_CONNECTIONn);
	
	//call-gsm-gprs control
	u.c[0] = (CALL_CONTROL | SMS_CONTROL | GPRS_CONTROL | EMAIL_CONTROL);
  	ReadWriteMemBuffer(MEM_WRITE,GPRS_CRa,u.c,GPRS_CRn);
	
        //email default configuration
        ReadWriteMemBuffer(MEM_WRITE,EMAIL_USERNAMEa,"0993b74745796f945e8778994619ad6f",EMAIL_USERNAMEn);
	ReadWriteMemBuffer(MEM_WRITE,EMAIL_PASSWORDa,"98dd545c5932ca902e74cba4debd1fb5",EMAIL_PASSWORDn);
	ReadWriteMemBuffer(MEM_WRITE,EMAIL_SENDER_EMAIL_ADDRESSa,"data@voltea.com",EMAIL_SENDER_EMAIL_ADDRESSn);
	ReadWriteMemBuffer(MEM_WRITE,EMAIL_OUTGOING_EMAIL_SERVERa,"in-v3.mailjet.com",EMAIL_OUTGOING_EMAIL_SERVERn);
        u.i[0] = 587;
	ReadWriteMemBuffer(MEM_WRITE,EMAIL_OUTGOING_EMAIL_SERVER_PORTa,u.c,EMAIL_OUTGOING_EMAIL_SERVER_PORTn);
    
        //server details
	for(uiTemp=0;uiTemp<FLASH_PAGE_SIZE;uiTemp++)
	  	PasteBuff[uiTemp]=0;
	ReadWriteMemBuffer(MEM_WRITE,SERVER_START_ADDRESS,PasteBuff,SERVER_RECORD_BYTE_SIZE);
	BufferToPage(BUFF1_TO_PAGE,SERVER_PAGE);
	
	/**************************************************************************/
	/**************************************************************************/
	//PERIODIC SAVE REQUIRED PAGE
	CIP_PumpRunTimeInSec = 0;
	CycleCounter = 0;
	
	OptimizedPAE_CurrentForPrepureStep = 1000;// OptimizedPAE_CurrentForPureStep. Divide it by 100
	OptimizedPAE_CurrentForPureStep = 1000;// OptimizedPAE_CurrentForPureStep. Divide it by 100
	OptimizedPAE_CurrentForWasteStep = 1000;// OptimizedPAE_CurrentForWasteStep. Divide it by 100
	LastCyclePureAverageConductivity = 0;
	PureVolumeOfBatchJustCompleted = 0;
	WasteVolumeOfBatchJustCompleted = 0;
	
	LogPtr = SECOND_LOG_START_ADD;
        UploadPtr = LogPtr;
	EmailPtr = LogPtr;
        UploadPtr2Increment = 0;
        PercentageRecovery[0]=PercentageRecovery[1]=PercentageRecovery[2]=PercentageRecovery[3]=PercentageRecovery[4]=0;
        LastFiveCyclePureAverageConductivitySamples[0]=LastFiveCyclePureAverageConductivitySamples[1]=LastFiveCyclePureAverageConductivitySamples[2]=LastFiveCyclePureAverageConductivitySamples[3]=LastFiveCyclePureAverageConductivitySamples[4]=0;
	LastFiveCyclePureAverageConductivityAverage = 0;
    
        Mode = MODE_LOGOUT;
        CurrentProcessStep = FREE_RUN_SEC;
        ProcessStepRunSec = 0;
        PeriodicSave();
	/**************************************************************************/
	DeleteAllUsers();
	/**************************************************************************/
	//--------------------------------------------------------------------------
	/*
	//project parameter 1 : Time
	PidMCR[MY_PID_TIME/8] |= (1<<(MY_PID_TIME%8));	//enable
	LogVal.i = 180;	//log after every 3 MIN
	LogUnit = UNIT_SEC;
	TrigPriority = TRIG_PRIORITY_NORMAL;
	TrigCondition = TRIG_CONDITION_NONE;
	TrigPara1Val.i = 90;
	//TrigPara1Unit = UNIT_DAYS;
	TrigPara2Val.i = 0;
	//TrigPara2Unit = UNIT_NONE;
	TrigPara3Val.i = 0;
	//TrigPara3Unit = UNIT_NONE;
	strcpy(TrigMsg,"SERVICE TIME TRIGGER");
	//strcpy(PasteBuff,"\r\nInitializing Parameter TIME");
	//PutStringUsb(PasteBuff);
	UpdateProjectParameter2Memory(MY_PID_TIME);
	//project parameter 5 : EXT BAT
	PidMCR[MY_PID_EXT_BATTERY/8] |= (1<<(MY_PID_EXT_BATTERY%8));	//enable
	LogVal.i = 100;	
	LogUnit = UNIT_MV;
	TrigPriority = TRIG_PRIORITY_NORMAL;
	TrigCondition = TRIG_CONDITION_SMALLER_THAN;
	TrigPara1Val.i = 10000;
	//TrigPara1Unit = UNIT_MV;
	TrigPara2Val.i = 0;
	//TrigPara2Unit = UNIT_NONE;
	TrigPara3Val.i = 0;	//max voltage that can come in analog port pin
	//TrigPara3Unit = UNIT_NONE;
	strcpy(TrigMsg,"EXTERNAL BATTERY CUT OFF");
	//strcpy(PasteBuff,"\r\nInitializing Parameter EXTERNAL BATTERY");
	//PutStringUsb(PasteBuff);
	UpdateProjectParameter2Memory(MY_PID_EXT_BATTERY);
	//project parameter 6 : AI1
	PidMCR[MY_PID_AI1/8] |= (1<<(MY_PID_AI1%8));	//enable
	LogVal.i = 100;	
	LogUnit = UNIT_MV;
	TrigPriority = TRIG_PRIORITY_NORMAL;
	TrigCondition = TRIG_CONDITION_CHANGE_GREATER_THAN;
	TrigPara1Val.i = 1000;	
	//TrigPara1Unit = UNIT_MV;
	TrigPara2Val.i = 36000;		//min voltage that can come in analog port pin
	//TrigPara2Unit = UNIT_MV;
	TrigPara3Val.i = 0;			//max voltage that can come in analog port pin
	//TrigPara3Unit = UNIT_MV;
	strcpy(TrigMsg,"FUEL THEFT OR FILL OCCURRED.");
	//strcpy(PasteBuff,"\r\nInitializing Parameter ANALOG INPUT 1");
	//PutStringUsb(PasteBuff);
	UpdateProjectParameter2Memory(MY_PID_AI1);
	
	//project parameter 8-11 : DI1-DI4
	for(uiTemp = MY_PID_DI_START;uiTemp<=MY_PID_DI_END;uiTemp++)
	{	PidMCR[uiTemp/8] |= (1<<(uiTemp%8));	//enable
		TrigPriority = TRIG_PRIORITY_NORMAL;
		TrigCondition = TRIG_CONDITION_NONE;
		sprintf(TrigMsg,"DIGITAL INPUT %d TRIGGER",uiTemp-MY_PID_DI_START+1);
		//sprintf(PasteBuff,"\r\nInitializing Parameter DIGITAL INPUT %d",uiTemp-MY_PID_DI_START+1);
		//PutStringUsb(PasteBuff);
		UpdateProjectParameter2Memory(uiTemp);
	}
	
	//--------------------------------------------------------------------------
	//input page
	PageToBuffer(INPUT_TABLE_START_PAGE,PAGE_TO_BUFF1);
	for(ucTemp=0;ucTemp<MAX_NO_OF_INPUT;ucTemp++)
	{	Input[ucTemp].RegisterBitId = ucTemp;
		switch(ucTemp)
		{	case 0: strcpy(CopyBuff,"IGNITION");break;
			case 1: strcpy(CopyBuff,"AC");break;
			case 2: strcpy(CopyBuff,"DOOR");break;
			case 3: strcpy(CopyBuff,"P.SWITCH");break;
			default: strcpy(CopyBuff,"");break;
		}
		CopyBuff[IT_REG_BIT_ADDRESSp] = Input[ucTemp].RegisterBitId;
		ReadWriteMemBuffer(MEM_WRITE,INPUT_TABLE_START_ADDRESS+ucTemp*INPUT_TABLE_RECORD_SIZE,CopyBuff,INPUT_TABLE_RECORD_SIZE);
	}
	BufferToPage(BUFF1_TO_PAGE,INPUT_TABLE_START_PAGE);
	*/
	//--------------------------------------------------------------------------
	ClearAlarmDate();
	AlarmRegister24Bit = 0x000000;
	AlarmRegisterForSMS = 0x000000;
	ClearDateTable();
	//ClearMonthTable();
	//ClearYearTable();
	//--------------------------------------------------------------------------
	//page SYSTEM SETTING START PAGE
	PageToBuffer(SYSTEM_SETTING_START_PAGE,PAGE_TO_BUFF1);
	
	u.c[0] = FIRMWARE_NOT_AVAILABLE;
        ReadWriteMemBuffer(MEM_WRITE,FOTA_STATUSa,u.c,FOTA_STATUSn);
	
	//ReadWriteMemBuffer(MEM_WRITE,ADMIN_EMAILa,"",ADMIN_EMAILn);
	
	//serial no is to be programmed after configuration
	
	strcpy(Device.Password,FACTORY_SET_DEVICE_PASSWORD);
	ReadWriteMemBuffer(MEM_WRITE,DEVICE_PASSWORDa,Device.Password,DEVICE_PASSWORDn);
	
	//admin, user, car & driver details to be programmed after configuration
	//product name is default product registration no
	ReadWriteMemBuffer(MEM_WRITE,PRODUCT_REG_NOa,PRODUCT_NAME,PRODUCT_REG_NOn);
	
	
	u.i[0] = 0;//GMT
	Device.TimeZone = u.i[0];
	ReadWriteMemBuffer(MEM_WRITE,TIME_ZONEa,u.c,TIME_ZONEn);

	//update pid master control register
	//ReadWriteMemBuffer(MEM_WRITE,PROJ_PARA_MCRa,PidMCR,PROJ_PARA_MCRn);

	//SWITCH OFF all output as default
	DigitalOutput = 0xffff;
        u.i[0] = DigitalOutput;
	ReadWriteMemBuffer(MEM_WRITE,LAST_OUTPUT_STATUSa,u.c,LAST_OUTPUT_STATUSn);
	
	//call action
	u.c[0] = CALL_ACTION_SMS_LOCATION;
	ReadWriteMemBuffer(MEM_WRITE,ADMIN_CALL_ACTIONa,u.c,ADMIN_CALL_ACTIONn);
	ReadWriteMemBuffer(MEM_WRITE,OPERATOR_CALL_ACTIONa,u.c,OPERATOR_CALL_ACTIONn);
	ReadWriteMemBuffer(MEM_WRITE,USER_CALL_ACTIONa,u.c,USER_CALL_ACTIONn);
	//balance check number
	strcpy(CopyBuff,"POST");
	ReadWriteMemBuffer(MEM_WRITE,BALANCE_CHECK_NOa,CopyBuff,BALANCE_CHECK_NOn);
	
	//project parameter starts
	#ifdef	DEBUG_MODE_TIMINGS
		u.c[0] = 5;//Prepurify Time
		ReadWriteMemBuffer(MEM_WRITE,PREPURIFY_TIME_IN_SECa,u.c,PREPURIFY_TIME_IN_SECn);
		u.i[0] = 10;//Purify Time
		ReadWriteMemBuffer(MEM_WRITE,PURIFY_TIME_IN_SECa,u.c,PURIFY_TIME_IN_SECn);
		u.i[0] = 7;//Waste time
		ReadWriteMemBuffer(MEM_WRITE,WASTE_TIME_IN_SECa,u.c,WASTE_TIME_IN_SECn);
		u.i[0] = 2;//HF Waste time
		ReadWriteMemBuffer(MEM_WRITE,HF_WASTE_TIME_IN_SECa,u.c,HF_WASTE_TIME_IN_SECn);
		u.c[0] = 4;//CIP Dosing On Time = CIP Step 1A
		ReadWriteMemBuffer(MEM_WRITE,CIP_DOSING_ON_TIME_IN_SECa,u.c,CIP_DOSING_ON_TIME_IN_SECn);
		u.i[0] = 9;//CIP Dosing Off Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_DOSING_OFF_TIME_IN_SECa,u.c,CIP_DOSING_OFF_TIME_IN_SECn);
		u.i[0] = 117;//CIP Dosing Total Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_DOSING_TOTAL_TIME_IN_SECa,u.c,CIP_DOSING_TOTAL_TIME_IN_SECn);
		u.i[0] = 180;//CIP Flow Pulse Total Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa,u.c,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn);
		u.c[0] = 6;//CIP Flow Pulse On Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLOW_PULSE_ON_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_ON_TIME_IN_SECn);
		u.c[0] = 20;//CIP Flow Pulse Off Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLOW_PULSE_OFF_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_OFF_TIME_IN_SECn);
		u.i[0] = 18;//CIP Flush Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLUSH_TIME_IN_SECa,u.c,CIP_FLUSH_TIME_IN_SECn);
		u.c[0] = 15;//WV Check Time
		ReadWriteMemBuffer(MEM_WRITE,WV_CHECK_TIME_IN_SECa,u.c,WV_CHECK_TIME_IN_SECn);
		u.i[0] = 60;//Wait HT Time
		ReadWriteMemBuffer(MEM_WRITE,WAIT_HT_TIME_IN_SECa,u.c,WAIT_HT_TIME_IN_SECn);
        #if (PRODUCT_TYPE == PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
            u.c[0] = 26;//Target Pure Flow for DiUse. Divide it by 10 to get actual value
        #else
            u.c[0] = 55;////Target Pure Flow for DiEntry. Divide it by 10 to get actual value
        #endif
        ReadWriteMemBuffer(MEM_WRITE,TARGET_PURE_FLOW_LITER_PER_MINa,u.c,TARGET_PURE_FLOW_LITER_PER_MINn);
		u.i[0] = 100;//Outlet Conductivity
		ReadWriteMemBuffer(MEM_WRITE,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECa,u.c,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECn);
		u.c[0] = 10;//Low Flow Purify Alarm. Divide it by 10 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,LOW_FLOW_PURIFY_ALARM_SET_POINTa,u.c,LOW_FLOW_PURIFY_ALARM_SET_POINTn);
		u.c[0] = 3;//Low Flow waste Alarm. Divide it by 10 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,LOW_FLOW_WASTE_ALARM_SET_POINTa,u.c,LOW_FLOW_WASTE_ALARM_SET_POINTn);
		u.i[0] = 5;//CIP Cycles
		ReadWriteMemBuffer(MEM_WRITE,MAX_CYCLE_RUNCOUNT_FOR_CIPa,u.c,MAX_CYCLE_RUNCOUNT_FOR_CIPn);
		u.c[0] = 40;//Temperature alarm High Setpoint
		ReadWriteMemBuffer(MEM_WRITE,CABINET_HIGH_TEMPERATURE_SET_VALa,u.c,CABINET_HIGH_TEMPERATURE_SET_VALn);
		u.i[0] = 930;//Max CIP Pump Runtime
		ReadWriteMemBuffer(MEM_WRITE,MAX_CIP_PUMP_RUNTIMEa,u.c,MAX_CIP_PUMP_RUNTIMEn);
        u.i[0] = 40;//Pump P-factor. Divide it by 100 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,PUMP_P_FACTORa,u.c,PUMP_P_FACTORn);
		u.i[0] = 5;//Dynamic P-factor. Divide it by 100 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,DYNAMIC_P_FACTORa,u.c,DYNAMIC_P_FACTORn);
		u.i[0] = 15;// Max Voltage PrePure. Divite it by 10
		ReadWriteMemBuffer(MEM_WRITE,PAE_PREPURIFY_CYCLE_VOLTAGEa,u.c,PAE_PURE_CYCLE_VOLTAGEn);
		u.i[0] = 25;// Max Voltage Pure. Divite it by 10
		ReadWriteMemBuffer(MEM_WRITE,PAE_PURE_CYCLE_VOLTAGEa,u.c,PAE_PURE_CYCLE_VOLTAGEn);
		u.i[0] = 25;// Max Voltage Waste. Divite it by 10
		ReadWriteMemBuffer(MEM_WRITE,PAE_WASTE_CYCLE_VOLTAGEa,u.c,PAE_WASTE_CYCLE_VOLTAGEn);
		u.i[0] = 105;// waste value. Divide it by 100
		ReadWriteMemBuffer(MEM_WRITE,WASTE_VALUE_CURRENTa,u.c,WASTE_VALUE_CURRENTn);
		u.i[0] = 600;// Flow meter K factor from range 10 to 2000. 600 to match with frequency generator easy calc
		ReadWriteMemBuffer(MEM_WRITE,FLOWMETER_K_FACTORa,u.c,FLOWMETER_K_FACTORn);
        u.c[0] = VOLUME_UNIT_GALLON;
        ReadWriteMemBuffer(MEM_WRITE,VOLUME_UNITa,u.c,VOLUME_UNITn);
        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                u.c[0] = BYPASS_INTERNAL;
                ReadWriteMemBuffer(MEM_WRITE,BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
                u.c[0] = FALSE;
                ReadWriteMemBuffer(MEM_WRITE,TANK_FULLa,u.c,TANK_FULLn);
        #endif
	#else
		u.c[0] = PREPURIFY_TIME_DEF_VAL;//Prepurify Time
		ReadWriteMemBuffer(MEM_WRITE,PREPURIFY_TIME_IN_SECa,u.c,PREPURIFY_TIME_IN_SECn);
		
        u.i[0] = PURIFY_TIME_DEF_VAL;//Purify Time
		ReadWriteMemBuffer(MEM_WRITE,PURIFY_TIME_IN_SECa,u.c,PURIFY_TIME_IN_SECn);
		
        u.i[0] = WASTE_TIME_DEF_VAL;//Waste time
		ReadWriteMemBuffer(MEM_WRITE,WASTE_TIME_IN_SECa,u.c,WASTE_TIME_IN_SECn);
		
        u.i[0] = HF_WASTE_TIME_DEF_VAL;//HF Waste time
		ReadWriteMemBuffer(MEM_WRITE,HF_WASTE_TIME_IN_SECa,u.c,HF_WASTE_TIME_IN_SECn);
		
        u.i[0] = CIP_DOSE_ON_TIME_DEF_VAL;//CIP Dosing On Time = CIP Step 1A/CIP Dose
		ReadWriteMemBuffer(MEM_WRITE,CIP_DOSING_ON_TIME_IN_SECa,u.c,CIP_DOSING_ON_TIME_IN_SECn);
		
        u.i[0] = CIP_DOSE_OFF_TIME_DEF_VAL;//CIP Dosing Off Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_DOSING_OFF_TIME_IN_SECa,u.c,CIP_DOSING_OFF_TIME_IN_SECn);
        
        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
        u.i[0] = CIP_DOSE_TOTAL_TIME_DEF_VAL;//CIP Dosing Total Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_DOSING_TOTAL_TIME_IN_SECa,u.c,CIP_DOSING_TOTAL_TIME_IN_SECn);
		
        u.i[0] = CIP_PULSE_TOTAL_TIME_DEF_VAL;//CIP Flow Pulse Total Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa,u.c,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn);
		
        u.c[0] = CIP_PULSE_ON_TIME_DEF_VAL;//CIP Flow Pulse On Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLOW_PULSE_ON_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_ON_TIME_IN_SECn);
		
        u.c[0] = CIP_PULSE_OFF_TIME_DEF_VAL;//CIP Flow Pulse Off Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLOW_PULSE_OFF_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_OFF_TIME_IN_SECn);
        #endif
        u.i[0] = CIP_FLUSH_TIME_DEF_VAL;//CIP Flush Time
		ReadWriteMemBuffer(MEM_WRITE,CIP_FLUSH_TIME_IN_SECa,u.c,CIP_FLUSH_TIME_IN_SECn);
		
        u.c[0] = WV_CHECK_TIME_DEF_VAL;//WV Check Time
		ReadWriteMemBuffer(MEM_WRITE,WV_CHECK_TIME_IN_SECa,u.c,WV_CHECK_TIME_IN_SECn);
		
        u.i[0] = WAIT_HT_TIME_DEF_VAL;//Wait HT Time
		ReadWriteMemBuffer(MEM_WRITE,WAIT_HT_TIME_IN_SECa,u.c,WAIT_HT_TIME_IN_SECn);
		
        u.c[0] = TARGET_PURE_FLOW_DEF_VAL;//Target Pure Flow Divide it by 10 to get actual value
        ReadWriteMemBuffer(MEM_WRITE,TARGET_PURE_FLOW_LITER_PER_MINa,u.c,TARGET_PURE_FLOW_LITER_PER_MINn);
		
        u.i[0] = PURE_EC_TARGET_DEF_VAL;//Outlet Conductivity
		ReadWriteMemBuffer(MEM_WRITE,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECa,u.c,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECn);
		
        u.c[0] = PURIFY_LOW_FLOW_DEF_VAL;//Low Flow Purify Alarm. Divide it by 10 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,LOW_FLOW_PURIFY_ALARM_SET_POINTa,u.c,LOW_FLOW_PURIFY_ALARM_SET_POINTn);
		
        u.c[0] = WASTE_LOW_FLOW_DEF_VAL;//Low Flow waste Alarm. Divide it by 10 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,LOW_FLOW_WASTE_ALARM_SET_POINTa,u.c,LOW_FLOW_WASTE_ALARM_SET_POINTn);
		
        u.i[0] = MAX_CYCLE_COUNT_FOR_CIP_DEF_VAL;//CIP Cycles
		ReadWriteMemBuffer(MEM_WRITE,MAX_CYCLE_RUNCOUNT_FOR_CIPa,u.c,MAX_CYCLE_RUNCOUNT_FOR_CIPn);
		
        u.c[0] = CABINET_HIGH_TEMP_DEF_VAL;//Temperature alarm High Setpoint
		ReadWriteMemBuffer(MEM_WRITE,CABINET_HIGH_TEMPERATURE_SET_VALa,u.c,CABINET_HIGH_TEMPERATURE_SET_VALn);
		
        u.i[0] = MAX_CIP_PUMP_RUN_TIME_DEF_VAL;//Max CIP Pump Runtime
        ReadWriteMemBuffer(MEM_WRITE,MAX_CIP_PUMP_RUNTIMEa,u.c,MAX_CIP_PUMP_RUNTIMEn);
		
        u.i[0] = ANA_OUT_P_FACTOR_DEF_VAL;//Pump P-factor. Divide it by 100 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,PUMP_P_FACTORa,u.c,PUMP_P_FACTORn);
		
        u.i[0] = DYNA_P_FACTOR_DEF_VAL;//Dynamic P-factor. Divide it by 100 to get actual value
		ReadWriteMemBuffer(MEM_WRITE,DYNAMIC_P_FACTORa,u.c,DYNAMIC_P_FACTORn);
		
        u.i[0] = PAE_PREPURIFY_STEP_VOLT_DEF_VAL;// Max Voltage PrePure. Divite it by 10
		ReadWriteMemBuffer(MEM_WRITE,PAE_PREPURIFY_CYCLE_VOLTAGEa,u.c,PAE_PREPURIFY_CYCLE_VOLTAGEn);
		
        u.i[0] = PAE_PURE_STEP_VOLT_DEF_VAL;// Max Voltage Pure. Divite it by 10
		ReadWriteMemBuffer(MEM_WRITE,PAE_PURE_CYCLE_VOLTAGEa,u.c,PAE_PURE_CYCLE_VOLTAGEn);
		
        u.i[0] = PAE_WASTE_STEP_VOLT_DEF_VAL;// Max Voltage Waste. Divite it by 10
		ReadWriteMemBuffer(MEM_WRITE,PAE_WASTE_CYCLE_VOLTAGEa,u.c,PAE_WASTE_CYCLE_VOLTAGEn);
		
        u.i[0] = CURRENT_P_FACTOR_DEF_VAL;// waste value. Divide it by 100
		ReadWriteMemBuffer(MEM_WRITE,WASTE_VALUE_CURRENTa,u.c,WASTE_VALUE_CURRENTn);
		
        u.i[0] = FLOWMETER_K_FACTOR_DEF_VAL;//720;// Flow meter K factor from range 10 to 2000.
		ReadWriteMemBuffer(MEM_WRITE,FLOWMETER_K_FACTORa,u.c,FLOWMETER_K_FACTORn);
        
        u.c[0] = VOLUME_UNIT_DEF_VAL;
        ReadWriteMemBuffer(MEM_WRITE,VOLUME_UNITa,u.c,VOLUME_UNITn);
        
        u.i[0] = 0;
        ReadWriteMemBuffer(MEM_WRITE,TEMPERATURE_OFFSET_REGISTERa,u.c,TEMPERATURE_OFFSET_REGISTERn);
        
        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                u.c[0] = BYPASS_OPTION_DEF_VAL;
                ReadWriteMemBuffer(MEM_WRITE,BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
                u.c[0] = FALSE;
                ReadWriteMemBuffer(MEM_WRITE,TANK_FULLa,u.c,TANK_FULLn);
        #endif
        
        u.i[0] = START_PRESSURE_DEF_VAL;// analog pressure start level less than this pressure in bar. Divide it by 100
        ReadWriteMemBuffer(MEM_WRITE,ANALOG_PRESSURE_START_LEVELa,u.c,ANALOG_PRESSURE_START_LEVELn);
        
        u.i[0] = STOP_PRESSURE_DEF_VAL;// analog pressure stop level > than this pressure in bar. Divide it by 100
        ReadWriteMemBuffer(MEM_WRITE,ANALOG_PRESSURE_STOP_LEVELa,u.c,ANALOG_PRESSURE_STOP_LEVELn);
        
        u.i[0] = BYPASS_PRESSURE_DEF_VAL;// analog pressure BYPASS level > than this pressure in bar. Divide it by 100
        ReadWriteMemBuffer(MEM_WRITE,ANALOG_PRESSURE_BYPASS_LEVELa,u.c,ANALOG_PRESSURE_BYPASS_LEVELn);
        
        u.i[0] = CIP_PRESSURE_DEF_VAL;// analog pressure CIP level > than this pressure in bar. Divide it by 100
        ReadWriteMemBuffer(MEM_WRITE,ANALOG_PRESSURE_CIP_LEVELa,u.c,ANALOG_PRESSURE_CIP_LEVELn);
        
        u.i[0] = CIP_WAIT_TIME_DEF_VAL;// CIP_WaitTimeInSec
        ReadWriteMemBuffer(MEM_WRITE,CIP_WAIT_TIME_IN_SECa,u.c,CIP_WAIT_TIME_IN_SECn);
        
        u.i[0] = WAIT_TIME_B4_BYPASS_ALARM_DEF_VAL;// WaitTimebB4BypassAlarmInSec
        ReadWriteMemBuffer(MEM_WRITE,WAIT_TIME_B4_BYPASS_ALARM_IN_SECa,u.c,WAIT_TIME_B4_BYPASS_ALARM_IN_SECn);
        
        u.c[0] = 50;
        ReadWriteMemBuffer(MEM_WRITE,SP_EC_ZERO_CYCLEa,u.c,SP_EC_ZERO_CYCLEn);
        
        #if(CVS_CIP==CVS_CIP_NO)
        u.i[0] = CIP_LOWER_LIMIT_CYCLE_DEF_VAL;// CIP_LowerLimitCycle
        ReadWriteMemBuffer(MEM_WRITE,CIP_LOWER_LIMIT_CYCLEa,u.c,CIP_LOWER_LIMIT_CYCLEn);
        
        u.i[0] = CIP_FAVORABLE_TIME_DEF_VAL;// CIP_FavorableTime
        ReadWriteMemBuffer(MEM_WRITE,CIP_FAVORABLE_TIMEa,u.c,CIP_FAVORABLE_TIMEn);
        
        u.i[0] = CIP_FAVORABLE_TIME_GRACE_MINUTES_DEF_VAL;// CIP_FavorableTimeGraceMin
        ReadWriteMemBuffer(MEM_WRITE,CIP_FAVORABLE_TIME_GRACE_MINa,u.c,CIP_FAVORABLE_TIME_GRACE_MINn);
        #endif
        
        u.i[0] = 0;
        ReadWriteMemBuffer(MEM_WRITE,PRESSURE_OFFSETa,u.c,PRESSURE_OFFSETn);
        
        u.c[0] = 5;
        ReadWriteMemBuffer(MEM_WRITE,WASTE_PRE_SHUNT_TIME_IN_SECa,u.c,WASTE_PRE_SHUNT_TIME_IN_SECn);
        #if(PRODUCT_TYPE==PRODUCT_DIENTRY)
            u.c[0] = 5;
            ReadWriteMemBuffer(MEM_WRITE,WASTE_POST_SHUNT_TIME_IN_SECa,u.c,WASTE_POST_SHUNT_TIME_IN_SECn);
        #else
            u.c[0] = 0;
            ReadWriteMemBuffer(MEM_WRITE,WASTE_POST_SHUNT_TIME_IN_SECa,u.c,WASTE_POST_SHUNT_TIME_IN_SECn);
            u.c[0] = 0;
            ReadWriteMemBuffer(MEM_WRITE,HF_WASTE_SHUNT_TIME_IN_SECa,u.c,HF_WASTE_SHUNT_TIME_IN_SECn);
        #endif   
            
        u.c[0] = 230;   //FOR 2.3V to omit decimal point, multiply by 100
        ReadWriteMemBuffer(MEM_WRITE,SWITCH_PAE_VOLT_PUREa,u.c,SWITCH_PAE_VOLT_PUREn);
        ReadWriteMemBuffer(MEM_WRITE,SWITCH_PAE_VOLT_WASTEa,u.c,SWITCH_PAE_VOLT_WASTEn);
	#endif
    
    //project non-volatile variable starts
	u.c[0] = 40;//Duty cycle of 40 gives 1Volt
	ReadWriteMemBuffer(MEM_WRITE,LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEa,u.c,LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEn);
	u.c[0] = SHUNT_TIME_IN_SEC_DEFAULT_VAL;
    ReadWriteMemBuffer(MEM_WRITE,SHUNT_TIME_IN_SECa,u.c,SHUNT_TIME_IN_SECn);
	u.c[0] = COMM_MEDIA_AUTO_SWITCH;
	ReadWriteMemBuffer(MEM_WRITE,COMMUNICATION_MEDIA_PREFEREDa,u.c,COMMUNICATION_MEDIA_PREFEREDn);
	
    
	/*******************************************************************************/
	/*******************************************************************************/
	
	/*******************************************************************************/
	u.c[0] = 0x55;
	ReadWriteMemBuffer(MEM_WRITE,SETTING_STATUS_REGa,u.c,SETTING_STATUS_REGn);
	BufferToPage(BUFF1_TO_PAGE,SYSTEM_SETTING_START_PAGE);
	//--------------------------------------------------------------------------
}
////////////////////////////////////////////////////////////////////////////////
void LoadSetting()
{	//SerialNoCheck();//to update serial valid flag, if serial code is sent though SMS
	ucTemp = 0;
    do
    {   //PutStringUsb("\r\nSERIAL NO. VALID?");//RESET TEST CODE
        DelayInMilliSec(1000);//this delay is required if Reset factory set and loadsetting is used consequitivly.
        ReadSerialInfoInCopyBuff();
        if(CopyBuff[0] == 0)
           SerialNoValidFlag = TRUE;
        else
            SerialNoValidFlag = FALSE;
        u.c[0] = CopyBuff[1];
        u.c[1] = CopyBuff[2];
        u.c[2] = CopyBuff[3];
        u.c[3] = CopyBuff[4];
        Device.SerialNo = u.i[0];
        
        if(SerialNoValidFlag == FALSE || Device.SerialNo == 0xFFFFFFFF)
            ucTemp++;
        else
            break;
    }while(ucTemp < 5);
    /*if(ucTemp >= 5) 
    {   PutStringUsb("\r\nRESTART DUE TO SERIAL NO. ERROR");//RESET TEST CODE
        SoftwareResetProcessor();
    }*/					
	DirectMemRead(FOTA_STATUSa,u.c,FOTA_STATUSn);
	Fota.Status = u.c[0];
	DirectMemRead(DEVICE_PASSWORDa,Device.Password,DEVICE_PASSWORDn);
	Device.Password[DEVICE_PASSWORDn] = '\0';
	
	u.l[0] = 0;//MUST before using it in DirectMemRead Function
	DirectMemRead(TIME_ZONEa,u.c,TIME_ZONEn);
	if((u.c[2]&0x80) > 0)
	  u.c[3]=0xFF;
	Device.TimeZone = u.i[0];	//in seconds
	
	DirectMemRead(MIN_REC_PER_CONNECTIONa,u.c,MIN_REC_PER_CONNECTIONn);
	MinRecPerConnection = u.c[0];
	//project parameter starts
	DirectMemRead(PREPURIFY_TIME_IN_SECa,u.c,PREPURIFY_TIME_IN_SECn);
	PrepurifyTimeInSec = u.c[0];//Prepurify Time
	u.i[0] = 0;
	DirectMemRead(PURIFY_TIME_IN_SECa,u.c,PURIFY_TIME_IN_SECn);
	PurifyTimeInSec = u.i[0];//Purify Time
	u.i[0] = 0;
	DirectMemRead(WASTE_TIME_IN_SECa,u.c,WASTE_TIME_IN_SECn);
	WasteTimeInSec = u.i[0];//Waste time
	
	u.i[0] = 0;//now onward all less than 2 byte read initialization
	DirectMemRead(HF_WASTE_TIME_IN_SECa,u.c,HF_WASTE_TIME_IN_SECn);
	HF_WasteTimeInSec = u.i[0];//HF Waste time
	DirectMemRead(CIP_DOSING_ON_TIME_IN_SECa,u.c,CIP_DOSING_ON_TIME_IN_SECn);
	CIP_DosingOnTimeInSec = u.c[0];//CIP Dosing On Time
	DirectMemRead(CIP_DOSING_OFF_TIME_IN_SECa,u.c,CIP_DOSING_OFF_TIME_IN_SECn);
	CIP_DosingOffTimeInSec = u.i[0];//CIP Dosing Off Time
    
    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
        DirectMemRead(CIP_DOSING_TOTAL_TIME_IN_SECa,u.c,CIP_DOSING_TOTAL_TIME_IN_SECn);
        CIP_DosingTotalTimeInSec = u.i[0];//CIP Dosing Total Time
        DirectMemRead(CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa,u.c,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn);
        CIP_PulseTotalTimeInSec = u.i[0];//CIP Flow Pulse Total Time
        DirectMemRead(CIP_FLOW_PULSE_ON_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_ON_TIME_IN_SECn);
        CIP_PulseOnTimeInSec = u.c[0];//CIP Flow Pulse On Time
        DirectMemRead(CIP_FLOW_PULSE_OFF_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_OFF_TIME_IN_SECn);
        CIP_PulseOffTimeInSec = u.c[0];//CIP Flow Pulse Off Time
	#endif
        DirectMemRead(CIP_FLUSH_TIME_IN_SECa,u.c,CIP_FLUSH_TIME_IN_SECn);
	CIP_FlushTimeInSec = u.i[0];//CIP Flush Time
	DirectMemRead(WV_CHECK_TIME_IN_SECa,u.c,WV_CHECK_TIME_IN_SECn);
	WV_CheckTimeInSec = u.c[0];//WV Check Time
	DirectMemRead(WAIT_HT_TIME_IN_SECa,u.c,WAIT_HT_TIME_IN_SECn);
	WaitHT_TimeInSec = u.i[0];//Wait HT Time
	DirectMemRead(TARGET_PURE_FLOW_LITER_PER_MINa,u.c,TARGET_PURE_FLOW_LITER_PER_MINn);
	TargetPureFlowLperM = u.c[0];//Target Pure Flow. Divide it by 10 to get actual value
	DirectMemRead(TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECa,u.c,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECn);
	TargetOutletConductivity = u.i[0];//Outlet Conductivity
	DirectMemRead(LOW_FLOW_PURIFY_ALARM_SET_POINTa,u.c,LOW_FLOW_PURIFY_ALARM_SET_POINTn);
	LowFlowPurifyAlarmSetpoint = u.c[0];//Low Flow Purify Alarm. Divide it by 10 to get actual value
	DirectMemRead(LOW_FLOW_WASTE_ALARM_SET_POINTa,u.c,LOW_FLOW_WASTE_ALARM_SET_POINTn);
	LowFlowWasteAlarmSetpoint = u.c[0];//Low Flow waste Alarm. Divide it by 10 to get actual value
	DirectMemRead(MAX_CYCLE_RUNCOUNT_FOR_CIPa,u.c,MAX_CYCLE_RUNCOUNT_FOR_CIPn);
	MaxCycleCountForCIP = u.i[0];//CIP Cycles
	DirectMemRead(CABINET_HIGH_TEMPERATURE_SET_VALa,u.c,CABINET_HIGH_TEMPERATURE_SET_VALn);
	CabinetHighTemperatureSetVal = u.c[0];//Temperature alarm High Setpoint
	DirectMemRead(MAX_CIP_PUMP_RUNTIMEa,u.c,MAX_CIP_PUMP_RUNTIMEn);
	MAX_CIP_PumpRunTimeInSec = u.i[0];//Max CIP Pump Runtime
	DirectMemRead(PUMP_P_FACTORa,u.c,PUMP_P_FACTORn);
	AnaOutProportionalFactor = u.i[0];//Pump P-factor. Divide it by 100 to get actual value
	DirectMemRead(DYNAMIC_P_FACTORa,u.c,DYNAMIC_P_FACTORn);
	DynaProportionalFactor = u.i[0];//Dynamic P-factor. Divide it by 100 to get actual value
	//DirectMemRead(AVERAGE_VOLTAGE_PUREa,u.c,AVERAGE_VOLTAGE_PUREa);
	//averageVoltagePure = u.i[0];
	u.i[0] = 0;
	DirectMemRead(PAE_PREPURIFY_CYCLE_VOLTAGEa,u.c,PAE_PREPURIFY_CYCLE_VOLTAGEn);
	PAE_PrepurifyStepVoltageSetpoint = u.i[0];//PAE Pure step voltage. Divide it by 10 to get actual value
	DirectMemRead(PAE_PURE_CYCLE_VOLTAGEa,u.c,PAE_PURE_CYCLE_VOLTAGEn);
	PAE_PureStepVoltageSetpoint = u.i[0];//PAE Pure step voltage. Divide it by 10 to get actual value
	DirectMemRead(PAE_WASTE_CYCLE_VOLTAGEa,u.c,PAE_WASTE_CYCLE_VOLTAGEn);
	PAE_WasteStepVoltageSetpoint = u.i[0];//PAE Waste step voltage. Divide it by 10 to get actual value
	u.i[0] = 0;// waste value. Divide it by 100
	DirectMemRead(WASTE_VALUE_CURRENTa,u.c,WASTE_VALUE_CURRENTn);
	CurrentProportionalFactor = u.i[0];
	u.i[0] = 0;// flow meter k factor
	DirectMemRead(FLOWMETER_K_FACTORa,u.c,FLOWMETER_K_FACTORn);
	FlowMeter.K_Factor = u.i[0];
        DirectMemRead(VOLUME_UNITa,u.c,VOLUME_UNITn);
	VolumeUnit = u.c[0];
        DirectMemRead(TEMPERATURE_OFFSET_REGISTERa,u.c,TEMPERATURE_OFFSET_REGISTERn);
    TemperatureOffsetRegister = u.i[0];
    #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
        DirectMemRead(BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
        BypassOption = u.c[0];
        DirectMemRead(TANK_FULLa,u.c,TANK_FULLn);
        TankFullFlag = u.c[0];
    #endif
    u.i[0] = 0;// Analog Pressure start level. Divide it by 100
	DirectMemRead(ANALOG_PRESSURE_START_LEVELa,u.c,ANALOG_PRESSURE_START_LEVELn);
	AnalogPressureStartLevel = u.i[0];
	u.i[0] = 0;// Analog Pressure stop level. Divide it by 100
	DirectMemRead(ANALOG_PRESSURE_STOP_LEVELa,u.c,ANALOG_PRESSURE_STOP_LEVELn);
	AnalogPressureStopLevel = u.i[0];
	u.i[0] = 0;// Analog Pressure bypass level. Divide it by 100
	DirectMemRead(ANALOG_PRESSURE_BYPASS_LEVELa,u.c,ANALOG_PRESSURE_BYPASS_LEVELn);
	AnalogPressureBypassLevel = u.i[0];
	u.i[0] = 0;// Analog Pressure cip level. Divide it by 100
	DirectMemRead(ANALOG_PRESSURE_CIP_LEVELa,u.c,ANALOG_PRESSURE_CIP_LEVELn);
	AnalogPressureCipLevel = u.i[0];
	u.i[0] = 0;// Wait Before cip time in sec
	DirectMemRead(CIP_WAIT_TIME_IN_SECa,u.c,CIP_WAIT_TIME_IN_SECn);
	CIP_WaitTimeInSec = u.i[0];
	u.i[0] = 0;// Wait Before BYPASS ALRAM time in sec
	DirectMemRead(WAIT_TIME_B4_BYPASS_ALARM_IN_SECa,u.c,WAIT_TIME_B4_BYPASS_ALARM_IN_SECn);
	WaitTimeB4BypassAlarmInSec = u.i[0];
	#if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_NO))
        u.i[0] = 0;// CIP_LowerLimitCycle
        DirectMemRead(CIP_LOWER_LIMIT_CYCLEa,u.c,CIP_LOWER_LIMIT_CYCLEn);
        CIP_LowerLimitCycle = u.i[0];
        u.i[0] = 0;// CIP_FavorableTime
        DirectMemRead(CIP_FAVORABLE_TIMEa,u.c,CIP_FAVORABLE_TIMEn);
        CIP_FavorableTime = u.i[0];
        u.i[0] = 0;// CIP_FavorableTimeGraceMin
        DirectMemRead(CIP_FAVORABLE_TIME_GRACE_MINa,u.c,CIP_FAVORABLE_TIME_GRACE_MINn);
        CIP_FavorableTimeGraceMin = u.i[0];
    #endif
    
    ///////////////////////////////////////////////// ARUN ///////////////////////////////////////////////////
	//DirectMemRead(REPORTING_TIMEa,u.c,REPORTING_TIMEn);
	//ReportingTime.tm_hour = u.c[0];
	//ReportingTime.tm_min = u.c[1];
	
	//project non-volatile variable starts
	DirectMemRead(LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEa,u.c,LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEn);
	LastCycleOptimizedOutputVoltageCode = u.c[0];//Duty cycle of 40 gives 1Volt max is 200 which is equal to 5Volts. 1 stepsize = 25mV
	DirectMemRead(TEMP_AT_WHICH_CONDUCTIVITY_WAS_CALIBRATEDa,u.c,TEMP_AT_WHICH_CONDUCTIVITY_WAS_CALIBRATEDn);
	TemperatureAtWhichConductivityWasCalibrated = u.i[0];
	DirectMemRead(SHUNT_TIME_IN_SECa,u.c,SHUNT_TIME_IN_SECn);
	ShuntSec = u.i[0];
	DirectMemRead(COMMUNICATION_MEDIA_PREFEREDa,u.c,COMMUNICATION_MEDIA_PREFEREDn);
	CommunicationMediaPrefered = u.c[0];
    u.f[0] = 0;
	DirectMemRead(PRESSURE_OFFSETa,u.c,PRESSURE_OFFSETn);
    if((u.f[0] >= PHYSICAL_VALUE_HIGH * -0.1) && (u.f[0] <= PHYSICAL_VALUE_HIGH * 0.1))
    {   PressureOffset = u.f[0];
    }
    else
    {   PressureOffset = 0;
    }
	DirectMemRead(WASTE_PRE_SHUNT_TIME_IN_SECa,u.c,WASTE_PRE_SHUNT_TIME_IN_SECn);
	ShuntPostPureStepTimeInSec = u.c[0];
	DirectMemRead(WASTE_POST_SHUNT_TIME_IN_SECa,u.c,WASTE_POST_SHUNT_TIME_IN_SECn);
	ShuntPostWasteStepTimeInSec = u.c[0];
	DirectMemRead(HF_WASTE_SHUNT_TIME_IN_SECa,u.c,HF_WASTE_SHUNT_TIME_IN_SECn);
	HF_WasteShuntTimeInSec = u.c[0];
        DirectMemRead(SWITCH_PAE_VOLT_WASTEa,u.c,SWITCH_PAE_VOLT_WASTEn);
	SwitchPAEvoltWaste = u.c[0];
	DirectMemRead(SWITCH_PAE_VOLT_PUREa,u.c,SWITCH_PAE_VOLT_PUREn);
	SwitchPAEvoltPure = u.c[0];
	/*******************************************************************************/
	/*******************************************************************************/
	/*******************************************************************************/
	/*******************************************************************************/
	/*
	//loading time log value
	GetPidMem2Var(MY_PID_TIME);
	switch(LogUnit)
	{  	case UNIT_SEC:	ServerTimeoutSec = LogVal.i;break;
		case UNIT_MIN:	ServerTimeoutSec = LogVal.i * 60;break;
		case UNIT_HR:	ServerTimeoutSec = LogVal.i * 3600;break;
		default:		ServerTimeoutSec = 14*60;break;//max value
	}
	ServerTimeoutSec *= MinRecPerConnection;
	if(MinRecPerConnection == 1)
	  ServerTimeoutSec *= 2;
	ServerTimeoutSec += 120;//2 min after log time
	
	//loading max value i.e 100% value from AI1
	GetPidMem2Var(MY_PID_AI1);
	Ana1_MaxMilliVolt = TrigPara3Val.i;
	Ana1_MinMilliVolt = TrigPara2Val.i;
	
	//LOADING PROJECT PARAMETER MCR
	DirectMemRead(PROJ_PARA_MCRa,PidMCR,PROJ_PARA_MCRn);
	*/
	DirectMemRead(GPRS_CRa,u.c,GPRS_CRn);
	GprsCR = u.c[0];
	
        //reading server details
	DirectMemRead(SERVER_START_ADDRESS,PasteBuff,SERVER_RECORD_BYTE_SIZE);
	Server.Sr = PasteBuff[ST_SRp];
	MyStrCpy(Server.IpOrDomainName,&PasteBuff[ST_IPorDOMAINp],ST_IPorDOMAINn);
	Server.IpOrDomainName[ST_IPorDOMAINn] = '\0';
	u.i[0] = 0;
	u.c[0] = PasteBuff[ST_PORTp];
	u.c[1] = PasteBuff[ST_PORTp+1];
	Server.Port = u.i[0];
	////////////////////////////////////////////////////////////////////////////
	//reading periodic save page	
	DirectMemRead(PERIODIC_SAVE_PAGE_START_ADDa,PasteBuff,FLASH_PAGE_SIZE);
	
	//PeriodicSaveMin = PasteBuff[PERIODIC_SAVE_MINa % FLASH_PAGE_SIZE];
	u.i[0] = 0;
	u.c[0] = PasteBuff[CIP_PUMP_RUNTIMEa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(CIP_PUMP_RUNTIMEa+1) % FLASH_PAGE_SIZE];				
	CIP_PumpRunTimeInSec = u.i[0];
	
	u.i[0] = 0;
	u.c[0] = PasteBuff[LAST_CYCLE_COUNTER_FOR_CIPa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(LAST_CYCLE_COUNTER_FOR_CIPa+1) % FLASH_PAGE_SIZE];	
	CycleCounter = u.i[0];//Already Run Cycles for CIP
	
	u.i[0] = 0;// OptimizedPAE_CurrentForPrepureStep. Divide it by 100
	u.c[0] = PasteBuff[OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPa+1) % FLASH_PAGE_SIZE];
	OptimizedPAE_CurrentForPrepureStep = u.i[0];
	
    u.i[0] = 0;// OptimizedPAE_CurrentForPureStep. Divide it by 100
	u.c[0] = PasteBuff[OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPa+1) % FLASH_PAGE_SIZE];
	OptimizedPAE_CurrentForPureStep = u.i[0];
	
	u.i[0] = 0;// OptimizedPAE_CurrentForWasteStep. Divide it by 100
	u.c[0] = PasteBuff[OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPa+1) % FLASH_PAGE_SIZE];
	OptimizedPAE_CurrentForWasteStep = u.i[0];
	
	Mode = PasteBuff[MODEa % FLASH_PAGE_SIZE];
	if(Mode == MODE_AUTO)
	{	if(SerialNoValidFlag == TRUE)
		{   DisplayPage=AUTO_0;
            // Activate Conductivity Probe by NABEEN on Feb 23 start
			u.c[0] = 1;//Activate
			WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
			// Activate Conductivity Probe by NABEEN on Feb 23 end
            
            // Activate Temperature Probe by NABEEN on JUN 07 start
            #ifdef TEMPERATURE_COMPENSATED_CONDUCTIVITY
                Delay(10);
                u.c[0] = 1;//Activate
                //WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);//activate temperature probe
            #endif
            // Activate Temperature Probe by NABEEN on JUN 07 start
		}
		else
		{	DisplayPage = DEV_INFO_3_1;
        }
	}
    
	u.i[0] = 0;// LastCyclePureAverageConductivity. Divide it by 100
	u.c[0] = PasteBuff[LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa+1) % FLASH_PAGE_SIZE];
	u.c[2] = PasteBuff[(LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa+2) % FLASH_PAGE_SIZE];
	LastCyclePureAverageConductivity = u.i[0];
	
	u.i[0] = 0;// PureVolumeOfBatchJustCompleted. Divide it by 100
	u.c[0] = PasteBuff[LAST_CYCLE_PURE_VOLUMEa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(LAST_CYCLE_PURE_VOLUMEa+1) % FLASH_PAGE_SIZE];
	PureVolumeOfBatchJustCompleted = u.i[0]/100;
	
	u.i[0] = 0;// WasteVolumeOfBatchJustCompleted. Divide it by 100
	u.c[0] = PasteBuff[LAST_CYCLE_WASTE_VOLUMEa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(LAST_CYCLE_WASTE_VOLUMEa+1) % FLASH_PAGE_SIZE];
	WasteVolumeOfBatchJustCompleted = u.i[0]/100;
	
	u.i[0] = 0;// CIP_DosingRunTimeInSec.
	u.c[0] = PasteBuff[CIP_DOSING_RUNTIME_IN_SECa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(CIP_DOSING_RUNTIME_IN_SECa+1) % FLASH_PAGE_SIZE];
	CIP_DosingRunTimeInSec = u.i[0];
	
    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    u.i[0] = 0;// CIP_PulseRunTimeInSec.
	u.c[0] = PasteBuff[CIP_PULSE_RUNTIME_IN_SECa % FLASH_PAGE_SIZE];
	u.c[1] = PasteBuff[(CIP_PULSE_RUNTIME_IN_SECa+1) % FLASH_PAGE_SIZE];
	CIP_PulseRunTimeInSec = u.i[0];
    #endif
    
    PercentageRecovery[0] = PasteBuff[PERCENT_RECOVERYa % FLASH_PAGE_SIZE];
    PercentageRecovery[1] = PasteBuff[(PERCENT_RECOVERYa+1) % FLASH_PAGE_SIZE];
    PercentageRecovery[2] = PasteBuff[(PERCENT_RECOVERYa+2) % FLASH_PAGE_SIZE];
    PercentageRecovery[3] = PasteBuff[(PERCENT_RECOVERYa+3) % FLASH_PAGE_SIZE];
    PercentageRecovery[4] = PasteBuff[(PERCENT_RECOVERYa+4) % FLASH_PAGE_SIZE];
    
    u.c[0] = PasteBuff[LAST_FIVE_CYCLE_AVG_ECa % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+1) % FLASH_PAGE_SIZE];
    u.c[2] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+2) % FLASH_PAGE_SIZE];
    u.c[3] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+3) % FLASH_PAGE_SIZE];
    LastFiveCyclePureAverageConductivitySamples[0] = u.i[0];
    
    u.c[0] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+4) % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+5) % FLASH_PAGE_SIZE];
    u.c[2] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+6) % FLASH_PAGE_SIZE];
    u.c[3] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+7) % FLASH_PAGE_SIZE];
    LastFiveCyclePureAverageConductivitySamples[1] = u.i[0];
    
    u.c[0] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+8) % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+9) % FLASH_PAGE_SIZE];
    u.c[2] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+10) % FLASH_PAGE_SIZE];
    u.c[3] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+11) % FLASH_PAGE_SIZE];
    LastFiveCyclePureAverageConductivitySamples[2] = u.i[0];
    
    u.c[0] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+12) % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+13) % FLASH_PAGE_SIZE];
    u.c[2] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+14) % FLASH_PAGE_SIZE];
    u.c[3] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+15) % FLASH_PAGE_SIZE];
    LastFiveCyclePureAverageConductivitySamples[3] = u.i[0];
    
    u.c[0] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+16) % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+17) % FLASH_PAGE_SIZE];
    u.c[2] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+18) % FLASH_PAGE_SIZE];
    u.c[3] = PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+19) % FLASH_PAGE_SIZE];
    LastFiveCyclePureAverageConductivitySamples[4] = u.i[0];
    LastFiveCyclePureAverageConductivityAverage = (LastFiveCyclePureAverageConductivitySamples[0] + LastFiveCyclePureAverageConductivitySamples[1] + LastFiveCyclePureAverageConductivitySamples[2] + LastFiveCyclePureAverageConductivitySamples[3] + LastFiveCyclePureAverageConductivitySamples[4])/5;
    
	//BalanceCheckFlag = (PasteBuff[BALANCE_CHECK_FLAGa % FLASH_PAGE_SIZE]%2);
	
	u.c[0]=PasteBuff[EMAIL_PTRa % FLASH_PAGE_SIZE];
	u.c[1]=PasteBuff[(EMAIL_PTRa+1) % FLASH_PAGE_SIZE];
	u.c[2]=PasteBuff[(EMAIL_PTRa+2) % FLASH_PAGE_SIZE];
	u.c[3]=0;
	EmailPtr = u.i[0];
	TempEmailPtr = EmailPtr;
	
	u.c[0]=PasteBuff[LOG_PTRa % FLASH_PAGE_SIZE];
	u.c[1]=PasteBuff[(LOG_PTRa+1) % FLASH_PAGE_SIZE];
	u.c[2]=PasteBuff[(LOG_PTRa+2) % FLASH_PAGE_SIZE];
	u.c[3]=0;
	LogPtr = u.i[0];
	
    u.i[0] = 0;
    u.c[0]=PasteBuff[(UPLOAD_PTR_PROFILE1a) % FLASH_PAGE_SIZE];
    u.c[1]=PasteBuff[(UPLOAD_PTR_PROFILE1a+1) % FLASH_PAGE_SIZE];
    u.c[2]=PasteBuff[(UPLOAD_PTR_PROFILE1a+2) % FLASH_PAGE_SIZE];
    UploadPtr = u.i[0];
    if(((UploadPtr-SECOND_LOG_START_ADD)%SECOND_LOG_PACKET_SIZE)!=0)
		  UploadPtr = LogPtr;
	
    CurrentProcessStep = PasteBuff[LAST_STEP_B4_POWER_OFFa % FLASH_PAGE_SIZE];
    u.i[0] = 0;
    u.c[0] = PasteBuff[LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE)+1];
    u.c[2] = PasteBuff[(LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE)+2];
    u.c[3] = PasteBuff[(LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE)+3];
    ProcessStepRunSec = u.i[0];
    
    u.i[0] = 0;
    u.c[0] = PasteBuff[T_FLOWa % FLASH_PAGE_SIZE];
    u.c[1] = PasteBuff[(T_FLOWa % FLASH_PAGE_SIZE)+1];
    u.c[2] = PasteBuff[(T_FLOWa % FLASH_PAGE_SIZE)+2];
    u.c[3] = PasteBuff[(T_FLOWa % FLASH_PAGE_SIZE)+3];
    TotalPureVolume = u.i[0]/100;
    TotalPureVolumeB4CycleStart = TotalPureVolume;
    FlowMeter.TotalFlowAfterRestart = 0;
    StartVolume = 0;
    
    //loading input id 
	for(ucTemp = 0; ucTemp < MAX_NO_OF_INPUT; ucTemp++)
	{	DirectMemRead(INPUT_TABLE_START_ADDRESS+ucTemp*INPUT_TABLE_RECORD_SIZE,PasteBuff,INPUT_TABLE_RECORD_SIZE);
		Input[ucTemp].RegisterBitId = PasteBuff[IT_REG_BIT_ADDRESSp];
		//strcpy(CopyBuff,PasteBuff);
		//CopyBuff[IT_REG_BIT_ADDRESSp] = '\0';
		//sprintf(PasteBuff,"\r\nInput%d Name=%s ID=%d",
		//		ucTemp+1,
		//		CopyBuff,
		//		Input[ucTemp].RegisterBitId+1);
		//PutStringUsb(PasteBuff);
	}
}
////////////////////////////////////////////////////////////////////////////////
void LogRecord()
{	//static struct tm PrevLogTimeStamp;
	static ui LastSavedEC = 0;
	
	NeedToLogFlag = FALSE;
    
    if(Fota.Status == FIRMWARE_NOT_AVAILABLE && DateValidFlag ==  TRUE && TimeValidFlag == TRUE)		
	{	//_ttemp = TimeStamp2Sec(PrevLogTimeStamp);
		//u.i[0] = _ttemp;
		
		_ttemp = TimeStamp2Sec(Device.TimeStamp);
		_ttemp += SecAfterTimeSync;
		//u.i[1] = _ttemp;
		TimeStampPtr = localtime(&_ttemp);
		TimeStampPtr->tm_year -= 100;
		TimeStampPtr->tm_mon += 1;
		RecordTimeStamp = *TimeStampPtr;
		/*sprintf(PasteBuff, "\r\nTimeStamp VALUE = %d \r\n",TimeStampPtr);
		PutStringUsb(PasteBuff);*/
		
		//log filter code STARTS here
		bTemp = TRUE;//filter prior state. TRUE = valid rec , FALSE = invalid rec
		
        /*if(TimeStampPtr->tm_year < 0)
        {	bTemp = FALSE;
            if(MemDebugFlag == TRUE)
                PutStringUsb("\r\n!LOG > Invalid Date/Time");
        }
        if((AtlasReplacement.TotalFlowAfterRestart - LastSavedTotalFlowAfterRestart) > (SecDiffBetweenLog*50))//0.5 lit per sec
        {	bTemp = FALSE;
            if(MemDebugFlag == TRUE){
                sprintf(CopyBuff,"\r\n!LOG > Total Flow > 0.5L/s TFAR=%d, LSTFAR=%d, SDBL=%d",
                        AtlasReplacement.TotalFlowAfterRestart,
                        LastSavedTotalFlowAfterRestart,
                        SecDiffBetweenLog);
                PutStringUsb(CopyBuff);
                //PutStringUsb("\r\n!LOG > Total Flow > 0.5L/s");
            }
        }
		
        if(AtlasReplacement.TotalFlowAfterRestart < 0 || AtlasReplacement.CurrentFlow < 0)
		{	bTemp = FALSE;
			if(MemDebugFlag == TRUE)
				PutStringUsb("\r\n!LOG > Negative Flow");
		}
		if(AtlasReplacement.CurrentFlow > 3000)// greater than 30 L/min
		{	bTemp = FALSE;
			if(MemDebugFlag == TRUE)
				PutStringUsb("\r\n!LOG > Current Flow > 30L/m");
		}
		if(abs(LastSavedEC - AtlasConductivity) > 200000)//Filter change greater 2000.00 uS
		{	bTemp = FALSE;
			if(MemDebugFlag == TRUE)
				PutStringUsb("\r\n!LOG > Conductivity Outside Limit");
		}
		//log filter code ENDS here
		*/
		if(bTemp == TRUE)
		{	//PrevLogTimeStamp = *TimeStampPtr;
			Server.Sr &= ~SERVER_PROFILE_DONT_ATTEMPT_CONNECTION;//If listener is close, attempt again coz new packet has been created
			
			//log in fifo method starts
			//date
			PasteBuff[LT_SECOND_DATEp] = TimeStampPtr->tm_year;//     Device.TimeStamp.tm_year;
			PasteBuff[LT_SECOND_DATEp+1] = TimeStampPtr->tm_mon;//   Device.TimeStamp.tm_mon;
			PasteBuff[LT_SECOND_DATEp+2] = TimeStampPtr->tm_mday;//   Device.TimeStamp.tm_mday;
			
			//time
			PasteBuff[LT_SECOND_TIMEp] = TimeStampPtr->tm_hour;// Device.TimeStamp.tm_hour;
			PasteBuff[LT_SECOND_TIMEp+1] = TimeStampPtr->tm_min;//  Device.TimeStamp.tm_min;
			PasteBuff[LT_SECOND_TIMEp+2] = TimeStampPtr->tm_sec;// Device.TimeStamp.tm_sec;
			//cycle
			u.i[0] = CycleCounter;
			PasteBuff[LT_SECOND_CYCLEp] = u.c[0];
			PasteBuff[LT_SECOND_CYCLEp+1] = u.c[1];
			//step
			PasteBuff[LT_SECOND_STEPp] = CurrentProcessStep;
			//Run Sec
			u.i[0] = ProcessStepRunSec;
			PasteBuff[LT_SECOND_RUN_SECp] = u.c[0];
			PasteBuff[LT_SECOND_RUN_SECp+1] = u.c[1];
			//PAE volt
			u.i[0] = CurrentPAEvolt;
			PasteBuff[LT_SECOND_PAE_VOLTp] = u.c[0];
			PasteBuff[LT_SECOND_PAE_VOLTp+1] = u.c[1];
			// Total Flow
			//LastSavedTotalFlowAfterRestart = FlowMeter.TotalFlowAfterRestart;//required for flow filtering. this filter is actually not necessary
			//u.i[0] = LastSavedTotalFlowAfterRestart;
			u.i[0] = (ui)(TotalPureVolume*100);
			PasteBuff[LT_SECOND_T_FLOWp] = u.c[0];
			PasteBuff[LT_SECOND_T_FLOWp+1] = u.c[1];
			PasteBuff[LT_SECOND_T_FLOWp+2] = u.c[2];
			PasteBuff[LT_SECOND_T_FLOWp+3] = u.c[3];
			
			// Current Flow
			u.i[0] = (ui)(FlowMeter.CurrentFlow*100);
			PasteBuff[LT_SECOND_CURRENT_FLOWp] = u.c[0];
			PasteBuff[LT_SECOND_CURRENT_FLOWp+1] = u.c[1];
			
			// EC till 7.3 it was AverageConductivity, from D7.4 LastFiveCyclePureAverageConductivityAverage
			LastSavedEC = LastFiveCyclePureAverageConductivityAverage;//AverageConductivity;//AtlasConductivity;
			u.i[0] = LastSavedEC;
			PasteBuff[LT_SECOND_AVERAGE_ECp] = u.c[0];
			PasteBuff[LT_SECOND_AVERAGE_ECp+1] = u.c[1];
			PasteBuff[LT_SECOND_AVERAGE_ECp+2] = u.c[2];
			
			//CIP PUMP RUN TIME
			u.i[0] = CIP_PumpRunTimeInSec;
			PasteBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp] =  u.c[0];
			PasteBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp+1] =  u.c[1];
			//Alarm
			u.i[0] = AlarmRegister24Bit;
			PasteBuff[LT_SECOND_ALARMp] =  u.c[0];
			PasteBuff[LT_SECOND_ALARMp+1] =  u.c[1];
			PasteBuff[LT_SECOND_ALARMp+2] =  u.c[2];
			//WATER TEMPERATURE
            u.i[0] = AtlasTemperature;  //ACTUAL TEMP * 1000 
			PasteBuff[LT_SECOND_WATER_TEMPERATUREp] =  u.c[0];
			PasteBuff[LT_SECOND_WATER_TEMPERATUREp+1] =  u.c[1];
			//CABINET TEMPERATURE
            u.i[0] = Temperature.i; //ACTUAL TEMP * 10 i.e upto one decimal point
			PasteBuff[LT_SECOND_CABINET_TEMPERATUREp] =  u.c[0];
			PasteBuff[LT_SECOND_CABINET_TEMPERATUREp+1] =  u.c[1];
			//PRESSURE
            if(AnaPressureAval < 0)
                u.i[0] = 65536 - (ui)((AnaPressureAval * -1)* 100);
            else
                u.i[0] = (ui)(AnaPressureAval*100); //ACTUAL PRESSURE * 100 i.e upto TWO decimal point
            PasteBuff[LT_SECOND_PRESSUREp] =  u.c[0];
			PasteBuff[LT_SECOND_PRESSUREp+1] =  u.c[1];
			//ANALOG OUTPUT VOLTAGE IN MILLI VOLT
            u.i[0] = DAC_FB_Aval*10; 
			PasteBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp] =  u.c[0];
			PasteBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp+1] =  u.c[1];
			//DIGITAL INPUT
            PasteBuff[LT_DIGITAL_INPUTp] =  CurDigitalInput;
			//DIGITAL OUTPUT
            #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                u.i[0] = DigitalOutput;
            #endif
            #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                switch(BypassOption)
                {   case BYPASS_INTERNAL:
                    case BYPASS_EXTERNAL_NO:
                        u.i[0] = DigitalOutput;
                    break;
                    case BYPASS_EXTERNAL_NC:
                        u.i[0] = DigitalOutput ^ 0x02;//complement bypass relay
                    break;
                }
            #endif
            PasteBuff[LT_DIGITAL_OUTPUTp] =  u.c[0];
			PasteBuff[LT_DIGITAL_OUTPUTp+1] =  u.c[1];
			// % RECOVERY
            u.c[0] = (PercentageRecovery[0] + PercentageRecovery[1] + PercentageRecovery[2] + PercentageRecovery[3] + PercentageRecovery[4])/5;
            PasteBuff[LT_PERCENTAGE_RECOVERYp] =  u.c[0];
			//MODE
            u.c[0] = Mode;
            PasteBuff[LT_MODEp] =  u.c[0];
            //LIVE EC
			u.i[0] = AtlasConductivity;
			PasteBuff[LT_SECOND_LIVE_ECp] = u.c[0];
			PasteBuff[LT_SECOND_LIVE_ECp+1] = u.c[1];
			PasteBuff[LT_SECOND_LIVE_ECp+2] = u.c[2];
            //PAE CURRENT
            u.i[0] = CurrentPAEcurrent;
            PasteBuff[LT_SECOND_PAE_CURRENTp] = u.c[0];
			PasteBuff[LT_SECOND_PAE_CURRENTp+1] = u.c[1];
				
			//check log pointer before saving record into memory, if issue reinitialized it
			if(LogPtr<SECOND_LOG_START_ADD)
			{	LogPtr = SECOND_LOG_START_ADD;
				UploadPtr = LogPtr;
				UploadPtr2Increment = 0;//by mistake if server sends ack again
				EmailPtr = LogPtr;
			}
			RandomMemWrite(LogPtr,PasteBuff,SECOND_LOG_PACKET_SIZE);
			//update pointer					
			LogPtr += SECOND_LOG_PACKET_SIZE;
			if(LogPtr>=SECOND_LOG_END_ADD)
			  LogPtr = SECOND_LOG_START_ADD;
            if(LogPtr == UploadPtr)
			{	UploadPtr = LogPtr + SECOND_LOG_PACKET_SIZE;
				if(UploadPtr >= SECOND_LOG_END_ADD)
                    UploadPtr = SECOND_LOG_START_ADD;
			}
			
            if(LogPtr == EmailPtr)
			{	EmailPtr = LogPtr + SECOND_LOG_PACKET_SIZE;
				if(EmailPtr >= SECOND_LOG_END_ADD)
				  EmailPtr = SECOND_LOG_START_ADD;
			}
			MinRecCounter4PeriodicSave++;
			
			//update post effect variables
			
			
			/*//option 2 for saving no of write cycle
			for(u.c[0]=0;u.c[0]<ucTemp;u.c[0]++)
			{	LogBuff[LogPtr%BYTE_PER_PAGE] = PasteBuff[u.c[0]];
				LogPtr++;
				if(LogPtr%BYTE_PER_PAGE == 0)//PAGE CHANGE OCCURS
				{	ReadWriteMemBuffer(MEM_WRITE,0,LogBuff,BYTE_PER_PAGE);
					BufferToPage(BUFF1_TO_PAGE,(LogPtr/BYTE_PER_PAGE)-1);
					DirectMemRead(LogPtr,LogBuff,BYTE_PER_PAGE);
				}
			}*/
			/*testing code starts*/
			if(MemDebugFlag == TRUE)
			{	ArrangeCurrentStatus2PasteBuff();
				PutStringUsb(PasteBuff);		
			}
			////testing code ends
        
		}
        else if(MemDebugFlag == TRUE && TimeValidFlag == FALSE || DateValidFlag == FALSE)
            PutStringUsb("\r\n!LOG > Please Set Date/Time");
          
	}
	else
	{	if(MemDebugFlag == TRUE)
        {	if(TimeValidFlag == FALSE || DateValidFlag == FALSE)
                PutStringUsb("\r\n!LOG > Please Set Date/Time");
            else
                PutStringUsb("\r\n!LOG > Fota On Progress");
        }
    }
    //ACTIVATE SENSORS
    u.c[0] = 1;//Activate
	//WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
	//WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);                                            											
}
void RegisterAlarm(uc alarm_code, boolean_t state)
{	if(state == ALARM_OCCURED)
	{	if((AlarmRegister24Bit & (1<<alarm_code)) == 0)//only if prev this alarm was not there
		{	AlarmRegister24Bit |= (1<<alarm_code);
			_ttemp = TimeStamp2Sec(Device.TimeStamp);
			_ttemp += SecAfterTimeSync;
			TimeStampPtr = localtime(&_ttemp);
			TimeStampPtr->tm_year -= 100;
			TimeStampPtr->tm_mon += 1;
			PasteBuff[0] = TimeStampPtr->tm_year;
			PasteBuff[1] = TimeStampPtr->tm_mon;
			PasteBuff[2] = TimeStampPtr->tm_mday;
			PasteBuff[3] = TimeStampPtr->tm_hour;
			PasteBuff[4] = TimeStampPtr->tm_min;
			PasteBuff[5] = TimeStampPtr->tm_sec;
			RandomMemWrite(ALARM_TABLE_START_ADDRESS + alarm_code * ALARM_TABLE_RECORD_SIZE, PasteBuff, ALARM_TABLE_RECORD_SIZE);
            AlarmFIFO(AlarmRegister24Bit);
		}
	}
	else
	{	AlarmRegister24Bit &= ~(1<<alarm_code);
        AlarmFIFO(AlarmRegister24Bit);
    }
}
void Report(uc table, uc respond_to, ui condition)
{	ui log_rec_add;
	switch(table)//second log table
	{	
		case SECOND_LOG_TABLE_START_PAGE:
			if(condition == ALL)//without date condition
			{	strcpy(PasteBuff,"\r\nLoc\tDate\tTime\tCycle\tStep\tP.Sec\tPAEvolt\tTot.Flow\tCur.Flow\tEC\tAlarm");
				switch(respond_to){
					case USB_COM: PutStringUsb(PasteBuff);break;
					case EMAIL:   PutStringGprs(PasteBuff);break;
				}
				for(log_rec_add=LogPtr-SECOND_LOG_PACKET_SIZE;log_rec_add != LogPtr;log_rec_add-=SECOND_LOG_PACKET_SIZE)
				{	
                    #ifdef WATCH_DOG_TIMER_ENABLED
                        RESET_WATCH_DOG_TIMER;
                    #endif
                    if(SecondChangeFlag == TRUE){
						SecondChangeFlag = FALSE;
						MAIN_PROG_LED_TOGGLE;
						/*if(Mode == MODE_AUTO){
							CheckKeyPress();
							AutoModeOperations();
							UpdateOutput();
							DisplayMenu();
						}*/
						strcpy(LcdLine1,"Uploading Data!!");
						strcpy(LcdLine2,"Please Wait.....");
						SendSerialHmiPacket();
					}
					if(log_rec_add < SECOND_LOG_START_ADD)
						log_rec_add = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
					
					DirectMemRead(log_rec_add,CopyBuff,SECOND_LOG_PACKET_SIZE);
                    //date
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					
					if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
					{	PutStringUsb("\r\nRecord Download Completed\r\n");
						//break;//for loop break; // issue here
					}
					
					#if(RELEASE_STATUS == DEBUG)
                        //loc
                        sprintf(PasteBuff,"\r\n%d\t%d/%d/%d",log_rec_add,u.c[0],u.c[1],u.c[2]);
					    DelayInMilliSec(20);//my laptop can not handle high speed so given this delay > 15mS
                    #else
                        sprintf(PasteBuff,"\r\n%d/%d/%d",u.c[0],u.c[1],u.c[2]);
                    #endif
					//time
					u.c[0] = CopyBuff[LT_SECOND_TIMEp];// = Device.TimeStamp.tm_hour;
					u.c[1] = CopyBuff[LT_SECOND_TIMEp+1];// = Device.TimeStamp.tm_min;
					u.c[2] = CopyBuff[LT_SECOND_TIMEp+2];// = Device.TimeStamp.tm_sec;
					sprintf(MsgBuff,"\t%d:%d:%d",u.c[0],u.c[1],u.c[2]);
					strcat(PasteBuff,MsgBuff);
					
					//cycle
					u.i[0] = 0;//CycleCounter;
					u.c[0] = CopyBuff[LT_SECOND_CYCLEp];// = u.c[0];
					u.c[1] = CopyBuff[LT_SECOND_CYCLEp+1];// = u.c[1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//step
					//CopyBuff[LT_SECOND_STEPp] = CurrentProcessStep;
					sprintf(MsgBuff,"\t%d",CopyBuff[LT_SECOND_STEPp]);
					strcat(PasteBuff,MsgBuff);
					
					//process run sec
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_RUN_SECp];
					u.c[1] = CopyBuff[LT_SECOND_RUN_SECp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//PAE volt
					u.i[0] = 0;//CurrentPAEvolt;
					u.c[0] = CopyBuff[LT_SECOND_PAE_VOLTp];
					u.c[1] = CopyBuff[LT_SECOND_PAE_VOLTp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Total Flow
					//u.i[0] = AtlasReplacement.TotalFlow;
					u.c[0] = CopyBuff[LT_SECOND_T_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_T_FLOWp+1];
					u.c[2] = CopyBuff[LT_SECOND_T_FLOWp+2];
					u.c[3] = CopyBuff[LT_SECOND_T_FLOWp+3];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Current Flow
					u.i[0] = 0;//AtlasReplacement.CurrentFlow;
					u.c[0] = CopyBuff[LT_SECOND_CURRENT_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_CURRENT_FLOWp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					//AVG EC
					u.i[0] = 0;//AtlasConductivity;
					u.c[0] = CopyBuff[LT_SECOND_AVERAGE_ECp];
					u.c[1] = CopyBuff[LT_SECOND_AVERAGE_ECp+1];
					u.c[2] = CopyBuff[LT_SECOND_AVERAGE_ECp+2];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					/*
					//CIP pump run time
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp];
					u.c[1] = CopyBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					*/
					//Alarm
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_ALARMp];
					u.c[1] = CopyBuff[LT_SECOND_ALARMp+1];
					u.c[2] = CopyBuff[LT_SECOND_ALARMp+2];
					sprintf(MsgBuff,"\t%xh",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					switch(respond_to){
						case USB_COM: PutStringUsb(PasteBuff);/*DelayInMilliSec(50); if hangs enable it */ break;
						case EMAIL:   PutStringGprs(PasteBuff);break;
					}
				}
			}
			else if(condition == TODAY)//download of today data only
			{	strcpy(PasteBuff,"\r\nDate\tTime\tCycle\tStep\tP.Sec\tPAEvolt\tTot.Flow\tCur.Flow\tEC\tAlarm");
				switch(respond_to){
					case USB_COM: PutStringUsb(PasteBuff);break;
					case EMAIL:   PutStringGprs(PasteBuff);break;
				}
				for(log_rec_add=LogPtr-SECOND_LOG_PACKET_SIZE;log_rec_add != LogPtr;log_rec_add-=SECOND_LOG_PACKET_SIZE)
				{	
                    #ifdef WATCH_DOG_TIMER_ENABLED
                        RESET_WATCH_DOG_TIMER;
                    #endif
                    if(SecondChangeFlag == TRUE){
						SecondChangeFlag = FALSE;
						MAIN_PROG_LED_TOGGLE;
						/*if(Mode == MODE_AUTO){
							CheckKeyPress();
							AutoModeOperations();
							UpdateOutput();
							DisplayMenu();
						}*/
						strcpy(LcdLine1,"Uploading Data!!");
						strcpy(LcdLine2,"Please Wait.....");
						SendSerialHmiPacket();
					}
					if(log_rec_add < SECOND_LOG_START_ADD)
						log_rec_add = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
					
					DirectMemRead(log_rec_add,CopyBuff,SECOND_LOG_PACKET_SIZE);
					
					//date
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					
					if(u.c[0]!=Device.TimeStamp.tm_year || u.c[1]!=Device.TimeStamp.tm_mon || u.c[2]!=Device.TimeStamp.tm_mday)
					{	PutStringUsb("\r\nRecord Download Completed");
						break;//for loop break;
					}
					if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
					{	PutStringUsb("\r\nRecord Download Completed");
						break;//for loop break;
					}
					
					sprintf(PasteBuff,"\r\n%d/%d/%d",u.c[0],u.c[1],u.c[2]);
					
					//time
					u.c[0] = CopyBuff[LT_SECOND_TIMEp];// = Device.TimeStamp.tm_hour;
					u.c[1] = CopyBuff[LT_SECOND_TIMEp+1];// = Device.TimeStamp.tm_min;
					u.c[2] = CopyBuff[LT_SECOND_TIMEp+2];// = Device.TimeStamp.tm_sec;
					sprintf(MsgBuff,"\t%d:%d:%d",u.c[0],u.c[1],u.c[2]);
					strcat(PasteBuff,MsgBuff);
					
					//cycle
					u.i[0] = 0;//CycleCounter;
					u.c[0] = CopyBuff[LT_SECOND_CYCLEp];
					u.c[1] = CopyBuff[LT_SECOND_CYCLEp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//step
					//CopyBuff[LT_SECOND_STEPp] = CurrentProcessStep;
					sprintf(MsgBuff,"\t%d",CopyBuff[LT_SECOND_STEPp]);
					strcat(PasteBuff,MsgBuff);
					
					//process run sec
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_RUN_SECp];
					u.c[1] = CopyBuff[LT_SECOND_RUN_SECp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//PAE volt
					u.i[0] = 0;//CurrentPAEvolt;
					u.c[0] = CopyBuff[LT_SECOND_PAE_VOLTp];
					u.c[1] = CopyBuff[LT_SECOND_PAE_VOLTp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Total Flow
					//u.i[0] = AtlasReplacement.TotalFlow;
					u.c[0] = CopyBuff[LT_SECOND_T_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_T_FLOWp+1];
					u.c[2] = CopyBuff[LT_SECOND_T_FLOWp+2];
					u.c[3] = CopyBuff[LT_SECOND_T_FLOWp+3];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Current Flow
					u.i[0] = 0;//AtlasReplacement.CurrentFlow;
					u.c[0] = CopyBuff[LT_SECOND_CURRENT_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_CURRENT_FLOWp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// EC
					u.i[0] = 0;//AtlasConductivity;
					u.c[0] = CopyBuff[LT_SECOND_AVERAGE_ECp];
					u.c[1] = CopyBuff[LT_SECOND_AVERAGE_ECp+1];
					u.c[2] = CopyBuff[LT_SECOND_AVERAGE_ECp+2];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					//Alarm
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_ALARMp];
					u.c[1] = CopyBuff[LT_SECOND_ALARMp+1];
					u.c[2] = CopyBuff[LT_SECOND_ALARMp+2];
					sprintf(MsgBuff,"\t%xh",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					switch(respond_to){
						case USB_COM: PutStringUsb(PasteBuff);break;
						case EMAIL:   PutStringGprs(PasteBuff);break;
					}
				}
			}
			else if(condition == YESTERDAY)//download of YESTERDAY data only
			{	strcpy(PasteBuff,"\r\nDate\tTime\tCycle\tStep\tP.Sec\tPAEvolt\tTot.Flow\tCur.Flow\tEC\tAlarm");
				switch(respond_to){
					case USB_COM: PutStringUsb(PasteBuff);break;
					case EMAIL:   PutStringGprs(PasteBuff);break;
				}
				for(log_rec_add=LogPtr-SECOND_LOG_PACKET_SIZE;log_rec_add != LogPtr;log_rec_add-=SECOND_LOG_PACKET_SIZE)
				{	
                    #ifdef WATCH_DOG_TIMER_ENABLED
                        RESET_WATCH_DOG_TIMER;
                    #endif
                    if(SecondChangeFlag == TRUE){
						SecondChangeFlag = FALSE;
						MAIN_PROG_LED_TOGGLE;
						/*if(Mode == MODE_AUTO){
							CheckKeyPress();
							AutoModeOperations();
							UpdateOutput();
							DisplayMenu();
						}*/
						strcpy(LcdLine1,"Uploading Data!!");
						strcpy(LcdLine2,"Please Wait.....");
						SendSerialHmiPacket();
					}
					if(log_rec_add < SECOND_LOG_START_ADD)
						log_rec_add = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
					
					DirectMemRead(log_rec_add,CopyBuff,SECOND_LOG_PACKET_SIZE);
					
					//date
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					
					temp_timestamp.tm_year = u.c[0];
					temp_timestamp.tm_mon = u.c[1];
					temp_timestamp.tm_mday = u.c[2];
					temp_timestamp.tm_hour = 0;
					temp_timestamp.tm_min = 0;
					temp_timestamp.tm_sec = 0;
					
					_ttemp = TimeStamp2Sec(temp_timestamp);
					u.i[0] = _ttemp;
					
					temp_timestamp.tm_year = Device.TimeStamp.tm_year;
					temp_timestamp.tm_mon = Device.TimeStamp.tm_mon;
					temp_timestamp.tm_mday = Device.TimeStamp.tm_mday;
					temp_timestamp.tm_hour = 0;
					temp_timestamp.tm_min = 0;
					temp_timestamp.tm_sec = 0;
					_ttemp = TimeStamp2Sec(temp_timestamp);
					u.i[1] = _ttemp;
					
					//ucTemp = DateDifference(rec_timestamp,Device.TimeStamp);
					if(u.i[1] - u.i[0] == 0)
					{	//PutStringUsb("\r\nRecord Download Complete");
						continue;//for loop skip 1 time
					}
					if(u.i[1] - u.i[0] >= 172800)
					{	PutStringUsb("\r\nRecord Download Complete");
						break;//for loop break;
					}
					
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					
					if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
					{	PutStringUsb("\r\nRecord Download Complete");
						break;//for loop break;
					}
					
					sprintf(PasteBuff,"\r\n%d/%d/%d",u.c[0],u.c[1],u.c[2]);
					
					//time
					u.c[0] = CopyBuff[LT_SECOND_TIMEp];// = Device.TimeStamp.tm_hour;
					u.c[1] = CopyBuff[LT_SECOND_TIMEp+1];// = Device.TimeStamp.tm_min;
					u.c[2] = CopyBuff[LT_SECOND_TIMEp+2];// = Device.TimeStamp.tm_sec;
					sprintf(MsgBuff,"\t%d:%d:%d",u.c[0],u.c[1],u.c[2]);
					strcat(PasteBuff,MsgBuff);
					
					//cycle
					u.i[0] = 0;//CycleCounter;
					u.c[0] = CopyBuff[LT_SECOND_CYCLEp];
					u.c[1] = CopyBuff[LT_SECOND_CYCLEp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//step
					//CopyBuff[LT_SECOND_STEPp] = CurrentProcessStep;
					sprintf(MsgBuff,"\t%d",CopyBuff[LT_SECOND_STEPp]);
					strcat(PasteBuff,MsgBuff);
					
					//process run sec
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_RUN_SECp];
					u.c[1] = CopyBuff[LT_SECOND_RUN_SECp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//PAE volt
					u.i[0] = 0;//CurrentPAEvolt;
					u.c[0] = CopyBuff[LT_SECOND_PAE_VOLTp];
					u.c[1] = CopyBuff[LT_SECOND_PAE_VOLTp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Total Flow
					//u.i[0] = AtlasReplacement.TotalFlow;
					u.c[0] = CopyBuff[LT_SECOND_T_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_T_FLOWp+1];
					u.c[2] = CopyBuff[LT_SECOND_T_FLOWp+2];
					u.c[3] = CopyBuff[LT_SECOND_T_FLOWp+3];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Current Flow
					u.i[0] = 0;//AtlasReplacement.CurrentFlow;
					u.c[0] = CopyBuff[LT_SECOND_CURRENT_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_CURRENT_FLOWp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// EC
					u.i[0] = 0;//AtlasConductivity;
					u.c[0] = CopyBuff[LT_SECOND_AVERAGE_ECp];
					u.c[1] = CopyBuff[LT_SECOND_AVERAGE_ECp+1];
					u.c[2] = CopyBuff[LT_SECOND_AVERAGE_ECp+2];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					//Alarm
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_ALARMp];
					u.c[1] = CopyBuff[LT_SECOND_ALARMp+1];
					u.c[2] = CopyBuff[LT_SECOND_ALARMp+2];
					sprintf(MsgBuff,"\t%xh",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					switch(respond_to){
						case USB_COM: PutStringUsb(PasteBuff);break;
						case EMAIL:   PutStringGprs(PasteBuff);break;
					}
				}
			}
		/*	else if(condition == SPECIFIC_DATE_TIME)
			{	strcpy(PasteBuff,"\r\nDate\tTime\tCycle\tStep\tP.Sec\tPAEvolt\tTot.Flow\tCur.Flow\tEC\tAlarm");
				switch(respond_to){
					case USB_COM: PutStringUsb(PasteBuff);break;
					case EMAIL:   PutStringGprs(PasteBuff);break;
				}
				ut.c[3] = SpecificReportDateTime%100;//hour
				SpecificReportDateTime/=100;
				ut.c[2] = SpecificReportDateTime%100;//date
				SpecificReportDateTime/=100;
				ut.c[1] = SpecificReportDateTime%100;//mon
				SpecificReportDateTime/=100;
				ut.c[0] = SpecificReportDateTime%100;//year
				
				for(log_rec_add=LogPtr-SECOND_LOG_PACKET_SIZE;log_rec_add != LogPtr;log_rec_add-=SECOND_LOG_PACKET_SIZE)
				{	
                    #ifdef WATCH_DOG_TIMER_ENABLED
                        RESET_WATCH_DOG_TIMER;
                    #endif
                    
					if(SecondChangeFlag == TRUE){
						SecondChangeFlag = FALSE;
						MAIN_PROG_LED_TOGGLE;
						strcpy(LcdLine1,"Uploading Data!!");
						strcpy(LcdLine2,"Please Wait.....");
						SendSerialHmiPacket();
					}
					
					if(log_rec_add < SECOND_LOG_START_ADD)
						log_rec_add = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
					
					DirectMemRead(log_rec_add,CopyBuff,SECOND_LOG_PACKET_SIZE);
					
					//date
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					//time
					u.c[3] = CopyBuff[LT_SECOND_TIMEp];// = Device.TimeStamp.tm_hour;
					u.c[4] = CopyBuff[LT_SECOND_TIMEp+1];// = Device.TimeStamp.tm_min;
					u.c[5] = CopyBuff[LT_SECOND_TIMEp+2];// = Device.TimeStamp.tm_sec;
					//case record date time hour is greater than needed date time hour
					if((u.c[0] > ut.c[0])||(u.c[0]==ut.c[0]&&u.c[1]>ut.c[1])||(u.c[0]==ut.c[0]&&u.c[1]==ut.c[1]&&u.c[2]>ut.c[2])||(u.c[0]==ut.c[0]&&u.c[1]==ut.c[1]&&u.c[2]==ut.c[2]&&u.c[3]>ut.c[3])) 
						continue;
					//case record date time hour is less than needed date time hour
					if((u.c[0] < ut.c[0])||(u.c[0]==ut.c[0]&&u.c[1]<ut.c[1])||(u.c[0]==ut.c[0]&&u.c[1]==ut.c[1]&&u.c[2]<ut.c[2])||(u.c[0]==ut.c[0]&&u.c[1]==ut.c[1]&&u.c[2]==ut.c[2]&&u.c[3]<ut.c[3])) 
					{	PutStringUsb("\r\nRecord Download Complete");
						break;//for loop break;
					}
					if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
					{	PutStringUsb("\r\nRecord Download Complete");
						break;//for loop break;
					}
					sprintf(PasteBuff,"\r\n%d/%d/%d\t%d:%d:%d",u.c[0],u.c[1],u.c[2],u.c[3],u.c[4],u.c[5]);
					
					//cycle
					u.i[0] = 0;//CycleCounter;
					u.c[0] = CopyBuff[LT_SECOND_CYCLEp];
					u.c[1] = CopyBuff[LT_SECOND_CYCLEp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//step
					//PasteBuff[LT_SECOND_STEPp] = CurrentProcessStep;
					sprintf(MsgBuff,"\t%d",CopyBuff[LT_SECOND_STEPp]);
					strcat(PasteBuff,MsgBuff);
					
					//process run sec
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_RUN_SECp];
					u.c[1] = CopyBuff[LT_SECOND_RUN_SECp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//PAE volt
					u.i[0] = 0;//CurrentPAEvolt;
					u.c[0] = CopyBuff[LT_SECOND_PAE_VOLTp];
					u.c[1] = CopyBuff[LT_SECOND_PAE_VOLTp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Total Flow
					//u.i[0] = AtlasReplacement.TotalFlow;
					u.c[0] = CopyBuff[LT_SECOND_T_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_T_FLOWp+1];
					u.c[2] = CopyBuff[LT_SECOND_T_FLOWp+2];
					u.c[3] = CopyBuff[LT_SECOND_T_FLOWp+3];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Current Flow
					u.i[0] = 0;//AtlasReplacement.CurrentFlow;
					u.c[0] = CopyBuff[LT_SECOND_CURRENT_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_CURRENT_FLOWp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// EC
					u.i[0] = 0;//AtlasConductivity;
					u.c[0] = CopyBuff[LT_SECOND_CURRENT_ECp];
					u.c[1] = CopyBuff[LT_SECOND_CURRENT_ECp+1];
					u.c[2] = CopyBuff[LT_SECOND_CURRENT_ECp+2];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					//Alarm
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_ALARMp];
					u.c[1] = CopyBuff[LT_SECOND_ALARMp+1];
					u.c[2] = CopyBuff[LT_SECOND_ALARMp+2];
					sprintf(MsgBuff,"\t%xh",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					switch(respond_to){
						case USB_COM: PutStringUsb(PasteBuff);break;
						case EMAIL:   PutStringGprs(PasteBuff);break;
					}
				}
			}*/
            else if(condition == REBOOT_LOG)
            {	strcpy(PasteBuff,"\r\nDate\tTime\tCycle\tStep\tP.Sec\tPAEvolt\tTot.Flow\tCur.Flow\tEC\tAlarm");
				switch(respond_to){
					case USB_COM: PutStringUsb(PasteBuff);break;
					case EMAIL:   PutStringGprs(PasteBuff);break;
				}
				for(log_rec_add=LogPtr-SECOND_LOG_PACKET_SIZE;log_rec_add != LogPtr;log_rec_add-=SECOND_LOG_PACKET_SIZE)
				{	
                    #ifdef WATCH_DOG_TIMER_ENABLED
                        RESET_WATCH_DOG_TIMER;
                    #endif
                    if(SecondChangeFlag == TRUE){
						SecondChangeFlag = FALSE;
						MAIN_PROG_LED_TOGGLE;
						/*if(Mode == MODE_AUTO){
							CheckKeyPress();
							AutoModeOperations();
							UpdateOutput();
							DisplayMenu();
						}*/
						strcpy(LcdLine1,"Uploading Data!!");
						strcpy(LcdLine2,"Please Wait.....");
						SendSerialHmiPacket();
					}
					if(log_rec_add < SECOND_LOG_START_ADD)
						log_rec_add = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
					
					DirectMemRead(log_rec_add,CopyBuff,SECOND_LOG_PACKET_SIZE);
					
					//date
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					
					temp_timestamp.tm_year = u.c[0];
					temp_timestamp.tm_mon = u.c[1];
					temp_timestamp.tm_mday = u.c[2];
					temp_timestamp.tm_hour = 0;
					temp_timestamp.tm_min = 0;
					temp_timestamp.tm_sec = 0;
					
					_ttemp = TimeStamp2Sec(temp_timestamp);
					u.i[0] = _ttemp;
					
					temp_timestamp.tm_year = Device.TimeStamp.tm_year;
					temp_timestamp.tm_mon = Device.TimeStamp.tm_mon;
					temp_timestamp.tm_mday = Device.TimeStamp.tm_mday;
					temp_timestamp.tm_hour = 0;
					temp_timestamp.tm_min = 0;
					temp_timestamp.tm_sec = 0;
					_ttemp = TimeStamp2Sec(temp_timestamp);
					u.i[1] = _ttemp;
					
					//ucTemp = DateDifference(rec_timestamp,Device.TimeStamp);
					if(CopyBuff[LT_SECOND_STEPp] != 0xFF)
					{	continue;//not reboot record, for loop skip 1 time
					}
                    /*
					if(u.i[1] - u.i[0] >= 172800)
					{	PutStringUsb("\r\nRecord Download Complete");
						break;//for loop break;
					}
					*/
					u.c[0] = CopyBuff[LT_SECOND_DATEp];// = Device.TimeStamp.tm_year;
					u.c[1] = CopyBuff[LT_SECOND_DATEp+1];// = Device.TimeStamp.tm_mon;
					u.c[2] = CopyBuff[LT_SECOND_DATEp+2];// = Device.TimeStamp.tm_mday;
					
					if(u.c[1] == 0 || u.c[1] > 12)//in case of blank memory
					{	PutStringUsb("\r\nRecord Download Complete");
						break;//for loop break;
					}
					
					sprintf(PasteBuff,"\r\n%d/%d/%d",u.c[0],u.c[1],u.c[2]);
					
					//time
					u.c[0] = CopyBuff[LT_SECOND_TIMEp];// = Device.TimeStamp.tm_hour;
					u.c[1] = CopyBuff[LT_SECOND_TIMEp+1];// = Device.TimeStamp.tm_min;
					u.c[2] = CopyBuff[LT_SECOND_TIMEp+2];// = Device.TimeStamp.tm_sec;
					sprintf(MsgBuff,"\t%d:%d:%d",u.c[0],u.c[1],u.c[2]);
					strcat(PasteBuff,MsgBuff);
					
					//cycle
					u.i[0] = 0;//CycleCounter;
					u.c[0] = CopyBuff[LT_SECOND_CYCLEp];
					u.c[1] = CopyBuff[LT_SECOND_CYCLEp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//step
					//CopyBuff[LT_SECOND_STEPp] = CurrentProcessStep;
					sprintf(MsgBuff,"\t%d",CopyBuff[LT_SECOND_STEPp]);
					strcat(PasteBuff,MsgBuff);
					
					//process run sec
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_RUN_SECp];
					u.c[1] = CopyBuff[LT_SECOND_RUN_SECp+1];
					sprintf(MsgBuff,"\t%d",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					//PAE volt
					u.i[0] = 0;//CurrentPAEvolt;
					u.c[0] = CopyBuff[LT_SECOND_PAE_VOLTp];
					u.c[1] = CopyBuff[LT_SECOND_PAE_VOLTp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Total Flow
					//u.i[0] = AtlasReplacement.TotalFlow;
					u.c[0] = CopyBuff[LT_SECOND_T_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_T_FLOWp+1];
					u.c[2] = CopyBuff[LT_SECOND_T_FLOWp+2];
					u.c[3] = CopyBuff[LT_SECOND_T_FLOWp+3];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// Current Flow
					u.i[0] = 0;//AtlasReplacement.CurrentFlow;
					u.c[0] = CopyBuff[LT_SECOND_CURRENT_FLOWp];
					u.c[1] = CopyBuff[LT_SECOND_CURRENT_FLOWp+1];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					// EC
					u.i[0] = 0;//AtlasConductivity;
					u.c[0] = CopyBuff[LT_SECOND_AVERAGE_ECp];
					u.c[1] = CopyBuff[LT_SECOND_AVERAGE_ECp+1];
					u.c[2] = CopyBuff[LT_SECOND_AVERAGE_ECp+2];
					sprintf(MsgBuff,"\t%.2f",(float)u.i[0]/100);
					strcat(PasteBuff,MsgBuff);
					
					//Alarm
					u.i[0] = 0;
					u.c[0] = CopyBuff[LT_SECOND_ALARMp];
					u.c[1] = CopyBuff[LT_SECOND_ALARMp+1];
					u.c[2] = CopyBuff[LT_SECOND_ALARMp+2];
					sprintf(MsgBuff,"\t%xh",u.i[0]);
					strcat(PasteBuff,MsgBuff);
					
					switch(respond_to){
						case USB_COM: PutStringUsb(PasteBuff);break;
						case EMAIL:   PutStringGprs(PasteBuff);break;
					}
				}
			}
		break;
		case DAILY_LOG_TABLE_START_PAGE:
			strcpy(PasteBuff,"\r\nDate\tAvgPV\tAvgWV\tT.Flow\tAvgEC\tT.Cyc\tAlarm\r\n");
			switch(respond_to){
				case USB_COM: PutStringUsb(PasteBuff);break;
				case EMAIL:   PutStringGprs(PasteBuff);break;
			}
			for(log_rec_add = 1;log_rec_add<32;log_rec_add++)
			{	
                #ifdef WATCH_DOG_TIMER_ENABLED
                    RESET_WATCH_DOG_TIMER;
                #endif
                DirectMemRead(DAILY_LOG_START_ADD+log_rec_add*DAILY_LOG_PACKET_SIZE,CopyBuff,DAILY_LOG_PACKET_SIZE);
				if(CopyBuff[LT_DAILY_DATEp] < 1 || CopyBuff[LT_DAILY_DATEp+1] > 99)//year
					continue;
				if(CopyBuff[LT_DAILY_DATEp+1] < 1 || CopyBuff[LT_DAILY_DATEp+1] > 12)//month
					continue;
				if(CopyBuff[LT_DAILY_DATEp+2] < 1 || CopyBuff[LT_DAILY_DATEp+2] > 31)//date
					continue;
				//u.i[0] = total_flow;
				u.c[0] = CopyBuff[LT_DAILY_T_FLOWp];
				u.c[1] = CopyBuff[LT_DAILY_T_FLOWp+1];
				u.c[2] = CopyBuff[LT_DAILY_T_FLOWp+2];
				u.c[3] = CopyBuff[LT_DAILY_T_FLOWp+3];
				uiTemp = u.i[0];
				
				u.i[1] = 0;
				u.c[4] = CopyBuff[LT_DAILY_AVG_ECp];
				u.c[5] = CopyBuff[LT_DAILY_AVG_ECp+1];
				u.c[6] = CopyBuff[LT_DAILY_AVG_ECp+2];
				iTemp = u.i[1];
				
				u.i[1] = 0;//total_cycles;
				u.c[4] = CopyBuff[LT_DAILY_T_CYCLEp];
				u.c[5] = CopyBuff[LT_DAILY_T_CYCLEp+1];
				
				//Alarm
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_DAILY_ALARMp];
				u.c[1] = CopyBuff[LT_DAILY_ALARMp+1];
				u.c[2] = CopyBuff[LT_DAILY_ALARMp+2];
				
				sprintf(PasteBuff,"\r\n%d/%d/%d\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%Xh",
						CopyBuff[LT_DAILY_DATEp],
						CopyBuff[LT_DAILY_DATEp+1],
						CopyBuff[LT_DAILY_DATEp+2],
						(float)CopyBuff[LT_DAILY_AVG_P_VOLTp]/100,
						(float)CopyBuff[LT_DAILY_AVG_W_VOLTp]/100,
						(float)uiTemp/100,
						(float)iTemp/100,
						u.i[1],
						u.i[0]);
				
				switch(respond_to){
					case USB_COM: PutStringUsb(PasteBuff);break;
					case EMAIL:   PutStringGprs(PasteBuff);break;
				}
			}
		break;
		/*
		case MONTHLY_LOG_TABLE_START_PAGE:
			strcpy(CopyBuff,"\r\nDate\tAvgPV\tAvgWV\tT.Flow\tAvgEC\tT.Cyc\tAlarm\r\n");
			switch(respond_to){
				case USB_COM: PutStringUsb(CopyBuff);break;
				case EMAIL:   PutStringGprs(CopyBuff);break;
			}
			for(log_rec_add = 1;log_rec_add<32;log_rec_add++)
			{	
                #ifdef WATCH_DOG_TIMER_ENABLED
                    RESET_WATCH_DOG_TIMER;
                #endif
                DirectMemRead(MONTHLY_LOG_START_ADD+log_rec_add*MONTHLY_LOG_PACKET_SIZE,PasteBuff,MONTHLY_LOG_PACKET_SIZE);
				if(PasteBuff[LT_MONTHLY_DATEp] < 1 || PasteBuff[LT_MONTHLY_DATEp+1] > 99)//year 
					continue;
				if(PasteBuff[LT_MONTHLY_DATEp+1] < 1 || PasteBuff[LT_MONTHLY_DATEp+1] > 12)//month
					continue;
				//u.i[0] = total_flow;
				u.c[0] = PasteBuff[LT_MONTHLY_T_FLOWp];
				u.c[1] = PasteBuff[LT_MONTHLY_T_FLOWp+1];
				u.c[2] = PasteBuff[LT_MONTHLY_T_FLOWp+2];
				u.c[3] = PasteBuff[LT_MONTHLY_T_FLOWp+3];
				uiTemp = u.i[0];
				
				u.i[1] = 0;
				u.c[4] = PasteBuff[LT_MONTHLY_AVG_ECp];
				u.c[5] = PasteBuff[LT_MONTHLY_AVG_ECp+1];
				u.c[6] = PasteBuff[LT_MONTHLY_AVG_ECp+2];
				iTemp = u.i[1];
				
				u.i[1] = 0;//total_cycles;
				u.c[4] = PasteBuff[LT_MONTHLY_T_CYCLEp];
				u.c[5] = PasteBuff[LT_MONTHLY_T_CYCLEp+1];
				
				//Alarm
				u.i[0] = 0;
				u.c[0] = PasteBuff[LT_SECOND_ALARMp];
				u.c[1] = PasteBuff[LT_SECOND_ALARMp+1];
				u.c[2] = PasteBuff[LT_SECOND_ALARMp+2];
				
				sprintf(CopyBuff,"\r\n%d/%d\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%Xh",
						PasteBuff[LT_MONTHLY_DATEp],
						PasteBuff[LT_MONTHLY_DATEp+1],
						(float)PasteBuff[LT_MONTHLY_AVG_P_VOLTp]/100,
						(float)PasteBuff[LT_MONTHLY_AVG_W_VOLTp]/100,
						(float)uiTemp/100,
						(float)iTemp/100,
						u.i[1],
						u.i[0]);
				
				switch(respond_to){
					case USB_COM: PutStringUsb(CopyBuff);break;
					case EMAIL:   PutStringGprs(CopyBuff);break;
				}
			}
		break;
		case YEARLY_LOG_TABLE_START_PAGE:
			strcpy(CopyBuff,"\r\nYear\tAvgPV\tAvgWV\tT.Flow\tAvgEC\tT.Cyc\tAlarm\r\n");
			switch(respond_to){
				case USB_COM: PutStringUsb(CopyBuff);break;
				case EMAIL:   PutStringGprs(CopyBuff);break;
			}
			for(log_rec_add = 0;log_rec_add<100;log_rec_add++)
			{	
                #ifdef WATCH_DOG_TIMER_ENABLED
                    RESET_WATCH_DOG_TIMER;
                #endif
                DirectMemRead(YEARLY_LOG_START_ADD+log_rec_add*YEARLY_LOG_PACKET_SIZE,PasteBuff,YEARLY_LOG_PACKET_SIZE);
				if(PasteBuff[LT_YEARLYp] < 1 || PasteBuff[LT_YEARLYp] > 99)//year
					continue;
				//u.i[0] = total_flow;
				u.c[0] = PasteBuff[LT_YEARLY_T_FLOWp];
				u.c[1] = PasteBuff[LT_YEARLY_T_FLOWp+1];
				u.c[2] = PasteBuff[LT_YEARLY_T_FLOWp+2];
				u.c[3] = PasteBuff[LT_YEARLY_T_FLOWp+3];
				uiTemp = u.i[0];
				
				u.i[1] = 0;
				u.c[4] = PasteBuff[LT_YEARLY_AVG_ECp];
				u.c[5] = PasteBuff[LT_YEARLY_AVG_ECp+1];
				u.c[6] = PasteBuff[LT_YEARLY_AVG_ECp+2];
				iTemp = u.i[1];
				
				u.i[1] = 0;//total_cycles;
				u.c[4] = PasteBuff[LT_YEARLY_T_CYCLEp];
				u.c[5] = PasteBuff[LT_YEARLY_T_CYCLEp+1];
				u.c[6] = PasteBuff[LT_YEARLY_T_CYCLEp+2];
				
				//Alarm
				u.i[0] = 0;
				u.c[0] = PasteBuff[LT_SECOND_ALARMp];
				u.c[1] = PasteBuff[LT_SECOND_ALARMp+1];
				u.c[2] = PasteBuff[LT_SECOND_ALARMp+2];
				
				sprintf(CopyBuff,"\r\n%d\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%Xh",
						PasteBuff[LT_YEARLYp],
						(float)PasteBuff[LT_YEARLY_AVG_P_VOLTp]/100,
						(float)PasteBuff[LT_YEARLY_AVG_W_VOLTp]/100,
						(float)uiTemp/100,
						(float)iTemp/100,
						u.i[1],
						u.i[0]);
				switch(respond_to){
					case USB_COM: PutStringUsb(CopyBuff);break;
					case EMAIL:   PutStringGprs(CopyBuff);break;
				}
			}
		break;
		*/
	}
}
/*
void CheckTriggers()
{	uc byte,bit;
  	if((MobileStage == MOBILE_STAGE_OPERATING_MODE || MobileStage == MOBILE_STAGE_READY_TO_OPEN_CONNECTION) && GprsDataModeFlag == FALSE)
	{	for(byte=0;byte<(MAX_PROJECT_PARAMETER/8);byte++)
		{	for(bit=0;bit<8 && PidTPR[byte]>0;bit++)
			{	if((PidTPR[byte] & (1<<bit)) > 0)
				{	DirectMemRead(PRODUCT_REG_NOa,PasteBuff,PRODUCT_REG_NOn);
					PasteBuff[PRODUCT_REG_NOn] = '\0';
					strcat(PasteBuff,Gmt2LocalTimeStamp(Device.TimeStamp));
					//sprintf(CopyBuff,"\r\nPidTPR= %d",byte*8+bit);
					//PutStringUsb(CopyBuff);
					DirectMemRead(PP_START_LOC+(byte*8+bit)*PP_MAX_BYTE+PP_MSGp,CopyBuff,PP_MSGn);
					CopyBuff[PP_MSGn] = '\0'; //just in case message is equal to mem alocatted size
					strcat(PasteBuff,"\r\n");
					strcat(PasteBuff,CopyBuff);
					//PutStringUsb(" SMS MESSAGE> ");
					//PutStringUsb(PasteBuff);
					if(((byte*8+bit) >= MY_PID_DI_START) && ((byte*8+bit) <= MY_PID_DI_END))
					{	GetPidMem2Var(byte*8+bit);
					  	DirectMemRead(INPUT_TABLE_START_ADDRESS+((byte*8+bit)-MY_PID_DI_START)*INPUT_TABLE_RECORD_SIZE,CopyBuff,INPUT_TABLE_RECORD_SIZE);
						CopyBuff[IT_REG_BIT_ADDRESSp] = '\0';
						strcat(PasteBuff,"\r\n");
						strcat(PasteBuff,CopyBuff);
				  		DirectMemRead(PP_START_LOC+(byte*8+bit)*PP_MAX_BYTE+PP_TRIG_PRIORITY_CONDITIONp,u.c,PP_TRIG_PRIORITY_CONDITIONn);
						sprintf(CopyBuff," Current Status %s",
								((CurDigitalInput & (1<<((byte*8+bit)-MY_PID_DI_START))) > 0)? "HIGH":"LOW");
						strcat(PasteBuff,CopyBuff);
					}
					else if((byte*8+bit) == MY_PID_TIME)
					{	sprintf(CopyBuff,"\r\nIMEI No: %lld",GprsPacket.Imei);
						strcat(PasteBuff,CopyBuff);
					}
					else if((byte*8+bit) == MY_PID_AI1)
					{	sprintf(CopyBuff,"\r\nFuel(%dmV)=%d%%",
								Ana1AV,//(ui)((Ana1DV*VOLT_STEP_SIZE*ANALOG_VOLTAGE_DIVIDER)*1000),
								((Ana1AVLastLogVal-Ana1AV)*100)/(Ana1_MaxMilliVolt-Ana1_MinMilliVolt));
						strcat(PasteBuff,CopyBuff);
					}
					sprintf(CopyBuff,"\r\nhttp://maps.google.com/?q=%.6f,%.6f",
							GprsPacket.Lat,
							GprsPacket.Lng);
					strcat(PasteBuff,CopyBuff);
					//PutStringUsb(PasteBuff);//TESTING CODE
					SendTriggerResponseToUser((byte*8+bit),PasteBuff);
					//if((GprsCR & CALL_CONTROL) > 0)
					//{	if(TriggerPriorityVal == TRIG_PRIORITY_PANIC)
					//	{	FindUserToPanicCall(TriggerPid);
					//		//first method:	FindUserToPanicCall(TriggerPid);
					//		//Second
					//	}
					//}
					PidTPR[byte] &= ~(1<<bit);	
				}
			}
		}
		if(CallResponsePendingFlag == TRUE)
		{	CallResponsePendingFlag = FALSE;
			SendStatusThroughSMS(CallResponsePendingCaller);
		}
	}
	TriggerPriorityVal = 0;
	TriggerPid = 0;
}*/
////////////////////////////////////////////////////////////////////////////////
void PeriodicSave()
{	//LogRecord();
	//PutStringUsb("\r\nPeriodic Save variable SAVED");//TESTING CODE
  	
  	PageToBuffer(PERIODIC_SAVE_PAGE,PAGE_TO_BUFF1);
	
	//PasteBuff[PERIODIC_SAVE_MINa % FLASH_PAGE_SIZE] = PeriodicSaveMin;
	
	_ttemp = TimeStamp2Sec(Device.TimeStamp);
	_ttemp += SecAfterTimeSync;
	TimeStampPtr = localtime(&_ttemp);
	TimeStampPtr->tm_year -= 100;
	TimeStampPtr->tm_mon += 1; 	
	
	//u.i[0] = Dt.Date;
	PasteBuff[POWER_OFF_DATEa % FLASH_PAGE_SIZE] = TimeStampPtr->tm_year;
	PasteBuff[(POWER_OFF_DATEa+1) % FLASH_PAGE_SIZE] = TimeStampPtr->tm_mon;
	PasteBuff[(POWER_OFF_DATEa+2) % FLASH_PAGE_SIZE] = TimeStampPtr->tm_mday;
	//u.i[0] = Dt.Time;
	PasteBuff[POWER_OFF_TIMEa % FLASH_PAGE_SIZE] = TimeStampPtr->tm_hour;
	PasteBuff[(POWER_OFF_TIMEa+1) % FLASH_PAGE_SIZE] = TimeStampPtr->tm_min;
	PasteBuff[(POWER_OFF_TIMEa+2) % FLASH_PAGE_SIZE] = TimeStampPtr->tm_sec;
	
	u.i[0] = CIP_PumpRunTimeInSec;
	PasteBuff[CIP_PUMP_RUNTIMEa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(CIP_PUMP_RUNTIMEa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
	u.i[0] = CycleCounter;
	PasteBuff[LAST_CYCLE_COUNTER_FOR_CIPa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(LAST_CYCLE_COUNTER_FOR_CIPa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
	u.i[0] = OptimizedPAE_CurrentForPrepureStep;// OptimizedPAE_CurrentForPrepureStep. Divide it by 100
	PasteBuff[OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
    u.i[0] = OptimizedPAE_CurrentForPureStep;// OptimizedPAE_CurrentForPureStep. Divide it by 100
	PasteBuff[OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
	u.i[0] = OptimizedPAE_CurrentForWasteStep;// OptimizedPAE_CurrentForWasteStep. Divide it by 100
	PasteBuff[OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
	PasteBuff[MODEa % FLASH_PAGE_SIZE] = Mode;
	
	u.i[0] = LastCyclePureAverageConductivity;// LastCyclePureAverageConductivity. Divide it by 100
	PasteBuff[LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa+1) % FLASH_PAGE_SIZE] = u.c[1];
	PasteBuff[(LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa+2) % FLASH_PAGE_SIZE] = u.c[2];
	
	u.i[0] = (ui)(PureVolumeOfBatchJustCompleted*100);// PureVolumeOfBatchJustCompleted. Divide it by 100
	PasteBuff[LAST_CYCLE_PURE_VOLUMEa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(LAST_CYCLE_PURE_VOLUMEa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
	u.i[0] = (ui)(WasteVolumeOfBatchJustCompleted*100);// WasteVolumeOfBatchJustCompleted. Divide it by 100
	PasteBuff[LAST_CYCLE_WASTE_VOLUMEa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(LAST_CYCLE_WASTE_VOLUMEa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
	u.i[0] = CIP_DosingRunTimeInSec;// CIP_DosingRunTimeInSec
	PasteBuff[CIP_DOSING_RUNTIME_IN_SECa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(CIP_DOSING_RUNTIME_IN_SECa+1) % FLASH_PAGE_SIZE] = u.c[1];
	
    #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
	u.i[0] = CIP_PulseRunTimeInSec;// CIP_PulseRunTimeInSec
	PasteBuff[CIP_PULSE_RUNTIME_IN_SECa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(CIP_PULSE_RUNTIME_IN_SECa+1) % FLASH_PAGE_SIZE] = u.c[1];
    #endif
    
    PasteBuff[(PERCENT_RECOVERYa) % FLASH_PAGE_SIZE] = PercentageRecovery[0];
    PasteBuff[(PERCENT_RECOVERYa+1) % FLASH_PAGE_SIZE] = PercentageRecovery[1];
    PasteBuff[(PERCENT_RECOVERYa+2) % FLASH_PAGE_SIZE] = PercentageRecovery[2];
    PasteBuff[(PERCENT_RECOVERYa+3) % FLASH_PAGE_SIZE] = PercentageRecovery[3];
    PasteBuff[(PERCENT_RECOVERYa+4) % FLASH_PAGE_SIZE] = PercentageRecovery[4];
    
    u.i[0] = LastFiveCyclePureAverageConductivitySamples[0];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa) % FLASH_PAGE_SIZE] = u.c[0];//LastFiveCyclePureAverageConductivitySamples[0];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+1) % FLASH_PAGE_SIZE] = u.c[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+2) % FLASH_PAGE_SIZE] = u.c[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+3) % FLASH_PAGE_SIZE] = u.c[3];
    
    u.i[0] = LastFiveCyclePureAverageConductivitySamples[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+4) % FLASH_PAGE_SIZE] = u.c[0];//LastFiveCyclePureAverageConductivitySamples[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+5) % FLASH_PAGE_SIZE] = u.c[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+6) % FLASH_PAGE_SIZE] = u.c[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+7) % FLASH_PAGE_SIZE] = u.c[3];
    
    u.i[0] = LastFiveCyclePureAverageConductivitySamples[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+8) % FLASH_PAGE_SIZE] = u.c[0];//LastFiveCyclePureAverageConductivitySamples[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+9) % FLASH_PAGE_SIZE] = u.c[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+10) % FLASH_PAGE_SIZE] = u.c[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+11) % FLASH_PAGE_SIZE] = u.c[3];
    
    u.i[0] = LastFiveCyclePureAverageConductivitySamples[3];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+12) % FLASH_PAGE_SIZE] = u.c[0];//LastFiveCyclePureAverageConductivitySamples[3];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+13) % FLASH_PAGE_SIZE] = u.c[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+14) % FLASH_PAGE_SIZE] = u.c[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+15) % FLASH_PAGE_SIZE] = u.c[3];
    
    u.i[0] = LastFiveCyclePureAverageConductivitySamples[4];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+16) % FLASH_PAGE_SIZE] = u.c[0];//LastFiveCyclePureAverageConductivitySamples[4];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+17) % FLASH_PAGE_SIZE] = u.c[1];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+18) % FLASH_PAGE_SIZE] = u.c[2];
    PasteBuff[(LAST_FIVE_CYCLE_AVG_ECa+19) % FLASH_PAGE_SIZE] = u.c[3];
    
    //PasteBuff[BALANCE_CHECK_FLAGa % FLASH_PAGE_SIZE] = BalanceCheckFlag;
	
	//update log pointer into memory PERIODIC SAVE PAGE
	u.i[0] = EmailPtr;
	PasteBuff[EMAIL_PTRa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(EMAIL_PTRa+1) % FLASH_PAGE_SIZE] = u.c[1];
	PasteBuff[(EMAIL_PTRa+2) % FLASH_PAGE_SIZE] = u.c[2];
	
	u.i[0] = LogPtr;
	PasteBuff[LOG_PTRa % FLASH_PAGE_SIZE] = u.c[0];
	PasteBuff[(LOG_PTRa+1) % FLASH_PAGE_SIZE] = u.c[1];
	PasteBuff[(LOG_PTRa+2) % FLASH_PAGE_SIZE] = u.c[2];
	
    u.i[0] = UploadPtr;
	PasteBuff[((UPLOAD_PTR_PROFILE1a) % FLASH_PAGE_SIZE)] = u.c[0];
	PasteBuff[((UPLOAD_PTR_PROFILE1a) % FLASH_PAGE_SIZE)+1] = u.c[1];
	PasteBuff[((UPLOAD_PTR_PROFILE1a) % FLASH_PAGE_SIZE)+2] = u.c[2];
    
    PasteBuff[LAST_STEP_B4_POWER_OFFa % FLASH_PAGE_SIZE] = CurrentProcessStep;
    
    u.i[0] = ProcessStepRunSec;
    PasteBuff[LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE] = u.c[0];
    PasteBuff[(LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE) + 1] = u.c[1];
    PasteBuff[(LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE) + 2] = u.c[2];
    PasteBuff[(LAST_RUN_SEC_B4_POWER_OFFa % FLASH_PAGE_SIZE) + 3] = u.c[3];
    
    u.i[0] = (ui)(TotalPureVolume * 100);
	PasteBuff[T_FLOWa % FLASH_PAGE_SIZE] = u.c[0];
    PasteBuff[(T_FLOWa % FLASH_PAGE_SIZE) + 1] = u.c[1];
    PasteBuff[(T_FLOWa % FLASH_PAGE_SIZE) + 2] = u.c[2];
    PasteBuff[(T_FLOWa % FLASH_PAGE_SIZE) + 3] = u.c[3];
    
    ReadWriteMemBuffer(MEM_WRITE,PERIODIC_SAVE_PAGE_START_ADDa,PasteBuff,FLASH_PAGE_SIZE);//all the changes save to buffer
	BufferToPage(BUFF1_TO_PAGE,PERIODIC_SAVE_PAGE);//buffer to page

	MinRecCounter4PeriodicSave = 0;
}
/*
int Pid2Constant(char *str)
{	RemoveCharFromString(' ',str);
  	String2Upper(str);
					  	
	if(*str == NULL)
  		return -1;
	else if(strlen(str)==0)
	  	return -1;
	else if(strcmp(str,PID_STR_TIME)==0)
	  	return MY_PID_TIME;
	else if(strcmp(str,PID_STR_EXT_BATTERY)==0)
	  	return MY_PID_EXT_BATTERY;
	else if(strcmp(str,PID_STR_AI1)==0)
	  	return MY_PID_AI1;
	else if(strcmp(str,PID_STR_AI1)==0)
	  	return MY_PID_AI1;
	else if(strcmp(str,PID_STR_DI1)==0)
	  	return MY_PID_DI1;
	else if(strcmp(str,strcatPid(MY_PID_DI1))==0)
	  	return MY_PID_DI1;
	else if(strcmp(str,PID_STR_DI2)==0)
	  	return MY_PID_DI2;
	else if(strcmp(str,strcatPid(MY_PID_DI2))==0)
	  	return MY_PID_DI2;
	else if(strcmp(str,PID_STR_DI3)==0)
	  	return MY_PID_DI3;
	else if(strcmp(str,strcatPid(MY_PID_DI3))==0)
	  	return MY_PID_DI3;
	else if(strcmp(str,PID_STR_DI4)==0)
	  	return MY_PID_DI4;
	else if(strcmp(str,strcatPid(MY_PID_DI4))==0)
	  	return MY_PID_DI4;
	else
	  	return -1;
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char SetParameterControl(char pid,char parano)
{	//control
	String2Upper(CommandFormat.Parameter[parano]);	//change string to uppercase before comparision.
	
	if(SearchString(CommandFormat.Parameter[parano],"ENABLE")!=-1)
	{   PidMCR[pid/8] |= (1<<(pid%8));
		strcat(PasteBuff," Control:ENABLED");
		
		if(pid == MY_PID_TOW_GEOFENCE || pid == MY_PID_PARK_GEOFENCE)
		{	
		  	GetPidMem2Var(pid);
			TrigPara1Val.f = GpsCurDataInRadian.f[1];//lng
			TrigPara2Val.f = GpsCurDataInRadian.f[0];//lat
			uiTemp = (pid-MY_PID_GEOFENCE_START);
			GeofenceLastTrigVal[uiTemp/8] &= ~(1<<(uiTemp%8));
			UpdateProjectParameter2Memory(pid);	//SAVE INTO MEMORY
		}						
		cFunRetVal = SUCCESS;
	}
	else if(SearchString(CommandFormat.Parameter[parano],"DISABLE")!=-1)
	{  	PidMCR[pid/8] &= ~(1<<(pid%8));
		strcat(PasteBuff," Control:DISABLED");
		cFunRetVal = SUCCESS;
	}
	else
	{  	strcat(PasteBuff,"ERROR: Illegal Control.");
		cFunRetVal = ERROR;
	}
	if(cFunRetVal == SUCCESS)
		RandomMemWrite(PROJ_PARA_MCRa,PidMCR,PROJ_PARA_MCRn);//update pid master control register
	  
	SendCommandReplyToSender(PasteBuff);
	return cFunRetVal;
}*/
////////////////////////////////////////////////////////////////////////////////
void ReadWriteMemoryCommand(int mid, int sid, char data_format,uli address,int no_of_bytes){
	if(strcmp(CommandFormat.Para,"?")==0){//READ
		if(data_format == DT_STR){
			DirectMemRead(address,CopyBuff,no_of_bytes);
			CopyBuff[no_of_bytes] = '\0';
			sprintf(PasteBuff,",%d.%d=%s",mid,sid,CopyBuff);
		}
		else{
			u.ll = 0;
			DirectMemRead(address,u.c,no_of_bytes);
			switch(data_format){
				case DT_INT: sprintf(PasteBuff,",%d.%d=%d",mid,sid,u.i[0]); break;
				case DT_FLOAT: sprintf(PasteBuff,",%d.%d=%f",mid,sid,u.f[0]);break;
				case DT_DOUBLE: sprintf(PasteBuff,",%d.%d=%lf",mid,sid,u.d);break;
				//case DT_LONG: sprintf(PasteBuff,",%d.%d=%ld",mid,sid,u.l);break;
				case DT_LONG_LONG: sprintf(PasteBuff,",%d.%d=%lld",mid,sid,u.ll);break;
			}
		}
	}
	else{//WRITE
		uiTemp = strlen(CommandFormat.Para);
		if(strlen(CommandFormat.Para)>0){
			if(data_format == DT_STR){
				if(uiTemp < no_of_bytes)//padding to be done by NULL
                {   for(;uiTemp < no_of_bytes;uiTemp++)
                        *(CommandFormat.Para+uiTemp) = '\0';
                }
                RandomMemWrite(address,CommandFormat.Para,no_of_bytes);
				DirectMemRead(address,CopyBuff,no_of_bytes);
				CopyBuff[no_of_bytes] = '\0';
				sprintf(PasteBuff,",%d.%d=%s",mid,sid,CopyBuff);
			}
			else{
				switch(data_format){
					case DT_INT: sscanf(CommandFormat.Para,"%d",&u.i[0]);
					case DT_FLOAT: sscanf(CommandFormat.Para,"%f",&u.f[0]);
					case DT_DOUBLE: sscanf(CommandFormat.Para,"%lf",&u.d);
					case DT_LONG_LONG: sscanf(CommandFormat.Para,"%lld",&u.ll);
				}
				//////////////// code if check is required starts from here ///////////////////////////////
				bTemp = TRUE;
				switch(address){
					case FOTA_STATUSa:	if(u.i[0] == FIRMWARE_UPGRADE_COMMAND_RECEIVED)
											FreeRunSec = 0;
										break;
					case LOG_PTRa:
					case UPLOAD_PTR_PROFILE0a:
					case UPLOAD_PTR_PROFILE1a:
					case UPLOAD_PTR_PROFILE2a:
					case UPLOAD_PTR_PROFILE3a:
					case UPLOAD_PTR_PROFILE4a:
					case UPLOAD_PTR_PROFILE5a:
					case EMAIL_PTRn:
						if(u.i[0] < SECOND_LOG_START_ADD)
							bTemp = FALSE;
						else if(u.i[0] > SECOND_LOG_END_ADD)
							bTemp = FALSE;
						else if(((u.i[0]-SECOND_LOG_START_ADD)%SECOND_LOG_PACKET_SIZE) > 0)
							bTemp = FALSE;
					break;
				}
				//////////////// code if check is required ends here ///////////////////////////////
				if(bTemp == TRUE){
					RandomMemWrite(address,u.c,no_of_bytes);
					u.ll = 0;
					DirectMemRead(address,u.c,no_of_bytes);
					switch(data_format){
						case DT_INT: sprintf(PasteBuff,",%d.%d=%d",mid,sid,u.i[0]); break;
						case DT_FLOAT: sprintf(PasteBuff,",%d.%d=%f",mid,sid,u.f[0]);break;
						case DT_DOUBLE: sprintf(PasteBuff,",%d.%d=%lf",mid,sid,u.d);break;
						//case DT_LONG: sprintf(PasteBuff,",%d.%d=%ld",mid,sid,u.l);break;
						case DT_LONG_LONG: sprintf(PasteBuff,",%d.%d=%lld",mid,sid,u.ll);break;
					}
				}
				else
					sprintf(PasteBuff,",%d.%d=!%d",mid,sid,ILLEGAL_VALUE_ERROR);
			}
			LoadSettingFlag = TRUE;
		}
	}
}
/*
void ReadWriteParameterControl(int mid, int sid,char pid){
	if(strcmp(CommandFormat.Para,"?")==0){
		sprintf(PasteBuff,",%d.%d=%d",
				mid,
				sid,
				((PidMCR[pid/8] & (1<<(pid%8)))>0)? 1:0);
	}
	else{//write
		if(strlen(CommandFormat.Para)>0){
			sscanf(CommandFormat.Para,"%d",&uiTemp);
			if(uiTemp==0){
				PidMCR[pid/8] &= ~(1<<(pid%8));//disable
			}
			else if(uiTemp==1){
				PidMCR[pid/8] |= (1<<(pid%8));
			}
		}
		else{//default value write
			switch(pid){
				case MY_PID_TIME:
									PidMCR[pid/8] |= (1<<(pid%8)); break;//enable
			}
		}
	}
}
*/
/*
void ReadWriteParameterLog(int mid,int sid,char pid){
	GetPidMem2Var(pid);
	if(strcmp(CommandFormat.Para,"?")==0){
		sprintf(PasteBuff,",%d.%d=%d",mid,sid,LogVal.i);
	}
	else{
		if(strlen(CommandFormat.Para)>0){
			sscanf(CommandFormat.Para,"%d",&LogVal.i);
			UpdateProjectParameter2Memory(pid);
		}
		else{//default value write
			switch(pid){
				case MY_PID_TIME:		LogVal.i = 180;break;
				case MY_PID_DISTANCE:	
				case MY_PID_ANGLE:	
				case MY_PID_SPEED:	 break;
			}
			UpdateProjectParameter2Memory(pid);
		}
	}
}
*/
/*
char ReadWriteParameterLogUnit(char pid)
{	GetPidMem2Var(pid);
	if(strlen(CommandFormat.Para)>0)
	{	sscanf(CommandFormat.Para,"%d",&uiTemp);
		if(uiTemp < UNIT_MAX_VAL)
			LogUnit = uiTemp;//Unit2Constant(CommandFormat.Para);
		UpdateProjectParameter2Memory(pid);
	}
	return LogUnit;
}
*/
/*
void ReadWriteParameterTrigPriority(int mid,int sid,char pid){
	GetPidMem2Var(pid);
	if(strcmp(CommandFormat.Para,"?")==0){
		sprintf(PasteBuff,",%d.%d=%d",mid,sid,TrigPriority);
	}
	else{
		if(strlen(CommandFormat.Para)>0){
			sscanf(CommandFormat.Para,"%d",&uiTemp);
			TrigPriority = uiTemp;
			UpdateProjectParameter2Memory(pid);
		}
		else{//default value write
			TrigPriority = TRIG_PRIORITY_NORMAL;
			UpdateProjectParameter2Memory(pid);
		}
	}
}
*/
/*
void ReadWriteParameterTrigCondition(int mid,int sid,char pid){
	GetPidMem2Var(pid);
	if(strcmp(CommandFormat.Para,"?")==0){
		sprintf(PasteBuff,",%d.%d=%d",mid,sid,TrigCondition);
	}
	else{
		if(strlen(CommandFormat.Para)>0){
			sscanf(CommandFormat.Para,"%d",&uiTemp);
			TrigCondition = uiTemp;
			UpdateProjectParameter2Memory(pid);
		}
		else{//default value write
			TrigCondition = TRIG_CONDITION_NONE;
			UpdateProjectParameter2Memory(pid);
		}
	}
}*/
/*
void ReadWriteParameterTrigPara1Val(int mid,int sid,char pid,char data_format){
	GetPidMem2Var(pid);
	if(strcmp(CommandFormat.Para,"?")==0){
		switch(data_format){
			case DT_INT: sprintf(PasteBuff,",%d.%d=%d",mid,sid,TrigPara1Val.i); break;
			case DT_FLOAT: sprintf(PasteBuff,",%d.%d=%f",mid,sid,TrigPara1Val.f);break;
		}
	}
	else{
		if(strlen(CommandFormat.Para)>0){
			switch(data_format){
				case DT_INT: sscanf(CommandFormat.Para,"%d",&TrigPara1Val.i);
				case DT_FLOAT: sscanf(CommandFormat.Para,"%f",&TrigPara1Val.f);
			}
			UpdateProjectParameter2Memory(pid);
		}
		else{//default value write
			TrigPara1Val.i = 0;
			UpdateProjectParameter2Memory(pid);
		}
	}
}*/
/*
void ReadWriteParameterTrigMsg(int mid,int sid,char pid){
	GetPidMem2Var(pid);
	if(strcmp(CommandFormat.Para,"?")==0){
		sprintf(PasteBuff,",%d.%d=%s",mid,sid,TrigMsg);
	}
	else{
		if(strlen(CommandFormat.Para)>0){
			CommandFormat.Para[PP_MSGn] = '\0';
			strcpy(TrigMsg,CommandFormat.Para);
			UpdateProjectParameter2Memory(pid);
		}
		else{//default value write
			strcpy(TrigMsg,"SOMETHING TRIGGERED");
			//switch(pid){
				//case MY_PID_TIME:		strcpy(TrigMsg,"SERVICE TIME REACHED");break;
				//case MY_PID_DISTANCE:	strcpy(TrigMsg,"SERVICE DISTANCE REACHED");break;
				//case MY_PID_SPEED:		strcpy(TrigMsg,"OVER SPEED");break;
				//case MY_PID_EXT_BATTERY:strcpy(TrigMsg,"EXTERNAL POWER CUT");break;
				//case MY_PID_AI1:		strcpy(TrigMsg,"ANALOG INPUT TRIGGER");break;
			//}	
			UpdateProjectParameter2Memory(pid);
		}
	}
}*/
////////////////////////////////////////////////////////////////////////////////
/*char SetParameterLog(char pid,char parano)
{	if(strlen(CommandFormat.Parameter[parano])>0)
	{  	GetPidMem2Var(pid);
		sscanf(CommandFormat.Parameter[parano],"%d",&LogVal.i);
		strcat(PasteBuff,"\r\nLog @ ");
		sprintf(CopyBuff,"%d ",LogVal.i);
		strcat(PasteBuff,CopyBuff);
		//Log Unit
		LogUnit = Unit2Constant(CommandFormat.Parameter[parano+1]);
		if(LogUnit != (char)-1)
		{  	strcatUnit(PasteBuff,LogUnit);
			cFunRetVal = SUCCESS;
		}
		else
		{  	strcat(PasteBuff," Illegal Unit.");
			cFunRetVal = ERROR;
		}
	}
	else
	{  	strcat(PasteBuff,"\r\nLog @ Illegal Value.");
		cFunRetVal = ERROR;
	}
		
	SendCommandReplyToSender(PasteBuff);
	
	if(cFunRetVal == SUCCESS)
		UpdateProjectParameter2Memory(pid);
  	return cFunRetVal;
}*/
////////////////////////////////////////////////////////////////////////////////
/*char SetParameterTrigger(char pid,char parano)
{	GetPidMem2Var(pid);
	//Trigger Priority
	TrigPriority = TrigPriority2Constant(CommandFormat.Parameter[parano]);
	if(TrigPriority == (char)-1)
	{  	strcat(PasteBuff,"\r\nIllegal Trigger Priority.");
		cFunRetVal = ERROR;
	}
	else
	{  	strcat(PasteBuff,"\r\nTrigger Priority = ");
		strcat(PasteBuff,TrigPriority2str(TrigPriority));
		//strcatTrigPriority(PasteBuff,TrigPriority);
		if(pid == MY_PID_TOW_GEOFENCE || pid == MY_PID_PARK_GEOFENCE)
		{	TrigCondition = TRIG_CONDITION_WINDOW_OUT;
			cFunRetVal = SetParameterTrigPara3(pid,parano+1);
		}
		else
		  cFunRetVal = SetParameterTrigCondition(pid,parano+1);
	}
	if(cFunRetVal == SUCCESS)
	{	strcat(PasteBuff,"\r\nSAVE SUCCESSFUL");
		SendCommandReplyToSender(PasteBuff);
		UpdateProjectParameter2Memory(pid);
	}
	else
	{  	strcat(PasteBuff,"\r\nERROR: SAVE UNSUCCESSFUL");
		SendCommandReplyToSender(PasteBuff);
	}
	return cFunRetVal;
}
*/
////////////////////////////////////////////////////////////////////////////////
/*char ReadWriteParameterTrigCondition(char pid)
{	GetPidMem2Var(pid);
	if(strlen(CommandFormat.Para)>0)
	{	sscanf(CommandFormat.Para,"%d",&uiTemp);
		if(uiTemp < TRIG_CONDITION_MAX_VAL)
			TrigCondition = uiTemp;//Unit2Constant(CommandFormat.Para);
		UpdateProjectParameter2Memory(pid);
	}
	return TrigCondition;
}
*/
/*
char SetParameterTrigCondition(char pid, char parano)
{	//Trigger condition
	TrigCondition = TrigCondition2Constant(CommandFormat.Parameter[parano]);
	if(pid < MY_PID_AI_START)
	{	switch(TrigCondition)
		{	case TRIG_CONDITION_NONE:
			case TRIG_CONDITION_GREATER_THAN:
			case TRIG_CONDITION_SMALLER_THAN:	
			case TRIG_CONDITION_CHANGE_GREATER_THAN:
				strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
				if(TrigCondition != TRIG_CONDITION_NONE)
					return SetParameterTrigPara1(pid,parano+1);//TOTAL 1 PARAMETER SO CALL 1
				else
				  	return SUCCESS;
			default:strcat(PasteBuff,"\r\nIllegal Trigger Condition.");
				return ERROR;
		}
	}
	else if(pid == MY_PID_EXT_BATTERY)
	{	switch(TrigCondition)
		{	case TRIG_CONDITION_NONE:
			case TRIG_CONDITION_GREATER_THAN:
			case TRIG_CONDITION_SMALLER_THAN:	
			  	strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
				if(TrigCondition != TRIG_CONDITION_NONE)
					return SetParameterTrigPara1(pid,parano+1);//TOTAL 1 PARAMETER SO CALL 1
				else
				  	return SUCCESS;
			case TRIG_CONDITION_WINDOW_OUT:
			  	strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
				if(TrigCondition != TRIG_CONDITION_NONE)
					return SetParameterTrigPara2(pid,parano+1);//TOTAL 2 PARAMETER SO CALL 2 THEN 1
				else
				  	return SUCCESS;
			default:strcat(PasteBuff,"\r\nIllegal Trigger Condition.");
				return ERROR;
		}
	}
	else if(pid == MY_PID_AI1)
	{	switch(TrigCondition)
		{	case TRIG_CONDITION_NONE:
			case TRIG_CONDITION_GREATER_THAN:
			case TRIG_CONDITION_SMALLER_THAN:	
			case TRIG_CONDITION_CHANGE_GREATER_THAN:
				strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
				if(TrigCondition != TRIG_CONDITION_NONE)
					return SetParameterTrigPara1(pid,parano+1);//TOTAL 1 PARAMETER SO CALL 1
				else
				  	return SUCCESS;
			case TRIG_CONDITION_WINDOW_IN:
			case TRIG_CONDITION_WINDOW_OUT:
			case TRIG_CONDITION_WINDOW_IN_OR_OUT:
				strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
				if(TrigCondition != TRIG_CONDITION_NONE)
					return SetParameterTrigPara2(pid,parano+1);//TOTAL 2 PARAMETER SO CALL 2 THEN 1
				else
				  	return SUCCESS;
			default:strcat(PasteBuff,"\r\nIllegal Trigger Condition.");
				return ERROR;
		}
	}
	else if(pid >= MY_PID_DI_START && pid <=MY_PID_DI_END)
	{	switch(TrigCondition)
		{	case TRIG_CONDITION_NONE:
			case TRIG_CONDITION_CHANGE_HIGH:
			case TRIG_CONDITION_CHANGE_LOW:
			case TRIG_CONDITION_CHANGE_BOTH:
				strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
			 	return SetParameterTrigMessage(pid,parano+1);
			default:strcat(PasteBuff,"\r\nIllegal Trigger Condition.");
				return ERROR;
		}
	}
	else if(pid > MY_PID_GEOFENCE_START && pid < MY_PID_GEOFENCE_END)
	{	switch(TrigCondition)
		{	case TRIG_CONDITION_NONE:
			case TRIG_CONDITION_WINDOW_IN:
			case TRIG_CONDITION_WINDOW_OUT:
			case TRIG_CONDITION_WINDOW_IN_OR_OUT:
				strcat(PasteBuff,"\r\nTrigger Condition = ");
				strcatTrigCondition(PasteBuff,TrigCondition);
				if(TrigCondition != TRIG_CONDITION_NONE)
					return SetParameterTrigPara3(pid,parano+1);//TOTAL 3 PARAMETER SO CALL 3 THEN 2 THEN 1
				else
					return SUCCESS;
			default:strcat(PasteBuff,"\r\nIllegal Trigger Condition.");
				return ERROR;
		}
	}
	else
	  strcat(PasteBuff,"\r\nIllegal PID");
	return ERROR;
}*/
////////////////////////////////////////////////////////////////////////////////
/*
char SetParameterTrigPara1(char pid, char parano)
{	strcat(PasteBuff,"\r\nParameter 1 = ");
	RemoveCharFromString(' ',CommandFormat.Parameter[parano]);
	if(strlen(CommandFormat.Parameter[parano])>0)
	{  	TrigPara1Unit = Unit2Constant(CommandFormat.Parameter[parano+1]);
		switch(pid)
		{	case MY_PID_TIME:
		  		switch(TrigPara1Unit)
				{	//case UNIT_SEC:
					//case UNIT_MIN:
					//case UNIT_HR:
					case UNIT_DAYS:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara1Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara1Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara1Unit);
					 	return SetParameterTrigMessage(pid,parano+2);
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_DISTANCE:
		  		switch(TrigPara1Unit)
				{	case UNIT_MTR:
					case UNIT_KM:
					case UNIT_MILE:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara1Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara1Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara1Unit);
					 	return SetParameterTrigMessage(pid,parano+2);
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_ANGLE:
		  		switch(TrigPara1Unit)
				{	case UNIT_DEG:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara1Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara1Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara1Unit);
					 	return SetParameterTrigMessage(pid,parano+2);
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_SPEED:
		  		switch(TrigPara1Unit)
				{	case UNIT_MPS:
					case UNIT_KMPH:
					case UNIT_MPH:
					case UNIT_KNOTS:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara1Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara1Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara1Unit);
					 	return SetParameterTrigMessage(pid,parano+2);
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_EXT_BATTERY:
		  		switch(TrigPara1Unit)
				{	case UNIT_MV:
					case UNIT_VOLT:
					case UNIT_PERCENTAGE:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara1Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara1Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara1Unit);
					 	return SetParameterTrigMessage(pid,parano+2);
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_AI1:
		  		switch(TrigPara1Unit)
				{	case UNIT_PERCENTAGE:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara1Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara1Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara1Unit);
					 	return SetParameterTrigMessage(pid,parano+2);
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_GEOFENCE1:
		  	case MY_PID_GEOFENCE2:
		  	case MY_PID_GEOFENCE3:
		  	case MY_PID_GEOFENCE4:
		  	case MY_PID_GEOFENCE5:
		  	case MY_PID_GEOFENCE6:
		  		sscanf(CommandFormat.Parameter[parano],"%f",&TrigPara1Val.f);
				sprintf(CopyBuff,"%.6f",TrigPara1Val.f);
				strcat(PasteBuff,CopyBuff);
				TrigPara1Val.f = (TrigPara1Val.f*PI)/180;
				TrigPara1Unit = UNIT_DEG;
				strcatUnit(PasteBuff,TrigPara1Unit);
			 	return SetParameterTrigMessage(pid,parano+1);
		}
	}
	else
	  	strcat(CopyBuff,"Illegal Value");
	return ERROR;
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char SetParameterTrigPara2(char pid, char parano)
{	strcat(PasteBuff,"\r\nParameter 2 = ");
	RemoveCharFromString(' ',CommandFormat.Parameter[parano]);
	if(strlen(CommandFormat.Parameter[parano])>0)
	{  	TrigPara2Unit = Unit2Constant(CommandFormat.Parameter[parano+1]);
		switch(pid)
		{	case MY_PID_EXT_BATTERY:
		  		switch(TrigPara2Unit)
				{	case UNIT_MV:
					case UNIT_VOLT:
					case UNIT_PERCENTAGE:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara2Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara2Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara2Unit);
						return SetParameterTrigPara1(pid,parano+2);//1 PARAMETER left SO CALL 1
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_AI1:
		  		switch(TrigPara2Unit)
				{	case UNIT_PERCENTAGE:
					  	sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara2Val.i);
					  	sprintf(CopyBuff,"%d",TrigPara2Val.i);
						strcat(PasteBuff,CopyBuff);
						strcatUnit(PasteBuff,TrigPara2Unit);
						return SetParameterTrigPara1(pid,parano+2);//1 PARAMETER left SO CALL 1
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
			case MY_PID_GEOFENCE1:
		  	case MY_PID_GEOFENCE2:
		  	case MY_PID_GEOFENCE3:
		  	case MY_PID_GEOFENCE4:
		  	case MY_PID_GEOFENCE5:
		  	case MY_PID_GEOFENCE6:
		  		sscanf(CommandFormat.Parameter[parano],"%f",&TrigPara2Val.f);
				sprintf(CopyBuff,"%.6f",TrigPara2Val.f);
				strcat(PasteBuff,CopyBuff);
				TrigPara2Val.f = (TrigPara2Val.f*PI)/180;
				TrigPara2Unit = UNIT_DEG;
				strcatUnit(PasteBuff,TrigPara2Unit);
				return SetParameterTrigPara1(pid,parano+1);//1 PARAMETER left SO CALL 1
		}
	}
	else
	  	strcat(CopyBuff,"Illegal Value");
	return ERROR;
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char SetParameterTrigPara3(char pid, char parano)
{	strcat(PasteBuff,"\r\nParameter 3 = ");
	RemoveCharFromString(' ',CommandFormat.Parameter[parano]);
	if(strlen(CommandFormat.Parameter[parano])>0)
	{  	TrigPara3Unit = Unit2Constant(CommandFormat.Parameter[parano+1]);
		switch(pid)
		{	case MY_PID_GEOFENCE0:
		  	case MY_PID_GEOFENCE1:
		  	case MY_PID_GEOFENCE2:
		  	case MY_PID_GEOFENCE3:
		  	case MY_PID_GEOFENCE4:
		  	case MY_PID_GEOFENCE5:
		  	case MY_PID_GEOFENCE6:
		  		sscanf(CommandFormat.Parameter[parano],"%d",&TrigPara3Val.i);
				sprintf(CopyBuff,"%d",TrigPara3Val.i);
				strcat(PasteBuff,CopyBuff);
				switch(TrigPara3Unit)
				{	case UNIT_MTR:
					case UNIT_KM:
					case UNIT_MILE:
					  	strcatUnit(PasteBuff,TrigPara3Unit);
						if(pid == MY_PID_TOW_GEOFENCE || pid == MY_PID_PARK_GEOFENCE)
		  					return SetParameterTrigMessage(pid,parano+2);//SUCCESS;
						else
							return SetParameterTrigPara2(pid,parano+2);//2 PARAMETER left SO CALL 2
					default:strcat(PasteBuff," Illegal Unit");
						return ERROR;
				}
				break;
		}
	}
	else
	  	strcat(CopyBuff,"Illegal Value");
	return ERROR;
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char SetParameterTrigMobileAndOnward(char pid, char parano)
{	//SMS Miss Call Mobile No.
	if(CommandFormat.Parameter[parano][0]==NULL)
	{	//DO NOTHING proceed to next
	  	SetParameterTrigMessage(pid,parano+1);
	}
  	else if(strlen(CommandFormat.Parameter[parano])>11 && strlen(CommandFormat.Parameter[parano])<=15)
	{	switch(AddOtherUserMobile(StrMobileNo2LL(CommandFormat.Parameter[parano]),pid))
		{	case MY_USER_TABLE_FULL: strcat(CopyBuff,"\r\nMobile Memory Full");break;
			case MY_USER_FOUND:	strcat(CopyBuff,"\r\nMobile with this trigger already exists.");break;
			default:strcat(CopyBuff,"\"\r\nTrigger Mobile Set to ");break;
		}
		strcat(CopyBuff,CommandFormat.Parameter[parano]);
		SetParameterTrigMessage(pid,parano+1);
	}
	else
	  strcat(CopyBuff,"\r\nTrigger Mobile Illegal Value.");
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char SetParameterTrigMessage(char pid, char parano)
{	//SMS message
	if(strlen(CommandFormat.Parameter[parano])>0 && strlen(CommandFormat.Parameter[parano])<=100)
	{	strcpy(TrigMsg,CommandFormat.Parameter[parano]);
		strcat(PasteBuff,"\r\nTrigger Message Set to \"");
		strcat(PasteBuff,TrigMsg);	
		strcat(PasteBuff,"\"");
		return SUCCESS;
	}
	return ERROR;
}*/
////////////////////////////////////////////////////////////////////////////////
/*
int SearchUserMobileNumber(long long mobno,char pid)//pid=0 if search with mobile only
{	int loc;
	u.ll = mobno;
	u.c[UT_PP_T_IDp] = pid;
	u.c[CALL_ATTEMPTp] = 0;
	llTemp = u.ll;
	for(loc=0;loc<MAX_NO_OF_USER;loc++)
	{	//sprintf(CopyBuff,"\r\nLoc=%d Search=%lld",loc,llTemp);//testing code
		//PutStringUsb(CopyBuff);//testing code
		u.ll = 0;
	  	DirectMemRead(USER_TABLE_START_ADDRESS+loc*USER_TABLE_RECORD_SIZE,u.c,USER_TABLE_RECORD_SIZE);
		u.c[CALL_ATTEMPTp] = 0;
		if(pid == 0)//if only mobile no is to be searched
		  u.c[UT_PP_T_IDp] = 0;
		//sprintf(CopyBuff," MemWcc=%lld",u.ll);//testing code
		//PutStringUsb(CopyBuff);//testing code
		if(CountryCodeFlag == FALSE)
		{	ucTemp = u.c[UT_PP_T_IDp];	//required if search with pid
			u.c[UT_PP_T_IDp] = 0;		//required if search with pid
			u.ll %= 10000000000;
			u.c[UT_PP_T_IDp] = ucTemp;	//required if search with pid
			
		    //sprintf(CopyBuff," MemXcc=%lld",u.ll);//testing code
			//PutStringUsb(CopyBuff);//testing code
		}
		if(u.ll==llTemp)
		{  	return loc;	//mobile no & pid both match
		}
	}
	return -1;	//no match
}
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//function to add unit to string given
/*
void strcatUnit(char *str,char unit)
{	switch(unit)
	{	case UNIT_NONE: 	strcat(str,UNIT_STR_NONE);	break;
		case UNIT_MTR: 		strcat(str,UNIT_STR_MTR);	break;
		case UNIT_KM: 		strcat(str,UNIT_STR_KM);	break;
		case UNIT_MILE: 	strcat(str,UNIT_STR_MILE);	break;
		case UNIT_MPS: 		strcat(str,UNIT_STR_MPS);	break;
		case UNIT_KMPH: 	strcat(str,UNIT_STR_KMPH);	break;
		case UNIT_MPH: 		strcat(str,UNIT_STR_MPH);	break;
		case UNIT_KNOTS:	strcat(str,UNIT_STR_KNOTS);break;
		case UNIT_SEC: 		strcat(str,UNIT_STR_SEC);	break;
		case UNIT_MIN: 		strcat(str,UNIT_STR_MIN);	break;
		case UNIT_HR: 		strcat(str,UNIT_STR_HR);	break;
		case UNIT_DAYS: 	strcat(str,UNIT_STR_DAYS);	break;
		case UNIT_MV: 		strcat(str,UNIT_STR_MV);	break;
		case UNIT_VOLT: 	strcat(str,UNIT_STR_VOLT);	break;
		case UNIT_DEG: 		strcat(str,UNIT_STR_DEG);	break;
		case UNIT_DEG_C: 	strcat(str,UNIT_STR_DEG_C);break;
		case UNIT_DEG_F: 	strcat(str,UNIT_STR_DEG_F);break;
		case UNIT_PERCENTAGE:strcat(str,UNIT_STR_PERCENTAGE);break;
		default:			strcat(str,"N.A");	break;
	}
}*/
////////////////////////////////////////////////////////////////////////////////
//function to add Trigger CONDITION to string given
/*
void strcatTrigCondition(char *str,char condition)
{	switch(condition)
	{	case TRIG_CONDITION_NONE: 				strcat(str,TRIG_CONDITION_STR_NONE);		break;
		case TRIG_CONDITION_CHANGE_HIGH:		strcat(str,"LOW TO HIGH");	break;
		case TRIG_CONDITION_CHANGE_LOW:			strcat(str,"HIGH TO LOW");	break;
		case TRIG_CONDITION_CHANGE_BOTH:		strcat(str,TRIG_CONDITION_STR_CHANGE_BOTH);	break;
		case TRIG_CONDITION_CHANGE_GREATER_THAN:strcat(str,"CHANGE >");	break;
		case TRIG_CONDITION_GREATER_THAN:		strcat(str,TRIG_CONDITION_STR_GREATER_THAN);break;
		case TRIG_CONDITION_SMALLER_THAN:		strcat(str,TRIG_CONDITION_STR_SMALLER_THAN);break;
		case TRIG_CONDITION_WINDOW_OUT:			strcat(str,"WINDOW OUT");	break;
		case TRIG_CONDITION_WINDOW_IN:			strcat(str,"WINDOW IN");	break;
		case TRIG_CONDITION_WINDOW_IN_OR_OUT:	strcat(str,"WINDOW IN OR OUT");break;
		default:								PutStringUsb("\r\nTrig Condition="); PutCharUsb((condition/100)+48);PutCharUsb((condition/10)%10+48);PutCharUsb(condition%10+48);strcat(str,"N.A ");break;
	}
}*/
////////////////////////////////////////////////////////////////////////////////
/*
char* TrigPriority2str(char priority)
{	switch(priority)
	{	case TRIG_PRIORITY_NORMAL:		return TRIG_PRIORITY_STR_NORMAL;
		case TRIG_PRIORITY_HIGH: 		return TRIG_PRIORITY_STR_HIGH;
		case TRIG_PRIORITY_PANIC:		return TRIG_PRIORITY_STR_PANIC;
		default:						PutStringUsb("\r\nTrig Priority="); PutCharUsb(priority+48);return "NA";
	}
}
*/
/*
//function to add Trigger PRIORITY to string given
void strcatTrigPriority(char *str,char priority)
{	switch(priority)
	{	case TRIG_PRIORITY_NONE:		strcat(str,TRIG_PRIORITY_STR_NONE);break;
		case TRIG_PRIORITY_LOW: 		strcat(str,TRIG_PRIORITY_STR_LOW);break;
		case TRIG_PRIORITY_HIGH: 		strcat(str,TRIG_PRIORITY_STR_HIGH);break;
		case TRIG_PRIORITY_PANIC:		strcat(str,TRIG_PRIORITY_STR_PANIC);break;
		default:						PutStringUsb("\r\nTrig Priority="); PutCharUsb(priority+48);strcat(str,"N.A");break;
	}
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char* strcatPid(ui pid)
{	switch(pid)
	{	case MY_PID_TIME: 		return PID_STR_TIME;
		case MY_PID_EXT_BATTERY:return PID_STR_EXT_BATTERY;
		case MY_PID_AI1:		return PID_STR_AI1;
		case MY_PID_DI1:		DirectMemRead(INPUT_TABLE_START_ADDRESS,TrigMsg,IT_NAMEn);
								TrigMsg[IT_NAMEn] = '\0';
		  						return TrigMsg;
		case MY_PID_DI2:		DirectMemRead(INPUT_TABLE_START_ADDRESS+INPUT_TABLE_RECORD_SIZE,TrigMsg,IT_NAMEn);
								TrigMsg[IT_NAMEn] = '\0';
		  						return TrigMsg;
		case MY_PID_DI3:		DirectMemRead(INPUT_TABLE_START_ADDRESS+INPUT_TABLE_RECORD_SIZE*2,TrigMsg,IT_NAMEn);
								TrigMsg[IT_NAMEn] = '\0';
		  						return TrigMsg;
		case MY_PID_DI4:		DirectMemRead(INPUT_TABLE_START_ADDRESS+INPUT_TABLE_RECORD_SIZE*3,TrigMsg,IT_NAMEn);
								TrigMsg[IT_NAMEn] = '\0';
		  						return TrigMsg;
		default:				return "NONE";
	}
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
char TrigPriority2Constant(char*str)
{	RemoveCharFromString(' ',str);
	String2Upper(str);
	if(*str == NULL)
  		return -1;
	else if(strlen(str)==0)
	  	return -1;
	else if(strcmp(str,TRIG_PRIORITY_STR_NORMAL)==0)
	  	return TRIG_PRIORITY_NORMAL;
	else if(strcmp(str,TRIG_PRIORITY_STR_HIGH)==0)
	  	return TRIG_PRIORITY_HIGH;
	else if(strcmp(str,TRIG_PRIORITY_STR_PANIC)==0)
	  	return TRIG_PRIORITY_PANIC;
	else
	  return -1;
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
void strcatUserAssociatedWithPid(char *dstr,char pid)
{	int i;
	for(i=3;i<MAX_NO_OF_USER;i++)
	{	u.ll = 0;
	  	DirectMemRead(USER_TABLE_START_ADDRESS+i*USER_TABLE_RECORD_SIZE,u.c,USER_TABLE_RECORD_SIZE);
	  	if(u.c[UT_PP_T_IDp] == pid)
		{	u.c[UT_PP_T_IDp] = 0;
			u.c[CALL_ATTEMPTp] = 0;
			strcat(dstr,"\r\n+");
			//D2A(u.ll,CopyBuff);
			sprintf(CopyBuff,"%lld",u.ll);
			strcat(dstr,CopyBuff);
		}
		if(strlen(dstr)>500)
		{	SendCommandReplyToSender(dstr);
			strcpy(dstr,"");
		}
	}
}*/
////////////////////////////////////////////////////////////////////////////////
/*
void UpdateProjectParameter2Memory(char pid)
{	u.i[0] = LogVal.i;
  	PasteBuff[PP_LOG_VALp] = u.c[0];
	PasteBuff[PP_LOG_VALp+1] = u.c[1];
	PasteBuff[PP_LOG_VALp+2] = u.c[2];
	PasteBuff[PP_LOG_VALp+3] = u.c[3];
	PasteBuff[PP_LOG_UNITp] = LogUnit;
	PasteBuff[PP_TRIG_PRIORITY_CONDITIONp] = ((TrigPriority<<5)+TrigCondition);
	u.i[0] = TrigPara1Val.i;
	PasteBuff[PP_TRIG_PARA1_VALp] = u.c[0];
	PasteBuff[PP_TRIG_PARA1_VALp+1] = u.c[1];
	PasteBuff[PP_TRIG_PARA1_VALp+2] = u.c[2];
	PasteBuff[PP_TRIG_PARA1_VALp+3] = u.c[3];
	//PasteBuff[PP_TRIG_PARA1_UNITp] = TrigPara1Unit;
	u.i[0] = TrigPara2Val.i;
	PasteBuff[PP_TRIG_PARA2_VALp] = u.c[0];
	PasteBuff[PP_TRIG_PARA2_VALp+1] = u.c[1];
	PasteBuff[PP_TRIG_PARA2_VALp+2] = u.c[2];
	PasteBuff[PP_TRIG_PARA2_VALp+3] = u.c[3];
	//PasteBuff[PP_TRIG_PARA2_UNITp] = TrigPara2Unit;
	u.i[0] = TrigPara3Val.i;
	PasteBuff[PP_TRIG_PARA3_VALp] = u.c[0];
	PasteBuff[PP_TRIG_PARA3_VALp+1] = u.c[1];
	PasteBuff[PP_TRIG_PARA3_VALp+2] = u.c[2];
	PasteBuff[PP_TRIG_PARA3_VALp+3] = u.c[3];
	//PasteBuff[PP_TRIG_PARA3_UNITp] = TrigPara3Unit;
	//HERE IN CASE RESERVED LOCATION USED
	for(ucTemp=0;TrigMsg[ucTemp]!='\0';ucTemp++)
		PasteBuff[PP_MSGp+ucTemp] = TrigMsg[ucTemp];
	for(;ucTemp<100;ucTemp++)
	  	PasteBuff[PP_MSGp+ucTemp] = '\0';
	
	RandomMemWrite(PP_START_LOC+pid*PP_MAX_BYTE,PasteBuff,PP_MAX_BYTE);
	
}*/
////////////////////////////////////////////////////////////////////////////////
