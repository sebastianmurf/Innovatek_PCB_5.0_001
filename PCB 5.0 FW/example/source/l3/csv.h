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
/** \file csv.h
 **
 ** Headerfile for CSV functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-10  1.1  MWi  Function Csv_EnableDisableParam() added
 **
 ******************************************************************************/

#ifndef __CSV_H__
#define __CSV_H__

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
 ** \defgroup CsvGroup Clock Supervisor (CSV)
 **
 ** \pre The clock(s) to be supervised must be enabled and stable!
 ** \pre For anomalous frequency detection the High Speed CR must be trimmed!
 **
 ** Provided functions of CSV module:
 ** - Csv_Init()
 ** - Csv_DeInit()
 ** - Csv_EnableDisable()
 ** - Csv_EnableDisableParam()
 ** - Csv_ReadStatus()
 **
 ** The function Csv_Init() initializes the Main- and Sub Clock Supervisor (CSV)
 ** if enabled and the (Anomalous) Frequency Count (Detection) Supervisor (FCS)
 ** if enabled.
 **
 ** The CSV is only able to perform a reset on failure detection. Therefore
 ** after reset (in the initialization part of the application) the function
 ** Csv_ReadStatus() should be used to determine a possible CSV failure
 ** reset cause.
 **
 ** The FCS first generates an interrupt on anomalous frequency detection.
 ** In this case a callback function is called with an argument of the content
 ** of the Frequency Detection Counter Register (FCSWD_CTL). If
 ** stc_csv_config.bFcsIrqAutoClear is TRUE the interupt request is cleared
 ** within the ISR. If this configuration is not set, the user may set
 ** stc_csv_config.bFcsIrqEnable to FALSE in the callback function, call
 ** Csv_EnableDisable() to disable the interrupt and thus let the FCS force a
 ** reset on next FCS failure.
 **
 ** If Csv_EnableDisableParam() is used instead of Csv_EnableDisable() the
 ** configuration #stc_csv_config_t can be purged on runtime after
 ** initialization.
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/**
 ******************************************************************************
 ** \brief Mfscallback function prototypes.
 ******************************************************************************/
typedef void (*csv_func_ptr_t)(uint16_t);

/**
 ******************************************************************************
 ** \brief Clock Supervisor FCS Count Cycle settings
 **
 ** \note The ennumerated values do not correspond to the FCD bit patterns due
 **       to upward compatibility reasons!
 ******************************************************************************/  
typedef enum en_csv_fcs_count_cycle
{
  CsvFcsFreqDiv256  = 0 ,  ///< 1/256 frequency of high-speed CR oscillation
  CsvFcsFreqDiv512  = 1 ,  ///< 1/512 frequency of high-speed CR oscillation
  CsvFcsFreqDiv1024 = 2    ///< 1/1024 frequency of high-speed CR oscillation
}
en_csv_fcs_count_cycle_t ;

/**
 ******************************************************************************
 ** \brief Clock Supervisor status
 **
 ** These enumeration values are returned, when calling Csv_ReadStatus().
 ** This function should be called after Reset to determine a possible clock
 ** failure detection.
 **
 ** \note This function only can be called one (after failure detection), 
 **       because it reads out the CSV_STR register, which is auto-cleared 
 **       after reading!
 ******************************************************************************/  
typedef enum en_csv_status
{
  CsvNoDetection    = 0 ,  ///< No failure detected
  CsvMainFailure    = 1 ,  ///< Main clock failure detected
  CsvSubFailure     = 2 ,  ///< Sub clock failure detected
  CsvMainSubFailure = 3    ///< Main- and Sub clock failure detected
}
en_csv_status_t ;

/**
 ******************************************************************************
 ** \brief Clock Supervisor internal data
 ******************************************************************************/ 
typedef struct stc_csv_intern_data
{
  boolean_t         bFcsIrqAutoClear ;  ///< TRUE: FCS Interrupt is cleared automatically in its ISR
  csv_func_ptr_t    pfnCallback ;       ///< CSV interrupt callback function
} stc_csv_intern_data_t ;

/**
 ******************************************************************************
 ** \brief Clock Supervisor configuration
 ** 
 ** The Clock Supervisor configuration settings
 ******************************************************************************/
typedef struct stc_csv_config
{
  boolean_t                   bMainCsvEnable ;         ///< TRUE: Main Clock Supervisor enabled
  boolean_t                   bSubCsvEnable ;          ///< TRUE: Sub Clock Supervisor enabled
  boolean_t                   bFcsEnable ;             ///< TRUE: FCS enabled
  boolean_t                   bFcsResetEnable ;        ///< TRUE: FCS Reset enabled
  boolean_t                   bFcsIrqEnable ;          ///< TRUE: FCS interrupt enabled
  boolean_t                   bFcsIrqAutoClear ;       ///< TRUE: FCS Interrupt is cleared automatically in its ISR
  en_csv_fcs_count_cycle_t    enFcsCountCycle;         ///< see #en_csv_fcs_count_cycle_t for details
  uint16_t                    u16FreqencyWindowLower ; ///< Frequency detection window lower setting
  uint16_t                    u16FreqencyWindowUpper ; ///< Frequency detection window upper setting
  csv_func_ptr_t              pfnCallback ;            ///< FCS interrupt callback function
} stc_csv_config_t ;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern stc_csv_intern_data_t stcCsvInternData ; 

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void CsvIrqHandler(void) ;

extern en_result_t Csv_Init(stc_csv_config_t* pstcConfig) ;

extern en_result_t Csv_DeInit(void) ;

extern en_result_t Csv_EnableDisable(stc_csv_config_t* pstcConfig) ;

extern en_result_t Csv_EnableDisableParam( boolean_t bMainCsvEnable,
                                           boolean_t bSubCsvEnable,
                                           boolean_t bFcsEnable,
                                           boolean_t bFcsIrqEnable,
                                           boolean_t bFcsIrqAutoClear
                                         ) ;

extern en_csv_status_t Csv_ReadStatus(void) ;

//@} // CsvGroup

#ifdef __cplusplus
}
#endif

#endif /* __CSV_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
