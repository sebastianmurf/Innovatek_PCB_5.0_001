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
/** \file csv.c
 **
 ** A detailed description is available at 
 ** @link CsvGroup Clock Supervisor Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-10  1.1  MWi  Function Csv_EnableDisableParam() added
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "csv.h"

#if (defined(L3_PERIPHERAL_CSV_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup CsvGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
stc_csv_intern_data_t stcCsvInternData = {NULL} ;

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
 ** \brief FCS ISR of the CSV module
 ** 
 ** If the frequency clock supervisor detects a first anomalous frequency
 ** this ISR is called. The frequency detection counter value then is taken
 ** as the argument of the callback function.
 ******************************************************************************/
void CsvIrqHandler(void)
{
  if (stcCsvInternData.bFcsIrqAutoClear == TRUE)
  {
    FM3_CRG->INT_CLR_f.FCSC = 0; // Clear FCS interrupt
  }
  
  stcCsvInternData.pfnCallback(FM3_CRG->FCSWD_CTL) ; // FCS Callback function
} // CsvIrqHandler

/**
 ******************************************************************************
 ** \brief Initialisation of the CSV module
 **
 ** This function initializes and enables the Clock Supervisor according the
 ** configuration.
 **
 ** \pre If a clock is to be supervised, be sure that the correspondig clock
 **      is enabled and its stabilization wait time has ended! Use the CLK
 **      driver functions to ensure this!
 **
 ** \param [in]  pstcConfig    CSV module configuration 
 **
 ** \retval Ok                    Initializiation of CSV module successfully 
 **                               done.
 ** \retval ErrorInvalidParameter pstcConfig == NULL, invalid FCS Count Cycle
 **                               setting.
 ******************************************************************************/
en_result_t Csv_Init(stc_csv_config_t* pstcConfig)
{
  // Check for valid pointer
  if (pstcConfig == NULL)
  {
      return ErrorInvalidParameter;
  }
  
  // Setup callback function pointer in internal data
  stcCsvInternData.pfnCallback = pstcConfig->pfnCallback ;
  
  // Setup FCS IRQ auto-clear to internal data for handling within ISR
  stcCsvInternData.bFcsIrqAutoClear = pstcConfig->bFcsIrqAutoClear ;
  
  // Frequency clock supervisor to be enabled?
  if (pstcConfig->bFcsEnable == TRUE)   
  {
    // Setup frequency window
    FM3_CRG->FCSWH_CTL = pstcConfig->u16FreqencyWindowUpper ;
    FM3_CRG->FCSWL_CTL = pstcConfig->u16FreqencyWindowLower ;
    
    switch(pstcConfig->enFcsCountCycle)
    {
      case CsvFcsFreqDiv256 :
        FM3_CRG->CSV_CTL = 0x50 ;   // FCSRE, FCSDE are set later
        break;
      case CsvFcsFreqDiv512 :
        FM3_CRG->CSV_CTL = 0x60 ;   // FCSRE, FCSDE are set later
        break;
      case CsvFcsFreqDiv1024 :
        FM3_CRG->CSV_CTL = 0x70 ;   // FCSRE, FCSDE are set later
        break;
      default:
        return ErrorInvalidParameter ;
    }
    
    FM3_CRG->CSV_CTL_f.FCSRE = (pstcConfig->bFcsResetEnable == TRUE) ? 1 : 0 ;
    
    // Init NVIC
    NVIC_ClearPendingIRQ(CSV_IRQn);
    NVIC_EnableIRQ(CSV_IRQn);
    NVIC_SetPriority(CSV_IRQn, L3_IRQ_LEVEL_CSV);
    
    // Clear possible interrupt ...
    FM3_CRG->INT_CLR_f.FCSC = 0 ;
    
    // ... and enable it according configuration
    FM3_CRG->INT_ENR_f.FCSE = (pstcConfig->bFcsIrqEnable == TRUE) ? 1 : 0 ;
    
    // Finally enable FCS
    FM3_CRG->CSV_CTL_f.FCSDE = 1 ;
  }
  
  // Enable supervisors according configuration
  FM3_CRG->CSV_CTL_f.MCSVE = (pstcConfig->bMainCsvEnable == TRUE) ? 1 : 0 ;
  FM3_CRG->CSV_CTL_f.SCSVE = (pstcConfig->bSubCsvEnable == TRUE)  ? 1 : 0 ;
  
  return Ok ;
} // ClkSvInit

/**
 ******************************************************************************
 ** \brief De-Initialisation of the CSV module
 **
 ** This function deinitializes and enables the Clock Supervisor.
 **
 **
 ** \retval Ok                 De-Initializiation of CSV module done. 
 ******************************************************************************/
en_result_t Csv_DeInit(void)
{
  FM3_CRG->INT_ENR_f.FCSE = 0 ; // Disable FCS interrupt
  FM3_CRG->CSV_CTL = 0;         // Disable FCS, Main CSV, Sub CSV
  
  FM3_CRG->FCSWH_CTL = 0 ;  // Reset upper frequency window
  FM3_CRG->FCSWL_CTL = 0 ;  // Reset lower frequency window

  // De-Init NVIC
  NVIC_ClearPendingIRQ(CSV_IRQn);
  NVIC_DisableIRQ(CSV_IRQn);
  NVIC_SetPriority(CSV_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  
  return Ok; 
} // ClkSvDeInit

/**
 ******************************************************************************
 ** \brief CSV module enable/disable parts by configuration
 **
 ** This function en- or disables the FCS, Main CSV, and Sub CSV according to
 ** its configuration. Also the FCS interrupt and its autoclear option can be
 ** en- or disabled with this function.
 **
 ** \pre CsvInit() must be called before
 **
 ** \param [in]  pstcConfig    CSV module configuration 
 **
 ** \retval Ok                    Change of CLKSV module settings successfully 
 **                               done.
 ** \retval ErrorInvalidParameter pstcConfig == NULL
 ******************************************************************************/
en_result_t Csv_EnableDisable(stc_csv_config_t* pstcConfig)
{
  // Check for valid pointer
  if (pstcConfig == NULL)
  {
      return ErrorInvalidParameter;
  }

  FM3_CRG->CSV_CTL_f.MCSVE = (pstcConfig->bMainCsvEnable == TRUE) ? 1 : 0 ;
  FM3_CRG->CSV_CTL_f.SCSVE = (pstcConfig->bSubCsvEnable == TRUE)  ? 1 : 0 ;
  FM3_CRG->CSV_CTL_f.FCSDE = (pstcConfig->bFcsEnable == TRUE)     ? 1 : 0 ;
  
  FM3_CRG->INT_ENR_f.FCSE  = (pstcConfig->bFcsIrqEnable == TRUE) ? 1 : 0 ;
  stcCsvInternData.bFcsIrqAutoClear = pstcConfig->bFcsIrqAutoClear ;
  
  return Ok;
} // CsvEnableDisable

/**
 ******************************************************************************
 ** \brief CSV module enable/disable parts by parameter
 **
 ** This function en- or disables the FCS, Main CSV, and Sub CSV according to
 ** its configuration. Also the FCS interrupt and its autoclear option can be
 ** en- or disabled with this function.
 **
 ** \pre CsvInit() must be called before, afterwards #stc_csv_config_t can be
 ** cleared on runtime, if Csv_EnableDisable() is not used
 **
 ** \param [in]  bMainCsvEnable    TRUE = CSV for Main Oscillator enable
 ** \param [in]  bSubCsvEnable     TRUE = CSV for Sub Oscillator enable
 ** \param [in]  bFcsEnable        TRUE = FCS enable
 ** \param [in]  bFcsIrqEnable     TRUE = FCS interrupt enable
 ** \param [in]  bFcsIrqAutoClear  TRUE = FCS interrupt auto-clear enable
 **
 ** \retval Ok                    Change of CLKSV module settings successfully 
 **                               done.
 ******************************************************************************/
en_result_t Csv_EnableDisableParam( boolean_t bMainCsvEnable,
                                    boolean_t bSubCsvEnable,
                                    boolean_t bFcsEnable,
                                    boolean_t bFcsIrqEnable,
                                    boolean_t bFcsIrqAutoClear
                                  )
{
  FM3_CRG->CSV_CTL_f.MCSVE = (bMainCsvEnable == TRUE) ? 1 : 0 ;
  FM3_CRG->CSV_CTL_f.SCSVE = (bSubCsvEnable == TRUE)  ? 1 : 0 ;
  FM3_CRG->CSV_CTL_f.FCSDE = (bFcsEnable == TRUE)     ? 1 : 0 ;
  
  FM3_CRG->INT_ENR_f.FCSE  = (bFcsIrqEnable == TRUE) ? 1 : 0 ;
  stcCsvInternData.bFcsIrqAutoClear = bFcsIrqAutoClear ;
  
  return Ok;
} // Csv_EnableDisableParam

/**
 ******************************************************************************
 ** \brief CSV module read status after failure
 **
 ** This function return the status of the Main- and Subclock Supervisor.
 ** Because the CSV_STR register is cleared during read, the return value
 ** is 'coded' in #en_csv_status_t.
 ** 
 ** This function should be called after Reset to determine a possible clock
 ** failure reset.
 **
 ** \retval CsvNoDetection      No failure has been detected.
 ** \retval CsvMainFailure      A Main clock only failure has been detected. 
 ** \retval CsvSubFailure       A Sub clock only failure has been detected. 
 ** \retval CsvMainSubFailure   A Main- and Sub clock failure has been detected.
 ******************************************************************************/
en_csv_status_t Csv_ReadStatus(void)
{
  uint16_t        u16RegisterRead ;
  en_csv_status_t enCsvReturnValue ;
  
  u16RegisterRead = 0x0003 & FM3_CRG->CSV_STR ; // Mask SCMF and MCMF
  
  switch (u16RegisterRead)
  {
    case 0x03 : // SCMF + MCMF
      enCsvReturnValue = CsvMainSubFailure ;
      break;
    case 0x02 : // SCMF
      enCsvReturnValue = CsvSubFailure ;
      break;
    case 0x01 : // MCMF
      enCsvReturnValue = CsvMainFailure ;
      break;
    case 0x00 : // No detection
    default :
      enCsvReturnValue = CsvNoDetection ;
  }
  
  return enCsvReturnValue ;
}

//@} // CsvGroup

#endif // #if (defined(L3_PERIPHERAL_CSV_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
