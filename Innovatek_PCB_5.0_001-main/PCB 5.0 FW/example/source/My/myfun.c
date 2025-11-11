#include"myfun.h"
#include"Variable.h"
//#include"AT45DB161D.h"
#include"gprs.h"
#include"constants.c"
#include"LED.H"
#include"GpsUart.h"
#include"GprsUart.h"
#include"string.h"
#include"command.h"
#include"gpio.h"
#include"delay.h"
#include "SerialHmi.h"
//------------------------------------------------------------------------------
//***************** MY VARIABLE STARTS *****************************************
//char Buff[BYTE_PER_PAGE];
//***************** MY VARIABLE ENDS *******************************************
//------------------------------------------------------------------------------
/*
long long A2D(const char*str)
{	long long d;
	char *s;
	for(d=0,s=(char*)str;*s!='\0';s++)
	{	d *= 10;
		d += *s-48;
	}
	return d;
}
*/
//------------------------------------------------------------------------------
/*
int A2I(char*str)
{	int i;
	//for(i=0;*str!='\0';str++)
	for(i=0;*str>='0' && *str<='9';str++)
	{	i *= 10;
		i += *str-48;
	}
	return i;
}
*/
//------------------------------------------------------------------------------
/*
void D2A(long long d,char*buff)   //function to convert long long into array
{   unsigned char n;
    char array[20];
    if(d>0)
	{	for(n=0;d>0;n++,d/=10)
        	array[n] = d%10+48;
    	for(;n>0;n--,buff++)
      		*buff = array[n-1];
	}
	else
	{	*buff = '0';
		buff++;
	}
    *buff = '\0';
}
*/



//------------------------------------------------------------------------------

//use sprintf for negative value
void F2A(float f,char*s,char digit_after_decimal)   //function to convert float into array
{   int i,j;
	char c,temp[20];
	i = j= (int)f;
	if(i>0)
	{	for(c=0;i>0;c++,i/=10)
	  		temp[c] = i%10+48;
		for(;c>0;c--,s++)
			*s = temp[c-1];
	}
	else
	{	*s = '0';
		s++;
	}
	*s = '.';
	s++;
	f = f - j;
	for(c=0;c < digit_after_decimal;c++,s++)
	{	f *= 10;
		i = (int)f;
		*s = i + 48;
		f = f - i;
	}
	*s = '\0';
}
/*
int DateDifference(struct tm date1, struct tm date2)
{	
	// assume date1 is older (<) date than date2
	int temp=0;
	if(date1.tm_mon == date2.tm_mon)
	{
		if(date2.tm_mday >= date1.tm_mday)
			temp += date2.tm_mday - date1.tm_mday;
		else
			temp += date1.tm_mday - date2.tm_mday;
	}
	else if(date1.tm_mon > date2.tm_mon)
	{	temp += date1.tm_mday;
		date1.tm_mon--;
		while(date1.tm_mon >= date2.tm_mon)
		{
			switch(date1.tm_mon)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:	temp += 31; break; 
				
				case 2:		if(date1.tm_year%4 == 0) 
								temp +=29;
							else 
								temp += 28;
							break;
				default: 	temp += 30; break;
			}
			date1.tm_mon--;
		} 
		temp -=date2.tm_mday;
	}
	else if(date1.tm_mon < date2.tm_mon)
	{
		for(int i=0; date1.tm_mon<date2.tm_mon;i++,date1.tm_mon++)
		{
			switch(date1.tm_mon)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:	if(i==0)
								temp += 31 - date1.tm_mday;
							else
								temp += 31;	
							break;
				
				case 2:		if(i==0)
							{	if(date1.tm_year%4==0) 
									temp += 29 - date1.tm_mday;
								else 
									temp += 28 - date1.tm_mday;
							}
							else if(date1.tm_year%4==0) 
								temp += 29;
							else 
								temp += 28;
							break;
				
				default:	if(i==0)
								temp += 30 - date1.tm_mday;
							else
								temp += 30;
							break;
			}
		}
		if(date1.tm_mday > date2.tm_mday)
			temp += date1.tm_mday - date2.tm_mday;
		else
			temp += date2.tm_mday - date1.tm_mday;
	}
	while(date1.tm_year < date2.tm_year){
		if(date1.tm_year%4!=0)
			temp += 365;
		else
			temp += 366;
	}
	return temp;		
}
*/
boolean_t ValidateDate(uc date, uc month, uc year)
{ 
  	boolean_t return_val = TRUE;
  	switch(month)
  	{
		case 2:		if(year%4 == 0)
					{
						if(date>29)
						  return_val = FALSE;
					}
					else
					{
						if(date>28)
							return_val = FALSE;
					}
					break;
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:	if(date>31)
						return_val = FALSE;
					break;
		case 4:
		case 6:
		case 9:
		case 11:	if(date>30)
						return_val = FALSE;
					break;
		default:	return_val = FALSE;
  }
  return return_val;

}

/*
void Gmt2LocalDateTime(ui date, ui time)
{	u.c[0] = date%100;//date
	u.c[1] = (date/100)%100;//month
	u.c[2] = date/10000;//year
	iTemp = time + Device.TimeZone;
	if(iTemp<0)//local date is one less than gps date
	{ 	//PutStringUsb("\r\nLocal date is one less than gps date");
		iTemp *= -1;
		iTemp = 86400-iTemp;
		u.c[0]--;
		if(u.c[0]==0)
		{ 	u.c[1]--;
			switch(u.c[1])
			{ 	case 0: u.c[2]--;u.c[1]=12;u.c[0]=31;break;
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10: u.c[0]=31;break;
				case 2: if(u.c[2]%4==0)
							u.c[0]=29;
						else
							u.c[0]=28;
						break;
				case 4:
				case 6:
				case 9:
				case 11: u.c[0] = 30;break;
			}
		}
	}
	else//local date is equal or one greater than gps date
	{ 	if(iTemp >= 86400)//local date is one greater than gps date
		{ 	//PutStringUsb("\r\nLocal date is one greater than gps date");
			u.c[0]++;
			switch(u.c[1])
			{ 	case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:if(u.c[0]==32)
						{ 	u.c[1]++;
							if(u.c[1]==12)
							{	u.c[2]++;
								u.c[1]=1;
							}
							u.c[0]=1;
						}
						break;
				case 2: if(u.c[2]%4==0)
						{ 	if(u.c[0]==30)
							{	u.c[1]++;
								u.c[0]=1;
							}
						}
						else
						{ 	if(u.c[0]==29)
							{ 	u.c[1]++;
								u.c[0]=1;
							}
						}
						break;
				case 4:
				case 6:
				case 9:
				case 11:if(u.c[0]==31)
						{ 	u.c[1]++;
							u.c[0]=1;
						}
						break; 
			}
		}
		//else
		  //PutStringUsb("\r\nLocal date is equal to gps date");
	}
	iTemp %= 86400;
}*/
//------------------------------------------------------------------------------
long long StrMobileNo2LL(const char *mobno)//format : +919910464351
{	long long mn;
	char *mno;
	RemoveCharFromString(' ',mobno);
	mno = (char*)mobno;
	if(*mno=='+' && strlen(mobno) > 11)
	{	mno++;
	  	for(mn=0;*mno>='0' && *mno<='9';mno++)
		{	mn *= 10;
			mn += *mno-48;
		}
		return mn;
	}
	return -1;//0xFFFFFFFFFFFFFFFF;
}
//------------------------------------------------------------------------------
/*
void I2A(unsigned int i,char*buff)   //function to convert integer into array
{   unsigned char n;
    char array[6];
    if(i>0)
	{	for(n=0;i>0;n++,i/=10)
        	array[n] = i%10+48;
    	for(;n>0;n--,buff++)
      	*buff = array[n-1];
	}
	else
	{	*buff = '0';
		buff++;
	}
    *buff = '\0';
}
*/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
void L2A(unsigned long l,char*buff)   //function to convert integer into array
{   unsigned char n;
    char array[11];
    if(l>0)
	{	for(n=0;l>0;n++,l/=10)
        	array[n] = l%10+48;
    	for(;n>0;n--,buff++)
      		*buff = array[n-1];
	}
	else
	{	*buff = '0';
		buff++;
	}
    *buff = '\0';
}
*/
//------------------------------------------------------------------------------
/* example: String = "hello nabeen how r u doing?"
	search text = "how"
	returns = 16 (address immediate after "how")
*/
int SearchString(const char*stringstartptr,const char*searchtextstartptr)
{	unsigned int foundloc;
	char* string;
	char* searchtext;
	char found;
	string = (char*)stringstartptr;
	searchtext = (char*)searchtextstartptr;
	for(foundloc=0;*string!='\0';string++,foundloc++)
	{   if(*string==*searchtext)
		{   found = 0;
			for(;*searchtext!='\0';searchtext++,string++,foundloc++)
			{	if(*string!=*searchtext && *searchtext !='?')
				{	found = -1;
					searchtext = (char*)searchtextstartptr;
					break;
				}
			}
			if(found ==0)
				return foundloc;//returns immediate address loc after serarch text
		}
	}
	return -1;
}
//------------------------------------------------------------------------------
unsigned int AsciiHex2ValInt(char* asciibuff)
{	ui temp=0;
	while(*asciibuff != '\0')
	{	temp <<= 4;
	  	switch(*asciibuff)
		{	case '0': break;
			case '1': temp += 1;break;
			case '2': temp += 2;break;
			case '3': temp += 3;break;
			case '4': temp += 4;break;
			case '5': temp += 5;break;
			case '6': temp += 6;break;
			case '7': temp += 7;break;
			case '8': temp += 8;break;
			case '9': temp += 9;break;
			case 'a':
			case 'A': temp += 10;break;
			case 'b':
			case 'B': temp += 11;break;
			case 'c':
			case 'C': temp += 12;break;
			case 'd':
			case 'D': temp += 13;break;
			case 'e':
			case 'E': temp += 14;break;
			case 'f':
			case 'F': temp += 15;break;
		}
		asciibuff++;
	}
	return temp;
}
unsigned char AsciiHex2Hex(char* ascii)	//converts 2 byte hex format ascii to actual hex 
{	unsigned char hex = 0;
	switch(*ascii)
	{	case '0': hex = 0x00;break;
		case '1': hex = 0x10;break;
		case '2': hex = 0x20;break;
		case '3': hex = 0x30;break;
		case '4': hex = 0x40;break;
		case '5': hex = 0x50;break;
		case '6': hex = 0x60;break;
		case '7': hex = 0x70;break;
		case '8': hex = 0x80;break;
		case '9': hex = 0x90;break;
		case 'A':
		case 'a': hex = 0xA0;break;
		case 'B': 
		case 'b': hex = 0xB0;break;
		case 'C':
		case 'c': hex = 0xC0;break;
		case 'D':
		case 'd': hex = 0xD0;break;
		case 'E':
		case 'e': hex = 0xE0;break;
		case 'F':
		case 'f': hex = 0xF0;break;
	}
	ascii++;
	switch(*ascii)
	{	case '0': hex += 0x00;break;
		case '1': hex += 0x01;break;
		case '2': hex += 0x02;break;
		case '3': hex += 0x03;break;
		case '4': hex += 0x04;break;
		case '5': hex += 0x05;break;
		case '6': hex += 0x06;break;
		case '7': hex += 0x07;break;
		case '8': hex += 0x08;break;
		case '9': hex += 0x09;break;
		case 'A':
		case 'a': hex += 0x0A;break;
		case 'B': 
		case 'b': hex += 0x0B;break;
		case 'C':
		case 'c': hex += 0x0C;break;
		case 'D':
		case 'd': hex += 0x0D;break;
		case 'E':
		case 'e': hex += 0x0E;break;
		case 'F':
		case 'f': hex += 0x0F;break;
	}
	return hex;
}
void MyStrCpy(char*dptr,char*sptr,ui no_of_char)
{	int i;
  	for(i=0;*sptr!='\0' && i< no_of_char;dptr++,sptr++,i++)
	{	*dptr = *sptr;
	}
	*dptr = '\0';
}
void FindAndReplaceCharInString(char*string, char find, char replace)
{   while(*string != '\0')
    {   if(*string == find)
            *string = replace;
        string++;
    }
}
void RemoveCharFromEndOfString(char ch,char *str)
{   char len;
    len = strlen(str);
    for(;*(str+len-1)==ch; len--)
      *(str+len-1) = '\0';
}
void RemoveCharFromString(char ch,const char *str)
{	char *sptr,*dptr;//source and destination pointer
	for(sptr=(char*)str,dptr=(char*)str;*sptr!='\0';sptr++)
	{	if(*sptr!=ch)
		{	*dptr = *sptr;
			dptr++;
		}
	}
	*dptr='\0';
}
void String2Upper(char* s)
{	while(*s != '\0')
	{	if(*s >= 'a' && *s <= 'z')
	  		*s -= ('a'-'A');
		s++;
	}
}
/*
char TrigCondition2Constant(char*str)
{	
  	RemoveCharFromString(' ',str);//remove space from the string before comparision
  	String2Upper(str);	//change string to uppercase before comparision.
	
  	if(*str == NULL)
  		return -1;
	else if(strlen(str)==0)
	  	return -1;
	else if(strcmp(str,TRIG_CONDITION_STR_NONE)==0)
	  	return TRIG_CONDITION_NONE;
	else if(strcmp(str,TRIG_CONDITION_STR_CHANGE_HIGH)==0)
	  	return TRIG_CONDITION_CHANGE_HIGH;
	else if(strcmp(str,TRIG_CONDITION_STR_CHANGE_LOW)==0)
	  	return TRIG_CONDITION_CHANGE_LOW;
	else if(strcmp(str,TRIG_CONDITION_STR_CHANGE_BOTH)==0)
	  	return TRIG_CONDITION_CHANGE_BOTH;
	else if(strcmp(str,TRIG_CONDITION_STR_WINDOW_IN)==0)
	  	return TRIG_CONDITION_WINDOW_IN;
	else if(strcmp(str,TRIG_CONDITION_STR_WINDOW_OUT)==0)
	  	return TRIG_CONDITION_WINDOW_OUT;
	else if(strcmp(str,TRIG_CONDITION_STR_CHANGE_GREATER_THAN)==0)
	  	return TRIG_CONDITION_CHANGE_GREATER_THAN;
	else if(strcmp(str,TRIG_CONDITION_STR_GREATER_THAN)==0)
	  	return TRIG_CONDITION_GREATER_THAN;
	else if(strcmp(str,TRIG_CONDITION_STR_SMALLER_THAN)==0)
	  	return TRIG_CONDITION_SMALLER_THAN;
	else
	  return -1;
}
*/
/*
char Unit2Constant(char*str)
{	RemoveCharFromString(' ',str);
	String2Upper(str);	//change string to uppercase before comparision.
	if(*str == NULL)
  		return -1;
	else if(strlen(str)==0)
	  	return -1;
	else if(strcmp(str,UNIT_STR_NONE)==0)
	  	return UNIT_NONE;
	else if(strcmp(str,UNIT_STR_MTR)==0)
	  	return UNIT_MTR;
	else if(strcmp(str,UNIT_STR_KM)==0)
	  	return UNIT_KM;
	else if(strcmp(str,UNIT_STR_MILE)==0)
	  	return UNIT_MILE;
	else if(strcmp(str,UNIT_STR_KMPH)==0)
	  	return UNIT_KMPH;
	else if(strcmp(str,UNIT_STR_MPH)==0)
	  	return UNIT_MPH;
	else if(strcmp(str,UNIT_STR_SEC)==0)
	  	return UNIT_SEC;
	else if(strcmp(str,UNIT_STR_MIN)==0)
	  	return UNIT_MIN;
	else if(strcmp(str,UNIT_STR_HR)==0)
	  	return UNIT_HR;
	else if(strcmp(str,UNIT_STR_DAYS)==0)
	  	return UNIT_DAYS;
	else if(strcmp(str,UNIT_STR_MV)==0)
	  	return UNIT_MV;
	else if(strcmp(str,UNIT_STR_VOLT)==0)
	  	return UNIT_VOLT;
	else if(strcmp(str,UNIT_STR_DEG)==0)
	  	return UNIT_DEG;
	else if(strcmp(str,UNIT_STR_DEG_C)==0)
	  	return UNIT_DEG_C;
	else if(strcmp(str,UNIT_STR_DEG_F)==0)
	  	return UNIT_DEG_F;
	else if(strcmp(str,UNIT_STR_PERCENTAGE)==0)
	  	return UNIT_PERCENTAGE;
	else
	  return -1;
}
*/
/*
ui MyPid2WorldPid(uc mypid)
{	switch(mypid)
	{	case MY_PID_TIME: 		return WORLD_PID_TIME;
		case MY_PID_TEMPERATURE:return WORLD_PID_TEMPERATURE;
		case MY_PID_EXT_BATTERY:return WORLD_PID_EXT_BATTERY;
		case MY_PID_INT_BATTERY:return WORLD_PID_INT_BATTERY;
		case MY_PID_AI1:		return WORLD_PID_AI1;
		case MY_PID_DI1:		return WORLD_PID_DI1;
		case MY_PID_DI2:		return WORLD_PID_DI2;
		case MY_PID_DI3:		return WORLD_PID_DI3;
		case MY_PID_DI4:		return WORLD_PID_DI4;
		
		case MY_PID_INVALID_UPLOAD_PTR: return WORLD_PID_INVALID_UPLOAD_PTR;
		case MY_PID_RESTART:	return WORLD_PID_RESTART;
		
		default: return -1;//in case my pid doesnot exist and world pid exists OR MY_PID & WORLD_PID are same
	}
}
*/
/*
ui WorldPid2MyPid(ui worldpid)
{	switch(worldpid)
	{	case WORLD_PID_TIME: 		return MY_PID_TIME;
		case WORLD_PID_TEMPERATURE:	return MY_PID_TEMPERATURE;
		case WORLD_PID_EXT_BATTERY:	return MY_PID_EXT_BATTERY;
		case WORLD_PID_INT_BATTERY:	return MY_PID_INT_BATTERY;
		case WORLD_PID_AI1:			return MY_PID_AI1;
		case WORLD_PID_DI1:			return MY_PID_DI1;
		case WORLD_PID_DI2:			return MY_PID_DI2;
		case WORLD_PID_DI3:			return MY_PID_DI3;
		case WORLD_PID_DI4:			return MY_PID_DI4;
		
		case WORLD_PID_RESTART:	return MY_PID_RESTART;
		
		default: return -1;//in case world pid doesnot exist and my pid exists
	}
}*/
ui AlarmFIFO(ui val_in)
{   char lp;
    ui val_out = AlarmFIFOBuff[0];
    for(lp=0;lp<ALARM_BUFFER_MAX_LIMIT-1;lp++)
    {   AlarmFIFOBuff[lp] = AlarmFIFOBuff[lp+1];
    }
    AlarmFIFOBuff[ALARM_BUFFER_MAX_LIMIT-1] = val_in;
    return val_out;
}
void SoftwareResetProcessor()
{  	PeriodicSave();
	//SendGPRScommand("AT+QPOWD = 0");
	GPRS_UART->SCR &= ~0x10;//DISABLE SERIAL receive interrupt in order to prevent controller being hang due to serial Rx pin gets low and controller gets stuck in serial interrupt
	DelayInMilliSec(2000);
    //DelayInMicroSec(2000000);
    NVIC_SystemReset();

}
void JustifyLeftForLcdLine(char* source, char* destination)
{   uc len;
    for(len=0;len<LCD_CHAR_PER_LINE_WITH_NULL-1 && *source != '\0';len++, source++, destination++)
    {   *destination = *source;   
    }
    for(;len<LCD_CHAR_PER_LINE_WITH_NULL-1; len++, destination++)
    {   *destination = ' ';
    }
}
void JustifyCenterForLcdLine(char* source, char* destination)
{	uc len;
	uc diff;
	uc i;
	
	len = strlen(source);
	if(LCD_CHAR_PER_LINE_WITH_NULL-1 > len)
	{	diff = LCD_CHAR_PER_LINE_WITH_NULL-1 - len;
		diff /=2;
		for(i=0;i<diff;i++)
		{	destination[i] = ' ';
		}
		destination[i] = '\0';
		strcat(destination,source);
		len = strlen(destination);
		for(;len<LCD_CHAR_PER_LINE_WITH_NULL-1;len++)
			strcat(destination," ");
	}
	else
	{	source[LCD_CHAR_PER_LINE_WITH_NULL-1] = '\0';
		strcpy(destination,source);
	}
}
/*
void SwitchEndian4ByteStoredIn_u_i_0()
{	//switch MSB <--> LSB. First byte received is MSB
	u.c[0] += u.c[3];
	u.c[1] += u.c[2];
	u.c[2] = u.c[1]-u.c[2];
	u.c[3] = u.c[0]-u.c[3];
	u.c[1] = u.c[1]-u.c[2];
	u.c[0] = u.c[0]-u.c[3];
}*/