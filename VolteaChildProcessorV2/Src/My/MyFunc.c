#include "MyFunc.h"
#include <string.h>
#include <stdlib.h>
#include "PC_Communication.h"
#include <time.h>
#include "MyDelay.h"
#include "MyTimer.h"
#include "InternalFlashMMU.h"
#include "main.h"
#include "ProjectVariable.h"
//extern WWDG_HandleTypeDef hwwdg;

/*###########################################################################################################*/
unsigned int AsciiHex2ValInt(char* asciibuff)//converts any length ASCII HEX to interger Value
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
/*###########################################################################################################*/
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
/*###########################################################################################################*/
void ByteToAsciiHex(uc int_val,char *buff)//this function is somewhat opp to Bcd2Hex fun^, where BCD doesn't have >9 but here AsciiHex may have >9 i.e A to F values
{	*buff = int_val>>4;
	if(*buff < 10)
		*buff += 48;
	else
		*buff += 'A'-10;
	
	buff++;
	
	*buff = int_val & 0x0F;
	if(*buff < 10)
		*buff += 48;
	else
		*buff += 'A'-10;
	
	buff++;
	*buff = '\0';
}
/*###########################################################################################################*/
uc Bcd2Hex(uc bcd)
{	return (((bcd>>4)*10) + (bcd&0x0F));
}
/*###########################################################################################################*/
/*
char CheckAuthority(char sender_type,char* password)
{	if(sender_type == SENDER_SERVER)
		CommParameter.AccessRight = ACCESS_RIGHT_SUPERADMIN;
	else if(strcmp(password,SUPER_PASSWORD)==0)
		CommParameter.AccessRight = ACCESS_RIGHT_SUPERADMIN;
	else if(strcmp(password,Device.AdminPassword)==0)
		CommParameter.AccessRight = ACCESS_RIGHT_ADMIN;
	else if(strcmp(password,Device.UserPassword)==0)
		CommParameter.AccessRight = ACCESS_RIGHT_USER;
	else 
		CommParameter.AccessRight = ACCESS_RIGHT_NONE;
	return 0;
}*/
/*###########################################################################################################*/
/*###########################################################################################################*/
/* clearing buffer is required for accepting any value through key pad*/
void ClearCopyBuff(void)
{	for(ucTemp=0;ucTemp<20;ucTemp++)
		CopyBuff[ucTemp]=0;
}
/*###########################################################################################################*/
/*
int DateDiff(void)
{
	uc greater;
	int datediff=0;
	u.c[0] = Bcd2Hex(Device.WarrantyDate);
	u.c[1] = Bcd2Hex(Device.WarrantyMonth);
	u.c[2] = Bcd2Hex(Device.WarrantyYear);
	u.c[3] = Bcd2Hex(TimeDate[DATE_LOC]);
	u.c[4] = Bcd2Hex(TimeDate[MONTH_LOC]);
	u.c[5] = Bcd2Hex(TimeDate[YEAR_LOC]);
	
	if(u.c[5] > u.c[2])
		greater = 2;
	else if((u.c[5] == u.c[2]) && (u.c[4] > u.c[1]))
		greater = 2;
	else if((u.c[5] == u.c[2]) && (u.c[4] == u.c[1]) && (u.c[3] > u.c[0]))
		greater = 2;
	else
		greater = 1;
	
	while(u.c[2]!=u.c[5] || u.c[1]!=u.c[4] || u.c[0]!=u.c[3])
	{	datediff++;
		if(greater==1){
			u.c[3]++;
			switch(u.c[4]){
				case 2: if(u.c[3] > 28)
								{	u.c[3]=1;
									u.c[4]++;
									if(u.c[4] > 12)
									{	u.c[4] = 1;
										u.c[5]++;
									}
								}
								break;
				case 4:
				case 6:
				case 9:
				case 11: 	if(u.c[3] > 30)
									{	u.c[3]=1;
										u.c[4]++;
										if(u.c[4] > 12)
										{	u.c[4] = 1;
											u.c[5]++;
										}
									}
									break;
				default: 	if(u.c[3] > 31)
									{	u.c[3]=1;
										u.c[4]++;
										if(u.c[4]>12)
										{	u.c[4] = 1;
											u.c[5]++;
										}
									}
									break;
			}
		}
		else{
		u.c[0]++;
		switch(u.c[4]){
			case 2: if(u.c[0] > 28)
							{	u.c[0]=1;
								u.c[1]++;
								if(u.c[1] > 12)
								{	u.c[1] = 1;
									u.c[2]++;
								}
							}
							break;
			case 4:
			case 6:
			case 9:
			case 11: 	if(u.c[0] > 30)
								{	u.c[0]=1;
									u.c[1]++;
									if(u.c[1] > 12)
									{	u.c[1] = 1;
										u.c[2]++;
									}
								}
								break;
			default: 	if(u.c[0] > 31)
								{	u.c[0]=1;
									u.c[1]++;
									if(u.c[1]>12)
									{	u.c[1] = 1;
										u.c[2]++;
									}
								}
								break;
			}
		}
	}
	if(greater == 2)
	 datediff *= -1;
	return datediff;
}
*/
/*###########################################################################################################*/
uc Hex2Bcd(uc hex)
{	return (((hex/10)<<4) + (hex%10));
}
/*###########################################################################################################*/
/*void HoldMsg(uc second)//THIS FUNCTION SHOULD BE MOVED TO HMI OR LCD FILE
{	Flag.MsgHold = TRUE;
	MsgHoldSecond = TimeDate[SEC_LOC]>>4;
	MsgHoldSecond *= 10;
	MsgHoldSecond += TimeDate[SEC_LOC] & 0x0f;
	MsgHoldSecond += second;
	MsgHoldSecond %= 60;
	MsgHoldSecond = Hex2Bcd(MsgHoldSecond);
}*/
/*###########################################################################################################*/
void MyStrCpy(char*dptr,char*sptr,ui no_of_char)
{	int i;
  	for(i=0;*sptr!='\0' && i< no_of_char;dptr++,sptr++,i++)
	{	*dptr = *sptr;
	}
	*dptr = '\0';
}
/*###########################################################################################################*/
/*
void PurchaseCheck(void)
{	EEPROM_Rd(PURCHASE_DATE_L,(char*)u.c,PURCHASE_DATE_N);//READING Purchase DATE
	Device.PurchaseDate = u.c[0];
	Device.PurchaseMonth = u.c[1];
	Device.PurchaseYear = u.c[2];
	EEPROM_Rd(WARRANTY_DATE_L,(char*)u.c,WARRANTY_DATE_N);//READING Purchase DATE
	Device.WarrantyDate = u.c[0];
	Device.WarrantyMonth = u.c[1];
	Device.WarrantyYear = u.c[2];
	EEPROM_Rd(MANUFACTURING_DATE_L,(char*)u.c,MANUFACTURING_DATE_N);//READING Purchase DATE
	Device.ManufacturingDate = u.c[0];
	Device.ManufacturingMonth = u.c[1];
	Device.ManufacturingYear = u.c[2];
	
	EEPROM_Rd(SN_L,(char*)u.c,SN_N);
	Device.SerialNo = u.l[0];
	if(u.l[0] == 0)
	{	//randomly initialize SERIAL NO
		LcdCommand(0x01);
		KeyFlag.keyMENU = FALSE;
		OutTextXY(0,1," Please Press M Key ");
		OutTextXY(0,2,"    for a while     ");
		while(ScanKeyMENU() == FALSE)
			uliTemp++;
		while(ScanKeyMENU() == TRUE)
			uliTemp++;
		srand(uliTemp);
		u.l[0] = (rand()%1000000);
		
		EEPROM_Wr(SN_L,(char*)u.c,SN_N);
		CopyBuff[0] = u.c[0];
		CopyBuff[1] = u.c[1];
		CopyBuff[2] = u.c[2];
		CopyBuff[3] = u.c[3];
		Device.SerialNo = u.l[0];
	}
	Device.UserPassword[0]=(u.c[0]>>4)+48;			//count of that day higher byte
	Device.UserPassword[1]=(u.c[0] & 0x0f)+48;		//count of that day lower byte
	Device.UserPassword[2]=(u.c[1]>>4)+48;			//date 10's location value
	Device.UserPassword[3]=(u.c[1] & 0x0f)+48;		//date 1's location value
	Device.UserPassword[4]=(u.c[2]>>4)+48;			//month 10's location value
	Device.UserPassword[5]=(u.c[2] & 0x0f)+48;		//month 1's location value
	Device.UserPassword[6]=(u.c[3]>>4)+48;			//year 10's location value
	Device.UserPassword[7]=(u.c[3] & 0x0f)+48;		//year 1's location value
	Device.UserPassword[8] = '\0';
	
	if(Device.ManufacturingDate == 0)
	{	EEPROM_Rd(SN_L,(char*)u.c,SN_N);
		OutTextXY(0,1," GET PRODUCTION CODE");
		OutTextXY(0,2,"For Device: ");
		OutTextXY(0,3,"                    ");
		OutTextXY(0,4,WEBSITE);
		
		OutCharXY(12,2,(u.l[0]/10000000)+48);			//count of that day higher byte
		OutCharXY(13,2,(u.l[0]/1000000)%10+48);		//count of that day lower byte
		OutCharXY(14,2,(u.l[0]/100000)%10+48);			//date 10's location value
		OutCharXY(15,2,(u.l[0]/10000)%10+48);		//date 1's location value
		OutCharXY(16,2,(u.l[0]/1000)%10+48);			//month 10's location value
		OutCharXY(17,2,(u.l[0]/100)%10+48);		//month 1's location value
		OutCharXY(18,2,(u.l[0]/10)%10+48);			//year 10's location value
		OutCharXY(19,2,u.l[0]%10+48);			//year 1's location value
		
		ClearCopyBuff();
		KeyPressCounter = 0;
		GotoXY(2,3);
		LcdCommand(0x0f);
		while(Device.ManufacturingDate==0)
		{	CheckCommand();
			if(ScanKey0() == TRUE)
			{	RecordKeyPress(0,NORMAL,16);
			}
			if(ScanKey1() == TRUE)
			{	RecordKeyPress(1,NORMAL,16);
			}
			if(ScanKey2() == TRUE)
			{	RecordKeyPress(2,NORMAL,16);
			}
			if(ScanKey3() == TRUE)
			{	RecordKeyPress(3,NORMAL,16);
			}
			if(ScanKey4() == TRUE)
			{	RecordKeyPress(4,NORMAL,16);
			}
			if(ScanKey5() == TRUE)
			{	RecordKeyPress(5,NORMAL,16);
			}
			if(ScanKey6() == TRUE)
			{	RecordKeyPress(6,NORMAL,16);
			}
			if(ScanKey7() == TRUE)
			{	RecordKeyPress(7,NORMAL,16);
			}
			if(ScanKey8() == TRUE)
			{	RecordKeyPress(8,NORMAL,16);
			}
			if(ScanKey9() == TRUE)
			{	RecordKeyPress(9,NORMAL,16);
			}
			if(ScanKeyBS() == TRUE)
			{	if(KeyPressCounter>0)
				{	KeyPressCounter--;
					OutCharXY(2+KeyPressCounter,3,' ');
					GotoXY(2+KeyPressCounter,3);
					CopyBuff[KeyPressCounter] = 0;
					LcdCommand(0x0f);
				}
			}	
			if(ScanKeyIN() == TRUE)
			{	for(ucTemp=0;ucTemp<16;ucTemp++)
					PC_RxBuff[5+ucTemp]=CopyBuff[ucTemp]+48;
				switch(SetCode(MANUFACTURING))
				{	case ILLEGAL_VALUE:	OutTextXY(0,3,"   ILLEGAL VALUE    ");LcdCommand(0x0f);break;
					case MEMORY_WRITE_ERROR: OutTextXY(0,3," MEMORY WRITE ERROR ");LcdCommand(0x0f);break;
					case DONE: OutTextXY(0,3,"    CODE ACCEPTED   ");break;
				}
				MyDelayMS(1000);
				OutTextXY(0,3,"                    ");
				ClearCopyBuff();
				KeyPressCounter = 0;
				GotoXY(2,3);
				
			}
			if(ScanKeyOUT() == TRUE)
			{	EEPROM_Rd(SN_L,CopyBuff,SN_N);
				OutTextXY(0,1," GET PURCHASE CODE  ");
				OutTextXY(0,2,"For Device: ");
				OutTextXY(0,3,"                    ");
				OutTextXY(0,4,WEBSITE);
			
				OutCharXY(12,2,(CopyBuff[0]>>4)+48);			//count of that day higher byte
				OutCharXY(13,2,(CopyBuff[0] & 0x0f)+48);		//count of that day lower byte
				OutCharXY(14,2,(CopyBuff[1]>>4)+48);			//date 10's location value
				OutCharXY(15,2,(CopyBuff[1] & 0x0f)+48);		//date 1's location value
				OutCharXY(16,2,(CopyBuff[2]>>4)+48);			//month 10's location value
				OutCharXY(17,2,(CopyBuff[2] & 0x0f)+48);		//month 1's location value
				OutCharXY(18,2,(CopyBuff[3]>>4)+48);			//year 10's location value
				OutCharXY(19,2,(CopyBuff[3] & 0x0f)+48);			//year 1's location value
				ClearCopyBuff();
				KeyPressCounter = 0;
				GotoXY(2,3);
				LcdCommand(0x0f);
			}
		}
	}
}*/
/*###########################################################################################################*/
/*###########################################################################################################*/
/*
#if(RENT_BASED == TRUE)
void LicenseCheck()
{	while(1)
	{	if((CurYear > LicYear) || (CurYear == LicYear && CurMonth > LicMonth) || (CurYear == LicYear && CurMonth == LicMonth && CurDate > LicDate))
		{	EEPROM_Rd(SN_L,CopyBuff,SN_N);
			bitTemp = FALSE;
			while(1)
			{ 	OutTextXY(0,1,"License For ");
				OutTextXY(0,2,"Expired On: ");
				OutTextXY(0,3,"Please Renew License");
				OutTextXY(0,4,"Contact your vendor.");
			
				OutCharXY(12,1,(CopyBuff[0]>>4)+48);			//date 10's location value
				OutCharXY(13,1,(CopyBuff[0] & 0x0f)+48);		//date 1's location value
				OutCharXY(14,1,(CopyBuff[1]>>4)+48);			//month 10's location value
				OutCharXY(15,1,(CopyBuff[1] & 0x0f)+48);		//month 1's location value
				OutCharXY(16,1,(CopyBuff[2]>>4)+48);			//year 10's location value
				OutCharXY(17,1,(CopyBuff[2] & 0x0f)+48);		//year 1's location value
				OutCharXY(18,1,(CopyBuff[3]/10+48));			//count of that day higher byte
				OutCharXY(19,1,(CopyBuff[3]%10+48));			//count of that day lower byte
										
				OutCharXY(12,2,(LicDate>>4)+48);
				OutCharXY(13,2,(LicDate&0x0f)+48);
				OutCharXY(14,2,'/');
				OutCharXY(15,2,(LicMonth>>4)+48);
				OutCharXY(16,2,(LicMonth&0x0f)+48);
				OutCharXY(17,2,'/');
				OutCharXY(18,2,(LicYear>>4)+48);
				OutCharXY(19,2,(LicYear&0x0f)+48);

				if(CommandReceiveFlag == TRUE)
				{	//checking device id
					u.i[0] = ((PC_RxBuff[0]-48) * 10) + (PC_RxBuff[1]-48);//the second condition is for sending command to all the slave connected
					if(u.i[0]==DeviceID || u.i[0]==177)
					{	if((PC_RxBuff[2]=='s' || PC_RxBuff[2]=='S') && (PC_RxBuff[3]=='l' || PC_RxBuff[3]=='L') && (PC_RxBuff[4]=='c' || PC_RxBuff[4]=='C'))
						{	CheckCommand();
							break;
						}
						else if((PC_RxBuff[2]=='s' || PC_RxBuff[2]=='S') && (PC_RxBuff[3]=='l' || PC_RxBuff[3]=='L') && (PC_RxBuff[4]=='d' || PC_RxBuff[4]=='D'))
						{	PC_RxBuff[3]='D';
							CheckCommand();
							bitTemp = TRUE;
							break;
						}
						else if((PC_RxBuff[2]=='s' || PC_RxBuff[2]=='S') && (PC_RxBuff[3]=='p' || PC_RxBuff[3]=='P') && (PC_RxBuff[4]=='c' || PC_RxBuff[4]=='C'))
						{	CheckCommand();
							break;
						}
						else
						{	SendResponseID();
							SendString("Please Give License Renew Command}");
						}
						CommandReceiveFlag = FALSE;
					}
				}
			}
			if(bitTemp == TRUE)
				break;	
		}
		else
			break;
	}
}
#endif
*/
/*###########################################################################################################*/
/*
void ReadyToAcceptNewNumber(void)
{	ClearCopyBuff();
	KeyPressCounter = 0;
	LcdCommand(0x99);
	LcdCommand(0x0f);
}
*/
/*###########################################################################################################*/
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
/*###########################################################################################################*/

/*###########################################################################################################*/
/*ui TimeInMin()
{	uiTemp = TimeDate[HOUR_LOC]>>4;
	uiTemp *= 10;
	uiTemp += TimeDate[HOUR_LOC] & 0X0F;
	uiTemp *= 60;
	uiTemp += (TimeDate[MIN_LOC]>>4)*10;
	uiTemp += (TimeDate[MIN_LOC]&0X0F);
	return uiTemp;//((TimeDate[HOUR_LOC]>>4)*10+(TimeDate[HOUR_LOC]&0X0F))*60+(TimeDate[MIN_LOC]>>4)*10+TimeDate[MIN_LOC]&0X0F;
}*/
/*###########################################################################################################*/
/*
#if(ERROR_INDUCTION==TRUE)
void CheckValidity()
{	if((CurYear > OkYear) || (CurYear == OkYear && CurMonth > OkMonth) || (CurYear == OkYear && CurMonth == OkMonth && CurDate > OkDate))
	{	ProductStatus = VALIDITY_OVER;
		EEPROM_Wr(PS_L,&ProductStatus,PS_N);
	}
}
#endif
*/
/*###########################################################################################################*/
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
				{	found = (char)-1;
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
/*###########################################################################################################*/
void String2Upper(char* s)
{	while(*s != '\0')
	{	if(*s >= 'a' && *s <= 'z')
	  		*s -= ('a'-'A');
		s++;
	}
}
/*###########################################################################################################*/

