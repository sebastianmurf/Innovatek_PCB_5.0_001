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
/** \file dac.h
 **
 ** Headerfile for DAC functions
 **  
 ** History:
 **   - 2012-04-20  1.0  MWi  First version.
 **
 ******************************************************************************/

#ifndef __DAC_H__
#define __DAC_H__

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
 ** \defgroup DacGroup Digital Analog Converter (DAC)
 **
 ** Provided functions of DAC module:
 ** - Dac_SetValue0()
 ** - Dac_SetValue1()
 ** - Dac_Enable0()
 ** - Dac_Enable1()
 ** - Dac_Disable0()
 ** - Dac_Disable1()  
 **
 ** Becaise the DAC does not need any configuration, no configuration structure
 ** and no Init or De-Init functions are needed. The output can be easily
 ** enabled and disabled by Dac_Enable0(), Dac_Enable1(), Dac_Disable0(), and
 ** Dac_Disable1(). The DAC value can be set by Dac_SetValue0() and
 ** Dac_SetValue1().
 **
 ** The indexes 0 and 1 correspond to the channel 0 and 1 of a DAC instance.
 **
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
 
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
#define DAC0  ((volatile stc_dacn_t *) L3_DAC_BASE)
  
/**
 ******************************************************************************
 ** \brief Redefinition of DADR bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_dacreg0_field
{
  __IO  uint16_t DA         : 10;
  __IO  uint16_t RESERVED   : 6;
} stc_dacreg0_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DACR bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_dacreg1_field
{
  __IO  uint8_t DAE        : 1;
  __IO  uint8_t RESERVED   : 7;
} stc_dacreg1_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of LVD register structure for unification of
 **        all configurations
 ******************************************************************************/
typedef struct stc_dacn
{
  stc_dacreg0_field_t  DADR0;
  stc_dacreg1_field_t  DACR0;
  uint8_t              RESERVED;
  stc_dacreg0_field_t  DADR1;
  stc_dacreg1_field_t  DACR1;
} stc_dacn_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern en_result_t Dac_SetValue0( stc_dacn_t* pstcDac,
                                  uint16_t    u16DacValue
                                );

extern en_result_t Dac_SetValue1( stc_dacn_t* pstcDac,
                                  uint16_t    u16DacValue
                                );

extern en_result_t Dac_Enable0( stc_dacn_t* pstcDac );

extern en_result_t Dac_Enable1( stc_dacn_t* pstcDac );

extern en_result_t Dac_Disable0( stc_dacn_t* pstcDac );

extern en_result_t Dac_Disable1( stc_dacn_t* pstcDac );

//@} // DacGroup

#ifdef __cplusplus
}
#endif

#endif /* __DAC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

