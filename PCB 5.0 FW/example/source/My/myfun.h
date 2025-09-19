#ifndef __MYFUN_H__
#define __MYFUN_H__

#include"common.h"

//extern long long A2D(const char*str);//function to convert array into double
//extern int A2I(char*str);	//function to convert array into integer
extern ui AlarmFIFO(ui val_in);
extern unsigned char AsciiHex2Hex(char* ascii);	//converts 2 byte character ascii(hex format) to actual hex 
extern unsigned int AsciiHex2ValInt(char* asciibuff);
extern boolean_t ValidateDate(uc date, uc month, uc year);
//extern void Gmt2LocalDateTime(ui date, ui time);
//extern void D2A(long long d,char*buff);   //function to convert double into array
extern void F2A(float f,char*buff,char digit_after_decimal);   //function to convert float into array
//extern void I2A(unsigned int i,char*buff);   //function to convert integer into array
//extern void L2A(unsigned long l,char*buff);   //function to convert integer into array
extern void MyStrCpy(char*dptr,char*sptr,ui no_of_char);//function to string copy with max no of char
extern void RemoveCharFromString(char ch,const char *str);
extern void RemoveCharFromEndOfString(char ch, char *str);
extern void String2Upper(char* s);
extern int SearchString(const char*stringstartptr,const char*searchtextstartptr);
extern long long StrMobileNo2LL(const char *mobno);//format : +919910464351
extern ui WorldPid2MyPid(ui worldpid);
extern char Stability2Constant(char*str);
extern char* Constant2StabilityStr(char stability);
extern ui MyPid2WorldPid(uc mypid);
extern void SoftwareResetProcessor();
extern void JustifyCenterForLcdLine(char* souce,char* destination);
extern void JustifyLeftForLcdLine(char* source, char* destination);
extern void FindAndReplaceCharInString(char*string, char find, char replace);

#endif