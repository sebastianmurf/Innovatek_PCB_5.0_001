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
/** \file wc.c
 **
 ** A detailed description is available at 
 ** @link WcGroup WC Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-11  1.1  MWi  Sub clock time out added, configuration structure
 **                           changed for global tm structure pointer
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "wc.h"

#if (defined(L3_PERIPHERAL_WC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup WcGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

///  Macro to return the number of enabled WC instances
#define WC_INSTANCE_COUNT (uint32_t)(sizeof(m_astcWcInstanceDataLut) / sizeof(m_astcWcInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled WC instances and their internal data
stc_wc_instance_data_t m_astcWcInstanceDataLut[] =
{
    #if (L3_PERIPHERAL_ENABLE_WC0 == L3_ON)
    {
      &WC0,   // pstcInstance
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
 ** \brief Return the internal data for a certain WC instance.
 **
 ** \param pstcWc Pointer to WC instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 *****************************************************************************/
static stc_wc_intern_data_t* WcGetInternDataPtr(volatile stc_wcn_t* pstcWc) 
{
    uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < WC_INSTANCE_COUNT; u32Instance++)
    {
        if (pstcWc == m_astcWcInstanceDataLut[u32Instance].pstcInstance)
        {
            return &m_astcWcInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}

/**
 *****************************************************************************
 ** \brief ISR callback for WC (channel 0 and 1)
 **
 ** This callbacks are called by the global WC ISR whenever an WC triggers an
 ** interrupt. It calls the callback functions that has been given during Wc
 ** initialization (see Wc_Init() and #stc_wc_config_t). If the
 ** pointer to a callback function is NULL, no call is performed.
 **
 ** The active interrupt request flags are cleared by the ISR
 **
 ** \param [in] pstcWc           WC instance
 ** \param [in] pstcWcInternData Internal data associated with the WC instance
 *****************************************************************************/
void WcIrqHandler0( volatile stc_wcn_t*   pstcWc,
                    stc_wc_intern_data_t* pstcWcInternData
                   )
{
  if ((pstcWc->WCCR & 0x01) == 0x01) // interrupt?
  {
    pstcWc->WCCR &= 0xFE;            // Clear interrupt
    
    pstcWcInternData->u32RawTime++;  /// Increment the Unix-Time representing Variable

    // Check for callback function pointer
    if ( pstcWcInternData->pfnCallbackWc0 != NULL )
    {
      pstcWcInternData->pfnCallbackWc0() ;
    }
  }
} // WcIrqHandler0

/**
 *****************************************************************************
 ** \brief Initialize WC
 **
 ** This function initializes the WC module and sets up the internal
 ** data structures.
 ** If the sub clock was not enabled before, the sub clock enable and
 ** stablilization is done in this function.
 **
 ** \param [in] pstcWc            WC instance
 ** \param [in] pstcConfig        WC configuration structure
 ** \param [in] bTouchNvic        TRUE = touch shared NVIC registers, FALSE = do
 **                               not touch
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcWc == NULL or pstcWcInternData == NULL
 ** \retval ErrorInvalidMode      Sub Clock can't be enabled (e.g. not existing)
 *****************************************************************************/
en_result_t Wc_Init( volatile stc_wcn_t*  pstcWc, 
                     stc_wc_config_t*     pstcConfig,
                     boolean_t            bTouchNvic
                   )
{
  uint32_t              u8WcSubClockTimeout;
  stc_wc_intern_data_t* pstcWcInternData ;
    
  // Check for NULL pointer
  if ( pstcWc == NULL )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcWcInternData = WcGetInternDataPtr( pstcWc ) ;
  // ... and check for NULL
  if ( pstcWcInternData == NULL )
  {
      return ErrorInvalidParameter ;
  }

  // Set the Unix-Time to zero
  pstcWcInternData->u32RawTime = 0;  

  if (TRUE == bTouchNvic)
  {
    // Initialize the interrupts for the wc if should be touched
    NVIC_EnableIRQ(OSC_PLL_WC_IRQn);
    // Initialize the priority of the wc interrupts (maybe wrong place)	 		
    NVIC_SetPriority(OSC_PLL_WC_IRQn, L3_IRQ_LEVEL_CLK_WC0); 	 
  }

  /// Initialization of the sub-oscillator (CLK module must be enabled!)
  
  // First check, if Sub clock already enabled
  if (FM3_CRG->SCM_CTL_f.SOSCE == 1)
  {
    pstcWcInternData->bSubClockWasEnabled = 1 ;
  }
  else
  {
    pstcWcInternData->bSubClockWasEnabled = 0 ;
    
    // Now enable Sub clock
    FM3_CRG->SCM_CTL_f.SOSCE = 1;
    
    // Check, if bit was set
    if (1 != FM3_CRG->SCM_CTL_f.SOSCE)
    {
      return ErrorInvalidMode ;
    }
    
    u8WcSubClockTimeout = WC_SUBCLOCK_TIMEOUT;
    // wait for Sub clock oscillation stable
    while (0 == FM3_CRG->SCM_STR_f.SORDY) 
    {
      L3_WAIT_LOOP_HOOK(); 
      
      u8WcSubClockTimeout--;
      if (0 == u8WcSubClockTimeout)
      {
        // Disable Sub clock to prevent CSV interrupt
        FM3_CRG->SCM_CTL_f.SOSCE = 0;
        return ErrorInvalidMode ;
      }
    }
  }

  // Setup Watch Counter Prescaler, using sub-clock
  pstcWc->CLK_SEL = 0x0;
  // Set "1" to CLK_EN bit of the division clock enable register (CLK_EN)    				
  pstcWc->CLK_EN = 0x1;  	    				 
  					
  pstcWc->WCRL = 0x1; 
  
  // Set Timer Mode
  switch (pstcConfig->enPrescalerDiv)
  {
    case WcPrescalerDiv1000ms:
      pstcWc->WCCR = 0x0e;
    break;
    case WcPrescalerDiv500ms:
      pstcWc->WCCR = 0x0a;
    break;
    case WcPrescalerDiv250ms:
      pstcWc->WCCR = 0x06;
    break;
    case WCPrescalerDiv125ms:
      pstcWc->WCCR = 0x02;
    break;
    default:
      return ErrorInvalidParameter ;
  }
  
  // Enable the watch counter by using the WCEN bit (WCEN = 1) in (WCCR)
  if (pstcConfig->bEnable)
  {
    pstcWc->WCCR |= 0x80;
  }
  else
  {
    pstcWc->WCCR &= 0x7F;
  }
  
  // Set calendar pointer
  pstcWcInternData->pstcCalendar = pstcConfig->pstcCalendar;
    
  // Set call back function pointer
  pstcWcInternData->pfnCallbackWc0 = pstcConfig->pfnCallback;
  
  return Ok;
} // Wc_Init

/**
 *****************************************************************************
 ** \brief De-Initialize WC
 **
 ** This function de-initializes the WC module. Differently to the Wc_init()
 ** function, Wc-DeInit() also accesses the WC hardware register. They are
 ** set to 0 (default reset values). The configuration is not changed, but the
 ** callback pointers are set to NULL.
 **
 ** \param [in] pstcWc            WC instance
 ** \param [in] bTouchNvic        TRUE = touch shared NVIC registers, FALSE = do
 **                               not touch
 **
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcWc == NULL or pstcWcInternData == NULL
 *****************************************************************************/
en_result_t Wc_DeInit( volatile stc_wcn_t* pstcWc,
                       boolean_t           bTouchNvic
                     )
{
  // Pointer to internal data
  stc_wc_intern_data_t* pstcWcInternData ;  
  
  // Check for NULL pointer
  if ( pstcWc == NULL )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcWcInternData = WcGetInternDataPtr( pstcWc ) ;
  // ... and check for NULL
  if ( pstcWcInternData != NULL )
  {
      pstcWcInternData->pfnCallbackWc0 = NULL ;
  }
  
  /// Stop the wc and clear the settings
  pstcWc->WCCR = 0;

  if (TRUE == bTouchNvic)
  {
    /// Disable IRQ of wc
    NVIC_DisableIRQ(OSC_PLL_WC_IRQn);
    /// Set the priority of the wc to the lowest number 		
    NVIC_SetPriority(OSC_PLL_WC_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }    

  if (pstcWcInternData->bSubClockWasEnabled == 0)
  {
    // Stop the sub clock oscillator
    FM3_CRG->SCM_CTL_f.SOSCE = 0 ;
  }
  
  return Ok;
} // Wc_DeInit

/**
 *****************************************************************************
 ** \brief GetTime
 **
 ** This function calculates the time and date, based on the UNIX-Time.
 ** The time-base is generated from a regularly triggered variable. 
 ** This variable is triggered from the watch-counter ISR. The Time-struct is
 ** stored in the global struct Calender.
 **
 ** \note Since L3 version 1.1 the 2nd argument #stc_wc_config_t was removed
 **
 ** \param [in] pstcWc            WC instance
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcWc == NULL or pstcWcInternData == NULL
 *****************************************************************************/
en_result_t Wc_GetTime(volatile stc_wcn_t* pstcWc) 
{
  // Pointer to internal data
  stc_wc_intern_data_t* pstcWcInternData ;  
  
  // Check for NULL pointer
  if ( pstcWc == NULL )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcWcInternData = WcGetInternDataPtr( pstcWc ) ;
  // ... and check for NULL
  if ( pstcWcInternData == NULL )
  {
      return ErrorInvalidParameter ;
  }

  *pstcWcInternData->pstcCalendar = *(localtime (&pstcWcInternData->u32RawTime));
  
  return Ok;
} // Wc_GetTime

/**
 *****************************************************************************
 ** \brief SetTime
 **
 ** This function calculates the Unix-time, based on the struct Calender.
 ** The time-base is generated from a regularly triggered variable. 
 ** This variable is triggered from the watch-counter ISR.
 **
 ** \param [in] pstcWc            WC instance
 ** \param [in] pstcConfig        WC configuration structure
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcWc == NULL or pstcWcInternData == NULL
 *****************************************************************************/
en_result_t Wc_SetTime( volatile stc_wcn_t* pstcWc, 
			stc_wc_config_t*    pstcConfig) 
{
  stc_wc_intern_data_t* pstcWcInternData ;
  struct tm stcWcTempCalendar;  // local temporary calendar struct for calculation
    
  // Check for NULL pointer
  if ( pstcWc == NULL )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcWcInternData = WcGetInternDataPtr( pstcWc ) ;
  // ... and check for NULL
  if ( pstcWcInternData == NULL )
  {
      return ErrorInvalidParameter ;
  }

  // seconds 0 to 59
  if (pstcConfig->u8SetSecond > 59)
  {
    return ErrorInvalidParameter ;
  }
  
  // minutes after the hour, 0 to 59
  if (pstcConfig->u8SetMinute > 59)
  {
    return ErrorInvalidParameter ;
  }
  
  // hours since midnight, 0 to 23
  if (pstcConfig->u8SetHour > 23)
  {
    return ErrorInvalidParameter ;
  }
  
  // day of the month, 1 to 31
  if (pstcConfig->u8SetDay > 31)
  {
    return ErrorInvalidParameter ;
  }

  // months since January, 0 to 11
  if (pstcConfig->u8SetMonth > 11)
  {
    return ErrorInvalidParameter ;
  }
  
  // years since 1900
  if (pstcConfig->u8SetYear > 235)
  {
    return ErrorInvalidParameter ;
  }
  
  stcWcTempCalendar.tm_sec   = (int32_t)pstcConfig->u8SetSecond;
  stcWcTempCalendar.tm_min   = (int32_t)pstcConfig->u8SetMinute;
  stcWcTempCalendar.tm_hour  = (int32_t)pstcConfig->u8SetHour;
  stcWcTempCalendar.tm_mday  = (int32_t)pstcConfig->u8SetDay;
  stcWcTempCalendar.tm_mon   = (int32_t)pstcConfig->u8SetMonth;
  stcWcTempCalendar.tm_year  = (int32_t)pstcConfig->u8SetYear;
  stcWcTempCalendar.tm_isdst = (int32_t)pstcConfig->i8SetIsdst;
  pstcWcInternData->u32RawTime = mktime(&stcWcTempCalendar);

  return Ok;
} // Wc_SetTime

//@} // WcGroup

#endif // #if (defined(L3_PERIPHERAL_WC_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

