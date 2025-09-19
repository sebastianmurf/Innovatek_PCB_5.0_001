#include "my_stm32f030_internal_flash.h"
#include "main.h"
#include "ProjectConstants.h"

#ifdef WATCH_DOG
	#if(PROJECT_MODE == RELEASED)
		extern WWDG_HandleTypeDef hwwdg;
	#endif
#endif

uint8_t InternalFlashPageSizeBuff[INTERNAL_FLASH_PAGE_SIZE];

/*########################################################################################*/
/* 	This function write one page of data to internal flash memory
	page_address	= initial address of the page
	data_ptr 		= pointer to one page of data
*/
void InternalFlashPageWrite(uint32_t page_address, uint8_t *data_ptr)
{	
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Allow Access to option bytes sector */
	HAL_FLASH_OB_Unlock();

	/* Fill EraseInit structure*/
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = page_address;
	EraseInitStruct.NbPages = 1;
	uint32_t PageError;

	volatile uint32_t index=0;

	volatile HAL_StatusTypeDef status;
	status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	while(index < 256)// 256*4 = 1024
	{
		#ifdef WATCH_DOG
			#if(PROJECT_MODE==RELEASED)
				HAL_WWDG_Refresh(&hwwdg);
			#endif
		#endif
		if (status == HAL_OK)
		{	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, page_address + index * 4, *(uint64_t*)data_ptr);//4 BYTE AT A TIME
			if(status == HAL_OK)
			{	index++;
				data_ptr +=4;
			}
		}
	}
	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();
}
/*########################################################################################*/
/*	This function write any length of data that is less than the page size to internal flash memory
	address		= physical memory address where data is to be written
	dataptr		= pointer pointing to the data
	no_of_byte	= number of byte that is to be updated/written in the memory
*/
void WriteToFlash(uint32_t address, uint8_t *dataptr, uint16_t no_of_byte)
{	uint16_t byte_count = 0;
	uint32_t page_add;
	
	page_add = address/INTERNAL_FLASH_PAGE_SIZE;
	page_add *= INTERNAL_FLASH_PAGE_SIZE;
	
	ReadInternalFlash(page_add, (uint8_t*)InternalFlashPageSizeBuff, INTERNAL_FLASH_PAGE_SIZE);
	
	while(byte_count < no_of_byte)
	{	
		#ifdef WATCH_DOG
			#if(PROJECT_MODE==RELEASED)
				HAL_WWDG_Refresh(&hwwdg);
			#endif
		#endif
		if(((address % INTERNAL_FLASH_PAGE_SIZE) + byte_count) == INTERNAL_FLASH_PAGE_SIZE)
			break;
		InternalFlashPageSizeBuff[(address % INTERNAL_FLASH_PAGE_SIZE) + byte_count] = *dataptr;
		byte_count++;
		dataptr++;
	}
	//write the buffer to flash memory
	InternalFlashPageWrite(INTERNAL_FLASH_PAGE_SIZE*(address/INTERNAL_FLASH_PAGE_SIZE), InternalFlashPageSizeBuff);
	if(byte_count < no_of_byte)//more byte to be written on next page
	{	ReadInternalFlash(page_add + INTERNAL_FLASH_PAGE_SIZE, (uint8_t*)InternalFlashPageSizeBuff, INTERNAL_FLASH_PAGE_SIZE);//Read next page
		while(byte_count < no_of_byte)
		{	
			#ifdef WATCH_DOG
				#if(PROJECT_MODE==RELEASED)
					HAL_WWDG_Refresh(&hwwdg);
				#endif
			#endif
			InternalFlashPageSizeBuff[(address + byte_count) % INTERNAL_FLASH_PAGE_SIZE] = *dataptr;
			byte_count++;
			dataptr++;
		}
		//write the buffer to flash memory next page
		InternalFlashPageWrite(page_add + INTERNAL_FLASH_PAGE_SIZE, InternalFlashPageSizeBuff);
	}
}
/*########################################################################################*/
/*	This function read any number of data that is <= page size from the internal flash memory
	address 	= physical address of the memory
	dataptr		= pointer pointing to the data
	no_of_byte	= number of byte to be read from the internal flash memory
*/
void ReadInternalFlash(uint32_t address, uint8_t* dataptr, uint16_t no_of_byte)
{	volatile uint16_t read_cnt=0;
	while(read_cnt < no_of_byte)
	{	
		#ifdef WATCH_DOG
			#if(PROJECT_MODE==RELEASED)
				HAL_WWDG_Refresh(&hwwdg);
			#endif
		#endif
		*dataptr = *(uint8_t*)(address + read_cnt);
		dataptr++;
		read_cnt++;
	}
}
/*########################################################################################*/
