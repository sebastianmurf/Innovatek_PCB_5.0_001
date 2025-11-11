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
/** \file dma.c
 **
 ** A detailed description is available at 
 ** @link DmaGroup DMA description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version
 **   - 2012-04-11  1.1  MWi  Configuration-less Set/De-Init functions added
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "dma.h"

#if (defined(L3_PERIPHERAL_DMA_ACTIVE))

  #if  (L3_DEVICE_TYPE == L3_TYPE3)
    #error Type3 devices do not support DMA!
  #endif

/**
 ******************************************************************************
 ** \addtogroup DmaGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/
// DMACAn => FM3_DMAC_BASE + size of register set + offset of DMACA0
// DMACAn => 0x40060000    + 0x10 * n             + 0x10
#define GET_DMA_ADDRESS(x) (FM3_DMAC_BASE + (0x10 * x) + 0x10)

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
stc_dma_intern_data_t stcDmaInternData;

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
 ** \brief DMA interrupt handler
 **
 ** \param [in]  u8DmaChannel     Channel number 
 **
 ** \note depending on the DMA stop status the DmaCallback function is called
 **       on successful transfer or DmaErrorCallback function on errornous
 **       transfer. The ErrorCallback has the error code as an (uint8_t)
 **       argument which reflects the 3 bits of the stop status.
 ******************************************************************************/
void DmaIrqHandler(uint8_t u8DmaChannel)
{
  uint32_t u32DmaRegisterBaseAddress;
  uint32_t u32RegisterReadOut;
  uint8_t  u8StopStatus;
  
  u32DmaRegisterBaseAddress = GET_DMA_ADDRESS(u8DmaChannel);
  
  // Readout DMACB and update
  u32RegisterReadOut = *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACB_OFFSET) ;
  
  u8StopStatus = (uint8_t)((u32RegisterReadOut & 0x00070000)>> 16);
  
  // Clear interrupt cause
  u32RegisterReadOut &= 0xFFF8FFFF;     
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACB_OFFSET) 
    = u32RegisterReadOut;  
  
  if (0x5 == u8StopStatus)  // Successful DMA transfer
  {
    stcDmaInternData.apfnDmaCallback[u8DmaChannel](); 
  }
  else                      // Errornous DMA transfer
  {
    stcDmaInternData.apfnDmaErrorCallback[u8DmaChannel](u8StopStatus);
  }
}

/**
 ******************************************************************************
 ** \brief Init DMA interrupt
 **
 ** Sets up an DMA error/completion interrupt.
 **
 ** \param [in]  u8DmaChannel     Channel number 
 **
 ** \retval Ok                    init successful
 **
 ******************************************************************************/
en_result_t Dma_Irq_Init(uint8_t u8DmaChannel)
{
  switch(u8DmaChannel)
  {
    case 0:
      NVIC_ClearPendingIRQ(DMAC0_IRQn);
      NVIC_EnableIRQ(DMAC0_IRQn);
      NVIC_SetPriority(DMAC0_IRQn, L3_DMA0_LEVEL);    
    break;
    case 1:
      NVIC_ClearPendingIRQ(DMAC1_IRQn);
      NVIC_EnableIRQ(DMAC1_IRQn);
      NVIC_SetPriority(DMAC1_IRQn, L3_DMA1_LEVEL);    
    break;
    case 2:
      NVIC_ClearPendingIRQ(DMAC2_IRQn);
      NVIC_EnableIRQ(DMAC2_IRQn);
      NVIC_SetPriority(DMAC2_IRQn, L3_DMA2_LEVEL);    
    break;
    case 3:
      NVIC_ClearPendingIRQ(DMAC3_IRQn);
      NVIC_EnableIRQ(DMAC3_IRQn);
      NVIC_SetPriority(DMAC3_IRQn, L3_DMA3_LEVEL);    
    break;
    case 4:
      NVIC_ClearPendingIRQ(DMAC4_IRQn);
      NVIC_EnableIRQ(DMAC4_IRQn);
      NVIC_SetPriority(DMAC4_IRQn, L3_DMA4_LEVEL);    
    break;
    case 5:
      NVIC_ClearPendingIRQ(DMAC5_IRQn);
      NVIC_EnableIRQ(DMAC5_IRQn);
      NVIC_SetPriority(DMAC5_IRQn, L3_DMA5_LEVEL);    
    break;
    case 6:
      NVIC_ClearPendingIRQ(DMAC6_IRQn);
      NVIC_EnableIRQ(DMAC6_IRQn);
      NVIC_SetPriority(DMAC6_IRQn, L3_DMA6_LEVEL);    
    break;
    case 7:
      NVIC_ClearPendingIRQ(DMAC7_IRQn);
      NVIC_EnableIRQ(DMAC7_IRQn);
      NVIC_SetPriority(DMAC7_IRQn, L3_DMA7_LEVEL);    
    break;    
  }
  return Ok;
}

/**
 ******************************************************************************
 ** \brief De-Init DMA interrupt
 **
 ** Sets up an DMA error/completion interrupt.
 **
 ** \param [in]  u8DmaChannel     Channel number 
 **
 ** \retval Ok                    de-init successful
 **
 ******************************************************************************/
en_result_t Dma_Irq_DeInit(uint8_t u8DmaChannel)
{
  switch(u8DmaChannel)
  {
    case 0:
      NVIC_ClearPendingIRQ(DMAC0_IRQn);
      NVIC_DisableIRQ(DMAC0_IRQn);
      NVIC_SetPriority(DMAC0_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 1:
      NVIC_ClearPendingIRQ(DMAC1_IRQn);
      NVIC_DisableIRQ(DMAC1_IRQn);
      NVIC_SetPriority(DMAC1_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 2:
      NVIC_ClearPendingIRQ(DMAC2_IRQn);
      NVIC_DisableIRQ(DMAC2_IRQn);
      NVIC_SetPriority(DMAC2_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 3:
      NVIC_ClearPendingIRQ(DMAC3_IRQn);
      NVIC_DisableIRQ(DMAC3_IRQn);
      NVIC_SetPriority(DMAC3_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 4:
      NVIC_ClearPendingIRQ(DMAC4_IRQn);
      NVIC_DisableIRQ(DMAC4_IRQn);
      NVIC_SetPriority(DMAC4_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 5:
      NVIC_ClearPendingIRQ(DMAC5_IRQn);
      NVIC_DisableIRQ(DMAC5_IRQn);
      NVIC_SetPriority(DMAC5_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 6:
      NVIC_ClearPendingIRQ(DMAC6_IRQn);
      NVIC_DisableIRQ(DMAC6_IRQn);
      NVIC_SetPriority(DMAC6_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;
    case 7:
      NVIC_ClearPendingIRQ(DMAC7_IRQn);
      NVIC_DisableIRQ(DMAC7_IRQn);
      NVIC_SetPriority(DMAC7_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
    break;    
  }
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Initializes a DMA channel
 **
 ** Sets up an DMA channel without starting immediate DMA transfer.
 ** Enable_Dma_Channel() is used for starting DMA transfer.
 **
 ** \param [in]  pstcConfig       DMA module configuration 
 **
 ** \retval Ok                    init successful
 ** \retval ErrorInvalidParameter pstcAdc == NULL or other invalid configuration
 ** \retval OperationInProgress   DMA channel already in use
 **
 ******************************************************************************/
en_result_t Dma_Init_Channel(volatile stc_dma_config_t* pstcConfig)                   
{
  stcDMACA_t             stcDMACA;
  stcDMACB_t             stcDMACB;
  stc_dmac_dmacr_field_t unDMACR;
  uint32_t        u32DmaRegisterBaseAddress;
  func_ptr_t      pfnCallBackPointer;
  func_ptr_arg1_t pfnErrorCallBackPointer;

  // Check for NULL pointer
  if (pstcConfig == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  stcDMACA.u32DMACA = 0;
  stcDMACB.u32DMACB = 0;

  u32DmaRegisterBaseAddress = GET_DMA_ADDRESS(pstcConfig->u8DmaChannel);
  
  // Set Callback function pointers
  pfnCallBackPointer = pstcConfig->pfnCallback;
  stcDmaInternData.apfnDmaCallback[pstcConfig->u8DmaChannel] = pfnCallBackPointer;
  
  pfnErrorCallBackPointer = pstcConfig->pfnErrorCallback;
  stcDmaInternData.apfnDmaErrorCallback[pstcConfig->u8DmaChannel] = pfnErrorCallBackPointer;
  
  // Check if DMA channel already used
  if (stcDmaInternData.au8DmaChannelActive[pstcConfig->u8DmaChannel] == DMA_CHANNEL_ACTIVE)
  {
    return OperationInProgress;
  }
  
  // Set DMA channel as active
  stcDmaInternData.au8DmaChannelActive[pstcConfig->u8DmaChannel] = DMA_CHANNEL_ACTIVE;
  
  // Setup DMACA ...
  //
  // Block Count
  stcDMACA.DMACA_f.BC = 0x0F & pstcConfig->u8BlockCount;
  
  // Transfer Count
  stcDMACA.DMACA_f.TC = 0xFFFF & pstcConfig->u16TransferCount;
  
  // Interrupt Request Channel
  if (pstcConfig->enDmaIdrq == Software)
  {
    stcDMACA.DMACA_f.IS = 0;
  }
  else
  {
    stcDMACA.DMACA_f.IS = 0x20 | (0x1F &  pstcConfig->enDmaIdrq);
  }
  
  // ... and update hardware
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) = stcDMACA.u32DMACA ;
  
  // Setup DMACB ...
  //
  // First Transfer mode
  switch (pstcConfig->enTransferMode)
  {
    case DmaBlockTransfer:
      stcDMACB.DMACB_f.MS = 0;
      break;
    case DmaBurstTransfer:
      stcDMACB.DMACB_f.MS = 1;
      break;
    case DmaDemandTransfer:
      stcDMACB.DMACB_f.MS = 2;
      break;
    default :
      return ErrorInvalidParameter;
  }
  
  // Transfer width
  switch (pstcConfig->enTransferWdith)
  {
    case Dma8Bit:
      stcDMACB.DMACB_f.TW = 0;
      break;
    case Dma16Bit:
      stcDMACB.DMACB_f.TW = 1;
      break;
    case Dma32Bit:
      stcDMACB.DMACB_f.TW = 2;
      break;
    default :
      return ErrorInvalidParameter;
  }
 
  // Fixed source
  stcDMACB.DMACB_f.FS = (pstcConfig->bFixedSource == TRUE) ? 1 : 0 ;
  
  // Fixed destination
  stcDMACB.DMACB_f.FD = (pstcConfig->bFixedDestination == TRUE) ? 1 : 0 ;
  
  // Reload Count (BC/TC reload)
  stcDMACB.DMACB_f.RC = (pstcConfig->bReloadCount == TRUE) ? 1 : 0 ;
  
  // Reload Source
  stcDMACB.DMACB_f.RS = (pstcConfig->bReloadSource == TRUE) ? 1 : 0 ;
  
  // Reload Destination
  stcDMACB.DMACB_f.RD = (pstcConfig->bReloadDestination == TRUE) ? 1 : 0 ;
  
  // Error interrupt enable
  stcDMACB.DMACB_f.EI = (pstcConfig->bErrorInterruptEnable == TRUE) ? 1 : 0 ;
  
  // Completion interrupt enable
  stcDMACB.DMACB_f.CI = (pstcConfig->bCompletionInterruptEnable == TRUE) ? 1 : 0 ;
  
  if (   (pstcConfig->bErrorInterruptEnable == TRUE)
      || (pstcConfig->bCompletionInterruptEnable == TRUE)
     )
  {
    Dma_Irq_Init(pstcConfig->u8DmaChannel);
  }
  
  // Enable bit mask
  stcDMACB.DMACB_f.EM = (pstcConfig->bEnableBitMask == TRUE) ? 1 : 0 ;
  
  // ... and update hardware
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACB_OFFSET)
    = stcDMACB.u32DMACB ;
  
  // Setup source address
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMASA_OFFSET)
    = pstcConfig->u32SourceAddress;
  
  // Setup destination address
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMADA_OFFSET)
    = pstcConfig->u32DestinationAddress;
  
  // Switch resource interrupt to DMA (except software DMA)
  if (pstcConfig->enDmaIdrq != Software)
  {
    uint32_t u32DrqselBit = (1 << pstcConfig->enDmaIdrq);
    FM3_INTREQ->DRQSEL |= u32DrqselBit;
  }
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Enable, disable, pause, or trigger a DMA channel via configuration
 **
 ** This function enables, disables, pauses or triggers a DMA transfer according
 ** to the settings in the configuration bits for EB (Enable), PB (Pause) and
 ** ST (Software Trigger)
 **
 ** \param [in]  pstcConfig       DMA module configuration 
 **
 ** \retval Ok                    Setting finished
 **
 ******************************************************************************/
en_result_t Dma_Set_Channel(volatile stc_dma_config_t* pstcConfig)
{
  stcDMACA_t stcDMACA;
  uint32_t u32RegisterReadOut ;
  uint32_t u32DmaRegisterBaseAddress ;

  // Check for NULL pointer
  if (pstcConfig == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  stcDMACA.u32DMACA = 0;
  stcDMACA.DMACA_f.EB = (pstcConfig->bEnable == TRUE) ? 1 : 0 ;
  stcDMACA.DMACA_f.PB = (pstcConfig->bPause == TRUE) ? 1 : 0 ;
  stcDMACA.DMACA_f.ST = (pstcConfig->bSoftwareTrigger == TRUE) ? 1 : 0 ;
  
  u32DmaRegisterBaseAddress = GET_DMA_ADDRESS(pstcConfig->u8DmaChannel);
  
  // Readout DMACA and update
  u32RegisterReadOut = *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) ;
  stcDMACA.u32DMACA |= (u32RegisterReadOut & 0x1FFFFFFF);   // masking EB, PB, ST ...
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) = stcDMACA.u32DMACA ;
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Enable DMA globally
 **
 ** \retval Ok                    Enable finished
 **
 ******************************************************************************/
en_result_t Dma_Enable(void)
{
  FM3_DMAC->DMACR = 0x80000000; /// Set DE Bit (DMA enable all channels)
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Disable DMA globally
 **
 ** \retval Ok                    Disable finished
 **
 ******************************************************************************/
en_result_t Dma_Disable(void)
{
  FM3_DMAC->DMACR = 0; /// Clear everything
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief De-Initializes a DMA channel
 **
 ** Clears an DMA channel.
 **
 ** \param [in]  pstcConfig       DMA module configuration 
 **
 ** \retval Ok                    init successful
 ** \retval ErrorInvalidParameter pstcAdc == NULL or other invalid configuration
 **
 ******************************************************************************/
en_result_t Dma_DeInit_Channel(volatile stc_dma_config_t* pstcConfig)  
{
  uint32_t u32DmaRegisterBaseAddress;
  
  // Check for NULL pointer
  if (pstcConfig == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  Dma_Irq_DeInit(pstcConfig->u8DmaChannel);
  
  u32DmaRegisterBaseAddress = GET_DMA_ADDRESS(pstcConfig->u8DmaChannel);
  
  // clear all registers of DMA channel
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) = 0;
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACB_OFFSET) = 0;
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMASA_OFFSET) = 0;
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMADA_OFFSET) = 0;

  // Mark DMA channel as inactive
  stcDmaInternData.au8DmaChannelActive[pstcConfig->u8DmaChannel]
    = DMA_CHANNEL_INACTIVE;
  
  return Ok;
} // Dma_DeInit_Channel

/**
 ******************************************************************************
 ** \brief Enable, disable, pause, or trigger a DMA channel via arguments
 **
 ** This function enables, disables, pauses or triggers a DMA transfer according
 ** to the settings in the configuration bits for EB (Enable), PB (Pause) and
 ** ST (Software Trigger)
 **
 ** \param [in]  u8DmaChannel         DMA channel to be set
 ** \param [in]  bEnable              TRUE = Channel enabled, FALSE = Channel
 **                                   disabled
 ** \param [in]  bPause               TRUE = Channel paused, FALSE = Channel
 **                                   working
 ** \param [in]  bSoftwareTrigger     TRUE = Channel trigger, FALSE = Channel
 **                                   no trigger
 **
 ** \retval Ok                        Setting finished
 **
 ******************************************************************************/
en_result_t Dma_Set_ChannelParam( uint8_t   u8DmaChannel,
                                  boolean_t bEnable,
                                  boolean_t bPause,
                                  boolean_t bSoftwareTrigger
                                )
{
  stcDMACA_t stcDMACA;
  uint32_t   u32RegisterReadOut ;
  uint32_t   u32DmaRegisterBaseAddress ;
  
  stcDMACA.u32DMACA = 0;
  stcDMACA.DMACA_f.EB = (bEnable          == TRUE) ? 1 : 0 ;
  stcDMACA.DMACA_f.PB = (bPause           == TRUE) ? 1 : 0 ;
  stcDMACA.DMACA_f.ST = (bSoftwareTrigger == TRUE) ? 1 : 0 ;
  
  u32DmaRegisterBaseAddress = GET_DMA_ADDRESS(u8DmaChannel);
  
  // Readout DMACA and update
  u32RegisterReadOut = *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) ;
  stcDMACA.u32DMACA |= (u32RegisterReadOut & 0x1FFFFFFF);   // masking EB, PB, ST ...
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) = stcDMACA.u32DMACA ;
  
  return Ok;
} // Dma_Set_ChannelParam

/**
 ******************************************************************************
 ** \brief De-Initializes a DMA channel via channel parameter
 **
 ** Clears an DMA channel.
 **
 ** \param [in]  u8DmaChannel     DMA channel to be de-initialized
 **
 ** \retval Ok                    init successful
 ** \retval ErrorInvalidParameter pstcAdc == NULL or other invalid configuration
 **
 ******************************************************************************/
en_result_t Dma_DeInit_ChannelParam(uint8_t u8DmaChannel)  
{
  uint32_t u32DmaRegisterBaseAddress;
  
  Dma_Irq_DeInit(u8DmaChannel);
  
  u32DmaRegisterBaseAddress = GET_DMA_ADDRESS(u8DmaChannel);
  
  // clear all registers of DMA channel
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACA_OFFSET) = 0;
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMACB_OFFSET) = 0;
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMASA_OFFSET) = 0;
  *(uint32_t*)(u32DmaRegisterBaseAddress + DMA_DMADA_OFFSET) = 0;

  // Mark DMA channel as inactive
  stcDmaInternData.au8DmaChannelActive[u8DmaChannel]
    = DMA_CHANNEL_INACTIVE;
  
  return Ok;
} // Dma_DeInit_ChannelParam

//@} // DmaGroup

#endif // #if (defined(L3_PERIPHERAL_DMA_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

