#ifndef __PC_COMMUNICATION_H__
#define __PC_COMMUNICATION_H__

#include "ProjectConstants.h"
#include "ProjectVariable.h"
#include "SerialCommunication.h"

extern I2C_HandleTypeDef hi2c2;

/*################################ FUNCTION LOCAL TO THE FILE ###############################################*/
void CheckCommand(void);
uc ExeCommand(void);
uc getch(void);					//function to get a key from PC keyboard
void SendByte(uc byte);		//function to send one byte to pc
void SendCharDataType(uc Char);
void SendIntDataType(ui Int);
void SendResponseID(void);	//function to send reply that command has been received
void SendPacket(void);			//function to send one packet of log record serially
void SendString(char* str);	//function to send string to pc
/*###########################################################################################################*/


#endif
