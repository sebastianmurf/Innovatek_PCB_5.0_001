/******************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)                */
/*                                                                           */
/* The following software deliverable is intended for and must only be       */
/* used for reference and in an evaluation laboratory environment.           */
/* It is provided on an as-is basis without charge and is subject to         */
/* alterations.                                                              */
/* It is the user’s obligation to fully test the software in its             */
/* environment and to ensure proper functionality, qualification and         */
/* compliance with component specifications.                                 */
/*                                                                           */
/* In the event the software deliverable includes the use of open            */
/* source components, the provisions of the governing open source            */
/* license agreement shall apply with respect to such software               */
/* deliverable.                                                              */
/* FSEU does not warrant that the deliverables do not infringe any           */
/* third party intellectual property right (IPR). In the event that          */
/* the deliverables infringe a third party IPR it is the sole                */
/* responsibility of the customer to obtain necessary licenses to            */
/* continue the usage of the deliverable.                                    */
/*                                                                           */
/* To the maximum extent permitted by applicable law FSEU disclaims all      */
/* warranties, whether express or implied, in particular, but not            */
/* limited to, warranties of merchantability and fitness for a               */
/* particular purpose for which the deliverable is not designated.           */
/*                                                                           */
/* To the maximum extent permitted by applicable law, FSEU’s liability       */
/* is restricted to intentional misconduct and gross negligence.             */
/* FSEU is not liable for consequential damages.                             */
/*                                                                           */
/* (V1.5)                                                                    */
/******************************************************************************/
/** \file ramcode.h
 **
 ** Headerfile for RAMCODE routines
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-10  1.1  Mwi  Flash defintions put to flash.h
 **
 ******************************************************************************/

#ifndef __RAMCODE_H__
#define __RAMCODE_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "l3.h"
#include "mcu.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup RamCodeGroup RAM Code Routines (RAMCODE)
 **
 ** This module contains all needed RAM routines. Recently the RAM routines
 ** for sinlge-bank Flash programming are implemented.
 **
 ** Provided functions of RAMCODE module:
 ** 
 ** Flash routines:
 ** - Ramcode_FlashErase()
 ** - Ramcode_FlashWrite16()
 ** - NMI_Ramcode_Handler()
 **
 ** NMI_Ramcode_Handler() is used by flash.c module and if
 ** L3_NMI_RAM_VECTOR_HANDLE == L3_ON and L3_FLASH_DMA_HALT == L3_ON
 **
 ** \note Ignore possible linker warning(s), if this ramcode section is being
 **       optimized away or activate #L3_PERIPHERAL_ENABLE_FLASH and use at
 **       least one Flash routine in your application! The Flash routines are 
 **       never compiled, if #L3_NO_FLASH_RAMCODE == #L3_ON.
 ******************************************************************************/
//@{

/******************************************************************************
 * Global definitions
 ******************************************************************************/
#define RAMCODE_VTOR_ADDRESS (*(( volatile uint32_t*)0xE000ED08))
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  #ifdef __ICCARM__
    #pragma location=(L3_RAM_VECTOR_TABLE_ADDRESS + 8)
    extern __no_init uint32_t u32L3NmiVectorAddress;  // This external declaration does not work in flash.c by IAR linker somehow!
  #elif __CC_ARM
    extern uint32_t u32L3NmiVectorAddress;
  #else
    #error Please check compiler and linker settings for variable address placement
  #endif
#endif // #if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

// Flash RAM routines only avaliable if L3_PERIPHERAL_FLASH_ACTIVE and
//   L3_NO_FLASH_RAMCODE deselected
#if ((defined(L3_PERIPHERAL_FLASH_ACTIVE)) && (L3_NO_FLASH_RAMCODE == L3_OFF))
  extern en_result_t Ramcode_FlashErase( uint32_t  u32SectorEraseAddress,
                                         boolean_t bWaitUntilFinished);
  
  extern en_result_t Ramcode_FlashWrite16( uint32_t u32ProgramAddress,
                                           uint16_t u16ProgamData );
#endif

#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  extern void NMI_Ramcode_Handler(void);
#endif

//@} // RamcodeGroup

#ifdef __cplusplus
}
#endif

#endif /* __RAMCODE_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
