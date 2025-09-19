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
/** \file dt.c
 **
 ** A detailed description is available at 
 ** @link DtGroup DT Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-07-09  1.1  MWi  Typos corrected
 **   - 2012-08-10  1.2  MWi  Bug fix in Dt_SetChannelConfig(), typos corrected
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "dt.h"

#if (defined(L3_PERIPHERAL_DT_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup DtGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

///  Macro to return the number of enabled DT instances
#define DT_INSTANCE_COUNT (uint32_t)(sizeof(m_astcDtInstanceDataLut) / sizeof(m_astcDtInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled DT instances and their internal data
stc_dt_instance_data_t m_astcDtInstanceDataLut[] =
{
    #if (L3_PERIPHERAL_ENABLE_DT0 == L3_ON)
    { &DT0,   // pstcInstance
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

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

/**
 *****************************************************************************
 ** \brief Return the internal data for a certain DT instance.
 **
 ** \param pstcDt Pointer to DT instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 *****************************************************************************/
static stc_dt_intern_data_t* DtGetInternDataPtr(volatile stc_dtn_t* pstcDt) 
{
    uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < DT_INSTANCE_COUNT; u32Instance++)
    {
        if (pstcDt == m_astcDtInstanceDataLut[u32Instance].pstcInstance)
        {
            return &m_astcDtInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}

/**
 ******************************************************************************
 ** \brief Device dependent initialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcDt Pointer to DT instance
 **
 ** \return Ok    Successful initialization
 **
 ******************************************************************************/
static en_result_t Dt_InitIrq( volatile stc_dtn_t* pstcDt ) 
{
  #if (L3_PERIPHERAL_ENABLE_DT0 == L3_ON)
  if (pstcDt == &DT0)
  {
    NVIC_ClearPendingIRQ(DTIM_QDU_IRQn);
    NVIC_EnableIRQ(DTIM_QDU_IRQn);
    NVIC_SetPriority(DTIM_QDU_IRQn, L3_IRQ_LEVEL_DT0);
  }
  #endif 
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Device dependent de-initialization of interrupts according CMSIS 
 **
 ** \param pstcDt Pointer to DT instance
 **
 ** \return Ok    Successful initialization
 **
 ******************************************************************************/
static en_result_t Dt_DeInitIrq( volatile stc_dtn_t* pstcDt ) 
{
  #if (L3_PERIPHERAL_ENABLE_DT0 == L3_ON)
  if (pstcDt == &DT0)
  {
    NVIC_ClearPendingIRQ(DTIM_QDU_IRQn);
    NVIC_DisableIRQ(DTIM_QDU_IRQn);
    NVIC_SetPriority(DTIM_QDU_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif 
  
  return Ok;
}

/**
 *****************************************************************************
 ** \brief ISR callback for DT (channel 0 and 1)
 **
 ** This callbacks are called by the global DT ISR whenever an DT triggers an
 ** interrupt (on channel 0 and/or 1). It calls the callback functions that
 ** has been given during Dt initialization (see Dt_Init() and
 ** #stc_dt_channel_config_t) for each channel individually. If the pointer
 ** to a callback function is NULL, no call is performed.
 **
 ** The active interrupt request flags are cleared by the ISR
 **
 ** \param [in] pstcDt           Dual Timer instance
 ** \param [in] pstcDtInternData Internal data associated with the DT instance
 *****************************************************************************/
void DtIrqHandler0( volatile stc_dtn_t* pstcDt,
                    stc_dt_intern_data_t* pstcDtInternData
                   )
{
  // Check channel
  if (pstcDt->TIMER1RIS_f.TIMERXRIS == 1) // Timer 0 interrupt?
  {
    pstcDt->TIMER1INTCLR = 0;             // Clear interrupt
    
    // Check for callback function pointer
    if ( pstcDtInternData->pfnCallbackChannel0 != NULL )
    {
      pstcDtInternData->pfnCallbackChannel0() ;
    }
  }
  
  if (pstcDt->TIMER2RIS_f.TIMERXRIS == 1) // Timer 1 interrupt?
  {
    pstcDt->TIMER2INTCLR = 0;             // Clear interrupt
    
    // Check for callback function pointer
    if ( pstcDtInternData->pfnCallbackChannel1 != NULL )
    {
      pstcDtInternData->pfnCallbackChannel1() ;
    }
  }
} // DtIrqHandler0

/**
 *****************************************************************************
 ** \brief Initialize DT
 **
 ** This function initializes the DT module and sets up the internal
 ** data structures
 **
 ** \note This function does not initialze the DT channels itself.
 **       Dt_SetChannelConfig() has to be called for each channel you wish to
 **       use after Dt_Init().
 **
 ** \param [in]  pstcDt           Dual Timer instance
 ** \param [in]  bTouchNVIC       TRUE: NVIC is set, FALSE: NVIC is not
 **                               touched
 **
 ** \retval Ok                    Internal data has been prepared
 ** \retval ErrorInvalidParameter pstcDt == NULL
 *****************************************************************************/
en_result_t Dt_Init( volatile stc_dtn_t* pstcDt,
                     boolean_t bTouchNVIC)
{
  // Pointer to internal data
  stc_dt_intern_data_t* pstcDtInternData ;  
  
  // Check for NULL pointer
  if ( pstcDt == NULL )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcDtInternData = DtGetInternDataPtr( pstcDt ) ;
  // ... and check for NULL
  if ( pstcDtInternData != NULL )
  {
      pstcDtInternData->pfnCallbackChannel0 = NULL ;
      pstcDtInternData->pfnCallbackChannel1 = NULL ;
  }
  
  if (bTouchNVIC == TRUE)
  {
    Dt_InitIrq( pstcDt );
  }
  
  return Ok;
} // Dt_Init

/**
 *****************************************************************************
 ** \brief Set DT channel configuration
 **
 ** This function initializes the given DT channel. The parameters are checked
 ** for plausibility and the given callback is stored internally.
 **
 ** The timer enable pstcConfig->bEnable can be set to start the timer via
 ** this function. Alternatively the enable is set to 0 here and the timer
 ** can be enabled via Dt_EnableChannel().
 **
 ** \param [in]  pstcDt     Dual Timer instance
 ** \param [in]  pstcConfig DT configuration parameters
 ** \param [in]  u8Channel  0 or 1
 **
 ** \retval Ok                    Channel configuration has been set
 ** \retval ErrorInvalidParameter If one of the following conditions is met
 **         - pstcDt     == NULL
 **         - pstcConfig == NULL
 **         - pstcIcuInternData == NULL (invalid or disabled DT unit
 **                                                 (L3_PERIPHERAL_ENABLE_DTn))
 **         - u8Channel >= DT_CHANNEL_COUNT
 *****************************************************************************/
en_result_t Dt_SetChannelConfig( volatile stc_dtn_t* pstcDt,
                                 stc_dt_channel_config_t* pstcConfig,
                                 uint8_t u8Channel
                                )
{
  // Pointer to internal data
  stc_dt_intern_data_t* pstcDtInternData ;
  stc_dtim_timer1control_field_t unT1CTRL = { 0 }; // Preset to zero
  stc_dtim_timer2control_field_t unT2CTRL = { 0 }; // Preset to zero

  // Check for NULL pointers
  if ( pstcDt     == NULL ||
       pstcConfig == NULL
     )
  {
      return ErrorInvalidParameter ;
  }

  // Get pointer to internal data structure ...
  pstcDtInternData = DtGetInternDataPtr( pstcDt ) ;
  // ... and check for NULL
  if ( pstcDtInternData == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  if (u8Channel == 0)
  {
    pstcDt->TIMER1CONTROL = 0;              // Disable everything
    
    // Set Timer Mode
    switch (pstcConfig->enMode)
    {
      case DtFreeRun:
        unT1CTRL.TIMERMODE = 0;
        unT1CTRL.ONESHOT = 0;
        break;
      case DtPeriodic:
        unT1CTRL.TIMERMODE = 1;
        unT1CTRL.ONESHOT = 0;
        break;
      case DtOneShot:
        unT1CTRL.TIMERMODE = 0;
        unT1CTRL.ONESHOT = 1;
        break;
      default:
        return ErrorInvalidParameter ;
    }
    
    // Set Prescaler
    switch (pstcConfig->enPrescalerDiv)
    {
      case DtPrescalerDiv1:
        unT1CTRL.TIMERPRE0 = 0;
        unT1CTRL.TIMERPRE1 = 0;
        break;
      case DtPrescalerDiv16:
        unT1CTRL.TIMERPRE0 = 1;
        unT1CTRL.TIMERPRE1 = 0;
        break;
      case DtPrescalerDiv256:
        unT1CTRL.TIMERPRE0 = 0;
        unT1CTRL.TIMERPRE1 = 1;
        break;
      default:
        return ErrorInvalidParameter ;
    }
    
    // Set Counter Size
    unT1CTRL.TIMERSIZE = pstcConfig->enCounterSize;
    
    // Check Counter Values in order to size
    if (pstcConfig->enCounterSize == DtCounterSize16)
    {
      if (pstcConfig->u32DtLoad > 0xFFFF)
      {
        return ErrorInvalidParameter ;
      }
      if (pstcConfig->u32DtBackgroundLoad > 0xFFFF)
      {
        return ErrorInvalidParameter ;
      }
    }
    
    // Set the counters
    pstcDt->TIMER1LOAD   = pstcConfig->u32DtLoad;
    pstcDt->TIMER1BGLOAD = pstcConfig->u32DtBackgroundLoad;
    
    // Set enable/disable ...
    unT1CTRL.TIMEREN = pstcConfig->bEnable;
    // ... and update hardware
    pstcDt->TIMER1CONTROL_f = unT1CTRL;
    
    // Set call back function pointer
    pstcDtInternData->pfnCallbackChannel0 = pstcConfig->pfnCallback;
  }
  else if (u8Channel == 1)
  {
    pstcDt->TIMER2CONTROL = 0;              // Disable everything
    
    // Set Timer Mode
    switch (pstcConfig->enMode)
    {
      case DtFreeRun:
        unT2CTRL.TIMERMODE = 0;
        unT2CTRL.ONESHOT = 0;
        break;
      case DtPeriodic:
        unT2CTRL.TIMERMODE = 1;
        unT2CTRL.ONESHOT = 0;
        break;
      case DtOneShot:
        unT2CTRL.TIMERMODE = 0;
        unT2CTRL.ONESHOT = 1;
        break;
      default:
        return ErrorInvalidParameter ;
    }
    
    // Set Prescaler
    switch (pstcConfig->enPrescalerDiv)
    {
      case DtPrescalerDiv1:
        unT2CTRL.TIMERPRE0 = 0;
        unT2CTRL.TIMERPRE1 = 0;
        break;
      case DtPrescalerDiv16:
        unT2CTRL.TIMERPRE0 = 1;
        unT2CTRL.TIMERPRE1 = 0;
        break;
      case DtPrescalerDiv256:
        unT2CTRL.TIMERPRE0 = 0;
        unT2CTRL.TIMERPRE1 = 1;
        break;
      default:
        return ErrorInvalidParameter ;
    }
    
    // Set Counter Size
    unT2CTRL.TIMERSIZE = pstcConfig->enCounterSize;
    
    // Check Counter Values in order to size
    if (pstcConfig->enCounterSize == DtCounterSize16)
    {
      if (pstcConfig->u32DtLoad > 0xFFFF)
      {
        return ErrorInvalidParameter ;
      }
      if (pstcConfig->u32DtBackgroundLoad > 0xFFFF)
      {
        return ErrorInvalidParameter ;
      }
    }
    
    // Set the counters
    pstcDt->TIMER2LOAD   = pstcConfig->u32DtLoad;
    pstcDt->TIMER2BGLOAD = pstcConfig->u32DtBackgroundLoad;
    
    // Set enable/disable ...
    unT2CTRL.TIMEREN = pstcConfig->bEnable;
    // ... and update hardware
    pstcDt->TIMER2CONTROL_f = unT2CTRL;
    
    // Set call back function pointer
    pstcDtInternData->pfnCallbackChannel1 = pstcConfig->pfnCallback;
  }
  else
  {
    return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Dt_SetChannelConfig

/**
 *****************************************************************************
 ** \brief Enable DT channel
 **
 ** This function enables the given DT channel. 
 **
 ** \param [in]  pstcDt     Dual Timer instance
 ** \param [in]  u8Channel  0 or 1
 **
 ** \retval Ok                    Channel enabled
 ** \retval ErrorInvalidParameter If one of the following conditions is met
 **         - pstcDt            == NULL
 **         - pstcIcuInternData == NULL (invalid or disabled DT unit
 **                                                 (L3_PERIPHERAL_ENABLE_DTn))
 **         - u8Channel >= DT_CHANNEL_COUNT
 *****************************************************************************/
en_result_t Dt_EnableChannel( volatile stc_dtn_t* pstcDt,
                              uint8_t  u8Channel
                             )
{
  // Check for NULL pointers
  if (pstcDt == NULL)
  {
      return ErrorInvalidParameter ;
  }

  if (u8Channel == 0)
  {
    pstcDt->TIMER1CONTROL_f.INTENABLE = 1;     
    pstcDt->TIMER1CONTROL_f.TIMEREN = 1;
  }
  else if (u8Channel == 1)
  {
    pstcDt->TIMER2CONTROL_f.INTENABLE = 1;     
    pstcDt->TIMER2CONTROL_f.TIMEREN = 1;
  }
  else
  {
    return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Dt_EnableChannel

/**
 *****************************************************************************
 ** \brief Disable DT channel
 **
 ** This function disables the given DT channel. 
 **
 ** \param [in]  pstcDt     Dual Timer instance
 ** \param [in]  u8Channel  0 or 1
 **
 ** \retval Ok                    Channel is disabled
 ** \retval ErrorInvalidParameter If one of the following conditions is met
 **         - pstcDt            == NULL
 **         - pstcIcuInternData == NULL (invalid or disabled DT unit
 **                                                 (L3_PERIPHERAL_ENABLE_DTn))
 **         - u8Channel >= DT_CHANNEL_COUNT
 *****************************************************************************/
en_result_t Dt_DisableChannel( volatile stc_dtn_t* pstcDt,
                               uint8_t u8Channel
                             )
{
  // Check for NULL pointers
  if (pstcDt == NULL)
  {
      return ErrorInvalidParameter ;
  }

  if (u8Channel == 0)
  {
     pstcDt->TIMER1CONTROL_f.TIMEREN = 0;
     pstcDt->TIMER1CONTROL_f.INTENABLE = 0;
  }
  else if (u8Channel == 1)
  {
     pstcDt->TIMER2CONTROL_f.TIMEREN = 0;
     pstcDt->TIMER2CONTROL_f.INTENABLE = 0;
  }
  else
  {
    return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Icu_DisableChannel

/**
 *****************************************************************************
 ** \brief Write Load Register of DT channel
 **
 ** This function writes to the load register of the given DT channel. 
 **
 ** \param [in]  pstcDt     Dual Timer instance
 ** \param [in]  pstcConfig DT configuration parameters
 ** \param [in]  u8Channel  0 or 1
 **
 ** \retval Ok                    Channel configuration has been set
 ** \retval ErrorInvalidParameter If one of the following conditions is met
 **         - pstcDt            == NULL
 **         - pstcIcuInternData == NULL (invalid or disabled DT unit
 **                                                 (L3_PERIPHERAL_ENABLE_DTn))
 **         - u8Channel >= DT_CHANNEL_COUNT
 *****************************************************************************/
en_result_t Dt_WriteLoadChannel( volatile stc_dtn_t* pstcDt,
                                 stc_dt_channel_config_t* pstcConfig,
                                 uint8_t u8Channel
                               )
{
  // Check for NULL pointers
  if (pstcDt == NULL)
  {
      return ErrorInvalidParameter ;
  }
 
  if (u8Channel == 0)
  {
    pstcDt->TIMER1LOAD = pstcConfig->u32DtLoad;
  }
  else if (u8Channel == 1)
  {
    pstcDt->TIMER2LOAD = pstcConfig->u32DtLoad;
  }
  else
  {
    return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Dt_WriteLoadChannel

/**
 *****************************************************************************
 ** \brief Write Background Load Register of DT channel
 **
 ** This function writes to the background load register of the given DT
 ** channel. 
 **
 ** \param [in]  pstcDt     Dual Timer instance
 ** \param [in]  pstcConfig DT configuration parameters
 ** \param [in]  u8Channel  0 or 1
 **
 ** \retval Ok                    Channel configuration has been set
 ** \retval ErrorInvalidParameter If one of the following conditions is met
 **         - pstcDt            == NULL
 **         - pstcIcuInternData == NULL (invalid or disabled DT unit
 **                                                 (L3_PERIPHERAL_ENABLE_DTn))
 **         - u8Channel >= DT_CHANNEL_COUNT
 *****************************************************************************/
en_result_t Dt_WriteBgLoadChannel( volatile stc_dtn_t* pstcDt,
                                   stc_dt_channel_config_t* pstcConfig,
                                   uint8_t u8Channel
                                 )
{
  // Check for NULL pointers
  if (pstcDt == NULL)
  {
      return ErrorInvalidParameter ;
  }

  if (u8Channel == 0)
  {
    pstcDt->TIMER1BGLOAD = pstcConfig->u32DtLoad;
  }
  else if (u8Channel == 1)
  {
    pstcDt->TIMER2BGLOAD = pstcConfig->u32DtLoad;
  }
  else
  {
    return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Dt_WriteBgLoadChannel

/**
 *****************************************************************************
 ** \brief Read Value Register of DT channel
 **
 ** This function returns the recent value of the DT count. In 16-Bit mode,
 ** the upper 16-bits are set to zero automatically.
 **
 ** \param [in]  pstcDt     Dual Timer instance
 ** \param [in]  pstcConfig DT configuration parameters
 ** \param [in]  u8Channel  0 or 1
 **
 ** \retval Value of DT channel                 
 *****************************************************************************/
uint32_t Dt_ReadValueChannel (volatile stc_dtn_t* pstcDt,
                              stc_dt_channel_config_t* pstcConfig,
                              uint8_t u8Channel
                             )
{
  uint32_t u32DtValue;
  // As always a value is returned there is no check for NULL pointers

  if (u8Channel == 0)
  {
    u32DtValue = pstcDt->TIMER1VALUE;
    
    if (pstcConfig->enCounterSize == DtCounterSize16)
    {
      u32DtValue &= 0x0000FFFF;
    }
  }
  else if (u8Channel == 1)
  {  
    u32DtValue = pstcDt->TIMER2VALUE;
    
    if (pstcConfig->enCounterSize == DtCounterSize16)
    {
      u32DtValue &= 0x0000FFFF;
    }
  }
  else
  {
    return 0;   // default value 
  }
  
  return u32DtValue;
}

/**
 *****************************************************************************
 ** \brief De-Initialize DT
 **
 ** This function de-initializes the DT module. Differently to the Dt_init()
 ** function, Dt-DeInit() also accesses the DT hardware register. They are
 ** set to 0 (default reset values). The configuration is not changed, but the
 ** callback pointers are set to NULL.
 **
 ** \note Dt_DeInit() does not set the bEnable struct element of the channel
 ** configurations to 0!
 **
 ** \param [in]  pstcDt           Dual Timer instance
 ** \param [in]  bTouchNVIC       TRUE: NVIC is reset, FALSE: NVIC is not
 **                               touched
 **
 ** \retval Ok                    Regsiters and internal data have been
 **                               cleared
 ** \retval ErrorInvalidParameter pstcDt == NULL
 *****************************************************************************/
en_result_t Dt_DeInit( volatile stc_dtn_t* pstcDt,
                       boolean_t bTouchNVIC )
{
  // Pointer to internal data
  stc_dt_intern_data_t* pstcDtInternData ;  
  
  // Check for NULL pointer
  if ( pstcDt == NULL )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcDtInternData = DtGetInternDataPtr( pstcDt ) ;
  // ... and check for NULL
  if ( pstcDtInternData != NULL )
  {
      pstcDtInternData->pfnCallbackChannel0 = NULL ;
      pstcDtInternData->pfnCallbackChannel1 = NULL ;
  }
  
  // De-Init DT Interrupt
  if (bTouchNVIC == TRUE)
  {
    Dt_DeInitIrq( pstcDt ); 
  }
  
  // Clear all registers
  pstcDt->TIMER1CONTROL = 0;
  pstcDt->TIMER2CONTROL = 0;
  
  pstcDt->TIMER1LOAD = 0;
  pstcDt->TIMER1INTCLR = 1;
  pstcDt->TIMER1BGLOAD = 0;
  
  pstcDt->TIMER2LOAD = 0;
  pstcDt->TIMER2INTCLR = 1;
  pstcDt->TIMER2BGLOAD = 0;
  
  return Ok;
} // Dt_DeInit

//@} // DtGroup

#endif // #if (defined(L3_PERIPHERAL_DT_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

