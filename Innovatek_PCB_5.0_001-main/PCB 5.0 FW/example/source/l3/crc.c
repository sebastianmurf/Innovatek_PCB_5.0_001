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
/** \file crc.c
 **
 ** A detailed description is available at 
 ** @link CrcGroup CRC Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "crc.h"

#if (defined(L3_PERIPHERAL_CRC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup CrcGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/// Macro to return the number of enabled CRC instances
#define CRC_INSTANCE_COUNT (uint32_t)(sizeof(m_astcCrcInstanceDataLut) / sizeof(m_astcCrcInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled CRC instances and their internal data
stc_crc_instance_data_t m_astcCrcInstanceDataLut[] =
{
  #if (L3_PERIPHERAL_ENABLE_CRC0 == L3_ON)
  &CRC0   // pstcInstance
  #endif
};

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
 ******************************************************************************
 ** \brief Initialisation of a CRC module.
 **
 ** \param [in]  pstcCrc       Pointer to CRC instance register area
 ** \param [in]  pstcConfig    CRC module configuration 
 **
 ** \retval Ok                     Initializiation of CRC module successfully 
 **                                done.
 ** \retval ErrorInvalidParameter  pstcCrc == NULL,
 **                                parameter out of range
 ******************************************************************************/
en_result_t Crc_Init( stc_crcn_t*       pstcCrc, 
                      stc_crc_config_t* pstcConfig
                    )
{
  stc_crc_crccr_field_t  unCRCCR ; // local preset of CRCCR
  
  // Check for valid pointers
  if ( (pstcCrc    == NULL) ||
       (pstcConfig == NULL)
     )
  {
      return ErrorInvalidParameter;
  }

  switch (pstcConfig->enMode)
  {
    case Crc16:
      unCRCCR.CRC32 = 0;
      pstcCrc->CRCINIT = 0x0000FFFF & (pstcConfig->u32CrcInitValue);
      break;
    case Crc32:
      unCRCCR.CRC32 = 1;
      pstcCrc->CRCINIT = pstcConfig->u32CrcInitValue;
      break;    
    default:
      return ErrorInvalidParameter;
  }
  
  unCRCCR.FXOR   = (pstcConfig->bFinalXor           == TRUE) ? 1 : 0 ;
  unCRCCR.CRCLSF = (pstcConfig->bResultLsbFirst     == TRUE) ? 1 : 0 ;
  unCRCCR.CRCLTE = (pstcConfig->bResultLittleEndian == TRUE) ? 1 : 0 ;
  unCRCCR.LSBFST = (pstcConfig->bDataLsbFirst       == TRUE) ? 1 : 0 ;
  unCRCCR.LTLEND = (pstcConfig->bDataLittleEndian   == TRUE) ? 1 : 0 ;
  unCRCCR.INIT = 1;
  
  pstcCrc->CRCINIT = pstcConfig->u32CrcInitValue ;
  pstcCrc->CRCCR_f = unCRCCR ;
    
  return Ok;
} // Crc_Init


/**
 ******************************************************************************
 ** \brief De-Initialisation of a CRC module.
 **
 ** \param [in]  pstcCrc       Pointer to CRC instance register area
 **
 ** \retval Ok                     De-Initializiation of CRC module successfully 
 **                                done.
 ** \retval ErrorInvalidParameter  pstcCrc == NULL
 ******************************************************************************/
en_result_t Crc_DeInit( stc_crcn_t* pstcCrc)
{
  // Check for valid pointers
  if (pstcCrc    == NULL)
  {
      return ErrorInvalidParameter;
  }

  // clear hardware
  pstcCrc->CRCIN = 0 ;
  pstcCrc->CRCCR = 0 ;

  return Ok;
}
    
/**
 ******************************************************************************
 ** \brief Push 8-bit integer data to a CRC module with if no DMA is used.
 **
 ** \param [in]  pstcCrc       Pointer to CRC instance register area
 ** \param [in]  u8DataToPush  8-Bit data to be pushed to CRC
 **
 ** \retval Ok                     Data pushed successfully
 ** \retval ErrorInvalidParameter  pstcCrc == NULL
 ******************************************************************************/
en_result_t Crc_Push8( stc_crcn_t* pstcCrc, 
                       uint8_t     u8DataToPush
                     )
{
  // Check for valid pointers
  if (pstcCrc == NULL)
  {
      return ErrorInvalidParameter;
  }

  pstcCrc->CRCINLL = u8DataToPush;

  return Ok;
} // Crc_Push8

/**
 ******************************************************************************
 ** \brief Push 16-bit integer data to a CRC module with if no DMA is used.
 **
 ** \note Be careful with the endianess. Byte swapping might have to be
 **       performed before pushing 16-bit data.
 **
 ** \param [in]  pstcCrc        Pointer to CRC instance register area
 ** \param [in]  u16DataToPush  16-Bit data to be pushed to CRC
 **
 ** \retval Ok                     Data pushed successfully
 ** \retval ErrorInvalidParameter  pstcCrc == NULL
 ******************************************************************************/
en_result_t Crc_Push16( stc_crcn_t* pstcCrc, 
                        uint16_t    u16DataToPush
                      )
{
  // Check for valid pointers
  if (pstcCrc == NULL)
  {
      return ErrorInvalidParameter;
  }

  pstcCrc->CRCINL = u16DataToPush;

  return Ok;
} // Crc_Push16

/**
 ******************************************************************************
 ** \brief Push 32-bit integer data to a CRC module with if no DMA is used.
 **
 ** \param [in]  pstcCrc        Pointer to CRC instance register area
 ** \param [in]  u32DataToPush  32-Bit data to be pushed to CRC
 **
 ** \retval Ok                     Data pushed successfully
 ** \retval ErrorInvalidParameter  pstcCrc == NULL
 ******************************************************************************/
en_result_t Crc_Push32( stc_crcn_t* pstcCrc, 
                        uint32_t    u32DataToPush
                      )
{
  // Check for valid pointers
  if (pstcCrc == NULL)
  {
      return ErrorInvalidParameter;
  }

  pstcCrc->CRCIN = u32DataToPush;

  return Ok;
} // Crc_Push32

/**
 ******************************************************************************
 ** \brief Read CRC result register
 **
 ** \note This function returns a 32-bit value regardless of a valid
 **       pointer to the CRC instance anyhow.
 **
 ** \param [in]  pstcCrc       Pointer to CRC instance register area
 **
 ******************************************************************************/
uint32_t Crc_ReadResult( stc_crcn_t* pstcCrc )
{
  return pstcCrc->CRCR;

} // Crc_ReadResult


//@} // CrcGroup

#endif // #if (defined(L3_PERIPHERAL_CRC_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
