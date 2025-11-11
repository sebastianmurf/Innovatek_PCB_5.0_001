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
/** \file dsm.h
 **
 ** Headerfile for DSM functions
 **  
 **
 ** History:
 **   - 2012-07-06  1.0  MWi  First version.
 **
 ******************************************************************************/

#ifndef __DSM_H__
#define __DSM_H__

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
 ** \defgroup DsmGroup Deep Standby Mode Functions (DSM)
 **
 ** Provided functions of DSM module:
 ** 
 ** - Dsm_SetMode()
 ** - Dsm_GetReturnCause()
 **
 ** Dsm_SetMode() sets the deep standby mode according to the configuration
 ** defined in a struct type #stc_dsm_config_t.
 ** The returen cause can be retrieved by calling Dsm_GetReturnCause(). The
 ** return cause is stored in a pointered structure.
 **
 ** For 8-, 16-, and 32-Bit access to the Backup registers, special pointer
 ** definitions are defined.
 **
 ** \note
 ** The DSM driver does not handle wake-up pins, which may shared with analog
 ** input pins. The user has to take care of correct port settings (ADE to '0',
 ** if wake-up input functionality shall be used).
 **
 ** \attention
 ** Consider that WKUP0 pin is always active (low level) for DS modes, so that
 ** it has to be pulled up by internal or external resisitor! Refere to the
 ** pin assignment of your device for setting corresponding PCR pull-up
 ** register.
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief Dsmcallback function prototypes.
 ******************************************************************************/

/******************************************************************************
 * Global definitions
 ******************************************************************************/
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
#define DSM_CRG (((volatile stc_dsm_crg_t *) (L3_CRG_BASE + 0x8UL)))
#define DSM     (((volatile stc_dsm_t *)      L3_DSM_BASE))
  
// Driver's own SCB_SCR definition to stay compatible for IAR and KEIL!
#define	DSM_SCB_SCR *(volatile uint32_t*) 0xE000ED10
#define DSM_SCB_SCR_SLEEPDEEP_BITPOSITION 4           // SCB SCR: SLEEPDEEP Position

  
// Backup Register pointer types for 8-, 16-, and 32-bit access
// 8-Bit
#define u8DSM_BUR01 (*(volatile uint8_t*) 0x40035900UL)
#define u8DSM_BUR02 (*(volatile uint8_t*) 0x40035901UL)
#define u8DSM_BUR03 (*(volatile uint8_t*) 0x40035902UL)
#define u8DSM_BUR04 (*(volatile uint8_t*) 0x40035903UL)
#define u8DSM_BUR05 (*(volatile uint8_t*) 0x40035904UL)
#define u8DSM_BUR06 (*(volatile uint8_t*) 0x40035905UL)
#define u8DSM_BUR07 (*(volatile uint8_t*) 0x40035906UL)
#define u8DSM_BUR08 (*(volatile uint8_t*) 0x40035907UL)
#define u8DSM_BUR09 (*(volatile uint8_t*) 0x40035908UL)
#define u8DSM_BUR10 (*(volatile uint8_t*) 0x40035909UL)
#define u8DSM_BUR11 (*(volatile uint8_t*) 0x4003590AUL)
#define u8DSM_BUR12 (*(volatile uint8_t*) 0x4003590BUL)
#define u8DSM_BUR13 (*(volatile uint8_t*) 0x4003590CUL)
#define u8DSM_BUR14 (*(volatile uint8_t*) 0x4003590DUL)
#define u8DSM_BUR15 (*(volatile uint8_t*) 0x4003590EUL)
#define u8DSM_BUR16 (*(volatile uint8_t*) 0x4003590FUL)
  
// 16-Bit
#define u16DSM_BUR0102 (*(volatile uint16_t*) 0x40035900UL)
#define u16DSM_BUR0304 (*(volatile uint16_t*) 0x40035902UL)
#define u16DSM_BUR0506 (*(volatile uint16_t*) 0x40035904UL)
#define u16DSM_BUR0708 (*(volatile uint16_t*) 0x40035906UL)
#define u16DSM_BUR0910 (*(volatile uint16_t*) 0x40035908UL)
#define u16DSM_BUR1112 (*(volatile uint16_t*) 0x4003590AUL)
#define u16DSM_BUR1314 (*(volatile uint16_t*) 0x4003590CUL)
#define u16DSM_BUR1516 (*(volatile uint16_t*) 0x4003590EUL)

// 32-Bit
#define u32DSM_BUR01020304 (*(volatile uint32_t*) 0x40035900UL)
#define u32DSM_BUR05060708 (*(volatile uint32_t*) 0x40035904UL)
#define u32DSM_BUR09101112 (*(volatile uint32_t*) 0x40035908UL)
#define u32DSM_BUR13141516 (*(volatile uint32_t*) 0x4003590CUL)
  
/**
 ******************************************************************************
 ** \brief Redefinition of CLK STB_CTL register
 ******************************************************************************/  
typedef struct stc_dsm_stb_ctl_field
{
  __IO uint32_t STM        : 2;
  __IO uint32_t DSTM       : 1;
       uint32_t RESERVED1  : 1;
  __IO uint32_t SPL        : 1;
       uint32_t RESERVED2  : 11;
  __IO uint32_t KEY        : 16;
} stc_dsm_stb_ctl_field_t;

typedef struct{
  union{
    uint32_t                u32STB_CTL;
    stc_dsm_stb_ctl_field_t STB_CTL_f;
  };
} stc_dsm_crg_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM REG_CTL register
 ******************************************************************************/ 
typedef struct stc_dsm_reg_ctl
{
       uint8_t RESERVED1 : 1;
  __IO uint8_t ISUBSEL   : 2;
       uint8_t RESERVED2 : 5;
} stc_dsm_reg_ctl_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM RCK_CTL register
 ******************************************************************************/ 
typedef struct stc_dsm_rck_ctl
{
  __IO uint8_t RTCCKE    : 1;
  __IO uint8_t CECCKE    : 1;
       uint8_t RESERVED1 : 6;
} stc_dsm_rck_ctl_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM PMD_CTL register
 ******************************************************************************/ 
typedef struct stc_dsm_pmd_ctl
{
  __IO uint8_t RTCE      : 1;
       uint8_t RESERVED1 : 7;
} stc_dsm_pmd_ctl_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM WRFSR register
 ******************************************************************************/ 
typedef struct stc_dsm_wrfsr
{
  __IO uint8_t WINITX    : 1;
  __IO uint8_t WLVDH     : 1;
       uint8_t RESERVED1 : 6;
} stc_dsm_wrfsr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM WIFSR register
 ******************************************************************************/ 
typedef struct stc_dsm_wifsr
{
  __IO uint16_t WRTCI     : 1;
  __IO uint16_t WLVDI     : 1;
  __IO uint16_t WUI0      : 1;
  __IO uint16_t WUI1      : 1;
  __IO uint16_t WUI2      : 1;
  __IO uint16_t WUI3      : 1;
  __IO uint16_t WUI4      : 1;
  __IO uint16_t WUI5      : 1;
  __IO uint16_t WCEC0I    : 1;
  __IO uint16_t WCEC1I    : 1; 
       uint16_t RESERVED1 : 6;
} stc_dsm_wifsr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM WIER register
 ******************************************************************************/ 
typedef struct stc_dsm_wier
{
  __IO uint16_t WRTCE     : 1;
  __IO uint16_t WLVDE     : 1;
       uint16_t RESERVED1 : 1;
  __IO uint16_t WUI1E     : 1;
  __IO uint16_t WUI2E     : 1;
  __IO uint16_t WUI3E     : 1;
  __IO uint16_t WUI4E     : 1;
  __IO uint16_t WUI5E     : 1;
  __IO uint16_t WCEC0E    : 1;
  __IO uint16_t WCEC1E    : 1; 
       uint16_t RESERVED2 : 6;
} stc_dsm_wier_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM WILVR register
 ******************************************************************************/ 
typedef struct stc_dsm_wilvr
{
  __IO uint8_t WUI1LV    : 1;
  __IO uint8_t WUI2LV    : 1;
  __IO uint8_t WUI3LV    : 1;
  __IO uint8_t WUI4LV    : 1;
  __IO uint8_t WUI5LV    : 1;
       uint8_t RESERVED1 : 3;
} stc_dsm_wilvr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM DSRAMR register
 ******************************************************************************/ 
typedef struct stc_dsm_dsramr
{
  __IO uint8_t SRAMR     : 2;
       uint8_t RESERVED1 : 6;
} stc_dsm_dsramr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DSM register set
 ******************************************************************************/
typedef struct stc_dsm
{
  union {
    __IO uint8_t           REG_CTL;
         stc_dsm_reg_ctl_t u8REG_CTL;
  };
  uint8_t                  Reserved0[3];
  union {
    __IO uint8_t           RCK_CTL;
         stc_dsm_rck_ctl_t u8RCK_CTL;
  };
  uint8_t                  Reserved1[3];
  uint32_t                 Reserved2[446];
  union {
    __IO uint8_t           PMD_CTL;
         stc_dsm_pmd_ctl_t u8PMD_CTL;
  };
  uint8_t                  Reserved3[3];
  union {
    __IO uint8_t           WRFSR;
         stc_dsm_wrfsr_t   u8WRFSR;
  };
  uint8_t                  Reserved4[3];
  union {
    __IO uint16_t          WIFSR;
         stc_dsm_wifsr_t   u16WIFSR;
  };
  uint16_t                 Reserved5;
  union {
    __IO uint16_t          WIER;
         stc_dsm_wier_t    u16WIER;
  };
  uint16_t                 Reserved6;
  union {
    __IO uint8_t           WILVR;
         stc_dsm_wilvr_t   u8WILVR;
  };
  uint8_t                  Reserved7[3];
  union {
    __IO uint8_t           DSRAMR;
         stc_dsm_dsramr_t  u8DSRAMR;
  };
} stc_dsm_t;

/**
 ******************************************************************************
 ** \brief Deep Standby Mode
 ** 
 ** Differentiator for the different Deep Standby modes.
 ** 
 ** \note
 ** This configuration uses together the STB_CTL register settings like in
 ** the CLK module.
 ******************************************************************************/
typedef enum en_dsm_mode
{
  /// None-Deep-Standby modes are defined in #en_clk_mode_t
  DeepStop = 0,  ///< Deep Standby Stop mode
  DeepRtc  = 1,  ///< Deep Standby RTC mode
} en_dsm_mode_t;

/**
 ******************************************************************************
 ** \brief HDMI-CEC return channel
 ** 
 ** Channel bitfields for return from deep stand by modes
 ******************************************************************************/
typedef struct stc_hdmi_channel
{
  uint8_t Channel0  : 1;
  uint8_t Channel1  : 1;
  uint8_t Reserved  : 6;
} stc_hdmi_channel_t;

/**
 ******************************************************************************
 ** \brief Wake-up pin return channel
 ** 
 ** Channel bitfields for return from deep stand by modes
 ******************************************************************************/
typedef struct stc_wkpin_channel
{
  uint8_t Channel0  : 1;  ///< This channel cannot be disabled by setting to '0'. The input level also cannot be set.
  uint8_t Channel1  : 1;
  uint8_t Channel2  : 1;
  uint8_t Channel3  : 1;
  uint8_t Channel4  : 1;
  uint8_t Channel5  : 1;
  uint8_t Reserved  : 2;
} stc_wkpin_channel_t;

/**
 ******************************************************************************
 ** \brief Deep Standby Mode Return structure
 ** 
 ** The DSM return cause structure
 ******************************************************************************/
typedef struct stc_dsm_return_cause
{
  uint16_t InitxReset     : 1;    ///< Return cause: Initx Reset
  uint16_t LvdReset       : 1;    ///< Return cause: LVD Reset
  uint16_t LvdInterrupt   : 1;    ///< Return cause: LVD Interrupt
  uint16_t RtcInterrupt   : 1;    ///< Return cause: RTC Interrupt
  uint16_t Hdmi0Interrupt : 1;    ///< Return cause: HDMI0 Interrupt
  uint16_t Hdmi1Interrupt : 1;    ///< Return cause: HDMI1 Interrupt
  uint16_t WakUpPin0      : 1;    ///< Return cause: Wakeup-Pin0 Interrupt
  uint16_t WakUpPin1      : 1;    ///< Return cause: Wakeup-Pin1 Interrupt
  uint16_t WakUpPin2      : 1;    ///< Return cause: Wakeup-Pin2 Interrupt
  uint16_t WakUpPin3      : 1;    ///< Return cause: Wakeup-Pin3 Interrupt
  uint16_t WakUpPin4      : 1;    ///< Return cause: Wakeup-Pin4 Interrupt
  uint16_t WakUpPin5      : 1;    ///< Return cause: Wakeup-Pin5 Interrupt
  uint16_t Reserved       : 4;
} stc_dsm_return_cause_t;

/**
 ******************************************************************************
 ** \brief Deep Standby Mode configuration
 ** 
 ** The DSM configuration settings
 ******************************************************************************/
typedef struct stc_dsm_config
{
  en_dsm_mode_t       enMode;               ///< Deep Standby Mode. See #en_dsm_mode_t for details
  boolean_t           bLpmPortHiZState;     ///< TRUE: Sets the status of each external port pin
                                            ///        to high impedance (Hi-Z) in Deep Stop/RTC mode
                                            ///< FALSE: Holds the status of each external port pin  
  boolean_t           bSubClkHdmiCec;       ///< TRUE: Sub clock is supplied to HDMI-CEC
                                            ///< FALSE: Sub clock is not supplied to HDMI-CEC
  boolean_t           bSubClkRtc;           ///< TRUE: Sub clock is supplied to RTC
                                            ///< FALSE: Sub clock is not supplied to RTC
  boolean_t           bInitxResetReturn;    ///< TRUE: Return from DSM by INITX Reset
                                            ///< FALSE: No return from DSM by INITX Reset
  boolean_t           bLvdResetReturn;      ///< TRUE: Return from DSM by LVD Reset
                                            ///< FALSE: No return from DSM by LVD Reset
  boolean_t           bLvdInterrupt;        ///< TRUE: Return from DSM by LVD Interrupt
                                            ///< FALSE: No return from DSM by LVD Interrupt
  boolean_t           bRtcInterrupt;        ///< TRUE: Return from DSM by RTC Interrupt
                                            ///< FALSE: No return from DSM by RTC Interrupt
  union { 
    uint8_t             HdmiChannelEnable;  ///< Defines return channel of HDMI-CEC/remote control
    stc_hdmi_channel_t  stcHdmiChannelEnable;
  };
  union {
    uint8_t             WkPinChannelEnable; ///< Defines wak-up pin active
    stc_wkpin_channel_t stcWkPinChannelEnable;
  };
  union {
    uint8_t             WkPinChannelLevel;  ///< Defines Wake-up pin level
    stc_wkpin_channel_t stcWkPinChannelLevel;
  };
  boolean_t           bSramRetentionEnable; ///< TRUE: Retain SRAM conentes in DSM
                                            ///< FALSE: No SRAM data retention is DSM
  func_ptr_t          pfnHook;              ///< Hook call for low power modes (called between SLEEPDEEP = 1 and WFI)     
} stc_dsm_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/
  
/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern en_result_t Dsm_SetMode(stc_dsm_config_t* pstcConfig) ;

extern en_result_t Dsm_GetReturnCause(stc_dsm_return_cause_t* pstcReturnCause) ;

//@} // DsmGroup

#ifdef __cplusplus
}
#endif

#endif /* __DSM_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
