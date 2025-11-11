#ifndef __MY_FUNC_H__
#define __MY_FUNC_H__

#include "ProjectConstants.h"
#include "ProjectVariable.h"

extern I2C_HandleTypeDef hi2c2;

extern unsigned int AsciiHex2ValInt(char* asciibuff);
extern unsigned char AsciiHex2Hex(char* ascii);	//converts 2 byte hex format ascii to actual hex 
extern uc Bcd2Hex(uc bcd);
extern void ByteToAsciiHex(uc int_val , char *buff);//this function is somewhat opp to Bcd2Hex fun^, where BCD doesn't have >9 but here AsciiHex may have >9 i.e A to F values
extern char CheckAuthority(char sender_type,char* password);
extern void CheckWarranty(void);
extern void ClearCopyBuff(void);
extern int DateDiff(void);
extern void Delay(ui t);
extern uc Hex2Bcd(uc hex);
extern void HoldMsg(uc second);
extern void MyStrCpy(char*dptr,char*sptr,ui no_of_char);
extern void OnError(ui ErrorNo);	//function to take step on error
extern void ProjectInit(void);			//function to initialize all the component of the device
extern void PurchaseCheck(void);
extern void ReadyToAcceptNewNumber(void);
extern void RemoveCharFromString(char ch,const char *str);
extern void String2Upper(char* s);
extern int SearchString(const char*stringstartptr,const char*searchtextstartptr);

#endif
