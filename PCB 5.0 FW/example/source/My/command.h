#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "common.h"
#include<stdio.h>

enum access_right
{	ACCESS_RIGHT_SUPERADMIN,
	ACCESS_RIGHT_ADMIN,
	ACCESS_RIGHT_USER,
	ACCESS_DEVICE_OUTPUT_GET,
	ACCESS_DEVICE_GET,
	ACCESS_NONE,
	ACCESS_SYSTEM_DEBUG
};
enum command_sender
{	SENDER_PC 		=1,
	SENDER_USB		=2,
	SENDER_SERVER	=3,
};
#define MAX_COMMAND_PARAMETER 10
struct commandformat
{   //char Sender[15];//max 14 character including +, country code & number
    char AccessRight;
  	long long Sender;
  	char Pass[5];	//max 4 character
    char Cmd[31];	//max 20 character
    char Parameter[MAX_COMMAND_PARAMETER][50];//36//max 35 character
	char Para[500];
};
extern struct fota Fota;
extern struct commandformat CommandFormat;
extern void CheckCallResponse(uc callerloc);
extern void ArrangeCurrentStatus2PasteBuff();
extern char Command(char* commbuff);
extern char ExecuteUsbCommand(char* commbuff);

//extern char ExeCommand();
//extern void GetVehicleInfo();

extern void SendCommandReplyToSender(char* msg);
extern void SendTriggerResponseToUser(char pid, char*msg);
extern void FindUserToPanicCall(char pid);

extern char SetDeviceTimeZone(char* para);
//extern void SetInputBatVoltCategory(char parano);

extern void strcatTimeZone(char *strptr);
			
#endif
