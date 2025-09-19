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
/** \file dac.c
 **
 ** A detailed description is available at 
 ** @link DacGroup Digital Analog Converter description @endlink
 **
 ** History:
 **   - 2012-04-20  1.0  MWi  First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "dac.h"

#if (defined(L3_PERIPHERAL_DAC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup DacGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

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
 ** \brief Set DAC0 10-bit value
 **
 ** \param [in]  pstcDac        Pointer to DAC instance
 ** \param [in]  u16DacValue    DAC0 value to be output
 **
 ** \retval Ok                  Value written.
 ******************************************************************************/
en_result_t Dac_SetValue0( stc_dacn_t* pstcDac,
                           uint16_t    u16DacValue
                         )
{
  pstcDac->DADR0.DA = u16DacValue;
  
  return Ok;
} // Dac_SetValue0


/**
 ******************************************************************************
 ** \brief Set DAC1 10-bit value
 **
 ** \param [in]  pstcDac        Pointer to DAC instance
 ** \param [in]  u16DacValue    DAC1 value to be output
 **
 ** \retval Ok                  Value written.
 ******************************************************************************/
en_result_t Dac_SetValue1( stc_dacn_t* pstcDac,
                           uint16_t    u16DacValue
                         )
{
  pstcDac->DADR1.DA = u16DacValue;
  
  return Ok;
} // Dac_SetValue1

/**
 ******************************************************************************
 ** \brief Enable DAC0
 **
 ** \param [in]  pstcDac        Pointer to DAC instance
 **
 ** \retval Ok                  DAC0 enabled
 ******************************************************************************/
en_result_t Dac_Enable0( stc_dacn_t* pstcDac )
{
  pstcDac->DACR0.DAE = 1;
  
  return Ok;
} // Dac_Enable0

/**
 ******************************************************************************
 ** \brief Enable DAC1
 **
 ** \param [in]  pstcDac        Pointer to DAC instance
 **
 ** \retval Ok                  DAC1 enabled
 ******************************************************************************/
en_result_t Dac_Enable1( stc_dacn_t* pstcDac )
{
  pstcDac->DACR1.DAE = 1;
  
  return Ok;
} // Dac_Enable1


/**
 ******************************************************************************
 ** \brief Disable DAC0
 **
 ** \param [in]  pstcDac        Pointer to DAC instance
 **
 ** \retval Ok                  DAC0 disabled
 ******************************************************************************/
en_result_t Dac_Disable0( stc_dacn_t* pstcDac )
{
  pstcDac->DACR0.DAE = 0;
  
  return Ok;
} // Dac_Disable0

/**
 ******************************************************************************
 ** \brief Disable DAC1
 **
 ** \param [in]  pstcDac        Pointer to DAC instance
 **
 ** \retval Ok                  DAC1 disabled
 ******************************************************************************/
en_result_t Dac_Disable1( stc_dacn_t* pstcDac )
{
  pstcDac->DACR1.DAE = 0;
  
  return Ok;
} // Dac_Disable1


//@} // DacGroup

#endif // #if (defined(L3_PERIPHERAL_DAC_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
