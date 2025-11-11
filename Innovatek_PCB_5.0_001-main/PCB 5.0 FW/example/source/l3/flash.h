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
/** \file flash.h
 **
 ** Headerfile for FLASH routines
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-05  1.1  MWi  Work Flash added
 **   - 2012-08-29  1.2  MWi  Static DMA halt and resume functions added, NMI
 **                           RAM vector option added
 **
 ******************************************************************************/

#ifndef __FLASH_H__
#define __FLASH_H__

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
 ** \defgroup FlashGroup Flash Routines (FLASH)
 **
 ** Provided functions of FLASH module:
 ** 
 ** - Flash_Erase()
 ** - Flash_Write8()
 ** - Flash_Write16()
 ** - Flash_WriteBlock8()
 ** - Flash_WriteBlock16()
 ** - Flash_WriteEccBlock()
 **
 ** Flash_Erase() erases a sector with the given address.
 ** Flash_Write8() and Flash_Write16() program a single byte or 16-bit word
 ** to a given Flash address. Within the functions it is checked, if a Flash
 ** bit cell is tried to programm from '0' to '1', which is not possible.
 ** An error is returned in this case and the Flash address is not programmed.
 ** Flash_WriteBlock8() and Flash_WriteBlock16() program a block of data with
 ** a given length. The write functions from above are used.
 **
 ** \attention If the used device has an ECC Flash the only allowed function
 **            calls are:
 **            - Flash_Erase()
 **            - Flash_WriteEccBlock()
 **
 ** \note 
 ** - #L3_ECC_FLASH == #L3_ON in l3_user.h defines ECC Flash
 ** - #L3_WORK_FLASH == #L3_ON in l3_user.h defines Work Flash functionality. If
 **   activated the Flash functions check additionally for the Work Flash memory
 **   area and call the corresponding Work Flash ROM routines. If additionally
 **   #L3_NO_FLASH_RAMCODE is set to #L3_ON only Work Flash functionality is
 **   available.
 ** - if #L3_FLASH_DMA_HALT == #L3_on and a Flash routine returns
 **   #ErrorTimeout the application has to check, whether possible DMA transfers
 **   are properly resumed again.
 **
 ******************************************************************************/
//@{

/******************************************************************************
 * Global definitions
 ******************************************************************************/
  
// Definitions for Flash routines
  
// Type 0 Flash sequence addresses
#define FLASH_SEQ_1550 ((volatile uint16_t*) 0x00001550) // sequence address 1
#define FLASH_SEQ_0AA8 ((volatile uint16_t*) 0x00000AA8) // sequence address 2

// Type 1 additional Flash sequence address
#define FLASH_SEQ_0554 ((volatile uint16_t*) 0x00000554) // sequence address 1

// Work Flash sequence addresses
#define FLASH_WORK_SEQ_0AA8 ((volatile uint16_t*) 0x200C0AA8) 
#define FLASH_WORK_SEQ_0554 ((volatile uint16_t*) 0x200C0554)
  
#define FLASH_CHIP_ERASE_1   0x00AA   // chip erase commands
#define FLASH_CHIP_ERASE_2   0x0055
#define FLASH_CHIP_ERASE_3   0x0080
#define FLASH_CHIP_ERASE_4   0x00AA
#define FLASH_CHIP_ERASE_5   0x0055
#define FLASH_CHIP_ERASE_6   0x0010

#define FLASH_SECTOR_ERASE_1 0x00AA   // sector erase commands
#define FLASH_SECTOR_ERASE_2 0x0055
#define FLASH_SECTOR_ERASE_3 0x0080
#define FLASH_SECTOR_ERASE_4 0x00AA
#define FLASH_SECTOR_ERASE_5 0x0055
#define FLASH_SECTOR_ERASE_6 0x0030

#define FLASH_WRITE_1        0x00AA   // flash write commands
#define FLASH_WRITE_2        0x0055 
#define FLASH_WRITE_3        0x00A0
  
#define FLASH_RESET          0x00F0   // Flash Reset command

#define FLASH_SECTOR_ERASE_SUSPEND 0x00B0 // Sector erase suspend command
#define FLASH_SECTOR_ERASE_RESTART 0x0030 // Sector erase restart (resume) command

#define FLASH_ADDRESS_SA0  0x200C0000     // address of sector SA0 (Work Flash)
#define FLASH_ADDRESS_SA1  0x200C2000     // address of sector SA1 (Work Flash)
#define FLASH_ADDRESS_SA2  0x200C4000     // address of sector SA2 (Work Flash)
#define FLASH_ADDRESS_SA3  0x200C6000     // address of sector SA3 (Work Flash)
  
#define FLASH_ADDRESS_SA4  0x00000000     // address of sector SA4 (Main Flash)
#define FLASH_ADDRESS_SA5  0x00000004     // address of sector SA5 (Main Flash)
#define FLASH_ADDRESS_SA6  0x00004000     // address of sector SA6 (Main Flash)
#define FLASH_ADDRESS_SA7  0x00004004     // address of sector SA7 (Main Flash)
#define FLASH_ADDRESS_SA8  0x00008000     // address of sector SA8 (Main Flash)
#define FLASH_ADDRESS_SA9  0x00008004     // address of sector SA9 (Main Flash)
#define FLASH_ADDRESS_SA10 0x00020000     // address of sector SA10 (Main Flash)
#define FLASH_ADDRESS_SA11 0x00020004     // address of sector SA11 (Main Flash)
#define FLASH_ADDRESS_SA12 0x00040000     // address of sector SA12 (Main Flash)
#define FLASH_ADDRESS_SA13 0x00040004     // address of sector SA13 (Main Flash)
#define FLASH_ADDRESS_SA14 0x00060000     // address of sector SA14 (Main Flash)
#define FLASH_ADDRESS_SA15 0x00060004     // address of sector SA15 (Main Flash)

#define FLASH_DQ7 0x0080    // data polling flag bit (DPOL) position
#define FLASH_DQ6 0x0040    // data toggle flag bit (TOGG) position
#define FLASH_DQ5 0x0020    // time limit exceeding flag bit (TLOV) position
#define FLASH_DQ3 0x0008    // sector erase timer flag bit (SETI) position
  
#define FLASH_SECTOR_ERASE_READY_TIMEOUT 10000  // Time out counter for sector erase ready wait polling
#define FLASH_DMA_HALT_TIMEOUT           10000  // Time out counter for DMA halt
  
#define FLASH_TIMEOUT_ERROR   -1
#define FLASH_OK              1

#define WORK_FLASH_START_ADDRESS   FLASH_ADDRESS_SA0
#define WORK_FLASH_END_ADDRESS     (FLASH_ADDRESS_SA3 + 0x1FFF)
  
#define FLASH_DMACR *(uint32_t*)0x40060000
  
#define FLASH_FASZR_DEFAULT_VALUE  0x02
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

extern en_result_t Flash_Erase( uint32_t  u32FlashAddress );

extern en_result_t Flash_Write8( uint32_t  u32FlashAddress, 
                                 uint8_t   u8Data
                               );

extern en_result_t Flash_Write16( uint32_t u32FlashAddress, 
                                  uint16_t u16Data
                                );

extern en_result_t Flash_WriteBlock8( uint32_t u32FlashAddress,
                                      uint8_t* pu8Data,
                                      uint32_t u32DataLength
                                    );

extern en_result_t Flash_WriteBlock16( uint32_t  u32FlashAddress,
                                       uint16_t* pu16Data,
                                       uint32_t  u32DataLength
                                     );

#if (L3_ECC_FLASH == L3_ON)
extern en_result_t Flash_WriteEccBlock( uint32_t  u32FlashAddress,
                                        uint32_t* pu32Data,
                                        uint32_t  u32DataLength
                                      );
#endif // #if (L3_ECC_FLASH == L3_ON)

//@} // FlashGroup

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
