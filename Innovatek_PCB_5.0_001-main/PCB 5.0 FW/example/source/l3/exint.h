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
/** \file exint.h
 **
 ** Headerfile for EXINT functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/

#ifndef __EXINT_H__
#define __EXINT_H__

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
 ** \defgroup ExintGroup External Interrupts (EXINT)
 **
 ** Provided functions of EXINT module:
 ** 
 ** - Exint_Init()
 ** - Exint_DeInit()
 ** - Exint_EnableChannel()
 ** - Exint_DisableChannel()
 ** - Exint_Nmi_Init()
 ** - Exint_Nmi_DeInit()
 **
 ** \brief How to use External Interrupts
 **
 ** With Exint_Init() the external interrupts are enabled as given in the
 ** configuration. Note, that both external interrupt groups (0-7 and 8-15)
 ** can be enabled individually.
 **
 ** If a single channel may be disabled and reenabled during runtime the
 ** functions Exint_DisableChannel() and Exint_EnableChannel() can be
 ** used.
 **
 ** The external interrupts are disabled globally by Exint_DeInit().
 **
 ** Each channel provides an individually callback function which is handled
 ** in the interrupt service routine.
 **
 ** Exint_Nmi_Init() "initializes" the None-Maskable Interrupt, which means
 ** that the callback function is set to the dedicated internal data and if
 ** set by stc_exint_nmi_config_t#bTouchNVIC the NVIC is initialized.
 ** Exint_Nmi_DeInit() resets the NVIC depending on the setting in
 ** stc_exint_nmi_config_t#bTouchNVIC.
 **
 ** \note 
 **       - The NMI shares with the Hardware Watchdog Timer the same interrupt
 **         vector. 
 **       - The driver recently only support External Interrupts from 0 - 15!
 ******************************************************************************/
//@{
  
/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
 
#define EXINT0_BITPOS      0x0001
#define EXINT1_BITPOS      0x0002
#define EXINT2_BITPOS      0x0004
#define EXINT3_BITPOS      0x0008
#define EXINT4_BITPOS      0x0010
#define EXINT5_BITPOS      0x0020
#define EXINT6_BITPOS      0x0040
#define EXINT7_BITPOS      0x0080
#define EXINT8_BITPOS      0x0100
#define EXINT9_BITPOS      0x0200
#define EXINT10_BITPOS     0x0400
#define EXINT11_BITPOS     0x0800
#define EXINT12_BITPOS     0x1000
#define EXINT13_BITPOS     0x2000
#define EXINT14_BITPOS     0x4000
#define EXINT15_BITPOS     0x8000

#define EXINT0_BITMASK     0xFFFE
#define EXINT1_BITMASK     0xFFFD
#define EXINT2_BITMASK     0xFFFB
#define EXINT3_BITMASK     0xFFF7
#define EXINT4_BITMASK     0xFFEF
#define EXINT5_BITMASK     0xFFDF
#define EXINT6_BITMASK     0xFFBF
#define EXINT7_BITMASK     0xFF7F
#define EXINT8_BITMASK     0xFEFF
#define EXINT9_BITMASK     0xFDFF
#define EXINT10_BITMASK    0xFBFF
#define EXINT11_BITMASK    0xF7FF
#define EXINT12_BITMASK    0xEFFF
#define EXINT13_BITMASK    0xDFFF
#define EXINT14_BITMASK    0xBFFF
#define EXINT15_BITMASK    0x7FFF  

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/
  
typedef enum en_exint_level
{
  ExIntLowLevel     = 0,  ///< "L" level detection
  ExIntHighLevel    = 1,  ///< "H" level detection
  ExIntRisingEdge   = 2,  ///< Rising edge detection
  ExIntFallingEdge  = 3   ///< Falling edge detection
} en_exint_level_t;

/**
 ******************************************************************************
 ** \brief EXINT configuration
 ******************************************************************************/

/**
 *****************************************************************************
 ** \brief External Interrupt configuration
 ** 
 ** The EXINT configuration
 *****************************************************************************/
typedef struct stc_exint_config
{
  boolean_t         abEnable[L3_EXINT_CHANNELS];           ///< TRUE: External Interrupt enable
  en_exint_level_t  aenLevel[L3_EXINT_CHANNELS];           ///< level detection, see #en_exint_level_t for details
  func_ptr_t        apfnExintCallback[L3_EXINT_CHANNELS];  ///< Callback pointers
} stc_exint_config_t;

/**
 *****************************************************************************
 ** \brief NMI configuration
 ** 
 ** The NMI configuration
 *****************************************************************************/
typedef struct stc_exint_nmi_config
{
  boolean_t         bTouchNVIC;       ///< TRUE: Set/clear NVIC registers during init and de-init
  func_ptr_t        pfnNMICallback;   ///< NMI Callback pointers
} stc_exint_nmi_config_t;

/**
 ******************************************************************************
 ** \brief Datatype for holding internal data needed for EXINT
 ******************************************************************************/
typedef struct stc_exint_intern_data
{
  func_ptr_t    apfnExintCallback[L3_EXINT_CHANNELS];  ///< Callback pointer array
} stc_exint_intern_data_t ;

/**
 ******************************************************************************
 ** \brief Datatype for holding internal data needed for NMI
 ******************************************************************************/
typedef struct stc_exint_nmi_intern_data
{
  func_ptr_t    pfnNMICallback;  ///< Callback pointer
} stc_exint_nmi_intern_data_t ;


/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern stc_exint_intern_data_t stcExintInternData;
extern stc_exint_nmi_intern_data_t stcExintNMIInternData;

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
 
extern void ExintIrqHandler0_7(void) ;

extern void ExintIrqHandler8_15(void) ;

extern void ExintNMIrqHandler(void) ;

en_result_t Exint_Init(stc_exint_config_t* pstcConfig) ;

en_result_t Exint_DeInit(void) ;

en_result_t Exint_EnableChannel(uint8_t u8Channel) ;

en_result_t Exint_DisableChannel(uint8_t u8Channel) ;

en_result_t Exint_Nmi_Init(stc_exint_nmi_config_t* pstcConfig) ;

en_result_t Exint_Nmi_DeInit(stc_exint_nmi_config_t* pstcConfig) ;
  
//@} // ExintGroup

#ifdef __cplusplus
}
#endif

#endif /* __EXINT_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
