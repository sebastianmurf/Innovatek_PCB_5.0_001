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
/** \file adc.c
 **
 ** A detailed description is available at 
 ** @link AdcGroup ADC Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  MWi  Pointer to user configuration removed in
 **                           #stc_adc_intern_data_t and necessary elements
 **                           added; TYPE4/5/6/7 added
 **   - 2012-07-02  1.2  MWi  Adc_PurgeFifo() and Adc_PurgePrioFifo() corrected
 **   - 2012-08-10  1.3  MWi  Bug fix in AdcIrqHandler()
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "adc.h"

#if (defined(L3_PERIPHERAL_ADC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup AdcGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

///  Macro to return the number of enabled ADC instances
#define ADC_INSTANCE_COUNT (uint32_t)(sizeof(m_astcAdcInstanceDataLut) / sizeof(m_astcAdcInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled ADC instances and their internal data
stc_adc_instance_data_t m_astcAdcInstanceDataLut[] =
{
    #if (L3_PERIPHERAL_ENABLE_ADC0 == L3_ON)
    {
      &ADC0,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_ADC1 == L3_ON)
    {
      &ADC1,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_ADC2 == L3_ON)
    {
      &ADC2,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
};

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
 ** \brief Return the internal data for a certain ADC instance.
 **
 ** \param pstcAdc Pointer to ADC instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled
 **         (or not known)
 **
 ******************************************************************************/
static stc_adc_intern_data_t* AdcGetInternDataPtr(volatile stc_adcn_t* pstcAdc) 
{
    uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < ADC_INSTANCE_COUNT; u32Instance++)
    {
        if (pstcAdc == m_astcAdcInstanceDataLut[u32Instance].pstcInstance)
        {
            return &m_astcAdcInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}

/**
 ******************************************************************************
 ** \brief Device dependent initialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcAdc Pointer to ADC instance
 **
 ** \retval Ok     Successful initialization
 **
 ******************************************************************************/
static en_result_t Adc_InitIrq( volatile stc_adcn_t* pstcAdc ) 
{
  #if (L3_PERIPHERAL_ENABLE_ADC0 == L3_ON)
  if (pstcAdc == &ADC0)
  {
    NVIC_ClearPendingIRQ(ADC0_IRQn);
    NVIC_EnableIRQ(ADC0_IRQn);
    NVIC_SetPriority(ADC0_IRQn, L3_IRQ_LEVEL_ADC0);
  }
  #endif
  #if (L3_PERIPHERAL_ENABLE_ADC1 == L3_ON)
  if (pstcAdc == &ADC1)
  {
    NVIC_ClearPendingIRQ(ADC1_IRQn);
    NVIC_EnableIRQ(ADC1_IRQn);
    NVIC_SetPriority(ADC1_IRQn, L3_IRQ_LEVEL_ADC1);
  }
  #endif
  #if (L3_PERIPHERAL_ENABLE_ADC2 == L3_ON)
  if (pstcAdc == &ADC2)
  {
    NVIC_ClearPendingIRQ(ADC2_IRQn);
    NVIC_EnableIRQ(ADC2_IRQn);
    NVIC_SetPriority(ADC2_IRQn, L3_IRQ_LEVEL_ADC2);
  }
  #endif
  
  return Ok;
} // Adc_InitIrq

/**
 ******************************************************************************
 ** \brief Device dependent de-initialization of interrupts according CMSIS 
 **
 ** \param pstcAdc Pointer to ADC instance
 **
 ** \retval Ok     Successful initialization
 **
 ******************************************************************************/
static en_result_t Adc_DeInitIrq( volatile stc_adcn_t* pstcAdc ) 
{
  #if (L3_PERIPHERAL_ENABLE_ADC0 == L3_ON)
  if (pstcAdc == &ADC0)
  {
    NVIC_ClearPendingIRQ(ADC0_IRQn);
    NVIC_DisableIRQ(ADC0_IRQn);
    NVIC_SetPriority(ADC0_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  #if (L3_PERIPHERAL_ENABLE_ADC1 == L3_ON)
  if (pstcAdc == &ADC1)
  {
    NVIC_ClearPendingIRQ(ADC1_IRQn);
    NVIC_DisableIRQ(ADC1_IRQn);
    NVIC_SetPriority(ADC1_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  #if (L3_PERIPHERAL_ENABLE_ADC2 == L3_ON)
  if (pstcAdc == &ADC2)
  {
    NVIC_ClearPendingIRQ(ADC2_IRQn);
    NVIC_DisableIRQ(ADC2_IRQn);
    NVIC_SetPriority(ADC2_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif 
  
  return Ok;
} // Adc_DeInitIrq

/**
 ******************************************************************************
 ** \brief Get number of enabled channels
 **
 ** \param [in]  pstcConfig       ADC module configuration 
 **
 ** \retval     Number of enabled Channels
 ******************************************************************************/
static uint8_t AdcGetNumberOfEnabledChannels( stc_adc_config_t* pstcConfig )
{
  uint8_t  u8EnabledChannels = 0;
  uint8_t  u8BitCount;
  uint32_t u32ChannelMask;
  
  u32ChannelMask = pstcConfig->u32CannelSelect.AD_CHn;
  
  u8BitCount = 32;
  
  while(u8BitCount--)
  {
    if ((u32ChannelMask & 0x0001) == 0x0001)
    {
      u8EnabledChannels++;
    }
    u32ChannelMask >>= 1;
  }
  
  return u8EnabledChannels;
} // AdcGetNumberOfEnabledChannels

/**
 ******************************************************************************
 ** \brief ISR for ADC instance
 **
 ** The active interrupt request flags are cleared by the ISR. If an error
 ** occurs the error callback function is called.
 **
 ** If scan conversion and interrupts are enabled the channel data collected
 ** in the FIFO is stored in the corresponding data buffers. The maximum
 ** FIFO depth is always used, if the remaining data to be converted are
 ** greater or equal to the maximum FIFO depth. If the remaining data is
 ** less, then the remaining data count is set to the FIFO interrupt trigger,
 ** so that after conversion of the last data, this ISR is called, and the
 ** scan conversion can be stopped. In this case the (none-error) callback
 ** function is called, if configured.
 **
 ** \param [in] pstcAdc           ADC instance
 ** \param [in] pstcAdcInternData Internal data associated with the ADC instance
 ******************************************************************************/
void AdcIrqHandler( volatile stc_adcn_t*   pstcAdc,
                    stc_adc_intern_data_t* pstcAdcInternData
                   )
{
  uint8_t  u8ByteShortFactor;
  uint8_t  u8Channel;
  uint16_t u16ResultLow;
  uint16_t u16ResultHigh;
  uint32_t u32BufferAddress;
  uint32_t u32DataRemaining;
  
  if (pstcAdc->SCCR_f.SOVR)   // Error case (FIFO overflow)
  {
    pstcAdc->SCCR_f.SOVR = 0;
    
    if (pstcAdcInternData->pfnErrorCallbackAdc != NULL)
    {
       pstcAdcInternData->pfnErrorCallbackAdc() ;
    }
  }
  else if (pstcAdc->PCCR_f.POVR)  // Error case (Priority FIFO overflow)
  {
    pstcAdc->PCCR_f.POVR = 0;
    
    if (pstcAdcInternData->pfnPrioErrorCallbackAdc != NULL)
    {
       pstcAdcInternData->pfnPrioErrorCallbackAdc() ;
    }
  }
  else if (pstcAdc->ADCR_f.SCIF)   // Scan conversion interrupt request?
  {
    pstcAdc->ADCR_f.SCIF = 0;
    
    // Interrupts configured?
    if (pstcAdcInternData->enUseInterruptsDma == AdcInterrupts)
    {
      u8ByteShortFactor = (pstcAdcInternData->enResolution == Adc8Bit) ? 1 : 2 ; 
    
      // Read out complete FIFO and update buffers
      while(pstcAdcInternData->u8FifoDepth--)
      {
        u16ResultLow  = pstcAdc->SCFDL;
        u16ResultHigh = pstcAdc->SCFDH;
    
        u8Channel = (uint8_t) 0x001F & u16ResultLow;
    
        if (pstcAdcInternData->u16ChannelBufferCounter <
           (pstcAdcInternData->u16ChannelBufferEndCount))
        {
          if (pstcAdcInternData->enResolution == Adc8Bit)
          {
            u32BufferAddress = (uint32_t) pstcAdcInternData->pu8ChannelBufferPointer[u8Channel];
            u32BufferAddress += pstcAdcInternData->u16ChannelBufferCounter;
            *(uint8_t*) u32BufferAddress = (uint8_t) (u16ResultHigh >> 8);        
          }
          else
          {
            u32BufferAddress = (uint32_t) pstcAdcInternData->pu16ChannelBufferPointer[u8Channel];
            u32BufferAddress += pstcAdcInternData->u16ChannelBufferCounter * u8ByteShortFactor;
            *(uint16_t*) u32BufferAddress = (uint16_t) u16ResultHigh;
          }
        }
        
        pstcAdcInternData->u8ChannelCount--;
        if (0 == pstcAdcInternData->u8ChannelCount)   // all channels converted?
        {
          pstcAdcInternData->u8ChannelCount = pstcAdcInternData->u8ActiveChannels;        
          pstcAdcInternData->u16ChannelBufferCounter++;
        }
      } // while(pstcAdcInternData->u8FifoDepth--)
      
      // Calculate remaining data
      u32DataRemaining = pstcAdcInternData->u8ActiveChannels
          * (pstcAdcInternData->u16ChannelBufferEndCount 
          - pstcAdcInternData->u16ChannelBufferCounter);
      
      if (u32DataRemaining == 0)      // Last data stored above?
      {
        // Stop conversion
        pstcAdc->SCCR_f.RPT = 0;
      }
      else if (u32DataRemaining < L3_ADC_FIFO_DEPTH) // Remaining data < max. FIFO depth?
      {
        pstcAdc->SFNS = u32DataRemaining - 1;
        pstcAdcInternData->u8FifoDepth = u32DataRemaining;
      }
      else                            // Remaining data >= max. FIFO depth
      {
        pstcAdc->SFNS = u32DataRemaining = L3_ADC_FIFO_DEPTH - 1;
        pstcAdcInternData->u8FifoDepth = L3_ADC_FIFO_DEPTH;
      }
    } // if (pstcAdcInternData->pstcUserConfiguration->enUseInterruptsDma == AdcInterrupts)
  
    // Check for callback function pointer and (buffer full or DMA)
    if ((pstcAdcInternData->pfnCallbackAdc != NULL)
        && (   (u32DataRemaining == 0)
#if (L3_DEVICE_TYPE != L3_TYPE3)
            || (pstcAdcInternData->enUseInterruptsDma == AdcDma)
#endif
           )
        )
    {
      pstcAdcInternData->pfnCallbackAdc() ;
    }  
  } // else if (pstcAdc->ADCR_f.SCIF)
  else if (pstcAdc->ADCR_f.PCIF)  // Priority Conversion
  {
    pstcAdc->ADCR_f.PCIF = 0;
    
    while (pstcAdc->PCCR_f.PEMP != 0) // Data in Priority FIFO
    {
      u16ResultLow  = pstcAdc->PCFDL;
      u16ResultHigh = pstcAdc->PCFDH;
    
      u8Channel = (uint8_t) 0x001F & u16ResultLow;
      
      if (pstcAdcInternData->enResolution == Adc8Bit)
      {
        u32BufferAddress = (uint32_t) pstcAdcInternData->pu8PriorityBufferPointer[u8Channel];
        *(uint8_t*) u32BufferAddress = (uint8_t) (u16ResultHigh >> 8);   
        *(uint8_t*) u32BufferAddress++;
      }
      else
      {
        u32BufferAddress = (uint32_t) pstcAdcInternData->pu16PriorityBufferPointer[u8Channel];
        *(uint16_t*) u32BufferAddress = (uint16_t) u16ResultHigh;
        *(uint16_t*) u32BufferAddress++;
      }
    }
  }
} // AdcIrqHandler

/**
 ******************************************************************************
 ** \brief Initialize ADC
 **
 ** This function initializes an ADC module and sets up the internal
 ** data structures
 **
 ** \param [in]  pstcAdc          ADC instance
 ** \param [in]  pstcConfig       ADC module configuration 
 **
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_Init( volatile stc_adcn_t* pstcAdc,
                      stc_adc_config_t* pstcConfig )
{
  stc_adc_adsr_field_t unADSR = { 0 } ; // local predefinition of ADSR
  stc_adc_sccr_field_t unSCCR = { 0 } ; // local predefinition of SCCR
  stc_adc_pccr_field_t unPCCR = { 0 } ; // local predefinition of PCCR
  
  uint32_t u32DataAmount;
  uint8_t  u8Counter;

  // Pointer to internal data
  stc_adc_intern_data_t* pstcAdcInternData ;  
  
  // Check for NULL pointer
  if ((pstcAdc    == NULL) ||
      (pstcConfig == NULL)
     )
  {
    return ErrorInvalidParameter ;
  }
  
  // Disable ADC in any case first
  pstcAdc->ADCEN_f.ENBL = 0;
  
  // Clear all relevant registers
  pstcAdc->ADCR = 0;
  pstcAdc->ADSR = 0;
  pstcAdc->SCCR = 0;
  
  // Get pointer to internal data structure ...
  pstcAdcInternData = AdcGetInternDataPtr( pstcAdc ) ;
  // ... and check for NULL
  if ( pstcAdcInternData != NULL )
  {
      pstcAdcInternData->pfnCallbackAdc          = NULL;
      pstcAdcInternData->pfnErrorCallbackAdc     = NULL;
      pstcAdcInternData->pfnPriorityCallbackAdc  = NULL;
      pstcAdcInternData->pfnPrioErrorCallbackAdc = NULL;
  }
  
#if (L3_DEVICE_TYPE != L3_TYPE3)
  // Check invalid configuration mode
  if (pstcConfig->enUseInterruptsDma == AdcDma)
  {
    if (   (pstcConfig->enConversionMode != AdcModeScanConversion) 
        || (pstcConfig->enResolution     != Adc12Bit)
       )
    {
      return ErrorInvalidParameter ; // DMA only allowed in 12-bit Scan Conversion mode
    }
  }
#endif
  
  // Set ADC input pin configuration
  #if (L3_ADC_CHANNELS == 16)
    // Device has only 16 AD channels
    FM3_GPIO->ADE = (uint16_t)(0x0000FFFF & pstcConfig->u32CannelSelect.AD_CHn);
  #else
    #error ADC channel number not supported!
  #endif

  // Set internal data according number of enabled channels
  pstcAdcInternData->u8ActiveChannels = AdcGetNumberOfEnabledChannels(pstcConfig); 
  pstcAdcInternData->u8ChannelCount = pstcAdcInternData->u8ActiveChannels;
         
  // Input selection according user channel activation
  pstcAdc->SCIS01 = (uint16_t)(0x0000FFFF & pstcConfig->u32CannelSelect.AD_CHn);
  pstcAdc->SCIS23 = (uint16_t)(0x0000FFFF & (pstcConfig->u32CannelSelect.AD_CHn >> 16));
  
  // Set sampling time
  pstcAdc->ADSS01 = 0;  // only one setting for sampling time implemented
  pstcAdc->ADSS23 = 0;  // only one setting for sampling time implemented
  
  if (pstcConfig->u8SamplingTime < 1)
  {
    return ErrorInvalidParameter ;
  }  
  pstcAdc->ADST0 = (pstcConfig->enSamplingTimeMultiplier) |
                   (pstcConfig->u8SamplingTime - 1) ;
  
  // Set comparison time
  #if (L3_DEVICE_TYPE == L3_TYPE0) || \
      (L3_DEVICE_TYPE == L3_TYPE2) || \
      (L3_DEVICE_TYPE == L3_TYPE4) || \
      (L3_DEVICE_TYPE == L3_TYPE5)
    if (   (pstcConfig->u8ComparisonTimeDivider < 2) 
        || (pstcConfig->u8ComparisonTimeDivider > 9)
       )
  #elif (L3_DEVICE_TYPE == L3_TYPE1) || \
        (L3_DEVICE_TYPE == L3_TYPE3) || \
        (L3_DEVICE_TYPE == L3_TYPE6) || \
        (L3_DEVICE_TYPE == L3_TYPE7)
    if (   (pstcConfig->u8ComparisonTimeDivider < 1) 
        || (pstcConfig->u8ComparisonTimeDivider > 65)
       )   
  #else
    #error Device Type not defined!  
  #endif
  {
    return ErrorInvalidParameter ;
  }
  
  if (pstcConfig->u8ComparisonTimeDivider == 1) // can only be TYPE1 device!
  {
     pstcAdc->ADCT = 0x80;
  }
  else
  {
     pstcAdc->ADCT = (0x3F & (pstcConfig->u8ComparisonTimeDivider - 2));
  }
  
  // Set FIFO result alignment for easier data handling due to data width
  unADSR.FDAS = (pstcConfig->enResolution == Adc8Bit) ? 0 : 1 ; 
  
  // Mode dependent intitialization
  switch (pstcConfig->enConversionMode)
  {
    case AdcModeScanConversion :
      pstcAdc->ADSR_f        = unADSR ; // Set data alignment
      pstcAdc->ADCR_f.SCIE   = 0 ;      // No interrupts during init
      unSCCR.SFCLR           = 1 ;      // Clear FIFO, ...
      unSCCR.RPT             = 1 ;      //   set repeat conversion mode, ...
      pstcAdc->SCCR_f        = unSCCR ; //   and setup hardware   
      
      if (pstcConfig->enUseInterruptsDma == AdcInterrupts)
      {
        // Check amount of sampling data according FIFO depth
        u32DataAmount = pstcConfig->u16ChannelBufferEndCount * pstcAdcInternData->u8ActiveChannels;
        
        if (u32DataAmount > L3_ADC_FIFO_DEPTH)
        {
          pstcAdc->SFNS = L3_ADC_FIFO_DEPTH - 1; // Maximum, set new level in ISR, if count < 16
          pstcAdcInternData->u8FifoDepth = L3_ADC_FIFO_DEPTH;
        }
        else
        {
          pstcAdc->SFNS = u32DataAmount - 1; // FIFO can hold each data 
          pstcAdcInternData->u8FifoDepth = u32DataAmount;
        }
        
        pstcAdc->ADCR_f.SCIE  = 1;     // Scan conversion interrupt enable
        pstcAdc->ADCR_f.OVRIE = 1;     // FIFO overrun error interupt enable
       
        // Set call back function pointer
        pstcAdcInternData->pfnCallbackAdc = pstcConfig->pfnCallback;
        pstcAdcInternData->pfnErrorCallbackAdc = pstcConfig->pfnErrorCallback;
      }
      
    // Type3 devices do not support DMA!
    #if (L3_DEVICE_TYPE != L3_TYPE3)
      else if (pstcConfig->enUseInterruptsDma == AdcDma)
      {
        pstcAdc->ADCR_f.SCIE = 1; // Irq -> DMA
        pstcAdc->SFNS = 0;        // FIFO depth to 0 for immediate results
        
        // Set callback function pointer
        pstcAdcInternData->pfnCallbackAdc = pstcConfig->pfnCallback;
        pstcAdcInternData->pfnErrorCallbackAdc = pstcConfig->pfnErrorCallback;        
      }
    #endif
      
      else
      {
        pstcAdc->ADCR_f.SCIE = 0; // No interrupt
        pstcAdc->SFNS = 0;        // FIFO depth to 0 for immediate results
      }
      
      break;
  
    case AdcModePriorityConversion :
      
      // Only interrupt mode allowed if priority scan conversion is selected
      if (pstcConfig->enUseInterruptsDma != AdcInterrupts)
      {
        return ErrorInvalidParameter ;
      }
      
      pstcAdc->ADSR_f        = unADSR ; // Set data alignment
      pstcAdc->ADCR_f.SCIE   = 0 ;      // No Scan interrupts during init
      pstcAdc->ADCR_f.PCIE   = 0 ;      // No Priority scan interrupts during init
      unSCCR.SFCLR           = 1 ;      // Clear FIFO, ...
      unPCCR.PFCLR           = 1 ;
      unSCCR.RPT             = 1 ;      //   set repeat conversion mode, ...
      pstcAdc->SCCR_f        = unSCCR ; //   and setup hardware   
      pstcAdc->PCCR_f        = unPCCR ; //   and setup hardware   
      
      // Check amount of sampling data according FIFO depth
      u32DataAmount = pstcConfig->u16ChannelBufferEndCount * pstcAdcInternData->u8ActiveChannels;
      
      if (u32DataAmount > L3_ADC_FIFO_DEPTH)
      {
        pstcAdc->SFNS = L3_ADC_FIFO_DEPTH - 1; // Maximum, set new level in ISR, if count < 16
        pstcAdcInternData->u8FifoDepth = L3_ADC_FIFO_DEPTH;
      }
      else
      {
        pstcAdc->SFNS = u32DataAmount - 1; // FIFO can hold each data 
        pstcAdcInternData->u8FifoDepth = u32DataAmount;
      }
      
      pstcAdc->PCCR_f.ESCE = (pstcConfig->bExternalTriggerAnalogInput      == TRUE) ? 1 : 0 ;
      pstcAdc->PCCR_f.PEEN = (pstcConfig->bPriorityConversionExternalStart == TRUE) ? 1 : 0 ;
      pstcAdc->PCCR_f.PHEN = (pstcConfig->bPriorityConversionTimerStart    == TRUE) ? 1 : 0 ;
      
      pstcAdc->ADCR_f.SCIE  = 1;     // Scan conversion interrupt enable
      pstcAdc->ADCR_f.PCIE  = 1;     // Priority scan conversion interrupt enable
      pstcAdc->ADCR_f.OVRIE = 1;     // FIFO overrun error interupt enable  

      switch (pstcConfig->enPriorityConversionIrqLevel)
      {
        case AdcPrioIrqFifoLevel1:
          pstcAdc->PFNS = 0;
          break;
        case AdcPrioIrqFifoLevel2:
          pstcAdc->PFNS = 1;
          break;   
        case AdcPrioIrqFifoLevel3:
          pstcAdc->PFNS = 2;
          break;
        case AdcPrioIrqFifoLevel4:
          pstcAdc->PFNS = 3;
          break;
        default:
          return ErrorInvalidParameter ;
      }
      
      if (pstcConfig->u8PrioLevel1AnalogInputChannel > 7)
      {
        return ErrorInvalidParameter ;
      }
      
      if (pstcConfig->u8PrioLevel2AnalogInputChannel > 31)
      {
        return ErrorInvalidParameter ;
      }
      
      pstcAdc->PCIS = ((pstcConfig->u8PrioLevel2AnalogInputChannel) << 3) |
                       (pstcConfig->u8PrioLevel1AnalogInputChannel);
      
      // Set callback function pointer
      pstcAdcInternData->pfnCallbackAdc          = pstcConfig->pfnCallback;
      pstcAdcInternData->pfnErrorCallbackAdc     = pstcConfig->pfnErrorCallback;
      pstcAdcInternData->pfnPriorityCallbackAdc  = pstcConfig->pfnPriorityCallbackAdc;
      pstcAdcInternData->pfnPrioErrorCallbackAdc = pstcConfig->pfnPrioErrorCallbackAdc; 
      break;
      
    default :
     return ErrorInvalidParameter ;
  } // switch (pstcConfig->enConversionMode)
  
  if (pstcConfig->bComparisonEnable == TRUE)
  {
    pstcAdc->CMPCR_f.CMPEN = 1 ; 
    pstcAdc->CMPCR_f.CMD1 = (pstcConfig->bComparisionGreaterEqual) ? 1 : 0 ;
    pstcAdc->CMPCR_f.CMD0 = (pstcConfig->bComparisonChannel) ? 1 : 0 ;
    
    if (pstcConfig->u8ComparisonChannel > 31)
    {
      return ErrorInvalidParameter ;
    }
    
    pstcAdc->CMPCR |= pstcConfig->u8ComparisonChannel;
    
    // CMPD:  31   30   29   28   27   26   25   24   23   22   21   20   19   18   17   16
    //       CV11 CV10 CV09 CV08 CV07 CV06 CV05 CV04 CV03 CV02  (-)  (-)   -    -    -    -
    
    pstcAdc->CMPD = (pstcConfig->u16ComparisonValue) << 4;  // 2 lest significant bits are not compared!
    
  } // if (pstcConfig->bComparisonEnable == TRUE)
  
  if (   (pstcConfig->enUseInterruptsDma == AdcInterrupts)
#if (L3_DEVICE_TYPE != L3_TYPE3)
      || (pstcConfig->enUseInterruptsDma == AdcDma)
#endif
     )
  {
     Adc_InitIrq(pstcAdc);
  }
  
  // Update internal data
  
  //pstcAdcInternData->pstcUserConfiguration = pstcConfig;
  for (u8Counter = 0; u8Counter < L3_ADC_CHANNELS; u8Counter++)
  {
    pstcAdcInternData->pu8ChannelBufferPointer[u8Counter] =
      pstcConfig->pu8ChannelBufferPointer[u8Counter];
    
    pstcAdcInternData->pu16ChannelBufferPointer[u8Counter] =
      pstcConfig->pu16ChannelBufferPointer[u8Counter];    
    
    pstcAdcInternData->pu8PriorityBufferPointer[u8Counter] =
      pstcConfig->pu8PriorityBufferPointer[u8Counter];
    
    pstcAdcInternData->pu16PriorityBufferPointer[u8Counter] =
      pstcConfig->pu16PriorityBufferPointer[u8Counter]; 
  }
  
  pstcAdcInternData->u16ChannelBufferCounter  = pstcConfig->u16ChannelBufferCounter;
  pstcAdcInternData->u16ChannelBufferEndCount = pstcConfig->u16ChannelBufferEndCount;
  pstcAdcInternData->enResolution             = pstcConfig->enResolution;
  pstcAdcInternData->enUseInterruptsDma       = pstcConfig->enUseInterruptsDma;
  
#if (L3_DEVICE_TYPE == L3_TYPE0)
  // Do nothing for Type0 devices
#elif (L3_DEVICE_TYPE == L3_TYPE3) || \
      (L3_DEVICE_TYPE == L3_TYPE6) || \
      (L3_DEVICE_TYPE == L3_TYPE7)
  if (pstcAdc == &ADC0)
  {  
    pu16ADCEN0 = (uint16_t)(pstcConfig->u8EnableCycleValue << 8);
  }
  else if (pstcAdc == &ADC1)
  {
    pu16ADCEN1 = (uint16_t)(pstcConfig->u8EnableCycleValue << 8);
  }
  else if (pstcAdc == &ADC2)
  {
    pu16ADCEN2 = (uint16_t)(pstcConfig->u8EnableCycleValue << 8);
  }
#else
  if (pstcAdc == &ADC0)
  {  
    pu8ADCEN0 = (uint8_t)(((0x03) & (pstcConfig->u8EnableCycleValue)) << 4);
  }
  else if (pstcAdc == &ADC1)
  {
    pu8ADCEN1 = (uint8_t)(((0x03) & (pstcConfig->u8EnableCycleValue)) << 4);
  }
  else if (pstcAdc == &ADC2)
  {
    pu8ADCEN2 = (uint8_t)(((0x03) & (pstcConfig->u8EnableCycleValue)) << 4);
  }  
#endif
  
  // Enable ADC
  pstcAdc->ADCEN_f.ENBL = 1;
  
  while (0 == pstcAdc->ADCEN_f.READY)
  {
    L3_WAIT_LOOP_HOOK();
  }
  
  return Ok; 
} // Adc_Init

/**
 ******************************************************************************
 ** \brief Trigger ADC
 **
 ** This function triggers an ADC instance for conversion
 **
 ** \note Since L3 version 1.1 the second argument stc_adc_config_t* pstcConfig
 **       is removed!
 **
 ** \param [in]  pstcAdc          ADC instance
 **
 ** \retval Ok                    Adc sucessfully triggered
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_TriggerStart( volatile stc_adcn_t* pstcAdc )
{
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return ErrorInvalidParameter ;
  }

  pstcAdc->SCCR_f.SSTR = 1;   // trigger start
  
  return Ok;
}

/**
 *****************************************************************************
 ** \brief Check for ADC conversion status
 **
 ** This function returns the status of a conversion
 **
 ** \note Since L3 version 1.1 the second argument stc_adc_config_t* pstcConfig
 **       is removed!
 **
 ** \param [in]  pstcAdc          ADC instance
 **
 ** \retval Ok                    Conversion finished
 ** \retval -1                    Conversion ongoing
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_ConversionStatus( volatile stc_adcn_t* pstcAdc )
{
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return ErrorInvalidParameter ;
  }

  if(pstcAdc->SCFDL_f.INVL == 0)
  {
    return Ok;
  }
  
  return OperationInProgress;
}

/**
 ******************************************************************************
 ** \brief Stores ADC channel data in buffer
 **
 ** This function stores a conversion data into the correct channel buffer.
 **
 ** \note Since L3 version 1.1 the second argument stc_adc_config_t* pstcConfig
 **       is removed!
 **
 ** \param [in]  pstcAdc          ADC instance
 **
 ** \retval Ok                    Storing finished
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_StoreData( volatile stc_adcn_t* pstcAdc )
{
  uint16_t  u16ResultLow;
  uint16_t  u16ResultHigh;
  uint8_t   u8Channel;
  uint32_t  u32BufferAddress;
  uint8_t   u8ByteShortFactor;
 
  // Pointer to internal data
  stc_adc_intern_data_t* pstcAdcInternData ;  
  
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return ErrorInvalidParameter;
  }
  
  // Get pointer to internal data structure ...
  pstcAdcInternData = AdcGetInternDataPtr( pstcAdc ) ;
  
  u16ResultLow  = pstcAdc->SCFDL;
  u16ResultHigh = pstcAdc->SCFDH;
  
  u8Channel = (uint8_t) 0x001F & u16ResultLow;
  
  // Check, if buffer is full
  u8ByteShortFactor = (pstcAdcInternData->enResolution == Adc8Bit) ? 1 : 2 ; 
  
  if (pstcAdcInternData->u16ChannelBufferCounter >=
      (pstcAdcInternData->u16ChannelBufferEndCount * u8ByteShortFactor))
  {
    return ErrorBufferFull;
  }
  
  if (pstcAdcInternData->enResolution == Adc8Bit)
  {
    u32BufferAddress = (uint32_t) pstcAdcInternData->pu8ChannelBufferPointer[u8Channel];
    u32BufferAddress += pstcAdcInternData->u16ChannelBufferCounter;
    *(uint8_t*) u32BufferAddress = (uint8_t) (u16ResultHigh >> 8);
    
    pstcAdcInternData->u16ChannelBufferCounter++;
  }
  else
  {
    u32BufferAddress = (uint32_t) pstcAdcInternData->pu16ChannelBufferPointer[u8Channel];
    u32BufferAddress += pstcAdcInternData->u16ChannelBufferCounter;
    *(uint16_t*) u32BufferAddress = (uint16_t) u16ResultHigh;
    
    pstcAdcInternData->u16ChannelBufferCounter += 2;    
  }
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief De-Initialize ADC
 **
 ** This function de-initializes an ADC module.
 **
 ** \param [in]  pstcAdc          ADC instance
 **
 ** \retval Ok                    ADC and interrupts have been deactivated
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_DeInit( volatile stc_adcn_t* pstcAdc )
{
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return ErrorInvalidParameter;
  }

  // Disable ADC
  pstcAdc->ADCEN_f.ENBL = 0;
  
  pstcAdc->ADCR = 0;
  pstcAdc->ADSR = 0;
  pstcAdc->SCCR = 0; 
  
  Adc_DeInitIrq(pstcAdc);   // De-Init anyway!
  
  return Ok; 
} // Adc_DeInit

/**
 ******************************************************************************
 ** \brief Purge Scan Conversion FIFO of ADC
 **
 ** This function purges the FIFO and rejects all data. It should be called
 ** in case of an error.
 **
 ** \param [in]  pstcAdc          ADC instance
 **
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_PurgeFifo( volatile stc_adcn_t* pstcAdc )
{
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return ErrorInvalidParameter;
  }
  
  pstcAdc->SCCR_f.SFCLR = 1;
  
  return Ok; 
} // Adc_PurgeFifo

/**
 ******************************************************************************
 ** \brief Purge Priority Scan Conversion FIFO of ADC
 **
 ** This function purges the Priority Scan FIFO and rejects all data. It
 ** should be called in case of an error.
 **
 ** \param [in]  pstcAdc          ADC instance
 **
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcAdc == NULL
 ******************************************************************************/
en_result_t Adc_PurgePrioFifo( volatile stc_adcn_t* pstcAdc )
{
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return ErrorInvalidParameter;
  }
  
  pstcAdc->PCCR_f.PFCLR = 1;
  
  return Ok; 
} // Adc_PurgePrioFifo

/**
 ******************************************************************************
 ** \brief Get ADC status
 **
 ** \param [in]  pstcAdc                       ADC instance
 **
 ** \retval AdcErrorUnknownState               pstcAdc == NULL or unknown state
 **                                            (should never happen)
 ** \retval AdcStandby                         Standby for A/D conversion.
 ** \retval AdcScanInProgress                  Scan A/D conversion is in progress.
 ** \retval AdcPrioInProgress                  Priority A/D conversion (priority
 **                                            level 1 or 2) is in progress.
 ** \retval AdcPrioInProgressScanPending       riority A/D conversion (priority
 **                                            level 1 or 2) is in progress.
 **                                            Scan conversion is pending.
 ** \retval AdcPrio1InProgressPrio2Pending     Priority A/D conversion (priority
 **                                            level 1) is in progress. Priority
 **                                            conversion (priority level 2) is
 **                                            pending.
 ** \retval AdcPrio1InProgressPrio2ScanPending Priority A/D conversion (priority
 **                                            level 1) is in progress. Scan
 **                                            conversion and priority conversion
 **                                            (priority level 2) are pending.
 ******************************************************************************/
en_adc_status_t Adc_GetStatus( volatile stc_adcn_t* pstcAdc )
{
  uint8_t u8Flags;
  
  // Check for NULL pointer
  if (pstcAdc == NULL)
  {
      return AdcErrorUnknownState;
  }
  
  u8Flags = 0x07 & pstcAdc->ADSR;
  
  switch (u8Flags)
  {
    case 0:
      return AdcStandby;
    case 1:
      return AdcScanInProgress;
    case 2:
      return AdcPrioInProgress;
    case 3:
      return AdcPrioInProgressScanPending;
    case 4:
      return AdcErrorUnknownState;  // Should not happen!
    case 5:
      return AdcErrorUnknownState;  // Should not happen!
    case 6:
      return AdcPrio1InProgressPrio2Pending;
    case 7:
      return AdcPrio1InProgressPrio2ScanPending;
    default:
      break;
  }
  
  return AdcErrorUnknownState; // should never be executed!
} // Adc_GetStatus


//@} // AdcGroup

#endif // #if (defined(L3_PERIPHERAL_ADC_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
