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
/** \file qprc.c
 **
 ** A detailed description is available at 
 ** @link QprcGroup QPRC Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-23  1.1  MWi  All device type support
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "qprc.h"

#if (defined(L3_PERIPHERAL_QPRC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup QprcGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/// Macro to return the number of enabled MFS instances
#define QPRC_INSTANCE_COUNT (uint32_t)(sizeof(m_astcQprcInstanceDataLut) / sizeof(m_astcQprcInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled QPRC instances and their internal data
stc_qprc_instance_data_t m_astcQprcInstanceDataLut[] =
{
    #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
    { &QPRC0, // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)
    { &QPRC1, // pstcInstance
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
 ** \brief Return the internal data for a certain QPRC instance.
 **
 ** \param pstcQprc  Pointer to QPRC instance
 **
 ** \return  Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_qprc_intern_data_t* QprcGetInternDataPtr(const stc_qprcn_t* pstcQprc) 
{
  uint32_t u32Instance;
   
  for (u32Instance = 0; u32Instance < QPRC_INSTANCE_COUNT; u32Instance++)
  {
    if (pstcQprc == m_astcQprcInstanceDataLut[u32Instance].pstcInstance)
    {
      return &m_astcQprcInstanceDataLut[u32Instance].stcInternData;
    }
  }

  return NULL;
}

/**
 ******************************************************************************
 ** \brief QPRC interrupt service routine.
 **
 ******************************************************************************/
void QprcIrqHandler ( stc_qprcn_t*            pstcQprc, 
                      stc_qprc_intern_data_t* pstcQprcInternData
                    )
{
  // Outrange interrupt
  if (pstcQprc->QECR_f.ORNGF == 1)
  {
     pstcQprc->QECR_f.ORNGF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnOutrangeCallback != NULL)
     {
        pstcQprcInternData->pfnOutrangeCallback(); 
     }
  }
  
  // Zero index interrupt
  if (pstcQprc->QICRL_f.ZIIF == 1)
  {
     pstcQprc->QICRL_f.ZIIF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnZeroIndexCallback != NULL)
     {
        pstcQprcInternData->pfnZeroIndexCallback(); 
     }
  }
  
  // Overflow interrupt
  if (pstcQprc->QICRL_f.OFDF == 1)
  {
     pstcQprc->QICRL_f.OFDF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnOverflowCallback != NULL)
     {
        pstcQprcInternData->pfnOverflowCallback(); 
     }
  }
  
  // Underflow interrupt
  if (pstcQprc->QICRL_f.UFDF == 1)
  {
     pstcQprc->QICRL_f.UFDF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnUnderflowCallback != NULL)
     {
        pstcQprcInternData->pfnUnderflowCallback(); 
     }
  }
  
  // PC and RC match interrupt
  if (pstcQprc->QICRL_f.QPRCMF == 1)
  {
     pstcQprc->QICRL_f.QPRCMF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnPcRcMatchCallback != NULL)
     {
        pstcQprcInternData->pfnPcRcMatchCallback(); 
     }
  }
 
  // PC match interrupt
  if (pstcQprc->QICRL_f.QPCMF == 1)
  {
     pstcQprc->QICRL_f.QPCMF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnPcMatchCallback != NULL)
     {
        pstcQprcInternData->pfnPcMatchCallback(); 
     }
  }

  // PC match and RC match interrupt
  if (pstcQprc->QICRH_f.QPCNRCMF == 1)
  {
     pstcQprc->QICRH_f.QPCNRCMF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnPcMatchRcMatchCallback != NULL)
     {
        pstcQprcInternData->pfnPcMatchRcMatchCallback(); 
     }
  }
  
  // Count inversion interrupt
  if (pstcQprc->QICRH_f.CDCF == 1)
  {
     pstcQprc->QICRH_f.CDCF = 0;  // clear flag
     
     if (pstcQprcInternData->pfnCountInversionCallback != NULL)
     {
        pstcQprcInternData->pfnCountInversionCallback(); 
     }
  }
}

/**
 ******************************************************************************
 ** \brief Initialisation of a QPRC module.
 **
 ** \param [in]  pstcQprc      Pointer to QPRC instance register area
 ** \param [in]  pstcConfig    QPRC module configuration 
 **
 ** \retval Ok                      Initializiation of QORC module successfully 
 **                                 done.
 ** \retval ErrorInvalidParameter   pstcQprc == NULL, pstcConfig == NULL, other
 **                                 invalid setting.
 ******************************************************************************/
en_result_t Qprc_Init( stc_qprcn_t*        pstcQprc, 
                       stc_qprc_config_t*  pstcConfig
                     )
{
  stc_qprc_intern_data_t* pstcQprcInternData;
  
  // predefinitions
  stc_qprc_qcrl_field_t  u8QCRL;
  stc_qprc_qcrh_field_t  u8QCRH;
  stc_qprc_qecr_field_t  u16QECR;
  stc_qprc_qicrl_field_t u8QICRL;
  stc_qprc_qicrh_field_t u8QICRH;
  
  // Check for valid pointers
  if ( (pstcQprc   == NULL) ||
       (pstcConfig == NULL)
     )
  {
    return ErrorInvalidParameter;
  }
  
  // Get ptr to internal data struct ...
  pstcQprcInternData = QprcGetInternDataPtr( pstcQprc ) ;
  // ... and check
  if ( pstcQprcInternData == NULL )
  {
    return ErrorInvalidParameter;
  }
  
  pstcQprc->QCR_f.PSTP = 1; // Stop counter first for unlocking counter registers
  u8QCRL.PSTP = 1 ;         // Hold stop counting at register update -> Qprc_Enable() starts counting
  
  // Set single bits
  u8QCRL.SWAP = (pstcConfig->bSwapAinBin       == TRUE) ? 1 : 0 ;
  u8QCRL.RSEL = (pstcConfig->bRegisterFunction == TRUE) ? 1 : 0 ;
  u8QCRL.CGSC = (pstcConfig->bGateFunction     == TRUE) ? 1 : 0 ;
  
  // Revolution Counter Mode
  switch (pstcConfig->enRcMode)
  {
    case QprcRcMode0:
      u8QCRL.RCM0 = 0;
      u8QCRL.RCM1 = 0;
      break;
    case QprcRcMode1:
      u8QCRL.RCM0 = 1;
      u8QCRL.RCM1 = 0;
      break;
    case QprcRcMode2:
      u8QCRL.RCM0 = 0;
      u8QCRL.RCM1 = 1;
      break;
    case QprcRcMode3:
      u8QCRL.RCM0 = 1;
      u8QCRL.RCM1 = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  // Position Counter Mode
  switch (pstcConfig->enPcMode)
  {
    case QprcPcMode0:
      u8QCRL.PCM0 = 0;
      u8QCRL.PCM1 = 0;
      break;
    case QprcPcMode1:
      u8QCRL.PCM0 = 1;
      u8QCRL.PCM1 = 0;
      break;
    case QprcPcMode2:
      u8QCRL.PCM0 = 0;
      u8QCRL.PCM1 = 1;
      break;
    case QprcPcMode3:
      u8QCRL.PCM0 = 1;
      u8QCRL.PCM1 = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  // ZIN edge or level detection
  switch (pstcConfig->enZinEdge)
  {
    case QprcZinDisable:
      u8QCRH.CGE0 = 0;
      u8QCRH.CGE1 = 0;
      break;
    case QprcZinFallingEdge:
      if(pstcConfig->bGateFunction == FALSE)
      {
        u8QCRH.CGE0 = 1;
        u8QCRH.CGE1 = 0;
      }
      else
      {
        return ErrorInvalidParameter;
      }
      break;
    case QprcZinRisingEdge:
      if(pstcConfig->bGateFunction == FALSE)
      {
        u8QCRH.CGE0 = 0;
        u8QCRH.CGE1 = 1;
      }
      else
      {
        return ErrorInvalidParameter;
      }
      break;
    case QprcZinBothEdges:
      if(pstcConfig->bGateFunction == FALSE)
      {
        u8QCRH.CGE0 = 1;
        u8QCRH.CGE1 = 1;
      }
      else
      {
        return ErrorInvalidParameter;
      }
      break;    
    case QprcZinLowLevel:
      if(pstcConfig->bGateFunction == TRUE)
      {
        u8QCRH.CGE0 = 1;
        u8QCRH.CGE1 = 0;
      }
      else
      {
        return ErrorInvalidParameter;
      }
      break;  
    case QprcZinHighLevel:
      if(pstcConfig->bGateFunction == TRUE)
      {
        u8QCRH.CGE0 = 0;
        u8QCRH.CGE1 = 1;
      }
      else
      {
        return ErrorInvalidParameter;
      }
      break; 
    default:
      return ErrorInvalidParameter;
  }
  
  // BIN edge detection
  switch (pstcConfig->enBinEdge)
  {
    case QprcBinDisable:
      u8QCRH.BES0 = 0;
      u8QCRH.BES1 = 0;
      break;
    case QprcBinFallingEdge:
      u8QCRH.BES0 = 1;
      u8QCRH.BES1 = 0;
      break;
    case QprcBinRisingEdge:
      u8QCRH.BES0 = 0;
      u8QCRH.BES1 = 1;
      break;
    case QprcBinBothEdges:
      u8QCRH.BES0 = 1;
      u8QCRH.BES1 = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }

  // AIN edge detection
  switch (pstcConfig->enAinEdge)
  {
    case QprcAinDisable:
      u8QCRH.AES0 = 0;
      u8QCRH.AES1 = 0;
      break;
    case QprcAinFallingEdge:
      u8QCRH.AES0 = 1;
      u8QCRH.AES1 = 0;
      break;
    case QprcAinRisingEdge:
      u8QCRH.AES0 = 0;
      u8QCRH.AES1 = 1;
      break;
    case QprcAinBothEdges:
      u8QCRH.AES0 = 1;
      u8QCRH.AES1 = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }

  // Reset mask settings
  switch (pstcConfig->enPcResetMask)
  {
    case QprcResetMaskDisable:
      u8QCRH.PCRM0 = 0;
      u8QCRH.PCRM1 = 0;
      break;
    case QprcResetMask2Times:
      u8QCRH.PCRM0 = 1;
      u8QCRH.PCRM1 = 0;
      break;
    case QprcResetMask4Times:
      u8QCRH.PCRM0 = 0;
      u8QCRH.PCRM1 = 1;
      break;
    case QprcResetMask8Times:
      u8QCRH.PCRM0 = 1;
      u8QCRH.PCRM1 = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }
   
  u16QECR.ORNGMD = (pstcConfig->b8KValue                 == TRUE) ? 1 : 0 ;
  u16QECR.ORNGIE = (pstcConfig->bEnableOutrangeInterrupt == TRUE) ? 1 : 0 ;
  
  u8QICRL.OUZIE   = (pstcConfig->bEnableOvrfUndfZIdxInterrupt == TRUE) ? 1 : 0 ;
  u8QICRL.QPRCMIE = (pstcConfig->bEnablePcRcMatchInterrupt    == TRUE) ? 1 : 0 ;
  u8QICRL.QPCMIE  = (pstcConfig->bEnablePcMatchInterrupt      == TRUE) ? 1 : 0 ;
  
  u8QICRH.QPCNRCMIE = (pstcConfig->bEnablePcMatchRcMatchInterupt  == TRUE) ? 1 : 0 ;
  u8QICRH.CDCIE     = (pstcConfig->bEnableCountInversionInterrupt == TRUE) ? 1 : 0 ;
  
  // Update internal callback pointers
  //
  pstcQprcInternData->pfnCountInversionCallback = pstcConfig->pfnCountInversionCallback ;
  pstcQprcInternData->pfnZeroIndexCallback      = pstcConfig->pfnZeroIndexCallback ;
  pstcQprcInternData->pfnOverflowCallback       = pstcConfig->pfnOverflowCallback ;
  pstcQprcInternData->pfnUnderflowCallback      = pstcConfig->pfnUnderflowCallback ;
  pstcQprcInternData->pfnPcRcMatchCallback      = pstcConfig->pfnPcRcMatchCallback ;
  pstcQprcInternData->pfnPcMatchCallback        = pstcConfig->pfnPcMatchCallback ;
  pstcQprcInternData->pfnPcMatchRcMatchCallback = pstcConfig->pfnPcMatchRcMatchCallback ;
  pstcQprcInternData->pfnOutrangeCallback       = pstcConfig->pfnOutrangeCallback ;
  
  // Update hardware
  //
  if( pstcConfig->u16PositionCount != 0 )
  {
    pstcQprc->QCRH_f.CGE0 = 0;  // should be zero during init, but
    pstcQprc->QCRH_f.CGE1 = 0;  //   cleared anyhow for safety reasons ...
    pstcQprc->QPCR = pstcConfig->u16PositionCount ;
  }

  if( pstcConfig->u16RevolutionCount != 0 )
  {
    pstcQprc->QCRL_f.RCM0 = 0;  // should be zero during init, but
    pstcQprc->QCRL_f.RCM1 = 0;  //   cleared anyhow for safety reasons ...
    pstcQprc->QRCR = pstcConfig->u16RevolutionCount ;
  }
  
  if( pstcConfig->u16MaximumPositionCount != 0 )
  {
    pstcQprc->QMPR = pstcConfig->u16MaximumPositionCount ;
  }
  
  pstcQprc->QPCCR = pstcConfig->u16PositionCounterCompare ;
  pstcQprc->QPRCR = pstcConfig->u16PositionRevolutionCounterCompare ;
    
  pstcQprc->QCRL_f  = u8QCRL;
  pstcQprc->QCRH_f  = u8QCRH;
  pstcQprc->QECR_f  = u16QECR;
  pstcQprc->QICRL_f = u8QICRL;
  pstcQprc->QICRH_f = u8QICRH;

  if (pstcConfig->bTouchNVIC)
  {
    // Enable interrupts
    NVIC_ClearPendingIRQ(DTIM_QDU_IRQn);
    NVIC_EnableIRQ(DTIM_QDU_IRQn);
    NVIC_SetPriority(DTIM_QDU_IRQn, L3_IRQ_LEVEL_QPRC0);
  }
    
  return Ok;
} // Qprc_Init

/**
 ******************************************************************************
 ** \brief Set mew Position Count Vaule
 **
 ** \param [in]  pstcQprc      Pointer to QPRC instance register area
 ** \param [in]  u16PcValue    New position count value
 **
 ** \note This function does not have a return value due to possible inline
 **       usage!
 ******************************************************************************/
void Qprc_SetPositionCount( stc_qprcn_t* pstcQprc,
                            uint16_t     u16PcValue )
{
  uint8_t u8RegisterBuffer;
  
  u8RegisterBuffer = pstcQprc->QCRH ; // Save register content
  pstcQprc->QCRH_f.CGE0 = 0 ;         // disable temporarily ZIN detection
  pstcQprc->QCRH_f.CGE1 = 0 ;
  pstcQprc->QPCR = u16PcValue ;       // Update value
  pstcQprc->QCRH = u8RegisterBuffer ; // restore register content
} // Qprc_SetPositionCount

/**
 ******************************************************************************
 ** \brief Set mew Revolution Count Vaule
 **
 ** \param [in]  pstcQprc      Pointer to QPRC instance register area
 ** \param [in]  u16RcValue    New revolution count value
 **
 ** \note This function does not have a return value due to possible inline
 **       usage!
 ******************************************************************************/
void Qprc_SetRevolutionCount( stc_qprcn_t* pstcQprc,
                              uint16_t     u16RcValue )
{
  uint8_t u8RegisterBuffer;
  
  u8RegisterBuffer = pstcQprc->QCRL ; // Save register content
  pstcQprc->QCRL_f.RCM0 = 0;          // Disable temporarily revolution counter
  pstcQprc->QCRL_f.RCM1 = 0;
  pstcQprc->QRCR = u16RcValue ;       // Update value
  pstcQprc->QCRL = u8RegisterBuffer ; // restore register content
} // Qprc_SetRevolutionCount

/**
 ******************************************************************************
 ** \brief Set mew Revolution Counter Compare Vaule
 **
 ** \param [in]  pstcQprc         Pointer to QPRC instance register area
 ** \param [in]  u16RcCompValue   New revolution counter compare value
 **
 ** \note This function does not have a return value due to possible inline
 **       usage!
 ******************************************************************************/
void Qprc_SetRevolutionCountCompare( stc_qprcn_t* pstcQprc,
                                     uint16_t     u16RcCompValue )
{
  pstcQprc->QPCCR = u16RcCompValue ;
} // Qprc_SetRevolutionCountCompare

/**
 ******************************************************************************
 ** \brief Set mew Position and Revolution Counter Compare Vaule
 **
 ** \param [in]  pstcQprc          Pointer to QPRC instance register area
 ** \param [in]  u16PcRcCompValue  New position and revolution couner compare
 **                                value
 **
 ** \note This function does not have a return value due to possible inline
 **       usage!
 ******************************************************************************/
void Qprc_SetPositionRevolutionCountCompare( stc_qprcn_t* pstcQprc,
                                             uint16_t     u16PcRcCompValue )
{
  pstcQprc->QPRCR = u16PcRcCompValue ;
} // Qprc_SetPositionRevolutionCountCompare

/**
 ******************************************************************************
 ** \brief Set Maximum Position Counter Compare Vaule
 **
 ** \param [in]  pstcQprc        Pointer to QPRC instance register area
 ** \param [in]  u16PcMaxValue   New maximum position couner value
 **
 ** \note This function does not have a return value due to possible inline
 **       usage!
 ******************************************************************************/
void Qprc_SetMaxPositionCount( stc_qprcn_t* pstcQprc,
                               uint16_t     u16PcMaxValue )
{
  pstcQprc->QMPR = u16PcMaxValue ;
} // Qprc_SetPositionRevolutionCountCompare

/**
 ******************************************************************************
 ** \brief Get recent Position Count Vaule
 **
 ** \param [in]  pstcQprc   Pointer to QPRC instance register area
 **
 ** \return                 Recent Position Count value 
 ******************************************************************************/
uint16_t Qprc_GetPositionCount( stc_qprcn_t* pstcQprc )
{
  return pstcQprc->QPCR ;
} // Qprc_GetPositionCount

/**
 ******************************************************************************
 ** \brief Get recent Revolution Count Vaule
 **
 ** \param [in]  pstcQprc  Pointer to QPRC instance register area
 **
 ** \return                Recent Revolution Count value 
 ******************************************************************************/
uint16_t Qprc_GetRevolutionCount( stc_qprcn_t* pstcQprc )
{
  return pstcQprc->QRCR ;
} // Qprc_GetRevolutionCount

/**
 ******************************************************************************
 ** \brief Get Revolution Count Compare Vaule
 **
 ** \param [in]  pstcQprc   Pointer to QPRC instance register area
 **
 ** \return                 Recent Revolution Count Comapre value 
 ******************************************************************************/
uint16_t Qprc_GetRevolutionCountCompare( stc_qprcn_t* pstcQprc )
{
  return pstcQprc->QPCCR ;
} // Qprc_GetRevolutionCountCompare

/**
 ******************************************************************************
 ** \brief Get Position and Revolution Count Compare Vaule
 **
 ** \param [in]  pstcQprc     Pointer to QPRC instance register area
 **
 ** \return                   Recent Position and Revolution Count Comapre
 **                           value 
 ******************************************************************************/
uint16_t Qprc_GetPositionRevolutionCountCompare( stc_qprcn_t* pstcQprc )
{
  return pstcQprc->QPRCR ;
} // Qprc_GetPositionRevolutionCountCompare

/**
 ******************************************************************************
 ** \brief Get Maximum Position Count Vaule
 **
 ** \param [in]  pstcQprc     Pointer to QPRC instance register area
 **
 ** \return                   Recent Maximum Position Count value 
 ******************************************************************************/
uint16_t Qprc_GetMaxPositionCount( stc_qprcn_t* pstcQprc )
{
  return pstcQprc->QMPR ;
}

#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7) || \
    (L3_DEVICE_TYPE == L3_TYPE8)

/**
 ******************************************************************************
 ** \brief Get recent Position and Revoultion Count Vaules simulataneously
 **
 ** \param [in]  pstcQprc         Pointer to QPRC instance register area
 ** \param [in]  pstcPosRevCount  Pointer to a #stc_qprc_pos_rev_count_t
 **                               structure
 ******************************************************************************/
void Qprc_GetPositionRevolutionCount( stc_qprcn_t*              pstcQprc,
                                      stc_qprc_pos_rev_count_t* pstcPosRevCount )
{
  uint32_t u32TempRead;
  
  u32TempRead = pstcQprc->QPRCRR ;
  
  pstcPosRevCount->u16PositionCount   = (uint16_t) (u32TempRead >> 16) ;
  pstcPosRevCount->u16RevolutionCount = (uint16_t) (u32TempRead &  0xFFFF) ;
}

#endif // #if (L3_DEVICE_TYPE == L3_TYPE3) || ... || (L3_DEVICE_TYPE == L3_TYPE8)

/**
 ******************************************************************************
 ** \brief Enable QPRC Counting
 **
 ** \param [in]  pstcQprc     Pointer to QPRC instance register area
 ******************************************************************************/
void Qprc_Enable( stc_qprcn_t* pstcQprc )
{
  pstcQprc->QCRL_f.PSTP = 0 ;
} // Qprc_Enable

/**
 ******************************************************************************
 ** \brief Disable QPRC Counting
 **
 ** \param [in]  pstcQprc     Pointer to QPRC instance register area
 ******************************************************************************/
void Qprc_Disable( stc_qprcn_t* pstcQprc )
{
  pstcQprc->QCRL_f.PSTP = 1 ;
} // Qprc_Disable

/**
 ******************************************************************************
 ** \brief De-Initialisation of a QPRC module.
 **
 ** \param [in]  pstcQprc      Pointer to QPRC instance register area
 ** \param [in]  pstcConfig    QPRC module configuration 
 **
 ** \retval Ok                      De-Initializiation of QORC module  
 **                                 successfully done.
 ** \retval ErrorInvalidParameter   pstcQprc == NULL, pstcConfig == NULL, other
 **                                 invalid setting.
 ******************************************************************************/
en_result_t Qprc_DeInit( stc_qprcn_t*        pstcQprc, 
                         stc_qprc_config_t*  pstcConfig
                       )
{
  stc_qprc_intern_data_t* pstcQprcInternData;
  
  // Check for valid pointers
  if ( (pstcQprc   == NULL) ||
       (pstcConfig == NULL)
     )
  {
    return ErrorInvalidParameter;
  }
  
  // Get ptr to internal data struct ...
  pstcQprcInternData = QprcGetInternDataPtr( pstcQprc ) ;
  // ... and check
  if ( pstcQprcInternData == NULL )
  {
    return ErrorInvalidParameter;
  }

  if (pstcConfig->bTouchNVIC)
  {
    // Enable interrupts
    NVIC_ClearPendingIRQ(DTIM_QDU_IRQn);
    NVIC_DisableIRQ(DTIM_QDU_IRQn);
    NVIC_SetPriority(DTIM_QDU_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  
  // Clear Hardware
  pstcQprc->QCR  = 0x0010;    // PSTP = 1 (Stop couning)
  pstcQprc->QECR = 0;
  pstcQprc->QICR = 0;
  
  // Clear internal callback pointers
  pstcQprcInternData->pfnCountInversionCallback = NULL ;
  pstcQprcInternData->pfnZeroIndexCallback      = NULL ;
  pstcQprcInternData->pfnOverflowCallback       = NULL ;
  pstcQprcInternData->pfnUnderflowCallback      = NULL ;
  pstcQprcInternData->pfnPcRcMatchCallback      = NULL ;
  pstcQprcInternData->pfnPcMatchCallback        = NULL ;
  pstcQprcInternData->pfnPcMatchRcMatchCallback = NULL ;
  pstcQprcInternData->pfnOutrangeCallback       = NULL ;
  
  return Ok;
} // Qprc_DeInit


//@} // QprcGroup

#endif // #if (defined(L3_PERIPHERAL_QPRC_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
