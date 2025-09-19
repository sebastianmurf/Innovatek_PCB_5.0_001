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
/** \file wc.h
 **
 ** Headerfile for WC functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-11  1.1  MWi  Sub clock time out added, configuration structure
 **                           changed for global tm structure pointer
 **
 ******************************************************************************/

#ifndef __WC_H__
#define __WC_H__

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
 ** \defgroup WcGroup Wc Functions (WC)
 **
 ** Provided functions of WC module:
 ** 
 ** - Wc_Init()
 ** - Wc_DeInit() 
 ** - Wc_GetTime()
 ** - Wc_SetTime()
 **
 ** \brief How to use WC module
 **
 ** Wc_Init() has to be called after configuration (date, time, prescaler
 ** settings, etc.) was done. This driver module uses the time.h standard
 ** library.
 ** Wc_SetTime() calculates from the date and time given in the configuration
 ** from the elements with prefix 'Set' the Unix time and sets the internal
 ** data stc_wc_intern_data#u32RawTime.
 ** Wc_GetTime() calculates from stc_wc_intern_data#u32RawTime the date and
 ** time and sets the substructure stc_wc_config#stcCalendar.
 **
 ** \note 
 **       - Because sub clock enable/disable is used the sub clock oscillation
 **         is checked and is not touched by Wc_Init() or Wc_DeInit(), if 
 **         enabled before. The WC driver works independently from the CLK
 **         driver.
 **       - This driver uses the subclock. Connecting a subclock crystal to
 **         XA0 and XA1 is mandatory!
 **       - Because the WC interrupt is shared with other interrupts (e.g.
 **         Clock Stabilization), Wc_Init() and Wc_DeInit() have an argument,
 **         whether to touch the NVIC registers or not to prevent interference
 **         with other driver parts.
 **
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief WcCallback function prototype.
 ******************************************************************************/
typedef void (*wc_cb_func_ptr_t)(void);

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define stc_wcn_t    FM3_WC_TypeDef  
#define WC0          (*((FM3_WC_TypeDef *) FM3_WC_BASE)) 

#define WC_SUBCLOCK_TIMEOUT 10000000  // Value for approx. 80 MHz MCLK

/**
 ******************************************************************************
 ** \brief Month name definitions (not used in driver - to be used by
 **        user applciation)
 ******************************************************************************/
#define Wc_January   0
#define Wc_Febuary   1
#define Wc_March     2
#define Wc_April     3
#define Wc_May       4
#define Wc_June      5
#define Wc_July      6
#define Wc_August    7
#define Wc_September 8
#define Wc_October   9
#define Wc_November  10
#define Wc_December  11

/**
 ******************************************************************************
 ** \brief Year calculation macro for adjusting time.h year
 ******************************************************************************/
#define Wc_Year(a) (a - 1900)

/**
 ******************************************************************************
 ** \brief Wc Prescaler
 ** 
 ** To select clock divider
 ** To trigger the WC only the sub-clock is used
 ******************************************************************************/
typedef enum en_wc_prescaler
{
    WcPrescalerDiv1000ms  = 0,  	///< Prescaler divisor 2^15, WCCK3
    WcPrescalerDiv500ms   = 1,  	///< Prescaler divisor 2^14, WCCK2
    WcPrescalerDiv250ms   = 2,   	///< Prescaler divisor 2^13, WCCK1
    WCPrescalerDiv125ms	  = 3		///< Prescaler divisor 2^12, WCCK0
} en_wc_prescaler_t;

/**
 ******************************************************************************
 ** \brief Wc configuration
 ** 
 ** Contains all parameter for configuratin of the WC
 ******************************************************************************/
 
typedef struct stc_wc_config
{
  boolean_t             bEnable;        ///< Timer enable
  en_wc_prescaler_t     enPrescalerDiv; ///< See description of #en_dt_prescaler_t 
  uint8_t               u8SetSecond;    ///< second to set
  uint8_t		u8SetMinute;    ///< minutes to set
  uint8_t		u8SetHour;      ///< hour to set
  uint8_t		u8SetDay;	///< day to set
  uint8_t		u8SetMonth;     ///< month to set
  uint8_t		u8SetYear;      ///< year to set
  int8_t		i8SetIsdst;     ///< timezone correction, daylight savings time
  struct tm*            pstcCalendar;   ///< time.h tm struct for recent time
  wc_cb_func_ptr_t      pfnCallback;    ///< Function pointer to Callback 
} stc_wc_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled WC instance
typedef enum en_wc_instance_index
{
  #if (L3_PERIPHERAL_ENABLE_WC0 == L3_ON)
  WcInstanceIndexWc0,
  #endif
  // Case if no DT instance is selected
  #if (L3_PERIPHERAL_ENABLE_WC0 == L3_OFF)
  WcInstanceDummy
  #endif
} en_wc_instance_index_t;

/// Datatype for holding internal data needed for WC
typedef struct stc_wc_intern_data
{
  func_ptr_t  pfnCallbackWc0 ; ///< Callback for interrupts of WC
  uint32_t    u32RawTime ;     ///< Unit Time (seconds since 1970-01-01)
  struct tm*  pstcCalendar ;   ///< Pointer to global calendar structure
  boolean_t   bSubClockWasEnabled;   ///< TRUE: Sub clock was enabled before initialization of WC
} stc_wc_intern_data_t ;

/// WC instance data type
typedef struct stc_wc_instance_data
{
  volatile stc_wcn_t*  pstcInstance;  ///< pointer to registers of an instance
  stc_wc_intern_data_t stcInternData; ///< module internal data of instance
} stc_wc_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled WC instances and their internal data
extern stc_wc_instance_data_t m_astcWcInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void WcIrqHandler0( volatile stc_wcn_t* pstcWc,
                    stc_wc_intern_data_t* pstcWcInternData
                   ) ;

en_result_t Wc_Init( volatile stc_wcn_t* pstcWc, 
                     stc_wc_config_t*    pstcConfig,
                     boolean_t           bTouchNvic
                   ) ;

en_result_t Wc_DeInit( volatile stc_wcn_t* pstcWc,
                       boolean_t           bTouchNvic
                     ) ;

en_result_t Wc_GetTime( volatile stc_wcn_t* pstcWc ) ;

en_result_t Wc_SetTime( volatile stc_wcn_t* pstcWc,
                        stc_wc_config_t* pstcConfig) ;

//@} // WcGroup

#ifdef __cplusplus
}
#endif

#endif /* __WC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
