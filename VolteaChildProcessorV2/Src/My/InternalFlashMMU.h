/********************************************************_***********************
FILE NAME			: 	InternalFlashMMU.h
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	18/05/2020
MOTHER VERSION NO	:	1.0
LAST MODIFIED DATE	:	18/05/2020
CURRENT VERSION NO	:	1.0
*******************************************************************************/
#ifndef __INTERNAL_FLASH_MMU_H__
#define __INTERNAL_FLASH_MMU_H__

#include<stm32f0xx_hal.h>
#include "my_stm32f030_internal_flash.h"

#define INTERNAL_FLASH_PAGE_SIZE 1024 //1KB

/*########################################################################################*/
//DEFINE	NAME			ADDRESS		NO OF BYTE
#define PAGE_31_START_A		0x08007C00
#define PAGE_31_END_A		0x08007FFF
#define	STATUS_REG_A		0x08007C00
#define STATUS_REG_N					1
#define K_VALUE_A			0x08007C01	
#define K_VALUE_N						2

/*
#define TIME_TABLE_START_A			0x0800FD00
#define TIME_TABLE_REC_SIZE			10
#define TIME_TABLE_MAX_SCHEDULE		76
#define TT_DAY_TIME_IN_SEC_P			0
	#define TT_DAY_TIME_IN_SEC_N				3
#define TT_OUTPUT_NOS_WITH_LOGIC_P		3
	#define TT_OUTPUT_NOS_WITH_LOGIC_N			1	//7LSB OUTPUT NO MSB=LOGIC (STRAIGHT OR REVERESE)
#define TT_ON_DUTY_IN_MILLI_SEC_P		4
	#define TT_ON_DUTY_IN_MILLI_SEC_N			3
#define TT_OFF_DUTY_IN_MILLI_SEC_P		7
	#define TT_OFF_DUTY_IN_MILLI_SEC_N			3

struct time_table
{	uint32_t TimeInSec;
	uint8_t  OutputAndLogic;
	uint32_t DutyCycleOn;
	uint32_t DutyCycleOff;
};
extern struct time_table TimeTable[TIME_TABLE_MAX_SCHEDULE],TimeTableTempRecord;
*/
#define NEIGHBOUR_TABLE_START_A	0x0800F800	//PAGE 62 START ADDRESS
#define NEIGHBOUR_TABLE_END_A	0x0800FBFF	//PAGE 62 END ADDRESS
#define NEIGHBOUR_TABLE_RECORD_SIZE		14
#define NEIGHBOUR_TABLE_MAX_RECORD		73
#define NEIGHBOUR_SERIAL_NO_L	0
#define NEIGHBOUR_SERIAL_NO_N			8
#define NEIGHBOUR_NTK_ID_L		8
#define NEIGHBOUR_NTK_ID_N				2
#define NEIGHBOUT_HW_ID_L		10
#define NEIGHBOUT_HW_ID_N				2
#define NEIGHBOUT_PC_L			12//PRODUCT CATAGORY
#define NEIGHBOUT_PC_N					1
#define NEIGHBOUT_PID_L			13//PRODUCT id
#define NEIGHBOUT_PID_N					1

extern void InternalFlashSettings(void);
extern void ResetFactorySettingForInternalFlash(void);

#endif
