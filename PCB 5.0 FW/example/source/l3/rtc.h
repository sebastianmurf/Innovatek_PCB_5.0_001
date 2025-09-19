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
/** \file rtc.h
 **
 ** Headerfile for RTC functions
 **  
 **
 ** History:
 **   - 2012-06-25  1.0  MWi  First version.
 **   - 2012-08-31  1.1  MWi  Register typedef name scheme changed
 **
 ******************************************************************************/

#ifndef __RTC_H__
#define __RTC_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "l3.h"
#include "mcu.h"
#include "time.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup RtcGroup Real Time Clock (RTC)
 **
 ** Provided functions of RTC module:
 ** 
 ** - Rtc_Init()
 ** - Rtc_SetDateTime()
 ** - Rtc_SetAlarmDateTime()
 ** - Rtc_EnableDisableInterrupts()
 ** - Rtc_EnableDisableAlarmRegisters()
 ** - Rtc_ReadDateTimePolling()
 ** - Rtc_RequestDateTime()
 ** - Rtc_StatusRead()
 ** - Rtc_TimerSet()
 ** - Rtc_TimerStart()
 ** - Rtc_TimerStop()
 ** - Rtc_TimerStatusRead()
 ** - Rtc_DeInit()
 ** - Rtc_GetRawTime()
 ** - Rtc_SetDayOfWeek()
 ** - Rtc_SetTime()
 **
 ** Rtc_Init() initializes the RTC with the given time and date given in the
 ** RTC configuration. It also initialized the NVIC, if specidfied.
 ** Rtc_SetDateTime() allows to set a new date and time.
 ** Rtc_SetAlarmDateTime() allows to set a new alarm date and time
 ** Rtc_EnableDisableInterrupts() allows to change the interrupt configurations
 ** on runtime without touching the RTC work.
 ** With Rtc_EnableDisableAlarmRegisters() the comparison to the recent time
 ** can be adjusted on runtime.
 ** Rtc_ReadClockStatus() reads out the status of the RTC.
 ** Rtc_TimerSet() sets the timer value for its interval or one-shot counting.
 ** Rtc_TimerStart() starts and Rtc_TimerStop() stops the Timer.
 ** With Rtc_ReadDateTimePolling() the recent date and time can be stored in
 ** the pointered configuration structure while the Read Completion Interrupt
 ** is temporarily disabled.
 ** Rtc_ReadTimerStatus() reads out the Timer status.
 ** And finally Rtc_DeInit() allows to de-initialize all RTC functions and
 ** possible interrupts. Also here to touch the NVIC registers can be set.
 **
 ** Rtc_GetRawTime() is a helper function, which calculates the 'raw' time
 ** (UNIX time) from the RTC time structure #stc_rtc_time_t.
 ** Rtc_SetDayOfWeek() is a helper function, which sets the day of the week
 ** calculated from the date and time given in #stc_rtc_time_t.
 ** Rtc_SetTime() is a helper function which calculates the RTC time structure
 ** from the 'raw' time.
 **
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief Rtccallback function prototypes.
 ******************************************************************************/

/******************************************************************************
 * Global definitions
 ******************************************************************************/

#define RTC0 (*((volatile stc_rtcn_t *)L3_RTC_BASE))
  
#define RTC_INTERRUPT_ENABLE_MASK  0x0F     ///< Number of Irq bit fields, see #stc_rtc_interrupts_t
#define RTC_POLLING_TIMEOUT        100000   ///< Timeout for WTCR2.CREAD bit polling
#define RTC_BUSY_POLLING_TIMEOUT   200000   ///< Timeout for BUSY bit polling
 
/**
 ******************************************************************************
 ** \brief Year calculation macro for adjusting RTC year format
 ******************************************************************************/
#define Rtc_Year(a) (a - 2000)  
  
/**
 ******************************************************************************
 ** \brief Month name definitions (not used in driver - to be used by
 **        user applciation)
 ******************************************************************************/
#define Rtc_January   1
#define Rtc_Febuary   2
#define Rtc_March     3
#define Rtc_April     4
#define Rtc_May       5
#define Rtc_June      6
#define Rtc_July      7
#define Rtc_August    8
#define Rtc_September 9
#define Rtc_October   10
#define Rtc_November  11
#define Rtc_December  12
 
/**
 ******************************************************************************
 ** \brief Day of week name definitions (not used in driver - to be used by
 **        user applciation)
 ******************************************************************************/
#define Rtc_Sunday    0
#define Rtc_Monday    1
#define Rtc_Tuesday   2
#define Rtc_Wednesday 3
#define Rtc_Thursday  4
#define Rtc_Friday    5
#define Rtc_Saturday  6
  
/**
 ******************************************************************************
 ** \brief Bit Positions of Interrupt Flasgs in WTCR1
 ******************************************************************************/  
#define RTC_WTCR1_INTSSI_FLAG_POSITION 0x00010000
#define RTC_WTCR1_INTSI_FLAG_POSITION  0x00020000 
#define RTC_WTCR1_INTMI_FLAG_POSITION  0x00040000
#define RTC_WTCR1_INTHI_FLAG_POSITION  0x00080000
#define RTC_WTCR1_INTTMI_FLAG_POSITION 0x00100000
#define RTC_WTCR1_INTALI_FLAG_POSITION 0x00200000
#define RTC_WTCR1_INTERI_FLAG_POSITION 0x00400000
#define RTC_WTCR1_INTCRI_FLAG_POSITION 0x00800000
  
/**
 ******************************************************************************
 ** \brief Maxmimum Frequency Correction Value and type cast according Device
 **        Types
 ******************************************************************************/  
#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5))
  #define RTC_MAX_FREQ_CORR_VALUE 0x7F
  typedef uint8_t rtc_frq_corr_t;
  
#elif ((L3_DEVICE_TYPE == L3_TYPE6) || \
       (L3_DEVICE_TYPE == L3_TYPE7))
  #define RTC_MAX_FREQ_CORR_VALUE 0x3FF
  typedef uint16_t rtc_frq_corr_t;

#endif
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/** function pointer type to void/uint8_t... function */
typedef void  (*func_ptr_rtc_arglist_t)(uint8_t,  // Second
                                        uint8_t,  // Minute
                                        uint8_t,  // Hour
                                        uint8_t,  // Day
                                        uint8_t,  // Day of Week
                                        uint8_t,  // Month
                                        uint8_t); // Year

/**
 ******************************************************************************
 ** \brief Redefinition of WTCLKS register
 ******************************************************************************/
typedef struct stc_rtc_wtclks
{
  __IO uint8_t WTCLKS    : 1;
       uint8_t RESERVED  : 7;
} stc_rtc_wtclks_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTCLKM register
 ******************************************************************************/
typedef struct stc_rtc_wtclkm
{
  __IO uint8_t WTCLKM    : 2;
       uint8_t RESERVED  : 6;
} stc_rtc_wtclkm_t;

#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
     (L3_DEVICE_TYPE == L3_TYPE4) || \
     (L3_DEVICE_TYPE == L3_TYPE5)) 
/**
 ******************************************************************************
 ** \brief Redefinition of WTCAL register (Clock Control Block A)
 ******************************************************************************/
typedef struct stc_rtc_wtcal
{
  __IO uint8_t WTCAL     : 7;
       uint8_t RESERVED  : 1;
} stc_rtc_wtcal_t;

#elif ((L3_DEVICE_TYPE == L3_TYPE6) || \
       (L3_DEVICE_TYPE == L3_TYPE7))
/**
 ******************************************************************************
 ** \brief Redefinition of WTCAL register (Clock Control Block B)
 ******************************************************************************/
typedef struct stc_rtc_wtcal
{
  __IO uint16_t WTCAL     : 10;
       uint16_t RESERVED  : 6;
} stc_rtc_wtcal_t;

#endif

/**
 ******************************************************************************
 ** \brief Redefinition of WTCALEN register
 ******************************************************************************/
typedef struct stc_rtc_wtcalen
{
  __IO uint8_t WTCALEN   : 1;
       uint8_t RESERVED  : 7;
} stc_rtc_wtcalen_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTDIV register
 ******************************************************************************/
typedef struct stc_rtc_wtdiv
{
  __IO uint8_t WTDIV     : 4;
       uint8_t RESERVED  : 4;
} stc_rtc_wtdiv_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTDIVEN register
 ******************************************************************************/
typedef struct stc_rtc_wtdiven
{
  __IO uint8_t WTDIVEN   : 1;
  __IO uint8_t WTDIVRDY  : 1;  
       uint8_t RESERVED  : 6;
} stc_rtc_wtdiven_t;

#if ((L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7))
/**
 ******************************************************************************
 ** \brief Redefinition of WTCALPRD register (Clock Control Block B)
 ******************************************************************************/
typedef struct stc_rtc_wtcalprd
{
  __IO uint8_t WTCALPRD  : 6;
       uint8_t RESERVED  : 2;
} stc_rtc_wtcalprd_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTCOSEL register (Clock Control Block B)
 ******************************************************************************/
typedef struct stc_rtc_wtcosel
{
  __IO uint8_t WTCOSEL   : 1;
       uint8_t RESERVED  : 7;
} stc_rtc_wtcosel_t;
#endif

/**
 ******************************************************************************
 ** \brief Redefinition of WTCR1 register
 ******************************************************************************/
typedef struct stc_rtc_wtcr1
{
  __IO uint32_t ST        : 1;
       uint32_t RESERVED0 : 1;
  __IO uint32_t RUN       : 1;
  __IO uint32_t SRST      : 1;
  __IO uint32_t SCST      : 1;
  __IO uint32_t SCRST     : 1;
  __IO uint32_t BUSY      : 1;
       uint32_t RESERVED1 : 1;
       
  __IO uint32_t MIEN      : 1;
  __IO uint32_t HEN       : 1;
  __IO uint32_t DEN       : 1;
  __IO uint32_t MOEN      : 1;
  __IO uint32_t YEN       : 1;
       uint32_t RESERVED2 : 3;
       
  __IO uint32_t INTSSI    : 1;
  __IO uint32_t INTSI     : 1;
  __IO uint32_t INTMI     : 1;
  __IO uint32_t INTHI     : 1;
  __IO uint32_t INTTMI    : 1;
  __IO uint32_t INTALI    : 1;
  __IO uint32_t INTERI    : 1;
  __IO uint32_t INTCRI    : 1;
  
  __IO uint32_t INTSSIE   : 1;
  __IO uint32_t INTSIE    : 1;
  __IO uint32_t INTMIE    : 1;
  __IO uint32_t INTHIE    : 1;
  __IO uint32_t INTTMIE   : 1;
  __IO uint32_t INTALIE   : 1;
  __IO uint32_t INTERIE   : 1;
  __IO uint32_t INTCRIE   : 1;
} stc_rtc_wtcr1_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTCR1 register
 ******************************************************************************/
typedef struct stc_rtc_wtcr2
{
  __IO uint32_t CREAD     : 1;
       uint32_t RESERVED0 : 7;
       
  __IO uint32_t TMST      : 1;
  __IO uint32_t TMEN      : 1;
  __IO uint32_t TMRUN     : 1;
  __IO uint32_t RESERVED1 : 5;
  
       uint32_t RESERVED2 : 16;
} stc_rtc_wtcr2_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTBR register
 ******************************************************************************/
typedef struct stc_rtc_wtbr
{
  __IO uint32_t BR        : 24;
       uint32_t RESERVED  : 8;
} stc_rtc_wtbr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTDR register
 ******************************************************************************/
typedef struct stc_rtc_wtdr
{
  __IO uint8_t D          : 4;
  __IO uint8_t TD         : 2;
       uint8_t RESERVED   : 2;
} stc_rtc_wtdr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTHR register
 ******************************************************************************/
typedef struct stc_rtc_wthr
{
  __IO uint8_t H          : 4;
  __IO uint8_t TH         : 2;
       uint8_t RESERVED   : 2;
} stc_rtc_wthr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTMIR register
 ******************************************************************************/
typedef struct stc_rtc_wtmir
{
  __IO uint8_t MI         : 4;
  __IO uint8_t TMI        : 3;
       uint8_t RESERVED   : 1;
} stc_rtc_wtmir_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTSR register
 ******************************************************************************/
typedef struct stc_rtc_wtsr
{
  __IO uint8_t S          : 4;
  __IO uint8_t TS         : 3;
       uint8_t RESERVED   : 1;
} stc_rtc_wtsr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTYR register
 ******************************************************************************/
typedef struct stc_rtc_wtyr
{
  __IO uint8_t Y          : 4;
  __IO uint8_t TY         : 4;
} stc_rtc_wtyr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTMOR register
 ******************************************************************************/
typedef struct stc_rtc_wtmor
{
  __IO uint8_t MO         : 4;
  __IO uint8_t TMO        : 1;
       uint8_t RESERVED   : 3;
} stc_rtc_wtmor_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTDW register
 ******************************************************************************/
typedef struct stc_rtc_wtdw
{
  __IO uint8_t DW         : 3;
       uint8_t RESERVED   : 5;
} stc_rtc_wtdw_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ALDR register
 ******************************************************************************/
typedef struct stc_rtc_aldr
{
  __IO uint8_t AD         : 4;
  __IO uint8_t TAD        : 2;  
       uint8_t RESERVED   : 2;
} stc_rtc_aldr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ALHR register
 ******************************************************************************/
typedef struct stc_rtc_alhr
{
  __IO uint8_t AH         : 4;
  __IO uint8_t TAH        : 2;
       uint8_t RESERVED   : 2;
} stc_rtc_alhr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ALMIR register
 ******************************************************************************/
typedef struct stc_rtc_almir
{
  __IO uint8_t AMI        : 4;
  __IO uint8_t TAMI       : 3;
       uint8_t RESERVED   : 1;
} stc_rtc_almir_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ALYR register
 ******************************************************************************/
typedef struct stc_rtc_alyr
{
  __IO uint8_t AY         : 4;
  __IO uint8_t TAY        : 4;
} stc_rtc_alyr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ALMOR register
 ******************************************************************************/
typedef struct stc_rtc_almor
{
  __IO uint8_t AMO        : 4;
  __IO uint8_t TAMO       : 1;
       uint8_t RESERVED   : 3;
} stc_rtc_almor_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WTTR register
 ******************************************************************************/
typedef struct stc_rtc_wttr
{
  __IO uint32_t TM        : 18;
       uint32_t RESERVED  : 14;
} stc_rtc_wttr_t;

/**
 ******************************************************************************
 ** \brief Redefinition of RTC register set
 ******************************************************************************/
typedef struct stc_rtcn
{
  union {
    __IO uint32_t          WTCR1;
         stc_rtc_wtcr1_t   u32WTCR1;
  };
  union {
    __IO uint32_t          WTCR2;
         stc_rtc_wtcr2_t   u32WTCR2;
  };
  union {
    __IO uint32_t          WTBR;
         stc_rtc_wtbr_t    u32WTBR;
  };
  union {
    __IO uint32_t          WTSMRD;  // New driver defintion
    union {
      union {
        __IO uint8_t           WTSR;
             stc_rtc_wtsr_t    u8WTSR;
      };
      union {
        __IO uint8_t           WTMIR;
             stc_rtc_wtmir_t   u8WTMIR;
      };
      union {
        __IO uint8_t           WTHR;
             stc_rtc_wthr_t    u8WTHR;
      };
      union {
        __IO uint8_t           WTDR;
             stc_rtc_wtdr_t    u8WTDR;
      };
    };
  };
  union {
    __IO uint32_t              WTDMY;  // New driver defintion 
    union {
      union {
        __IO uint8_t           WTDW;
             stc_rtc_wtdw_t    u8WTDW;
      };
      union {
        __IO uint8_t           WTMOR;
             stc_rtc_wtmor_t   u8WTMOR;
      };
      union {
        __IO uint8_t           WTYR;
             stc_rtc_wtyr_t    u8WTYR;
      };
      uint8_t                  RESERVED0;
    };
  };
  uint8_t                       RESERVED1;
  union {
    __IO uint8_t           ALMIR;
         stc_rtc_almir_t   u8ALMIR;
  };
  union {
    __IO uint8_t           ALHR;
         stc_rtc_alhr_t    u8ALHR;
  };
  union {
    __IO uint8_t           ALDR;
         stc_rtc_aldr_t    u8ALDR;
  };
  uint8_t                  RESERVED2;
  union {
    __IO uint8_t           ALMOR;
         stc_rtc_almor_t   u8ALMOR;
  };
  union {
    __IO uint8_t           ALYR;
         stc_rtc_alyr_t    u8ALYR;
  };
  uint8_t                  RESERVED3;
  union {
    __IO uint32_t          WTTR;
         stc_rtc_wttr_t    u32WTTR;
  };  
  union {
    __IO uint8_t           WTCLKS;
         stc_rtc_wtclks_t  u8WTCLKS;
  };
  union {
    __IO uint8_t           WTCLKM;
         stc_rtc_wtclkm_t  u8WTCLKM;
  };
  uint16_t                 RESERVED4;

#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
     (L3_DEVICE_TYPE == L3_TYPE4) || \
     (L3_DEVICE_TYPE == L3_TYPE5)) 
  union {
    __IO uint8_t           WTCAL;
         stc_rtc_wtcal_t   u8WTCAL;
  };
  union {
    __IO uint8_t           WTCALEN;
         stc_rtc_wtcalen_t u8WTCALEN;
  };
  uint16_t                 RESERVED5;
  union {
    __IO uint8_t           WTDIV;
         stc_rtc_wtdiv_t   u8WTDIV;
  };
  union {
    __IO uint8_t           WTDIVEN;
         stc_rtc_wtdiven_t u8WTDIVEN;
  };
  
#elif ((L3_DEVICE_TYPE == L3_TYPE6) || \
       (L3_DEVICE_TYPE == L3_TYPE7))
  union {
    __IO uint16_t          WTCAL;
         stc_rtc_wtcal_t   u16WTCAL;
  };
  union {
    __IO uint8_t           WTCALEN;
         stc_rtc_wtcalen_t u8WTCALEN;
  };
  uint8_t                  RESERVED5;
  union {
    __IO uint8_t           WTDIV;
         stc_rtc_wtdiv_t   u8WTDIV;
  };
  union {
    __IO uint8_t           WTDIVEN;
         stc_rtc_wtdiven_t u8WTDIVEN;
  };
  uint16_t                 RESERVED6;
  union {
    __IO uint8_t            WTCALPRD;
         stc_rtc_wtcalprd_t u8WTCALPRD;
  };
  uint8_t                  RESERVED7[3];
  union {
    __IO uint8_t           WTCOSEL;
         stc_rtc_wtcosel_t u8WTCOSEL;
  };
#endif
} stc_rtcn_t;

/**
 ******************************************************************************
 ** \brief Rtc Clock Status
 ** 
 ** Read-out Status od RTC Clock Status (WTCLKM Register)
 ******************************************************************************/
typedef enum en_rtc_clock_status
{
  RtcRinClockNoOperation = 0, ///< RIN Clock not operating
  RtcSubClockSelected    = 1, ///< Sub Clock is selected
  RtcMainClockSelected   = 2, ///< Main Clock is selected
  RtcCSInvalidInstance   = 3  ///< Error: RTC instance pointer == NULL
} en_rtc_clock_status_t ;

/**
 ******************************************************************************
 ** \brief Rtc Timer Status
 ** 
 ** Read-out Status od RTC Timer Status (WTCR2.TMRUN Register Bit)
 ******************************************************************************/
typedef enum en_rtc_timer_status
{
  RtcTimerNoOperation  = 0, ///< Timer Counter is not operating
  RtcTinerInOperation  = 1, ///< Timer Counter is operationg
  RtcTSInvalidInstance = 2  ///< Error: RTC instance pointer == NULL
} en_rtc_timer_status_t ;

/**
 ******************************************************************************
 ** \brief Rtc Divider Ratio Setting
 ** 
 ** Divider Ration Settings for WTC. The enumarted values do not correspond to
 ** the bit patterns of WTDIV!
 ******************************************************************************/
typedef enum en_rtc_divider_ratio
{
  RtcDivRatio1    = 0,  ///< RIN clock is not divided
  RtcDivRatio2    = 1,  ///< RIN clock is divided by 2
  RtcDivRatio4    = 2,  ///< RIN clock is divided by 4
  RtcDivRatio8    = 3,  ///< RIN clock is divided by 8
  RtcDivRatio16   = 4,  ///< RIN clock is divided by 16
  RtcDivRatio32   = 5,  ///< RIN clock is divided by 32
  RtcDivRatio64   = 6,  ///< RIN clock is divided by 64
  RtcDivRatio128  = 7,  ///< RIN clock is divided by 128
  RtcDivRatio256  = 8,  ///< RIN clock is divided by 256
  RtcDivRatio512  = 9   ///< RIN clock is divided by 512
} en_rtc_divider_ratio_t ;

#if ((L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7))
/**
 ******************************************************************************
 ** \brief Rtc CO Division
 ******************************************************************************/
typedef enum en_rtc_co_div
{
  RtcCoDiv1 = 0,  ///< CO signal of RTC count part is output
  RtcCoDiv2 = 1   ///< Division of 2 of CO signal is output
} en_rtc_co_div_t ;
#endif

/**
 ******************************************************************************
 ** \brief Interrupt configuration structure
 ** 
 ** The RTC interrupt configuration structure
 ******************************************************************************/
typedef struct stc_rtc_interrupts
{
  uint16_t ReadCompletionIrqEn   : 1; ///< 1: RTC Read Completion interrupt enabled
                                      ///< 0: RTC Read Completion interrupt disabled
  uint16_t TimeRewriteErrorIrqEn : 1; ///< 1: Time Rewrite Error interrupt enabled
                                      ///< 0: Time Rewrite Error interrupt disabled
  uint16_t AlarmIrqEn            : 1; ///< 1: Alarm interrupt enabled
                                      ///< 0: Alarm interrupt disabled
  uint16_t TimerIrqEn            : 1; ///< 1: Timer interrupt enabled
                                      ///< 0: Timer interrupt disabled  
  uint16_t OneHourIrqEn          : 1; ///< 1: 1-Hour interrupt enabled
                                      ///< 0: 1-Hour interrupt disabled
  uint16_t OneMinuteIrqEn        : 1; ///< 1: 1-Hour interrupt enabled
                                      ///< 0: 1-Hour interrupt disabled
  uint16_t OneSecondIrqEn        : 1; ///< 1: 1-Second interrupt enabled
                                      ///< 0: 1-Second interrupt disabled
  uint16_t HalfSecondIrqEn       : 1; ///< 1: 0.5-Second interrupt enabled
                                      ///< 0: 0.5-Second interrupt disabled
  uint16_t Reserved              : 8;
} stc_rtc_interrupts_t;

/**
 ******************************************************************************
 ** \brief Alarm Register En- and Disable
 ** 
 ** The RTC Alarm register configuration structure
 ******************************************************************************/
typedef struct stc_rtc_alarm_register
{
  uint8_t AlarmYearEnable       : 1; ///< 1: RTC Alarm Year Register enabled
                                     ///< 0: RTC Alarm Year Register disnabled
  uint8_t AlarmMonthEnable      : 1; ///< 1: RTC Alarm Month Register enabled
                                     ///< 0: RTC Alarm Month Register disnabled
  uint8_t AlarmDayEnable        : 1; ///< 1: RTC Alarm Day Register enabled
                                     ///< 0: RTC Alarm Day Register disnabled
  uint8_t AlarmHourEnable       : 1; ///< 1: RTC Alarm Hour Register enabled
                                     ///< 0: RTC Alarm Hour Register disnabled
  uint8_t AlarmMinuteEnable     : 1; ///< 1: RTC Alarm Minute Register enabled
                                     ///< 0: RTC Alarm Minute Register disnabled
  uint8_t Reserved              : 3;
} stc_rtc_alarm_register_t;

/**
 ******************************************************************************
 ** \brief Real Time Clock Date and Time structure
 ******************************************************************************/
typedef struct stc_rtc_time
{
  uint8_t                u8Second;       ///< Second (Format 0...59)
  uint8_t		 u8Minute;       ///< Minutes (Format 0...59)
  uint8_t		 u8Hour;         ///< Hour (Format 0...23)
  uint8_t		 u8Day;	         ///< Day (Format 1...31)
  uint8_t		 u8DayOfWeek;    ///< Day of the week (Format 0...6)
  uint8_t	         u8Month;        ///< Month (Foramt 1...12)
  uint8_t	         u8Year;         ///< Year (Format 1...99) + 2000
} stc_rtc_time_t;

/**
 ******************************************************************************
 ** \brief Real Time Clock Alarm structure
 ******************************************************************************/
typedef struct stc_rtc_alarm
{
  uint8_t	         u8AlarmMinute;  ///< Alarm minutes (Format 1...59)
  uint8_t	         u8AlarmHour;    ///< Alarm hour (Format 0...23)  
  uint8_t	         u8AlarmDay;     ///< Alarm day (Format 1...31) 
  uint8_t	         u8AlarmMonth;   ///< Alarm month (Foramt 1...12)
  uint8_t	         u8AlarmYear;    ///< Alarm year (Format 1...99) + 2000  
} stc_rtc_alarm_t;

/**
 ******************************************************************************
 ** \brief Real Time Clock configuration
 ** 
 ** The RTC configuration settings
 ******************************************************************************/
typedef struct stc_rtc_config
{
  uint32_t               u32RtcClock;    ///< Used RTC input clock for sub second register
  boolean_t              bUseMainClock;  ///< TRUE: Use man clock as input clock source
                                         ///< FALSE: Use sub clock as input clock source
  boolean_t              bUseFreqCorr;   ///< TRUE: Use Frequency correction value
                                         ///< FALSE: Do not use Frequency correction value
  uint16_t               u16FreqCorrValue; ///< Frequency correction value (6 bit for Type3/4/6, 10 bit for Type6/7
  boolean_t              bUseDivider;    ///< TRUE: Use Divider for Divider Ratio
                                         ///< FALSE: Disable Divider for Divider Ratio
  en_rtc_divider_ratio_t enDividerRatio; ///< Divider Ratio Setting, see #en_rtc_divider_ratio_t for details
#if ((L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7))
  uint8_t                u8FreqCorrCycle; /// < Frequency Correction Cycle Value
  en_rtc_co_div_t        enCoSignalDiv;  ///< Divider of CO signal, see #en_rtc_co_div_t for details
#endif
  stc_rtc_alarm_register_t stcAlarmRegisterEnable; ///< see #stc_rtc_alarm_register_t for details
  union {
    stc_rtc_interrupts_t  stcRtcInterruptEnable; ///< See #stc_rtc_interrupts_t for details
    uint16_t              u16AllInterrupts;      ///< Only used by driver
  };
  func_ptr_rtc_arglist_t pfnReadCallback; ///< Callback function pointer for read completion Interrupt
  func_ptr_t             pfnAlarmCallback; ///< Callback function pointer Alarm Interrupt
  func_ptr_t             pfnTimerCallback; ///< Callback function pointer Timer Interrupt
  func_ptr_t             pfnHalfSecondCallback; ///< Callback function pointer 0.5-Second Interrupt
  func_ptr_t             pfnOneSecondCallback; ///< Callback function pointer One Second Interrupt
  func_ptr_t             pfnOneMinuteCallback; ///< Callback function pointer One Minute Interrupt
  func_ptr_t             pfnOneHourCallback;   ///< Callback function pointer One Hour Interrupt
} stc_rtc_config_t;

/**
 ******************************************************************************
 ** \brief Real Time Clock Timer configuration
 ** 
 ** The RTC Timer configuration settings
 ******************************************************************************/
typedef struct stc_rtc_timer_config
{
  uint32_t     u32TimerValue;         ///< 18-Bit vlaue for RTC Timer
  boolean_t    bTimerIntervalEnable;  ///< TRUE: Timer is set in count interval mode
                                      ///< FALSE: Timer is set in count elapse mode ('one shot')
} stc_rtc_timer_config_t ;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled RTC instance
typedef enum en_rtc_instance_index
{
  #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
  RtcInstanceIndexRtc0,
  #endif
  // Case if no RTC instance is selected
  #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_OFF)
  RtcInstanceDummy
  #endif
} en_rtc_instance_index_t;

/// Datatype for holding internal data needed for RTC
typedef struct stc_rtc_intern_data
{
  func_ptr_rtc_arglist_t  pfnReadCallback;  ///< Callback function pointer for read completion Interrupt
  func_ptr_t              pfnAlarmCallback; ///< Callback function pointer Alarm Interrupt
  func_ptr_t              pfnTimerCallback; ///< Callback function pointer Timer Interrupt
  func_ptr_t              pfnHalfSecondCallback; ///< Callback function pointer 0.5-Second Interrupt
  func_ptr_t              pfnOneSecondCallback; ///< Callback function pointer One Second Interrupt
  func_ptr_t              pfnOneMinuteCallback; ///< Callback function pointer One Minute Interrupt
  func_ptr_t              pfnOneHourCallback;   ///< Callback function pointer One Hour Interrupt
  
  func_ptr_t  pfnCallbackRtc0 ; ///< Callback for interrupts of WC
  uint32_t    u32RawTime ;      ///< Unit Time (seconds since 1970-01-01)
  struct tm*  pstcCalendar ;    ///< Pointer to global calendar structure
  boolean_t   bSubClockWasEnabled;   ///< TRUE: Sub clock was enabled before initialization of WC
} stc_rtc_intern_data_t ;

/// RTC instance data type
typedef struct stc_rtc_instance_data
{
  volatile stc_rtcn_t*  pstcInstance;  ///< pointer to registers of an instance
  stc_rtc_intern_data_t stcInternData; ///< module internal data of instance
} stc_rtc_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled RTC instances and their internal data
extern stc_rtc_instance_data_t m_astcRtcInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
void RtcIrqHander( volatile stc_rtcn_t*   pstcRtc,
                   stc_rtc_intern_data_t* pstcRtcInternData
                 ) ;

extern en_result_t Rtc_Init( volatile stc_rtcn_t* pstcRtc, 
                             stc_rtc_config_t*    pstcConfig,
                             stc_rtc_time_t*      pstcTime,
                             stc_rtc_alarm_t*     pstcAlarm,
                             boolean_t            bTouchNvic
                           ) ;

extern en_result_t Rtc_SetDateTime(volatile stc_rtcn_t*  pstcRtc, 
                                   stc_rtc_time_t*       pstcTime,
                                   boolean_t             bPollBusy) ;

extern en_result_t Rtc_SetAlarmDateTime(volatile stc_rtcn_t*  pstcRtc, 
                                        stc_rtc_alarm_t*      pstcRtcAlarm) ;

extern en_result_t Rtc_EnableDisableInterrupts( volatile stc_rtcn_t* pstcRtc, 
                                                stc_rtc_config_t*    pstcConfig
                                              ) ;

extern en_result_t Rtc_EnableDisableAlarmRegisters( volatile stc_rtcn_t* pstcRtc, 
                                                    stc_rtc_config_t*    pstcConfig
                                                  ) ;

extern en_result_t Rtc_ReadDateTimePolling( volatile stc_rtcn_t* pstcRtc, 
                                            stc_rtc_time_t*      pstcRtcTime
                                          ) ;

extern en_result_t Rtc_RequestDateTime( volatile stc_rtcn_t* pstcRtc ) ;

extern en_rtc_clock_status_t Rtc_StatusRead( volatile stc_rtcn_t* pstcRtc ) ;

extern en_result_t Rtc_TimerSet( volatile stc_rtcn_t*     pstcRtc,
                                 stc_rtc_timer_config_t*  pstcConfig
                               ) ;

extern en_result_t Rtc_TimerStart( volatile stc_rtcn_t* pstcRtc ) ;

extern en_result_t Rtc_TimerStop( volatile stc_rtcn_t* pstcRtc ) ;

extern en_rtc_timer_status_t Rtc_TimerStatusRead( volatile stc_rtcn_t* pstcRtc ) ;

extern en_result_t Rtc_DeInit( volatile stc_rtcn_t* pstcRtc, 
                               boolean_t            bTouchNvic
                             ) ;

extern time_t Rtc_GetRawTime( stc_rtc_time_t* pstcRtcTime ) ;

extern en_result_t Rtc_SetDayOfWeek( stc_rtc_time_t* pstcRtcTime ) ;

extern en_result_t  Rtc_SetTime( stc_rtc_time_t* pstcRtcTime,
                                 time_t          tRawTime );

//@} // DsmGroup

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
