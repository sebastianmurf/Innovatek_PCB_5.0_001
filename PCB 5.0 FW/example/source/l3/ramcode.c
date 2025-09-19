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
/** \file ramcode.c
 **
 ** A detailed description is available at 
 ** @link RamCodeGroup RAM Code Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-10  1.1  MWi  flash.h inclusion added, L3_NO_FLASH_RAMCODE
 **                           switch added
 **   - 2012-06-26  1.2  MWi  Device types 6 and 7 added
 **   - 2012-08-29  1.3  MWi  Static Flash Reset on exception for further usage
 **                           (NMI, Bus fault, etc.) added
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "ramcode.h"
#include "flash.h"

// Put all activator macros here which need RAM code:
#if ((defined(L3_PERIPHERAL_FLASH_ACTIVE)) && (L3_NO_FLASH_RAMCODE == L3_OFF))

/**
 ******************************************************************************
 ** \addtogroup RamCodeGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

// IAR compiler RAM code section pragma directive. See also linker file (*.icf)
#ifdef __ICCARM__
  #pragma section = ".ram_code"
#endif

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))
  #ifdef __ICCARM__
    #pragma location=(L3_RAM_VECTOR_TABLE_ADDRESS + 8)
    __no_init uint32_t u32L3NmiVectorAddress;
  #elif __CC_ARM
    uint32_t u32L3NmiVectorAddress __attribute__((at(L3_RAM_VECTOR_TABLE_ADDRESS + 8)));
  #else
    #error Please check compiler and linker settings for variable address placement
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

/**
 *****************************************************************************
 ** \brief Hook function, which is called from polling loops
 **
 ** This functionality should be the same as in l3.c!
 *****************************************************************************/
#ifdef __ICCARM__
  __ramfunc
#elif  __CC_ARM
  __attribute__ ((section (".ramfunc")))
#else
  #error Please check compiler and linker settings for RAM code
#endif
static void L3_RAMCODE_WAIT_LOOP_HOOK(void)
{
    // Place code for triggering Watchdog counters here, if needed
}

#if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))

/**
 *****************************************************************************
 ** \brief Flash Reset function for exception handling (NMI)
 **
 *****************************************************************************/
#ifdef __ICCARM__
  __ramfunc
#elif  __CC_ARM
  __attribute__ ((section (".ramfunc")))
#else
  #error Please check compiler and linker settings for RAM code
#endif
static void Ramcode_FlashReadReset(void)
{
  volatile uint32_t u32DummyRead;
  
  // Write Flash Read/Reset command to address 0
  *(volatile uint16_t*)0 = FLASH_RESET;
  
  FM3_FLASH_IF->FASZR = FLASH_FASZR_DEFAULT_VALUE;
  u32DummyRead = FM3_FLASH_IF->FASZR; // dummy read of FASZR
} // Ramcode_FlashReadReset

/**
 *****************************************************************************
 ** \brief NMI RAMCODE handler
 **
 *****************************************************************************/
#ifdef __ICCARM__
  __ramfunc
#elif  __CC_ARM
  __attribute__ ((section (".ramfunc")))
#else
  #error Please check compiler and linker settings for RAM code
#endif
void NMI_Ramcode_Handler(void)
{
  FM3_EXTI->NMICL = 0;  // Clear NMI
  
  Ramcode_FlashReadReset(); // Stop all possible started Flash commands
  
} // NMI_Ramcode_Handler

#endif // #if ((L3_NMI_RAM_VECTOR_HANDLE == L3_ON) && (L3_FLASH_DMA_HALT == L3_ON))

/**
 ******************************************************************************
 ** \brief Flash routine: Sector erase RAM function (Type 0, 1, 2 devices)
 **
 ** \param [in]  u32SectorEraseAddress   Sector address to be erased
 ** \param [in]  bWaitUntilFinished      TRUE: Wait for ready state,
 **                                      FALSE: Does not wait for ready state.
 **                                      May be used for Sector Erase suspend
 **                                      (Not implemented.)
 **
 ** \retval Ok                        Sector successfully erased
 ** \retval ErrorTimeout              Sector erase time out error
 ** \retval ErrorOperationInProgress  Flash not ready for sector erase
 ******************************************************************************/
#ifdef __ICCARM__
  __ramfunc
#elif  __CC_ARM
  __attribute__ ((section (".ramfunc")))
#else
  #error Please check compiler and linker settings for RAM code
#endif
en_result_t Ramcode_FlashErase( uint32_t  u32SectorEraseAddress,
                                boolean_t bWaitUntilFinished )
{
  volatile int32_t  i32FlashFlag = 0;
  volatile uint32_t u32DummyRead;
  volatile uint32_t u32TimeOut = FLASH_SECTOR_ERASE_READY_TIMEOUT;
  volatile uint16_t u16FaszrReadout;
  
  if(FM3_FLASH_IF->FSTR_f.RDY == 0)
  {
    return ErrorOperationInProgress; 
  }
  
  u16FaszrReadout = FM3_FLASH_IF->FASZR;  
  FM3_FLASH_IF->FASZR &= 0xFFFD;      // ASZ[1:0] = 2'b01
  FM3_FLASH_IF->FASZR |= 1;
  u32DummyRead = FM3_FLASH_IF->FASZR; // dummy read of FASZR
  
#if (L3_DEVICE_TYPE == L3_TYPE0) || \
    (L3_DEVICE_TYPE == L3_TYPE2) || \
    (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7) 
  *(FLASH_SEQ_1550) = FLASH_SECTOR_ERASE_1;
  *(FLASH_SEQ_0AA8) = FLASH_SECTOR_ERASE_2;
  *(FLASH_SEQ_1550) = FLASH_SECTOR_ERASE_3;
  *(FLASH_SEQ_1550) = FLASH_SECTOR_ERASE_4;
  *(FLASH_SEQ_0AA8) = FLASH_SECTOR_ERASE_5;
#elif (L3_DEVICE_TYPE == L3_TYPE1)
  *(FLASH_SEQ_0AA8) = FLASH_SECTOR_ERASE_1;
  *(FLASH_SEQ_0554) = FLASH_SECTOR_ERASE_2;
  *(FLASH_SEQ_0AA8) = FLASH_SECTOR_ERASE_3;
  *(FLASH_SEQ_0AA8) = FLASH_SECTOR_ERASE_4;
  *(FLASH_SEQ_0554) = FLASH_SECTOR_ERASE_5;
#else
  #error Device type not implemented
#endif
  
  *(volatile uint16_t*)u32SectorEraseAddress = FLASH_SECTOR_ERASE_6;
  
  if (bWaitUntilFinished == TRUE)
  {
    // sector erase timer ready?
    while (((*(volatile uint16_t *)u32SectorEraseAddress & FLASH_DQ3) != FLASH_DQ3) &&
           (u32TimeOut > 0))
    {
      L3_RAMCODE_WAIT_LOOP_HOOK();
      u32TimeOut--; 
    }
    
    // Time out occurred?
    if (0 == u32TimeOut)
    {
      // Read/Reset command
      *(volatile uint16_t*)u32SectorEraseAddress = FLASH_RESET;
 
      return ErrorTimeout;
    }
  
    // Data polling for sector erase finish
    while(0 == i32FlashFlag)
    {
      // Flash timeout?
      if((*(volatile uint16_t *)u32SectorEraseAddress & FLASH_DQ5) == FLASH_DQ5) 
      {
        i32FlashFlag = FLASH_TIMEOUT_ERROR;
      }
  
      // Data correct?
      if((*(volatile uint16_t *)u32SectorEraseAddress & FLASH_DQ7) == FLASH_DQ7)
      {
        i32FlashFlag = FLASH_OK;
      }
      
      L3_RAMCODE_WAIT_LOOP_HOOK();
    }
    
    FM3_FLASH_IF->FASZR = u16FaszrReadout;
    u32DummyRead = FM3_FLASH_IF->FASZR; // dummy read of FASZR
    
    if (i32FlashFlag == FLASH_TIMEOUT_ERROR)
    {
      return ErrorTimeout;
    }
  }
  
  return Ok;  
} // Ramcode_FlashErase

/**
 ******************************************************************************
 ** \brief Flash routine: Program 16-bit word RAM function (Type 0, 1 devices)
 **
 ** \param [in]  u32ProgramAddress       Flash address to be programmed
 ** \param [in]  u16ProgamData           Flash data to be programmed
 **
 ** \retval Ok                        Flash programming successful
 ** \retval ErrorTimeout              Flash programming time out error
 ** \retval ErrorOperationInProgress  Flash not ready to program
 ******************************************************************************/
#ifdef __ICCARM__
  __ramfunc
#elif  __CC_ARM
  __attribute__ ((section (".ramfunc")))
#else
  #error Please check compiler and linker settings for RAM code
#endif
en_result_t Ramcode_FlashWrite16( uint32_t u32ProgramAddress,
                                  uint16_t u16ProgamData )
{
  volatile int32_t  i32FlashFlag = 0;
  volatile uint32_t u32DummyRead;
  volatile uint8_t  u8DummyRead;
  volatile uint16_t u16FaszrReadout;
  
  if(FM3_FLASH_IF->FSTR_f.RDY == 0)
  {
    return ErrorOperationInProgress; 
  }
  
  u16FaszrReadout = FM3_FLASH_IF->FASZR;
  FM3_FLASH_IF->FASZR &= 0xFFFD;      // ASZ[1:0] = 2'b01
  FM3_FLASH_IF->FASZR |= 1;
  u32DummyRead = FM3_FLASH_IF->FASZR; // dummy read of FASZR
 
#if (L3_DEVICE_TYPE == L3_TYPE0) || \
    (L3_DEVICE_TYPE == L3_TYPE2) || \
    (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  *(FLASH_SEQ_1550) = FLASH_WRITE_1;
  *(FLASH_SEQ_0AA8) = FLASH_WRITE_2;
  *(FLASH_SEQ_1550) = FLASH_WRITE_3;
#elif (L3_DEVICE_TYPE == L3_TYPE1)  
  *(FLASH_SEQ_0AA8) = FLASH_WRITE_1;
  *(FLASH_SEQ_0554) = FLASH_WRITE_2;
  *(FLASH_SEQ_0AA8) = FLASH_WRITE_3;
#else
  #error Device type not implemented
#endif  
  *(volatile uint16_t*)u32ProgramAddress = u16ProgamData;  

  u8DummyRead = FM3_FLASH_IF->FSTR; // dummy read of FSTR for first read
    
  while(0 == i32FlashFlag)
  {
    // Ready?
    if(FM3_FLASH_IF->FSTR_f.RDY == 1) 
    {
      i32FlashFlag = FLASH_OK;
    }

    // Flash Hang?
    if(FM3_FLASH_IF->FSTR_f.HNG == 1) 
    {
      i32FlashFlag = FLASH_TIMEOUT_ERROR;
    }
    
    L3_RAMCODE_WAIT_LOOP_HOOK();
  }
  
  FM3_FLASH_IF->FASZR = u16FaszrReadout;
  u32DummyRead = FM3_FLASH_IF->FASZR; // dummy read of FASZR
  
  if (i32FlashFlag == FLASH_TIMEOUT_ERROR)
  {
    return ErrorTimeout;
  }
  
  return Ok; 
} // Ramcode_FlashWrite16
                                 

//@} // RamCodeGroup

#endif // #if ((defined(L3_PERIPHERAL_FLASH_ACTIVE)) && (L3_NO_FLASH_RAMCODE == L3_OFF))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
