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
/** \file clk.c
 **
 ** A detailed description is available at 
 ** @link ClkGroup Clock Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "clk.h"

#if (defined(L3_PERIPHERAL_CLK_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup ClkGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
stc_clk_intern_data_t stcClkInternData;

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
 ** \brief Clock Stabilization Interrupt Handler
 ******************************************************************************/
void ClkIrqHandler(void)
{
  uint8_t u8IntStrReadOut;
  
  u8IntStrReadOut = FM3_CRG->INT_STR;
  
  // PLL stabilization ready?
  if (0 != (u8IntStrReadOut & FM3_INT_STR_PCSI_BITPOS))
  {
    FM3_CRG->INT_CLR |= FM3_INT_CLR_PCSC_BITPOS;  // Clear Irq
    
    // PLL ready callback if defined
    if (stcClkInternData.pfnPllStabCb != NULL)
    {
      stcClkInternData.pfnPllStabCb();
    }
  }

  // Sub Clock stabilization ready?  
  if (0 != (u8IntStrReadOut & FM3_INT_STR_SCSI_BITPOS))
  {
    FM3_CRG->INT_CLR |= FM3_INT_CLR_SCSC_BITPOS;  // Clear Irq
    
    // Sub Clock ready callback if defined
    if (stcClkInternData.pfnScoStabCb != NULL)
    {
      stcClkInternData.pfnScoStabCb();
    }
  }

  // Main Clock stabilization ready?  
  if (0 != (u8IntStrReadOut & FM3_INT_STR_MCSI_BITPOS))
  {
    FM3_CRG->INT_CLR |= FM3_INT_CLR_MCSC_BITPOS;  // Clear Irq
    
    // Main Clock ready callback if defined
    if (stcClkInternData.pfnMcoStabCb != NULL)
    {
      stcClkInternData.pfnMcoStabCb();
    }
  }
}

/**
 ******************************************************************************
 ** \brief Get current Clock parameters
 **
 ** This function sets the 'current' elements of the configuration according
 ** the clock registers.
 ** 
 ** \note This function overwrites any configuration. To avoid this, a second
 **       configuration structure like 'ConfigRecent' may be used.
 **
 ** \note This function does not set any hook function pointer! If this function
 **       is used to get the current clock settings as a base for new settings,
 **       a possible hook function pointer must be set explicitly after copying
 **       the configuration!
 **
 ** \param [out]  pstcConfig       Clock configuration parameters
 **
 ** \retval Ok                     Clock configuration has been set
 ** \retval ErrorInvalidParameter  If pstcConfig == NULL
 ** \retval ErrorInvalidMode       Illegal clock mode has been detected
 **
 ******************************************************************************/
en_result_t Clk_GetParameters(stc_clk_config_t* pstcConfig)
{
  uint8_t u8Dummy;
  
  if ( pstcConfig == NULL )
  {
      return ErrorInvalidParameter ;
  } 
  
  u8Dummy = (0x07 & (FM3_CRG->SCM_STR >> 5));            
  switch (u8Dummy)
  {
    case 0:
      pstcConfig->enSource = ClkHsCr ;
      break;
    case 1:
      pstcConfig->enSource = ClkMain ;
      break;   
    case 2:
      pstcConfig->enSource = ClkPll ;
      break;      
    case 4:
      pstcConfig->enSource = ClkLsCr ;
      break;    
    case 5:
      pstcConfig->enSource = ClkSub ;
      break;  
    default:
      return ErrorInvalidMode ;
  }

  pstcConfig->bEnablePll       = (FM3_CRG->SCM_CTL_f.PLLE == 1)  ? 1 : 0 ;
  pstcConfig->bEnableMainClock = (FM3_CRG->SCM_CTL_f.MOSCE == 1) ? 1 : 0 ;
  pstcConfig->bEnableSubClock  = (FM3_CRG->SCM_CTL_f.SOSCE == 1) ? 1 : 0 ;
  
  pstcConfig->enBaseClkDiv = (en_clk_baseclkdiv_t)(0x07 & FM3_CRG->BSC_PSR) ;
  pstcConfig->enAPB0Div    = (en_clk_apb0div_t)(0x03 & FM3_CRG->APBC0_PSR) ;
  pstcConfig->enAPB1Div    = (en_clk_apb1div_t)(0x03 & FM3_CRG->APBC1_PSR) ;
  pstcConfig->enAPB2Div    = (en_clk_apb2div_t)(0x03 & FM3_CRG->APBC2_PSR) ;
  
  pstcConfig->enSCOWaitTime = (en_clk_scowaittime_t)(0x07 & (FM3_CRG->CSW_TMR >> 4)) ;
  pstcConfig->enMCOWaitTime = (en_clk_mcowaittime_t)(0x07 & FM3_CRG->CSW_TMR) ;
  
  pstcConfig->u8PllK = ((0x0F & (FM3_CRG->PLL_CTL1)) >> 4) + 1 ;
  pstcConfig->u8PllM = (0x0F & (FM3_CRG->PLL_CTL1)) + 1 ;
  pstcConfig->u8PllN = (0x1F & (FM3_CRG->PLL_CTL2)) + 1 ;
  
  pstcConfig->bPllIrq = (FM3_CRG->INT_ENR_f.PCSE) ? 1 : 0 ;
  pstcConfig->bScoIrq = (FM3_CRG->INT_ENR_f.SCSE) ? 1 : 0 ;
  pstcConfig->bMcoIrq = (FM3_CRG->INT_ENR_f.MCSE) ? 1 : 0 ;
  
  pstcConfig->enMode = ClkRun ; // Function can only be called in run mode
  
  return Ok;
} // Clk_GetParameters

/**
 ******************************************************************************
 ** \brief Set Clock Dividers
 **
 ** This function sets the clock dividers.
 **
 ** \note It is strongly recommended to disable any resource of its
 **       corresponding bus, if the bus division setting is changed! Malfunction
 **       of the resources may result (i.e. wrong baud rates, lost data, etc.).
 **
 ** \note Do not access any of the resource registers, if the corresponding
 **       resource's bus is disabled! An immediate bus fault exception will
 **       occur in this case!
 **
 ** \param [in]  pstcConfig        Clock configuration parameters
 ** 
 ** \retval Ok                     Dividers set
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal divider
 **                                setting
 ******************************************************************************/
en_result_t Clk_SetDividers(stc_clk_config_t* pstcConfig)
{
  if ( pstcConfig == NULL )
  {
      return ErrorInvalidParameter ;
  } 

  // Set Base Clock divider
  if (pstcConfig->enBaseClkDiv > BSC_PSR_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  {
    FM3_CRG->BSC_PSR = pstcConfig->enBaseClkDiv;
  }

  // Set APB0 Clock divider
  if (pstcConfig->enAPB0Div > APBC0_PSR_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  {
    // Setting APCC0[1:0]
    FM3_CRG->APBC0_PSR = pstcConfig->enAPB0Div;
  }  

  // Set APB1 Clock divider
  if (pstcConfig->enAPB1Div > APBC1_PSR_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  {
    // Setting APCC1[1:0] and APBC1EN (APBC1RST is always set to 0)
    FM3_CRG->APBC1_PSR = (pstcConfig->enAPB1Div) | (((pstcConfig->bAPB1Disable == 1) ? 0 : 1) << 7);
  } 
  
  // Set APB2 Clock divider
  if (pstcConfig->enAPB2Div > APBC2_PSR_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  {
    // Setting APCC2[1:0] and APBC2EN (APBC2RST is always set to 0)
    FM3_CRG->APBC2_PSR = (pstcConfig->enAPB2Div) | (((pstcConfig->bAPB2Disable == 1) ? 0 : 1) << 7);
  }   
  
  return Ok;
} // Clk_SetDividers

/**
 ******************************************************************************
 ** \brief Set Oscillation Stabilization Wait Times
 **
 ** This function sets the stabilization wait times.
 **
 ** \param [in]  pstcConfig        Clock configuration parameters
 ** 
 ** \retval Ok                     Dividers set
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal timing
 **                                setting
 ******************************************************************************/
en_result_t Clk_SetStabilizationWaitTimes(stc_clk_config_t* pstcConfig)
{
  uint8_t u8Dummy;
  uint8_t u8Pinc = 0;
  
  if ( pstcConfig == NULL )
  {
      return ErrorInvalidParameter ;
  } 
  
  // local variable for CSW_TMR
  u8Dummy = 0;
  
  if (pstcConfig->enSCOWaitTime > CSW_TMR_SOWT_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  {
    u8Dummy = (pstcConfig->enSCOWaitTime) << 4 ;
  }
  
  if (pstcConfig->enMCOWaitTime > CSW_TMR_MOWT_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  {
    u8Dummy |= pstcConfig->enMCOWaitTime ;
  }
  
  // update hardware
  FM3_CRG->CSW_TMR = u8Dummy ;
  
  if (pstcConfig->enPLLOWaitTime > PSW_TMR_POWT_MAX)
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    if (pstcConfig->enPllSource == PllSrcClkHc)
    {
      u8Pinc = FM3_PSW_TMR_PINC_BITPOS;
    }
    FM3_CRG->PSW_TMR = (pstcConfig->enPLLOWaitTime) | u8Pinc ;
  }
  
  return Ok;
} // Clk_SetStabilizationWaitTimes

/**
 ******************************************************************************
 ** \brief Wait for Main Oscillation Stabilization
 **
 ** This function waits for the Main Oscillator stabilization via polling.
 ** L3_WAIT_LOOP_HOOK() is called during polling. It should be called, if the
 ** system needs a stable main clock (i.e. for communication or switching to
 ** PLL clock, etc.).
 **
 ** \param [in]  u32MaxTimeOut   Time out counter start value
 ** 
 ** \retval Ok                   Clock stabilized
 ** \retval ErrorTimeout         Clock not stabilized after timeout count
 ******************************************************************************/
en_result_t Clk_WaitForMainOscillator(uint32_t u32MaxTimeOut)
{
  uint32_t u32Timeout = u32MaxTimeOut;
  
  while (u32Timeout--)
  {
    if (FM3_CRG->SCM_STR_f.MORDY == 1)
    {
      return Ok;
    }
    
    L3_WAIT_LOOP_HOOK();
  }
  
  return ErrorTimeout;
} // Clk_WaitForMainOscillator

/**
 ******************************************************************************
 ** \brief Wait for Sub Oscillation Stabilization
 **
 ** This function waits for the Sub Oscillator stabilization via polling.
 ** L3_WAIT_LOOP_HOOK() is called during polling. It should be called, if the
 ** system needs a stable sub clock (i.e. for communication).
 **
 ** \param [in]  u32MaxTimeOut  Time out counter start value
 ** 
 ** \retval Ok                  Clock stabilized
 ** \retval ErrorTimeout        Clock not stabilized after timeout count
 ******************************************************************************/
en_result_t Clk_WaitForSubOscillator(uint32_t u32MaxTimeOut)
{
  uint32_t u32Timeout = u32MaxTimeOut;
  
  while (u32Timeout--)
  {
    if (FM3_CRG->SCM_STR_f.SORDY == 1)
    {
      return Ok;
    }
    
    L3_WAIT_LOOP_HOOK();
  }
  
  return ErrorTimeout;
} // Clk_WaitForSubOscillator

/**
 ******************************************************************************
 ** \brief Wait for PLL Oscillation Stabilization
 **
 ** This function waits for the PLL Oscillator stabilization via polling.
 ** L3_WAIT_LOOP_HOOK() is called during polling. It should be called, if the
 ** system needs a stable PLL clock (i.e. for communication).
 **
 ** \param [in]  u32MaxTimeOut   Time out counter start value
 ** 
 ** \retval Ok                   Clock stabilized
 ** \retval ErrorTimeout         Clock not stabilized after timeout count
 ******************************************************************************/
en_result_t Clk_WaitForPllOscillator(uint32_t u32MaxTimeOut)
{
  uint32_t u32Timeout = u32MaxTimeOut;
  
  while (u32Timeout--)
  {
    if (FM3_CRG->SCM_STR_f.PLRDY == 1)
    {
      return Ok;
    }
    
    L3_WAIT_LOOP_HOOK();
  }
  
  return ErrorTimeout;
} // Clk_WaitForPllOscillator

/**
 ******************************************************************************
 ** \brief Wait for Clock Mode ready (transition finished)
 **
 ** This function waits for a clock source, meaning a clock source already
 ** available or a clock source transition to be expected to be ready soon or
 ** already available. L3_WAIT_LOOP_HOOK() is called during polling.
 **
 ** \note This function is not needed to be called, if the user has performed
 **       the stabilization wait time for the desired source clock before.
 **       For safety reasons, this function can be called anyhow with a small
 **       timeout count (<<10).
 **
 ** \param [in]  enSource          Clock Source to be checked
 ** \param [in]  u32MaxTimeOut     Time out counter start value
 ** 
 ** \retval Ok                     Clock Source ready
 ** \retval ErrorTimeout           Clock Source not ready within time out count
 ** \retval ErrorInvalidParameter  Not a valid Clock Mode to be checked
 ******************************************************************************/
en_result_t Clk_WaitForClockSourceReady(en_clk_source_t enSource,
                                        uint32_t        u32MaxTimeOut)
{
  uint32_t u32Timeout = u32MaxTimeOut;
  uint8_t  u8SourcePattern;
  
  switch(enSource)
  {
    case ClkHsCr:
      u8SourcePattern = 0x0;
      break;   
    case ClkMain:
      u8SourcePattern = 0x1;
      break;
    case ClkPll:
      u8SourcePattern = 0x2;
      break; 
    case ClkLsCr:
      u8SourcePattern = 0x4;
      break; 
    case ClkSub:
      u8SourcePattern = 0x5;
      break;  
    default:
      return ErrorInvalidParameter;
  }
  
  while (u32Timeout--)
  {
    if ((0x07 & (FM3_CRG->SCM_STR >> 5)) == u8SourcePattern)
    {
      return Ok;
    }
    
    L3_WAIT_LOOP_HOOK();
  }
  
  return ErrorTimeout;
} // Clk_WaitForClockSourceReady

/**
 ******************************************************************************
 ** \brief Main Oscillation Ready?
 **
 ** This function checks the availability of a stable Main Clock.
 **
 ** \retval Ok                   Main Clock stabilized
 ** \retval ErrorNotReady        Main Clock not stabilized yet
 ******************************************************************************/
en_result_t Clk_MainOscillatorReady(void)
{
  if (FM3_CRG->SCM_STR_f.MORDY == 1)
  {
    return Ok;
  }

  return ErrorNotReady;
} // Clk_MainOscillatorReady

/**
 ******************************************************************************
 ** \brief Sub Oscillation Ready?
 **
 ** This function checks the availability of a stable Sub Clock.
 **
 ** \retval Ok                   Sub Clock stabilized
 ** \retval ErrorNotReady        Sub Clock not stabilized yet
 ******************************************************************************/
en_result_t Clk_SubOscillatorReady(void)
{
  if (FM3_CRG->SCM_STR_f.SORDY == 1)
  {
    return Ok;
  }

  return ErrorNotReady;
} // Clk_SubOscillatorReady

/**
 ******************************************************************************
 ** \brief PLL Oscillation Ready?
 **
 ** This function checks the availability of a stable PLL Clock.
 **
 ** \retval Ok                   PLL Clock stabilized
 ** \retval ErrorNotReady        PLL Clock not stabilized yet
 ******************************************************************************/
en_result_t Clk_PllOscillatorReady(void)
{
  if (FM3_CRG->SCM_STR_f.PLRDY == 1)
  {
    return Ok;
  }

  return ErrorNotReady;
} // Clk_PllOscillatorReady

/**
 ******************************************************************************
 ** \brief Set Clock Source
 **
 ** This function sets the clock source and performs transition, if wanted.
 ** Illegal transitions (e.g. changing PLL settings or switch to PLL from
 ** other modes than Main Clock) are checked and corresponding error values
 ** are returned.
 **
 ** \pre Clk_SetStabilizationWaitTimes() has to be called before.
 **
 ** \param [in]  pstcConfig        Clock configuration parameters
 ** 
 ** \retval Ok                     Clock source set
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal mode
 ** \retval ErrorInvalidMode       Clock setting not possible
 ******************************************************************************/
en_result_t Clk_SetSource(stc_clk_config_t* pstcConfig)
{
  stc_clk_config_t stcCurrentConfig;      // local read-out configuration
  uint8_t          u8Dummy;               // local register preset variable
  stcSCM_CTL_t     stcSCM_CTL;

  if ( pstcConfig == NULL )
  {
    return ErrorInvalidParameter ;
  } 
  
  if (Ok != Clk_GetParameters(&stcCurrentConfig))
  {
    return ErrorInvalidParameter ;
  }
  
  u8Dummy = 0;
  
  // PLL enabled?
  if (pstcConfig->bEnablePll == TRUE)
  {    
    // Check for illegal PLL settings change when already running ...
    if (stcCurrentConfig.bEnablePll == TRUE) 
    {
      if ((pstcConfig->u8PllK != stcCurrentConfig.u8PllK) ||
          (pstcConfig->u8PllM != stcCurrentConfig.u8PllM) ||
          (pstcConfig->u8PllN != stcCurrentConfig.u8PllN)
          )
      {
        return ErrorInvalidParameter ;  
      }
    }
    
    // Check for PLL enable only possible from Main Clock
    if (!(pstcConfig->bEnableMainClock == TRUE) &&
         (stcCurrentConfig.bEnableMainClock == TRUE))
    {
      return ErrorInvalidParameter ;
    }
  
    // PLL only possible, if Main Clock stabilized
    if (FM3_CRG->SCM_STR_f.MORDY != 1)
    {
      return ErrorInvalidMode ;
    }
    
    // PLLK
    if ((pstcConfig->u8PllK > PLL_CTL1_PLLK_MAX) ||
        (pstcConfig->u8PllK == 0)
       )
    {
      return ErrorInvalidParameter ;
    }
    else
    {
      u8Dummy = ((pstcConfig->u8PllK) - 1) << 4 ; // PLLK value (0000) + 1 => 1/1
    }
    
    // PLLM
    if ((pstcConfig->u8PllM > PLL_CTL1_PLLM_MAX) ||
        (pstcConfig->u8PllM == 0)
       )
    {
      return ErrorInvalidParameter ;
    }
    else
    {
      u8Dummy |= (pstcConfig->u8PllM) - 1 ; // PLLM value (0000) + 1 => 1/1
    }
    
    FM3_CRG->PLL_CTL1 = u8Dummy ;
    
    // PLLN
    if ((pstcConfig->u8PllN > PLL_CTL2_PLLN_MAX) ||
        (pstcConfig->u8PllN == 0)
       )
    {
      return ErrorInvalidParameter ;
    }
    else
    {
      FM3_CRG->PLL_CTL2 =( pstcConfig->u8PllN) - 1 ; // PLLM value (00000) + 1 => 1/1
    }
  }
  
  // Set System Clock Mode Control Register (SCM_CTL)
  stcSCM_CTL.u8SCM_CTL = 0; // local predefinition
  
  stcSCM_CTL.SCM_CTL_f.PLLE  = (pstcConfig->bEnablePll == TRUE)       ? 1 : 0 ;
  stcSCM_CTL.SCM_CTL_f.MOSCE = (pstcConfig->bEnableMainClock == TRUE) ? 1 : 0 ;
  stcSCM_CTL.SCM_CTL_f.SOSCE = (pstcConfig->bEnableSubClock == TRUE)  ? 1 : 0 ;
  
  // Now set RCS bits of SCM_CTL
  switch ((uint8_t) pstcConfig->enSource)
  {
    case ClkMain :
      // Check if main clock oscillator is enabled
      if (stcCurrentConfig.bEnableMainClock == TRUE)
      {
        stcSCM_CTL.SCM_CTL_f.RCS = 1 ;
      }
      else
      {
        return ErrorInvalidMode ;
      }
      break;
    case ClkSub :
      // Check if sub clock oscillator is enabled
      if (stcCurrentConfig.bEnableSubClock == TRUE)
      {
        stcSCM_CTL.SCM_CTL_f.RCS = 5 ;
       }
      else
      {
        return ErrorInvalidMode ;
      }       
      break;
    case ClkHsCr :
      stcSCM_CTL.SCM_CTL_f.RCS = 0 ;
      break;
    case ClkLsCr :
      stcSCM_CTL.SCM_CTL_f.RCS = 4 ;
      break;   
    case ClkPll :
      stcSCM_CTL.SCM_CTL_f.RCS = 2 ;
      break;         
    default:
      return ErrorInvalidParameter ;
  }
  
  // Update Hardware
  FM3_CRG->SCM_CTL = stcSCM_CTL.u8SCM_CTL ;
  
  return Ok;
} // Clk_SetSource

/**
 ******************************************************************************
 ** \brief Set PLL Input Clock Source
 **
 ** This function sets the PLL input clock source, if the device supports
 ** HS-RC-Clock input for PLL! This function only works, if PLL is disabled.
 **
 ** \attention This driver does not check the PINC bit availability!
 **
 ** \param [in]  pstcConfig        Clock configuration parameters
 ** 
 ** \retval Ok                     PLL Clock sourse set
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal mode
 ** \retval ErrorInvalidMode       Clock setting not possible (PLL not disabled)
 ******************************************************************************/
en_result_t Clk_SetPllSource(stc_clk_config_t* pstcConfig)
{
  stc_clk_config_t stcCurrentConfig;      // local read-out configuration

  if ( pstcConfig == NULL )
  {
    return ErrorInvalidParameter ;
  } 
  
  if (Ok != Clk_GetParameters(&stcCurrentConfig))
  {
    return ErrorInvalidParameter ;
  }  
  
  if (stcCurrentConfig.bEnablePll == TRUE)
  {
    return ErrorInvalidMode ;
  }
  
  if (pstcConfig->enPllSource == PllSrcClkHc)
  {
    FM3_CRG->PSW_TMR |= FM3_PSW_TMR_PINC_BITPOS ;
  }
  else
  {
    FM3_CRG->PSW_TMR &= (0xFF ^ FM3_PSW_TMR_PINC_BITPOS) ;
  }
  
  return Ok;
} // Clk_SetPllSource

/**
 ******************************************************************************
 ** \brief Set Clock Mode
 **
 ** This function sets the clock mode and performs the transition. For
 ** individual settings (such as USB and CAN low power configuration) a hook
 ** function is called after setting SLEEPDEEP to 1 and final WFI instruction.
 ** This function is only called, if the function pointer is unequal to NULL.
 ** Additionally the ports will go into Hi-Z state, if
 ** stc_clk_config_t#bPortHiZState is TRUE.
 **
 ** \param [in]  pstcConfig        Clock configuration parameters
 ** 
 ** \retval Ok                     Clock mode set
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal mode
 ******************************************************************************/
en_result_t Clk_SetMode(stc_clk_config_t* pstcConfig)
{
  uint32_t u32PortState; 
  
  if ( pstcConfig == NULL )
  {
      return ErrorInvalidParameter ;
  } 
  
  u32PortState = (pstcConfig->bLpmPortHiZState == TRUE) ? 0x00000010 : 0 ; // SPL setting
  
  switch(pstcConfig->enMode)
  {
    case ClkRun : // obsolete, just for the sake of completeness ...
      break;
      
    case ClkSleep :
      FM3_PMD_CTL = 0x00;                 // PMD_CTL.RTCE = 0 (no effect, if RTC not available)
      SCB->SCR = 0;                       // Sleepdeep = 0
      
      if (pstcConfig->pfnHook != NULL)    // Hook function specified?
      {
         pstcConfig->pfnHook();
      }
      __WFI();                            // wait for interrupt -> CPU stopped.
      break;
      
    case ClkTimer :
      FM3_PMD_CTL = 0x00;                 // PMD_CTL.RTCE = 0 (no effect, if RTC not available)
      FM3_CRG->STB_CTL = 0x1ACC0000 | u32PortState; // STM = 0b00
      CLK_SCB_SCR = CLK_SCB_SCR_SLEEPDEEP_Mask;	    // Sleepdeep = 1
      
      if (pstcConfig->pfnHook != NULL)    // Hook function specified?
      {
         pstcConfig->pfnHook();
      }
      __WFI();                            // wait for interrupt -> CPU stopped.
      break;
      
    case ClkStop :
      FM3_PMD_CTL = 0x00;                 // PMD_CTL.RTCE = 0 (no effect, if RTC not available)
      FM3_CRG->STB_CTL = 0x1ACC0002 | u32PortState; // STM = 0b10
      CLK_SCB_SCR = CLK_SCB_SCR_SLEEPDEEP_Mask;	    // Sleepdeep = 1
      
      if (pstcConfig->pfnHook != NULL)    // Hook function specified?
      {
         pstcConfig->pfnHook();
      }
      __WFI();                            // wait for interrupt -> CPU stopped.      
      break;

#if (L3_RTC_AVAILABLE == L3_ON)
    case ClkRtc :
      FM3_PMD_CTL = 0x01;                 // PMD_CTL.RTCE = 1
      FM3_CRG->STB_CTL = 0x1ACC0002 | u32PortState; // STM = 0b10
      
      if (pstcConfig->pfnHook != NULL)    // Hook function specified?
      {
         pstcConfig->pfnHook();
      }
      __WFI();                            // wait for interrupt -> CPU stopped.      
      break;      
      
    case ClkDeepRtc :
      FM3_PMD_CTL = 0x01;                 // PMD_CTL.RTCE = 1
      FM3_CRG->STB_CTL = 0x1ACC0002 | u32PortState | FM3_STB_CTL_DSTM_BITPOS ; // STM = 0b10, DSTM = 1
      
      if (pstcConfig->pfnHook != NULL)    // Hook function specified?
      {
         pstcConfig->pfnHook();
      }
      __WFI();                            // wait for interrupt -> CPU stopped.      
      break;
#endif  

    default :
      return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Clk_SetMode

/**
 ******************************************************************************
 ** \brief Enable Sub Clock
 **
 ** This function easily enables the Sub Clock. No configuration is needed.
 ** For stabilization wait time Clk_WaitForSubOscillator() has to be called
 ** afterwards.
 **
 ** \retval Ok                Sub Clock enabled
 ** \retval ErrorInvalidMode  Sub Clock not enabled (e.g. not existing)
 ******************************************************************************/
en_result_t Clk_EnableSubClock(void)
{
  FM3_CRG->SCM_CTL_f.SOSCE = 1;
  
  if (FM3_CRG->SCM_CTL_f.SOSCE != 1)
  {
    return ErrorInvalidMode;
  }
  
  return Ok;
} // Clk_EnableSubClock

/**
 ******************************************************************************
 ** \brief Disable Sub Clock
 **
 ** This function easily disables the Sub Clock. No configuration is needed.
 **
 ** \retval Ok                Sub Clock disabled
 ******************************************************************************/
en_result_t Clk_DisableSubClock(void)
{
  FM3_CRG->SCM_CTL_f.SOSCE = 0;

  return Ok;
} // Clk_DisableSubClock

/**
 ******************************************************************************
 ** \brief Enable Main Clock
 **
 ** This function easily enables the Main Clock. No configuration is needed.
 ** For stabilization wait time Clk_WaitForMainOscillator() has to be called
 ** afterwards.
 **
 ** \retval Ok                Main Clock enabled
 ******************************************************************************/
en_result_t Clk_EnableMainClock(void)
{
  FM3_CRG->SCM_CTL_f.MOSCE = 1;

  return Ok;
} // Clk_EnableMainClock

/**
 ******************************************************************************
 ** \brief Disable Main Clock
 **
 ** This function easily disables the Main Clock. No configuration is needed.
 **
 ** \retval Ok                Main Clock disabled
 ******************************************************************************/
en_result_t Clk_DisableMainClock(void)
{
  FM3_CRG->SCM_CTL_f.MOSCE = 0;

  return Ok;
} // Clk_DisableMainClock

/**
 ******************************************************************************
 ** \brief Set/Clear Clock interrupts
 **
 ** This function enables or disables the clock stabilization interrupts 
 ** according configuration
 **
 ** \param [in]  pstcConfig        Clock configuration parameters
 ** \param [in]  bTouchNvic        TRUE = Touch NVIC registers, FALSE = Do not
 **                                touch NVIC registers
 **
 ** \retval Ok                     Interrupts have been set
 ** \retval ErrorInvalidParameter  If pstcConfig == NULL
 ******************************************************************************/
en_result_t Clk_SetIrq( stc_clk_config_t* pstcConfig,
                        boolean_t         bTouchNvic
                      )
{
  uint8_t u8SetNvic = 0;      // 1 = Enable NVIC interrupts, 0 = Disable NVIC interrupts
  
  if ( pstcConfig == NULL )
  {
      return ErrorInvalidParameter ;
  } 
  
  // PLL Stabilization Interrupt
  if (pstcConfig->bPllIrq == TRUE)
  {
    FM3_CRG->INT_ENR |= FM3_INT_ENR_PCSE_BITPOS;
    
    if (pstcConfig->pfnPllStabCb != NULL)
    {
      stcClkInternData.pfnPllStabCb = pstcConfig->pfnPllStabCb;
      u8SetNvic = 1;
    }
  }
  else
  {
    FM3_CRG->INT_ENR &= 0xFF ^ FM3_INT_ENR_PCSE_BITPOS;
  }

  // Sub Clock Stabilization Interrupt
  if (pstcConfig->bScoIrq == TRUE)
  {
    FM3_CRG->INT_ENR |= FM3_INT_ENR_SCSE_BITPOS;
    
    if (pstcConfig->pfnScoStabCb != NULL)
    {
      stcClkInternData.pfnScoStabCb = pstcConfig->pfnScoStabCb;
      u8SetNvic |= 1;
    }
  }
  else
  {
    FM3_CRG->INT_ENR &= 0xFF ^ FM3_INT_ENR_SCSE_BITPOS;
  }  

  // Main Clock Stabilization Interrupt
  if (pstcConfig->bMcoIrq == TRUE)
  {
    FM3_CRG->INT_ENR |= FM3_INT_ENR_MCSE_BITPOS;
    
    if (pstcConfig->pfnMcoStabCb != NULL)
    {
      stcClkInternData.pfnMcoStabCb = pstcConfig->pfnMcoStabCb;
      u8SetNvic |= 1;
    }
  }
  else
  {
    FM3_CRG->INT_ENR &= 0xFF ^ FM3_INT_ENR_MCSE_BITPOS;
  }  
  
  if (TRUE == bTouchNvic)
  {
    if (0 != u8SetNvic)
    {
      NVIC_ClearPendingIRQ(OSC_PLL_WC_IRQn);
      NVIC_EnableIRQ(OSC_PLL_WC_IRQn);
      NVIC_SetPriority(OSC_PLL_WC_IRQn, L3_IRQ_LEVEL_CAN0);         
    }
    else
    {
      NVIC_ClearPendingIRQ(OSC_PLL_WC_IRQn);
      NVIC_DisableIRQ(OSC_PLL_WC_IRQn);
      NVIC_SetPriority(OSC_PLL_WC_IRQn, L3_DEFAULT_INTERRUPT_LEVEL); 
    }
  }
  
  return Ok;
} // Clk_SetIrq

//@} // ClkGroup

#endif // #if (defined(L3_PERIPHERAL_CLK_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
