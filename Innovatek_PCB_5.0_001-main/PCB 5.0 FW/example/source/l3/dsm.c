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
/** \file dsm.c
 **
 ** A detailed description is available at 
 ** @link DsmGroup DSM Module description @endlink
 **
 ** History:
 **   - 2012-07-06  1.0  MWi  First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "dsm.h"

#if (defined(L3_PERIPHERAL_DSM_ACTIVE))

  #if (L3_DSM_SUPPORT == L3_OFF)
    #error Device does not support Deep Standby Modes!
  #endif

/**
 ******************************************************************************
 ** \addtogroup DsmGroup
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
 ** \brief Set Deep Standby Mode
 **
 ** This function sets the Deep Standby STOP or RTC mode. The pinstate defined
** by stc_dsm_config_t#bLpmPortHiZState is also set.
 **
 ** \param [in]  pstcConfig        DSM configuration parameters
 ** 
 ** \retval Ok                     DSM mode set
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal mode
 ******************************************************************************/
en_result_t Dsm_SetMode(stc_dsm_config_t* pstcConfig)
{
  stc_dsm_stb_ctl_field_t unSTB_CTL = {0};  // Local STB_CTL predefinition
  stc_dsm_pmd_ctl_t       unPMD_CTL = {0};  // Local PMD_CTL predefinition
  stc_dsm_reg_ctl_t       unREG_CTL = {0};  // Local REG_CTL predefinition
  stc_dsm_rck_ctl_t       unRCK_CTL = {0};  // Local RCK_CTL predefinition
  stc_dsm_wrfsr_t         unWRFSR   = {0};  // Local WRFSR predefinition
  union {                                   // Local WIER predefinition
    stc_dsm_wier_t        unWIER;  
    uint16_t              u16WIER;
  };
  union {                                   // Local WILVR predefinition
    stc_dsm_wilvr_t       unWILVR;  
    uint16_t              u16WILVR;
  };
  stc_dsm_dsramr_t        unDSRAMR = {0};   // Local DSRAMR predefinition
  
  if ( pstcConfig == NULL )
  {
      return ErrorInvalidParameter ;
  } 
  
  unSTB_CTL.KEY  = 0x1ACC; // Access key to STB_CTL
  unSTB_CTL.SPL  = (pstcConfig->bLpmPortHiZState == TRUE) ? 1 : 0;
  unSTB_CTL.DSTM = 1; // Deep Standby
  unSTB_CTL.STM  = 0x2;
  
  unREG_CTL.ISUBSEL = 0x2;  // Fix value to 360nA
  
  switch (pstcConfig->enMode)
  {
    case DeepStop:
      unPMD_CTL.RTCE = 0;
      break;
    case DeepRtc:
      unPMD_CTL.RTCE = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  unRCK_CTL.CECCKE = (pstcConfig->bSubClkHdmiCec == TRUE) ? 1 : 0;
  unRCK_CTL.RTCCKE = (pstcConfig->bSubClkRtc     == TRUE) ? 1 : 0;
  
  unWRFSR.WLVDH  = (pstcConfig->bLvdResetReturn   == TRUE) ? 1 : 0;
  unWRFSR.WINITX = (pstcConfig->bInitxResetReturn == TRUE) ? 1 : 0;
  
  u16WIER = 0;
  unWIER.WRTCE = (pstcConfig->bRtcInterrupt  == TRUE) ? 1 : 0;
  unWIER.WLVDE = (pstcConfig->bLvdInterrupt  == TRUE) ? 1 : 0;
  u16WIER |= ((0x3E & (pstcConfig->WkPinChannelEnable)) << 2); // Set wake-up pin channel enables (w/o channel 0)
  u16WIER |= ((0x3  & (pstcConfig->HdmiChannelEnable))  << 8); // Set HDMI channel enables
  
  u16WILVR = ((0x3E & (pstcConfig->WkPinChannelLevel)) >> 1); // Set wake-up pin channel levels (w/o channel 0)
  
  unDSRAMR.SRAMR = (pstcConfig->bSramRetentionEnable == TRUE) ? 0x3 : 0;
    
  // Set up hardware
  DSM->u8REG_CTL     = unREG_CTL;
  DSM->u8RCK_CTL     = unRCK_CTL;
  DSM->WIER          = u16WIER;
  DSM->WILVR         = u16WILVR;
  DSM->u8DSRAMR      = unDSRAMR;
  DSM->u8PMD_CTL     = unPMD_CTL;
  
  DSM_CRG->STB_CTL_f = unSTB_CTL;
  DSM_SCB_SCR        = DSM_SCB_SCR_SLEEPDEEP_BITPOSITION; // Set Cortex-M3 SLEEPDEEP
  
  // Now perform transition to Deep Standby Mode
  if (pstcConfig->pfnHook != NULL)    // Hook function specified?
  {
     pstcConfig->pfnHook();
  }
  __WFI();                            // wait for interrupt -> CPU stopped.      
  
  return Ok;
} // Dsm_SetMode(stc_dsm_config_t* pstcConfig)

/**
 ******************************************************************************
 ** \brief Get return cause from Deep Standby Mode
 **
 ** This function sets via a pointer to a structure type #stc_dsm_return_cause_t
 ** the according return cause bits.
 **
 ** \param [out]  pstcReturnCause  DSM return cause structure
 ** 
 ** \retval Ok                     DSM return cause set sucessfully
 ** \retval Error                  No return cause was found
 ******************************************************************************/
en_result_t Dsm_GetReturnCause(stc_dsm_return_cause_t* pstcReturnCause)
{
  en_result_t enError = Error;  // preset to error
  
  // check bits
  if (DSM->u8WRFSR.WLVDH == 1)
  {
     pstcReturnCause->LvdReset = 1;
     enError = Ok;
  }
  
  if (DSM->u8WRFSR.WINITX == 1)
  {
     pstcReturnCause->InitxReset = 1;
     enError = Ok;
  }
  
  if (DSM->u16WIFSR.WCEC0I == 1)
  {
     pstcReturnCause->Hdmi0Interrupt = 1;
     enError = Ok;
  }

  if (DSM->u16WIFSR.WCEC1I == 1)
  {
     pstcReturnCause->Hdmi1Interrupt = 1;
     enError = Ok;
  }

  if (DSM->u16WIFSR.WUI0 == 1)
  {
     pstcReturnCause->WakUpPin0 = 1;
     enError = Ok;
  }
  
  if (DSM->u16WIFSR.WUI1 == 1)
  {
     pstcReturnCause->WakUpPin1 = 1;
     enError = Ok;
  }
  
  if (DSM->u16WIFSR.WUI2 == 1)
  {
     pstcReturnCause->WakUpPin2 = 1;
     enError = Ok;
  }
  
  if (DSM->u16WIFSR.WUI3 == 1)
  {
     pstcReturnCause->WakUpPin3 = 1;
     enError = Ok;
  }
  
  if (DSM->u16WIFSR.WUI4 == 1)
  {
     pstcReturnCause->WakUpPin4 = 1;
     enError = Ok;
  }
  
  if (DSM->u16WIFSR.WUI5 == 1)
  {
     pstcReturnCause->WakUpPin5 = 1;
     enError = Ok;
  }
  return enError;
}

//@} // DsmGroup

#endif // #if (defined(L3_PERIPHERAL_DSM_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
