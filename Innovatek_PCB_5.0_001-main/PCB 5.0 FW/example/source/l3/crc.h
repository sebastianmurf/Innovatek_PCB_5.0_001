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
/** \file crc.h
 **
 ** Headerfile for CRC functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/

#ifndef __CRC_H__
#define __CRC_H__

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
 ** \defgroup CrcGroup Crc Functions (CRC)
 **
 ** Provided functions of CRC module:
 ** 
 ** - Crc_Init()
 ** - Crc_DeInit()
 ** - Crc_Push8()
 ** - Crc_Push16()
 ** - Crc_Push32()
 ** - Crc_ReadResult()
 **
 ** Initialize the CRC with Crc_Init(). After this there are to ways to
 ** calculate the check result:
 ** - Providing data by CPU via Crc_Push8(), Crc_Push16(), or Crc_Push32()
 ** - Using DMA
 **
 ** The result can be read by calling Crc_ReadResult().
 **
 ** Be careful with the endianess.
 ******************************************************************************/
//@{

/******************************************************************************
 * Global definitions
 ******************************************************************************/
#define stc_crcn_t    FM3_CRC_TypeDef
#define CRC0          (*((FM3_CRC_TypeDef *) FM3_CRC_BASE))

/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/**
 ******************************************************************************
 ** \brief Crc mode
 ** 
 ** To select between CRC16 and CRC32
 ******************************************************************************/
typedef enum en_crc_mode
{
    Crc16  = 0,  ///< CCITT CRC16 standard
    Crc32  = 1   ///< IEEE-802.3 CRC32 Ethernet standard
} en_crc_mode_t;  


/**
 *****************************************************************************
 ** \brief Crc configuration
 *****************************************************************************/
typedef struct stc_crc_config
{
  en_crc_mode_t    enMode;               ///< See description of #en_crc_mode_t
  boolean_t        bUseDma;              ///< TRUE: DMA usage, needs DMA driver
  boolean_t        bFinalXor;            ///< TRUE: CRC result as XOR value
  boolean_t        bResultLsbFirst;      ///< CRC result bit order: TRUE: LSB first, FALSE MSB first
  boolean_t        bResultLittleEndian;  ///< CRC result byte order: TRUE: Litte endian, FLASE: big endian
  boolean_t        bDataLsbFirst;        ///< CRC feed data bit order: TRUE: LSB first, FALSE MSB first
  boolean_t        bDataLittleEndian;    ///< CRC feed data byte order: TRUE: Litte endian, FLASE: big endian
  uint32_t         u32CrcInitValue;      ///< Initial value, upper 16 bit ignored using CRC16
  
} stc_crc_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled CRC instance
typedef enum en_crc_instance_index
{
    #if (L3_PERIPHERAL_ENABLE_CRC0 == L3_ON)
    CrcInstanceIndexCrc0,
    #endif
    CrcInstanceIndexMax
} en_crc_instance_index_t;


/// CRC instance data type
typedef struct stc_crc_instance_data
{
    volatile stc_crcn_t*  pstcInstance;   ///< pointer to registers of an instance
} stc_crc_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled CRC instances and their internal data
extern stc_crc_instance_data_t m_astcCrcInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

extern en_result_t Crc_Init( stc_crcn_t* pstcCrc, 
                      stc_crc_config_t* pstcConfig
                     ) ;

extern en_result_t Crc_DeInit( stc_crcn_t* pstcCrc ) ;

extern en_result_t Crc_Push8( stc_crcn_t* pstcCrc, 
                       uint8_t u8DataToFeed
                     ) ;

extern en_result_t Crc_Push16( stc_crcn_t* pstcCrc, 
                       uint16_t u16DataToFeed
                     ) ;

extern en_result_t Crc_Push32( stc_crcn_t* pstcCrc, 
                       uint32_t u32DataToFeed
                     ) ;

uint32_t Crc_ReadResult( stc_crcn_t*  pstcCrc ) ;


//@} // CrcGroup

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
