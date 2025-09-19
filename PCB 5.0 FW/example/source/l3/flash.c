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
/** \file flash.c
 **
 ** A detailed description is available at 
 ** @link FlashGroup Flash Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-05  1.1  MWi  Work Flash functions added
 **   - 2012-08-29  1.2  MWi  Static DMA halt and resume functions added, NMI
 **                           RAM vector option added
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "flash.h"
#include "ramcode.h"

#if (defined(L3_PERIPHERAL_FLASH_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup FlashGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

// Originally defined in ramcode.c and as 'extern' in ramcode.h. Not recognized
// by IAR linker (V6.20)!
#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  #ifdef __ICCARM__
    #pragma location=(L3_RAM_VECTOR_TABLE_ADDRESS + 8)
    __no_init uint32_t u32L3NmiVectorAddress;
  #endif
#endif // #if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
    
/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

#if (L3_FLASH_DMA_HALT == L3_ON)

/**
 ******************************************************************************
 ** \brief DMA and IRQ resume
 ******************************************************************************/
static void Flash_DmaIrqResume(void)
{
  uint32_t u32DmacrReadout;
  
  u32DmacrReadout = FLASH_DMACR;
  FLASH_DMACR = u32DmacrReadout & 0xF0FFFFFF; // Set DH-Bits to '0'
  
  __enable_irq();                             // CMSIS core function
}

/**
 ******************************************************************************
 ** \brief DMA and IRQ halt routine
 **
 ** \retval Ok                        DMA successfully halted (or not enabled
 **                                   before)
 ** \retval ErrorTimeout              DMA halt failed within time-out
 ******************************************************************************/
static en_result_t Flash_DmaIrqHalt(void)
{
  uint32_t u32DmacrReadout;
  uint32_t u32TimeOut;
  
  __disable_irq();                  // CMSIS core function
  
  u32DmacrReadout = FLASH_DMACR;
  
  if (u32DmacrReadout & 0x80000000) // DMA enabled (DE-Bit set)?
  {
    FLASH_DMACR = u32DmacrReadout | 0x0F000000;  // Set DH to 'F'
     
    u32TimeOut = FLASH_DMA_HALT_TIMEOUT;
     
    while ((0x40000000 != (FLASH_DMACR & 0x40000000)) && (u32TimeOut > 0))  // DS-Bit
    {
      u32TimeOut--; 
    }
    
    if (0 == u32TimeOut)
    {
      Flash_DmaIrqResume();  // Try to resume DMA ...
      return ErrorTimeout;
    }  
  }

  return Ok;
}

#endif // #if (L3_FLASH_DMA_HALT == L3_ON)

#if (L3_WORK_FLASH == L3_ON)
/**
 ******************************************************************************
 ** \brief Check if u32FlashAddress is within Work Flash memory area
 **
 ** \param [in]  u32FlashAddress   Work Flash address to be programmed
 **
 ** \retval TRUE                   Address within Work Flash memory area
 ** \retval FALSE                  Address out of Work Flash memory area
 ******************************************************************************/
static boolean_t Flash_IsWorkFlash(uint32_t u32FlashAddress)
{
  if ((u32FlashAddress >= WORK_FLASH_START_ADDRESS) &&
      (u32FlashAddress <= WORK_FLASH_END_ADDRESS)
     )
  {
    return TRUE;
  }
  
  return FALSE;
}
#endif // #if (L3_WORK_FLASH == L3_ON)

#if (L3_WORK_FLASH == L3_ON)
/**
 ******************************************************************************
 ** \brief Work Flash Sector Erase ROM function
 **
 ** \param [in]  u32FlashAddress   Work Flash address to be programmed
 **
 ** \retval Ok                     Work Flash programming successful
 ** \retval ErrorTimeout           Work Flash programming time out error
 ** \retval ErrorInvalidParameter  u32FlashAddress out of Work Flash area
 ******************************************************************************/
static en_result_t Flash_WorkFlashErase( uint32_t u32FlashAddress )
{
  volatile int32_t  i32FlashFlag = 0;
  volatile uint32_t u32DummyRead;
  
  FM3_WORKFLASH_IF->WFASZR = 0;            // ASZ = 0
  u32DummyRead = FM3_WORKFLASH_IF->WFASZR; // dummy read of FASZR
  
  *(FLASH_WORK_SEQ_0AA8) = FLASH_SECTOR_ERASE_1;
  *(FLASH_WORK_SEQ_0554) = FLASH_SECTOR_ERASE_2;
  *(FLASH_WORK_SEQ_0AA8) = FLASH_SECTOR_ERASE_3;
  *(FLASH_WORK_SEQ_0AA8) = FLASH_SECTOR_ERASE_4;
  *(FLASH_WORK_SEQ_0554) = FLASH_SECTOR_ERASE_5;
  *(volatile uint16_t*)u32FlashAddress = FLASH_SECTOR_ERASE_6;
    
  // sector erase timer ready?
  while ((*(volatile uint16_t *)u32FlashAddress & FLASH_DQ3) != FLASH_DQ3);  

  while(0 == i32FlashFlag)
  {
    // Flash timeout?
    if((*(volatile uint16_t *)u32FlashAddress & FLASH_DQ5) == FLASH_DQ5) 
    {
      i32FlashFlag = FLASH_TIMEOUT_ERROR;
    }

    // Data correct?
    if((*(volatile uint16_t *)u32FlashAddress & FLASH_DQ7) == FLASH_DQ7)
    {
      i32FlashFlag = FLASH_OK;
    }
    
    L3_WAIT_LOOP_HOOK();
  }
  
  FM3_WORKFLASH_IF->WFASZR = 1;            // ASZ = 1
  u32DummyRead = FM3_WORKFLASH_IF->WFASZR; // dummy read of FASZR
  
  if (i32FlashFlag == FLASH_TIMEOUT_ERROR)
  {
    return ErrorTimeout;
  }
  
  return Ok; 
} // Flash_WorkFlashErase

/**
 ******************************************************************************
 ** \brief Work Flash Flash programming ROM function
 **
 ** \param [in]  u32FlashAddress   Work Flash address to be programmed
 ** \param [in]  u16FlashData      Work Flash data to be programmed
 **
 ** \retval Ok                     Work Flash programming successful
 ** \retval ErrorTimeout           Work Flash programming time out error
 ** \retval ErrorInvalidParameter  u32FlashAddress out of Work Flash area
 ******************************************************************************/
static en_result_t Flash_WorkFlashWrite16( uint32_t u32FlashAddress,
                                           uint16_t u16FlashData
                                         )
{
  volatile int32_t  i32FlashFlag = 0;
  volatile uint32_t u32DummyRead;
  
  FM3_WORKFLASH_IF->WFASZR = 0;            // ASZ = 0
  u32DummyRead = FM3_WORKFLASH_IF->WFASZR; // dummy read of FASZR
  
  *(FLASH_WORK_SEQ_0AA8) = FLASH_WRITE_1;
  *(FLASH_WORK_SEQ_0554) = FLASH_WRITE_2;
  *(FLASH_WORK_SEQ_0AA8) = FLASH_WRITE_3;
  
  *(volatile uint16_t*)u32FlashAddress = u16FlashData;  

  while(0 == i32FlashFlag)
  {
    // Flash timeout?
    if((*(volatile uint16_t *)u32FlashAddress & FLASH_DQ5) == FLASH_DQ5) 
    {
      i32FlashFlag = FLASH_TIMEOUT_ERROR;
    }

    // Data correct?
    if((*(volatile uint16_t *)u32FlashAddress & FLASH_DQ7) == (u16FlashData & FLASH_DQ7))
    {
      i32FlashFlag = FLASH_OK;
    }
  }
  
  FM3_WORKFLASH_IF->WFASZR = 1;            // ASZ = 1
  u32DummyRead = FM3_WORKFLASH_IF->WFASZR; // dummy read of FASZR
  
  if (i32FlashFlag == FLASH_TIMEOUT_ERROR)
  {
    return ErrorTimeout;
  }  
  
  return Ok; 
} // Flash_WorkFlashWrite16

#endif    // (L3_WORK_FLASH == L3_ON)
/**
 ******************************************************************************
 ** \brief Check if a "0" on any bit position should be changed to a "1".
 **
 ** \note This Check is only applicable, if the used device does not support
 **       ECC Flash!
 **
 ** \param [in]  u32DataInFlash       Data value from flash location.
 ** \param [in]  u32DataToWrite       Data to write to flash location.
 ** \param [in]  u8ByteCount          Number of bytes to write to 
 **                                    u32DataToWrite (1, 2 or 4).
 ** \param [in]  u8FirstBytePosition  First byte in u32DataToWrite to compare 
 **                                    (0 to 3).
 **  
 ** \retval TRUE                      Flash write can be done. 
 ** \retval FALSE                     If any of following conditions are met:
 **          - Flash write can not be done due to 0 to 1 transition.
 **          - u8ByteCount is invalid (not either 1, 2 or 4)
 **          - u8FirstBytePosition is greater than 3
 ******************************************************************************/
static boolean_t FlashWriteCanBeDone( uint32_t u32DataInFlash, 
                                      uint32_t u32DataToWrite, 
                                      uint8_t  u8ByteCount, 
                                      uint8_t  u8FirstBytePosition
                                    )
{
  // Only 1, 2 or 4 byte access allowed
  switch( u8ByteCount )
  {
    case 1:
    case 2:
    case 4:
      break ;
    default:
      return FALSE ;
  } // u8ByteCount
  
  // Invalid index of first byte to write
  if ( u8FirstBytePosition > 3 )
  {
    return FALSE;
  }    

  // Write would exceed the 4th byte
  if ( u8FirstBytePosition + u8ByteCount > 4 )
  {
    return FALSE;
  }
  
  // Check for unaligned 16-bit access
  if ( u8ByteCount == 2 && 
       u8FirstBytePosition == 1
     )
  {
    return FALSE;
  }
  
   // XOR every bit in current flash value (turn 0 to 1 and 1 to 0)
  u32DataInFlash ^= 0xFFFFFFFF;

  // Check if masking is neccessary
  if ( u8ByteCount == 1 )       // Type uint8_t value
  {
    // Left shift mask by (u8FirstBytePositon * 8) bits, 
    // AND it with data to write
    u32DataToWrite &= (0x000000FF << (u8FirstBytePosition << 3)) ;
  }
  else if ( u8ByteCount == 2 )  // Type uint16_t value
  {
    // legal first byte position at this point is 0 and 2
    if ( u8FirstBytePosition == 0 )
    {
      // Mask out upper 16 bit
      u32DataToWrite &= 0x0000FFFF; 
    }
    else
    {
      // Mask out lower 16 bit
      u32DataToWrite &= 0xFFFF0000;   
    }
  }
  
  // AND operation of flash data and data to write (any 1 to zero transition 
  // can now be seen by a result different to 0)
  return ((u32DataInFlash & u32DataToWrite) == 0) ; 
} // FlashWriteCanBeDone

/**
 ******************************************************************************
 ** \brief Erase Flash Sector
 ** If u32FlashAddress is within the Work Flash memory area the corresponding
 ** ROM Flash routines are called (if L3_WORK_FLASH is enabled).
 **
 ** \param [in]  u32FlashAddress   Flash address to be programmed
 **
 ** \retval Ok                     Flash programming successful
 ** \retval ErrorTimeout           Flash programming time out error
 ** \retval ErrorInvalidParameter  If L3_NO_FLASH_RAMCODE == L3_ON and address
 **                                not within Work Flash
 ******************************************************************************/
en_result_t Flash_Erase( uint32_t u32FlashAddress )
{
#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  uint32_t u32VtorReadOut;
#endif  
	
#if (L3_WORK_FLASH == L3_ON)
  if (TRUE == Flash_IsWorkFlash(u32FlashAddress))
  {
    if (Ok != Flash_WorkFlashErase(u32FlashAddress)) // Wait for Ready always set
    {
      return ErrorTimeout;
    }
    
    return Ok;
  }
#endif  // (L3_WORK_FLASH == L3_ON)
  
#if (L3_NO_FLASH_RAMCODE == L3_ON)
  return ErrorInvalidParameter;
#else
 
#if (L3_FLASH_DMA_HALT == L3_ON)
  if (Ok != Flash_DmaIrqHalt())
  {
    return ErrorTimeout;
  }
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  u32VtorReadOut = RAMCODE_VTOR_ADDRESS;                    // Save VTOR
  u32L3NmiVectorAddress = (uint32_t)&NMI_Ramcode_Handler;   // Set RAM NMI Handler vector
  RAMCODE_VTOR_ADDRESS = L3_RAM_VECTOR_TABLE_ADDRESS;       // User RAM vector table
  #endif
#endif // #if (L3_FLASH_DMA_HALT == L3_ON)
  
  if (Ok != Ramcode_FlashErase(u32FlashAddress, TRUE)) // Wait for Ready always set
  {
#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
    RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
    Flash_DmaIrqResume();
#endif // #if (L3_FLASH_DMA_HALT == L3_ON)

    return ErrorTimeout;
  }

#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
  Flash_DmaIrqResume();
#endif // #if (L3_FLASH_DMA_HALT == L3_ON)
  
  return Ok;
#endif
} // Flash_Erase

/**
 ******************************************************************************
 ** \brief Program byte
 ** If u32FlashAddress is within the Work Flash memory area the corresponding
 ** ROM Flash routines are called (if L3_WORK_FLASH is enabled).
 **
 ** \param [in]  u32FlashAddress      Flash address to be programmed
 ** \param [in]  u8Data               Flash data to be programmed
 **
 ** \retval Ok                        Flash programming successful
 ** \retval ErrorTimeout              Flash programming time out error
 ** \retval ErrorOperationInProgress  Flash not ready to program
 ** \retval ErrorInvalidParameter     If any of following conditions are met:
 **                                   - Flash data cannot be programmed, because
 **                                     one or more bits cannot be programmed 
 **                                     from '0' to '1'.
 **                                   - If L3_NO_FLASH_RAMCODE == L3_ON and 
 **                                     address not within Work Flash
 ******************************************************************************/
en_result_t Flash_Write8( uint32_t  u32FlashAddress, 
                          uint8_t   u8Data
                        )
{
  uint16_t    u16DataInFlash;
  uint16_t    u16DataToFlash;
  en_result_t enErrorCode;
#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  uint32_t u32VtorReadOut;
#endif   
	
  u16DataInFlash = *(int16_t*)(0xFFFFFFFE & u32FlashAddress);
  
  if (u32FlashAddress & 1)  // odd address?
  {
    u16DataToFlash = (u16DataInFlash & 0x00FF) | (u8Data << 8);
  }
  else
  {
    u16DataToFlash = (u16DataInFlash & 0xFF00) | u8Data;
  }
  
  if (FALSE == FlashWriteCanBeDone(u16DataInFlash, u16DataToFlash, 1, 1))
  {
    return ErrorInvalidParameter;
  }
  
#if (L3_WORK_FLASH == L3_ON)
  if (TRUE == Flash_IsWorkFlash(u32FlashAddress))
  {
    return Flash_WorkFlashWrite16((0xFFFFFFFE & u32FlashAddress), u16DataToFlash);
  }
#endif
  
#if (L3_NO_FLASH_RAMCODE == L3_ON)
  return ErrorInvalidParameter;
#else
  
#if (L3_FLASH_DMA_HALT == L3_ON)
  if (Ok != Flash_DmaIrqHalt())
  {
    return ErrorTimeout;
  }
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  u32VtorReadOut = RAMCODE_VTOR_ADDRESS;                    // Save VTOR
  u32L3NmiVectorAddress = (uint32_t)&NMI_Ramcode_Handler;   // Set RAM NMI Handler vector
  RAMCODE_VTOR_ADDRESS = L3_RAM_VECTOR_TABLE_ADDRESS;       // User RAM vector table
  #endif
#endif // #if (L3_FLASH_DMA_HALT == L3_ON)
  
  enErrorCode = Ramcode_FlashWrite16((0xFFFFFFFE & u32FlashAddress), u16DataToFlash);
  
#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
  Flash_DmaIrqResume();
#endif // if #if (L3_FLASH_DMA_HALT == L3_ON)
  
  return enErrorCode;
#endif
} // Flash_Write16

/**
 ******************************************************************************
 ** \brief Program 16-bit word
 ** If u32FlashAddress is within the Work Flash memory area the corresponding
 ** ROM Flash routines are called (if L3_WORK_FLASH is enabled).
 **
 ** \param [in]  u32FlashAddress      Flash address to be programmed
 ** \param [in]  u16Data              Flash data to be programmed
 **
 ** \retval Ok                        Flash programming successful
 ** \retval ErrorAddressAlignment     u32FlashAddress not an even address
 ** \retval ErrorTimeout              Flash programming time out error
 ** \retval ErrorOperationInProgress  Flash not ready to program
 ** \retval ErrorInvalidParameter     If any of following conditions are met:
 **                                   - Flash data cannot be programmed, because
 **                                     one or more bits cannot be programmed 
 **                                     from '0' to '1'.
 **                                   - If L3_NO_FLASH_RAMCODE == L3_ON and 
 **                                     address not within Work Flash
 ******************************************************************************/
en_result_t Flash_Write16( uint32_t u32FlashAddress, 
                           uint16_t u16Data
                         )
{
  uint16_t    u16DataInFlash;
  en_result_t enErrorCode;
#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  uint32_t u32VtorReadOut;
#endif  
  
  // Check for even address
  if (1 == (u32FlashAddress & 1))
  {
    return ErrorAddressAlignment;
  }
  
  u16DataInFlash = *(int16_t*)u32FlashAddress;
  
  if (FALSE == FlashWriteCanBeDone(u16DataInFlash, u16Data, 2, 0))
  {
    return ErrorInvalidParameter;
  }

#if (L3_WORK_FLASH == L3_ON)
  if (TRUE == Flash_IsWorkFlash(u32FlashAddress))
  {
    return Flash_WorkFlashWrite16(u32FlashAddress, u16Data);
  }
#endif
 
#if (L3_NO_FLASH_RAMCODE == L3_ON)
  return ErrorInvalidParameter;
#else

#if (L3_FLASH_DMA_HALT == L3_ON)
  if (Ok != Flash_DmaIrqHalt())
  {
    return ErrorTimeout;
  }
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  u32VtorReadOut = RAMCODE_VTOR_ADDRESS;                    // Save VTOR
  u32L3NmiVectorAddress = (uint32_t)&NMI_Ramcode_Handler;   // Set RAM NMI Handler vector
  RAMCODE_VTOR_ADDRESS = L3_RAM_VECTOR_TABLE_ADDRESS;       // User RAM vector table
  #endif
#endif // #if (L3_FLASH_DMA_HALT == L3_ON)
  
  enErrorCode = Ramcode_FlashWrite16(u32FlashAddress, u16Data);
  
#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
  Flash_DmaIrqResume();
#endif // if #if (L3_FLASH_DMA_HALT == L3_ON)
  
  return enErrorCode;
#endif
} // Flash_Write16

/**
 ******************************************************************************
 ** \brief Writes byte data to Flash in block mode.
 **
 ** The return code ErrorInvalidParameter is returned when an input parameter
 ** is incorrect or if one data value can not be written because of an invalid
 ** 0 bit to 1 bit transition. (if bit is currently set to 0, it cannot be 
 ** changed to 1, there has to be an erase command previously!).
 ** If u32FlashAddress is within the Work Flash memory area the corresponding
 ** ROM Flash routines are called (if L3_WORK_FLASH is enabled).
 ** 
 **
 ** \param [in] u32FlashAddress    Offset to write the data to
 ** \param [in] pu8Data            Pointer to the data in a buffer to be 
 **                                  written to Flash.
 ** \param [in] u32DataLength      Number of bytes in pu8Data to be written to
 **                                  Flash must be at least 1.
 ** 
 ** \retval Ok                      Block write successfully done.
 ** \retval ErrorTimeout            Flash programming time-out error
 ** \retval ErrorInvalidParameter   If one of following conditions is met:
 **                                 - pu8Data is NULL
 **                                 - u32DataLength is 0
 **                                 - Flash can not be written because of
 **                                   invalid 0 to 1 transition.   
 ******************************************************************************/
en_result_t Flash_WriteBlock8( uint32_t  u32FlashAddress,
                               uint8_t*  pu8Data,
                               uint32_t  u32DataLength
                             )
{
  uint32_t    u32DataPointer = 0;
  uint32_t    u32FlashAddressToProgram;
  uint16_t    u16DataToFlash;
  en_result_t enErrorCode;
  
  // Parameter checks
  if (( pu8Data       == NULL ) ||
      ( u32DataLength == 0    ))
  {
    return ErrorInvalidParameter;
  }

  u32FlashAddressToProgram = u32FlashAddress;
  
  // Start at odd address?
  if ((u32FlashAddressToProgram & 1) == 1)
  {
    enErrorCode = Flash_Write8(u32FlashAddressToProgram, *pu8Data);
  
    if (enErrorCode != Ok)
    {
      return enErrorCode; 
    }
    
    *pu8Data++;
    u32DataPointer++;
    u32FlashAddressToProgram++;
  }

  // Just one byte?
  if (u32DataPointer >= u32DataLength)
  {
    return Ok; 
  }
  
  while (u32DataPointer < (u32DataLength - 1))
  {
    u16DataToFlash = *pu8Data;          // Lower byte (little endian)
    *pu8Data++;
    u16DataToFlash |= (*pu8Data << 8);  // Upper byte (little endian)
    *pu8Data++;

    enErrorCode = Flash_Write16(u32FlashAddressToProgram, u16DataToFlash);
  
    if (enErrorCode != Ok)
    {
      return enErrorCode; 
    }    
    
    u32DataPointer += 2;
    u32FlashAddressToProgram += 2;
  }
  
  // Last byte left? If yes, must be even address ...
  if (u32DataPointer < u32DataLength)
  {
    enErrorCode = Flash_Write8(u32FlashAddressToProgram, *pu8Data);

    if (enErrorCode != Ok)
    {
      return enErrorCode; 
    }      
  }

  return Ok;
} // Flash_WriteBlock8

/**
 ******************************************************************************
 ** \brief Writes aligned 16-bit data to Flash in block mode.
 **
 ** The return code ErrorInvalidParameter is returned when an input parameter
 ** is incorrect or if one data value can not be written because of an invalid
 ** 0 bit to 1 bit transition. (if bit is currently set to 0, it cannot be 
 ** changed to 1, there has to be an erase command previously!).
 ** If u32FlashAddress is within the Work Flash memory area the corresponding
 ** ROM Flash routines are called (if L3_WORK_FLASH is enabled).
 **
 ** \param [in] u32FlashAddress    Offset to write the data to
 ** \param [in] pu16Data           Pointer to the data in a buffer to be 
 **                                  written to Flash.
 ** \param [in] u32DataLength      Number of 16-bit words (<b>not</b> bytes!)
 **                                  in pu16Data to be written to
 **                                  Flash must be at least 1.
 ** 
 ** \retval Ok                      Block write successfully done.
 ** \retval ErrorTimeout            Flash programming time-out error
 ** \retval ErrorInvalidParameter   If one of following conditions is met:
 **                                 - pu8Data is NULL
 **                                 - u32DataLength is 0
 **                                 - Flash can not be written because of
 **                                   invalid 0 to 1 transition.  
 ** \retval ErrorAddressAlignment   u32FlashAddress not an even address
 ******************************************************************************/
en_result_t Flash_WriteBlock16( uint32_t  u32FlashAddress,
                                uint16_t* pu16Data,
                                uint32_t  u32DataLength
                              )
{
  uint32_t u32DataPointer = 0;
  uint32_t u32FlashAddressToProgram;
  en_result_t enErrorCode;
  
  // Parameter checks
  if (( pu16Data      == NULL ) || 
      ( u32DataLength == 0    ))
  {
    return ErrorInvalidParameter;
  }

  if (1 == (u32FlashAddress & 1))
  {
    return ErrorAddressAlignment;
  }
  
  u32FlashAddressToProgram = u32FlashAddress;
  
  while (u32DataPointer < u32DataLength) 
  {
    enErrorCode = Flash_Write16(u32FlashAddressToProgram, *pu16Data);
  
    if (enErrorCode != Ok)
    {
      return enErrorCode; 
    }    
    
    *pu16Data++;
    u32FlashAddressToProgram += 2;
    u32DataPointer++;
  }
  
  return Ok;
}

#if (L3_ECC_FLASH == L3_ON)

/**
 ******************************************************************************
 ** \brief Writes aligned 32-bit data to ECC Flash in block mode.
 **
 ** The return code ErrorInvalidParameter is returned when an input parameter
 ** is incorrect or if one data value can not be written because of an not
 ** empty ECC 32-bit cell. ECC does not support Flash cell update without
 ** ECC error!
 **
 ** \note This function is not available for Work Flash area.
 ** 
 ** \param [in] u32FlashAddress    Offset to write the data to
 ** \param [in] pu32Data           Pointer to the data in a buffer to be 
 **                                  written to Flash.
 ** \param [in] u32DataLength      Number of 32-bit words (<b>not</b> bytes!)
 **                                  in pu32Data to be written to
 **                                  Flash must be at least 1.
 ** 
 ** \retval Ok                      Block write successfully done.
 ** \retval ErrorTimeout            Flash programming time-out error
 ** \retval ErrorInvalidParameter   If one of following conditions is met:
 **                                 - pu8Data is NULL
 **                                 - u32DataLength is 0
 **                                 - Flash can not be written because of
 **                                   non-empty 32-bit ECC Flash cell
 ** \retval ErrorAddressAlignment   u32FlashAddress not an 32-bit aligned
 **                                 address
 ******************************************************************************/
en_result_t Flash_WriteEccBlock( uint32_t  u32FlashAddress,
                                 uint32_t* pu32Data,
                                 uint32_t  u32DataLength
                                )
{
  uint32_t u32DataPointer = 0;
  uint32_t u32FlashAddressToProgram;
  uint32_t u32FlashData;
  en_result_t enErrorCode;
  
  // Parameter checks
  if (( pu32Data      == NULL ) || 
      ( u32DataLength == 0    ))
  {
    return ErrorInvalidParameter;
  }

  if (0 != (u32FlashAddress % 4))
  {
    return ErrorAddressAlignment;
  }  
  
  u32FlashAddressToProgram = u32FlashAddress;
  
  while (u32DataPointer < u32DataLength) 
  {
    if (0xFFFFFFFF != *(uint32_t*)u32FlashAddress)
    {
      return enErrorCode;   // ECC not empty!
    }
    
    u32FlashData = *pu32Data;

#if (L3_FLASH_DMA_HALT == L3_ON)
  if (Ok != Flash_DmaIrqHalt())
  {
    return ErrorTimeout;
  }
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  u32VtorReadOut = RAMCODE_VTOR_ADDRESS;                    // Save VTOR
  u32L3NmiVectorAddress = (uint32_t)&NMI_Ramcode_Handler;   // Set RAM NMI Handler vector
  RAMCODE_VTOR_ADDRESS = L3_RAM_VECTOR_TABLE_ADDRESS;       // User RAM vector table
  #endif
#endif // #if (L3_FLASH_DMA_HALT == L3_ON)
  
    enErrorCode = Ramcode_FlashWrite16(u32FlashAddressToProgram, (uint16_t)u32FlashData);
    
    if (enErrorCode != Ok)
    {
      
#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
  Flash_DmaIrqResume();
#endif // if #if (L3_FLASH_DMA_HALT == L3_ON)
      
      return enErrorCode; 
    }    
    
    u32FlashData >>= 16;
    
    enErrorCode = Ramcode_FlashWrite16(u32FlashAddressToProgram + 2, (uint16_t)u32FlashData);
    
    if (enErrorCode != Ok)
    {
      
#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
  Flash_DmaIrqResume();
#endif // if #if (L3_FLASH_DMA_HALT == L3_ON)
      
      return enErrorCode; 
    }   
    
    *pu32Data++;
    u32FlashAddressToProgram += 4;
    u32DataPointer++;
  }
  
#if (L3_FLASH_DMA_HALT == L3_ON)
  #if (L3_NMI_RAM_VECTOR_HANDLE == L3_ON)
  RAMCODE_VTOR_ADDRESS = u32VtorReadOut;  // Restore VTOR
  #endif
  Flash_DmaIrqResume();
#endif // if #if (L3_FLASH_DMA_HALT == L3_ON)
  
  return Ok;
} // Flash_WriteEccBlock

#endif // #if (L3_ECC_FLASH == L3_ON)

//@} // FlashGroup

#endif // #if (defined(L3_PERIPHERAL_FLASH_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
