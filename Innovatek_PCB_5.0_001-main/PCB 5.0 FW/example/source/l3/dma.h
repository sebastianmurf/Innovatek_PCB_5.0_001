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
/** \file dma.h
 **
 ** Headerfile for DMA functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version
 **   - 2012-04-11  1.1  MWi  Configuration-less Set/De-Init functions added
 **
 ******************************************************************************/

#ifndef __DMA_H__
#define __DMA_H__

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
 ** \defgroup DmaGroup Direct Memory Access (DMA)
 **
 ** Provided functions of DMA module:
 ** 
 ** - Dma_Init_Channel()
 ** - Dma_Set_Channel()
 ** - Dma_Enable()
 ** - Dma_Disable()
 ** - Dma_DeInit_Channel()
 ** - Dma_Set_ChannelParam()
 ** - Dma_DeInit_ChannelParam()
 **
 ** The DMA is configured by Dma_Init_Channel() but not started then.
 ** With the function Dma_Set_Channel() the enable, pause and/or trigger
 ** bits can be set.
 ** Dma_Enable() enables globally the DMA and Dma_Disable() disables DMA
 ** globally.
 ** Dma_DeInit_Channel() clears a channel for a possible new configuration.
 **
 ** Once a DMA channel was setup by Dma_Init_Channel() it cannot be 
 ** re-initialzied by this function (with a new configuration) anymore.
 ** result_t#OperationInProgress ist returned in this case. Dma_DeInit_Channel()
 ** has to be called before to unlock the channel for a new configuration.
 **
 ** Dma_Set_ChannelParam() and Dma_DeInit_ChannelParam() perform the same
 ** functionality as Dma_Set_Channel() and Dma_DeInit_Channel() instead of
 ** configuration usage. Here direct arguments are used.
 **
 ** \note Set stc_dma_config_t#u16TransferCount to "Number of Transfers - 1"!
 **
 ******************************************************************************/
//@{
  
/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
 
// register address offset
#define DMA_DMACA_OFFSET 0x00
#define DMA_DMACB_OFFSET 0x04
#define DMA_DMASA_OFFSET 0x08
#define DMA_DMADA_OFFSET 0x0C
  
// DMA channel active markers for internal data:
// au8DmaChannelActive[DMA_MAX_CHANNELS]
#define DMA_CHANNEL_ACTIVE   1
#define DMA_CHANNEL_INACTIVE 0
  
/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Redefinition of DMA DMACA register
 ******************************************************************************/  
typedef struct stc_dmac_dmaca_field
{
  uint32_t TC         : 16;
  uint32_t BC         : 4;
  uint32_t RESERVED1  : 3;
  uint32_t IS         : 6;
  uint32_t ST         : 1;
  uint32_t PB         : 1;
  uint32_t EB         : 1;
} stc_dmac_dmaca_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DMA DMACB register
 ******************************************************************************/ 
typedef struct stc_dmac_dmacb_field
{
  uint32_t EM         : 1;
  uint32_t RESERVED1  : 15;
  uint32_t SS         : 3;
  uint32_t CI         : 1;
  uint32_t EI         : 1;
  uint32_t RD         : 1;
  uint32_t RS         : 1;
  uint32_t RC         : 1;
  uint32_t FD         : 1;
  uint32_t FS         : 1;
  uint32_t TW         : 2;
  uint32_t MS         : 2;
  uint32_t RESERVED2  : 2;
} stc_dmac_dmacb_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of DMA single channel register structure
 ******************************************************************************/
typedef struct stc_dmac {
  union {
    uint32_t DMACA;
    stc_dmac_dmaca_field_t u32DMACA;
  };
  union {
    uint32_t DMACB;
    stc_dmac_dmacb_field_t u32DMACB0;
  };
  uint32_t DMACSA;
  uint32_t DMACDA;  
} stc_dmac_t ;
  
typedef struct{
  union{
    uint32_t               u32DMACA;
    stc_dmac_dmaca_field_t DMACA_f;
  };
} stcDMACA_t;

typedef struct{
  union{
    uint32_t               u32DMACB;
    stc_dmac_dmacb_field_t DMACB_f;
  };
} stcDMACB_t;

/**
 ******************************************************************************
 ** \brief DMA transfer data width
 ******************************************************************************/
typedef enum en_dma_transfermode
{
  DmaBlockTransfer  = 0,
  DmaBurstTransfer  = 1,
  DmaDemandTransfer = 2 
} en_dma_transfermode_t ;

/**
 ******************************************************************************
 ** \brief DMA transfer data width
 ******************************************************************************/
typedef enum en_dma_transferwidth
{
  Dma8Bit  = 0,  ///< 8 bit transfer via DMA
  Dma16Bit = 1,  ///< 16 bit transfer via DMA
  Dma32Bit = 2   ///< 32 bit transfer via DMA
} en_dma_transferwidth_t ;

/**
 ******************************************************************************
 ** \brief DMA IDREQ number
 ******************************************************************************/
typedef enum en_dma_idreq
{
#if (L3_DEVICE_TYPE != L3_TYPE2)
    Ep1DrqUsb0  = 0,   ///< Interrupt signal from EP1 DRQ of USB ch.0
    Ep2DrqUsb0  = 1,   ///< Interrupt signal from EP2 DRQ of USB ch.0
    Ep3DrqUsb0  = 2,   ///< Interrupt signal from EP3 DRQ of USB ch.0
    Ep4DrqUsb0  = 3,   ///< Interrupt signal from EP4 DRQ of USB ch.0
    Ep5DrqUsb0  = 4,   ///< Interrupt signal from EP5 DRQ of USB ch.0
    Adc0        = 5,   ///< Scan conversion interrupt signal from A/D converter unit0
    Adc1        = 6,   ///< Scan conversion interrupt signal from A/D converter unit1
    Adc2        = 7,   ///< Scan conversion interrupt signal from A/D converter unit2
    Bt0Irq0     = 8,   ///< Interrupt signal from IRQ0 of base timer ch.0
    Bt2Irq0     = 9,   ///< Interrupt signal from IRQ0 of base timer ch.2
    Bt4Irq0     = 10,  ///< Interrupt signal from IRQ0 of base timer ch.4
    Bt6Irq0     = 11,  ///< Interrupt signal from IRQ0 of base timer ch.6
    MfsRx0      = 12,  ///< Receiving interrupt signal from MFS ch.0
    MfsTx0      = 13,  ///< Sending interrupt signal from MFS ch.0   
    MfsRx1      = 14,  ///< Receiving interrupt signal from MFS ch.1
    MfsTx1      = 15,  ///< Sending interrupt signal from MFS ch.1
    MfsRx2      = 16,  ///< Receiving interrupt signal from MFS ch.2
    MfsTx2      = 17,  ///< Sending interrupt signal from MFS ch.2
    MfsRx3      = 18,  ///< Receiving interrupt signal from MFS ch.3
    MfsTx3      = 19,  ///< Sending interrupt signal from MFS ch.3 
    MfsRx4      = 20,  ///< Receiving interrupt signal from MFS ch.4
    MfsTx4      = 21,  ///< Sending interrupt signal from MFS ch.4 
    MfsRx5      = 22,  ///< Receiving interrupt signal from MFS ch.5
    MfsTx5      = 23,  ///< Sending interrupt signal from MFS ch.5 
    MfsRx6      = 24,  ///< Receiving interrupt signal from MFS ch.6
    MfsTx6      = 25,  ///< Sending interrupt signal from MFS ch.6 
    MfsRx7      = 26,  ///< Receiving interrupt signal from MFS ch.7
    MfsTx7      = 27,  ///< Sending interrupt signal from MFS ch.7 
    ExtIrq0     = 28,  ///< Interrupt signal from external interrupt unit ch.0
    ExtIrq1     = 29,  ///< Interrupt signal from external interrupt unit ch.1    
    ExtIrq2     = 30,  ///< Interrupt signal from external interrupt unit ch.2
    ExtIrq3     = 31,  ///< Interrupt signal from external interrupt unit ch.3 
    Software    = 1234 ///< Software Demand (just a high number)
#else
  #error Type2 device DMA not supported! Workaround: Use enumeration table \
         above, if extension selector is not used.
#endif
} en_dma_idreq_t ;

/**
 ******************************************************************************
 ** \brief Datatype for holding internal data needed for DMA
 ******************************************************************************/
typedef struct stc_dma_intern_data
{
  uint8_t         au8DmaChannelActive[L3_DMA_CHANNELS];   ///< Channel active marker array
  func_ptr_t      apfnDmaCallback[L3_DMA_CHANNELS];       ///< Callback pointer array
  func_ptr_arg1_t apfnDmaErrorCallback[L3_DMA_CHANNELS];  ///< Error Callback pointer array

} stc_dma_intern_data_t ;

/**
 ******************************************************************************
 ** \brief DMA instance data type
 ******************************************************************************/
typedef struct stc_dma_instance_data
{
  stc_dma_intern_data_t stcInternData; ///< module internal data of instance
} stc_dma_instance_data_t;

/**
 ******************************************************************************
 ** \brief DMA configuration
 ******************************************************************************/
typedef struct stc_dma_config
{
  boolean_t              bEnable ;               ///< Overall DMA channel enable (only set by Enable_Dma_Channel())
  boolean_t              bPause ;                ///< Pause bit of DMA (only set by Enable_Dma_Channel())
  boolean_t              bSoftwareTrigger ;      ///< Trigger bit for software transfer (only set by Enable_Dma_Channel())
  uint8_t                u8DmaChannel ;          ///< DmaChannel for recent configuration
  en_dma_idreq_t         enDmaIdrq ;             ///< ID Request number (see #en_dma_idreq_t for details)
  uint8_t                u8BlockCount ;          ///< Block counter
  uint16_t               u16TransferCount ;      ///< Transfer counter, set <b>TC = Number of Transfers - 1</b>!
  en_dma_transfermode_t  enTransferMode ;        ///< DMA transfer mode (see #en_dma_transfermode_t for details)
  en_dma_transferwidth_t enTransferWdith ;       ///< DMA transfer width (see #en_dma_transferwidth_t for details)
  uint32_t               u32SourceAddress;       ///< Source address
  uint32_t               u32DestinationAddress ; ///< Destination address
  boolean_t              bFixedSource ;          ///< TRUE = source address not increased
  boolean_t              bFixedDestination ;     ///< TRUE = destiantion address not increased
  boolean_t              bReloadCount ;          ///< TRUE = count is reloaded
  boolean_t              bReloadSource ;         ///< TRUE = source address is reloaded
  boolean_t              bReloadDestination ;    ///< TRUE = destination address is reloaded
  boolean_t              bErrorInterruptEnable ; ///< TRUE = interrupt on error ocurrance
  boolean_t              bCompletionInterruptEnable ; ///< TRUE = interrupt on DMA tranfer completion
  boolean_t              bEnableBitMask ;        ///< FALSE = Clear EB (bEnable) bit on completion (mandatory for transfer end!)
  func_ptr_t             pfnCallback ;           ///< Callback on sucessful DMA transfer
  func_ptr_arg1_t        pfnErrorCallback ;      ///< Callback(uint8_t) on errornous DMA transfer
  uint8_t                u8ErrorStopStatus ;     ///< Error code from Stop Status
} stc_dma_config_t ;


/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern stc_dma_intern_data_t stcDmaInternData;

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/  

extern void DmaIrqHandler(uint8_t u8DmaChannel) ;

en_result_t Dma_Init_Channel(volatile stc_dma_config_t* pstcConfig) ;

en_result_t Dma_Set_Channel(volatile stc_dma_config_t* pstcConfig) ;

en_result_t Dma_Enable(void) ;

en_result_t Dma_Disable(void) ;

en_result_t Dma_DeInit_Channel(volatile stc_dma_config_t* pstcConfig) ;

en_result_t Dma_Set_ChannelParam( uint8_t   u8DmaChannel,
                                  boolean_t bEnable,
                                  boolean_t bPause,
                                  boolean_t bSoftwareTrigger
                                ) ;

en_result_t Dma_DeInit_ChannelParam(uint8_t u8DmaChannel) ;
  
//@} // DmaGroup

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
