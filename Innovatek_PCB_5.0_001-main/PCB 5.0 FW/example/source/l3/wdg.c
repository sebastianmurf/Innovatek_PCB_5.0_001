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
/** \file wdg.c
 **
 ** A detailed description is available at 
 ** @link WdgGroup Watchdog Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "wdg.h"

#if (defined(L3_PERIPHERAL_WDG_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup ExtifGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

func_ptr_t pfnWdgHwCallback; ///< callback function pointer for HW-Wdg Irq
func_ptr_t pfnWdgSwCallback; ///< callback function pointer for SW-Wdg Irq

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
 ** \brief Software Watchdog Interrupt Handler
 ******************************************************************************/
void WdgIrqHandlerSwWdg(void)
{
  if (FM3_SWWDT->WDOGRIS_f.RIS == 1)
  {
    pfnWdgSwCallback();
  }
} // WdgIrqHandlerSwWdg

/**
 ******************************************************************************
 ** \brief Hardware Watchdog Interrupt Handler
 ******************************************************************************/
void WdgIrqHandlerHwWdg(void)
{
  // RIS flag check is done in interrupts.c
  pfnWdgHwCallback();
} // WdgIrqHandlerHwWdg

/**
 ******************************************************************************
 ** \brief Setup Software Watchdog
 **
 ** \param [in] pstcConfig   Pointer to Software Watchdog configuration
 **
 ** \retval Ok                      Setup successful
 ** \retval ErrorInvalidParameter   pstcConfig == NULL
 **
 ** \note The Softtware Watchdog is only activated, if the configuration
 **       stc_wdg_sw_config_t#bIrqEnable is TRUE!
 ******************************************************************************/
en_result_t Wdg_InitSwWdg(stc_wdg_config_t* pstcConfig)
{
  uint8_t u8WdogControl = 0;  // Preset register
  
  // Check for NULL Pointer
  if (pstcConfig == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  FM3_SWWDT->WDOGLOAD = pstcConfig->u32LoadValue; // Setup interval
  
  if (pstcConfig->bResetEnable == TRUE)
  {
    u8WdogControl |= 0x02;    // RESEN bit
  }
  
  if (pstcConfig->bIrqEnable == TRUE)
  {
    u8WdogControl |= 0x01;    // INTEN bit
  }
  
  FM3_SWWDT->WDOGCONTROL = u8WdogControl; // Setup SW-Watchdog and start
  FM3_SWWDT->WDOGLOCK = 0;                // Lock it!
  
  // Set callback pointer to global variable for SW-Wdg-ISR
  pfnWdgSwCallback = pstcConfig->pfnCallback;
  
  if (pstcConfig->bIrqEnable == TRUE)
  {  
    NVIC_ClearPendingIRQ(SWDT_IRQn);
    NVIC_EnableIRQ(SWDT_IRQn);
    NVIC_SetPriority(SWDT_IRQn, L3_IRQ_LEVEL_SWWDG);
  }
  
  return Ok;  
} // Wdg_InitSwWdg

/**
 ******************************************************************************
 ** \brief Feed Software Watchdog (Call function)
 **
 ** This function feeds the Software Watchdog with the unlock, feed, and lock
 ** sequence.
 ******************************************************************************/
void Wdg_FeedSwWdg(void)
{
  FM3_SWWDT->WDOGLOCK = 0x1ACCE551; // Release Lock
  FM3_SWWDT->WDOGINTCLR = 1;        // Clear possible interrupt and reload value
  FM3_SWWDT->WDOGLOCK = 0;          // Lock again it!
} // Wdg_FeedSwWdg

/**
 ******************************************************************************
 ** \brief Quickly feed Software Watchdog (inline function)
 **
 ** This function feeds the Software Watchdog with the unlock, feed, and lock
 ** sequence as an inline function for quick execution in polling loops.
 ******************************************************************************/
void Wdg_QuickFeedSwWdg(void)
{
  FM3_SWWDT->WDOGLOCK = 0x1ACCE551; // Release Lock
  FM3_SWWDT->WDOGINTCLR = 1;        // Clear possible interrupt and reload value
  FM3_SWWDT->WDOGLOCK = 0;          // Lock again it!
} // Wdg_QuickFeedSwWdg


/**
 ******************************************************************************
 ** \brief Setup Hardware Watchdog
 **
 ** \param [in] pstcConfig   Pointer to Hardware Watchdog configuration
 **
 ** \retval Ok                      Setup successful
 ** \retval ErrorInvalidParameter   pstcConfig == NULL
 **
 ** \note The Hardware Watchdog shares its interrupt vector with the NMI.
 **       Therefore use the stc_wdg_config_t#bTouchNVIC configuration whether
 **       to touch the NMI-HWWdg-NVIC registers or not.
 ******************************************************************************/
en_result_t Wdg_InitHwWdg(stc_wdg_config_t* pstcConfig)
{
  uint8_t u8WdogControl = 0;  // Preset register
  
  // Check for NULL Pointer
  if (pstcConfig == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  FM3_HWWDT->WDG_LCK = 0x1ACCE551; // Release Lock
  FM3_HWWDT->WDG_LDR = pstcConfig->u32LoadValue; 
  
  if (pstcConfig->bResetEnable == TRUE)
  {
    u8WdogControl |= 0x02;    // RESEN bit
  }
  
  if (pstcConfig->bIrqEnable == TRUE)
  {
    u8WdogControl |= 0x01;    // INTEN bit
  }
  
  FM3_HWWDT->WDG_LCK = 0x1ACCE551;    // HW Watchdog Control Register unlock sequence
  FM3_HWWDT->WDG_LCK = 0xE5331AAE;  
  FM3_HWWDT->WDG_CTL = u8WdogControl; // Setup HW-Watchdog and start
  
  // Set callback pointer to global variable for SW-Wdg-ISR
  pfnWdgHwCallback = pstcConfig->pfnCallback;
  
  if ((pstcConfig->bIrqEnable == TRUE) &&
      (pstcConfig->bTouchNVIC == TRUE)
     )
  {
    NVIC_ClearPendingIRQ(NMI_IRQn);
    NVIC_EnableIRQ(NMI_IRQn);
    NVIC_SetPriority(NMI_IRQn, L3_IRQ_LEVEL_HWWDG);
  }
  
  return Ok;  
} // Wdg_InitHwWdg


/**
 ******************************************************************************
 ** \brief Disable Hardware Watchdog
 **
 ** This function disables the Hardware Watchdog, when the first argument is
 ** 0xC72E51A3 and the second agrument 0x89DB2E43. The magic words are together 
 ** 64-bit wide and have a balanced entropy (32 zero and 32 one bits).
 **
 ** \param [in] pstcConfig      Pointer to Hardware Watchdog configuration
 ** \param [in] u32MagicWord1   1st Magic word for disabling (0xC72E51A3)
 ** \param [in] u32MagicWord2   2nd Magic word for disabling (0x89DB2E43)
 **
 ** \retval Ok                      disable sucessful
 ** \retval ErrorInvalidParameter   not diabled => magic word wrong!
 **
 ** \note The Hardware Watchdog shares its interrupt vector with the NMI.
 **       Therefore use the stc_wdg_config_t#bTouchNVIC configuration whether
 **       to touch the NMI-HWWdg-NVIC registers or not.
 ******************************************************************************/
en_result_t Wdg_DisableHwWdg(stc_wdg_config_t* pstcConfig,
                             uint32_t u32MagicWord1,
                             uint32_t u32MagicWord2)
{
  // Inverted magic word check is done to avoid "plain text magic word" in ROM.
  if (((u32MagicWord1 ^ 0x38D1AE5C) != 0xFFFFFFFF) ||
      ((u32MagicWord2 ^ 0x7624D1BC) != 0xFFFFFFFF)
     )
  {
    return ErrorInvalidParameter;
  }
  
  if (pstcConfig->bTouchNVIC == TRUE)
  {
    NVIC_ClearPendingIRQ(NMI_IRQn);
    NVIC_DisableIRQ(NMI_IRQn);
    NVIC_SetPriority(NMI_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  
  FM3_HWWDT->WDG_LCK = 0x1ACCE551; // HW Watchdog Control Register unlock sequence
  FM3_HWWDT->WDG_LCK = 0xE5331AAE;
  FM3_HWWDT->WDG_CTL = 0;          // HW Watchdog stop
  
  return Ok;
} // Wdg_DisableHwWdg

/**
 ******************************************************************************
 ** \brief Feed Hardware Watchdog (Call function)
 **
 ** This function feeds the Hardware Watchdog with the unlock, feed, and lock
 ** sequence. Take care of the arbitrary values, because there are not checked
 ** for plausibility!
 **
 ** \param [in] u8ClearPattern1   Pattern of arbitrary value
 ** \param [in] u8ClearPattern2   Inverted arbitrary value
 ******************************************************************************/
void Wdg_FeedHwWdg(uint8_t u8ClearPattern1, uint8_t u8ClearPattern2)
{
  FM3_HWWDT->WDG_LCK = 0x1ACCE551;      // Release Lock
  FM3_HWWDT->WDG_ICL = u8ClearPattern1; // Clear possible interrupt and reload value, step 1
  FM3_HWWDT->WDG_ICL = u8ClearPattern2; // Clear possible interrupt and reload value, step 2
} // Wdg_FeedHwWdg

/**
 ******************************************************************************
 ** \brief Quickly feed Hardware Watchdog (inline function)
 **
 ** This function feeds the Hardware Watchdog with the unlock, feed, and lock
 ** sequence as an inline function for quick execution in polling loops.
 ** Take care of the arbitrary values, because there are not checked for
 ** plausibility!
 **
 ** \param [in] u8ClearPattern1   Pattern of arbitrary value
 ** \param [in] u8ClearPattern2   Inverted arbitrary value
 ******************************************************************************/
void Wdg_QuickFeedHwWdg(uint8_t u8ClearPattern1, uint8_t u8ClearPattern2)
{
  FM3_HWWDT->WDG_LCK = 0x1ACCE551;      // Release Lock
  FM3_HWWDT->WDG_ICL = u8ClearPattern1; // Clear possible interrupt and reload value, step 1
  FM3_HWWDT->WDG_ICL = u8ClearPattern2; // Clear possible interrupt and reload value, step 2
} // Wdg_QuickFeedHwWdg


//@} // WdgGroup

#endif // #if (defined(L3_PERIPHERAL_WDG_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
