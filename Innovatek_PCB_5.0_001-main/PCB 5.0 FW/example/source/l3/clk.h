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
/** \file clk.h
 **
 ** Headerfile for CLK functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-06-25  1.1  MWi  Deep Standby driver note added; DSM enumerations
 **                           moved from en_clk_mode_t to en_dsm_mode_t, Device
 **                           types 6 and 7 added
 **
 ******************************************************************************/

#ifndef __CLK_H__
#define __CLK_H__

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
 ** \defgroup ClkGroup Clock Functions (CLK)
 **
 ** Provided functions of CLK module:
 ** - Clk_GetParameters()
 ** - Clk_SetDividers()
 ** - Clk_SetStabilizationWaitTimes()
 ** - Clk_WaitForMainOscillator()
 ** - Clk_WaitForSubOscillator()
 ** - Clk_WaitForPllOscillator()
 ** - Clk_MainOscillatorReady()
 ** - Clk_SubOscillatorReady()
 ** - Clk_PllOscillatorReady()
 ** - Clk_SetSource()
 ** - Clk_SetPllSource()
 ** - Clk_SetMode()
 ** - Clk_EnableSubClock()
 ** - Clk_DisableSubClock()
 ** - Clk_EnableMainClock()
 ** - Clk_DisableMainClock()
 ** - Clk_WaitForClockSourceReady()
 ** - Clk_SetIrq()
 **
 ** \brief How to use the CLK module
 **
 ** This driver manages the FM3 clock settings for:
 ** - Clock Source (Main, Sub, HS-CR, LS-CR, PLL)
 ** - Clock Mode (Run, Sleep, Timer, Stop)
 ** - Bus dividers (HCLK, PCLK0, PCLK1, PCLK2)
 ** - Stabilization Wait Times (Main, Sub, PLL)
 **
 ** Also polling routines for the oscillation stabilization wait are provided,
 ** e.g. Clk_WaitForMainOscillator().
 ** If this waiting/polling is not wanted in the user application, there are
 ** also function, which return the current state of the stability of a clock,
 ** e.g. Clk_MainOscillatorReady().
 **
 ** For Clock Mode transition a hook function is called (if specified) for
 ** making individual power saving settings between setting SLEEPDEEP and
 ** the final WFI instruction (Timer-, Sleep-, Stop modes).
 **
 ** Each wait function has at lest a parameter for time out counting. The
 ** value of this parameter depends on the user's wait time settings.
 ** Additionally these functions call the library's L3_WAIT_LOOP_HOOK()
 ** function.
 **
 ** \note 
 **       - After each Clock Source change Clk_WaitForClockSourceReady() with
 **         the clock source enumerator #en_clk_source_t and a time out count
 **         value as parameters has to be called, if a previous waiting for
 **         stabilization was not performed.
 **       - If the used device supports RTC and the RTC low power modes should
 **         be used, switch L3_RTC_AVAILABLE to L3_ON in l3_user.h.
 **       - The configuration en_clk_pll_src_t#PllSrcClkHc and the function
 **         Clk_SetPllSource() can only be used, if the used device supports
 **         HS-RC-Clock as PLL input clock source! This driver does not check
 **         for availability!
 **       - Because the Clock interrupts are shared with other interrupts (e.g.
 **         Watch Counter), Clk_SetIrq() has an argument, whether to touch
 **         the NVIC registers or not to prevent interference with other 
 **         driver parts.
 **       - Deep Standby Modes are only available in DSM driver module
 **
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/

#if   (L3_DEVICE_TYPE == L3_TYPE0)
  #define PLLK_MAX  4 // Setting according Peripheral Manual
  #define PLLN_MAX 20 // Setting according Peripheral Manual
  #define PLLM_MAX  2 // Setting according Peripheral Manual
  
#elif (L3_DEVICE_TYPE == L3_TYPE1) || \
      (L3_DEVICE_TYPE == L3_TYPE5)
  #define PLLK_MAX  4 // Setting according Peripheral Manual
  #define PLLN_MAX 10 // Setting according Peripheral Manual
  #define PLLM_MAX  7 // Setting according Peripheral Manual
  
#elif (L3_DEVICE_TYPE == L3_TYPE2) || \
      (L3_DEVICE_TYPE == L3_TYPE4)
  #define PLLK_MAX  4 // Setting according Peripheral Manual
  #define PLLN_MAX 36 // Setting according Peripheral Manual
  #define PLLM_MAX  2 // Setting according Peripheral Manual
  
#elif (L3_DEVICE_TYPE == L3_TYPE3) || \
      (L3_DEVICE_TYPE == L3_TYPE7)
  #define PLLK_MAX  2 // Setting according Peripheral Manual
  #define PLLN_MAX  5 // Setting according Peripheral Manual
  #define PLLM_MAX  1 // Setting according Peripheral Manual

#elif (L3_DEVICE_TYPE == L3_TYPE6)
  #define PLLK_MAX  4 // Setting according Peripheral Manual
  #define PLLN_MAX 10 // Setting according Peripheral Manual
  #define PLLM_MAX  3 // Setting according Peripheral Manual
  
#else
  #error Device type not defined!
#endif
  
#define BSC_PSR_MAX       0x06
#define APBC0_PSR_MAX     0x03 
#define APBC1_PSR_MAX     0x03 
#define APBC2_PSR_MAX     0x03 
#define CSW_TMR_SOWT_MAX  0x07 
#define CSW_TMR_MOWT_MAX  0x0F 
#define PSW_TMR_POWT_MAX  0x07 
#define PLL_CTL1_PLLK_MAX PLLK_MAX
#define PLL_CTL1_PLLM_MAX PLLM_MAX
#define PLL_CTL2_PLLN_MAX PLLN_MAX

// Driver's own SCB_SCR definition to stay compatible for IAR and KEIL!
#define	CLK_SCB_SCR *(volatile uint32_t*) 0xE000ED10
#define CLK_SCB_SCR_SLEEPDEEP_Position  2                                   // SCB SCR: SLEEPDEEP Position
#define CLK_SCB_SCR_SLEEPDEEP_Mask      (1UL << CLK_SCB_SCR_SLEEPDEEP_Position) // SCB SCR: SLEEPDEEP Mask
  
// Driver's own PMD_CTL address definition
#define FM3_PMD_CTL *(volatile uint8_t*) 0x40035800
  
// Deep Standby Mode Select (DSTM) Bit Position
#define FM3_STB_CTL_DSTM_BITPOS  0x00000004
  
// PLL Input Clock Select (PINC) Bit Position
#define FM3_PSW_TMR_PINC_BITPOS 0x10
  
// Interrupt enable bitpositions in INT_ENR
#define FM3_INT_ENR_PCSE_BITPOS 0x04
#define FM3_INT_ENR_SCSE_BITPOS 0x02
#define FM3_INT_ENR_MCSE_BITPOS 0x01
  
// Interrupt status bitpositions in INT_STR
#define FM3_INT_STR_PCSI_BITPOS 0x04
#define FM3_INT_STR_SCSI_BITPOS 0x02
#define FM3_INT_STR_MCSI_BITPOS 0x01
  
// Interrupt ckear bitpositions in INT_CLR
#define FM3_INT_CLR_PCSC_BITPOS 0x04
#define FM3_INT_CLR_SCSC_BITPOS 0x02
#define FM3_INT_CLR_MCSC_BITPOS 0x01
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/**
 ******************************************************************************
  ** \brief Redefinition of CLK SCM_CTL register
 ******************************************************************************/  
typedef struct stc_clk_scm_ctl_field
{
  uint32_t RESERVED1  : 1;
  uint32_t MOSCE      : 1;
  uint32_t RESERVED2  : 1;
  uint32_t SOSCE      : 1;
  uint32_t PLLE       : 1;
  uint32_t RCS        : 3;
} stc_clk_scm_ctl_field_t;

typedef struct{
  union{
    uint8_t                 u8SCM_CTL;
    stc_clk_scm_ctl_field_t SCM_CTL_f;
  };
} stcSCM_CTL_t;

/**
 ******************************************************************************
 ** \brief Clock Source
 ** 
 ** Differentiator for the different clock sources
 **
 ** \note The enumerated values do not correspond to the RCS/RCM bits of the
 **       clock control and status registers due to having upward compatibility,
 **       if this bit coding may change in future devices. The correct bit
 **       patterns are set by switch(en_clk_source)/case statements individually
 **       in the corresponding function(s).
 ******************************************************************************/
typedef enum en_clk_source
{
  ClkMain = 0,  ///< Main Clock Oscillator
  ClkSub  = 1,  ///< Sub Clock Oscillator
  ClkHsCr = 2,  ///< High-Speed CR Clock Oscillator
  ClkLsCr = 3,  ///< Low-Speed CR Clock Oscillator
  ClkPll  = 4   ///< PLL Clock
} en_clk_source_t;

/**
 ******************************************************************************
 ** \brief Clock Mode
 ** 
 ** Differentiator for the different clock modes
 **
 ** \note
 ** This configuration uses together the STB_CTL register settings like in
 ** the DSM module.
 ******************************************************************************/
typedef enum en_clk_mode
{
  ClkRun      = 0,  ///< Run modes
  ClkSleep    = 1,  ///< Sleep modes
  ClkTimer    = 2,  ///< Timer modes
  ClkStop     = 3,  ///< Stop mode
  ClkRtc      = 4   ///< RTC mode (not implemented yet!)
  /// Deep Standby modes are defined in #en_dsm_mode_t
} en_clk_mode_t;

/**
 ******************************************************************************
 ** \brief Base Clock Prescaler Settings
 ** 
 ** Enumeration of the dividers of the Base Clock (HCLK)
 ******************************************************************************/
typedef enum en_clk_baseclkdiv
{
  BaseClkDiv1  = 0,  ///< HCLK Division 1/1
  BaseClkDiv2  = 1,  ///< HCLK Division 1/2
  BaseClkDiv3  = 2,  ///< HCLK Division 1/3
  BaseClkDiv4  = 3,  ///< HCLK Division 1/4
  BaseClkDiv6  = 4,  ///< HCLK Division 1/6
  BaseClkDiv8  = 5,  ///< HCLK Division 1/8    
  BaseClkDiv16 = 6   ///< HCLK Division 1/16  
} en_clk_baseclkdiv_t;

/**
 ******************************************************************************
 ** \brief APB0 Prescaler Settings
 ** 
 ** Enumeration of the dividers of the APB0 (PCLK0)
 ******************************************************************************/
typedef enum en_clk_apb0div
{
  Apb0Div1  = 0,  ///< PCLK0 Division 1/1
  Apb0Div2  = 1,  ///< PCLK0 Division 1/2
  Apb0Div4  = 2,  ///< PCLK0 Division 1/4
  Apb0Div8  = 3   ///< PCLK0 Division 1/8
} en_clk_apb0div_t;

/**
 ******************************************************************************
 ** \brief APB1 Prescaler Settings
 ** 
 ** Enumeration of the dividers of the APB0 (PCLK1)
 ******************************************************************************/
typedef enum en_clk_apb1div
{
  Apb1Div1  = 0,  ///< PCLK1 Division 1/1
  Apb1Div2  = 1,  ///< PCLK1 Division 1/2
  Apb1Div4  = 2,  ///< PCLK1 Division 1/4
  Apb1Div8  = 3   ///< PCLK1 Division 1/8
} en_clk_apb1div_t;

/**
 ******************************************************************************
 ** \brief APB2 Prescaler Settings
 ** 
 ** Enumeration of the dividers of the APB0 (PCLK2)
 ******************************************************************************/
typedef enum en_clk_apb2div
{
  Apb2Div1  = 0,  ///< PCLK2 Division 1/1
  Apb2Div2  = 1,  ///< PCLK2 Division 1/2
  Apb2Div4  = 2,  ///< PCLK2 Division 1/4
  Apb2Div8  = 3   ///< PCLK2 Division 1/8
} en_clk_apb2div_t;

/**
 ******************************************************************************
 ** \brief Sub Clock oscillation stablilization wait time
 ** 
 ** Enumeration for the Sub Clock oscillation stabilization wait time settings
 ******************************************************************************/
typedef enum en_clk_scowaittime
{
  ScoWaitExp10  = 0,  ///< 2^10 / F(CL) => ~10.3 ms
  ScoWaitExp11  = 1,  ///< 2^11 / F(CL) => ~20.5 ms
  ScoWaitExp12  = 2,  ///< 2^12 / F(CL) => ~41   ms
  ScoWaitExp13  = 3,  ///< 2^13 / F(CL) => ~82   ms
  ScoWaitExp14  = 4,  ///< 2^14 / F(CL) => ~164  ms
  ScoWaitExp15  = 5,  ///< 2^15 / F(CL) => ~327  ms
  ScoWaitExp16  = 6,  ///< 2^16 / F(CL) => ~655  ms
  ScoWaitExp17  = 7   ///< 2^17 / F(CL) => ~1.31 s      
} en_clk_scowaittime_t;

/**
 ******************************************************************************
 ** \brief Main Clock oscillation stablilization wait time
 ** 
 ** Enumeration for the Main Clock oscillation stabilization wait time settings
 ******************************************************************************/
typedef enum en_clk_mcowaittime
{
  McoWaitExp11   = 0,   ///< 2^1  / F(CH) => ~500  ns (F(CH) = 4 MHz)
  McoWaitExp15   = 1,   ///< 2^5  / F(CH) => ~8    us (F(CH) = 4 MHz)
  McoWaitExp16   = 2,   ///< 2^6  / F(CH) => ~16   us (F(CH) = 4 MHz)
  McoWaitExp17   = 3,   ///< 2^7  / F(CH) => ~32   us (F(CH) = 4 MHz)
  McoWaitExp18   = 4,   ///< 2^8  / F(CH) => ~64   us (F(CH) = 4 MHz)
  McoWaitExp19   = 5,   ///< 2^9  / F(CH) => ~128  us (F(CH) = 4 MHz)
  McoWaitExp110  = 6,   ///< 2^10 / F(CH) => ~256  us (F(CH) = 4 MHz)
  McoWaitExp111  = 7,   ///< 2^11 / F(CH) => ~512  us (F(CH) = 4 MHz)
  McoWaitExp112  = 8,   ///< 2^12 / F(CH) => ~1.0  ms (F(CH) = 4 MHz)
  McoWaitExp113  = 9,   ///< 2^13 / F(CH) => ~2.0  ms (F(CH) = 4 MHz)
  McoWaitExp114  = 10,  ///< 2^14 / F(CH) => ~4.0  ms (F(CH) = 4 MHz)
  McoWaitExp115  = 11,  ///< 2^15 / F(CH) => ~8.0  ms (F(CH) = 4 MHz)
  McoWaitExp117  = 12,  ///< 2^17 / F(CH) => ~33.0 ms (F(CH) = 4 MHz)
  McoWaitExp119  = 13,  ///< 2^18 / F(CH) => ~131  ms (F(CH) = 4 MHz)
  McoWaitExp121  = 14,  ///< 2^21 / F(CH) => ~524  ms (F(CH) = 4 MHz)
  McoWaitExp123  = 15   ///< 2^23 / F(CH) => ~2.0  s  (F(CH) = 4 MHz)
} en_clk_mcowaittime_t;

/**
 ******************************************************************************
 ** \brief PLL Clock oscillation stablilization wait time
 ** 
 ** Enumeration for the PLL Clock oscillation stabilization wait time settings
 ******************************************************************************/
typedef enum en_clk_pllowaittime
{
  PlloWaitExp19   = 0,   ///< 2^9  / F(CH) => ~128  us (F(CH) = 4 MHz)
  PlloWaitExp110  = 1,   ///< 2^10 / F(CH) => ~256  us (F(CH) = 4 MHz)
  PlloWaitExp111  = 2,   ///< 2^11 / F(CH) => ~512  us (F(CH) = 4 MHz)
  PlloWaitExp112  = 3,   ///< 2^12 / F(CH) => ~1.02 ms (F(CH) = 4 MHz)
  PlloWaitExp113  = 4,   ///< 2^13 / F(CH) => ~2.05 ms (F(CH) = 4 MHz)
  PlloWaitExp114  = 5,   ///< 2^14 / F(CH) => ~4.10 ms (F(CH) = 4 MHz)
  PlloWaitExp115  = 6,   ///< 2^15 / F(CH) => ~8.20 ms (F(CH) = 4 MHz)
  PlloWaitExp116  = 7    ///< 2^16 / F(CH) => ~16.4 ms (F(CH) = 4 MHz)
} en_clk_pllowaittime_t;

/**
 ******************************************************************************
 ** \brief PLL SourceClock (PINC bit of PSW_TMR)
 ** 
 ** Enumeration for the PLL Clock Source.
 **
 ** \attention HS-RC source only available, if used device supports PLL-CLKHC
 **            setting! PINC bit availability is <b>not</b> checked in this
 **            driver!
 ******************************************************************************/
typedef enum en_clk_pll_src
{
  PllSrcClkMo = 0,   ///< Use Main Clock as PLL source (always available, default)
  PllSrcClkHc = 123  ///< Use HS-RC Clock as PLL source (only if available!)
} en_clk_pll_src_t;

/**
 ******************************************************************************
 ** \brief Datatype for holding internal data needed for CLK
 ******************************************************************************/
typedef struct stc_clk_intern_data
{
  func_ptr_t            pfnPllStabCb;     ///< PLL stabilization callback function pointer
  func_ptr_t            pfnScoStabCb;     ///< Sub Clock stabilization callback function pointer
  func_ptr_t            pfnMcoStabCb;     ///< Main Clock stabilization callback function pointer
} stc_clk_intern_data_t ;

/**
 ******************************************************************************
 ** \brief Clock configuration
 ** 
 ** The Clock configuration settings
 ******************************************************************************/
typedef struct stc_clk_config
{
  en_clk_source_t       enSource;         ///< See description of #en_clk_source_t
  boolean_t             bEnablePll;       ///< PLL enable
  boolean_t             bEnableMainClock; ///< Main Clock enable 
  boolean_t             bEnableSubClock;  ///< Sub Clock enable  
  en_clk_mode_t         enMode;           ///< See description of #en_clk_mode_t
  boolean_t             bLpmPortHiZState; ///< TRUE: Sets the status of each external port pin
                                          ///        to high impedance (Hi-Z) in Timer or Stop mode
                                          ///< FALSE: Holds the status of each external port pin
  en_clk_baseclkdiv_t   enBaseClkDiv;     ///< See description of #en_clk_baseclkdiv_t
  en_clk_apb0div_t      enAPB0Div;        ///< See description of #en_clk_apb0div_t
  en_clk_apb1div_t      enAPB1Div;        ///< See description of #en_clk_apb1div_t
  en_clk_apb2div_t      enAPB2Div;        ///< See description of #en_clk_apb2div_t
  boolean_t             bAPB1Disable;     ///< TRUE: Disables APB1 regardless of divider settings
  boolean_t             bAPB2Disable;     ///< TRUE: Disables APB2 regardless of divider settings 
  en_clk_scowaittime_t  enSCOWaitTime;    ///< See description of #en_clk_scowaittime_t
  en_clk_mcowaittime_t  enMCOWaitTime;    ///< See description of #en_clk_mcowaittime_t
  en_clk_pllowaittime_t enPLLOWaitTime;   ///< See description of #en_clk_pllowaittime_t
  uint8_t               u8PllK;           ///< PLL input clock frequency division ratio, PLLK
  uint8_t               u8PllM;           ///< PLL VCO clock frequency division ratio, PLLM
  uint8_t               u8PllN;           ///< PLL feedback frequency division ration, PLLN
  en_clk_pll_src_t      enPllSource;      ///< Only available, if HC clock as PLL clock source supported! See #en_clk_pll_src_t for details!
  boolean_t             bPllIrq;          ///< TRUE: enable PLL oscillation stabilization completion interrupt
  boolean_t             bScoIrq;          ///< TRUE: enable Sub Clock oscillation stabilization completion interrupt
  boolean_t             bMcoIrq;          ///< TRUE: enable Main Clock oscillation stabilization completion interrupt
  func_ptr_t            pfnPllStabCb;     ///< PLL stabilization callback function pointer
  func_ptr_t            pfnScoStabCb;     ///< Sub Clock stabilization callback function pointer
  func_ptr_t            pfnMcoStabCb;     ///< Main Clock stabilization callback function pointer
  func_ptr_t            pfnHook;          ///< Hook call for low power modes (called between SLEEPDEEP = 1 and WFI)      
} stc_clk_config_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern stc_clk_intern_data_t stcClkInternData;

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

extern void ClkIrqHandler(void) ;

en_result_t Clk_GetParameters(stc_clk_config_t* pstcConfig) ;

en_result_t Clk_SetDividers(stc_clk_config_t* pstcConfig) ;

en_result_t Clk_SetStabilizationWaitTimes(stc_clk_config_t* pstcConfig) ;

en_result_t Clk_WaitForMainOscillator(uint32_t u32MaxTimeOut) ;

en_result_t Clk_WaitForSubOscillator(uint32_t u32MaxTimeOut) ;

en_result_t Clk_WaitForPllOscillator(uint32_t u32MaxTimeOut) ;

en_result_t Clk_MainOscillatorReady(void) ;

en_result_t Clk_SubOscillatorReady(void) ;

en_result_t Clk_PllOscillatorReady(void) ;

en_result_t Clk_SetSource(stc_clk_config_t* pstcConfig) ;

en_result_t Clk_SetPllSource(stc_clk_config_t* pstcConfig) ;

en_result_t Clk_SetMode(stc_clk_config_t* pstcConfig) ;

en_result_t Clk_EnableSubClock(void) ;

en_result_t Clk_DisableSubClock(void) ;

en_result_t Clk_EnableMainClock(void) ;

en_result_t Clk_DisableMainClock(void) ;

en_result_t Clk_WaitForClockSourceReady(en_clk_source_t enSource,
                                        uint32_t        u32MaxTimeOut) ;

en_result_t Clk_SetIrq( stc_clk_config_t* pstcConfig,
                        boolean_t         bTouchNvic
                      ) ;

//@} // ClkGroup

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
