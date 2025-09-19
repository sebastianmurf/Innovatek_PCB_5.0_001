/*******************************************************************************
FILE NAME			: 	my_stm32f030_internal_flash.h
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	17/05/2020
MOTHER VERSION NO	:	1.0
LAST MODIFIED DATE	:	17/05/2020
CURRENT VERSION NO	:	1.0
*******************************************************************************/
#ifndef __MY_STM32F030_INTERNAL_FLASH_H__
#define __MY_STM32F030_INTERNAL_FLASH_H__

#include "stm32f0xx_hal.h"

#define INTERNAL_FLASH_PAGE_SIZE 1024 //1KB

extern uint8_t InternalFlashPageSizeBuff[INTERNAL_FLASH_PAGE_SIZE];

/*########################################################################################*/
/*	This function read any number of data that is <= page size from the internal flash memory
	address 	= physical address of the memory
	dataptr		= pointer pointing to the data
	no_of_byte	= number of byte to be read from the internal flash memory
*/
extern void ReadInternalFlash(uint32_t address, uint8_t* dataptr, uint16_t no_of_byte);

/*########################################################################################*/
/*	This function write any length of data (0 to that is <= the page size) to internal flash memory with any offset address
	address		= physical memory address where data is to be written
	dataptr		= pointer pointing to the data
	no_of_byte	= number of byte that is to be updated/written in the memory
*/
extern void WriteToFlash(uint32_t address, uint8_t *dataptr, uint16_t no_of_byte);

/*########################################################################################*/
/* 	This function write one page of data to internal flash memory
	page_address	= initial address of the page
	data_ptr 		= pointer to one page of data
*/
extern void InternalFlashPageWrite(uint32_t page_address, uint8_t *data_ptr);

#endif
