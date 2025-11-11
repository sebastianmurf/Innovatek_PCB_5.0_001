#include"mcu.h"
#include"GpsUart.h"
#include"delay.h"
#include"common.h"
#include"command.h"
#include"board.h"
#include"myfun.h"
#include"constants.c"
#include"variable.h"
#include"AT45DB161.h"
#include"gprs.h"
#include"DualTimer.h"
#include"gpio.h"
#include"MyUsb.h"
//----------------
#include<math.h>
#include<ctype.h>
#include"string.h"
#include<stdio.h>
//----------------
/******************* GPS COMMUNICATION VARIABLE INITIALIZATION STARTS **********/
struct gpselement GpsCurData, GpsLastValidData, LastLogGpsData;
char GpsBuff[4][GPS_PKT_BUFF_SIZE];//GpsBuff packet should be one more then GPS_MAX_PKT else creating problem by sending unknown sms to unknown number while sending command
union union_temp GpsCurDataInRadian;
uc GpsPktCntr;
uc GpsCrcCodeOut;
boolean_t GpsCrcCodeCalFlag = FALSE;
//unsigned char GpsReceiveCounter;
//boolean_t GpsReceiveFlag = FALSE;
//unsigned char GpsReceiveByte;
boolean_t GpsCommandFlag = FALSE;
boolean_t GpsInterruptRecvDebugFlag = FALSE;
boolean_t GpsPacketValidFlag;
//boolean_t GpsRawDataDebugFlag = FALSE;
uc StopDistanceMoveCounter = 0;
ui StopNextLogTime = 0;
ui GpsValidTimeOutSec = 0;	//if gps valid is not received within GPS_VALID_TIMEOUT_SEC then gps will restart without any condition.
/******************* GPS COMMUNICATION VARIABLE INITIALIZATION ENDS ************/
void CheckGpsPacket()
{   //TESTING BYPASS 
	uc c;
	uc gpsrecvchecksum;
	uc gpscommacounter;
	uc loop;
  	uc uctemp;
	//ui uitemp;
  	
	GpsPacketValidFlag = TRUE;
	//packet identification and calculation code here
    //calculate checksum // c is gps data buffer column
    //PutStringUsb("\r\nGPS >--->>");
	for(GpsPktCntr--;GpsPktCntr>0;GpsPktCntr--)
	{	//PutStringUsb("\r\n");//testing code
	  	//PutStringUsb(GpsBuff[GpsPktCntr-1]);//testing code
		for(gpsrecvchecksum = 0,c = 0; GpsBuff[GpsPktCntr-1][c]!= '*';c++){
        	gpsrecvchecksum ^= GpsBuff[GpsPktCntr-1][c];
    	}
		//uctemp = AsciiHex2Hex(&GpsBuff[GpsPktCntr-1][c+1]);
		//sprintf(CopyBuff,"\r\n%d> Calculate=%d, Received=%d\r\n",GpsPktCntr,gpsrecvchecksum,uctemp);
		//PutStringUsb(CopyBuff);
		
		if(gpsrecvchecksum == AsciiHex2Hex(&GpsBuff[GpsPktCntr-1][c+1]))
		{   if(SearchString(GpsBuff[GpsPktCntr-1],"GPGGA") != -1)
			{	for(gpscommacounter = 0,c=5;GpsBuff[GpsPktCntr-1][c]!='\0';c++)
				{   if(GpsBuff[GpsPktCntr-1][c] == ',')
					{   gpscommacounter++;
						switch(gpscommacounter)
						{   case 7:
								for(uctemp=0,loop=1;GpsBuff[GpsPktCntr-1][c+loop]!=',';loop++)
								{	uctemp *= 10;
									uctemp += GpsBuff[GpsPktCntr-1][c+loop]-48;
								}
								if(uctemp > 3 && uctemp < 20)
								{   GpsCurData.NoS = uctemp;
								}
								else
								{	GpsPacketValidFlag = FALSE;
									PutStringUsb("\r\nGPS> NoS error");
								}
							break;
							case 9:
							  	for(uctemp=0,loop=1;GpsBuff[GpsPktCntr-1][c+loop]!=',';loop++)
								{	u.c[uctemp] = GpsBuff[GpsPktCntr-1][c+loop];
								}
								GpsCurData.Altitude = (ui)(atof(u.c)*100);
							  break;
						}
					}
				}
			}
			else if(SearchString(GpsBuff[GpsPktCntr-1],"GPRMC") != -1)
			{	//get time code here
				for(gpscommacounter = 0,c=5;GpsBuff[GpsPktCntr-1][c]!='\0';c++)
				{   if(GpsBuff[GpsPktCntr-1][c] == ',')
					{   gpscommacounter++;
						switch(gpscommacounter)
						{   case 1: 
								GpsCurData.TimeStamp.tm_hour = (((GpsBuff[GpsPktCntr-1][c+1]-48) * 10) + (GpsBuff[GpsPktCntr-1][c+2]-48));
								GpsCurData.TimeStamp.tm_min = (((GpsBuff[GpsPktCntr-1][c+3]-48) * 10) + (GpsBuff[GpsPktCntr-1][c+4]-48));
								GpsCurData.TimeStamp.tm_sec = (((GpsBuff[GpsPktCntr-1][c+5]-48) * 10) + (GpsBuff[GpsPktCntr-1][c+6]-48));
								c += 6;
							break;
							case 2: 
								if(GpsBuff[GpsPktCntr-1][c+1]=='V')
								{   GpsPacketValidFlag = FALSE;
									PutStringUsb("\r\nGPS> Inval RMC packet");
								}
								else if(GpsBuff[GpsPktCntr-1][c+1]=='A')
								{   //PutStringUsb(" Val");
								}
								c++;
							break;
							case 3: 
								if(GpsBuff[GpsPktCntr-1][c+1] != ',')	//2845.6789
								{   GpsCurData.Lat = (((GpsBuff[GpsPktCntr-1][c+6]-48)*1000)+((GpsBuff[GpsPktCntr-1][c+7]-48)*100)+((GpsBuff[GpsPktCntr-1][c+8]-48)*10)+(GpsBuff[GpsPktCntr-1][c+9]-48));
									GpsCurData.Lat /= 10000;
									GpsCurData.Lat += (((GpsBuff[GpsPktCntr-1][c+3]-48)*10)+(GpsBuff[GpsPktCntr-1][c+4]-48));
									GpsCurData.Lat /= 60;
									GpsCurData.Lat += (((GpsBuff[GpsPktCntr-1][c+1]-48)*10)+(GpsBuff[GpsPktCntr-1][c+2]-48));
									if(GpsCurData.Lat > 90)
									  GpsPacketValidFlag = FALSE;
								}
								else
								  GpsCurData.Lat = 0;
							break;
							case 4: 
								if(GpsPacketValidFlag == TRUE)
								{   if(GpsBuff[GpsPktCntr-1][c+1] == 'S')
									{   GpsCurData.Lat *= -1;
									}
									c++;
								}
							break;
							case 5: 
								if(GpsBuff[GpsPktCntr-1][c+1] != ',')	//07706.7241
								{   GpsCurData.Lng = (((GpsBuff[GpsPktCntr-1][c+7]-48) *1000) + ((GpsBuff[GpsPktCntr-1][c+8]-48) *100) + ((GpsBuff[GpsPktCntr-1][c+9]-48)*10) + (GpsBuff[GpsPktCntr-1][c+10]-48));
									GpsCurData.Lng /= 10000;
									GpsCurData.Lng +=  (((GpsBuff[GpsPktCntr-1][c+4]-48)*10) + (GpsBuff[GpsPktCntr-1][c+5]-48));	//total minutes
									GpsCurData.Lng /= 60;	//minutes converted into degree
									GpsCurData.Lng +=  (((GpsBuff[GpsPktCntr-1][c+1]-48)*100) + ((GpsBuff[GpsPktCntr-1][c+2]-48)*10) + (GpsBuff[GpsPktCntr-1][c+3]-48));	//total lat in degree
									if(GpsCurData.Lng > 180)
									  GpsPacketValidFlag = FALSE;
								}
								else
								  GpsCurData.Lng = 0;
								break;
							case 6: 
								if(GpsPacketValidFlag == TRUE)
								{   if(GpsBuff[GpsPktCntr-1][c+1] == 'W')
									{   GpsCurData.Lng *= -1;
									}
									c++;
								}
							break;
							case 7: 
								for(loop=1,uctemp=0,GpsCurData.Speed=0;GpsBuff[GpsPktCntr-1][c+loop]!=',';loop++)
								{   if(GpsBuff[GpsPktCntr-1][c+loop] == '.')
										continue;
									else
									{  	GpsCurData.Speed *= 10;
										GpsCurData.Speed += (GpsBuff[GpsPktCntr-1][c+loop]-48);
									}
								}
								GpsCurData.Speed /= 100;		//converting to KNOTS
								GpsCurData.Speed *= KNOTS2CMPS;//actually speed in cm/s
								c++;
								break;
							case 8:
								for(loop=1,uctemp=0,GpsCurData.Angle=0;GpsBuff[GpsPktCntr-1][c+loop]!=',';loop++)
								{   if(GpsBuff[GpsPktCntr-1][c+loop] == '.')
										continue;
									else
									{  	GpsCurData.Angle *= 10;
										GpsCurData.Angle += (GpsBuff[GpsPktCntr-1][c+loop]-48);
									}
								}
								GpsCurData.Angle /= 100;	//in degree
								c++; 
							break;
							case 9: 
								GpsCurData.TimeStamp.tm_year = (GpsBuff[GpsPktCntr-1][c+5]-48)*10 + (GpsBuff[GpsPktCntr-1][c+6]-48);	//year loc
								GpsCurData.TimeStamp.tm_mon = (GpsBuff[GpsPktCntr-1][c+3]-48)*10 + (GpsBuff[GpsPktCntr-1][c+4]-48);	//year loc
								GpsCurData.TimeStamp.tm_mday = (GpsBuff[GpsPktCntr-1][c+1]-48)*10 + (GpsBuff[GpsPktCntr-1][c+2]-48);	//date loc
							break;
						}//switch end
					}//if end
				}//for end
			}
			else if(SearchString(GpsBuff[GpsPktCntr-1],"GPTXT") != -1)
			{	if(SearchString(GpsBuff[GpsPktCntr-1],"ANTSTATUS") != -1)
				{	if(SearchString(GpsBuff[GpsPktCntr-1],"OK") != -1)
					{	GpsCurData.AntStat = ANT_OK;	
					}
					else if(SearchString(GpsBuff[GpsPktCntr-1],"OPEN") != -1)
					{	GpsCurData.AntStat = ANT_OPEN;	
					}
					else if(SearchString(GpsBuff[GpsPktCntr-1],"SHORT") != -1)
					{	GpsCurData.AntStat = ANT_SHORT;	
					}
					else
					{	GpsCurData.AntStat = ANT_STAT_NOT_KNOWN;	
					}
				}
			}
		}
		else
		{   PutStringUsb("\r\nGPS> ChkSm Error");
			GpsPacketValidFlag = FALSE;
		}
	}
	//PutStringUsb("\r\nGPS >-X->>");
	//testing bypass end
	if(GpsCurData.Lat == 0 && GpsCurData.Lng == 0)
		GpsPacketValidFlag = FALSE;	  
		
	if(GpsPacketValidFlag == TRUE)
    {   LED_ON(LED_BLUE);
		
		GpsValidTimeOutSec = FreeRunSec;
		GpsLastValidData.TimeStamp = GpsCurData.TimeStamp;
		Device.TimeStamp = GpsCurData.TimeStamp;
		SecAfterTimeSync = 0;
		GpsLastValidData.Lat = GpsCurData.Lat;
		GpsLastValidData.Lng = GpsCurData.Lng;
		GpsLastValidData.Altitude = GpsCurData.Altitude;
		GpsLastValidData.Speed = GpsCurData.Speed;
		GpsLastValidData.Angle = GpsCurData.Angle;
		GpsLastValidData.NoS = GpsCurData.NoS;
		GpsLastValidData.AntStat = GpsCurData.AntStat;
		
		GpsCurDataInRadian.f[0] = GpsLastValidData.Lat;
		GpsCurDataInRadian.f[0] *= PI;
		GpsCurDataInRadian.f[0] /= 180;  //lat in radian
		GpsCurDataInRadian.f[1] = GpsLastValidData.Lng;
		GpsCurDataInRadian.f[1] *= PI;
		GpsCurDataInRadian.f[1] /= 180;  //lng in radian
		//calculate last log to current distance
		LastToCurrDistance.f[0] = (GpsCurDataInRadian.f[1]-LastLogGpsData.Lng) * cos((GpsCurDataInRadian.f[0]+LastLogGpsData.Lat)/2);
		LastToCurrDistance.f[1] = GpsCurDataInRadian.f[0]-LastLogGpsData.Lat;
		LastToCurrDistance.f[0] = LastToCurrDistance.f[0]*LastToCurrDistance.f[0]+LastToCurrDistance.f[1]*LastToCurrDistance.f[1];
		LastToCurrDistance.i[0] = (long)(6371000 * sqrt(LastToCurrDistance.f[0]));
		if(LastToCurrDistance.i[0] > 0)
			LastToCurrDistance.i[0]++;	//round off to upper
		/*
		if(GpsRawDataDebugFlag == TRUE)
		{	sprintf(PasteBuff,"\r\nT:%d:%d:%d D:%d/%d/%d Lt:%.6f Lg:%.6f Dis:%d Alt:%d Ag:%d Nos:%d Sp:%d AS:",
					GpsLastValidData.TimeStamp.tm_hour,
					GpsLastValidData.TimeStamp.tm_min,
					GpsLastValidData.TimeStamp.tm_sec,
					GpsLastValidData.TimeStamp.tm_year,
					GpsLastValidData.TimeStamp.tm_mon,
					GpsLastValidData.TimeStamp.tm_mday,
					GpsLastValidData.Lat,
					GpsLastValidData.Lng,
					LastToCurrDistance.i[0],
					GpsLastValidData.Altitude,
					GpsLastValidData.Angle,
					GpsLastValidData.NoS,
					GpsLastValidData.Speed);
			PutStringUsb(PasteBuff);
			if(GpsLastValidData.AntStat == ANT_OK)
			  PutStringUsb("Ok");
			else if(GpsLastValidData.AntStat == ANT_OPEN)
			  PutStringUsb("Opn");
			else if(GpsLastValidData.AntStat == ANT_SHORT)
			  PutStringUsb("Shrt");
			else
			  PutStringUsb("Unknwn");
		}
		*/		
	  	//calculation starts here
		//calculation with PID 0: TIME
		if((PidMCR[MY_PID_TIME/8] & (1<<(MY_PID_TIME%8))) > 0)
		{	//log calculation
			uiTemp = TimeStamp2Sec(GpsLastValidData.TimeStamp) + SecAfterTimeSync - TimeStamp2Sec(LastLogGpsData.TimeStamp);
			GetPidMem2Var(MY_PID_TIME);
			switch(LogUnit)
			{	case UNIT_SEC:	break;
				case UNIT_MIN:	LogVal.i *= 60;break;
				case UNIT_HR:	LogVal.i *= 3600;break;
				default:		LogVal.i = 0xffffffff;break;//max value
			}
			if(uiTemp >= LogVal.i)//need to log
			{	NeedToLogFlag = TRUE;
				/*if(((CurDigitalInput & DI_IGNITION) == 0) && ((PrevDigitalInput & DI_IGNITION) == 0))
				{	ContinueLastLogData();
				}*/
				if(GpsLastValidData.NoS <= LastLogGpsData.NoS)
				{	if((PidMCR[MY_PID_DISTANCE/8] & (1<<(MY_PID_DISTANCE%8))) == 0)//distance is disabled
					{	if(LastToCurrDistance.i[0] < 50)
							ContinueLastLogData();
					}
					else
					  	ContinueLastLogData();
				}
				if(TriggerPriorityVal == 0)
					TriggerPid = MY_PID_TIME;  
			}
			//trigger calculation
			if((TrigCondition > 0) && (ServiceTimeTriggerFlag==FALSE))//trigger active & previous there is no trigger of any parameter
			{	//TriggerValidFlag = FALSE;
				/* by default unit is days
				switch(TrigCondition)
				{	case TRIG_CONDITION_GREATER_THAN:
					case TRIG_CONDITION_CHANGE_GREATER_THAN:
						switch(TrigPara1Unit)
						{	case UNIT_HR:	TriggerValidFlag=TRUE;break;
							case UNIT_DAYS:	TrigPara1Val.i *= 24;TriggerValidFlag=TRUE;break;
						}
						break;
				}
				*/
				GpsCurData.TimeStamp.tm_year = LastServiceDate[0];
				GpsCurData.TimeStamp.tm_mon = LastServiceDate[1];
				GpsCurData.TimeStamp.tm_mday = LastServiceDate[2];
				uiTemp = TimeStamp2Sec(GpsLastValidData.TimeStamp) + SecAfterTimeSync - TimeStamp2Sec(GpsCurData.TimeStamp);
				uiTemp /= 3600;//sec to HOUR
				uiTemp /=24;//hour to days
				if(uiTemp > TrigPara1Val.i)//need to trigger
				{	PutStringUsb("\r\nTrig:TIME");
					//TrigResponseFlag = 1;
					//UpdateProjectParameter2Memory(MY_PID_TIME);
	
					if(TriggerPriorityVal <= TrigPriority)
					{	TriggerPriorityVal = TrigPriority; 
						TriggerPid = MY_PID_TIME;
					}
					//save it to last trigger value
					u.c[0]=LastServiceDate[0] = GpsLastValidData.TimeStamp.tm_year;
					u.c[1]=LastServiceDate[1] = GpsLastValidData.TimeStamp.tm_mon;
					u.c[2]=LastServiceDate[2] = GpsLastValidData.TimeStamp.tm_mday;
					u.c[3] |= SERVICE_TIME_TRIGGER_BIT;
					ServiceTimeTriggerFlag = TRUE;
					RandomMemWrite(LAST_TIME_TRIG_DATEa,u.c,LAST_TIME_TRIG_DATEn);
					PidTPR[MY_PID_TIME/8] |= (1<<(MY_PID_TIME%8));
					NeedToLogFlag = TRUE;
				}
			}
		}
		
		//calculation with PID 1: DISTANCE
		if((PidMCR[MY_PID_DISTANCE/8] & (1<<(MY_PID_DISTANCE%8))) > 0)
		{	
			if(PowerOnPositionShiftFlag == TRUE)
			{	if(((CurDigitalInput & DI_IGNITION) == 0) && ((PrevDigitalInput & DI_IGNITION) == 0))
				{	//if(VibrationSenseCounter < VibrationSensitivity)
					//	ContinueLastLogData();
					//else 
					{	if(LastToCurrDistance.i[0] < 50)
						{	//sprintf(CopyBuff,"\r\nCONTINUE LAST LOG DATA FreeRunSec=%d StopNextLogTime=%d Diff=%d Dist=%d",FreeRunSec,StopNextLogTime,abs(FreeRunSec-StopNextLogTime),LastToCurrDistance.i[0]);
							//PutStringUsb(CopyBuff);
							ContinueLastLogData();
							StopNextLogTime = FreeRunSec;
							StopDistanceMoveCounter = 0;
						}
						else if(abs(FreeRunSec - StopNextLogTime) < 30)
						{	//sprintf(CopyBuff,"\r\nCONTINUE LAST LOG DATA FreeRunSec=%d StopNextLogTime=%d Diff=%d Dist=%d",FreeRunSec,StopNextLogTime,abs(FreeRunSec-StopNextLogTime),LastToCurrDistance.i[0]);
							//PutStringUsb(CopyBuff);
							ContinueLastLogData();
						}
						else if(abs(FreeRunSec - StopNextLogTime) > 270)
						{	//sprintf(CopyBuff,"\r\nCONTINUE LAST LOG DATA FreeRunSec=%d StopNextLogTime=%d Diff=%d Dist=%d",
									//FreeRunSec,StopNextLogTime,abs(FreeRunSec-StopNextLogTime),LastToCurrDistance.i[0]);
							//PutStringUsb(CopyBuff);
							ContinueLastLogData();
							StopNextLogTime = FreeRunSec;
						}
						else
						{	//sprintf(CopyBuff,"\r\nFRESH LOG DATA FreeRunSec=%d StopNextLogTime=%d Diff=%d Dist=%d",FreeRunSec,StopNextLogTime,abs(FreeRunSec-StopNextLogTime),LastToCurrDistance.i[0]);
							//PutStringUsb(CopyBuff);
						  	
						  	if(++StopDistanceMoveCounter<3)
								ContinueLastLogData();
							else
								StopNextLogTime = FreeRunSec;//current data to be recorded
						}  
					}
				}
			}
			
			GetPidMem2Var(MY_PID_DISTANCE);
			
			switch(LogUnit)
			{	case UNIT_MTR:	break;
				case UNIT_KM:	LogVal.i *= 1000;break;
				case UNIT_MILE:	LogVal.i *= 1600;break;
				default:		LogVal.i = 0xffffffff;break;//max value
			}
			
			if(LastToCurrDistance.i[0] > LogVal.i)//need to log
			{	NeedToLogFlag = TRUE;
				if(TriggerPriorityVal == 0)
					TriggerPid = MY_PID_DISTANCE;  
			}
			//trigger distance calculation
			if(TrigCondition > 0 && ServiceDateTriggerFlag==FALSE)//trigger active & previous there is no trigger of any parameter
			{	//TriggerValidFlag = FALSE;
				//calculate accumulativ distance travelled
				/*by default unit is KM
				switch(TrigCondition)
				{	case TRIG_CONDITION_GREATER_THAN:
					case TRIG_CONDITION_CHANGE_GREATER_THAN:
						switch(TrigPara1Unit)
						{	case UNIT_MTR:	TriggerValidFlag=TRUE;break;
							case UNIT_KM:	TrigPara1Val.i *= 1000;TriggerValidFlag=TRUE;break;
							case UNIT_MILE:	TrigPara1Val.i *= 1600;TriggerValidFlag=TRUE;break;
						}
						break;
				}
				*/
				if((((MileageMeter-LastServiceMileage)/1000) >= TrigPara1Val.i))// && /*(TrigResponseFlag == 0) &&*/ (TriggerValidFlag==TRUE))//need to TRIGGER
				{	TriggerValidFlag=FALSE;
					PutStringUsb("\r\nTrig:Distance");
					//TrigResponseFlag = 1;
					//UpdateProjectParameter2Memory(MY_PID_DISTANCE);
					
					if(TriggerPriorityVal <= TrigPriority)
					{	TriggerPriorityVal = TrigPriority; 
						TriggerPid = MY_PID_DISTANCE;
					}
					//save it to last trigger value
					u.c[0]=LastServiceDate[0] = GpsLastValidData.TimeStamp.tm_year;
					u.c[1]=LastServiceDate[1] = GpsLastValidData.TimeStamp.tm_mon;
					u.c[2]=LastServiceDate[2] = GpsLastValidData.TimeStamp.tm_mday;
					u.c[3]|= SERVICE_DATE_TRIGGER_BIT;
					ServiceDateTriggerFlag = TRUE;
					RandomMemWrite(LAST_TIME_TRIG_DATEa,u.c,LAST_TIME_TRIG_DATEn);
					PidTPR[MY_PID_DISTANCE/8] |= (1<<(MY_PID_DISTANCE%8));
					NeedToLogFlag = TRUE;
				}
			}
		}
		
		//calculation with PID 2: ANGLE
		if(((PidMCR[MY_PID_ANGLE/8] & (1<<(MY_PID_ANGLE%8))) > 0) && ((CurDigitalInput & DI_IGNITION) > 0))
		{	GetPidMem2Var(MY_PID_ANGLE);
			//log calculation
			if(abs(GpsCurData.Angle - LastLogGpsData.Angle) > 270)   //possible because 360 & 0 notation
			{   if(GpsCurData.Angle < LastLogGpsData.Angle)
				{   u.i[0] = GpsCurData.Angle+360;
					u.i[1] = LastLogGpsData.Angle;
				}
				else
				{   u.i[0] = GpsCurData.Angle;
					u.i[1] = LastLogGpsData.Angle+360;
				}
			}
			else
			{   u.i[0] = GpsCurData.Angle;
				u.i[1] = LastLogGpsData.Angle;
			}
			u.i[0] = abs(u.i[0]-u.i[1]);
			switch(LogUnit)
			{	case UNIT_DEG:	break;
				default:		LogVal.i = 361;break;//impossible value
			}
			  
			if(u.i[0] >= LogVal.i && GpsCurData.Speed>0)	//need to log
			{   NeedToLogFlag = TRUE;
				if(TriggerPriorityVal == 0)
					TriggerPid = MY_PID_ANGLE;  
			}
			//trigger calcualtion
			if(TrigCondition > 0)//trigger active & previous there is no trigger of any parameter
			{	//TriggerValidFlag = FALSE;
				if(abs(GpsCurData.Angle - LastTriggerAngle) > 270)   //possible because 360 & 0 notation
				{   if(GpsCurData.Angle < LastTriggerAngle)
					{   u.i[0] = GpsCurData.Angle+360;
						u.i[1] = LastTriggerAngle;
					}
					else
					{   u.i[0] = GpsCurData.Angle;
						u.i[1] = LastTriggerAngle+360;
					}
				}
				else
				{   u.i[0] = GpsCurData.Angle;
					u.i[1] = LastTriggerAngle;
				}
				/*by default unit is UNIT_DEG
				switch(TrigPara1Unit)
				{	case UNIT_DEG:	TriggerValidFlag = TRUE;break;
				}
				*/
				TriggerValidFlag = TRUE;
				switch(TrigCondition)
				{	case TRIG_CONDITION_GREATER_THAN: 
						if(u.i[0] < TrigPara1Val.i)
							TriggerValidFlag = FALSE;
						break;
					case TRIG_CONDITION_SMALLER_THAN:
						if(u.i[0] > TrigPara1Val.i)
							TriggerValidFlag = FALSE;
						break;
					case TRIG_CONDITION_CHANGE_GREATER_THAN:
						u.i[0] = abs(u.i[0]-u.i[1]);
						if(u.i[0] < TrigPara1Val.i)		
							TriggerValidFlag = FALSE;
						break;
					default:TriggerValidFlag = FALSE;break;
				}
				if(TriggerValidFlag == TRUE)//need to Trigger
				{	//PutStringUsb("\r\nTrig:Angle");
					if(TriggerPriorityVal <= TrigPriority)
					{	TriggerPriorityVal = TrigPriority; 
						TriggerPid = MY_PID_ANGLE;
					}
					PidTPR[MY_PID_ANGLE/8] |= (1<<(MY_PID_ANGLE%8));
					LastTriggerAngle = GpsCurData.Angle;
					NeedToLogFlag = TRUE;
				}
			}	
		}
		else
		  	GpsCurData.Angle = 0;
		
		//calculation with PID 3: SPEED
		if((PidMCR[MY_PID_SPEED/8] & (1<<(MY_PID_SPEED%8))) > 0)
		{	GetPidMem2Var(MY_PID_SPEED);
			//log calculation
			u.i[0] = abs(GpsCurData.Speed - LastLogGpsData.Speed); //in cm/s
			switch(LogUnit)
			{	case UNIT_MPS:	LogVal.i *= MPS2CMPS;	break;
				case UNIT_KMPH:	LogVal.i *= KMPH2CMPS;	break;
				case UNIT_MPH:	LogVal.i *= MPH2CMPS;	break;
				case UNIT_KNOTS:LogVal.i *= KNOTS2CMPS;	break;
				default:		LogVal.i = 999;break;//impossible value
			}
			if(u.i[0]>LogVal.i)//need to log
			{	NeedToLogFlag = TRUE;
				if(TriggerPriorityVal == 0)
					TriggerPid = MY_PID_SPEED;  
			}
			else if((CurDigitalInput & DI_IGNITION) > 0)//if ignition is on
			{	if(LastLogGpsData.Speed > 0 && GpsCurData.Speed ==0 )//if prev speed is > 5 & cur speed < 5 then log 1 packet
				{	NeedToLogFlag = TRUE;
					PeriodicSaveFlag = TRUE;//car is going to stop. device may restart if ignition off to on
					if(TriggerPriorityVal == 0)
						TriggerPid = MY_PID_SPEED;  
				}
				else if(LastLogGpsData.Speed == 0 && GpsCurData.Speed > 0)//if prev speed is > 5 & cur speed < 5 then log 1 packet
				{	NeedToLogFlag = TRUE;
					if(TriggerPriorityVal == 0)
						TriggerPid = MY_PID_SPEED;  
				}
			}
			//trigger calcualtion
			if(TrigCondition > 0)//trigger active & previous there is no trigger of any parameter
			{	bTemp = FALSE;
				/*BY default unit is KMPH
				switch(TrigPara1Unit)
				{	case UNIT_MPS:	TrigPara1Val.i *= MPS2CMPS;TriggerValidFlag = TRUE;break;
					case UNIT_KMPH:	TrigPara1Val.i *= KMPH2CMPS;TriggerValidFlag = TRUE;break;
					case UNIT_MPH:	TrigPara1Val.i *= MPH2CMPS;TriggerValidFlag = TRUE;break;
					case UNIT_KNOTS:TrigPara1Val.i *= KNOTS2CMPS;TriggerValidFlag = TRUE;break;
				}
				*/
				TrigPara1Val.i *= KMPH2CMPS;
		   		switch(TrigCondition)
				{	case TRIG_CONDITION_GREATER_THAN:
						if(GpsCurData.Speed > (TrigPara1Val.i + (TrigPara1Val.i * 0.05)))
				  		{	if(LastTriggerSpeed.i < TrigPara1Val.i)
								TriggerValidFlag = TRUE;
							else
							  	TriggerValidFlag = FALSE;
						}
				  		else
						{	TriggerValidFlag = FALSE;
							if(GpsCurData.Speed < (TrigPara1Val.i - ((TrigPara1Val.i*5)/100)))
								LastTriggerSpeed.i = GpsCurData.Speed;
						}  
						if(GpsCurData.Speed > TrigPara1Val.i)
				  			bTemp = TRUE;//FOR OUTPUT DEPENDENT WITH SPEED
						break;
					case TRIG_CONDITION_SMALLER_THAN:
						if(GpsCurData.Speed < (TrigPara1Val.i - ((TrigPara1Val.i*5)/100)))
				  		{	if(LastTriggerSpeed.i > TrigPara1Val.i)
								TriggerValidFlag = TRUE;
							else
						  	TriggerValidFlag = FALSE;
						}
				  		else
						{	TriggerValidFlag = FALSE;
							if(GpsCurData.Speed > (TrigPara1Val.i + ((TrigPara1Val.i*5)/100)))
								LastTriggerSpeed.i = GpsCurData.Speed;
						}  
						if(GpsCurData.Speed < TrigPara1Val.i)
				  			bTemp = TRUE;//FOR OUTPUT DEPENDENT WITH SPEED
						break;
					case TRIG_CONDITION_CHANGE_GREATER_THAN:
						u.i[0] = abs(GpsCurData.Speed - LastTriggerSpeed.i);
						if(u.i[0] > TrigPara1Val.i)
						{	TriggerValidFlag = TRUE;
							bTemp = TRUE;
						}
						else
							TriggerValidFlag = FALSE;
						break;
				}
				if(TriggerValidFlag == TRUE)//need to Trigger
				{	PutStringUsb("\r\nTrig:Speed");
					if(TriggerPriorityVal <= TrigPriority)
					{	TriggerPriorityVal = TrigPriority; 
						TriggerPid = MY_PID_SPEED;
					}
					LastTriggerSpeed.f = GpsCurData.Speed;
					PidTPR[MY_PID_SPEED/8] |= (1<<(MY_PID_SPEED%8));
					NeedToLogFlag = TRUE;
				}
				if(bTemp == TRUE)
				{	//check if any output is allocated to SPEED then switch on relay
					UpdateOutputAsPerDependency(OUTPUT_DEPENDENCY_SPEED,ON);
				}
				else
				{	//check if any output is allocated to SPEED then switch off relay
					UpdateOutputAsPerDependency(OUTPUT_DEPENDENCY_SPEED,OFF);
				}
			}
			if(DisableIgnitionFlag==TRUE && GpsCurData.Speed < 300)
			{	if(UpdateOutputAsPerDependency(OUTPUT_DEPENDENCY_ENGINE_OFF,ON) == TRUE)
				{	strcpy(PasteBuff,"Engine DISABLED.");
					NeedToLogFlag = TRUE;
				}
				else
				{	strcpy(PasteBuff,"ERROR: ENGINE Dependency is NOT assigned to any Digital Output.");
				}  
				DisableIgnitionFlag = FALSE;
				u.ll = 0;
				DirectMemRead(USER_ADMIN_MOBa,u.c,UT_MOBn);
				CommandFormat.Sender = u.ll;
				SendCommandReplyToSender(PasteBuff);
			}
		}
		//calculation with GEOFENCE
		for(uiTemp=MY_PID_GEOFENCE_START;uiTemp<=MY_PID_GEOFENCE_END;uiTemp++)
		{	FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
			FM3_SWWDT->WDOGINTCLR = 0x11111111;
			FM3_SWWDT->WDOGLOCK = 0;
			//sprintf(PasteBuff,"\r\nGoefence%d",uiTemp-MY_PID_GEOFENCE_START);
			//PutStringUsb(PasteBuff);
		  	if((PidMCR[(uiTemp)/8] & (1<<((uiTemp)%8))) > 0)
			{	//PutStringUsb(" Control ENABLED");
			  	GetPidMem2Var(uiTemp);
				if(TrigCondition > 0)//trigger active & previous there is no trigger of any parameter
				{	//PutStringUsb(" TrigCondition>0");
				  	//calculate distance from geofence x y
					distancecalc.f[0] = (GpsCurDataInRadian.f[1]-TrigPara1Val.f) * cos((GpsCurDataInRadian.f[0]+TrigPara2Val.f)/2);
					distancecalc.f[1] = GpsCurDataInRadian.f[0]-TrigPara2Val.f;
					u.f[0] = distancecalc.f[0]*distancecalc.f[0]+distancecalc.f[1]*distancecalc.f[1];
					distancecalc.d = (long)(6371000 * sqrt(u.f[0]));
					//trigger distance calculation
					/*by default unit is mtr
					TriggerValidFlag = FALSE;
					switch(TrigPara3Unit)
					{	case UNIT_MTR:							TriggerValidFlag=TRUE;break;
						case UNIT_KM:	TrigPara3Val.i *= 1000;	TriggerValidFlag=TRUE;break;
						case UNIT_MILE:	TrigPara3Val.i *= 1600;	TriggerValidFlag=TRUE;break;
					}
					if(TriggerValidFlag == TRUE)//MATCH UNIT FOUND SO CHECK DISTANCE
					*/
					{	//PutStringUsb(" TriggerValid");
						TriggerValidFlag=TRUE;
					  	switch(TrigCondition)
						{	case TRIG_CONDITION_WINDOW_OUT: 
								//sprintf(PasteBuff," WindowOUT distcalc = %f > %d",distancecalc.d,TrigPara3Val.i + (TrigPara3Val.i/10));
								//PutStringUsb(PasteBuff);
						  		if(distancecalc.d > (TrigPara3Val.i + (TrigPara3Val.i/10)))
						  		{	ucTemp = (uiTemp-MY_PID_GEOFENCE_START);
									if((GeofenceLastTrigVal[ucTemp/8] & (1<<(ucTemp%8))) > 0)
									{	TriggerValidFlag = FALSE;
										//PutStringUsb(" OUTSIDE BOUNDARY");
						  			}
									//else
									//	PutStringUsb(" INSIDE BOUNDARY");
						  			GeofenceLastTrigVal[ucTemp/8] |= (1<<(ucTemp%8));
								}
								else
								{	TriggerValidFlag = FALSE;
									if(distancecalc.d < (TrigPara3Val.i - (TrigPara3Val.i/10)))
						  			{	ucTemp = (uiTemp-MY_PID_GEOFENCE_START);
										GeofenceLastTrigVal[ucTemp/8] &= ~(1<<(ucTemp%8));
										//if((GeofenceLastTrigVal[ucTemp/8] & (1<<(ucTemp%8))) == 0)
										//	PutStringUsb(" INSIDE BOUNDARY");
									}
									//else
									//  PutStringUsb(" INBETWEEN BOUNDARY");
								}
						  		if(uiTemp == MY_PID_TOW_GEOFENCE && ((CurDigitalInput & DI_IGNITION) > 0))//IF IGNITION ON
									TriggerValidFlag=FALSE;
								else if(uiTemp == MY_PID_PARK_GEOFENCE && ((CurDigitalInput & DI_IGNITION) == 0))//IF IGNITION OFF
									TriggerValidFlag=FALSE;
								
								//Activate below code if repeated message is required
								//if(uiTemp == TOW && uiTemp==PARKING)//if it is auto geofence
								//{	GeoParameter[uiTemp].TrigLatVal = GpsCurDataInRadian.f[0];
								//	GeoParameter[uiTemp].TrigLngVal = GpsCurDataInRadian.f[1];
								//}
								break;
							case TRIG_CONDITION_WINDOW_IN:
								if(distancecalc.d < (TrigPara3Val.i - (TrigPara3Val.i/10)))
						  		{	if(GeofenceLastTrigVal[uiTemp] == INSIDE_BOUNDARY)
								  	{	TriggerValidFlag = FALSE;
									}
									GeofenceLastTrigVal[uiTemp] = INSIDE_BOUNDARY;
								}
								else
								{	TriggerValidFlag = FALSE;
									if(distancecalc.d > (TrigPara3Val.i + (TrigPara3Val.i/10)))
						  			{	GeofenceLastTrigVal[uiTemp] = OUTSIDE_BOUNDARY;
									}
								}
						  		break;
							case TRIG_CONDITION_WINDOW_IN_OR_OUT:
								if(distancecalc.d > (TrigPara3Val.i + (TrigPara3Val.i/10)))
						  		{	if(GeofenceLastTrigVal[uiTemp] == OUTSIDE_BOUNDARY)
								  		TriggerValidFlag = FALSE;
									GeofenceLastTrigVal[uiTemp] = OUTSIDE_BOUNDARY;
						  		}
								else
								{	if(distancecalc.d < (TrigPara3Val.i - (TrigPara3Val.i/10)))
									{	if(GeofenceLastTrigVal[uiTemp] == INSIDE_BOUNDARY)
											TriggerValidFlag = FALSE;
										GeofenceLastTrigVal[uiTemp] = INSIDE_BOUNDARY;
									}
									else
									{	TriggerValidFlag = FALSE;
									}
								}
						  		break;
						}
					}
					if(TriggerValidFlag==TRUE)//need to TRIGGER
					{	TriggerValidFlag=FALSE;
						//CHECK trigger TIME BOUND
						u.i[0] = 0;
						DirectMemRead(PP_START_LOC + MY_PID_GEOFENCE5 * PP_MAX_BYTE + PP_STOP_TIMESTAMp,u.c,PP_STOP_TIMESTAMn);
						u.i[1] = (u.i[0]/100)*60 + u.i[0]%100;//converting timestamp in min
						u.i[0] = 0;
						DirectMemRead(PP_START_LOC + MY_PID_GEOFENCE5 * PP_MAX_BYTE + PP_START_TIMESTAMp,u.c,PP_START_TIMESTAMn);
						uiTemp = (u.i[0]/100)*60 + u.i[0]%100;//converting timestamp in min
						u.i[1] = uiTemp;
						uiTemp = (TimeStamp2Sec(Device.TimeStamp) + SecAfterTimeSync)/60;
						if(uiTemp >= u.i[0] && uiTemp < u.i[1]){
							if(TriggerPriorityVal <= TrigPriority)
							{	TriggerPriorityVal = TrigPriority; 
								TriggerPid = uiTemp;
							}
							PidTPR[uiTemp/8] |= (1<<(uiTemp%8));
							NeedToLogFlag = TRUE;
							sprintf(CopyBuff,"\r\nTrig:Geofence %d ",uiTemp-MY_PID_GEOFENCE_START);//TESTING CODE
							PutStringUsb(CopyBuff);//TESTING CODE
							//TESTING CODE
							ucTemp = (uiTemp-MY_PID_GEOFENCE_START);
							if((GeofenceLastTrigVal[ucTemp/8] & (1<<(ucTemp%8))) > 0)
								PutStringUsb("OUTSIDE BOUNDARY");//TESTING CODE
							else//TESTING CODE
								PutStringUsb("INSIDE BOUNDARY");//TESTING CODE
							//break;//break from for loop
						}
					}
				}
			}
			
			
		}
		
		LED_OFF(LED_BLUE);
		
		//if(NeedToLogFlag == TRUE)
		//	LogRecord();
	}
	else
	{  	GpsLastValidData.NoS = 0;	//to indicate current gps packet is of last valid gps data received
		if(!(FreeRunSec % 60))
			LED_ON(LED_BLUE);
		else
		  	LED_OFF(LED_BLUE);
		if(PowerOnPositionShiftFlag == TRUE)
		{	//to make device in server network even if GPS signal is not available
			uiTemp = TimeStamp2Sec(Device.TimeStamp) + SecAfterTimeSync - TimeStamp2Sec(LastLogGpsData.TimeStamp);
			GetPidMem2Var(MY_PID_TIME);
			switch(LogUnit)
			{	case UNIT_SEC:	break;
				case UNIT_MIN:	LogVal.i *= 60;break;
				case UNIT_HR:	LogVal.i *= 3600;break;
				default:		LogVal.i = 0xffffffff;break;//max value
			}
			if(uiTemp >= LogVal.i)//need to log
			{	//PutStringUsb("\r\nDEBUG > Logging data while GPS signal is not available");
				NeedToLogFlag = TRUE;
				ContinueLastLogData();
				if(TriggerPriorityVal == 0)
					TriggerPid = MY_PID_TIME;  
			}
		}
	}
}
void ContinueLastLogData()
{	LastToCurrDistance.i[0] = 0;
	GpsCurDataInRadian.f[0] = LastLogGpsData.Lat;//it will always store the last gps valid data in radian
	GpsCurDataInRadian.f[1] = LastLogGpsData.Lng;//it will always store the last gps valid data in radian
	GpsLastValidData.Altitude = LastLogGpsData.Altitude;
	GpsLastValidData.Angle = LastLogGpsData.Angle;
	GpsLastValidData.Speed = 0;
	GpsLastValidData.Angle = LastLogGpsData.Angle;//to omit angle log
	GpsLastValidData.NoS = LastLogGpsData.NoS;	
}
//------------------------------------------------------------------------------
void GetLocation(char whichone)
{	DirectMemRead(VEHICLE_REG_NOa,PasteBuff,VEHICLE_REG_NOn);
	PasteBuff[VEHICLE_REG_NOn] = '\0';
	if(whichone == GPS)
	{	strcat(PasteBuff,"\r\nFROM SATELLITE");
	  	strcat(PasteBuff,Gmt2LocalTimeStamp(GpsLastValidData.TimeStamp,LAST));
		sprintf(CopyBuff,"\r\nhttp://maps.google.com/?q=%.6f,%.6f",
					(LastLogGpsData.Lat*180)/PI,
					(LastLogGpsData.Lng*180)/PI);
		strcat(PasteBuff,CopyBuff);
	}
	else
	{	strcat(PasteBuff,"\r\nFROM TOWER");
	  	strcat(PasteBuff,Gmt2LocalTimeStamp(GprsPacket.TimeStamp,LAST));
		sprintf(CopyBuff,"\r\nhttp://maps.google.com/?q=%.6f,%.6f",
				GprsPacket.Lat,
				GprsPacket.Lng);
		strcat(PasteBuff,CopyBuff);
	}
	sprintf(CopyBuff,"\r\nCurrently\r\nEngine=%s",
				((CurDigitalInput & 0x01) > 0)?"ON":"OFF");
		strcat(PasteBuff,CopyBuff);
	if(GpsPacketValidFlag == TRUE)
    {	strcat(PasteBuff,"\r\nGps:FIXED");
		GetPidMem2Var(MY_PID_SPEED);
		switch(LogUnit)
		{	case UNIT_MPS:	sprintf(CopyBuff,"\r\nSpeed: %.1f MPS",(float)(GpsCurData.Speed / MPS2CMPS));break;
			default:
			case UNIT_KMPH:	sprintf(CopyBuff,"\r\nSpeed: %.1f KMPH",(float)(GpsCurData.Speed / KMPH2CMPS));break;
			case UNIT_MPH:	sprintf(CopyBuff,"\r\nSpeed: %.1f MPH",(float)(GpsCurData.Speed / MPH2CMPS));	break;
			case UNIT_KNOTS:sprintf(CopyBuff,"\r\nSpeed: %.1f Knots",(float)(GpsCurData.Speed / KNOTS2CMPS));break;
		}
		strcat(PasteBuff,CopyBuff);
	}
	else
	{	strcat(PasteBuff,"\r\nGps:NOT FIXED");
	}
}
//------------------------------------------------------------------------------
void GPS_RX_IRQHandler(void)
{	static unsigned char FirstDollorFlag = FALSE;
	static unsigned char GpsRecvCounter = 0;
	uc GpsRecvByte;

	//LED_TOG(LED_BLUE);

  if ((GPS_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	GPS_UART->SSR |= (1 << 7);      // Clear error flags 
    	GpsRecvByte = GPS_UART->RDR;         // Flush RDR
  	}
  	else if (GPS_UART->SSR & 0x04)    // RDRF?
  	{	GpsRecvByte = GPS_UART->RDR;
    	//PutCharGps(GpsRecvByte);	//for debug on same port
		if(GpsInterruptRecvDebugFlag == TRUE)
			PutCharUsb(GpsRecvByte);
		if(GpsCommandFlag == FALSE)
		{	switch(GpsRecvByte)
			{ 	case '$': GpsRecvCounter = 0; FirstDollorFlag = TRUE;break;
				case '*':	GpsBuff[GpsPktCntr%GPS_MAX_PKT][GpsRecvCounter % GPS_PKT_BUFF_SIZE] = GpsRecvByte;
							//check the message type and discard if not required one
				  			if(GpsBuff[GpsPktCntr%GPS_MAX_PKT][2] == 'R' && GpsBuff[GpsPktCntr%GPS_MAX_PKT][3] == 'M' && GpsBuff[GpsPktCntr%GPS_MAX_PKT][4] == 'C')
							{	
							}
							else if(GpsBuff[GpsPktCntr%GPS_MAX_PKT][2] == 'G' && GpsBuff[GpsPktCntr%GPS_MAX_PKT][3] == 'G' && GpsBuff[GpsPktCntr%GPS_MAX_PKT][4] == 'A')
							{	
							}
							else if(GpsBuff[GpsPktCntr%GPS_MAX_PKT][2] == 'T' && GpsBuff[GpsPktCntr%GPS_MAX_PKT][3] == 'X' && GpsBuff[GpsPktCntr%GPS_MAX_PKT][4] == 'T')
							{	
							}
							else
							  FirstDollorFlag = FALSE;
							GpsRecvCounter++;
							break;
				case 0x0A:  if(FirstDollorFlag == TRUE)
							{   GpsBuff[GpsPktCntr%GPS_MAX_PKT][GpsRecvCounter % GPS_PKT_BUFF_SIZE] = 0x0a;
								GpsBuff[GpsPktCntr%GPS_MAX_PKT][(GpsRecvCounter+1) % GPS_PKT_BUFF_SIZE] = '\0';
								if(GpsPktCntr >= GPS_MAX_PKT)
								{	GpsCommandFlag = TRUE;
								}
								else
								{  	GpsPktCntr++;
								}
								FirstDollorFlag = FALSE;
							}
							break;
				default:    GpsBuff[GpsPktCntr%GPS_MAX_PKT][GpsRecvCounter % GPS_PKT_BUFF_SIZE] = GpsRecvByte;
							GpsRecvCounter++;
							break;
			}
		}
	}
}
//------------------------------------------------------------------------------
void InitGps()
{	int temp;
  	GPS_GPIO_INIT;
	GPS_GPIO_HIGH(GPS_EN);
	GPS_GPIO_HIGH(GPS_RESET);
	InitUartGps();
	for(temp = 400; temp > 0; temp--) 
		Delay(65530);
	GPS_GPIO_LOW(GPS_RESET);
}  	
//------------------------------------------------------------------------------
void InitUartGps()
{	// Initialize ports for UART
  	GPS_UART_PFR;
	GPS_UART_EPFR;  
  
  	// Initialize UART asynchronous mode
  	GPS_UART->SCR =  0x80;   // UART reset
  	GPS_UART->SMR =  0x01;   // serial data output enable, operation mode 0
  	GPS_UART->SSR =  0x00;   // 0x80 = clear receive error flag
  	GPS_UART->ESCR = 0x00;          
 
  	GPS_UART->BGR = ((40000000UL / (GPS_BAUDRATE -1))/2);
  
	GPS_UART->SCR |= 0x03;   // RX, TX enable  
  	//FM3_MFS0_UART->FBYTE = 0X0003;//INTERRUPT ON FIVE BYTE RECEIVE TESTED
  
  	// UART interrupt settings
  	NVIC_ClearPendingIRQ(GPS_RX_IRQn);
  	NVIC_EnableIRQ(GPS_RX_IRQn);
  	NVIC_SetPriority(GPS_RX_IRQn, 1);
  
  	GPS_UART->SCR |= 0x10;//set RIE serial receive interrupt enable bit
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharGps(uint8_t u8Char)
{	while (!(GPS_UART->SSR & (1 << 1))); // wait for transmit buffer empty    
  	GPS_UART->TDR = u8Char;                // put u8Char into buffer
  	if(GpsCrcCodeCalFlag == TRUE)	
		GpsCrcCodeOut ^= u8Char;
//  	Delay(20000);
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *  RETURNS:        None
 *****************************************************************************/
void PutStringGps(char *pu8Buffer)              
{	uc temp;
	//PutStringUsb("Sending Command to GPS");
	
	PutCharGps('$');
	//PutCharUsb('$');
	GpsCrcCodeOut = 0;
    GpsCrcCodeCalFlag = TRUE;
	while (*pu8Buffer != '\0')
  	{ 	PutCharGps(*pu8Buffer);	// send every char of string
		//PutCharUsb(*pu8Buffer);
		pu8Buffer++;
		//Delay(65000);Delay(65000); //must for long replay from gprs
  	}
	GpsCrcCodeCalFlag = FALSE;	
	//SENDING CRC CODE
	PutCharGps('*');
	//PutCharUsb('*');
	temp = GpsCrcCodeOut >> 4;
	switch(temp)
	{	case 0:	PutCharGps('0');
				//PutCharUsb('0');
				break;
		case 1: PutCharGps('1');
				//PutCharUsb('1');
				break;
		case 2: PutCharGps('2');
				//PutCharUsb('2');
				break;
		case 3: PutCharGps('3');
				//PutCharUsb('3');
				break;
		case 4: PutCharGps('4');
				//PutCharUsb('4');
				break;
		case 5: PutCharGps('5');
				//PutCharUsb('5');
				break;
		case 6: PutCharGps('6');
				//PutCharUsb('6');
				break;
		case 7: PutCharGps('7');
				//PutCharUsb('7');
				break;
		case 8: PutCharGps('8');
				//PutCharUsb('8');
				break;
		case 9: PutCharGps('9');
				//PutCharUsb('9');
				break;
		case 10: PutCharGps('A');
				//PutCharUsb('A');
				break;
		case 11: PutCharGps('B');
				//PutCharUsb('B');
				break;
		case 12: PutCharGps('C');
				//PutCharUsb('C');
				break;
		case 13: PutCharGps('D');
				//PutCharUsb('D');
				break;
		case 14: PutCharGps('E');
				//PutCharUsb('E');
				break;
		case 15: PutCharGps('F');
				//PutCharUsb('F');
				break;
	}
	temp = GpsCrcCodeOut & 0x0f;
	switch(temp)
	{	case 0:	PutCharGps('0');
				//PutCharUsb('0');
				break;
		case 1: PutCharGps('1');
				//PutCharUsb('1');
				break;
		case 2: PutCharGps('2');
				//PutCharUsb('2');
				break;
		case 3: PutCharGps('3');
				//PutCharUsb('3');
				break;
		case 4: PutCharGps('4');
				//PutCharUsb('4');
				break;
		case 5: PutCharGps('5');
				//PutCharUsb('5');
				break;
		case 6: PutCharGps('6');
				//PutCharUsb('6');
				break;
		case 7: PutCharGps('7');
				//PutCharUsb('7');
				break;
		case 8: PutCharGps('8');
				//PutCharUsb('8');
				break;
		case 9: PutCharGps('9');
				//PutCharUsb('9');
				break;
		case 10: PutCharGps('A');
				//PutCharUsb('A');
				break;
		case 11: PutCharGps('B');
				//PutCharUsb('B');
				break;
		case 12: PutCharGps('C');
				//PutCharUsb('C');
				break;
		case 13: PutCharGps('D');
				//PutCharUsb('D');
				break;
		case 14: PutCharGps('E');
				//PutCharUsb('E');
				break;
		case 15: PutCharGps('F');
				//PutCharUsb('F');
				break;
	}
	PutCharGps('\r');
	PutCharGps('\n');
	//PutStringUsb("\r\n");
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    returns incoming character, if received
 *  PARAMETERS:     None
 *  RETURNS:        Character or 0xFF (Error) or 0 (Nothing)
 *****************************************************************************/
uint8_t GetCharGps(void)   
{	uint8_t u8Char;
  	if ((GPS_UART->SSR & 0xE0) != 0)  // Check for errors PE, ORE, FRE
  	{	GPS_UART->SSR |= (1 << 7);      // Clear error flags 
    	u8Char = GPS_UART->RDR;         // Flush RDR
    	return 0xFF;
  	}
  	else if (GPS_UART->SSR & 0x04)    // RDRF?
  	{	u8Char = GPS_UART->RDR;
    	if ('\r' == u8Char)
    	{	u8Char = '\n';
    	}
    	return (u8Char); 
  	}
  	else
  	{	return 0; 
  	}
}
//------------------------------------------------------------------------------
