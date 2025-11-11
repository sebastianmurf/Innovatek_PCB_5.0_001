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
/** \file lvd.c
 **
 ** A detailed description is available at 
 ** @link LvdGroup Low voltage Detection description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-19  1.1  MWi  Change for configuration A, B, and C
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "lvd.h"

#if (defined(L3_PERIPHERAL_LVD_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup LvdGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
stc_lvd_intern_data_t stcLvdInternData = {NULL} ;

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
 ** \brief ISR of the LVD module
 ** 
 ******************************************************************************/
void LvdIrqHandler(void)
{
  LVD0->LVD_CLR = 0x00;  // Clear LVD interrupt
  
  if (stcLvdInternData.pfnCallback != NULL)
  {
    stcLvdInternData.pfnCallback();
  }
} // LvdIrqHandler

/**
 ******************************************************************************
 ** \brief Init the Interrupt of the LVD module
 ** 
 ** Because the LVD (reset) is always active, only generating interrupts
 ** can be used as a user option. This function enables the interrupt and the
 ** user setting voltage for the LVD interrupt.
 **
 ** \param [in]  pstcConfig    LVD module configuration 
 **
 ** \retval Ok                    Initializiation of LVD interupt successfully 
 **                               done.
 ** \retval ErrorInvalidParameter pstcConfig == NULL or invalid 
 **                               stc_lvd_config#enIrqVoltage setting.
 ******************************************************************************/
en_result_t Lvd_InitIrq(stc_lvd_config_t* pstcConfig)
{
  stc_lvd_ctll_field_t unLVD_CTLL ;   // local LVD_CTL predefintion
#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  stc_lvd_ctlh_field_t unLVD_CTLH;
#endif
  
  // Check for valid pointer
  if (pstcConfig == NULL)
  {
      return ErrorInvalidParameter;
  }

  unLVD_CTLL.LVDIE = 1;
  
  // Prepare LVD_CTLL
  switch (pstcConfig->enIrqVoltage)
  {
#if (L3_DEVICE_TYPE == L3_TYPE0) || \
    (L3_DEVICE_TYPE == L3_TYPE1) || \
    (L3_DEVICE_TYPE == L3_TYPE2) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5)
    
    case LvdIrqVoltage28 : 
      unLVD_CTLL.SVHI = 0;
      break;
    case LvdIrqVoltage30 : 
      unLVD_CTLL.SVHI = 1;
      break;
    case LvdIrqVoltage32 : 
      unLVD_CTLL.SVHI = 2;
      break;
    case LvdIrqVoltage36 : 
      unLVD_CTLL.SVHI = 3;
      break;
    case LvdIrqVoltage37 : 
      unLVD_CTLL.SVHI = 4;
      break;
    case LvdIrqVoltage40 : 
      unLVD_CTLL.SVHI = 7;
      break;
    case LvdIrqVoltage41 : 
      unLVD_CTLL.SVHI = 8;
      break;
    case LvdIrqVoltage42 : 
      unLVD_CTLL.SVHI = 9;
      break;
      
#elif (L3_DEVICE_TYPE == L3_TYPE3) || \
      (L3_DEVICE_TYPE == L3_TYPE7)
      
    case LvdIrqVoltage20 : 
      unLVD_CTLL.SVHI = 0;
      break;
    case LvdIrqVoltage21 : 
      unLVD_CTLL.SVHI = 1;
      break;
    case LvdIrqVoltage22 : 
      unLVD_CTLL.SVHI = 2;
      break;
    case LvdIrqVoltage23 : 
      unLVD_CTLL.SVHI = 3;
      break;
    case LvdIrqVoltage24 : 
      unLVD_CTLL.SVHI = 4;
      break;
    case LvdIrqVoltage25 : 
      unLVD_CTLL.SVHI = 5;
      break;
    case LvdIrqVoltage26 : 
      unLVD_CTLL.SVHI = 6;
      break;    
    case LvdIrqVoltage28 : 
      unLVD_CTLL.SVHI = 7;
      break;
    case LvdIrqVoltage30 : 
      unLVD_CTLL.SVHI = 8;
      break;
    case LvdIrqVoltage32 : 
      unLVD_CTLL.SVHI = 9;
      break;
    case LvdIrqVoltage36 : 
      unLVD_CTLL.SVHI = 10;
      break;
    case LvdIrqVoltage37 : 
      unLVD_CTLL.SVHI = 11;
      break;
    case LvdIrqVoltage40 : 
      unLVD_CTLL.SVHI = 12;
      break;
    case LvdIrqVoltage41 : 
      unLVD_CTLL.SVHI = 13;
      break;
    case LvdIrqVoltage42 : 
      unLVD_CTLL.SVHI = 14;
      break;   
      
#elif (L3_DEVICE_TYPE == L3_TYPE6)

    case LvdIrqVoltage170 : 
      unLVD_CTLL.SVHI = 4;
      break;
    case LvdIrqVoltage175 : 
      unLVD_CTLL.SVHI = 5;
      break;
    case LvdIrqVoltage180 : 
      unLVD_CTLL.SVHI = 6;
      break;    
    case LvdIrqVoltage185 : 
      unLVD_CTLL.SVHI = 7;
      break;
    case LvdIrqVoltage190 : 
      unLVD_CTLL.SVHI = 8;
      break;
    case LvdIrqVoltage195 : 
      unLVD_CTLL.SVHI = 9;
      break;
    case LvdIrqVoltage200 : 
      unLVD_CTLL.SVHI = 10;
      break;
    case LvdIrqVoltage205 : 
      unLVD_CTLL.SVHI = 11;
      break;
    case LvdIrqVoltage250 : 
      unLVD_CTLL.SVHI = 12;
      break;
    case LvdIrqVoltage260 : 
      unLVD_CTLL.SVHI = 13;
      break;
    case LvdIrqVoltage270 : 
      unLVD_CTLL.SVHI = 14;
      break;  
    case LvdIrqVoltage280 : 
      unLVD_CTLL.SVHI = 15;
      break;
    case LvdIrqVoltage290 : 
      unLVD_CTLL.SVHI = 16;
      break;
    case LvdIrqVoltage300 : 
      unLVD_CTLL.SVHI = 17;
      break;
    case LvdIrqVoltage310 : 
      unLVD_CTLL.SVHI = 18;
      break;     
    case LvdIrqVoltage320 : 
      unLVD_CTLL.SVHI = 19;
      break;  
      
#else
  #error Device Type not supported!
#endif
      
    default :
      return ErrorInvalidParameter;  
  }
 
#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  
  unLVD_CTLH.LVDRE = ((pstcConfig->bEnableLvdReset) == TRUE) ? 1 : 0 ;
  
  // Prepare LVD_CTLH
  switch (pstcConfig->enResVoltage)
  {
    
  #if (L3_DEVICE_TYPE == L3_TYPE3) || \
      (L3_DEVICE_TYPE == L3_TYPE7)
      
    case LvdResVoltage153 : 
      unLVD_CTLH.SVHR = 1;
      break; 
    case LvdResVoltage193 : 
      unLVD_CTLH.SVHR = 4;
      break; 
      
  #elif (L3_DEVICE_TYPE == L3_TYPE6)
      
    case LvdResVoltage150 : 
      unLVD_CTLH.SVHR = 0;
      break; 
    case LvdResVoltage155 : 
      unLVD_CTLH.SVHR = 1;
      break; 
    case LvdResVoltage160 : 
      unLVD_CTLH.SVHR = 2;
      break; 
    case LvdResVoltage165 : 
      unLVD_CTLH.SVHR = 3;
      break; 
    case LvdResVoltage170 : 
      unLVD_CTLH.SVHR = 4;
      break; 
    case LvdResVoltage175 : 
      unLVD_CTLH.SVHR = 5;
      break; 
    case LvdResVoltage180 : 
      unLVD_CTLH.SVHR = 6;
      break; 
    case LvdResVoltage185 : 
      unLVD_CTLH.SVHR = 7;
      break; 
    case LvdResVoltage190 : 
      unLVD_CTLH.SVHR = 8;
      break; 
    case LvdResVoltage195 : 
      unLVD_CTLH.SVHR = 9;
      break; 
    case LvdResVoltage200 : 
      unLVD_CTLH.SVHR = 10;
      break; 
    case LvdResVoltage205 : 
      unLVD_CTLH.SVHR = 11;
      break; 
    case LvdResVoltage250 : 
      unLVD_CTLH.SVHR = 12;
      break; 
    case LvdResVoltage260 : 
      unLVD_CTLH.SVHR = 13;
      break; 
    case LvdResVoltage270 : 
      unLVD_CTLH.SVHR = 14;
      break; 
  #else
    #error Device Type not supported!
  #endif
      
    default :
      return ErrorInvalidParameter;  
  }
#endif
  
  // Setup callback function pointer in internal data
  stcLvdInternData.pfnCallback = pstcConfig->pfnCallback ;
  
  // Init NVIC
  NVIC_ClearPendingIRQ(LVD_IRQn);
  NVIC_EnableIRQ(LVD_IRQn);
  NVIC_SetPriority(LVD_IRQn, L3_IRQ_LEVEL_LVD);
  
  
  
#if (L3_DEVICE_TYPE == L3_TYPE0) || \
    (L3_DEVICE_TYPE == L3_TYPE1) || \
    (L3_DEVICE_TYPE == L3_TYPE2) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5)  
  unLVD_CTLL.RESERVED2 = 1;         // LVD Configuration A needs 1 at bit#6
#endif
  
  LVD0->LVD_CLR = 0x00;             // Clear possible pending LVD interrupt
  LVD0->LVD_RLR = 0x1ACCE553 ;      // unlock LVD_CTL
  LVD0->u8LVD_CTLL = unLVD_CTLL ; 
  
#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  
  LVD0->u8LVD_CTLH = unLVD_CTLH ;
#endif
  
  LVD0->LVD_RLR = 0 ;               // lock LVD_CTL
  
  return Ok ;
} // Lvd_InitIrq

/**
 ******************************************************************************
 ** \brief De-Init the Interrupt of the LVD module
 ** 
 ** \retval Ok                    De-Initializiation of LVD interupt
 **                               successfully done.
 ******************************************************************************/
en_result_t Lvd_DeInitIrq(void)
{
  // De-Init NVIC
  NVIC_ClearPendingIRQ(LVD_IRQn);
  NVIC_DisableIRQ(LVD_IRQn);
  NVIC_SetPriority(LVD_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  
  LVD0->LVD_RLR = 0x1ACCE553 ;       // unlock LVD_CTL
  LVD0->LVD_CTLL = 0 ;               // Clea all (LVDIE (bit #7))
  LVD0->LVD_RLR = 0 ;                // lock LVD_CTL 
  
  return Ok ;
} // Lvd_DeInitIrq


//@} // LvdGroup

#endif // #if (defined(L3_PERIPHERAL_LVD_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
