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
/** \file reset.h
 **
 ** Headerfile for RESET FACTOR functions
 **  
 ** History:
 **   - 2012-05-10  1.0  MWi  First version.
 **
 ******************************************************************************/

#ifndef __RESET_H__
#define __RESET_H__

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
 ** \defgroup ResetGroup Reset Factor (RESET)
 **
 ** Provided functions of RESET module:
 ** - Reset_GetCause() Reads the Reset Casue and clears the HW register
 **
 ** This driver does not need any configuration strucutre, but uses a result
 ** structure #stc_reset_result_t, which has to provided by the caller.
 **
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
 
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
#define RESET_CAUSE  ((volatile stc_reset_t *) (L3_CRG_BASE + 0xCUL))
  
/**
 ******************************************************************************
 ** \brief Redefinition of RST_STR Reset Status Register bit definitions
 ******************************************************************************/
typedef struct stc_reset_field
{
  __IO  uint16_t PONR       : 1;
  __IO  uint16_t INITX      : 1;
  __IO  uint16_t RESERVED1  : 1;
  __IO  uint16_t LVDH       : 1;  // only for Type3 and Type7
  __IO  uint16_t SWDT       : 1;
  __IO  uint16_t HWDT       : 1;
  __IO  uint16_t CSVR       : 1;
  __IO  uint16_t FCSR       : 1;
  __IO  uint16_t SRST       : 1;
  __IO  uint16_t RESERVED2  : 7;
} stc_reset_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of RST_STR Reset Status Register
 ******************************************************************************/
typedef struct stc_reset
{
  stc_reset_field_t  u8RST_STR;
} stc_reset_t;

/**
 ******************************************************************************
 ** \brief Datatype for Reset Cause return structure
 ******************************************************************************/
typedef struct stc_reset_result
{
  boolean_t    bPowerOn;              ///< TRUE: Power on reset ocurred
  boolean_t    bInitx;                ///< TRUE: INITX (external) reset ocurred
  boolean_t    bLowVoltageDetection;  ///< TRUE: Low Voltage Detection reset ocurred (only applicabel for Type3 and 7, always FALSE otherwise)
  boolean_t    bSoftwareWatchdog;     ///< TRUE: Software Watchdog reset ocurred
  boolean_t    bHardwareWatchdog;     ///< TRUE: Hardware Watchdog reset ocurred
  boolean_t    bClockSupervisor;      ///< TRUE: Clock Supervisor reset ocurred
  boolean_t    bAnomalousFrequency;   ///< TRUE: Anomalous Frequency reset ocurred
  boolean_t    bSoftware;             ///< TRUE: Software reset ocurred
} stc_reset_result_t ;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

extern en_result_t Reset_GetCause(stc_reset_result_t* pstcResult) ;

//@} // ResetGroup

#ifdef __cplusplus
}
#endif

#endif /* __RESET_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
