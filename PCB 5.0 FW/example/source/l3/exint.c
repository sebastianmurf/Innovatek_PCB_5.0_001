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
/** \file exint.c
 **
 ** A detailed description is available at 
 ** @link AdcGroup EXINT Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "exint.h"
#include "myusb.h"	//nabeen

#if (defined(L3_PERIPHERAL_EXINT_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup ExintGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/


/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
stc_exint_intern_data_t stcExintInternData;
stc_exint_nmi_intern_data_t stcExintNMIInternData;

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
 ** \brief Interrupt handler for External interrupts 0 - 7
 **
 ** This function clears the cause bit(s) and calls the callback function, if
 ** specified.
 ******************************************************************************/
#if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)  
void ExintIrqHandler0_7(void)
{
  if ((FM3_EXTI->EIRR & EXINT0_BITPOS) && (FM3_EXTI->ENIR & EXINT0_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT0_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[0] != NULL)
    {
      stcExintInternData.apfnExintCallback[0](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT1_BITPOS) && (FM3_EXTI->ENIR & EXINT1_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT1_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[1] != NULL)
    {
      stcExintInternData.apfnExintCallback[1](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT2_BITPOS) && (FM3_EXTI->ENIR & EXINT2_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT2_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[2] != NULL)
    {	PutStringUsb("\n\rISR2 found");
      stcExintInternData.apfnExintCallback[2](); 
    }
	else
	  PutStringUsb("\n\rNo ISR2 found");
  }
  
  if ((FM3_EXTI->EIRR & EXINT3_BITPOS) && (FM3_EXTI->ENIR & EXINT3_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT3_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[3] != NULL)
    {
      stcExintInternData.apfnExintCallback[3](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT4_BITPOS) && (FM3_EXTI->ENIR & EXINT4_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT4_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[4] != NULL)
    {
      stcExintInternData.apfnExintCallback[4](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT5_BITPOS) && (FM3_EXTI->ENIR & EXINT5_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT5_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[5] != NULL)
    {
      stcExintInternData.apfnExintCallback[5](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT6_BITPOS) && (FM3_EXTI->ENIR & EXINT6_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT6_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[6] != NULL)
    {
      stcExintInternData.apfnExintCallback[6](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT7_BITPOS) && (FM3_EXTI->ENIR & EXINT7_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT7_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[7] != NULL)
    {
      stcExintInternData.apfnExintCallback[7](); 
    }
  }
} // ExintIrqHandler0_7
#endif

/**
 ******************************************************************************
 ** \brief Interrupt handler for External interrupts 8 - 15
 **
 ** This function clears the cause bit(s) and calls the callback function, if
 ** specified.
 ******************************************************************************/
#if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)  
void ExintIrqHandler8_15(void)
{
  if ((FM3_EXTI->EIRR & EXINT8_BITPOS) && (FM3_EXTI->ENIR & EXINT8_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT8_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[8] != NULL)
    {
      stcExintInternData.apfnExintCallback[8](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT9_BITPOS) && (FM3_EXTI->ENIR & EXINT9_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT9_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[9] != NULL)
    {
      stcExintInternData.apfnExintCallback[9](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT10_BITPOS) && (FM3_EXTI->ENIR & EXINT10_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT10_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[10] != NULL)
    {
      stcExintInternData.apfnExintCallback[10](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT11_BITPOS) && (FM3_EXTI->ENIR & EXINT11_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT11_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[11] != NULL)
    {
      stcExintInternData.apfnExintCallback[11](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT12_BITPOS) && (FM3_EXTI->ENIR & EXINT12_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT12_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[12] != NULL)
    {
      stcExintInternData.apfnExintCallback[12](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT13_BITPOS) && (FM3_EXTI->ENIR & EXINT13_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT13_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[13] != NULL)
    {
      stcExintInternData.apfnExintCallback[13](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT14_BITPOS) && (FM3_EXTI->ENIR & EXINT14_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT14_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[14] != NULL)
    {
      stcExintInternData.apfnExintCallback[14](); 
    }
  }
  
  if ((FM3_EXTI->EIRR & EXINT15_BITPOS) && (FM3_EXTI->ENIR & EXINT15_BITPOS))
  {
    FM3_EXTI->EICL &= EXINT15_BITMASK;
    
    if (stcExintInternData.apfnExintCallback[15] != NULL)
    {
      stcExintInternData.apfnExintCallback[15](); 
    }
  }  
} // ExintIrqHandler8_15
#endif

/**
 ******************************************************************************
 ** \brief Interrupt handler for None-Maskable Interrupt (NMI)
 **
 ** This function clears the cause bit and calls the NMI callback function, if
 ** specified.
 ******************************************************************************/
#if ((L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON) || (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON))
void ExintNMIrqHandler(void)
{
  if ((FM3_EXTI->NMIRR & 0x01) == 0x01)
  {
    FM3_EXTI->NMICL = 0;
    stcExintNMIInternData.pfnNMICallback();
  }
}
#endif

/**
 ******************************************************************************
 ** \brief Init External Interrupts
 **
 ** This function initializes the external interrupts according the given
 ** configuration.
 **
 ** \param [in]  pstcConfig        EXINT configuration parameters
 ** 
 ** \retval Ok                     EXINT initialized
 ** \retval ErrorInvalidParameter  pstcConfig == NULL or Illegal mode
 ******************************************************************************/
en_result_t Exint_Init(stc_exint_config_t* pstcConfig)
{
  uint16_t u16ENIR;
  uint32_t u32ELVR;
  uint8_t  u8Count;
  
  if ( pstcConfig == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  u16ENIR = 0;
  u32ELVR = 0;
  
  // First disable all external interrupts
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)  
    NVIC_DisableIRQ(EXINT0_7_IRQn);
  #endif
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
    NVIC_DisableIRQ(EXINT8_15_IRQn);
  #endif
  
  // Configure external interrupts
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)
    for (u8Count = 0; u8Count < 8; u8Count++)
    {
      if (pstcConfig->abEnable[u8Count] == TRUE)
      {
        u16ENIR |= 1 << u8Count;
      }
      stcExintInternData.apfnExintCallback[u8Count] = pstcConfig->apfnExintCallback[u8Count];
    }
  #endif
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
    for (u8Count = 8; u8Count < 16; u8Count++)
    {
      if (pstcConfig->abEnable[u8Count] == TRUE)
      {
        u16ENIR |= 1 << u8Count;
      }
      stcExintInternData.apfnExintCallback[u8Count] = pstcConfig->apfnExintCallback[u8Count];
    }
  #endif
    
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)
    for (u8Count = 0; u8Count < 8; u8Count++)
    {
      u32ELVR |= pstcConfig->aenLevel[u8Count] << (u8Count * 2);
    }
  #endif  
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
    for (u8Count = 8; u8Count < 16; u8Count++)
    {
      u32ELVR |= pstcConfig->aenLevel[u8Count] << (u8Count * 2);
    }
  #endif   
    
  // Setup hardware
  FM3_EXTI->ELVR = u32ELVR;
  FM3_EXTI->EICL = 0;         // Clear all possible interrupts
  FM3_EXTI->ENIR = u16ENIR;
  
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)
    NVIC_SetPriority(EXINT0_7_IRQn, L3_IRQ_LEVEL_EXINT0_7);
    NVIC_ClearPendingIRQ(EXINT0_7_IRQn);
    NVIC_EnableIRQ(EXINT0_7_IRQn);
  #endif
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
    NVIC_SetPriority(EXINT8_15_IRQn, L3_IRQ_LEVEL_EXINT8_15);
    NVIC_EnableIRQ(EXINT8_15_IRQn);
  #endif  

  return Ok;
} // Exint_Init

/**
 ******************************************************************************
 ** \brief Init External Interrupts
 **
 ** This function de-initializes all external interrupts.
 ** 
 ** \retval Ok                     EXINT sucessful disabled
 ******************************************************************************/
en_result_t Exint_DeInit(void)
{
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)  
    NVIC_DisableIRQ(EXINT0_7_IRQn);
  #endif
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
    NVIC_DisableIRQ(EXINT8_15_IRQn);
  #endif
    
  FM3_EXTI->EICL = 0xFFFF;    // Clear all possible interrupts
  FM3_EXTI->ENIR = 0;         // Disable all external interupts
    
  return Ok;  
} // Exint_DeInit

/**
 ******************************************************************************
 ** \brief Enable a single External Interrupt
 **
 ** This function enables a single External Interrupt.
 **
 ** \note This function does not set
 **       stc_exint_config_t#abEnable[u8Channel]!
 **
 ** \param [in]  u8Channel         External Interrupt channel number
 ** 
 ** \retval Ok                     Channel enabled
 ** \retval ErrorInvalidParameter  Invalid channel number
 ******************************************************************************/
en_result_t Exint_EnableChannel(uint8_t u8Channel)
{
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_OFF) 
  if (u8Channel < 8)
    return ErrorInvalidParameter;
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_OFF) 
  if (u8Channel > 7)
    return ErrorInvalidParameter;
  #endif
  
  FM3_EXTI->ENIR |= 1 << u8Channel;
  
  return Ok;
} // Exint_EnableChannel

/**
 ******************************************************************************
 ** \brief Disable a single External Interrupt
 **
 ** This function disables a single External Interrupt.
 **
 ** \note This function does not clear
 **       stc_exint_config_t#abEnable[u8Channel]!
 **
 ** \param [in]  u8Channel         External Interrupt channel number
 ** 
 ** \retval Ok                     Channel enabled
 ** \retval ErrorInvalidParameter  Invalid channel number
 ******************************************************************************/
en_result_t Exint_DisableChannel(uint8_t u8Channel)
{
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_OFF) 
  if (u8Channel < 8)
    return ErrorInvalidParameter;
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_OFF) 
  if (u8Channel > 7)
    return ErrorInvalidParameter;
  #endif
  
  FM3_EXTI->ENIR &= (0xFFFF ^ (1 << u8Channel));
  
  return Ok;
} // Exint_DisableChannel

/**
 ******************************************************************************
 ** \brief Init Non-Maskable Interrupt (NMI)
 **
 ** This function initializes the NMI according to the given
 ** configuration.
 **
 ** \param [in]  pstcConfig        NMI configuration parameters
 ** 
 ** \retval Ok                     NMI initialized
 ** \retval ErrorInvalidParameter  pstcConfig == NULL
 **
 ** \note NMI shares the interrupt vector with the Hardware Watchog. To avoid
 **       cross influence of initialization of the NVIC use
 **       stc_exint_nmi_config_t#bTouchNVIC to determine whether to touch the
 **       corresponding NVIC registers or not.
 ******************************************************************************/
en_result_t Exint_Nmi_Init(stc_exint_nmi_config_t* pstcConfig)
{
  if ( pstcConfig == NULL )
  {
    return ErrorInvalidParameter ;
  }

  // Set internal data
  stcExintNMIInternData.pfnNMICallback = pstcConfig->pfnNMICallback;
  
  if (pstcConfig->bTouchNVIC == TRUE)
  {
    NVIC_ClearPendingIRQ(NMI_IRQn);
    NVIC_EnableIRQ(NMI_IRQn);
    NVIC_SetPriority(NMI_IRQn, L3_IRQ_LEVEL_NMI);    
  }
  
  return Ok; 
} // Exint_Nmi_Init

/**
 ******************************************************************************
 ** \brief De-Init Non-Maskable Interrupt (NMI)
 **
 ** This function de-initializes the NMI according to the given
 ** configuration.
 **
 ** \param [in]  pstcConfig        NMI configuration parameters
 ** 
 ** \retval Ok                     NMI initialized
 ** \retval ErrorInvalidParameter  pstcConfig == NULL
 **
 ** \note NMI shares the interrupt vector with the Hardware Watchog. To avoid
 **       cross influence of initialization of the NVIC use
 **       stc_exint_nmi_config_t#bTouchNVIC to determine whether to touch the
 **       corresponding NVIC registers or not.
 ******************************************************************************/
en_result_t Exint_Nmi_DeInit(stc_exint_nmi_config_t* pstcConfig)
{
  if ( pstcConfig == NULL )
  {
    return ErrorInvalidParameter ;
  }

  // Set internal data
  stcExintNMIInternData.pfnNMICallback = pstcConfig->pfnNMICallback;
  
  if (pstcConfig->bTouchNVIC == TRUE)
  {
    NVIC_ClearPendingIRQ(NMI_IRQn);
    NVIC_DisableIRQ(NMI_IRQn);
    NVIC_SetPriority(NMI_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);    
  }
  
  return Ok; 
} // Exint_Nmi_DeInit


//@} // ExitGroup

#endif // #if (defined(L3_PERIPHERAL_EXINT_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
