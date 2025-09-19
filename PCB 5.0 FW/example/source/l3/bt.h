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
/** \file bt.h
 **
 ** Headerfile for BT functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  Mwi  L3 address definition for instance pointer used
 **
 ******************************************************************************/

#ifndef __BT_H__
#define __BT_H__

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
 ** \defgroup BtGroup Base Timer Functions (BT)
 **
 ** Provided functions of BT module:
 ** 
 ** - Bt_Init()
 ** - Bt_Enable()
 ** - Bt_Disable()
 ** - Bt_Trigger()
 ** - Bt_DeInit()
 ** - Bt_InitIoSel()
 ** - Bt_DeInitIoSel()
 ** - Bt_ResetIoSel()
 ** - Bt_TriggerInstances()
 ** - Bt_ReadTimer16()
 ** - Bt_ReadTimer32()
 ** - Bt_CheckPwcError()
 ** - Bt_ReadDataBuffer16()
 ** - Bt_ReadDataBuffer32()
 **
 ** To initialize a BT instance, use Bt_Init(). After this the BT instance
 ** can be enabled by Bt_Enable() and disabled by Bt_Disable(). To trigger
 ** an instance by software use Bt_Trigger(). After operation Bt_DeInit() can
 ** be used to de-initialize a BT instance and disable its interrupts.
 **
 ** For each BT mode callback functions are provided for the following
 ** events:
 ** - Callback0: Timer start trigger detection for PWM, PPG, RLT; Completion of
 **              measurment for PWC
 ** - Callback1: Underflow detection for PWM, PPG, RLT; Overflow detection for
 **              PWC
 ** - Callback2: Duty cycle detection for PWM
 **
 ** \attention If an interrupt reason is enbaled by the configuration, the
 **            corresponding callback function pointer <b>must</b> be defined.
 **            The BT ISR does not check the callback pointer for definition
 **            because of speed reasons!
 **
 ** The BT I/O selector is configured by Bt_InitIoSel(). The parameters are
 ** the BT instance pair (0/1, 2/3, 4/5, or 6/7) and the mode.
 ** Bt_TriggerInstances() trigger bit-pattern-selected BT instances software-
 ** based. An I/O selector can be de-initialized (reset to mode 0) by using
 ** Bt_DeInitIoSel().
 ** To reset all BT I/O selectors Bt_ResetIoSel() can be used.
 ** Bt_ReadTimer16() or Bt_ReadTimer32() reads out the recent timer count,
 ** if RLT mode was set.
 **
 ** Bt_ReadDataBuffer16() or Bt_ReadDataBuffer32() reads out the result count,
 ** if PWC mode was set. Reading the data buffer is <b>mandatory</b> for the 
 ** callback function, because the interrupt request is only cleared by reading
 ** the data buffer - not by 'clearing' (read-only) EDIR bit!
 ** It's recommended to use Bt_CheckPwcError() before to check counter overrun.
 **
 ** If 32-bit is set for RLT or PWC mode, settings are only done in an even
 ** numbered BT instance number. The instance number + 1 is then not
 ** available for individual settings anymore! Do not use this "upper" BT
 ** instance, because the driver does not check this.
 **
 ** If 32-bit is set for RLT mode, the driver takes care of correct 32-bit
 ** reload value setting in Bt_Init(). Note that only even numbered BT
 ** instances can be used for 32-bit mode and the number + 1 is not available
 ** for individual settings anymore.
 **
 ** \note For speed reasons this module does not provide an instance data
 **       look-up table. The callback pointers are held in individual global
 **       variables which are only defined, if a BT instances is actually
 **       activated.
 **
 ** \pre If several BTs should be combined by certain BTIOSEL I/O modes,
 **      each BT instance must be activated individually by
 **      L3_PERIPHERAL_ENABLE_BTn with L3_ON in l3_user.h! Otherwise
 **      malfunction of the BT driver may result.
 ******************************************************************************/
//@{

/******************************************************************************
 * Global definitions
 ******************************************************************************/
  
#define BT0       (*((volatile stc_btn_t *) L3_BT0_BASE))
#define BT1       (*((volatile stc_btn_t *) L3_BT1_BASE))
#define BT2       (*((volatile stc_btn_t *) L3_BT2_BASE))
#define BT3       (*((volatile stc_btn_t *) L3_BT3_BASE))
#define BT4       (*((volatile stc_btn_t *) L3_BT4_BASE))
#define BT5       (*((volatile stc_btn_t *) L3_BT5_BASE))
#define BT6       (*((volatile stc_btn_t *) L3_BT6_BASE))
#define BT7       (*((volatile stc_btn_t *) L3_BT7_BASE))

/**
 ******************************************************************************
 ** Bit positions for Software-based Simultaneous Register
 **
 ** Example usage: Bt_TriggerInstances(BTSSSR_BT0_BIT | BTSSSR_BT9_BIT);
 ******************************************************************************/ 
#define BTSSSR_BT0_BIT  0x0001
#define BTSSSR_BT1_BIT  0x0002
#define BTSSSR_BT2_BIT  0x0004
#define BTSSSR_BT3_BIT  0x0008
#define BTSSSR_BT4_BIT  0x0010
#define BTSSSR_BT5_BIT  0x0020
#define BTSSSR_BT6_BIT  0x0040
#define BTSSSR_BT7_BIT  0x0080
#define BTSSSR_BT8_BIT  0x0100
#define BTSSSR_BT9_BIT  0x0200
#define BTSSSR_BT10_BIT 0x0400
#define BTSSSR_BT11_BIT 0x0800
#define BTSSSR_BT12_BIT 0x1000
#define BTSSSR_BT13_BIT 0x2000
#define BTSSSR_BT14_BIT 0x4000
#define BTSSSR_BT15_BIT 0x8000

/**
 ******************************************************************************
 ** Bit positions mask for interrut enable bits and interupt flag bits in STC
 ** register. These definitions are needed, because in the corresponding ISR
 ** both enable and flag bits have to be checked for an interrupt cause.
 ** IRQ Monitoring is not helping, because Duty Cycle and Underflow Irq share
 ** the same IRQ number (0) using PWM functionality.
 ******************************************************************************/ 
#define BT_TGIE_TGIR_MASK 0x44
#define BT_DTIE_DTIR_MASK 0x22
#define BT_UDIE_UDIR_MASK 0x11  
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief Redefinition of BT TMCR register for all modes
 ******************************************************************************/
typedef struct stc_btn_tmcr_field
{
  __IO  uint16_t STRG      : 1;
  __IO  uint16_t CTEN      : 1;
  __IO  uint16_t MDSE      : 1;
  __IO  uint16_t OSEL      : 1;
  __IO  uint16_t FMD       : 3;
  __IO  uint16_t T32       : 1;
  __IO  uint16_t EGS       : 2;
  __IO  uint16_t PMSK_EGS2 : 1;
  __IO  uint16_t RTGEN     : 1;
  __IO  uint16_t CKS       : 3;
  __IO  uint16_t RESERVED2 : 1;  
} stc_btn_tmcr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of BT STC register for all modes
 ******************************************************************************/
typedef struct stc_btn_stc_field
{
  __IO  uint8_t UDIR_OVIR : 1;
  __IO  uint8_t DTIR      : 1;
  __IO  uint8_t TGIR_EDIR : 1;
  __IO  uint8_t RESERVED1 : 1;
  __IO  uint8_t UDIE_OVIE : 1;
  __IO  uint8_t DTIE      : 1;   // only available for RWM
  __IO  uint8_t TGIE_EDIE : 1; 
  __IO  uint8_t ERR       : 1;   // only for PWC
} stc_btn_stc_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of BT TMCR2 register for all modes
 ******************************************************************************/
typedef struct stc_btn_tmcr2_field
{
  __IO  uint8_t CKS3     : 1;
  __IO  uint8_t RESERVED : 7;
} stc_btn_tmcr2_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of BT register structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_btn {
  union {
    uint16_t                  PCSR_PRLL;
    union {
      uint8_t                 PCSRL_PRLLL;
      uint8_t                 PCSRH_PRLLH;
    };
  };
  uint16_t                    Reserved0;
  union {
    uint16_t                  PDUT_PRLH_DTBF;
    union {
      uint8_t                 PDUTL_PRLHL_DTBFL;
      uint8_t                 PDUTH_PRLHH_DTBFH;
    };
  };  
  uint16_t                    Reserved1;
  uint16_t                    TMR;
  uint16_t                    Reserved2;  
  union {
    uint16_t                  TMCR;
    stc_btn_tmcr_field_t      u16TMCR;
  };
  uint16_t                    Reserved3;
  union {
    uint8_t                   STC;
    stc_btn_stc_field_t       u8STC;
  };  
  union {
    uint8_t                   TMCR2;
    stc_btn_tmcr2_field_t      u8TMCR2;
  };  
} stc_btn_t;

/**
 ******************************************************************************
 ** \brief BT mode
 ** 
  ** To select between:
 ** - PWM
 ** - PPG
 ** - RLT
 ** - PWC
 **
 ** \note The Reset Mode is set by Bt_DeInit() function.
 **
 ******************************************************************************/
typedef enum en_bt_mode
{
  BtPwm      = 0,  ///< Pulse Width Modulation mode
  BtPpg      = 1,  ///< Programmable Pulse Generator mode
  BtRlt      = 2,  ///< Reload Timer mode
  BtPwc      = 3   ///< Pulse Width Counter mode
} en_bt_mode_t;

/**
 ******************************************************************************
 ** \brief Bt prescaler dividers
 ******************************************************************************/
typedef enum en_bt_prescaler
{
  BtExtClkRisingEdge  = 0,  ///< External clock, BT triggered by rising edge
  BtExtClkFallingEdge = 1,  ///< External clock, BT triggered by falling edge
  BtExtClkBothEdges   = 2,  ///< External clock, BT triggered by rising and falling edge
  BtIntClkDiv1        = 3,  ///< Internal PCLK
  BtIntClkDiv4        = 4,  ///< Internal PCLK / 4
  BtIntClkDiv16       = 5,  ///< Internal PCLK / 16
  BtIntClkDiv128      = 6,  ///< Internal PCLK / 128
  BtIntClkDiv256      = 7,  ///< Internal PCLK / 256
  BtIntClkDiv512      = 8,  ///< Internal PCLK / 512
  BtIntClkDiv1024     = 9,  ///< Internal PCLK / 1024
  BtIntClkDiv2048     = 10  ///< Internal PCLK / 2048
} en_bt_prescaler_t ;

/**
 ******************************************************************************
 ** \brief Bt external trigger (PWM, PPG, RLT) and measurement edge (PWC) input
 **        sensitivity
 ******************************************************************************/
typedef enum en_bt_trigger_input
{
  BtTriggerDisable      = 0,  ///< Trigger input disabled (PWM, PPG, RLT)
  BtTriggerRisingEdge   = 1,  ///< Trigger by rising edge (PWM, PPG, RLT)
  BtTriggerFallingEdge  = 2,  ///< Trigger by falling edge (PWM, PPG, RLT)
  BtTriggerBothEdges    = 3,  ///< Trigger by both edges (PWM, PPG, RLT) 
  BtMeasureHighPulse    = 4,  ///< Measurement of high pulse (PWC)
  BtMeasureLowPulse     = 5,  ///< Measurement of low pulse (PWC)
  BtMeasureRisingEdges  = 6,  ///< Measurement between rising edges (PWC)
  BtMeasureFallingEdges = 7,  ///< Measurement between falling edges (PWC)
  BtMeasureAllEdges     = 8   ///< Measurement between all edges (PWC)
} en_bt_trigger_input_t ;

/**
 ******************************************************************************
 ** \brief Bt I/O selector modes
 ******************************************************************************/
typedef enum en_bt_iosel_mode
{
  BtMode0   = 0,  ///< I/O mode 0 (Standard 16-bit timer mode)
  BtMode1   = 1,  ///< I/O mode 1 (Timer full mode)
  BtMode2   = 2,  ///< I/O mode 2 (Shared external trigger mode)
  BtMode3   = 3,  ///< I/O mode 3 (Shared channel signal trigger mode)
  BtMode4   = 4,  ///< I/O mode 4 (Timer start/stop mode)
  BtMode5   = 5,  ///< I/O mode 5 (Software-based simultaneous startup mode)
  BtMode6   = 6,  ///< I/O mode 6 (Software-based startup and timer start/stop mode)
  BtMode7   = 7,  ///< I/O mode 4 (Timer start mode)
  BtMode8   = 8   ///< I/O mode 4 (Shared channel signal timer and timer start/stop mode)
} en_bt_iosel_mode_t ;

/**
 ******************************************************************************
 ** \brief I/O selector BT instance pair numbers
 ******************************************************************************/
typedef enum en_bt_iosel_instance
{
  BtSel01   = 0,  ///< Selector for BT0 and BT1
  BtSel23   = 1,  ///< Selector for BT2 and BT3
  BtSel45   = 2,  ///< Selector for BT4 and BT5
  BtSel67   = 3   ///< Selector for BT6 and BT7
} en_bt_iosel_instance_t ;

/**
 ******************************************************************************
 ** \brief Bt configuration.
 **
 ** Contains all parameter for configuratin a BT channel.  
 ******************************************************************************/
typedef struct stc_bt_config
{
    en_bt_mode_t           enMode;               ///< See description of #en_bt_mode_t.
    en_bt_prescaler_t      enPrescaler;          ///< See description of #en_bt_prescaler_t.
    boolean_t              bRestart;             ///< TRUE: Restart by software trigger or trigger input enabled
    boolean_t              bOutputLow;           ///< TRUE: PWM and PPG output masked to Low output, not used by PLT and PWC
    boolean_t              bOutputInvert;        ///< TRUE: Output inverted (High output after reset), not available for PWC
    boolean_t              bOneShot;             ///< TRUE: One-shot operation, FALSE: Continous operation
    boolean_t              bTimer32;             ///< TRUE: RLT, PWC 32 bit, FALSE: RLT, pWC 16 bit, not used by PWM, PPG
    en_bt_trigger_input_t  enExtTriggerMode;     ///< See description of #en_bt_trigger_input_t
    union {
      struct {
        union {
          uint16_t         u16Cycle;             ///< PWM, RLT cycle setting
          uint16_t         u16LowWidth;          ///< PPG low width setting
        };
        union {
          uint16_t         u16Duty;              ///< PWM duty cycle setting
          uint16_t         u16HighWidth;         ///< PPG high width setting
        };
      };
      uint32_t             u32Cycle;             ///< RLT reload value, 32-bit 
    };
    union {  
      boolean_t            bUnderflowIrqEnable;  ///< TRUE: PWM, PPG, RLT: Enables underflow interrupt request and callback1
      boolean_t            bOverflowIrqEnable;   ///< TRUE: PWC: Enables overflow interrupt and callback0
    };
    union {  
      boolean_t            bTriggerIrqEnable;    ///< TRUE: PWM, PPG, RLT: Enables trigger interrupt request and callback0
      boolean_t            bCompletionIrqEnable; ///< TRUE: PWC: Enables measuremnet completion interrupt and callback1
    };
    boolean_t              bDutyIrqEnable;       ///< TRUE: PWM: Enables duty match interrupt request and callback2
    func_ptr_t             pfnCallback0;         ///< IRQ1 callback:
                                                 ///< - PWM function: Timer start trigger detection
                                                 ///< - PPG function: Timer start trigger detection
                                                 ///< - RLT function: Timer start trigger detection
                                                 ///< - PWC function: Completion of measurement detection
    func_ptr_t             pfnCallback1;         ///< IRQ0 callback:
                                                 ///< - PWM function: Underflow detection
                                                 ///< - PPG function: Underflow detection  
                                                 ///< - RLT function: Underflow detection 
                                                 ///< - PWC function: Overflow detection 
    func_ptr_t             pfnCallback2;         ///< IRQ0 callback:
                                                 ///< - PWM function: Match in duty detection    
} stc_bt_config_t;

/**
 ******************************************************************************
 ** \brief Bt IOSEL configuration.
 **
 ** Contains all parameter for configuratin of the BT IOSEL
 ******************************************************************************/
typedef struct stc_bt_iosel_config
{
  en_bt_iosel_instance_t  enBtInstance ; /// Selects BT-instance pairm, see descrioption of #en_bt_iosel_instance_t
  en_bt_iosel_mode_t      enMode ;       ///< See description of #en_bt_iosel_mode_t.
} stc_bt_iosel_config_t ;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Callback pointer collection
 ******************************************************************************/
#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  extern func_ptr_t pfnCallback0Bt0;
  extern func_ptr_t pfnCallback1Bt0;
  extern func_ptr_t pfnCallback2Bt0;
#endif
#if (L3_PERIPHERAL_ENABLE_BT1 == L3_ON)
  extern func_ptr_t pfnCallback0Bt1;
  extern func_ptr_t pfnCallback1Bt1;
  extern func_ptr_t pfnCallback2Bt1;
#endif
#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  extern func_ptr_t pfnCallback0Bt2;
  extern func_ptr_t pfnCallback1Bt2;
  extern func_ptr_t pfnCallback2Bt2;
#endif
#if (L3_PERIPHERAL_ENABLE_BT3 == L3_ON)
  extern func_ptr_t pfnCallback0Bt3;
  extern func_ptr_t pfnCallback1Bt3;
  extern func_ptr_t pfnCallback2Bt3;
#endif
#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  extern func_ptr_t pfnCallback0Bt4;
  extern func_ptr_t pfnCallback1Bt4;
  extern func_ptr_t pfnCallback2Bt4;
#endif
#if (L3_PERIPHERAL_ENABLE_BT5 == L3_ON)
  extern func_ptr_t pfnCallback0Bt5;
  extern func_ptr_t pfnCallback1Bt5;
  extern func_ptr_t pfnCallback2Bt5;
#endif
#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  extern func_ptr_t pfnCallback0Bt6;
  extern func_ptr_t pfnCallback1Bt6;
  extern func_ptr_t pfnCallback2Bt6;
#endif
#if (L3_PERIPHERAL_ENABLE_BT7 == L3_ON)
  extern func_ptr_t pfnCallback0Bt7;
  extern func_ptr_t pfnCallback1Bt7;
  extern func_ptr_t pfnCallback2Bt7;
#endif

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void BtIrqHandler( void ) ;

extern en_result_t Bt_Init( stc_btn_t*       pstcBt, 
                            stc_bt_config_t* pstcConfig
                          ) ;

extern en_result_t Bt_Enable( stc_btn_t* pstcBt ) ;

extern en_result_t Bt_Disable( stc_btn_t* pstcBt ) ;

extern en_result_t Bt_Trigger( stc_btn_t* pstcBt ) ;

extern en_result_t Bt_DeInit( stc_btn_t* pstcBt,
                              boolean_t  bDisableBtInterrupts
                            ) ;

extern en_result_t Bt_InitIoSel(stc_bt_iosel_config_t* pstcConfig) ;

extern en_result_t Bt_DeInitIoSel(stc_bt_iosel_config_t* pstcConfig) ;

extern en_result_t Bt_TriggerInstances(uint16_t u16TriggerBtInstance) ;

extern en_result_t Bt_ResetIoSel(void) ;

extern uint16_t Bt_ReadTimer16(stc_btn_t* pstcBt) ;

extern uint32_t Bt_ReadTimer32(stc_btn_t* pstcBt) ;

extern en_result_t Bt_CheckPwcError(stc_btn_t* pstcBt) ;

extern uint16_t Bt_ReadDataBuffer16(stc_btn_t* pstcBt) ;

extern uint32_t Bt_ReadDataBuffer32(stc_btn_t* pstcBt) ;

//@} // BtGroup

#ifdef __cplusplus
}
#endif

#endif /* __BT_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
