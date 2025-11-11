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
/** \file qprc.h
 **
 ** Headerfile for QPRC functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  MWi  L3 address definition for instance pointer used
 **   - 2012-04-23  1.1  MWi  All device type support
 **
 ******************************************************************************/

#ifndef __QPRC_H__
#define __QPRC_H__

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
 ** \defgroup QprcGroup Quadrature Position and Revolution Counter Functions (QPRC)
 **
 ** Provided functions of QPRC module:
 ** 
 ** - Qprc_Init()
 ** - Qprc_DeInit()
 ** - Qprc_Enable()
 ** - Qprc_Disable()
 ** - Qprc_SetPositionCount()
 ** - Qprc_SetRevolutionCount()
 ** - Qprc_SetRevolutionCountCompare()
 ** - Qprc_SetPositionRevolutionCountCompare()
 ** - Qprc_SetMaxPositionCount()
 ** - Qprc_GetPositionCount()
 ** - Qprc_GetRevolutionCount()
 ** - Qprc_GetRevolutionCountCompare()
 ** - Qprc_GetPositionRevolutionCountCompare()
 ** - Qprc_GetMaxPositionCount()
 ** - Qprc_GetPositionRevolutionCount()
 **
 ** Init a QPRC instance with Qprc_Init(). Here the callback function pointers
 ** for each interrupt cause can be set in the #stc_qprc_config_t configuration.
 ** The recent position count can be read after Qprc_Init() at any time by
 ** Qprc_GetPositionCount(). The same is valid for the revolution count and
 ** Qprc_GetRevolutionCount().
 **
 ** After initialization the QPRC does not count immediately. Use Qprc_Enable()
 ** to enable the initialized instance and Qprc_Disable() to disable it
 ** temporarily.
 **
 ** To change the position and revolution count Qprc_SetPositionCount() and
 ** Qprc_SetRevolutionCount() can be used. Note that these functions temporarily
 ** disable the counter function for some CPU cycles.
 ** Qprc_SetRevolutionCountCompare(), Qprc_SetPositionRevolutionCountCompare(),
 ** and Qprc_SetMaxPositionCount() can set new values after initialization, if
 ** needed.
 ** The functions Qprc_GetRevolutionCountCompare(),
 ** Qprc_GetPositionRevolutionCountCompare(), and Qprc_GetMaxPositionCount()
 ** return the values of the corresponding registers.
 **
 ** Qprc_DeInit() should be called, if the QPRC instance is no longer needed
 ** at runtime, or a new configuration should be initialized afterwards.
 **
 ** Qprc_GetPositionRevolutionCount() can only be used on >= TYPE3 devices.
 ** It reads out the Position and Revolution Count simultaneously and stores
 ** their values into a pointer structure .
 ** 
 ** \note
 **       - The QPRC shares the interrupt vector with the Dual Timer. Therefore
 **         stc_qprc_config_t#bTouchNVIC determines whether to touch the NVIC
 **         registers or not. Take care, that in the 2nd case, if the Dual Timer
 **         is used, the NVIC initialization and deinitialization must be handeled
 **         by the Dual Timer driver!
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief Qprc callback function prototypes.
 ******************************************************************************/

/******************************************************************************
 * Global definitions
 ******************************************************************************/
#define QPRC0  (*((volatile stc_qprcn_t *) L3_QPRC0_BASE))
#define QPRC1  (*((volatile stc_qprcn_t *) L3_QPRC1_BASE))
#define QPRC2  (*((volatile stc_qprcn_t *) L3_QPRC2_BASE))

/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
 /**
 ******************************************************************************
 ** \brief Redefinition of QPRC registers for including TYPE3 device register
 **        (QPRCRR)
 ******************************************************************************/  
typedef struct stc_qprcn
{
  __IO uint16_t QPCR;
        uint8_t RESERVED0[2];
  __IO uint16_t QRCR;
        uint8_t RESERVED1[2];
  __IO uint16_t QPCCR;
        uint8_t RESERVED2[2];
  __IO uint16_t QPRCR;
        uint8_t RESERVED3[2];
  __IO uint16_t QMPR;
        uint8_t RESERVED4[2];
  union {
    union {
      __IO uint16_t QICR;
      stc_qprc_qicr_field_t QICR_f;
    };
    struct {
      union {
        __IO  uint8_t QICRL;
        stc_qprc_qicrl_field_t QICRL_f;
      };
      union {
        __IO  uint8_t QICRH;
        stc_qprc_qicrh_field_t QICRH_f;
      };
    };
  };
        uint8_t RESERVED5[2];
  union {
    union {
      __IO uint16_t QCR;
      stc_qprc_qcr_field_t QCR_f;
    };
    struct {
      union {
        __IO  uint8_t QCRL;
        stc_qprc_qcrl_field_t QCRL_f;
      };
      union {
        __IO  uint8_t QCRH;
        stc_qprc_qcrh_field_t QCRH_f;
      };
    };
  };
        uint8_t RESERVED6[2];
  union {
    __IO uint16_t QECR;
    stc_qprc_qecr_field_t QECR_f;
  };

        uint8_t RESERVED7[0x1A];
   __IO uint32_t QPRCRR;        /// >= TYPE3 device register
}stc_qprcn_t;    
  
 /**
 ******************************************************************************
 ** \brief Position Counter mode
 ******************************************************************************/
typedef enum en_qprc_pcmode
{
  QprcPcMode0 = 0,  ///< PC_Mode0: Disable position counter
  QprcPcMode1 = 1,  ///< PC_Mode1: Increments with AIN active edge and decrements with BIN active edge
  QprcPcMode2 = 2,  ///< PC_Mode2: Phase difference count mode: Counts up if AIN is leading BIN, down if BIN leading.
  QprcPcMode3 = 3   ///< PC_Mode3: Directional count mode: Counts up/down with BIN active edge and AIN level
} en_qprc_pcmode_t;

/**
 ******************************************************************************
 ** \brief Revolution Counter mode
 ******************************************************************************/
typedef enum en_qprc_rcmode
{
  QprcRcMode0 = 0,  ///< RC_Mode0: Disable revolution counter
  QprcRcMode1 = 1,  ///< RC_Mode1: Up/down count of revolution counter with ZIN active edge
  QprcRcMode2 = 2,  ///< RC_Mode2: Up/down count of revolution counter on over- oder underflow in position count match
  QprcRcMode3 = 3   ///< RC_Mode3: Up/down count of revolution counter on over- oder underflow in position count match and ZIN active edge
} en_qprc_rcmode_t;

/**
 ******************************************************************************
 ** \brief ZIN Usage
 **
 ** The driver checks also the CGSC bit for mode plausibility.
 ** ErrorInvalidParameter is returned, if forbidden mode is tried to set.
 ******************************************************************************/
typedef enum en_qprc_zinedge
{
  QprcZinDisable     = 0,  ///< Disables edge and level detection
  QprcZinFallingEdge = 1,  ///< ZIN active at falling edge
  QprcZinRisingEdge  = 2,  ///< ZIN active at rising edge
  QprcZinBothEdges   = 3,  ///< ZIN active at falling or rising edge
  QprcZinLowLevel    = 4,  ///< ZIN active at low level detected
  QprcZinHighLevel   = 5   ///< ZIN active at high level detected
} en_qprc_zinedge_t;

/**
 ******************************************************************************
 ** \brief BIN Usage
 ******************************************************************************/
typedef enum en_qprc_binedge
{
  QprcBinDisable     = 0,  ///< Disables edge detection
  QprcBinFallingEdge = 1,  ///< BIN active at falling edge
  QprcBinRisingEdge  = 2,  ///< BIN active at rising edge
  QprcBinBothEdges   = 3   ///< BIN active at falling or rising edge
} en_qprc_binedge_t;

/**
 ******************************************************************************
 ** \brief AIN Usage
 ******************************************************************************/
typedef enum en_qprc_ainedge
{
  QprcAinDisable     = 0,  ///< Disables edge detection
  QprcAinFallingEdge = 1,  ///< AIN active at falling edge
  QprcAinRisingEdge  = 2,  ///< AIN active at rising edge
  QprcAinBothEdges   = 3   ///< AIN active at falling or rising edge
} en_qprc_ainedge_t;

/**
 ******************************************************************************
 ** \brief Position Counter Reset Mask
 ******************************************************************************/
typedef enum en_qprc_pcresetmask
{
  QprcResetMaskDisable  = 0,  ///< No reset mask
  QprcResetMask2Times   = 1,  ///< The position counter reset or a revolution counter count-up or -down events are ignored until the position counter changes twice
  QprcResetMask4Times   = 2,  ///< The position counter reset or a revolution counter count-up or -down events are ignored until the position counter changes four times
  QprcResetMask8Times   = 3   ///< The position counter reset or a revolution counter count-up or -down events are ignored until the position counter changes eight times
} en_qprc_pcresetmask_t;

/**
 ******************************************************************************
 ** \brief Qprc register compare mode
 ******************************************************************************/
typedef enum en_qprc_compmode
{
  QprcComapreWithPosition   = 0,  ///< Compares the value of the QPRC Position and Revolution Counter Compare Register (QPRCR) with that of the position counter.
  QprcComapreWithRevolution = 1   ///< Compares the value of the QPRC Position and Revolution Counter Compare Register (QPRCR) with that of the revolution counter.
} en_qprc_compmode_t;

/**
 ******************************************************************************
 ** \brief Qprc clear or gate selection of ZIN pin
 ******************************************************************************/
typedef enum en_qprc_zinmode
{
  QprcZinCounterClear = 0,  ///< ZIN: Counter clear function
  QprcZinGate         = 1   ///< ZIN: Gate function
} en_qprc_zinmode_t;

#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7) || \
    (L3_DEVICE_TYPE == L3_TYPE8)

/**
 ******************************************************************************
 ** \brief Qprc Position and Revolution Counter structure
 ******************************************************************************/
typedef struct stc_qprc_pos_rev_count
{
  uint16_t u16PositionCount;
  uint16_t u16RevolutionCount;
} stc_qprc_pos_rev_count_t;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE3) || ... || (L3_DEVICE_TYPE == L3_TYPE8)

/**
 ******************************************************************************
 ** \brief Qprc configuration.
 **
 ** Contains all parameter for configurating a QPRC instance.  
 ******************************************************************************/
typedef struct stc_qprc_config
{
  en_qprc_pcmode_t      enPcMode;                       ///< See description of #en_qprc_pcmode_t
  en_qprc_rcmode_t      enRcMode;                       ///< See description of #en_qprc_rcmode_t
  boolean_t             bSwapAinBin;                    ///< TRUE: Swap AIN and BIN inputs
  boolean_t             bRegisterFunction;              ///< TRUE: Compare PC, FALSE: Comapre RC
  boolean_t             bGateFunction;                  ///< TRUE: Count gate function, FALSE: Count clear function
  en_qprc_compmode_t    enComapreMode;                  ///< Description see #en_qprc_compmode_t
  en_qprc_zinedge_t     enZinEdge;                      ///< Detection mode of the ZIN pin
  en_qprc_binedge_t     enBinEdge;                      ///< Detection mode of the BIN pin
  en_qprc_ainedge_t     enAinEdge;                      ///< Detection mode of the AIN pin
  en_qprc_zinmode_t     enZinMode;                      ///< TRUE: ZIN Gate function, FALSE: ZIN Counter clear function
  en_qprc_pcresetmask_t enPcResetMask;                  ///< Position counter reset mask
  boolean_t             b8KValue;                       ///< TRUE: Outrange mode from 0 to 0x7FFF, FALSE: Outrange mode from 0 to 0xFFFF: 
  uint16_t              u16PositionCount;               ///< Position Count register update, if QCR.PSTP = 1; not updated, if == 0
  uint16_t              u16RevolutionCount;             ///< Revolution Count register update, if QCR.RCM = 2'b00
  uint16_t              u16PositionCounterCompare;      ///< Position Counter Compare register value
  uint16_t              u16PositionRevolutionCounterCompare; ///< Position and Revolution Counter Compare register value
  uint16_t              u16MaximumPositionCount;        ///< Maximum position count register (increases revolution, if stepped beyond)
  boolean_t             bEnableOutrangeInterrupt;       ///< TRUE: QPRC outrange interrupt is enabled
  boolean_t             bEnableOvrfUndfZIdxInterrupt;   ///< TRUE: Overflow, underflow, or zero index interrupt enabled         
  boolean_t             bEnablePcRcMatchInterrupt;      ///< TRUE: QPRC PC and RC match interrupt is enabled
  boolean_t             bEnablePcMatchRcMatchInterupt;  ///< TRUE: QPRC PC match and RC match interrupt is enabled
  boolean_t             bEnablePcMatchInterrupt;        ///< TRUE: QPRC PC match interrupt is enabled
  boolean_t             bEnableCountInversionInterrupt; ///< TRUE: Count inversion interrupt enabled
  func_ptr_t            pfnCountInversionCallback;      ///< Callback pointer for count inversion interrupt cause
  func_ptr_t            pfnZeroIndexCallback;           ///< Callback pointer for zero index interrupt cause
  func_ptr_t            pfnOverflowCallback;            ///< Callback pointer for overflow interrupt cause
  func_ptr_t            pfnUnderflowCallback;           ///< Callback pointer for underflow interrupt cause
  func_ptr_t            pfnPcRcMatchCallback;           ///< Callback pointer for PC and RC match interrupt cause
  func_ptr_t            pfnPcMatchCallback;             ///< Callback pointer for PC match interrupt cause
  func_ptr_t            pfnPcMatchRcMatchCallback;      ///< Callback pointer for PC match and RC match interrupt cause
  func_ptr_t            pfnOutrangeCallback;            ///< Callback pointer for outrange interrupt cause
  boolean_t             bTouchNVIC;                     ///< TRUE: Touch NVIC registers at Init and DeInit functions
} stc_qprc_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled QPRC instance
typedef enum en_qprc_instance_index
{
#if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
  QprcInstanceIndexQprc0,
#endif
#if (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)
  QprcInstanceIndexQprc1,
#endif
  QprcInstanceIndexMax
} en_qprc_instance_index_t;

/// QPRC module internal data, storing internal information for each enabled QPRC instance.
typedef struct stc_qprc_intern_data
{  
  func_ptr_t  pfnCountInversionCallback; 
  func_ptr_t  pfnZeroIndexCallback; 
  func_ptr_t  pfnOverflowCallback;  
  func_ptr_t  pfnUnderflowCallback; 
  func_ptr_t  pfnPcRcMatchCallback;  
  func_ptr_t  pfnPcMatchCallback;  
  func_ptr_t  pfnPcMatchRcMatchCallback;  
  func_ptr_t  pfnOutrangeCallback;  
} stc_qprc_intern_data_t;

/// QPRC instance data type
typedef struct stc_qprc_instance_data
{
  volatile stc_qprcn_t*  pstcInstance;  ///< pointer to registers of an instance
  stc_qprc_intern_data_t stcInternData; ///< module internal data of instance
} stc_qprc_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled MFS instances and their internal data
extern stc_qprc_instance_data_t m_astcQprcInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void QprcIrqHandler ( stc_qprcn_t* pstcQprc, 
                             stc_qprc_intern_data_t* pstcQprcInternData );

extern en_result_t Qprc_Init( stc_qprcn_t*        pstcQprc, 
                              stc_qprc_config_t*  pstcConfig );

extern en_result_t Qprc_DeInit( stc_qprcn_t*       pstcQprc, 
                               stc_qprc_config_t*  pstcConfig );

extern void Qprc_SetPositionCount( stc_qprcn_t* pstcQprc,
                                   uint16_t     u16PcValue ) ;

extern void Qprc_SetRevolutionCount( stc_qprcn_t* pstcQprc,
                                     uint16_t     u16RcValue ) ;

extern void Qprc_SetRevolutionCountCompare( stc_qprcn_t* pstcQprc,
                                            uint16_t     u16RcCompValue ) ;

extern void Qprc_SetPositionRevolutionCountCompare( stc_qprcn_t* pstcQprc,
                                                    uint16_t     u16PcRcCompValue ) ;

extern void Qprc_SetMaxPositionCount( stc_qprcn_t* pstcQprc,
                                      uint16_t     u16PcMaxValue ) ;

extern uint16_t Qprc_GetPositionCount( stc_qprcn_t* pstcQprc ) ;

extern uint16_t Qprc_GetRevolutionCount( stc_qprcn_t* pstcQprc ) ;

extern uint16_t Qprc_GetRevolutionCountCompare( stc_qprcn_t* pstcQprc ) ;

extern uint16_t Qprc_GetPositionRevolutionCountCompare( stc_qprcn_t* pstcQprc ) ;

extern uint16_t Qprc_GetMaxPositionCount( stc_qprcn_t* pstcQprc ) ;

extern void Qprc_Enable( stc_qprcn_t* pstcQprc ) ;

extern void Qprc_Disable( stc_qprcn_t* pstcQprc ) ;

#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7) || \
    (L3_DEVICE_TYPE == L3_TYPE8)

extern void Qprc_GetPositionRevolutionCount( stc_qprcn_t*              pstcQprc,
                                             stc_qprc_pos_rev_count_t* pstcPosRevCount ) ;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE3) || ... || (L3_DEVICE_TYPE == L3_TYPE8)

//@} // QprcGroup

#ifdef __cplusplus
}
#endif

#endif /* __QPRC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
