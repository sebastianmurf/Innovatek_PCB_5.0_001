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
/** \file wdg.h
 **
 ** Headerfile for WDG functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/

#ifndef __WDG_H__
#define __WDG_H__

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
 ** \defgroup WdgGroup Watchdog (WDG)
 **
 ** Provided functions of WDG module:
 ** 
 ** - Wdg_InitSWdg()
 ** - Wdg_FeedSwWdg()
 ** - Wdg_QuickFeedSwWdg()
 ** - Wdg_InitHwWdg()
 ** - Wdg_DisableHwWdg()
 ** - Wdg_FeedHwWdg()
 ** - Wdg_QuickFeedHwWdg()
 **
 ** Both the Hardware and Software Watchdog use the same configuration
 ** structure. Both have dedicated interrupt callback functions, in which the
 ** user has to feed the corresponding Watchdog.
 **
 ** For the Software Watchdog Wdg_InitSWdg() is used for setting the interval
 ** time. Wdg_FeedSwWdg() resets the Watchdog timer by a function call.
 ** Wdg_QuickFeedSwWdg() does the same, but the code is inline expanded for
 ** e.g. time-critical polling loops.
 **
 ** Wdg_InitHwWdg() sets the Hardware Watchdog interval.
 ** Wdg_DisableHwWdg() disables the Watchdog, if 2 magic words as parameters
 ** are used. Wdg_FeedHwWdg() and Wdg_QuickFeedHwWdg() do the same as their
 ** correspondig functions for the Software Watchdog, but here are two
 ** parameter needed, the 2nd one the inverted value of the 1st.
 **
 ** \note The Hardware Watchdog shares its interrupt vector with the NMI.
 **       Therefore use the stc_wdg_config_t#bTouchNVIC configuration whether
 **       to touch the NMI-HWWdg-NVIC registers in Wdg_InitHwWdg()/
 **       Wdg_DisableHwWdg() or not.
 ******************************************************************************/
//@{

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/**
 *****************************************************************************
 ** \brief Software and Hardware Watchdog configuration
 *****************************************************************************/
typedef struct stc_wdg_config
{
  uint32_t   u32LoadValue;     ///< Timer interval
  boolean_t  bResetEnable;     ///< TRUE:  Enables SW watchdog reset
  boolean_t  bIrqEnable;       ///< TRUE:  Enables SW wathcdog interrupt
  boolean_t  bTouchNVIC;       ///< (Only valid for HW Watchdog)
                               ///< TRUE:  Enable NVIC interrupts in init function,
                               ///<        disable NVIC interrupts in de-init function.    
                               ///< FALSE: Do not access NVIC registers in init
                               ///<        or de-init function
  func_ptr_t pfnCallback;      ///< Interrupt callback function pointer
} stc_wdg_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern func_ptr_t pfnWdgHwCallback;
extern func_ptr_t pfnWdgSwCallback;

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void WdgIrqHandlerSwWdg(void);

extern void WdgIrqHandlerHwWdg(void);

extern en_result_t Wdg_InitSwWdg(stc_wdg_config_t* pstcConfig) ;

extern void Wdg_FeedSwWdg(void) ;

extern void Wdg_QuickFeedSwWdg(void) ;

extern en_result_t Wdg_InitHwWdg(stc_wdg_config_t* pstcConfig) ;

extern en_result_t Wdg_DisableHwWdg(stc_wdg_config_t* pstcConfig,
                                    uint32_t u32MagicWord1,
                                    uint32_t u32MagicWord2
                                   ) ;

extern void Wdg_FeedHwWdg(uint8_t u8ClearPattern1,
                          uint8_t u8ClearPattern2
                         ) ;

extern void Wdg_QuickFeedHwWdg(uint8_t u8ClearPattern1,
                                      uint8_t u8ClearPattern2
                                     ) ;

//@} // WdgGroup

#ifdef __cplusplus
}
#endif

#endif /* __WDG_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
