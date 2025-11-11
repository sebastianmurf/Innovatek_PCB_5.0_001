#include "InternalFlashMMU.h"
#include "ProjectVariable.h"
#include "main.h"
#include "MyDelay.h"

void ResetFactorySettingForInternalFlash()
{	InternalFlashPageSizeBuff[STATUS_REG_A % INTERNAL_FLASH_PAGE_SIZE] = 0x55;
	
	u.i[0] = 720;
	InternalFlashPageSizeBuff[K_VALUE_A % INTERNAL_FLASH_PAGE_SIZE] = u.c[0];//by default ADDRESS IS 1
	InternalFlashPageSizeBuff[(K_VALUE_A+1) % INTERNAL_FLASH_PAGE_SIZE] = u.c[1];//by default ADDRESS IS 1
	
	InternalFlashPageWrite(STATUS_REG_A, InternalFlashPageSizeBuff);
}
void InternalFlashSettings(void)
{	//Check if internal flash is initialized or not
	ReadInternalFlash(STATUS_REG_A, u.c, STATUS_REG_N);
	if(u.c[0] != 0x55)
	{	ResetFactorySettingForInternalFlash();
		
	}
	
	//load required flash setting to respective variables
	ReadInternalFlash(STATUS_REG_A, InternalFlashPageSizeBuff, INTERNAL_FLASH_PAGE_SIZE);//read from flash
	u.c[0] = InternalFlashPageSizeBuff[K_VALUE_A % INTERNAL_FLASH_PAGE_SIZE];
	u.c[1] = InternalFlashPageSizeBuff[(K_VALUE_A+1) % INTERNAL_FLASH_PAGE_SIZE];
	FlowKvalue = u.i[0]; 
	
}
