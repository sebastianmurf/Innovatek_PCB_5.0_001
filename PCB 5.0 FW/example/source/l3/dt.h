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
/** \file dt.h
 **
 ** Headerfile for DT functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-08-10  1.1  MWi  DT Register Bitfield definitions added, typos
 **                           corrected (DtConterSize16, DtConterSize32,
 **                           u32DtBakgroundLoad)
 **
 ******************************************************************************/

#ifndef __DT_H__
#define __DT_H__

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
 ** \defgroup DtGroup Dual Timer (DT)
 **
 ** Provided functions of DT module:
 ** 
 ** - Dt_Init()
 ** - Dt_DeInit() 
 ** - Dt_SetChannelConfig()
 ** - Dt_EnableChannel()
 ** - Dt_DisableChannel()
 ** - Dt_WriteLoadChannel()
 ** - Dt_WriteBgLoadChannel()
 ** - Dt_ReadValueChannel()
 **
 ** \brief How to use DT module
 **
 ** Dt_Init() must be used for activation of the internal data structures for
 ** using the Dual Timer (DT). Here the interrupt callback functions for each
 ** of the 2 channels are set-up. Because the DT has no output pins, its
 ** interrupt service is always used.
 **
 ** For configuration of a DT channel a strucutre with the type
 ** #stc_dt_channel_config_t has to be used. After configuration the DT channel
 ** can be set-up by Dt_SetChannelConfig().
 **
 ** A DT Channel can be enabled by the function Dt_EnableChannel().
 ** Depending on the used mode, it is started in:
 ** - Free-run mode
 ** - Periodic mode
 ** - Single-shot mode
 **
 ** With Dt_WriteLoadChannel() the recent DT counter is set to the value
 ** given in the configuration stc_dt_channel_config_t#u32DtLoad. This works in
 ** each of the three operation modes.
 **
 ** With Dt_WriteBgLoadChannel() the background reload value can be set,
 ** which is then set to the load value after the DT counter reaches the
 ** next 0.
 **
 ** Before deinitialization of the DT by Dt_DeInit(), it is recommended
 ** to disable all channels via Dt_DisableChannel() before, to avoid a
 ** possible, unwanted interrupt.
 **
 ** \note The Dual Timer shares its interrupt vector with the QPRC.
 **       Therefore use bTouchNVIC parameter in Dt_Init() and Dt_DeInit()
 **       whether to touch the QPRC-NVIC registers or not.
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief DtCallback function prototype.
 ******************************************************************************/
typedef void (*dt_cb_func_ptr_t)(void);

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define DT0  (*((volatile stc_dtn_t *) L3_DT0_BASE))

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief Redefinition of DT TimerXControl bit field structure
 ******************************************************************************/
typedef struct stc_dtn_timerxcontrol_field
{
  __IO uint32_t ONESHOT    : 1;
  __IO uint32_t TIMERSIZE  : 1;
  __IO uint32_t TIMERPRE0  : 1;
  __IO uint32_t TIMERPRE1  : 1;
       uint32_t RESERVED1  : 1;
  __IO uint32_t INTENABLE  : 1;
  __IO uint32_t TIMERMODE  : 1;
  __IO uint32_t TIMEREN    : 1;
} stc_dtn_timerxcontrol_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DT Interrupt Status Register  bit field structure
 ******************************************************************************/
typedef struct stc_dtn_timerxris_field
{
  __IO uint32_t TIMERXRIS  : 1;
} stc_dtn_timerxris_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DT Masked Interrupt Status Register  bit field
 **        structure
 ******************************************************************************/
typedef struct stc_dtim_timerxmis_field
{
  __IO uint32_t TIMERXRIS  : 1;
} stc_dtim_timerxmis_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DT register structure 
 ******************************************************************************/
typedef struct stc_dtn
{
  __IO uint32_t TIMER1LOAD;
  __IO uint32_t TIMER1VALUE;
  union {
    __IO uint32_t TIMER1CONTROL;
    stc_dtn_timerxcontrol_field_t TIMER1CONTROL_f;
  };
  __IO uint32_t TIMER1INTCLR;
  union {
    __IO uint32_t TIMER1RIS;
    stc_dtn_timerxris_field_t TIMER1RIS_f;
  };
  union {
    __IO uint32_t TIMER1MIS;
    stc_dtim_timerxmis_field_t TIMER1MIS_f;
  };
  __IO uint32_t TIMER1BGLOAD;
        uint8_t RESERVED0[4];
  __IO uint32_t TIMER2LOAD;
  __IO uint32_t TIMER2VALUE;
  union {
    __IO uint32_t TIMER2CONTROL;
    stc_dtn_timerxcontrol_field_t TIMER2CONTROL_f;
  };
  __IO uint32_t TIMER2INTCLR;
  union {
    __IO uint32_t TIMER2RIS;
    stc_dtn_timerxris_field_t TIMER2RIS_f;
  };
  union {
    __IO uint32_t TIMER2MIS;
    stc_dtim_timerxmis_field_t TIMER2MIS_f;
  };
  __IO uint32_t TIMER2BGLOAD;
} stc_dtn_t;

/**
 ******************************************************************************
 ** \brief Dt mode
 ** 
 ** To select between Free-run, Periodic, and One-Shot mode
 ******************************************************************************/
typedef enum en_dt_mode
{
    DtFreeRun      = 0,  ///< Free-running mode
    DtPeriodic     = 1,  ///< Periodic mode
    DtOneShot      = 2   ///< One-shot mode
} en_dt_mode_t;

/**
 ******************************************************************************
 ** \brief Dt Prescaler
 ** 
 ** To select clock divider
 ******************************************************************************/
typedef enum en_dt_prescaler
{
    DtPrescalerDiv1     = 0,  ///< Prescaler divisor 1
    DtPrescalerDiv16    = 1,  ///< Prescaler divisor 16
    DtPrescalerDiv256   = 2   ///< Prescaler divisor 256
} en_dt_prescaler_t;

/**
 ******************************************************************************
 ** \brief Dt Counter Size
 ** 
 ** To select the size of the counter
 ******************************************************************************/
typedef enum en_dt_countersize
{
    DtCounterSize16     = 0,  ///< 16 Bit counter size
    DtCounterSize32     = 1,  ///< 32 Bit counter size
} en_dt_countersize_t;

/**
 *****************************************************************************
 ** \brief Dt channel configuration
 ** 
 ** The DT configuration is done on a per channel basis
 *****************************************************************************/
typedef struct stc_dt_channel_config
{
  en_dt_mode_t         enMode;             ///< See description of #en_dt_mode_t
  en_dt_prescaler_t    enPrescalerDiv;     ///< See description of #en_dt_prescaler_t
  en_dt_countersize_t  enCounterSize;      ///< 0 = 16 bit, 1 = 32 bit
  boolean_t            bEnable;            ///< Timer enable
  uint32_t             u32DtLoad;          ///< Ticks of Load Register
  uint32_t             u32DtBackgroundLoad;///< Ticks of Background Load Register
  dt_cb_func_ptr_t     pfnCallback;        ///< Function pointer to Callback 
} stc_dt_channel_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled DT instance
typedef enum en_dt_instance_index
{
    #if (L3_PERIPHERAL_ENABLE_DT0 == L3_ON)
    DtInstanceIndexDt0,
    #endif
    // Case if no DT instance is selected
    #if (L3_PERIPHERAL_ENABLE_DT0 == L3_OFF)
    DtInstanceDummy
    #endif
} en_dt_instance_index_t;

/// Datatype for holding internal data needed for DT
typedef struct stc_dt_intern_data
{
    ///< Callback for interrupts of DT channel 0
    func_ptr_t  pfnCallbackChannel0 ;
    ///< Callback for interrupts of DT channel 1
    func_ptr_t  pfnCallbackChannel1 ;
} stc_dt_intern_data_t ;

/// DT instance data type
typedef struct stc_dt_instance_data
{
    volatile stc_dtn_t* pstcInstance;   ///< pointer to registers of an instance
    stc_dt_intern_data_t stcInternData; ///< module internal data of instance
} stc_dt_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled DT instances and their internal data
extern stc_dt_instance_data_t m_astcDtInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void DtIrqHandler0( volatile stc_dtn_t* pstcDt,
                    stc_dt_intern_data_t* pstcDtInternData
                   ) ;

en_result_t Dt_Init( volatile stc_dtn_t* pstcDt,
                     boolean_t bTouchNVIC
                   ) ;

en_result_t Dt_SetChannelConfig( 
                               volatile stc_dtn_t* pstcDt, 
                               stc_dt_channel_config_t* pstcConfig,
                               uint8_t u8Channel
                              ) ;

en_result_t Dt_EnableChannel( volatile stc_dtn_t* pstcDt,
                              uint8_t u8Channel
                             ) ;

en_result_t Dt_DisableChannel( volatile stc_dtn_t* pstcDt,
                               uint8_t u8Channel
                             ) ;

en_result_t Dt_WriteLoadChannel( volatile stc_dtn_t* pstcDt,
                                 stc_dt_channel_config_t* pstcConfig,
                                 uint8_t u8Channel
                               ) ;

en_result_t Dt_WriteBgLoadChannel( volatile stc_dtn_t* pstcDt,
                                   stc_dt_channel_config_t* pstcConfig,
                                   uint8_t u8Channel
                                 ) ;

uint32_t Dt_ReadValueChannel ( volatile stc_dtn_t* pstcDt,
                              stc_dt_channel_config_t* pstcConfig,
                              uint8_t u8Channel
                             ) ;

en_result_t Dt_DeInit( volatile stc_dtn_t* pstcDt,
                       boolean_t bTouchNVIC
                     ) ;

//@} // DtGroup

#ifdef __cplusplus
}
#endif

#endif /* __DT_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
