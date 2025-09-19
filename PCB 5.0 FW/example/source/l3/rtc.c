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
/** \file rtc.c
 **
 ** A detailed description is available at 
 ** @link RtcGroup RTC Module description @endlink
 **
 ** History:
 **   - 2012-06-25  1.0  MWi  First version.
 **   - 2012-08-31  1.1  MWi  Device type check corrected
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "rtc.h"

#if (defined(L3_PERIPHERAL_RTC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup RtcGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

///  Macro to return the number of enabled RTC instances
#define RTC_INSTANCE_COUNT (uint32_t)(sizeof(m_astcRtcInstanceDataLut) / sizeof(m_astcRtcInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled RTC instances and their internal data
stc_rtc_instance_data_t m_astcRtcInstanceDataLut[] =
{
    #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
    {
      &RTC0,  // pstcInstance
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
 ** \brief Return the internal data for a certain RTC instance.
 **
 ** \param pstcRtc Pointer to RTC instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_rtc_intern_data_t* RtcGetInternDataPtr(volatile stc_rtcn_t* pstcRtc) 
{
    uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < RTC_INSTANCE_COUNT; u32Instance++)
    {
        if (pstcRtc == m_astcRtcInstanceDataLut[u32Instance].pstcInstance)
        {
            return &m_astcRtcInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
} // RtcGetInternDataPtr

/**
 ******************************************************************************
 ** \brief Convert RTC time structure to time.h tm structure
 **
 ** \param [in]  pstcRtcTime    RTC Time structure
 ** \param [out] pstcTime       tm Time structure
 ** \retval  Ok                 Time converted
 ******************************************************************************/
static en_result_t RtcToTm( stc_rtc_time_t* pstcRtcTime,
                            struct tm*      pstcTime)
{
  pstcTime->tm_year  = (2000 + pstcRtcTime->u8Year) - 1900;
  pstcTime->tm_mon   = pstcRtcTime->u8Month - 1;
  pstcTime->tm_mday  = pstcRtcTime->u8Day;
  pstcTime->tm_sec   = pstcRtcTime->u8Second;
  pstcTime->tm_min   = pstcRtcTime->u8Minute;
  pstcTime->tm_hour  = pstcRtcTime->u8Hour;
  pstcTime->tm_isdst = 0;
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Binary to BCD conversion
 **
 ** \param [in]   u8BinValue  Binary Value
 **
 ** \return  8-Bit BCD Value
 **
 ******************************************************************************/
static uint8_t BinToBcd(uint8_t u8BinValue)
{
  uint8_t u8BcdValue = 0;
  
  // Ten's digit
  u8BcdValue = (uint8_t)((u8BinValue / 10) << 4);

  // One's digit
  u8BcdValue |= u8BinValue % 10;
   
  return u8BcdValue;
} // BinToBcd

/**
 ******************************************************************************
 ** \brief BCD to Binary conversion
 **
 ** \param [in]   u8BcdValue  BCD Value
 **
 ** \return  8-Bit Bin Value
 **
 ******************************************************************************/
static uint8_t BcdToBin(uint8_t u8BcdValue)
{
  uint8_t u8BinValue = 0;
  
  // Ten's digit
  u8BinValue = ((0xF0 & u8BcdValue) >> 4) * 10;

  // One's digit
  u8BinValue += u8BcdValue & 0x0F;
   
  return u8BinValue;
} // BcdToBin

/**
 ******************************************************************************
 ** \brief Device dependent initialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcRtc Pointer to RTC instance
 **
 ** \retval Ok     Successful initialization
 ******************************************************************************/
static en_result_t Rtc_InitIrq(volatile stc_rtcn_t* pstcRtc)
{
  #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
  if (pstcRtc == &RTC0)
  {
    // MFS0 interrupt settings
    NVIC_ClearPendingIRQ(OSC_PLL_WC_IRQn);
    NVIC_EnableIRQ(OSC_PLL_WC_IRQn);
    NVIC_SetPriority(OSC_PLL_WC_IRQn, L3_IRQ_LEVEL_RTC0);
  }
  #endif
  
  // If ever a device has more than 1 RTC, than next inits will follow here ...
  
  return Ok;
} // Rtc_InitIrq

/**
 ******************************************************************************
 ** \brief Device dependent de-initialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcRtc Pointer to RTC instance
 **
 ** \retval Ok     Successful de-initialization
 ******************************************************************************/
static en_result_t Rtc_DeInitIrq(volatile stc_rtcn_t* pstcRtc)
{
  #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
  if (pstcRtc == &RTC0)
  {
    // MFS0 interrupt settings
    NVIC_ClearPendingIRQ(OSC_PLL_WC_IRQn);
    NVIC_DisableIRQ(OSC_PLL_WC_IRQn);
    NVIC_SetPriority(OSC_PLL_WC_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  // If ever a device has more than 1 RTC, than next inits will follow here ...
  
  return Ok;
} // Rtc_InitIrq

/**
 ******************************************************************************
 ** \brief RTC  interrupt service routine.
 **
 ** 
 ******************************************************************************/
void RtcIrqHander( volatile stc_rtcn_t*   pstcRtc,
                   stc_rtc_intern_data_t* pstcRtcInternData
                 )
{
  uint32_t u32Wtcr1ReadOut;
  
  // Local variables to prevent multiple volatile I/O read-out
  //   warining in function call 'pfnReadCallback(...)'
  uint8_t u8Second;
  uint8_t u8Minute;
  uint8_t u8Hour;
  uint8_t u8Day;
  uint8_t u8DayOfWeek;
  uint8_t u8Month;
  uint8_t u8Year;  
  
  u32Wtcr1ReadOut = pstcRtc->WTCR1;
  
  // Read Completion?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTCRI_FLAG_POSITION)
  {
    // Clear INTCRI Flag
    pstcRtc->u32WTCR1.INTCRI = 0;
    
    if (pstcRtcInternData->pfnReadCallback != NULL)
    {
      u8Second = BcdToBin(pstcRtc->WTSR);
      u8Minute = BcdToBin(pstcRtc->WTMIR);
      u8Hour = BcdToBin(pstcRtc->WTHR);
      u8Day = BcdToBin(pstcRtc->WTDR);
      u8DayOfWeek = BcdToBin(pstcRtc->WTDW);
      u8Month = BcdToBin(pstcRtc->WTMOR);
      u8Year = BcdToBin(pstcRtc->WTYR);
      
      pstcRtcInternData->pfnReadCallback(u8Second,
                                         u8Minute,
                                         u8Hour,
                                         u8Day,
                                         u8DayOfWeek,
                                         u8Month,
                                         u8Year);
    }
  }
  
  // Alarm Completion?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTALI_FLAG_POSITION)
  {
    // Clear INTALI Flag
    pstcRtc->u32WTCR1.INTALI = 0;
    
    if (pstcRtcInternData->pfnAlarmCallback != NULL)
    {
       pstcRtcInternData->pfnAlarmCallback();
    }
  }
  
  // Timer Interrupt?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTTMI_FLAG_POSITION)
  {
    // Clear INTTMI Flag
    pstcRtc->u32WTCR1.INTTMI = 0;
    
    if (pstcRtcInternData->pfnTimerCallback != NULL)
    {
       pstcRtcInternData->pfnTimerCallback();
    }
  }
  
  // 0.5-Second Interrupt?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTSSI_FLAG_POSITION)
  {
    // Clear INTSSI Flag
    pstcRtc->u32WTCR1.INTSSI = 0;
    
    if (pstcRtcInternData->pfnHalfSecondCallback != NULL)
    {
       pstcRtcInternData->pfnHalfSecondCallback();
    }
  }

  // One Second Interrupt?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTSI_FLAG_POSITION)
  {
    // Clear INTSI Flag
    pstcRtc->u32WTCR1.INTSI = 0;
    
    if (pstcRtcInternData->pfnOneSecondCallback != NULL)
    {
       pstcRtcInternData->pfnOneSecondCallback();
    }
  }

  // One Minute Interrupt?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTMI_FLAG_POSITION)
  {
    // Clear INTMI Flag
    pstcRtc->u32WTCR1.INTMI = 0;
    
    if (pstcRtcInternData->pfnOneMinuteCallback != NULL)
    {
       pstcRtcInternData->pfnOneMinuteCallback();
    }
  } 
  
  // One Hour Interrupt?
  if (u32Wtcr1ReadOut & RTC_WTCR1_INTHI_FLAG_POSITION)
  {
    // Clear INTHI Flag
    pstcRtc->u32WTCR1.INTHI = 0;
    
    if (pstcRtcInternData->pfnOneHourCallback != NULL)
    {
       pstcRtcInternData->pfnOneHourCallback();
    }
  }  
}

/**
 ******************************************************************************
 ** \brief Initialize RTC
 **
 ** This function initializes the RTC module and sets up the internal
 ** data structures.
 ** If the sub clock was not enabled before, the sub clock enable and
 ** stablilization is done in this function.
 **
 ** \param [in] pstcRtc           RTC instance
 ** \param [in] pstcConfig        RTC configuration structure
 ** \param [in] pstcTime          RTC Time structure
 ** \param [in] pstcAlarm         RTC Alarm structure
 ** \param [in] bTouchNvic        TRUE = touch shared NVIC registers, FALSE = do
 **                               not touch
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcRtc == NULL, pstcConfig == NULL,
 **                               or pstcRtcInternData == NULL
 ******************************************************************************/
en_result_t Rtc_Init( volatile stc_rtcn_t* pstcRtc, 
                      stc_rtc_config_t*    pstcConfig,
                      stc_rtc_time_t*      pstcTime,
                      stc_rtc_alarm_t*     pstcAlarm,
                      boolean_t            bTouchNvic
                    )
{
  stc_rtc_intern_data_t*   pstcRtcInternData ;

  // Check for NULL pointer
  if ((pstcRtc    == NULL) ||
      (pstcConfig == NULL) )
  {
      return ErrorInvalidParameter ;
  }
  
  // Get pointer to internal data structure ...
  pstcRtcInternData = RtcGetInternDataPtr( pstcRtc ) ;
  // ... and check for NULL
  if ( pstcRtcInternData == NULL )
  {
      return ErrorInvalidParameter ;
  }

  // Set-up intern data
  pstcRtcInternData->pfnReadCallback       = pstcConfig->pfnReadCallback;
  pstcRtcInternData->pfnAlarmCallback      = pstcConfig->pfnAlarmCallback;
  pstcRtcInternData->pfnTimerCallback      = pstcConfig->pfnTimerCallback;
  pstcRtcInternData->pfnHalfSecondCallback = pstcConfig->pfnHalfSecondCallback;
  pstcRtcInternData->pfnOneSecondCallback  = pstcConfig->pfnOneSecondCallback;
  pstcRtcInternData->pfnOneMinuteCallback  = pstcConfig->pfnOneMinuteCallback;
  pstcRtcInternData->pfnOneHourCallback    = pstcConfig->pfnOneHourCallback;
  
  // For initialization: Force all bits of WTCR1 (inclusive ST bit)
  //   to '0' (w/o previous check)
  pstcRtc->WTCR1 = 0;
  
  // Select clock
  pstcRtc->u8WTCLKS.WTCLKS = ((pstcConfig->bUseMainClock) == TRUE) ? 1 : 0 ;
  
  // Set Date and Time
  if (ErrorInvalidParameter == Rtc_SetDateTime(pstcRtc, pstcTime, FALSE))
  {
    return ErrorInvalidParameter;
  }

  // Set Alarm Date and Time
  if (ErrorInvalidParameter == Rtc_SetAlarmDateTime(pstcRtc, pstcAlarm))
  {
    return ErrorInvalidParameter;
  }
  
  // Set Sub Second Register
  pstcRtc->WTBR = pstcConfig->u32RtcClock;
  
#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
     (L3_DEVICE_TYPE == L3_TYPE4) || \
     (L3_DEVICE_TYPE == L3_TYPE5))  
  // Set Frequency correction value
  if ((pstcConfig->u16FreqCorrValue) > RTC_MAX_FREQ_CORR_VALUE)
  {
    return ErrorInvalidParameter;
  }
  pstcRtc->WTCAL = (rtc_frq_corr_t)(pstcConfig->u16FreqCorrValue);
  
  pstcRtc->WTCALEN = ((pstcConfig->bUseFreqCorr) == TRUE) ? 1 : 0 ;
  
  switch (pstcConfig->enDividerRatio)
  {
    case RtcDivRatio1:
      pstcRtc->WTDIV = 0x00;
      break;
    case RtcDivRatio2:
      pstcRtc->WTDIV = 0x01;
      break;
    case RtcDivRatio4:
      pstcRtc->WTDIV = 0x02;
      break;
    case RtcDivRatio8:
      pstcRtc->WTDIV = 0x03;
      break;
    case RtcDivRatio16:
      pstcRtc->WTDIV = 0x04;
      break;
    case RtcDivRatio32:
      pstcRtc->WTDIV = 0x05;
      break;
    case RtcDivRatio64:
      pstcRtc->WTDIV = 0x06;
      break;
    case RtcDivRatio128:
      pstcRtc->WTDIV = 0x07;
      break;
    case RtcDivRatio256:
      pstcRtc->WTDIV = 0x08;
      break;
    case RtcDivRatio512:
      pstcRtc->WTDIV = 0x09;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  pstcRtc->u8WTDIVEN.WTDIVEN = ((pstcConfig->bUseDivider) == TRUE) ? 1 : 0;
  
#endif
  
#if ((L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7))  
  
  if (pstcConfig->u8FreqCorrCycle > 0x3F)
  {
    return ErrorInvalidParameter;
  }
  pstcRtc->WTCALPRD = pstcConfig->u8FreqCorrCycle;
  
  switch (pstcConfig->enCoSignalDiv)
  {
    case RtcCoDiv1:
      pstcRtc->u8WTCOSEL.WTCOSEL = 0;
      break;
    case RtcCoDiv2:
      pstcRtc->u8WTCOSEL.WTCOSEL = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
  // Check for Interrupt enables
  if (0 != ((pstcConfig->u16AllInterrupts) & RTC_INTERRUPT_ENABLE_MASK))
  {
    Rtc_EnableDisableInterrupts(pstcRtc, pstcConfig); // No errors expected here (pointers checked previously)
    
    if (TRUE == bTouchNvic)
    {
      Rtc_InitIrq(pstcRtc);
    }
  }
  
  // Setup Alarm Registers
  Rtc_EnableDisableAlarmRegisters(pstcRtc, pstcConfig); // No errors expected here (pointers checked previously)
  
  // Start RTC
  pstcRtc->u32WTCR1.ST = 1;
    
  return Ok;
} // Rtc_Init

/**
 ******************************************************************************
 ** \brief Check and Set RTC Date and RTC Time
 **
 ** This function builds two 32-bit words for minimum write access to the
 ** RTC's date and time registers.
 **
 ** \note During update interrupts are globally disabled!
 **
 ** \param [in]     pstcRtc         RTC instance
 ** \param [in]     pstcRtcTime     RTC Date and Time structure
 ** \param [in]     bPollBusy       Include BUSY bit polling for update on
 **                                 runtime (if WTCR1.ST == 1)
 **
 ** \retval  Ok                       Successful initialization
 ** \retval  ErrorInvalidParameter    Invalid Date or Time format
 ** \retval  ErrorTimeout             Polling loop timed out
 ** \retval  ErrorOperationInProgress Update not possible
 ******************************************************************************/
en_result_t Rtc_SetDateTime(volatile stc_rtcn_t*    pstcRtc, 
                            stc_rtc_time_t*         pstcRtcTime,
                            boolean_t               bPollBusy)
{
  uint32_t u32WTSMRD = 0;
  uint32_t u32WTDMY  = 0;
  uint32_t u32BusyTimeout = RTC_BUSY_POLLING_TIMEOUT;

  if ((pstcRtc     == NULL) ||
      (pstcRtcTime == NULL))
  {
    return ErrorInvalidParameter ;
  }
  
  // Set Second
  if (pstcRtcTime->u8Second > 59)
  {
    return ErrorInvalidParameter ;
  }
  u32WTSMRD = (uint32_t)BinToBcd(pstcRtcTime->u8Second);
 
  // Set Minute
  if (pstcRtcTime->u8Minute > 59)
  {
    return ErrorInvalidParameter ;
  }
  u32WTSMRD |= (uint32_t)(BinToBcd(pstcRtcTime->u8Minute) << 8);
 
  // Set Hour
  if (pstcRtcTime->u8Hour > 23)
  {
    return ErrorInvalidParameter ;
  }
  u32WTSMRD |= (uint32_t)(BinToBcd(pstcRtcTime->u8Hour) << 16);
  
  // Set Day
  if ((pstcRtcTime->u8Day > 31)|| (pstcRtcTime->u8Day == 0))
  {
    return ErrorInvalidParameter ;
  }
  u32WTSMRD |= (uint32_t)(BinToBcd(pstcRtcTime->u8Day) << 24);

  // Set Day of the week
  if (pstcRtcTime->u8DayOfWeek > 6)
  {
    return ErrorInvalidParameter ;
  }
  u32WTDMY = (uint32_t)pstcRtcTime->u8DayOfWeek;

  // Set Month
  if ((pstcRtcTime->u8Month > 12) || (pstcRtcTime->u8Month == 0))
  {
    return ErrorInvalidParameter ;
  }
  u32WTDMY |= (uint32_t)(BinToBcd(pstcRtcTime->u8Month) << 8);
  
  // Set Year
  if ((pstcRtcTime->u8Year > 99) || (pstcRtcTime->u8Year == 0))
  {
    return ErrorInvalidParameter ;
  }
  u32WTDMY |= (uint32_t)(BinToBcd(pstcRtcTime->u8Year) << 16);

  if (pstcRtc->u32WTCR1.ST == 1)  // RTC running?
  {
    // RTC running ...
    if (TRUE == bPollBusy)
    {
      while(u32BusyTimeout && ((pstcRtc->u32WTCR1.BUSY != 0) || (pstcRtc->u32WTCR1.SCST != 0)))
      {
         u32BusyTimeout--;
      }
      
      if (0 == u32BusyTimeout)
      {
        return ErrorTimeout;
      }
    }
    
    __disable_irq();  // Next instructions must be free of interruptions
    
    // Re-check bits within disabled interrupts period
    if ((pstcRtc->u32WTCR1.BUSY != 0) || (pstcRtc->u32WTCR1.SCST != 0))
    {
       __enable_irq();   // Restore normal interupt handling
       return ErrorOperationInProgress; 
    }
    
    pstcRtc->u32WTCR1.SCST = 1;
  }
  else
  {
    // RTC not running ...
    __disable_irq();  // Next instructions must be free of interruptions
  }
  
  // update RTC with two 32-bit word write accesses
  pstcRtc->WTSMRD = u32WTSMRD;  
  pstcRtc->WTDMY  = u32WTDMY;
  
  if (pstcRtc->u32WTCR1.ST == 1)  // RTC running?
  {
    pstcRtc->u32WTCR1.SCST = 0;
  }
  
  __enable_irq();   // Restore normal interupt handling

  return Ok;
} // Rtc_SetDateTime


/**
 ******************************************************************************
 ** \brief Check and Set Alarm Date and Alarm Time
 **
 ** \param [in]     pstcRtc         RTC instance
 ** \param [in]     pstcRtcAlarm    RTC Alarm Date & Time structure
 **
 ** \retval  Ok                     Successful initialization
 ** \retval  ErrorInvalidParameter  Invalid Date or Time format
 **
 ******************************************************************************/
en_result_t Rtc_SetAlarmDateTime(volatile stc_rtcn_t*    pstcRtc, 
                                 stc_rtc_alarm_t*        pstcRtcAlarm)
{
  // Alarm Day
  if ((pstcRtcAlarm->u8AlarmDay > 31) || (pstcRtcAlarm->u8AlarmDay == 0))
  {
    return ErrorInvalidParameter ;
  }
  
  // Alarm Hour
  if (pstcRtcAlarm->u8AlarmHour > 23)
  {
    return ErrorInvalidParameter ;
  }
  
  // Alarm Minute
  if (pstcRtcAlarm->u8AlarmMinute > 59)
  {
    return ErrorInvalidParameter ;
  }
  
  // Alarm Year
  if ((pstcRtcAlarm->u8AlarmYear > 99) || (pstcRtcAlarm->u8AlarmYear == 0))
  {
    return ErrorInvalidParameter ;
  }
  
  // Alarm Month
  if ((pstcRtcAlarm->u8AlarmMonth > 12) || (pstcRtcAlarm->u8AlarmMonth == 0))
  {
    return ErrorInvalidParameter ;
  }
  
  pstcRtc->ALDR  = BinToBcd(pstcRtcAlarm->u8AlarmDay);
  pstcRtc->ALHR  = BinToBcd(pstcRtcAlarm->u8AlarmHour);
  pstcRtc->ALMIR = BinToBcd(pstcRtcAlarm->u8AlarmMinute);
  pstcRtc->ALYR  = BinToBcd(pstcRtcAlarm->u8AlarmYear);
  pstcRtc->ALMOR = BinToBcd(pstcRtcAlarm->u8AlarmMonth);
  
  return Ok;
} // Rtc_SetAlarmDateTime

/**
 ******************************************************************************
 ** \brief Enable and/or Disable RTC (and Timer) Interrupts
 **
 ** With this function the interrupts en- and disabled by the RTC configuration
 ** can be changed.
 **
 ** \param [in] pstcRtc           RTC instance
 ** \param [in] pstcConfig        RTC configuration structure
 ** \retval Ok                    Interrupt settings done
 ** \retval ErrorInvalidParameter pstcRtc == NULL, pstcConfig == NULL
 ******************************************************************************/
en_result_t Rtc_EnableDisableInterrupts( volatile stc_rtcn_t* pstcRtc, 
                                         stc_rtc_config_t*    pstcConfig
                                       )
{
  union {
    stc_rtc_wtcr1_t  stcWtcr1;
    uint32_t         u32Wtcr1;
  };
  
  // Check for NULL pointers
  if ((pstcRtc    == NULL) ||
      (pstcConfig == NULL) )
  {
      return ErrorInvalidParameter ;
  }
  
  u32Wtcr1 = pstcRtc->WTCR1;  // Readout WTCR1
  
  stcWtcr1.INTCRIE = ((pstcConfig->stcRtcInterruptEnable.ReadCompletionIrqEn)   == 1) ? 1 : 0 ;
  stcWtcr1.INTERIE = ((pstcConfig->stcRtcInterruptEnable.TimeRewriteErrorIrqEn) == 1) ? 1 : 0 ;
  stcWtcr1.INTALIE = ((pstcConfig->stcRtcInterruptEnable.AlarmIrqEn)            == 1) ? 1 : 0 ;
  stcWtcr1.INTTMIE = ((pstcConfig->stcRtcInterruptEnable.TimerIrqEn)            == 1) ? 1 : 0 ;
  stcWtcr1.INTHIE  = ((pstcConfig->stcRtcInterruptEnable.OneHourIrqEn)          == 1) ? 1 : 0 ;
  stcWtcr1.INTMIE  = ((pstcConfig->stcRtcInterruptEnable.OneMinuteIrqEn)        == 1) ? 1 : 0 ;
  stcWtcr1.INTSIE  = ((pstcConfig->stcRtcInterruptEnable.OneSecondIrqEn)        == 1) ? 1 : 0 ;
  stcWtcr1.INTSSIE = ((pstcConfig->stcRtcInterruptEnable.HalfSecondIrqEn)       == 1) ? 1 : 0 ;
  
  pstcRtc->WTCR1 = u32Wtcr1;  // Update WTCR1
  
  return Ok;
} // Rtc_EnableDisableInterrupts

/**
 ******************************************************************************
 ** \brief Enable and/or Disable RTC Alarm Register
 **
 ** With this function the Alarm registers can be en- or disabled according
 ** the configuration
 **
 ** \param [in] pstcRtc           RTC instance
 ** \param [in] pstcConfig        RTC configuration structure
 ** \retval Ok                    Alarm Register set
 ** \retval ErrorInvalidParameter pstcRtc == NULL, pstcConfig == NULL
 ******************************************************************************/
en_result_t Rtc_EnableDisableAlarmRegisters( volatile stc_rtcn_t* pstcRtc, 
                                             stc_rtc_config_t*    pstcConfig
                                           )
{
  union {
    stc_rtc_wtcr1_t  stcWtcr1;
    uint32_t         u32Wtcr1;
  };
  
  // Check for NULL pointers
  if ((pstcRtc    == NULL) ||
      (pstcConfig == NULL) )
  {
      return ErrorInvalidParameter ;
  }
  
  u32Wtcr1 = pstcRtc->WTCR1;  // Readout WTCR1
  
  stcWtcr1.YEN  = ((pstcConfig->stcAlarmRegisterEnable.AlarmYearEnable)   == 1) ? 1 : 0 ;
  stcWtcr1.MOEN = ((pstcConfig->stcAlarmRegisterEnable.AlarmMonthEnable)  == 1) ? 1 : 0 ;
  stcWtcr1.DEN  = ((pstcConfig->stcAlarmRegisterEnable.AlarmDayEnable)    == 1) ? 1 : 0 ;
  stcWtcr1.HEN  = ((pstcConfig->stcAlarmRegisterEnable.AlarmHourEnable)   == 1) ? 1 : 0 ;
  stcWtcr1.MIEN = ((pstcConfig->stcAlarmRegisterEnable.AlarmMinuteEnable) == 1) ? 1 : 0 ;
  
  pstcRtc->WTCR1 = u32Wtcr1;  // Update WTCR1
  
  return Ok;
} // Rtc_EnableDisableAlarmRegisters

/**
 ******************************************************************************
 ** \brief Read recent Time of RTC in polling mode
 **
 ** This function requests a copy of the recent time to the RTC registers and
 ** waits until finish. The recent time is transfered to the configuration
 ** pointer structure.
 **
 ** \note In this function a possible enabled Read Completion Interrupt
 **       (INTCRI) is temporarily disabled.
 **
 ** \param [in]       pstcRtc          RTC instance
 ** \param [in,out]   pstcRtcTime      RTC Date and Time structure
 ** \retval Ok                         Date and time sucessfully transfered
 ** \retval ErrorInvalidParameter      pstcRtc == NULL, pstcConfig == NULL
 ** \retval ErrorTimeout               Polling loop timed out
 ******************************************************************************/
en_result_t Rtc_ReadDateTimePolling( volatile stc_rtcn_t* pstcRtc, 
                                     stc_rtc_time_t*      pstcRtcTime
                                   )
{
  uint32_t u32TimeOutCount = 0;
  uint32_t u32IntcrieSave;
  
  // Check for NULL pointers
  if ((pstcRtc     == NULL) ||
      (pstcRtcTime == NULL) )
  {
      return ErrorInvalidParameter ;
  }
  
  u32IntcrieSave = pstcRtc->WTCR1; // Save INTCRIE bit
  pstcRtc->u32WTCR1.INTCRIE = 0;   // Disable INTCRIE
    
  // Wait for readiness
  u32TimeOutCount = RTC_POLLING_TIMEOUT;
  while ((1 == pstcRtc->u32WTCR2.CREAD) && u32TimeOutCount)
  {
    L3_WAIT_LOOP_HOOK();
    
    u32TimeOutCount--;
  }
  
  if (0 == u32TimeOutCount)
  {
    // Restore INTCRE bit
    pstcRtc->WTCR1 |= (RTC_WTCR1_INTCRI_FLAG_POSITION & u32IntcrieSave);
    
    return ErrorTimeout;
  }
  
  // Start transfer
  pstcRtc->u32WTCR2.CREAD = 1;
  
  // Wait for readiness
  u32TimeOutCount = RTC_POLLING_TIMEOUT;
  while ((1 == pstcRtc->u32WTCR2.CREAD) && u32TimeOutCount)
  {
    L3_WAIT_LOOP_HOOK();
    
    u32TimeOutCount--;
  }
  
  if (0 == u32TimeOutCount)
  {
    // Restore INTCRE bit
    pstcRtc->WTCR1 |= (RTC_WTCR1_INTCRI_FLAG_POSITION & u32IntcrieSave);
    
    return ErrorTimeout;
  }

  // Clear INTCRI flag
  pstcRtc->u32WTCR1.INTCRI = 0;
  
  // Store Date & Time in pointered structure
  pstcRtcTime->u8Second    = BcdToBin(pstcRtc->WTSR);
  pstcRtcTime->u8Minute    = BcdToBin(pstcRtc->WTMIR);
  pstcRtcTime->u8Hour      = BcdToBin(pstcRtc->WTHR);
  pstcRtcTime->u8Day       = BcdToBin(pstcRtc->WTDR);
  pstcRtcTime->u8DayOfWeek = BcdToBin(pstcRtc->WTDW);
  pstcRtcTime->u8Month     = BcdToBin(pstcRtc->WTMOR);
  pstcRtcTime->u8Year      = BcdToBin(pstcRtc->WTYR);
  
  // Restore INTCRE bit
  pstcRtc->WTCR1 |= (RTC_WTCR1_INTCRI_FLAG_POSITION & u32IntcrieSave);
  
  return Ok;
} // Rtc_ReadDateTimePolling

/**
 ******************************************************************************
 ** \brief Request for Read recent Time of RTC
 **
 ** This function requests a copy of the recent time to the RTC registers.
 ** In the RTC ISR the callback function stc_rtc_config_t#pfnReadCallback
 ** with all its 7 arguments for date and time is called. 
 **
 ** \note This function needs the INTCRIE bit (Read Completion Interrupt
 **       Enable) set to '1' by Rtc_Init() or Rtc_EnableDisableInterrupts()
 **
 ** \param [in]      pstcRtc           RTC instance
 ** \retval Ok                         Date and time sucessfully transfered
 ** \retval ErrorInvalidParameter      pstcRtc == NULL
 ** \retval ErrorNotReady              A previously request was not finished
 ******************************************************************************/
en_result_t Rtc_RequestDateTime( volatile stc_rtcn_t* pstcRtc )
{
  // Check for NULL pointer
  if (pstcRtc == NULL)
  {
      return ErrorInvalidParameter ;
  }
  
  // Check if a former request was started and did not finish yet ...
  if (pstcRtc->u32WTCR2.CREAD == 1)
  {
    return ErrorNotReady;
  }
  
  // Start request
  pstcRtc->u32WTCR2.CREAD = 1;
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief Read Clock Status of RTC
 **
 ** This function provides the enumerated status (#en_rtc_clock_status_t) of the
 ** WTCLKM register
 **
 ** \param [in]   pstcRtc                  RTC instance
 ** \retval       RtcRinClockNoOperation   RIN Clock not operating
 ** \retval       RtcSubClockSelected      Sub Clock is selected
 ** \retval       RtcMainClockSelected     Main Clock is selected
 ** \retval       RtcInvalidInstance       Error: RTC instance pointer == NULL
 ******************************************************************************/
en_rtc_clock_status_t Rtc_StatusRead( volatile stc_rtcn_t* pstcRtc )
{
  uint8_t               u8ReadOutWtclkm;
  en_rtc_clock_status_t enStatus;
  
  // Check for NULL pointer
  if ( pstcRtc == NULL )
  {
      return RtcCSInvalidInstance ;
  }
  
  u8ReadOutWtclkm = 0x3 & (pstcRtc->WTCLKM);
  
  switch (u8ReadOutWtclkm)
  {
    case 0:
    case 1:
      enStatus = RtcRinClockNoOperation;
      break;
    case 2:
      enStatus = RtcSubClockSelected;
      break;
    case 3:
      enStatus = RtcMainClockSelected;
      break;
    default:
      break;  // will never ever happen!
  }
  
  return enStatus;
} // Rtc_StatusRead

/**
 ******************************************************************************
 ** \brief Set Timer Status of RTC
 **
 ** This function sets the mode and Timer value of the RTC Timer.
 **
 ** \pre The RTC has to be initialized before to use this function properly. 
 **      Also use the Timer Interrupt enable by Rtc_Init()
 **
 ** \param [in]   pstcRtc                RTC instance
 ** \param [in]   pstcConfig             Timer Configuration
 ** \retval       Ok                     Timer sucessfully set
 ** \retval       ErrorInvalidParameter  pstcRtc == NULL, pstcConfig == NULL,
 **                                      invalid Counter Value
 ******************************************************************************/
en_result_t Rtc_TimerSet( volatile stc_rtcn_t*     pstcRtc,
                          stc_rtc_timer_config_t*  pstcConfig )
{
  // Check for NULL pointers
  if ((pstcRtc    == NULL) ||
      (pstcConfig == NULL) )
  {
    return ErrorInvalidParameter ;
  }
  
  if ((pstcConfig->u32TimerValue) > 0x2FFFF)
  {
    return ErrorInvalidParameter ;
  }
  
  // First stop the timer
  pstcRtc->u32WTCR2.TMST = 0;
  
  pstcRtc->u32WTCR2.TMEN = ((pstcConfig->bTimerIntervalEnable) == TRUE) ? 1 : 0 ;
  
  // Set up new Value
  pstcRtc->WTTR = pstcConfig->u32TimerValue;
  
  return Ok;
} // Rtc_TimerSet

/**
 ******************************************************************************
 ** \brief Start Timer Status of RTC
 **
 ** This function starts the Timer of the RTC Timer.
 **
 ** \pre The RTC has to be initialized before to use this function properly. 
 **      Also use the Timer Interrupt enable by Rtc_Init()
 **
 ** \param [in]   pstcRtc                RTC instance
 ** \retval       Ok                     Timer sucessfully set
 ** \retval       ErrorInvalidParameter  pstcRtc == NULL
 ******************************************************************************/
en_result_t Rtc_TimerStart( volatile stc_rtcn_t* pstcRtc )
{
  // Check for NULL pointer
  if (pstcRtc == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  pstcRtc->u32WTCR2.TMST = 1;

  return Ok;
} // Rtc_TimerStart


/**
 ******************************************************************************
 ** \brief Stops Timer Status of RTC
 **
 ** This function stops the Timer of the RTC Timer.
 **
 ** \pre The RTC has to be initialized before to use this function properly. 
 **      Also use the Timer Interrupt enable by Rtc_Init()
 **
 ** \param [in]   pstcRtc                RTC instance
 ** \retval       Ok                     Timer sucessfully set
 ** \retval       ErrorInvalidParameter  pstcRtc == NULL
 ******************************************************************************/
en_result_t Rtc_TimerStop( volatile stc_rtcn_t* pstcRtc )
{
  // Check for NULL pointer
  if (pstcRtc == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  pstcRtc->u32WTCR2.TMST = 0;

  return Ok;
} // Rtc_TimerStop

/**
 ******************************************************************************
 ** \brief Read Timer Status of RTC
 **
 ** This function provides the status of the TMRUN bit of the WTCR2 register.
 **
 ** \param [in]   pstcRtc                  RTC instance
 ** \retval       RtcTimerNoOperation      Timer Counter is not operating
 ** \retval       RtcTinerInOperation      Timer Counter is operationg
 ** \retval       RtcInvalidInstance       Error: RTC instance pointer == NULL
 ******************************************************************************/
en_rtc_timer_status_t Rtc_TimerStatusRead( volatile stc_rtcn_t* pstcRtc )
{
  uint32_t              u8ReadOutWtcr2;
  en_rtc_timer_status_t enStatus;
  
  // Check for NULL pointer
  if ( pstcRtc == NULL )
  {
    return RtcTSInvalidInstance ;
  }
  
  u8ReadOutWtcr2 = (pstcRtc->u32WTCR2.TMRUN);
  
  switch (u8ReadOutWtcr2)
  {
    case 0:
      enStatus = RtcTimerNoOperation;
      break;
    case 1:
      enStatus = RtcTinerInOperation;
      break;
    default:
      break;  // will never ever happen!
  }
  
  return enStatus;
} // Rtc_TimerStatusRead

/**
 ******************************************************************************
 ** \brief De-Initialize RTC
 **
 ** This function initializes the RTC module and its interrupts, if bTouchNVIC
 ** is TRUE.
 **
 ** \param [in] pstcRtc           RTC instance
 ** \param [in] bTouchNvic        TRUE = touch shared NVIC registers, FALSE = do
 **                               not touch
 ** \retval Ok                    Internal data has been setup
 ** \retval ErrorInvalidParameter pstcRtc == NULL
 ******************************************************************************/
en_result_t Rtc_DeInit( volatile stc_rtcn_t* pstcRtc, 
                        boolean_t            bTouchNvic
                      )
{
  // Check for NULL pointer
  if ( pstcRtc == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  if (TRUE == bTouchNvic)
  {
    Rtc_DeInitIrq(pstcRtc);
  }
  
  // Set all registers of RTC instance to '0'
  pstcRtc->WTCR1   = 0;
  pstcRtc->WTCR2   = 0;
  pstcRtc->WTBR    = 0;
  pstcRtc->WTDR    = 0;
  pstcRtc->WTHR    = 0;
  pstcRtc->WTMIR   = 0;
  pstcRtc->WTSR    = 0;
  pstcRtc->WTYR    = 0;
  pstcRtc->WTDW    = 0;
  pstcRtc->ALDR    = 0;
  pstcRtc->ALHR    = 0;
  pstcRtc->ALMIR   = 0;
  pstcRtc->ALMOR   = 0;
  pstcRtc->WTCLKS  = 0;
  pstcRtc->WTCLKM  = 0;
  
#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
     (L3_DEVICE_TYPE == L3_TYPE4) || \
     (L3_DEVICE_TYPE == L3_TYPE5))  
  pstcRtc->WTCAL   = 0;
  pstcRtc->WTCALEN = 0;
  pstcRtc->WTDIV   = 0;
  pstcRtc->WTDIVEN = 0;
#endif
  
#if ((L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7)) 
  pstcRtc->WTCALPRD = 0;
  pstcRtc->WTCOSEL = 0;
#endif
  
  return Ok;
} // Rtc_DeInit

/**
 ******************************************************************************
 ** \brief Get raw time
 **
 ** This function calculates the "raw" time ('UNIX time').
 **
 ** \param [in] pstcRtcTime     RTC Time structure
 ** \return                     Calculated time or '-1' on error
 ******************************************************************************/
time_t Rtc_GetRawTime( stc_rtc_time_t* pstcRtcTime )
{
  struct tm stcTime;
  
  // Check for NULL pointer
  if ( pstcRtcTime == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  RtcToTm(pstcRtcTime, &stcTime);
  
  return mktime(&stcTime);
} // Rtc_GetRawTime

/**
 ******************************************************************************
 ** \brief Set Day of the Week
 **
 ** This function calculates the day of the week from YY-MM-DD in the Time
 ** structure. It uses mktime of time.h library.
 **
 ** \param [in,out] pstcRtcTime     RTC Time structure
 ** \retval Ok                       Internal data has been setup
 ** \retval ErrorInvalidParameter    pstcRtcTime == NULL or mktime failed
 ******************************************************************************/
en_result_t Rtc_SetDayOfWeek( stc_rtc_time_t* pstcRtcTime )
{
  struct tm stcTime;
  
  // Check for NULL pointer
  if ( pstcRtcTime == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  RtcToTm(pstcRtcTime, &stcTime);
  
  if(mktime(&stcTime) == -1)
  {
    return ErrorInvalidParameter ;
  }
  
  pstcRtcTime->u8DayOfWeek = stcTime.tm_wday;
  
  return Ok;
} // Rtc_SetDayOfWeek

/**
 ******************************************************************************
 ** \brief Sets the RTC time structure from raw time
 **
 ** This function calculates from the RTC time structure "raw" time
 ** ('UNIX time').
 **
 ** \param [out] pstcRtcTime         RTC Time structure
 ** \param [in]  tRawTime            "Raw" time
 ** \retval Ok                       Internal data has been setup
 ** \retval ErrorInvalidParameter    pstcRtcTime == NULL or localtime failed
 ******************************************************************************/
en_result_t  Rtc_SetTime( stc_rtc_time_t* pstcRtcTime,
                          time_t          tRawTime )
{
  struct tm* pstcTime;
  
  // Check for NULL pointer
  if ( pstcRtcTime == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  pstcTime = localtime((const time_t*) &tRawTime);
  
  if (pstcTime == NULL)
  {
    return ErrorInvalidParameter ;
  }

  pstcRtcTime->u8Year      = (pstcTime->tm_year + 1900) - 2000;
  pstcRtcTime->u8Month     = pstcTime->tm_mon + 1;
  pstcRtcTime->u8Day       = pstcTime->tm_mday;
  pstcRtcTime->u8Second    = pstcTime->tm_sec;
  pstcRtcTime->u8Minute    = pstcTime->tm_min;
  pstcRtcTime->u8Hour      = pstcTime->tm_hour;
  pstcRtcTime->u8DayOfWeek = pstcTime->tm_wday;
  
  return Ok;
} // Rtc_GetRawTime

//@} // RtcGroup

#endif // #if (defined(L3_PERIPHERAL_RTC_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
