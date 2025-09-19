/*******************************************************************************
FILE NAME			: 	MyUsb.h
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	4/8/2014
MOTHER VERSION NO	:	1.0
LAST MODIFIED DATE	:	4/8/2014
CURRENT VERSION NO	:	1.0
*******************************************************************************/
#ifndef __MYUSB_H__
#define __MYUSB_H__
/* other header file necessary to use this file */ 
#include "common.h"

#define USB_BUFF_SIZE 500

enum usb_stage
{	USB_NOT_INITIALIZED,
	USB_SWITCH_TO_DEVICE,
	USB_MAIN_APP_AS_A_DEVICE,
	USB_SWITCH_TO_HOST
};

//extern char UsbBuff[USB_BUFF_SIZE];
extern char_t UsbReceiveBuffer[512];
extern ui UsbCrcCodeOut;
extern boolean_t UsbCommandFlag;
extern boolean_t WaitForConnectionFlag;//IF DURING POWER ON USB IS DETECTED
extern ui UsbReceiveCounter;

extern void CheckUsbStage();
extern void PutStringUsb(const char *pu8Buffer);
extern void PutCharUsb(uint8_t u8Char);

#endif