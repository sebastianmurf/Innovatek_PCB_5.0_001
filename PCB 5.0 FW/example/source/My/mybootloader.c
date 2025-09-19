#include "mybootloader.h"

void BootloaderAPI_JumpBoot(uint32_t u32Address)
  {
     __asm("LDR SP, [R0]");		//Load new stack pointer address
     __asm("LDR PC, [R0, #4]");	        //Load new program counter address
  }
