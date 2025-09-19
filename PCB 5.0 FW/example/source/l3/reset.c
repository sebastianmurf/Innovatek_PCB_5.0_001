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
/** \file reset.c
 **
 ** A detailed description is available at 
 ** @link ResetGroup Reset Cause description @endlink
 **
 ** History:
 **   - 2012-05-10  1.0  MWi  First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "reset.h"

#if (defined(L3_PERIPHERAL_RESET_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup ResetGroup
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
 ** \brief Get current Reset Cause Register contents
 **
 ** This function reads the Reset Cause Register and stores the cause bits
 ** in the result structure pointer.
 **
 ** \attention Calling this function clears all bits in the Reset Cause Register
 **            RST_STR! Reset_GetCause() should only be called after Start-up
 **            code!
 **
 ** \param [out]  pstcResult     Reset Cause result structure
 **
 ** \retval Ok                   Function done sucessfully
 **
 ******************************************************************************/
en_result_t Reset_GetCause(stc_reset_result_t* pstcResult)
{
  stc_reset_field_t u8ReadResetCause;
  
  u8ReadResetCause = RESET_CAUSE->u8RST_STR;

  // Check Power-on bit
  if (1 == u8ReadResetCause.PONR)
  {
    pstcResult->bPowerOn = TRUE;
  }
  else
  {
    pstcResult->bPowerOn = FALSE;
  }
  
  // Check INITX/External Reset bit
  if (1 == u8ReadResetCause.INITX)
  {
    pstcResult->bInitx = TRUE;
  }
  else
  {
    pstcResult->bInitx = FALSE;
  }
  
  // Check Low Voltage Detection bit
#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  if (1 == u8ReadResetCause.LVDH)
  {
    pstcResult->bLowVoltageDetection = TRUE;
  }
  else
  {
    pstcResult->bLowVoltageDetection = FALSE;
  }
#else
  pstcResult->bLowVoltageDetection = FALSE; // Always FALSE for other Types than 3 and 7
#endif
  
  // Check Software Watchdog bit
  if (1 == u8ReadResetCause.SWDT)
  {
    pstcResult->bSoftwareWatchdog = TRUE;
  }
  else
  {
    pstcResult->bSoftwareWatchdog = FALSE;
  }
  
  // Check Hardware Watchdog bit
  if (1 == u8ReadResetCause.HWDT)
  {
    pstcResult->bHardwareWatchdog = TRUE;
  }
  else
  {
    pstcResult->bHardwareWatchdog = FALSE;
  }
  
  // Check Clock Supervisor bit
  if (1 == u8ReadResetCause.CSVR)
  {
    pstcResult->bClockSupervisor = TRUE;
  }
  else
  {
    pstcResult->bClockSupervisor = FALSE;
  }
  
  // Check Anomalous Frequency bit
  if (1 == u8ReadResetCause.FCSR)
  {
    pstcResult->bAnomalousFrequency = TRUE;
  }
  else
  {
    pstcResult->bAnomalousFrequency = FALSE;
  }
  
  // Check Software Reset bit
  if (1 == u8ReadResetCause.SRST)
  {
    pstcResult->bSoftware = TRUE;
  }
  else
  {
    pstcResult->bSoftware = FALSE;
  }
  
  return Ok;
} // Reset_GetCause

//@} // ResetGroup

#endif // #if (defined(L3_PERIPHERAL_RESET_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
