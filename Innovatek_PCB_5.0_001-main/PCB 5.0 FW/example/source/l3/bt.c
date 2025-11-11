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
/** \file bt.c
 **
 ** A detailed description is available at 
 ** @link BtGroup Base Timer Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "bt.h"

#if (defined(L3_PERIPHERAL_BT_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup BtGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Callback pointer collection
 ******************************************************************************/
#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  func_ptr_t pfnCallback0Bt0;
  func_ptr_t pfnCallback1Bt0;
  func_ptr_t pfnCallback2Bt0;
#endif
#if (L3_PERIPHERAL_ENABLE_BT1 == L3_ON)
  func_ptr_t pfnCallback0Bt1;
  func_ptr_t pfnCallback1Bt1;
  func_ptr_t pfnCallback2Bt1;
#endif
#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  func_ptr_t pfnCallback0Bt2;
  func_ptr_t pfnCallback1Bt2;
  func_ptr_t pfnCallback2Bt2;
#endif
#if (L3_PERIPHERAL_ENABLE_BT3 == L3_ON)
  func_ptr_t pfnCallback0Bt3;
  func_ptr_t pfnCallback1Bt3;
  func_ptr_t pfnCallback2Bt3;
#endif
#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  func_ptr_t pfnCallback0Bt4;
  func_ptr_t pfnCallback1Bt4;
  func_ptr_t pfnCallback2Bt4;
#endif
#if (L3_PERIPHERAL_ENABLE_BT5 == L3_ON)
  func_ptr_t pfnCallback0Bt5;
  func_ptr_t pfnCallback1Bt5;
  func_ptr_t pfnCallback2Bt5;
#endif
#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  func_ptr_t pfnCallback0Bt6;
  func_ptr_t pfnCallback1Bt6;
  func_ptr_t pfnCallback2Bt6;
#endif
#if (L3_PERIPHERAL_ENABLE_BT7 == L3_ON)
  func_ptr_t pfnCallback0Bt7;
  func_ptr_t pfnCallback1Bt7;
  func_ptr_t pfnCallback2Bt7;
#endif

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
 ** \brief ISR of the BT module
 ******************************************************************************/
void BtIrqHandler(void)
{
#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  if ((FM3_BT0_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT0_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt0();
  }
  
  if ((FM3_BT0_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT0_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt0();
  }
  
  if ((FM3_BT0_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT0_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt0();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT1 == L3_ON)
  if ((FM3_BT1_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT1_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt1();
  }
  
  if ((FM3_BT1_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT1_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt1();
  }
  
  if ((FM3_BT1_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT1_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt1();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  if ((FM3_BT2_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT2_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt2();
  }
  
  if ((FM3_BT2_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT2_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt2();
  }
  
  if ((FM3_BT2_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT2_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt2();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT3 == L3_ON)
  if ((FM3_BT3_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT3_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt3();
  }
  
  if ((FM3_BT3_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT3_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt3();
  }
  
  if ((FM3_BT3_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT3_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt3();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  if ((FM3_BT4_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT4_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt4();
  }
  
  if ((FM3_BT4_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT4_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt4();
  }
  
  if ((FM3_BT4_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT4_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt4();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT5 == L3_ON)
  if ((FM3_BT5_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT5_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt5();
  }
  
  if ((FM3_BT5_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT5_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt5();
  }
  
  if ((FM3_BT5_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT5_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt5();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  if ((FM3_BT6_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT6_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt6();
  }
  
  if ((FM3_BT6_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT6_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt6();
  }
  
  if ((FM3_BT6_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT6_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt6();
  } 
#endif

#if (L3_PERIPHERAL_ENABLE_BT7 == L3_ON)
  if ((FM3_BT7_PWM->STC & BT_TGIE_TGIR_MASK) == BT_TGIE_TGIR_MASK) 
  {
    FM3_BT7_PWM->STC_f.TGIR = 0;   // Clear Trigger/Completion Irq 1
    pfnCallback0Bt7();
  }
  
  if ((FM3_BT7_PWM->STC & BT_UDIE_UDIR_MASK) == BT_UDIE_UDIR_MASK)
  {
    FM3_BT7_PWM->STC_f.UDIR = 0;   // Underflow/Overflow Irq 0
    pfnCallback1Bt7();
  }
  
  if ((FM3_BT7_PWM->STC & BT_DTIE_DTIR_MASK) == BT_DTIE_DTIR_MASK)
  {
    FM3_BT7_PWM->STC_f.DTIR = 0;   // Duty cycle match Irq 0
    pfnCallback2Bt7();
  } 
#endif
} // BtIrqHandler

/**
 ******************************************************************************
 ** \brief Enable BT interrupts
 ** 
 ** Enables the BT0-7 interrupts. If already enabled by a previous called
 ** Bt_Init(), the function has no effect.
 ******************************************************************************/
static void Bt_InitIrq(void)
{
  uint32_t u32BtIrqMask = (uint32_t)((uint32_t)1 << ((uint32_t)(BTIM_IRQn & 0x1F)));
                           
  if ((NVIC->ISER[((uint32_t)(BTIM_IRQn) >> 5)] & u32BtIrqMask) != u32BtIrqMask)
  {
    NVIC_ClearPendingIRQ(BTIM_IRQn);
    NVIC_EnableIRQ(BTIM_IRQn);
    NVIC_SetPriority(BTIM_IRQn, L3_IRQ_LEVEL_BT);
  }
} // Bt_Enable_Interrupts

/**
 ******************************************************************************
 ** \brief Disable BT interrupts
 ** 
 ** Disables the BT interrupts.
 ******************************************************************************/
static void Bt_DeInitIrq(void)
{
  NVIC_ClearPendingIRQ(BTIM_IRQn);
  NVIC_DisableIRQ(BTIM_IRQn);
  NVIC_SetPriority(BTIM_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
} // Bt_Disable_Interrupts

/**
 ******************************************************************************
 ** \brief Init a BT instance
 ** 
 ** Initializes a BT instance according to its configuration. It does not
 ** enable the operation. Use Bt_Enable() for enabling a timer after Bt_Init().
 **
 ** \param [in]  pstcBt      BT instance
 ** \param [in]  pstcConfig  Pointer to BT configuration structure
 **
 ** \retval Ok                    Initializiation of BT successfully done.
 ** \retval ErrorInvalidParameter pstcBt == NULL,
 **                               pstcConfig == NULL,
 **                               32 bit mode set to odd numbered BT instance,
 **                               other invalid configuration settings
 ******************************************************************************/
en_result_t Bt_Init( stc_btn_t* pstcBt, 
                     stc_bt_config_t* pstcConfig)
{
  stc_btn_tmcr_field_t   unTMCR  = { 0 } ;  // local TMCR predefinition
  stc_btn_tmcr2_field_t  unTMCR2 = { 0 } ;  // local TMCR2 predefinition
  stc_btn_stc_field_t    unSTC   = { 0 } ;  // local TMCR2 predefinition
  
  // Check for NULL Pointers
  if ((pstcBt == NULL) || (pstcConfig == NULL))
  {
    return ErrorInvalidParameter ;
  } 
  
  // First of all set mode
  switch (pstcConfig->enMode)
  {
    case BtPwm :
      unTMCR.FMD = 1 ;
      pstcBt->u16TMCR.FMD = 1 ;
      break;
    case BtPpg :
      unTMCR.FMD = 2 ;
      pstcBt->u16TMCR.FMD = 2 ;
      break;   
    case BtRlt :
      unTMCR.FMD = 3 ;
      pstcBt->u16TMCR.FMD = 3 ;
      break;    
    case BtPwc :
      unTMCR.FMD = 4 ;
      pstcBt->u16TMCR.FMD = 4 ;
      break;   
    default:
      return ErrorInvalidParameter ;
  }
  
  // Set prescaler
  switch (pstcConfig->enPrescaler)
  {
    case BtExtClkRisingEdge :
      if (pstcConfig->enMode != BtPwc)
      {
        unTMCR.CKS   = 5 ;
        unTMCR2.CKS3 = 0 ;
      }
      else
      {
        return ErrorInvalidParameter ;  // External clock not allowed in PWC mode
      }
      break;
    case BtExtClkFallingEdge :
      if (pstcConfig->enMode != BtPwc)
      {
        unTMCR.CKS   = 6 ;
        unTMCR2.CKS3 = 0 ;
      }
      else
      {
        return ErrorInvalidParameter ;  // External clock not allowed in PWC mode
      }
      break;
    case BtExtClkBothEdges :
      if (pstcConfig->enMode != BtPwc)
      {
        unTMCR.CKS   = 7 ;
        unTMCR2.CKS3 = 0 ;
      }
      else
      {
        return ErrorInvalidParameter ;  // External clock not allowed in PWC mode
      }
      break;
    case BtIntClkDiv1 : 
      unTMCR.CKS   = 0 ;
      unTMCR2.CKS3 = 0 ;
      break;
    case BtIntClkDiv4 : 
      unTMCR.CKS   = 1 ;
      unTMCR2.CKS3 = 0 ;
      break;
    case BtIntClkDiv16 : 
      unTMCR.CKS   = 2 ;
      unTMCR2.CKS3 = 0 ;
      break;
    case BtIntClkDiv128 : 
      unTMCR.CKS   = 3 ;
      unTMCR2.CKS3 = 0 ;
      break;      
    case BtIntClkDiv256 : 
      unTMCR.CKS   = 4 ;
      unTMCR2.CKS3 = 0 ;
      break;
    case BtIntClkDiv512 : 
      unTMCR.CKS   = 0 ;
      unTMCR2.CKS3 = 1 ;
      break;
    case BtIntClkDiv1024 : 
      unTMCR.CKS   = 1 ;
      unTMCR2.CKS3 = 1 ;
      break;
    case BtIntClkDiv2048 : 
      unTMCR.CKS   = 2 ;
      unTMCR2.CKS3 = 1 ;
      break; 
    default :
      return ErrorInvalidParameter ;
  }
  
  // Restart trigger
  unTMCR.RTGEN = (pstcConfig->bRestart == TRUE ) ? 1 : 0 ;
  
  // Trigger
  if (pstcConfig->enMode != BtPwc)
  {
    // Output forced to low (only available for PWM, PPG, RLT)
    unTMCR.PMSK_EGS2 = (pstcConfig->bOutputLow == TRUE ) ? 1 : 0 ;
    
    switch (pstcConfig->enExtTriggerMode)
    {
     case BtTriggerDisable :
       unTMCR.EGS = 0;
       break;
     case BtTriggerRisingEdge :
       unTMCR.EGS = 1;
       break;
     case BtTriggerFallingEdge :
       unTMCR.EGS = 2;
       break;
     case BtTriggerBothEdges :
       unTMCR.EGS = 3;
       break;
     default:
       return ErrorInvalidParameter ;
    }
  }
  else // if (pstcConfig->enMode != BtPwc)
  {
    switch (pstcConfig->enExtTriggerMode)
    {
      case BtMeasureHighPulse :
        unTMCR.EGS       = 0;
        unTMCR.PMSK_EGS2 = 0;
        break;
      case BtMeasureLowPulse :
        unTMCR.EGS       = 0;
        unTMCR.PMSK_EGS2 = 1;
        break;
      case BtMeasureRisingEdges :
        unTMCR.EGS       = 1;
        unTMCR.PMSK_EGS2 = 0;
        break;
      case BtMeasureFallingEdges :
        unTMCR.EGS       = 2;
        unTMCR.PMSK_EGS2 = 0;
        break;  
      case BtMeasureAllEdges :
        unTMCR.EGS       = 3;
        unTMCR.PMSK_EGS2 = 0;
        break;
      default:
        return ErrorInvalidParameter ;
    }
  }
  
  // Timer size (16/32 bit), applicable only for RLT and PWC mode and for
  //   even numbered BT instances.
  if ((pstcConfig->enMode == BtRlt) || (pstcConfig->enMode == BtPwc))
  {
    if ((pstcConfig->bTimer32 == TRUE) &&
        ((pstcBt == &BT0) ||
         (pstcBt == &BT2) ||
         (pstcBt == &BT4) ||
         (pstcBt == &BT6)
        )
       )
    {
      unTMCR.T32 = (pstcConfig->bTimer32 == TRUE) ? 1 : 0 ;
    }
    else
    {
       return ErrorInvalidParameter ; // Not available for odd numbered BT instance!
    }
  }
  
  // Output polarity (except for PWC):
  unTMCR.OSEL = ((pstcConfig->bOutputInvert == TRUE) && (pstcConfig->enMode != BtPwc)) ? 1 : 0 ;
  
  // One-shot mode
  unTMCR.MDSE = (pstcConfig->bOneShot == TRUE) ? 1 : 0 ;

  // Trigger/Completion interrupt enable
  if (pstcConfig->bTriggerIrqEnable == TRUE)
  {
    if (pstcConfig->pfnCallback0 != NULL)
    {
      unSTC.TGIE_EDIE = 1;
    }
    else
    {
      return ErrorInvalidParameter ; // Irq enable with NULL pointer to callback not allowed!
    }
  }
  
  // Underflow/Overflow interrupt enable
  if (pstcConfig->bUnderflowIrqEnable == TRUE)
  {
    if (pstcConfig->pfnCallback1 != NULL)
    {
      unSTC.UDIE_OVIE = 1;
    }
    else
    {
      return ErrorInvalidParameter ; // Irq enable with NULL pointer to callback not allowed!
    }
  }
  
  // Duty cycle (if PWM mode)
  if (pstcConfig->enMode == BtPwm)
  {
    if (pstcConfig->bDutyIrqEnable == TRUE)
    {
      if (pstcConfig->pfnCallback2 != NULL)
      {
        unSTC.DTIE = 1;
      }
      else
      {
        return ErrorInvalidParameter ; // Irq enable with NULL pointer to callback not allowed!
      }
    }
  }
  
  // Now setup hardware ...
  pstcBt->u16TMCR = unTMCR ;
  pstcBt->u8TMCR2 = unTMCR2 ;
  pstcBt->u8STC   = unSTC ;
  
  // Timer/Conter registers:
  
  // 32-bit mode for RLT?
  if ((pstcConfig->enMode == BtRlt) && (pstcConfig->bTimer32 == TRUE))
  {
    if (pstcBt == &BT0)
    {
      // Set "upper" 16 bit of next BT instance first
      FM3_BT1_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle >> 16); 
      
      // Set "lower" 16 bit of recent BT instance
      FM3_BT0_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle & 0x0000FFFF);
    }
    else if (pstcBt == &BT2)
    {
      // Set "upper" 16 bit of next BT instance first
      FM3_BT3_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle >> 16); 
      
      // Set "lower" 16 bit of recent BT instance
      FM3_BT2_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle & 0x0000FFFF);
    }
    else if (pstcBt == &BT4)
    {
      // Set "upper" 16 bit of next BT instance first
      FM3_BT5_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle >> 16); 
      
      // Set "lower" 16 bit of recent BT instance
      FM3_BT4_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle & 0x0000FFFF);
    }
    else if (pstcBt == &BT6)
    {
      // Set "upper" 16 bit of next BT instance first
      FM3_BT7_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle >> 16); 
      
      // Set "lower" 16 bit of recent BT instance
      FM3_BT6_RT->PCSR = (uint16_t)(pstcConfig->u32Cycle & 0x0000FFFF);
    }
    else
    {
      return ErrorInvalidParameter ;
    }
  }
  else if (pstcConfig->enMode != BtPwc)
  {
    pstcBt->PCSR_PRLL      = pstcConfig->u16Cycle ;
    pstcBt->PDUT_PRLH_DTBF = pstcConfig->u16Duty ;
  }
  
  // Setup internal callback data
#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  if (pstcBt == &BT0)
  {
    pfnCallback0Bt0 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt0 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt0 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT1 == L3_ON)
  if (pstcBt == &BT1)
  {
    pfnCallback0Bt1 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt1 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt1 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  if (pstcBt == &BT2)
  {
    pfnCallback0Bt2 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt2 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt2 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT3 == L3_ON)
  if (pstcBt == &BT3)
  {
    pfnCallback0Bt3 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt3 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt3 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  if (pstcBt == &BT4)
  {
    pfnCallback0Bt4 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt4 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt4 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT5 == L3_ON)
  if (pstcBt == &BT5)
  {
    pfnCallback0Bt5 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt5 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt5 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  if (pstcBt == &BT6)
  {
    pfnCallback0Bt6 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt6 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt6 = pstcConfig->pfnCallback2 ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT7 == L3_ON)
  if (pstcBt == &BT7)
  {
    pfnCallback0Bt7 = pstcConfig->pfnCallback0 ;
    pfnCallback1Bt7 = pstcConfig->pfnCallback1 ;
    pfnCallback2Bt7 = pstcConfig->pfnCallback2 ;
  }
#endif
  
  // Finally enable Irq globaly
  Bt_InitIrq();
  
  return Ok;
  
} // Bt_Init

/**
 ******************************************************************************
 ** \brief Enables the operation of a BT instance
 **
 ** \pre Bt_Init() has to be called before.
 ** 
 ** \param [in]  pstcBt      BT instance
 **
 ** \retval Ok               Trigger done
 ******************************************************************************/
en_result_t Bt_Enable( stc_btn_t* pstcBt )
{
  pstcBt->u16TMCR.CTEN = 1 ;  // Enable counting
  
  return Ok;
} // Bt_Enable

/**
 ******************************************************************************
 ** \brief Disables the operation of a BT instance
 **
 ** \param [in]  pstcBt      BT instance
 **
 ** \retval Ok               Trigger done
 ******************************************************************************/
en_result_t Bt_Disable( stc_btn_t* pstcBt )
{
  pstcBt->u16TMCR.CTEN = 0 ;  // Disable counting
  
  return Ok;
} // Bt_Disable


/**
 ******************************************************************************
 ** \brief Trigger a BT instance
 ** 
 ** Triggers a BT instance by software. This function has no effect, if BT is
 ** configured to PWC mode.
 **
 ** \param [in]  pstcBt      BT instance
 **
 ** \retval Ok               Trigger done
 ******************************************************************************/
en_result_t Bt_Trigger(stc_btn_t* pstcBt)
{
  pstcBt->u16TMCR.STRG = 1;  // Trigger it!
  
  return Ok;
} // Bt_Trigger

/**
 ******************************************************************************
 ** \brief De-Init a BT instance
 **
 ** This function de-inits a BT instance. Because all Base Timers share one
 ** IRQ this interrupt can be disbaled by parameter. This parameter should
 ** be set to TRUE, when the last BT instance is de-initialized.
 **
 ** \param [in]  pstcBt                BT instance
 ** \param [in]  bDisableBtInterrupts  Disable the BT0-7 shared interupt
 **
 ** \retval Ok                         De-init sucessfully done
 ******************************************************************************/
en_result_t Bt_DeInit( stc_btn_t* pstcBt, boolean_t bDisableBtInterrupts)
{
  if (TRUE == bDisableBtInterrupts)
  {
    Bt_DeInitIrq() ;
  }
  
  pstcBt->TMCR           = 0 ;
  pstcBt->TMCR2          = 0 ;
  pstcBt->STC            = 0 ;
  pstcBt->PCSR_PRLL      = 0 ;
  pstcBt->PDUT_PRLH_DTBF = 0 ;  

  // Clear internal callback data
#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  if (pstcBt == &BT0)
  {
    pfnCallback0Bt0 = NULL ;
    pfnCallback1Bt0 = NULL ;
    pfnCallback2Bt0 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT1 == L3_ON)
  if (pstcBt == &BT1)
  {
    pfnCallback0Bt1 = NULL ;
    pfnCallback1Bt1 = NULL ;
    pfnCallback2Bt1 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  if (pstcBt == &BT2)
  {
    pfnCallback0Bt2 = NULL ;
    pfnCallback1Bt2 = NULL ;
    pfnCallback2Bt2 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT3 == L3_ON)
  if (pstcBt == &BT3)
  {
    pfnCallback0Bt3 = NULL ;
    pfnCallback1Bt3 = NULL ;
    pfnCallback2Bt3 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  if (pstcBt == &BT4)
  {
    pfnCallback0Bt4 = NULL ;
    pfnCallback1Bt4 = NULL ;
    pfnCallback2Bt4 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT5 == L3_ON)
  if (pstcBt == &BT5)
  {
    pfnCallback0Bt5 = NULL ;
    pfnCallback1Bt5 = NULL ;
    pfnCallback2Bt5 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  if (pstcBt == &BT6)
  {
    pfnCallback0Bt6 = NULL ;
    pfnCallback1Bt6 = NULL ;
    pfnCallback2Bt6 = NULL ;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_BT7 == L3_ON)
  if (pstcBt == &BT7)
  {
    pfnCallback0Bt7 = NULL ;
    pfnCallback1Bt7 = NULL ;
    pfnCallback2Bt7 = NULL ;
  }
#endif
  
  return Ok;
} // Bt_DeInit

/**
 ******************************************************************************
 ** \brief Reset BT I/O Selector
 **
 ** This function resets the Base Timers I/O Selectors
 **
 ** \retval Ok                   Reset sucessfully done
 ******************************************************************************/
en_result_t Bt_ResetIoSel(void)
{
  FM3_BTIOSEL03->BTSEL0123 = 0;
  FM3_BTIOSEL47->BTSEL4567 = 0;
  
  return Ok ;
} // Bt_Reset_IoSel

/**
 ******************************************************************************
 ** \brief Init a BT I/O Selector
 **
 ** This function initializes a Base Timer I/O Selector.
 **
 ** \param [in]  pstcConfig  Pointer to BT IOSEL configuration structure
 **
 ** \retval Ok                    Init sucessfully done
 ** \retval ErrorInvalidParameter pstcConfig == NULL,
 **                               other invalid configuration settings
 ******************************************************************************/
en_result_t Bt_InitIoSel(stc_bt_iosel_config_t* pstcConfig)
{
  uint8_t u8BitPattern ;  // used for mode bit patterns
  
  // Check for NULL Pointer
  if (pstcConfig == NULL)
  {
      return ErrorInvalidParameter ;
  } 
  
  // Prepare bit pattern
  switch (pstcConfig->enMode)
  {
    case BtMode0 :
      u8BitPattern = 0 ;
      break ;
    case BtMode1 :
      u8BitPattern = 1 ;
      break ;
    case BtMode2 :
      u8BitPattern = 2 ;
      break ;
    case BtMode3 :
      u8BitPattern = 3 ;
      break ;
    case BtMode4 :
      u8BitPattern = 4 ;
      break ;
    case BtMode5 :
      u8BitPattern = 5 ;
      break ;
    case BtMode6 :
      u8BitPattern = 6 ;
      break ;
    case BtMode7 :
      u8BitPattern = 7 ;
      break ;
    case BtMode8 :
      u8BitPattern = 8 ;
      break ;  
    default:
      return ErrorInvalidParameter ;
  }
  
  // Set Selector
  switch (pstcConfig->enBtInstance)
  {
    case BtSel01 :
      FM3_BTIOSEL03->BTSEL0123 &= 0xF0 ;
      FM3_BTIOSEL03->BTSEL0123  = u8BitPattern ; 
      break;
    case BtSel23 :
      FM3_BTIOSEL03->BTSEL0123 &= 0x0F ;
      FM3_BTIOSEL03->BTSEL0123  = (u8BitPattern << 4) ; 
      break;
    case BtSel45 :
      FM3_BTIOSEL47->BTSEL4567 &= 0xF0 ;
      FM3_BTIOSEL47->BTSEL4567  = u8BitPattern ; 
      break;
    case BtSel67 :
      FM3_BTIOSEL47->BTSEL4567 &= 0x0F ;
      FM3_BTIOSEL47->BTSEL4567  = (u8BitPattern << 4) ; 
      break;
    default:
      return ErrorInvalidParameter ;
  }
  
  return Ok;
} // Bt_Init_Iosel

/**
 ******************************************************************************
 ** \brief De-Init a BT I/O Selector
 **
 ** This function de-initializes a Base Timer I/O Selector (resets to mode 0).
 **
 ** \param [in]  pstcConfig  Pointer to BT IOSEL configuration structure
 **
 ** \retval Ok                    De-Init sucessfully done
 ** \retval ErrorInvalidParameter pstcConfig == NULL,
 **                               other invalid configuration settings
 ******************************************************************************/
en_result_t Bt_DeInitIoSel(stc_bt_iosel_config_t* pstcConfig)
{
  // Check for NULL Pointer
  if (pstcConfig == NULL)
  {
      return ErrorInvalidParameter ;
  } 

  // Reset Selector
  switch (pstcConfig->enBtInstance)
  {
    case BtSel01 :
      FM3_BTIOSEL03->BTSEL0123 &= 0xF0 ;
      break;
    case BtSel23 :
      FM3_BTIOSEL03->BTSEL0123 &= 0x0F ;
      break;
    case BtSel45 :
      FM3_BTIOSEL47->BTSEL4567 &= 0xF0 ;
      break;
    case BtSel67 :
      FM3_BTIOSEL47->BTSEL4567 &= 0x0F ;
      break;
    default:
      return ErrorInvalidParameter ;
  } 
  
  return Ok;
} // Bt_DeInit_Iosel

/**
 ******************************************************************************
 ** \brief Software-Trigger a BT instance
 **
 ** This function triggers a BT instance by the software-based simultaneous
 ** startup register.
 **
 ** \note Do not use this function unless the selected BT instances are set to
 **       either of the following modes:
 **       - I/O mode 5 (Software-based simultaneous startup mode)
 **       - I/O mode 6 (Software-based startup and timer start/stop mode)
 **         (Even channels only)
 **
 ** \pre For the BT instance started up by using this method, select the rising
 **      edge as a trigger input edge using
 **      en_bt_trigger_input_t#BtTriggerRisingEdge configuration.
 **
 ** \param [in]  u16TriggerBtInstance  Bit pattern of BTSSSR_BTn_BIT definitions
 **                                    in bt.h
 **
 ** \retval Ok    Trigger sucessfully done
 ******************************************************************************/
en_result_t Bt_TriggerInstances(uint16_t u16TriggerBtInstance)
{
  FM3_SBSSR->BTSSSR = u16TriggerBtInstance;
    
  return Ok; 
} // Bt_TriggerInstances

/**
 ******************************************************************************
 ** \brief Read Timer value 16-bit
 **
 ** This function reads the recent 16-bit timer value.
 **
 ** \param [in]  pstcBt     Pointer to BT instance
 **
 ** \retval  uint16_t        Counter value
 ******************************************************************************/
uint16_t Bt_ReadTimer16(stc_btn_t* pstcBt)
{
  return pstcBt->TMR ; 
} // Bt_Read_Timer16

/**
 ******************************************************************************
 ** \brief Read Timer value 32-bit
 **
 ** This function reads the recent 32-bit timer value. Use this function only
 ** when RLT is set to 32-bit mode!
 **
 ** \param [in]  pstcBt     Pointer to BT instance
 **
 ** \retval  uint32_t        Counter value, 0 = Error
 ******************************************************************************/
uint32_t Bt_ReadTimer32(stc_btn_t* pstcBt)
{
  uint32_t u32CounterValue;

#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  if (pstcBt == &BT0)
  {
    u32CounterValue = FM3_BT1_RT->TMR << 16;
    return u32CounterValue | FM3_BT0_RT->TMR;
  }
#endif
#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  if (pstcBt == &BT2)
  {
    u32CounterValue = FM3_BT3_RT->TMR << 16;
    return u32CounterValue | FM3_BT2_RT->TMR;
  }
#endif
#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  if (pstcBt == &BT4)
  {
    u32CounterValue = FM3_BT5_RT->TMR << 16;
    return u32CounterValue | FM3_BT4_RT->TMR;
  }
#endif
#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  if (pstcBt == &BT6)
  {
    u32CounterValue = FM3_BT7_RT->TMR << 16;
    return u32CounterValue | FM3_BT6_RT->TMR;
  }
#endif  
  
  return 0 ; // Should never be executed on runtime!
} // Bt_Read_Timer32

/**
 ******************************************************************************
 ** \brief Check PWC error state
 **
 ** This function reads the ERR bit of the STC register. It assumes that the
 ** BT is set to PWC mode. Otherwise the return value is regardless.
 ** This bit indicates that the next measurement has been completed in
 ** continuous measurement mode before the measurement result is read from the
 ** DTBF register. In this case the result of the previous
 ** measurement in the DTBF register is replaced by that of the next
 ** measurement. The measurement is continued regardless of the ERR bit value.
 ** The ERR bit is cleared by reading the measurement result (DTBF).
 **
 ** \param [in]  pstcBt     Pointer to BT instance
 **
 ** \retval Ok     ERR bit not set
 ** \retval Error  ERR bit set
 ******************************************************************************/
en_result_t Bt_CheckPwcError(stc_btn_t* pstcBt)
{
  if (pstcBt->u8STC.ERR == 1)
  {
    return Error;
  }
  
  return Ok;
} // Bt_CheckPwcError

/**
 ******************************************************************************
 ** \brief Read Data Buffer value 16-bit
 **
 ** This function read the 16-bit PWC result counter.
 **
 ** \note This function (or Bt_ReadDataBuffer32()) must be called in the 
 **       callback function to clear the interrupt request!
 **
 ** \param [in]  pstcBt     Pointer to BT instance
 **
 ** \retval  uint16_t        Counter value
 ******************************************************************************/
uint16_t Bt_ReadDataBuffer16(stc_btn_t* pstcBt)
{
  return pstcBt->PDUT_PRLH_DTBF ; 
} // Bt_ReadDataBuffer16

/**
 ******************************************************************************
 ** \brief Read Data Buffer value 32-bit
 **
 ** This function read the 32-bit PWC result counter. Use this function only
 ** when PWC is set to 32-bit mode!
 **
 ** \note This function (or Bt_ReadDataBuffer16()) must be called in the 
 **       callback function to clear the interrupt request!
 **
 ** \param [in]  pstcBt     Pointer to BT instance
 **
 ** \retval  uint32_t        Counter value
 ******************************************************************************/
uint32_t Bt_ReadDataBuffer32(stc_btn_t* pstcBt)
{
  uint32_t u32CounterValue;

#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON)
  if (pstcBt == &BT0)
  {
    u32CounterValue = FM3_BT1_PWC->DTBF << 16;
    return u32CounterValue | FM3_BT0_PWC->DTBF;
  }
#endif
#if (L3_PERIPHERAL_ENABLE_BT2 == L3_ON)
  if (pstcBt == &BT2)
  {
    u32CounterValue = FM3_BT3_PWC->DTBF << 16;
    return u32CounterValue | FM3_BT2_PWC->DTBF;
  }
#endif
#if (L3_PERIPHERAL_ENABLE_BT4 == L3_ON)
  if (pstcBt == &BT4)
  {
    u32CounterValue = FM3_BT5_PWC->DTBF << 16;
    return u32CounterValue | FM3_BT4_PWC->DTBF;
  }
#endif
#if (L3_PERIPHERAL_ENABLE_BT6 == L3_ON)
  if (pstcBt == &BT6)
  {
    u32CounterValue = FM3_BT7_PWC->DTBF << 16;
    return u32CounterValue | FM3_BT6_PWC->DTBF;
  }
#endif 
  
  return 0 ; // Should never be executed on runtime!
} // Bt_ReadDataBuffer32


//@} // BtGroup

#endif // #if (defined(L3_PERIPHERAL_BT_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
