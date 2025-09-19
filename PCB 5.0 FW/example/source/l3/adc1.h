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
/** \file adc.h
 **
 ** Headerfile for ADC functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  MWi  Pointer to user configuration removed in
 **                           #stc_adc_intern_data_t and necessary elements
 **                           added; L3 address definition for instance pointer
 **                           used
 **
 ******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__

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
 ** \defgroup AdcGroup Analog to Digital Converter (ADC)
 **
 ** Provided functions of ADC module:
 ** 
 ** - Adc_Init()
 ** - Adc_DeInit() 
 ** - Adc_TriggerStart()
 ** - Adc_ConversionStatus()
 ** - Adc_StoreData()
 ** - Adc_PurgeFifo()
 ** - Adc_PurgePrioFifo()
 ** - Adc_GetStatus()
 **
 ** \brief This section describes the interface for the Analog to 
 **        Digital Converter (ADC).
 **
 ** Each ADC channel (max. 16) gets an own buffer, which has to be set-up by 
 ** the user application. These buffers and their states are defined in the
 ** ADC configuration #stc_adc_config_t.
 ** The buffer pointers are collected in
 ** stc_adc_config_t#pu8ChannelBufferPointer[], if 8 Bit conversion method is
 ** used or stc_adc_config_t#pu16ChannelBufferPointer[], if 12 Bit conversion
 ** method is used.
 ** The recent buffer index is stored in
 ** stc_adc_config_t#u16ChannelBufferCounter. This value has to be initialized
 ** to 0 by the user or to be set to any value less than the buffer size.
 ** The end index has to be stored in
 ** stc_adc_config_t#u16ChannelBufferEndCount. This value can be the end
 ** of the buffer or any value between the recent index and the buffer size.
 **
 ** If no interrupts are used, init the ADC first, start the conversion
 ** via Adc_TriggerStart(), the status can be polled by Adc_ConversionStatus().
 ** The buffer(s) then have to be filled by Adc_StoreData()
 **
 ** If interrupts or DMA is used, init ADC first and start the conversion via
 ** Adc_TriggerStart(), the callback function referenced in
 ** stc_adc_config_t#pfnCallback then indicates the complete filled buffer(s).
 **
 ** Adc_PurgeFifo() and Adc_PurgePrioFifo() can be called, if an error (FIFO
 ** overrun) occurred. All remaining data will get lost in this case!
 ** Nevertheless the overrun flags are cleared automatically by the ADC
 ** interrupt handler before the error callback is used (if defined).
 **
 ** \note 
 **       - The FM3 ADC supports only DMA in scan conversion mode.
 **       - If DMA transfer is selected only full-12-bit conversion mode is
 **         supported!
 **       - If DMA transfer is selected only one common buffer can be used for
 **         each activated channel (HW restriction)! Recently all 32-bit data
 **         (including ADC value and channel number) are stored in an 32-bit
 **         buffer.
 **       - Using priority scan mode, interrupts must be enabled and DMA must
 **         be disabled. The user has to take care, that the priority scan
 **         buffer is big enough (according priority FIFO scan interrupt
 **         depth).
 **       - Type3 devices do not support DMA.
 **         Thus en_adc_interruptsdma_t#AdcDma is not available if type3 is
 **         selected.
 **       - Since L3 version 1.1 only Adc_Init() need a pointer to the user
 **         configuration #stc_adc_config_t.
 **
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief DtCallback function prototype.
 ******************************************************************************/
typedef void (*adc_cb_func_ptr_t)(void);

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
#define stc_adcn_t   FM3_ADC_TypeDef
#define ADC0         (*((FM3_ADC_TypeDef *) L3_ADC0_BASE))
#define ADC1         (*((FM3_ADC_TypeDef *) L3_ADC1_BASE))
#define ADC2         (*((FM3_ADC_TypeDef *) L3_ADC2_BASE))

#define pu8ADCEN0 *((volatile uint8_t*)(0x4002703CUL))
#define pu8ADCEN1 *((volatile uint8_t*)(0x4002713CUL))
#define pu8ADCEN2 *((volatile uint8_t*)(0x4002723CUL))

#define pu16ADCEN0 *((volatile uint16_t*)(0x4002703CUL))
#define pu16ADCEN1 *((volatile uint16_t*)(0x4002713CUL))
#define pu16ADCEN2 *((volatile uint16_t*)(0x4002723CUL))

#define ADC_MIN_COMPARISON_TIME          0x1
#define ADC_MAX_COMPARISON_TIME         0x3F

#define ADC_MIN_SAMPLING_TIME            0x1
#define ADC_MAX_SAMPLING_TIME          0x3FF


/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief ADC conversion mode
 ** 
 ** To select between
 ** - Scan Conversion
 ** - Priority Conversion
 ** 
 ** \note Only one sampling time (ADST = 0) setting is recently implemented
 **
 ******************************************************************************/
typedef enum en_adc_conversionmode
{
  AdcModeScanConversion              = 0,  ///< Scan Conversion
  AdcModePriorityConversion          = 1   ///< Priority Conversion
} en_adc_conversionmode_t;

/**
 ******************************************************************************
 ** \brief ADC sampling time multiplier
 ** 
 ** Selects the sampling timing multiplier for the ADC sampling time
 **
 ** \note Only setting one smapling time (for ADSS = 0) are used here
 ******************************************************************************/
typedef enum en_adc_samplingtimemultiplier
{
  AdcSamplingTimeMultiplier1   = 0x00, ///< Sampling time multiplied by 1
  AdcSamplingTimeMultiplier4   = 0x20, ///< Sampling time multiplied by 4
  AdcSamplingTimeMultiplier8   = 0x40, ///< Sampling time multiplied by 5
  AdcSamplingTimeMultiplier16  = 0x50, ///< Sampling time multiplied by 16
  AdcSamplingTimeMultiplier32  = 0x80, ///< Sampling time multiplied by 32
  AdcSamplingTimeMultiplier64  = 0xA0, ///< Sampling time multiplied by 64
  AdcSamplingTimeMultiplier128 = 0xC0, ///< Sampling time multiplied by 128
  AdcSamplingTimeMultiplier256 = 0xE0  ///< Sampling time multiplied by 256
} en_adc_samplingtimemultiplier_t ;

/**
 ******************************************************************************
 ** \brief ADC Resolution
 ******************************************************************************/
typedef enum en_adc_resolution
{
  Adc12Bit = 0,   ///< Conversin result 12 bit data width
  Adc8Bit  = 1,   ///< Conversin result 8 bit data width
} en_adc_resolution_t;

/**
 ******************************************************************************
 ** \brief ADC usage for polling, interrupts, DMA
 ******************************************************************************/
typedef enum en_adc_interruptsdma
{
  AdcPolling    = 0, ///< Polling until conversion done
  AdcInterrupts = 1, ///< Interrupt after conversion (including Callback)
  
  // Type3 devices do not support DMA
#if  (L3_DEVICE_TYPE != L3_TYPE3)
  AdcDma        = 2  ///< DMA transfer, interupt after transfer (including Callback)
#endif
    
} en_adc_interruptsdma_t;

/**
 ******************************************************************************
 ** \brief ADC priority conversion interrupt for FIFO level
 ******************************************************************************/
typedef enum en_adc_prio_irq_level
{
  AdcPrioIrqFifoLevel1 = 0,   ///< Generates an interrupt request when conversion result is stored in the first FIFO stage
  AdcPrioIrqFifoLevel2 = 1,   ///< Generates an interrupt request when conversion result is stored in the second FIFO stage
  AdcPrioIrqFifoLevel3 = 2,   ///< Generates an interrupt request when conversion result is stored in the third FIFO stage
  AdcPrioIrqFifoLevel4 = 3    ///< Generates an interrupt request when conversion result is stored in the fourth FIFO stage
} en_adc_prio_irq_level_t;


/**
 ******************************************************************************
 ** \brief ADC status enumaration
 ******************************************************************************/
typedef enum en_adc_status
{
  AdcStandby                         = 0, ///< Standby for A/D conversion.
  AdcScanInProgress                  = 1, ///< Scan A/D conversion is in progress.
  AdcPrioInProgress                  = 2, ///< Priority A/D conversion (priority level 1 or 2) is in progress.
  AdcPrioInProgressScanPending       = 3, ///< Priority A/D conversion (priority level 1 or 2) is in progress. Scan conversion is pending.
  AdcPrio1InProgressPrio2Pending     = 4, ///< Priority A/D conversion (priority level 1) is in progress. Priority conversion (priority level 2) is pending.
  AdcPrio1InProgressPrio2ScanPending = 5, ///< Priority A/D conversion (priority level 1) is in progress. Scan conversion and priority conversion (priority level 2) are pending.
  AdcErrorUnknownState               = 6  ///< No vaild ADC pointer used or unknown state
} en_adc_status_t;

/**
 ******************************************************************************
 ** \brief ADC Channel list with same sort order as the ER32 registers
 ******************************************************************************/
typedef union stc_ad_channel_list
{
  union
  {
    uint32_t AD_CHn ;
    struct 
    {
      uint32_t AD_CH_0  : 1 ;
      uint32_t AD_CH_1  : 1 ;
      uint32_t AD_CH_2  : 1 ;
      uint32_t AD_CH_3  : 1 ;
      uint32_t AD_CH_4  : 1 ;
      uint32_t AD_CH_5  : 1 ;
      uint32_t AD_CH_6  : 1 ;
      uint32_t AD_CH_7  : 1 ;
      uint32_t AD_CH_8  : 1 ;
      uint32_t AD_CH_9  : 1 ;
      uint32_t AD_CH_10 : 1 ;
      uint32_t AD_CH_11 : 1 ;
      uint32_t AD_CH_12 : 1 ;
      uint32_t AD_CH_13 : 1 ;
      uint32_t AD_CH_14 : 1 ;
      uint32_t AD_CH_15 : 1 ;
      uint32_t AD_CH_16 : 1 ;
      uint32_t AD_CH_17 : 1 ;
      uint32_t AD_CH_18 : 1 ;
      uint32_t AD_CH_19 : 1 ;
      uint32_t AD_CH_20 : 1 ;
      uint32_t AD_CH_21 : 1 ;
      uint32_t AD_CH_22 : 1 ;
      uint32_t AD_CH_23 : 1 ;
      uint32_t AD_CH_24 : 1 ;
      uint32_t AD_CH_25 : 1 ;
      uint32_t AD_CH_26 : 1 ;
      uint32_t AD_CH_27 : 1 ;
      uint32_t AD_CH_28 : 1 ;
      uint32_t AD_CH_29 : 1 ;
      uint32_t AD_CH_30 : 1 ;
      uint32_t AD_CH_31 : 1 ;
    };
  };
} stc_ad_channel_list_t ;

/**
 ******************************************************************************
 ** \brief ADC configuration
 ******************************************************************************/
typedef struct stc_adc_config
{
  stc_ad_channel_list_t    u32CannelSelect;               ///< Selects channels for ADC (1 = active)
  en_adc_conversionmode_t  enConversionMode;              ///< Selects the conversion mode, see #en_adc_conversionmode_t
  uint8_t                  u8SamplingTime;                ///< Sampling time (see HW manual for proper calculation).
  en_adc_samplingtimemultiplier_t 
                           enSamplingTimeMultiplier;      ///< Sampling Time Multiplier, see #en_adc_samplingtimemultiplier_t
  uint8_t                  u8ComparisonTimeDivider;       ///< Frequency division ratio for comparison time
  uint8_t                  u8ComparisonTime;              ///< Comparison time (see HW manual for proper calculation).
  en_adc_resolution_t      enResolution;                  ///< Enable 8bit/12bit resolution.
  en_adc_interruptsdma_t   enUseInterruptsDma;            ///< See #en_adc_interruptsdma_t for details
  uint8_t*                 pu8ChannelBufferPointer[L3_ADC_CHANNELS];   ///< Pointer list to channel buffers (8 bit resolution)
  uint16_t*                pu16ChannelBufferPointer[L3_ADC_CHANNELS];  ///< Pointer list to channel buffers (12 bit resolution)
  uint8_t*                 pu8PriorityBufferPointer[L3_ADC_CHANNELS];  ///< Pointer list to priority channel buffers (8 bit resolution)
  uint16_t*                pu16PriorityBufferPointer[L3_ADC_CHANNELS]; ///< Pointer list to priority channel buffers (12 bit resolution)  
  uint32_t*                pu32DmaBufferPointer;          ///< Pointer to DMA (plain) data
  uint16_t                 u16ChannelBufferCounter;       ///< Counter of channel buffers
  uint16_t                 u16ChannelBufferEndCount;      ///< Maximum buffer size or end count
  boolean_t                bExternalTriggerAnalogInput;   ///< TRUE: Set External trigger analog input selection bit
  boolean_t                bPriorityConversionExternalStart; ///< TRUE: Priority conversion external start enable bit
  boolean_t                bPriorityConversionTimerStart; ///< TRUE: Priority conversion timer start enable bit
  en_adc_prio_irq_level_t  enPriorityConversionIrqLevel;  ///< See #en_adc_prio_irq_level_t for details
  uint8_t                  u8PrioLevel1AnalogInputChannel; ///< Priority level 1 analog input selection ch0..7
  uint8_t                  u8PrioLevel2AnalogInputChannel; ///< Priority level 2 analog input selection ch0..31
  boolean_t                bComparisonEnable;             ///< TRUE: Enables comparison function
  boolean_t                bComparisionGreaterEqual;      ///< TRUE: Interupt, if ADC value >= CMPD, FALSE: ADC value < CMPD
  boolean_t                bComparisonChannel;            ///< TRUE: Enables channel compaison
  uint8_t                  u8ComparisonChannel;           ///< Defined channel of comparison function, if enabled; ch0..31  
  uint16_t                 u16ComparisonValue;            ///< Comparison value
  uint8_t                  u8EnableCycleValue;            ///< For device types != 0: Enable cycle setting. Use 0xFF as default.
  func_ptr_t               pfnCallback;                   ///< End of conversion callback (buffer full)
  func_ptr_t               pfnErrorCallback;              ///< Error during conversion
  func_ptr_t               pfnPriorityCallbackAdc;        ///< Priority scan callback for ADC interrupt
  func_ptr_t               pfnPrioErrorCallbackAdc;       ///< Priority FIFO overrun error callback pointer
} stc_adc_config_t;

/**
 ******************************************************************************
 ** \brief Enumeration to define an index for each enabled ADC instance
 ******************************************************************************/
typedef enum en_adc_instance_index
{
  #if (L3_PERIPHERAL_ENABLE_ADC0 == L3_ON)
    AdcInstanceIndexAdc0,
  #endif
  #if (L3_PERIPHERAL_ENABLE_ADC1 == L3_ON)
    AdcInstanceIndexAdc1,
  #endif
  #if (L3_PERIPHERAL_ENABLE_ADC2 == L3_ON)
    AdcInstanceIndexAdc2,
  #endif
  AdcInstanceIndexMax
} en_adc_instance_index_t;		   

/**
 ******************************************************************************
 ** \brief Datatype for holding internal data needed for ADC
 ******************************************************************************/
typedef struct stc_adc_intern_data
{
  en_adc_resolution_t    enResolution;             ///< Enable 8bit/12bit resolution.
  en_adc_interruptsdma_t enUseInterruptsDma;       ///< See #en_adc_interruptsdma_t for details
  uint8_t*               pu8ChannelBufferPointer[L3_ADC_CHANNELS];   ///< Pointer list to channel buffers (8 bit resolution)
  uint16_t*              pu16ChannelBufferPointer[L3_ADC_CHANNELS];  ///< Pointer list to channel buffers (12 bit resolution)
  uint8_t*               pu8PriorityBufferPointer[L3_ADC_CHANNELS];  ///< Pointer list to priority channel buffers (8 bit resolution)
  uint16_t*              pu16PriorityBufferPointer[L3_ADC_CHANNELS]; ///< Pointer list to priority channel buffers (12 bit resolution)  
  uint16_t               u16ChannelBufferCounter;  ///< Counter of channel buffers
  uint16_t               u16ChannelBufferEndCount; ///< Maximum buffer size or end count
  uint8_t                u8ActiveChannels ;        ///< number of enabled channels
  uint8_t                u8ChannelCount;           ///< recent channel for storing data
  uint8_t                u8FifoDepth;              ///< Scan conversion FIFO depth
  func_ptr_t             pfnCallbackAdc ;          ///< Callback for scan conversion interrupts of ADC
  func_ptr_t             pfnErrorCallbackAdc ;     ///< Error callback for ADC interrupt
  func_ptr_t             pfnPriorityCallbackAdc ;  ///< Priority scan callback for ADC interrupt
  func_ptr_t             pfnPrioErrorCallbackAdc;  ///< Priority FIFO overrun error callback pointer
} stc_adc_intern_data_t ;

/**
 ******************************************************************************
 ** \brief ADC instance data type
 ******************************************************************************/
typedef struct stc_adc_instance_data
{
  volatile stc_adcn_t*  pstcInstance;  ///< pointer to registers of an instance
  stc_adc_intern_data_t stcInternData; ///< module internal data of instance
} stc_adc_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled ADC instances and their internal data
extern stc_adc_instance_data_t m_astcAdcInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void AdcIrqHandler( volatile stc_adcn_t*   pstcAdc,
                           stc_adc_intern_data_t* pstcAdcInternData
                          ) ;

en_result_t Adc_Init( volatile stc_adcn_t* pstcAdc,
                      stc_adc_config_t* pstcConfig ) ;

en_result_t Adc_DeInit( volatile stc_adcn_t* pstcAdc ) ;

en_result_t Adc_TriggerStart( volatile stc_adcn_t* pstcAdc ) ;

en_result_t Adc_ConversionStatus( volatile stc_adcn_t* pstcAdc ) ;

en_result_t Adc_StoreData( volatile stc_adcn_t* pstcAdc ) ;

en_result_t Adc_PurgeFifo( volatile stc_adcn_t* pstcAdc ) ;

en_result_t Adc_PurgePrioFifo( volatile stc_adcn_t* pstcAdc ) ;

en_adc_status_t Adc_GetStatus( volatile stc_adcn_t* pstcAdc ) ;

//@} // AdcGroup

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
