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
/** \file mft.c
 **
 ** A detailed description is available at 
 ** @link MftGroup Base Timer Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  MWi  PPG IGBT mode for Type7 devices added
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "mft.h"

#if (defined(L3_PERIPHERAL_MFT_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup MftGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/// Macro to return the number of enabled MFT instances
#define MFT_INSTANCE_COUNT (uint32_t)(sizeof(m_astcMftInstanceDataLut) / sizeof(m_astcMftInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled MFT instances and their internal data
stc_mft_instance_data_t m_astcMftInstanceDataLut[] =
{
  #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    { 
      &MFT0,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
  #endif
  #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    { 
      &MFT1,  // pstcInstance
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

/******************************************************************************/
/* Local Functions                                                            */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Return the internal data for a certain MFT instance.
 **
 ** \param pstcMft Pointer to MFT instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_mft_intern_data_t* MftGetInternDataPtr(const stc_mftn_t* pstcMft) 
{
    uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < MFT_INSTANCE_COUNT; u32Instance++)
    {
        if (pstcMft == m_astcMftInstanceDataLut[u32Instance].pstcInstance)
        {
            return &m_astcMftInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}

/**
 ******************************************************************************
 ** \brief Returns the number of a MFT instance pointer
 **
 ** \param [in]  pstcMft       Pointer to MFT instance register area
 **
 ** \return    Number 0, 1 if instance found, -1 on error
 **
 ******************************************************************************/
static int8_t MftGetInstanceNumber(stc_mftn_t* pstcMft)
{
  if (pstcMft == &MFT0)
  {
    return 0;
  }
  
  if (pstcMft == &MFT1)
  {
    return 1;
  }
  
  return -1;
}

/**
 ******************************************************************************
 ** \brief Returns the address of the MFT instance's dedicated PPG
 **
 ** \param [in]  pstcMft       Pointer to MFT instance register area
 **
 ** \return    address pointer
 **
 ******************************************************************************/
static volatile stc_ppgn_t* MftGetPpgPointer(stc_mftn_t* pstcMft)
{
  if (pstcMft == &MFT0)
  {
    return &PPG_MFT0;
  }

  return &PPG_MFT1;
}

/**
 ******************************************************************************
 ** \brief Init MFT WFG-DTIF Interrupt
 ******************************************************************************/
static void MftInitWfgInterrupt(void)
{
  NVIC_ClearPendingIRQ(WFG_IRQn);
  NVIC_EnableIRQ(WFG_IRQn);
  NVIC_SetPriority(WFG_IRQn, L3_IRQ_LEVEL_MFT_WFG_DTIF);
}

/**
 ******************************************************************************
 ** \brief Init MFT FRT Interrupt
 ******************************************************************************/
static void MftInitFrtInterrupt(void)
{
  NVIC_ClearPendingIRQ(FRTIM_IRQn);
  NVIC_EnableIRQ(FRTIM_IRQn);
  NVIC_SetPriority(FRTIM_IRQn, L3_IRQ_LEVEL_MFT_FRT);
}

/**
 ******************************************************************************
 ** \brief Init MFT ICU Interrupt
 ******************************************************************************/
static void MftInitIcuInterrupt(void)
{
  NVIC_ClearPendingIRQ(INCAP_IRQn);
  NVIC_EnableIRQ(INCAP_IRQn);
  NVIC_SetPriority(INCAP_IRQn, L3_IRQ_LEVEL_MFT_ICU);
}

/**
 ******************************************************************************
 ** \brief Init MFT OCU Interrupt
 ******************************************************************************/
static void MftInitOcuInterrupt(void)
{
  NVIC_ClearPendingIRQ(OUTCOMP_IRQn);
  NVIC_EnableIRQ(OUTCOMP_IRQn);
  NVIC_SetPriority(OUTCOMP_IRQn, L3_IRQ_LEVEL_MFT_OCU);
}

/**
 ******************************************************************************
 ** \brief Init MFT PPG Interrupt
 ******************************************************************************/
static void MftInitPpgInterrupt(void)
{
  NVIC_ClearPendingIRQ(PPG_IRQn);
  NVIC_EnableIRQ(PPG_IRQn);
  NVIC_SetPriority(PPG_IRQn, L3_IRQ_LEVEL_MFT_PPG);
}

/**
 ******************************************************************************
 ** \brief De-Init MFT WFG-DTIF Interrupt
 ******************************************************************************/
static void MftDeInitWfgInterrupt(void)
{
  NVIC_ClearPendingIRQ(WFG_IRQn);
  NVIC_DisableIRQ(WFG_IRQn);
  NVIC_SetPriority(WFG_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
}

/**
 ******************************************************************************
 ** \brief De-Init MFT FRT Interrupt
 ******************************************************************************/
static void MftDeInitFrtInterrupt(void)
{
  NVIC_ClearPendingIRQ(FRTIM_IRQn);
  NVIC_DisableIRQ(FRTIM_IRQn);
  NVIC_SetPriority(FRTIM_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
}

/**
 ******************************************************************************
 ** \brief De-Init MFT ICU Interrupt
 ******************************************************************************/
static void MftDeInitIcuInterrupt(void)
{
  NVIC_ClearPendingIRQ(INCAP_IRQn);
  NVIC_DisableIRQ(INCAP_IRQn);
  NVIC_SetPriority(INCAP_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
}

/**
 ******************************************************************************
 ** \brief De-Init MFT OCU Interrupt
 ******************************************************************************/
static void MftDeInitOcuInterrupt(void)
{
  NVIC_ClearPendingIRQ(OUTCOMP_IRQn);
  NVIC_DisableIRQ(OUTCOMP_IRQn);
  NVIC_SetPriority(OUTCOMP_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
}

/**
 ******************************************************************************
 ** \brief De-Init MFT PPG Interrupt
 ******************************************************************************/
static void MftDeInitPpgInterrupt(void)
{
  NVIC_ClearPendingIRQ(PPG_IRQn);
  NVIC_DisableIRQ(PPG_IRQn);
  NVIC_SetPriority(PPG_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
}

/******************************************************************************/
/* Global Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* Interrupt Handlers                                                         */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief MFTn-WFG Interrupt Handler
 **
 ** This function checks the MFTn-WFG interrupt cause, clears the cause, and
 ** calls the callback handlers.
 **
 ** \param [in]  pstcMft             Pointer to MFT instance
 ** \param [in]  pstcMftInternData   Pointer to MFT instance intern data
 **
 ******************************************************************************/
void MftWfgIrqHandler( stc_mftn_t*            pstcMft,
                       stc_mft_intern_data_t* pstcMftInternData
                     )
{
  // Check WFG0/1 ICR Irq
  if (pstcMft->u16WFIR.TMIF10 == 1)
  {
    pstcMft->u16WFIR.TMIC10 = 1;  // clear Irq
    
    if (pstcMftInternData->pfnWfg01Callback != NULL)
    {
      pstcMftInternData->pfnWfg01Callback();
    } 
  }

  // Check WFG2/3 Irq 
  if (pstcMft->u16WFIR.TMIF32 == 1)
  {
    pstcMft->u16WFIR.TMIC32 = 1;  // clear Irq
    
    if (pstcMftInternData->pfnWfg23Callback != NULL)
    {
      pstcMftInternData->pfnWfg23Callback();
    } 
  }
  
  // Check WFG4/5 Irq 
  if (pstcMft->u16WFIR.TMIF54 == 1)
  {
    pstcMft->u16WFIR.TMIC54 = 1;  // clear Irq
    
    if (pstcMftInternData->pfnWfg45Callback != NULL)
    {
      pstcMftInternData->pfnWfg45Callback();
    } 
  }
  
  // Check NZCL Irq
  if (pstcMft->u16WFIR.DTIF == 1)
  {
    pstcMft->u16WFIR.DTIC = 1; // Clear Irq
    
    if (pstcMftInternData->pfnNzclCallback != NULL)
    {
       pstcMftInternData->pfnNzclCallback();
    }  
  }
}

/**
 ******************************************************************************
 ** \brief MFTn-FRT Interrupt Handler
 **
 ** This function checks the MFTn-FRT interrupt cause, clears the cause, and
 ** calls the callback handlers.
 **
 ** \param [in]  pstcMft             Pointer to MFT instance
 ** \param [in]  pstcMftInternData   Pointer to MFT instance intern data
 **
 ******************************************************************************/
void MftFrtIrqHandler( stc_mftn_t*            pstcMft,
                       stc_mft_intern_data_t* pstcMftInternData
                     )
{
  // Check FRT0 Zero Irq 
  if (pstcMft->u16TCSA0.IRQZF == 1)
  {
    pstcMft->u16TCSA0.IRQZF = 0; // Clear Irq

    if (pstcMftInternData->pfnFrt0ZeroCallback != NULL)
    {
       pstcMftInternData->pfnFrt0ZeroCallback();
    }
  }  
  
  // Check FRT1 Zero Irq 
  if (pstcMft->u16TCSA1.IRQZF == 1)
  {
    pstcMft->u16TCSA1.IRQZF = 0; // Clear Irq
    
    if (pstcMftInternData->pfnFrt1ZeroCallback != NULL)
    {
       pstcMftInternData->pfnFrt1ZeroCallback();
    }
  } 

  // Check FRT2 Zero Irq 
  if (pstcMft->u16TCSA2.IRQZF == 1)
  {
    pstcMft->u16TCSA2.IRQZF = 0; // Clear Irq
    
    if (pstcMftInternData->pfnFrt2ZeroCallback != NULL)
    {
       pstcMftInternData->pfnFrt2ZeroCallback();
    }
  } 
  
  // Check FRT0 Peak Irq
  if (pstcMft->u16TCSA0.ICLR == 1)
  {
    pstcMft->u16TCSA0.ICLR = 0; // Clear Irq
    
    if (pstcMftInternData->pfnFrt0PeakCallback != NULL)
    {
       pstcMftInternData->pfnFrt0PeakCallback();
    }
  }  
  
  // Check FRT1 Peak Irq 
  if (pstcMft->u16TCSA1.ICLR == 1)
  {
    pstcMft->u16TCSA1.ICLR = 0; // Clear Irq

    if (pstcMftInternData->pfnFrt1PeakCallback != NULL)
    {
       pstcMftInternData->pfnFrt1PeakCallback();
    }
  } 
  
  // Check FRT2 Peak Irq 
  if (pstcMft->u16TCSA2.ICLR == 1)
  {
    pstcMft->u16TCSA2.ICLR = 0; // Clear Irq

    if (pstcMftInternData->pfnFrt2PeakCallback != NULL)
    {
       pstcMftInternData->pfnFrt2PeakCallback();
    }
  } 
}

/**
 ******************************************************************************
 ** \brief MFTn-OCU Interrupt Handler
 **
 ** This function checks the MFTn-OCU interrupt cause, clears the cause, and
 ** calls the callback handlers.
 **
 ** \param [in]  pstcMft             Pointer to MFT instance
 ** \param [in]  pstcMftInternData   Pointer to MFT instance intern data
 **
 ******************************************************************************/
void MftOcuIrqHandler( stc_mftn_t*            pstcMft,
                       stc_mft_intern_data_t* pstcMftInternData
                     )
{
  // Check OCU0 Irq
  if (pstcMft->u8OCSA10.IOP0 == 1)
  {
    pstcMft->u8OCSA10.IOP0 = 0; // Clear Irq

    if (pstcMftInternData->pfnOcu0Callback != NULL)
    {
       pstcMftInternData->pfnOcu0Callback();
    } 
  }

  // Check OCU1 Irq 
  if (pstcMft->u8OCSA10.IOP1 == 1)
  {
    pstcMft->u8OCSA10.IOP1 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnOcu1Callback != NULL)
    {
       pstcMftInternData->pfnOcu1Callback();
    } 
  }

  // Check OCU2 Irq 
  if (pstcMft->u8OCSA32.IOP2 == 1)
  {
    pstcMft->u8OCSA32.IOP2 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnOcu2Callback != NULL)
    {
       pstcMftInternData->pfnOcu2Callback();
    } 
  }

  // Check OCU3 Irq 
  if (pstcMft->u8OCSA32.IOP3 == 1)
  {
    pstcMft->u8OCSA32.IOP3 = 0; // Clear Irq
   
    if (pstcMftInternData->pfnOcu3Callback != NULL)
    {
       pstcMftInternData->pfnOcu3Callback();
    } 
  }

  // Check OCU4 Irq 
  if (pstcMft->u8OCSA54.IOP4 == 1)
  {
    pstcMft->u8OCSA54.IOP4 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnOcu4Callback != NULL)
    {
       pstcMftInternData->pfnOcu4Callback();
    } 
  }

  // Check OCU5 Irq 
  if (pstcMft->u8OCSA54.IOP5 == 1)
  {
     pstcMft->u8OCSA54.IOP5 = 0; // Clear Irq
     
    if (pstcMftInternData->pfnOcu5Callback != NULL)
    {
       pstcMftInternData->pfnOcu5Callback();
    } 
  }
}

/**
 ******************************************************************************
 ** \brief MFTn-ICU Interrupt Handler
 **
 ** This function checks the MFTn-ICU interrupt cause, clears the cause, and
 ** calls the callback handlers.
 **
 ** \param [in]  pstcMft             Pointer to MFT instance
 ** \param [in]  pstcMftInternData   Pointer to MFT instance intern data
 **
 ******************************************************************************/
void MftIcuIrqHandler( stc_mftn_t*            pstcMft,
                       stc_mft_intern_data_t* pstcMftInternData
                     )
{
  // Check ICU0 Irq 
  if (pstcMft->u8ICSA10.ICP0 == 1)
  {
    pstcMft->u8ICSA10.ICP0 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnIcu0Callback != NULL)
    {
      pstcMftInternData->pfnIcu0Callback();
    }   
  }  
  
  // Check ICU1 Irq 
  if (pstcMft->u8ICSA10.ICP1 == 1)
  {
    pstcMft->u8ICSA10.ICP1 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnIcu1Callback != NULL)
    {
      pstcMftInternData->pfnIcu1Callback();
    }   
  }  

  // Check ICU2 Irq 
  if (pstcMft->u8ICSA32.ICP2 == 1)
  {
    pstcMft->u8ICSA32.ICP2 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnIcu2Callback != NULL)
    {
      pstcMftInternData->pfnIcu2Callback();
    }   
  }  

  // Check ICU3 Irq 
  if (pstcMft->u8ICSA32.ICP3 == 1)
  {
    pstcMft->u8ICSA32.ICP3 = 0; // Clear Irq
    
    if (pstcMftInternData->pfnIcu3Callback != NULL)
    {
      pstcMftInternData->pfnIcu3Callback();
    }   
  }   
}

/**
 ******************************************************************************
 ** \brief MFTn-PPG Interrupt Handler
 **
 ** This function checks the MFTn-PPG interrupt cause, clears the cause, and
 ** calls the callback handlers.
 **
 ** \param [in]  pstcPpg             Pointer to PPG instance
 ** \param [in]  pstcMftInternData   Pointer to MFT instance intern data
 **
 ******************************************************************************/
void MftPpgIrqHandler( stc_ppgn_t*            pstcPpg,
                       stc_mft_intern_data_t* pstcMftInternData
                     )
{
  // Check for PPG0 / PPG8
  if (pstcPpg->u8PPGC0.PUF == 1)
  {
    pstcPpg->u8PPGC0.PUF = 0; // Clear Irq
    
    if (pstcMftInternData->pfnPpg0Callback != NULL)
    {
      pstcMftInternData->pfnPpg0Callback();
    }    
  }

  // Check for PPG2 / PPG10
  if (pstcPpg->u8PPGC2.PUF == 1)
  {
    pstcPpg->u8PPGC2.PUF = 0; // Clear Irq
    
    if (pstcMftInternData->pfnPpg2Callback != NULL)
    {
      pstcMftInternData->pfnPpg2Callback();
    }    
  }
  
  // Check for PPG4 / PPG12
  if (pstcPpg->u8PPGC4.PUF == 1)
  {
    pstcPpg->u8PPGC4.PUF = 0; // Clear Irq
    
    if (pstcMftInternData->pfnPpg4Callback != NULL)
    {
      pstcMftInternData->pfnPpg4Callback();
    }    
  }
}

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Inits an MFT instance
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  pstcFrtOcuConfig  Configuration of FRT and OCU of MFT instance
 ** \param [in]  pstcWfgConfig     Configuration of WFG of MFT instance
 ** \param [in]  pstcNzclConfig    Configuration of NZCL of MFT instance
 ** \param [in]  pstcIcuConfig     Configuration of ICU of MFT instance
 ** \param [in]  pstcAdcmpConfig   Configuration of ADCMP of MFT instance
 ** \param [in]  pstcPpgConfig     Configuration of PPG of MFT instance
 **
 ** \retval Ok                     Initialisation of MFT module 
 **                                successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMft is illegal address pointer
 **             - pstcFrtOcuConfig == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **
 ******************************************************************************/
en_result_t Mft_Init( stc_mftn_t*                pstcMft, 
                      stc_mft_frt_ocu_config_t*  pstcFrtOcuConfig,
                      stc_mft_wfg_config_t*      pstcWfgConfig,
                      stc_mft_nzcl_config_t*     pstcNzclConfig,
                      stc_mft_icu_config_t*      pstcIcuConfig,
                      stc_mft_adcmp_config_t*    pstcAdcmpConfig,
                      stc_mft_ppg_config_t*      pstcPpgConfig
                    )
{
  volatile stc_ppg_base_t*   pstcMftPpgBase = &PPG;  // Pointer to PPG base registers
  volatile stc_ppgn_t*       pstcMftPpg;             // Pointer to dedicated PPG registers
  stc_mft_intern_data_t*     pstcMftInternData;      // Pointer to internal data structure
  int8_t                     i8MftInstanceNumber;    // MFT instance number
  
  // Local preset register variables
  stc_mft_frt_tcsa_t         unTCSA0  = {0};  // Typedef in mft.h
  stc_mft_frt_tcsa_t         unTCSA1  = {0};  // Typedef in mft.h
  stc_mft_frt_tcsa_t         unTCSA2  = {0};  // Typedef in mft.h
  stc_mft_frt_tcsb0_field_t  unTCSB0  = {0};
  stc_mft_frt_tcsb1_field_t  unTCSB1  = {0};
  stc_mft_frt_tcsb2_field_t  unTCSB2  = {0};
  stc_mft_ocu_ocfs_t         unOCFS10 = {0};  // Typedef in mft.h
  stc_mft_ocu_ocfs_t         unOCFS32 = {0};  // Typedef in mft.h
  stc_mft_ocu_ocfs_t         unOCFS54 = {0};  // Typedef in mft.h
  stc_mft_ocu_ocsa10_field_t unOCSA10 = {0};
  stc_mft_ocu_ocsb10_field_t unOCSB10 = {0};
  stc_mft_ocu_ocsa32_field_t unOCSA32 = {0};
  stc_mft_ocu_ocsb32_field_t unOCSB32 = {0};
  stc_mft_ocu_ocsa54_field_t unOCSA54 = {0};
  stc_mft_ocu_ocsb54_field_t unOCSB54 = {0};
  stc_mft_ocu_ocsc_field_t   unOCSC   = {0};
  stc_mft_wfg_wfsa_t         unWFSA10 = {0};
  stc_mft_wfg_wfsa_t         unWFSA32 = {0};
  stc_mft_wfg_wfsa_t         unWFSA54 = {0};
  uint16_t                   u16WFTM10 = 0;
  uint16_t                   u16WFTM32 = 0;
  uint16_t                   u16WFTM54 = 0;
  stc_mft_wfg_nzcl_t         unNZCL   = {0};  // Typedef in mft.h
  stc_mft_icu_icfs_t         unICFS10 = {0};  // Typedef in mft.h
  stc_mft_icu_icfs_t         unICFS32 = {0};  // Typedef in mft.h
  stc_mft_icu_icsa10_field_t unICSA10 = {0};
  stc_mft_icu_icsa32_field_t unICSA32 = {0};
  stc_mft_adcmp_acsa_t       unACSA   = {0};  // Typedef in mft.h
  stc_mft_adcmp_acsb_field_t unACSB   = {0};
  stc_mft_adcmp_atsa_t       unATSA   = {0};
  stc_mft_ppg_ppgc0_field_t  unPPGC0  = {0};
  stc_mft_ppg_ppgc2_field_t  unPPGC2  = {0};
  stc_mft_ppg_ppgc4_field_t  unPPGC4  = {0};
  stc_mft_ppg_ttcr0_field_t  unTTCRn  = {0};
  stc_mft_ppg_gatec0_field_t unGATEC0 = {0};
  stc_mft_ppg_gatec4_field_t unGATEC4 = {0};
  stc_mft_ppg_revc_field_t   unREVC0  = {0};

  // Check for NULL Pointer
  if ( (pstcMft          == NULL ) ||
       (pstcFrtOcuConfig == NULL))
  {
    return ErrorInvalidParameter ;
  }

  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if ( pstcMftInternData == NULL )
  {
    return ErrorInvalidParameter;
  }
  
  // Get MFT instance number ...
  i8MftInstanceNumber = MftGetInstanceNumber(pstcMft);
  // ... and check
  if (i8MftInstanceNumber < 0)
  {
    return ErrorInvalidParameter ;
  }
  
  // Set instance number to internal data
  pstcMftInternData->i8MftInstanceNumber = i8MftInstanceNumber;
  
  // Get pointer to dedicated PPG registers
  pstcMftPpg = MftGetPpgPointer(pstcMft);
  
  // Pre-Init intern data callback function pointers (of optional configuration)
  pstcMftInternData->pfnWfg01Callback = NULL;
  pstcMftInternData->pfnWfg23Callback = NULL;
  pstcMftInternData->pfnWfg45Callback = NULL;
  pstcMftInternData->pfnNzclCallback  = NULL;
  pstcMftInternData->pfnOcu0Callback  = NULL;
  pstcMftInternData->pfnOcu1Callback  = NULL;
  pstcMftInternData->pfnOcu2Callback  = NULL;
  pstcMftInternData->pfnOcu3Callback  = NULL;
  pstcMftInternData->pfnOcu4Callback  = NULL;
  pstcMftInternData->pfnOcu5Callback  = NULL;
  pstcMftInternData->pfnIcu0Callback  = NULL;
  pstcMftInternData->pfnIcu1Callback  = NULL;
  pstcMftInternData->pfnIcu2Callback  = NULL;
  pstcMftInternData->pfnIcu3Callback  = NULL;
  pstcMftInternData->pfnPpg0Callback  = NULL;
  pstcMftInternData->pfnPpg2Callback  = NULL;
  pstcMftInternData->pfnPpg4Callback  = NULL;
  
  // FRT0 - Clock setting
  switch (pstcFrtOcuConfig->enClock0)
  {
    case MftPclk1:
      unTCSA0.CLK = 0;
      break;
    case MftPclk2:
      unTCSA0.CLK = 1;
      break;   
    case MftPclk4:
      unTCSA0.CLK = 2;
      break;
    case MftPclk8:
      unTCSA0.CLK = 3;
      break; 
    case MftPclk16:
      unTCSA0.CLK = 4;
      break;
    case MftPclk32:
      unTCSA0.CLK = 5;
      break;   
    case MftPclk64:
      unTCSA0.CLK = 6;
      break;
    case MftPclk128:
      unTCSA0.CLK = 7;
      break; 
    case MftPclk256:
      unTCSA0.CLK = 8;
      break; 
    default:
      return ErrorInvalidParameter;
  }
  
  // FRT1 - Clock setting
  switch (pstcFrtOcuConfig->enClock1)
  {
    case MftPclk1:
      unTCSA1.CLK = 0;
      break;
    case MftPclk2:
      unTCSA1.CLK = 1;
      break;   
    case MftPclk4:
      unTCSA1.CLK = 2;
      break;
    case MftPclk8:
      unTCSA1.CLK = 3;
      break; 
    case MftPclk16:
      unTCSA1.CLK = 4;
      break;
    case MftPclk32:
      unTCSA1.CLK = 5;
      break;   
    case MftPclk64:
      unTCSA1.CLK = 6;
      break;
    case MftPclk128:
      unTCSA1.CLK = 7;
      break; 
    case MftPclk256:
      unTCSA1.CLK = 8;
      break; 
    default:
      return ErrorInvalidParameter;
  }
  
  // FRT2 - Clock setting
  switch (pstcFrtOcuConfig->enClock2)
  {
    case MftPclk1:
      unTCSA2.CLK = 0;
      break;
    case MftPclk2:
      unTCSA2.CLK = 1;
      break;   
    case MftPclk4:
      unTCSA2.CLK = 2;
      break;
    case MftPclk8:
      unTCSA2.CLK = 3;
      break; 
    case MftPclk16:
      unTCSA2.CLK = 4;
      break;
    case MftPclk32:
      unTCSA2.CLK = 5;
      break;   
    case MftPclk64:
      unTCSA2.CLK = 6;
      break;
    case MftPclk128:
      unTCSA2.CLK = 7;
      break; 
    case MftPclk256:
      unTCSA2.CLK = 8;
      break; 
    default:
      return ErrorInvalidParameter;
  }
  
  // FRT0 - Operation Mode
  switch (pstcFrtOcuConfig->enMode0)
  {
    case MftUpCount:
      unTCSA0.MODE = 0;
      break;
    case MftUpDownCount:
      unTCSA0.MODE = 1;   
      break;
    default:
      return ErrorInvalidParameter;
  }

  // FRT1 - Operation Mode
  switch (pstcFrtOcuConfig->enMode1)
  {
    case MftUpCount:
      unTCSA1.MODE = 0;
      break;
    case MftUpDownCount:
      unTCSA1.MODE = 1;   
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  // FRT2 - Operation Mode
  switch (pstcFrtOcuConfig->enMode2)
  {
    case MftUpCount:
      unTCSA2.MODE = 0;
      break;
    case MftUpDownCount:
      unTCSA2.MODE = 1;   
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  // FRT - TCCP Buffering
  unTCSA0.BFE = ((pstcFrtOcuConfig->bTccp0BufferEnable) == TRUE) ? 1 : 0 ;
  unTCSA1.BFE = ((pstcFrtOcuConfig->bTccp1BufferEnable) == TRUE) ? 1 : 0 ;
  unTCSA2.BFE = ((pstcFrtOcuConfig->bTccp2BufferEnable) == TRUE) ? 1 : 0 ;

  // FRT - TCSA.TCLR Interrupt Enable
  unTCSA0.ICRE = ((pstcFrtOcuConfig->bTcsa0IclrInterruptEnable) == TRUE) ? 1 : 0 ;
  unTCSA1.ICRE = ((pstcFrtOcuConfig->bTcsa1IclrInterruptEnable) == TRUE) ? 1 : 0 ;
  unTCSA2.ICRE = ((pstcFrtOcuConfig->bTcsa2IclrInterruptEnable) == TRUE) ? 1 : 0 ;
  
  // FRT - TCSA.IRQZ Interrupt Enable
  unTCSA0.IRQZE = ((pstcFrtOcuConfig->bTcsa0IrqzfInterruptEnable) == TRUE) ? 1 : 0 ;
  unTCSA1.IRQZE = ((pstcFrtOcuConfig->bTcsa1IrqzfInterruptEnable) == TRUE) ? 1 : 0 ;
  unTCSA2.IRQZE = ((pstcFrtOcuConfig->bTcsa2IrqzfInterruptEnable) == TRUE) ? 1 : 0 ;

  // FRT - External Clock Enable
  unTCSA0.ECKE = ((pstcFrtOcuConfig->bExternalClock0Enable) == TRUE) ? 1 : 0 ;
  unTCSA1.ECKE = ((pstcFrtOcuConfig->bExternalClock1Enable) == TRUE) ? 1 : 0 ;
  unTCSA2.ECKE = ((pstcFrtOcuConfig->bExternalClock2Enable) == TRUE) ? 1 : 0 ;
  
  // FRT - Stop State
  unTCSA0.STOP = 1;
  unTCSA1.STOP = 1;
  unTCSA2.STOP = 1;
  
  pstcMftInternData->pfnFrt0PeakCallback = pstcFrtOcuConfig->pfnFrt0PeakCallback; 
  pstcMftInternData->pfnFrt1PeakCallback = pstcFrtOcuConfig->pfnFrt1PeakCallback; 
  pstcMftInternData->pfnFrt2PeakCallback = pstcFrtOcuConfig->pfnFrt2PeakCallback; 
  pstcMftInternData->pfnFrt0ZeroCallback = pstcFrtOcuConfig->pfnFrt0ZeroCallback; 
  pstcMftInternData->pfnFrt1ZeroCallback = pstcFrtOcuConfig->pfnFrt1ZeroCallback; 
  pstcMftInternData->pfnFrt2ZeroCallback = pstcFrtOcuConfig->pfnFrt2ZeroCallback; 
  
  if (((pstcFrtOcuConfig->bTcsa0IclrInterruptEnable)  == TRUE) ||
      ((pstcFrtOcuConfig->bTcsa1IclrInterruptEnable)  == TRUE) ||
      ((pstcFrtOcuConfig->bTcsa2IclrInterruptEnable)  == TRUE) ||
      ((pstcFrtOcuConfig->bTcsa0IrqzfInterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bTcsa1IrqzfInterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bTcsa2IrqzfInterruptEnable) == TRUE)
     )
  {
    MftInitFrtInterrupt();
  }
  
  // OCU0 - Connection to FRTn
  switch(pstcFrtOcuConfig->enFrtOcu0Connection)
  {
    case MftFrt0Ocu:
      unOCFS10.FSO0 = 0;
      break;
    case MftFrt1Ocu:
      unOCFS10.FSO0 = 1;
      break;
    case MftFrt2Ocu:
      unOCFS10.FSO0 = 2;
      break;
    case MftOcuFrtMft1:
      unOCFS10.FSO0 = 3;  
      break;
    case MftOcuFrtMft2:
      unOCFS10.FSO0 = 4; 
      break;
    default:
      return ErrorInvalidParameter;
  }

  // OCU1 - Connection to FRTn
  switch(pstcFrtOcuConfig->enFrtOcu1Connection)
  {
    case MftFrt0Ocu:
      unOCFS10.FSO1 = 0;
      break;
    case MftFrt1Ocu:
      unOCFS10.FSO1 = 1;
      break;
    case MftFrt2Ocu:
      unOCFS10.FSO1 = 2;
      break;
    case MftOcuFrtMft1:
      unOCFS10.FSO1 = 3;  
      break;
    case MftOcuFrtMft2:
      unOCFS10.FSO1 = 4; 
      break;
    default:
      return ErrorInvalidParameter;
  }

  // OCU2 - Connection to FRTn
  switch(pstcFrtOcuConfig->enFrtOcu2Connection)
  {
    case MftFrt0Ocu:
      unOCFS32.FSO0 = 0;
      break;
    case MftFrt1Ocu:
      unOCFS32.FSO0 = 1;
      break;
    case MftFrt2Ocu:
      unOCFS32.FSO0 = 2;
      break;
    case MftOcuFrtMft1:
      unOCFS32.FSO0 = 3;  
      break;
    case MftOcuFrtMft2:
      unOCFS32.FSO0 = 4; 
      break;
    default:
      return ErrorInvalidParameter;
  }

  // OCU3 - Connection to FRTn
  switch(pstcFrtOcuConfig->enFrtOcu3Connection)
  {
    case MftFrt0Ocu:
      unOCFS32.FSO1 = 0;
      break;
    case MftFrt1Ocu:
      unOCFS32.FSO1 = 1;
      break;
    case MftFrt2Ocu:
      unOCFS32.FSO1 = 2;
      break;
    case MftOcuFrtMft1:
      unOCFS32.FSO1 = 3;  
      break;
    case MftOcuFrtMft2:
      unOCFS32.FSO1 = 4; 
      break;
    default:
      return ErrorInvalidParameter;
  }
 
  // OCU4 - Connection to FRTn
  switch(pstcFrtOcuConfig->enFrtOcu4Connection)
  {
    case MftFrt0Ocu:
      unOCFS54.FSO0 = 0;
      break;
    case MftFrt1Ocu:
      unOCFS54.FSO0 = 1;
      break;
    case MftFrt2Ocu:
      unOCFS54.FSO0 = 2;
      break;
    case MftOcuFrtMft1:
      unOCFS54.FSO0 = 3;  
      break;
    case MftOcuFrtMft2:
      unOCFS54.FSO0 = 4; 
      break;
    default:
      return ErrorInvalidParameter;
  }

  // OCU5 - Connection to FRTn
  switch(pstcFrtOcuConfig->enFrtOcu5Connection)
  {
    case MftFrt0Ocu:
      unOCFS54.FSO1 = 0;
      break;
    case MftFrt1Ocu:
      unOCFS54.FSO1 = 1;
      break;
    case MftFrt2Ocu:
      unOCFS54.FSO1 = 2;
      break;
    case MftOcuFrtMft1:
      unOCFS54.FSO1 = 3;  
      break;
    case MftOcuFrtMft2:
      unOCFS54.FSO1 = 4; 
      break;
    default:
      return ErrorInvalidParameter;
  }  
 
  // OCU - Enable
  unOCSA10.CST0 = ((pstcFrtOcuConfig->bOcu0Enable) == TRUE) ? 1 : 0 ;
  unOCSA10.CST1 = ((pstcFrtOcuConfig->bOcu1Enable) == TRUE) ? 1 : 0 ;
  unOCSA32.CST2 = ((pstcFrtOcuConfig->bOcu2Enable) == TRUE) ? 1 : 0 ;
  unOCSA32.CST3 = ((pstcFrtOcuConfig->bOcu3Enable) == TRUE) ? 1 : 0 ;
  unOCSA54.CST4 = ((pstcFrtOcuConfig->bOcu4Enable) == TRUE) ? 1 : 0 ;
  unOCSA54.CST5 = ((pstcFrtOcuConfig->bOcu5Enable) == TRUE) ? 1 : 0 ;
  
  // OCU - OCCP Buffering Enable
  unOCSA10.BDIS0 = ((pstcFrtOcuConfig->bOccp0BufferEnable) == TRUE) ? 1 : 0 ;
  unOCSA10.BDIS1 = ((pstcFrtOcuConfig->bOccp1BufferEnable) == TRUE) ? 1 : 0 ;
  unOCSA32.BDIS2 = ((pstcFrtOcuConfig->bOccp2BufferEnable) == TRUE) ? 1 : 0 ;
  unOCSA32.BDIS3 = ((pstcFrtOcuConfig->bOccp3BufferEnable) == TRUE) ? 1 : 0 ;
  unOCSA54.BDIS4 = ((pstcFrtOcuConfig->bOccp4BufferEnable) == TRUE) ? 1 : 0 ;
  unOCSA54.BDIS5 = ((pstcFrtOcuConfig->bOccp5BufferEnable) == TRUE) ? 1 : 0 ;
  
  // OCU - OCSA.IOP Interrupt Enable
  unOCSA10.IOE0 = ((pstcFrtOcuConfig->bOcsaIop0InterruptEnable) == TRUE) ? 1 : 0 ;
  unOCSA10.IOE1 = ((pstcFrtOcuConfig->bOcsaIop1InterruptEnable) == TRUE) ? 1 : 0 ;
  unOCSA32.IOE2 = ((pstcFrtOcuConfig->bOcsaIop2InterruptEnable) == TRUE) ? 1 : 0 ;
  unOCSA32.IOE3 = ((pstcFrtOcuConfig->bOcsaIop3InterruptEnable) == TRUE) ? 1 : 0 ;
  unOCSA54.IOE4 = ((pstcFrtOcuConfig->bOcsaIop4InterruptEnable) == TRUE) ? 1 : 0 ;
  unOCSA54.IOE5 = ((pstcFrtOcuConfig->bOcsaIop5InterruptEnable) == TRUE) ? 1 : 0 ;
  
  // OCU - RT Output High Level Enable
  unOCSB10.OTD0 = ((pstcFrtOcuConfig->bRt0HighLevel) == TRUE) ? 1 : 0 ;
  unOCSB10.OTD1 = ((pstcFrtOcuConfig->bRt1HighLevel) == TRUE) ? 1 : 0 ;
  unOCSB32.OTD2 = ((pstcFrtOcuConfig->bRt2HighLevel) == TRUE) ? 1 : 0 ;
  unOCSB32.OTD3 = ((pstcFrtOcuConfig->bRt3HighLevel) == TRUE) ? 1 : 0 ;
  unOCSB54.OTD4 = ((pstcFrtOcuConfig->bRt4HighLevel) == TRUE) ? 1 : 0 ;
  unOCSB54.OTD5 = ((pstcFrtOcuConfig->bRt5HighLevel) == TRUE) ? 1 : 0 ;

  // OCU - CMOD
  unOCSB10.CMOD = ((pstcFrtOcuConfig->bCmod01) == TRUE) ? 1 : 0 ;
  unOCSB32.CMOD = ((pstcFrtOcuConfig->bCmod23) == TRUE) ? 1 : 0 ;
  unOCSB54.CMOD = ((pstcFrtOcuConfig->bCmod45) == TRUE) ? 1 : 0 ;
  
  // OCU - Buffer Transfer on Peak Detection Enable
  unOCSB10.BTS0 = ((pstcFrtOcuConfig->bOccp0BufTransferPeakDetection) == TRUE) ? 1 : 0 ;
  unOCSB10.BTS1 = ((pstcFrtOcuConfig->bOccp1BufTransferPeakDetection) == TRUE) ? 1 : 0 ;
  unOCSB32.BTS2 = ((pstcFrtOcuConfig->bOccp2BufTransferPeakDetection) == TRUE) ? 1 : 0 ;
  unOCSB32.BTS3 = ((pstcFrtOcuConfig->bOccp3BufTransferPeakDetection) == TRUE) ? 1 : 0 ;
  unOCSB54.BTS4 = ((pstcFrtOcuConfig->bOccp4BufTransferPeakDetection) == TRUE) ? 1 : 0 ;
  unOCSB54.BTS5 = ((pstcFrtOcuConfig->bOccp5BufTransferPeakDetection) == TRUE) ? 1 : 0 ; 
  
  // OCU - Mode
  unOCSC.MOD0 = ((pstcFrtOcuConfig->bMod0) == TRUE) ? 1 : 0 ; 
  unOCSC.MOD1 = ((pstcFrtOcuConfig->bMod1) == TRUE) ? 1 : 0 ; 
  unOCSC.MOD2 = ((pstcFrtOcuConfig->bMod2) == TRUE) ? 1 : 0 ; 
  unOCSC.MOD3 = ((pstcFrtOcuConfig->bMod3) == TRUE) ? 1 : 0 ; 
  unOCSC.MOD4 = ((pstcFrtOcuConfig->bMod4) == TRUE) ? 1 : 0 ; 
  unOCSC.MOD5 = ((pstcFrtOcuConfig->bMod5) == TRUE) ? 1 : 0 ; 
  
  pstcMftInternData->pfnOcu0Callback  = pstcFrtOcuConfig->pfnOcu0Callback;
  pstcMftInternData->pfnOcu1Callback  = pstcFrtOcuConfig->pfnOcu1Callback;
  pstcMftInternData->pfnOcu2Callback  = pstcFrtOcuConfig->pfnOcu2Callback;
  pstcMftInternData->pfnOcu3Callback  = pstcFrtOcuConfig->pfnOcu3Callback;
  pstcMftInternData->pfnOcu4Callback  = pstcFrtOcuConfig->pfnOcu4Callback;
  pstcMftInternData->pfnOcu5Callback  = pstcFrtOcuConfig->pfnOcu5Callback;
  
  if (((pstcFrtOcuConfig->bOcsaIop0InterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bOcsaIop1InterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bOcsaIop2InterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bOcsaIop3InterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bOcsaIop4InterruptEnable) == TRUE) ||
      ((pstcFrtOcuConfig->bOcsaIop5InterruptEnable) == TRUE)
     )
  {
    MftInitOcuInterrupt();
  }
  
  // WFG
  if (pstcWfgConfig != NULL)
  {
    // WFG01 - Clock
    switch(pstcWfgConfig->enWfgClock01)
    {
      case MftWfgPclk1:
        unWFSA10.DCK = 0;
        break;
      case MftWfgPclk2:
        unWFSA10.DCK = 1;
        break;
      case MftWfgPclk4:
        unWFSA10.DCK = 2;
        break;
      case MftWfgPclk8:
        unWFSA10.DCK = 3;
        break;
      case MftWfgPclk16:
        unWFSA10.DCK = 4;
        break;
      case MftWfgPclk32:
        unWFSA10.DCK = 5;
        break;
      case MftWfgPclk64:
        unWFSA10.DCK = 6;
        break;
      default:
        return ErrorInvalidParameter;
    }

    // WFG23 - Clock
    switch(pstcWfgConfig->enWfgClock23)
    {
      case MftWfgPclk1:
        unWFSA32.DCK = 0;
        break;
      case MftWfgPclk2:
        unWFSA32.DCK = 1;
        break;
      case MftWfgPclk4:
        unWFSA32.DCK = 2;
        break;
      case MftWfgPclk8:
        unWFSA32.DCK = 3;
        break;
      case MftWfgPclk16:
        unWFSA32.DCK = 4;
        break;
      case MftWfgPclk32:
        unWFSA32.DCK = 5;
        break;
      case MftWfgPclk64:
        unWFSA32.DCK = 6;
        break;
      default:
        return ErrorInvalidParameter;      
    }

    // WFG45 - Clock
    switch(pstcWfgConfig->enWfgClock45)
    {
      case MftWfgPclk1:
        unWFSA54.DCK = 0;
        break;
      case MftWfgPclk2:
        unWFSA54.DCK = 1;
        break;
      case MftWfgPclk4:
        unWFSA54.DCK = 2;
        break;
      case MftWfgPclk8:
        unWFSA54.DCK = 3;
        break;
      case MftWfgPclk16:
        unWFSA54.DCK = 4;
        break;
      case MftWfgPclk32:
        unWFSA54.DCK = 5;
        break;
      case MftWfgPclk64:
        unWFSA54.DCK = 6;
        break;
      default:
        return ErrorInvalidParameter;      
    }
    
    // WFG01 - Operation Mode
    switch(pstcWfgConfig->enWfgMode01)
    {
      case MftWfgThroughMode:
        unWFSA10.TMD = 0;
        break;
      case MftWfgRtPpgMode:
        unWFSA10.TMD = 1;
        break;
      case MftWfgTimerPpgMode:
        unWFSA10.TMD = 2;
        break;
      case MftWfgRtDeadTimerMode:
        unWFSA10.TMD = 4;
        break;
      case MftWfgPpgDeadTimerMode:
        unWFSA10.TMD = 7;
        break;
      default:
        return ErrorInvalidParameter;     
    }
 
    // WFG23 - Operation Mode
    switch(pstcWfgConfig->enWfgMode23)
    {
      case MftWfgThroughMode:
        unWFSA32.TMD = 0;
        break;
      case MftWfgRtPpgMode:
        unWFSA32.TMD = 1;
        break;
      case MftWfgTimerPpgMode:
        unWFSA32.TMD = 2;
        break;
      case MftWfgRtDeadTimerMode:
        unWFSA32.TMD = 4;
        break;
      case MftWfgPpgDeadTimerMode:
        unWFSA32.TMD = 7;
        break;
      default:
        return ErrorInvalidParameter;     
    }
    
    // WFG45 - Operation Mode
    switch(pstcWfgConfig->enWfgMode45)
    {
      case MftWfgThroughMode:
        unWFSA54.TMD = 0;
        break;
      case MftWfgRtPpgMode:
        unWFSA54.TMD = 1;
        break;
      case MftWfgTimerPpgMode:
        unWFSA54.TMD = 2;
        break;
      case MftWfgRtDeadTimerMode:
        unWFSA54.TMD = 4;
        break;
      case MftWfgPpgDeadTimerMode:
        unWFSA54.TMD = 7;
        break;
      default:
        return ErrorInvalidParameter;     
    }
    
    // WFG01 - GATEn Signal Output
    switch(pstcWfgConfig->enWfgGate01)
    {
      case MftWfgLowLevel:
        unWFSA10.GTEN = 0;
        break;
      case MftWfgRt0Flag0:
        unWFSA10.GTEN = 1;
        break;     
      case MftWfgRt1Flag1:
        unWFSA10.GTEN = 2;
        break;   
      case MftWfgRt10Flag10:
        unWFSA10.GTEN = 3;
        break;
      default:
        return ErrorInvalidParameter;  
    }
    
    // WFG23 - GATEn Signal Output
    switch(pstcWfgConfig->enWfgGate23)
    {
      case MftWfgLowLevel:
        unWFSA32.GTEN = 0;
        break;
      case MftWfgRt0Flag0:
        unWFSA32.GTEN = 1;
        break;     
      case MftWfgRt1Flag1:
        unWFSA32.GTEN = 2;
        break;   
      case MftWfgRt10Flag10:
        unWFSA32.GTEN = 3;
        break;
      default:
        return ErrorInvalidParameter;  
    }
    
    // WFG45 - GATEn Signal Output
    switch(pstcWfgConfig->enWfgGate45)
    {
      case MftWfgLowLevel:
        unWFSA54.GTEN = 0;
        break;
      case MftWfgRt0Flag0:
        unWFSA54.GTEN = 1;
        break;     
      case MftWfgRt1Flag1:
        unWFSA54.GTEN = 2;
        break;   
      case MftWfgRt10Flag10:
        unWFSA54.GTEN = 3;
        break;
      default:
        return ErrorInvalidParameter;  
    }    
    
    // WFG01 - GATEn to PPGn Connection
    switch(pstcWfgConfig->enWfgPpgSel01)
    {
      case MftWfgGatePpg0:
        unWFSA10.PSEL = 0;
        break;
      case MftWfgGatePpg2:
        unWFSA10.PSEL = 1;
        break;     
      case MftWfgGatePpg4:
        unWFSA10.PSEL = 2;
        break;   
      default:
        return ErrorInvalidParameter;  
    }   
     
    // WFG23 - GATEn to PPGn Connection
    switch(pstcWfgConfig->enWfgPpgSel23)
    {
      case MftWfgGatePpg0:
        unWFSA32.PSEL = 0;
        break;
      case MftWfgGatePpg2:
        unWFSA32.PSEL = 1;
        break;     
      case MftWfgGatePpg4:
        unWFSA32.PSEL = 2;
        break;   
      default:
        return ErrorInvalidParameter;  
    }   
     
    // WFG45 - GATEn to PPGn Connection
    switch(pstcWfgConfig->enWfgPpgSel45)
    {
      case MftWfgGatePpg0:
        unWFSA54.PSEL = 0;
        break;
      case MftWfgGatePpg2:
        unWFSA54.PSEL = 1;
        break;     
      case MftWfgGatePpg4:
        unWFSA54.PSEL = 2;
        break;   
      default:
        return ErrorInvalidParameter;  
    }     
    
    // WFG01 - RT/PPG Enable
    switch(pstcWfgConfig->enWfgPpgEnable01)
    {
      case MftWfgRt1Rt0:
        unWFSA10.PGEN = 0;
        break;
      case MftWfgRt1Ppg:
        unWFSA10.PGEN = 1;
        break;     
      case MftWfgPpgRt0:
        unWFSA10.PGEN = 2;
        break;   
      case MftWfgPpgPpg:
        unWFSA10.PGEN = 3;
        break; 
      default:
        return ErrorInvalidParameter;  
    }    
    
    // WFG23 - RT/PPG Enable
    switch(pstcWfgConfig->enWfgPpgEnable23)
    {
      case MftWfgRt1Rt0:
        unWFSA32.PGEN = 0;
        break;
      case MftWfgRt1Ppg:
        unWFSA32.PGEN = 1;
        break;     
      case MftWfgPpgRt0:
        unWFSA32.PGEN = 2;
        break;   
      case MftWfgPpgPpg:
        unWFSA32.PGEN = 3;
        break; 
      default:
        return ErrorInvalidParameter;  
    } 
     
    // WFG45 - RT/PPG Enable
    switch(pstcWfgConfig->enWfgPpgEnable45)
    {
      case MftWfgRt1Rt0:
        unWFSA54.PGEN = 0;
        break;
      case MftWfgRt1Ppg:
        unWFSA54.PGEN = 1;
        break;     
      case MftWfgPpgRt0:
        unWFSA54.PGEN = 2;
        break;   
      case MftWfgPpgPpg:
        unWFSA54.PGEN = 3;
        break; 
      default:
        return ErrorInvalidParameter;  
    } 
    
    unWFSA10.DMOD = ((pstcWfgConfig->bNonOverlapHighPolarity0) == TRUE) ? 1 : 0 ;
    unWFSA32.DMOD = ((pstcWfgConfig->bNonOverlapHighPolarity1) == TRUE) ? 1 : 0 ;
    unWFSA54.DMOD = ((pstcWfgConfig->bNonOverlapHighPolarity2) == TRUE) ? 1 : 0 ;
    
    u16WFTM10 = pstcWfgConfig->u16Wfg01TimerValue;
    u16WFTM32 = pstcWfgConfig->u16Wfg23TimerValue;
    u16WFTM54 = pstcWfgConfig->u16Wfg45TimerValue;
    
    pstcMftInternData->pfnWfg01Callback = pstcWfgConfig->pfnWfg01Callback;
    pstcMftInternData->pfnWfg23Callback = pstcWfgConfig->pfnWfg23Callback;
    pstcMftInternData->pfnWfg45Callback = pstcWfgConfig->pfnWfg45Callback;
    
    if (pstcWfgConfig->bWfgInterruptEnable == TRUE)
    {
      MftInitWfgInterrupt();
    }
  }
  else // if (pstcWfgConfig != NULL)
  {
    u16WFTM10 = 0;
    u16WFTM32 = 0;
    u16WFTM54 = 0;    
  } // else if (pstcWfgConfig != NULL)
  
  // NZCL - Cancel Time
  if (pstcNzclConfig != NULL)
  {
    switch(pstcNzclConfig->enNoiceCancelTime)
    {
      case MftNzclNoNoiseCancel:
        unNZCL.MWS = 0;
        break;
      case MftNzclNoiseCancel4Pclk:
        unNZCL.MWS = 1;
        break;     
      case MftNzclNoiseCancel8Pclk:
        unNZCL.MWS = 2;
        break; 
      case MftNzclNoiseCancel16Pclk:
        unNZCL.MWS = 3;
        break; 
      case MftNzclNoiseCancel32Pclk:
        unNZCL.MWS = 4;
        break; 
      default:
        return ErrorInvalidParameter; 
    }
    
    // NZCL - Interrupt enable
    unNZCL.DTIE = ((pstcNzclConfig->bDtifInterruptEnable) == TRUE) ? 1 : 0 ;
    
    // NZCL Callback
    pstcMftInternData->pfnNzclCallback = pstcNzclConfig->pfnNzclCallback;
  } // if (pstcNzclConfig != NULL)
  
  // ICU
  if (pstcIcuConfig != NULL)
  {
    // ICU0 - Connection to FRTn
    switch(pstcIcuConfig->enIcu0Connect)
    {
      case MftIcuFrt0:
        unICFS10.FSI0 = 0;
        break;
      case MftIcuFrt1:
        unICFS10.FSI0 = 1;
        break;
      case MftIcuFrt2:
        unICFS10.FSI0 = 2;
        break;
      case MftIcuFrtMft1:
        unICFS10.FSI0 = 3;
        break;
      case MftIcuFrtMft2:
        unICFS10.FSI0 = 4;
        break;
      default:
        return ErrorInvalidParameter; 
    }
 
    // ICU1 - Connection to FRTn
    switch(pstcIcuConfig->enIcu1Connect)
    {
      case MftIcuFrt0:
        unICFS10.FSI1 = 0;
        break;
      case MftIcuFrt1:
        unICFS10.FSI1 = 1;
        break;
      case MftIcuFrt2:
        unICFS10.FSI1 = 2;
        break;
      case MftIcuFrtMft1:
        unICFS10.FSI1 = 3;
        break;
      case MftIcuFrtMft2:
        unICFS10.FSI1 = 4;
        break;
      default:
        return ErrorInvalidParameter; 
    }
    
    // ICU2 - Connection to FRTn
    switch(pstcIcuConfig->enIcu2Connect)
    {
      case MftIcuFrt0:
        unICFS32.FSI0 = 0;
        break;
      case MftIcuFrt1:
        unICFS32.FSI0 = 1;
        break;
      case MftIcuFrt2:
        unICFS32.FSI0 = 2;
        break;
      case MftIcuFrtMft1:
        unICFS32.FSI0 = 3;
        break;
      case MftIcuFrtMft2:
        unICFS32.FSI0 = 4;
        break;
      default:
        return ErrorInvalidParameter; 
    }
 
    // ICU3 - Connection to FRTn
    switch(pstcIcuConfig->enIcu3Connect)
    {
      case MftIcuFrt0:
        unICFS32.FSI1 = 0;
        break;
      case MftIcuFrt1:
        unICFS32.FSI1 = 1;
        break;
      case MftIcuFrt2:
        unICFS32.FSI1 = 2;
        break;
      case MftIcuFrtMft1:
        unICFS32.FSI1 = 3;
        break;
      case MftIcuFrtMft2:
        unICFS32.FSI1 = 4;
        break;
      default:
        return ErrorInvalidParameter; 
    }
    
    // ICU - Interrupt Enable
    unICSA10.ICE0 = ((pstcIcuConfig->bIcu0InterruptEnable) == TRUE) ? 1 : 0 ;
    unICSA10.ICE1 = ((pstcIcuConfig->bIcu1InterruptEnable) == TRUE) ? 1 : 0 ;
    unICSA32.ICE2 = ((pstcIcuConfig->bIcu2InterruptEnable) == TRUE) ? 1 : 0 ;
    unICSA32.ICE3 = ((pstcIcuConfig->bIcu3InterruptEnable) == TRUE) ? 1 : 0 ;
    
    // ICU - Callback Pointers
    pstcMftInternData->pfnIcu0Callback = pstcIcuConfig->pfnIcu0Callback;
    pstcMftInternData->pfnIcu1Callback = pstcIcuConfig->pfnIcu1Callback;
    pstcMftInternData->pfnIcu2Callback = pstcIcuConfig->pfnIcu2Callback;
    pstcMftInternData->pfnIcu3Callback = pstcIcuConfig->pfnIcu3Callback;
    
    if(((pstcIcuConfig->bIcu0InterruptEnable) == TRUE) ||
       ((pstcIcuConfig->bIcu1InterruptEnable) == TRUE) ||
       ((pstcIcuConfig->bIcu2InterruptEnable) == TRUE) ||
       ((pstcIcuConfig->bIcu3InterruptEnable) == TRUE)
      )
    {
      MftInitIcuInterrupt();
    }
  } // if (pstcIcuConfig != NULL)
  
  // ADCMP
  if (pstcAdcmpConfig != NULL)
  {
    // ADCMP0 - FRTn
    switch(pstcAdcmpConfig->enAdcmp0Frt)
    {
      case MftAdcCmpDisable:
        unACSA.CE0 = 0;
        break;
      case MftAdcCmpFrt0:
        unACSA.CE0 = 1;
        break;
      case MftAdcCmpFrt1:
        unACSA.CE0 = 2;
        break;
      case MftAdcCmpFrt2:
        unACSA.CE0 = 3;
        break;
      default:
        return ErrorInvalidParameter;      
    }
    
    // ADCMP1 - FRTn
    switch(pstcAdcmpConfig->enAdcmp1Frt)
    {
      case MftAdcCmpDisable:
        unACSA.CE1 = 0;
        break;
      case MftAdcCmpFrt0:
        unACSA.CE1 = 1;
        break;
      case MftAdcCmpFrt1:
        unACSA.CE1 = 2;
        break;
      case MftAdcCmpFrt2:
        unACSA.CE1 = 3;
        break;
      default:
        return ErrorInvalidParameter;      
    }   
    
    // ADCMP2 - FRTn
    switch(pstcAdcmpConfig->enAdcmp2Frt)
    {
      case MftAdcCmpDisable:
        unACSA.CE2 = 0;
        break;
      case MftAdcCmpFrt0:
        unACSA.CE2 = 1;
        break;
      case MftAdcCmpFrt1:
        unACSA.CE2 = 2;
        break;
      case MftAdcCmpFrt2:
        unACSA.CE2 = 3;
        break;
      default:
        return ErrorInvalidParameter;      
    }

    // ADCMP0 - Count mode
    switch(pstcAdcmpConfig->enAdcmp0Count)
    {
      case MftAdcCmpUpPeakDown:
        unACSA.SEL0 = 0;
        break;
      case MftAdcCmpUp:
        unACSA.SEL0 = 1;
        break;
      case MftAdcCmpPeakDown:
        unACSA.SEL0 = 2;
        break;
      case MftAdcCmpUpPeakDown2:
        unACSA.SEL0 = 3;
        break;
      default:
        return ErrorInvalidParameter;      
    }

    // ADCMP1 - Count mode
    switch(pstcAdcmpConfig->enAdcmp1Count)
    {
      case MftAdcCmpUpPeakDown:
        unACSA.SEL1 = 0;
        break;
      case MftAdcCmpUp:
        unACSA.SEL1 = 1;
        break;
      case MftAdcCmpPeakDown:
        unACSA.SEL1 = 2;
        break;
      case MftAdcCmpUpPeakDown2:
        unACSA.SEL1 = 3;
        break;
      default:
        return ErrorInvalidParameter;      
    }

    // ADCMP2 - Count mode
    switch(pstcAdcmpConfig->enAdcmp2Count)
    {
      case MftAdcCmpUpPeakDown:
        unACSA.SEL2 = 0;
        break;
      case MftAdcCmpUp:
        unACSA.SEL2 = 1;
        break;
      case MftAdcCmpPeakDown:
        unACSA.SEL2 = 2;
        break;
      case MftAdcCmpUpPeakDown2:
        unACSA.SEL2 = 3;
        break;
      default:
        return ErrorInvalidParameter;      
    }
    
    // ADCMPn - Buffer disable
    unACSB.BDIS0 = ((pstcAdcmpConfig->bAdcmp0BufferDisable) == TRUE) ? 1 : 0 ;
    unACSB.BDIS1 = ((pstcAdcmpConfig->bAdcmp1BufferDisable) == TRUE) ? 1 : 0 ;
    unACSB.BDIS2 = ((pstcAdcmpConfig->bAdcmp2BufferDisable) == TRUE) ? 1 : 0 ;
    
    // ADCMPn - Buffer transfer mode
    unACSB.BTS0 = ((pstcAdcmpConfig->bAdcmp0PeakValueEnable) == TRUE) ? 1 : 0 ;
    unACSB.BTS1 = ((pstcAdcmpConfig->bAdcmp1PeakValueEnable) == TRUE) ? 1 : 0 ;
    unACSB.BTS2 = ((pstcAdcmpConfig->bAdcmp2PeakValueEnable) == TRUE) ? 1 : 0 ;
    
    // ADCMP0 - Scan signal logic
    switch(pstcAdcmpConfig->enAdcmp0Start)
    {
      case MftAdcCmpAdcmp:
        unATSA.AD0S = 0;
        break;
      case MftAdcCmpFrt02:
        unATSA.AD0S = 1;
        break;
      default:
        return ErrorInvalidParameter;      
    }  
    
    // ADCMP1 - Scan signal logic
    switch(pstcAdcmpConfig->enAdcmp1Start)
    {
      case MftAdcCmpAdcmp:
        unATSA.AD1S = 0;
        break;
      case MftAdcCmpFrt02:
        unATSA.AD1S = 1;
        break;
      default:
        return ErrorInvalidParameter;      
    } 
    
    // ADCMP2 - Scan signal logic
    switch(pstcAdcmpConfig->enAdcmp2Start)
    {
      case MftAdcCmpAdcmp:
        unATSA.AD2S = 0;
        break;
      case MftAdcCmpFrt02:
        unATSA.AD2S = 1;
        break;
      default:
        return ErrorInvalidParameter;      
    } 
    
    // ADCMP0 - Priority scan logic
    switch(pstcAdcmpConfig->enAdcmp0StartPrio)
    {
      case MftAdcCmpAdcmpPrio:
        unATSA.AD0P = 0;
        break;
      case MftAdcCmpFrt02Prio:
        unATSA.AD0P = 1;
        break;
      default:
        return ErrorInvalidParameter;      
    }  
    
    // ADCMP1 - Priority scan logic
    switch(pstcAdcmpConfig->enAdcmp1StartPrio)
    {
      case MftAdcCmpAdcmpPrio:
        unATSA.AD1P = 0;
        break;
      case MftAdcCmpFrt02Prio:
        unATSA.AD1P = 1;
        break;
      default:
        return ErrorInvalidParameter;      
    }  
    
    // ADCMP2 - Priority scan logic
    switch(pstcAdcmpConfig->enAdcmp2StartPrio)
    {
      case MftAdcCmpAdcmpPrio:
        unATSA.AD2P = 0;
        break;
      case MftAdcCmpFrt02Prio:
        unATSA.AD2P = 1;
        break;
      default:
        return ErrorInvalidParameter;      
    }    
    
    // FRT0 ADC start at zero detection enable
    unTCSB0.AD0E = ((pstcAdcmpConfig->bFrt0Adc0StartZeroDetection) == TRUE) ? 1 : 0 ;
    unTCSB0.AD1E = ((pstcAdcmpConfig->bFrt0Adc1StartZeroDetection) == TRUE) ? 1 : 0 ;
    unTCSB0.AD2E = ((pstcAdcmpConfig->bFrt0Adc2StartZeroDetection) == TRUE) ? 1 : 0 ;

    // FRT1 ADC start at zero detection enable
    unTCSB1.AD0E = ((pstcAdcmpConfig->bFrt1Adc0StartZeroDetection) == TRUE) ? 1 : 0 ;
    unTCSB1.AD1E = ((pstcAdcmpConfig->bFrt1Adc1StartZeroDetection) == TRUE) ? 1 : 0 ;
    unTCSB1.AD2E = ((pstcAdcmpConfig->bFrt1Adc2StartZeroDetection) == TRUE) ? 1 : 0 ;

    // FRT2 ADC start at zero detection enable
    unTCSB2.AD0E = ((pstcAdcmpConfig->bFrt2Adc0StartZeroDetection) == TRUE) ? 1 : 0 ;
    unTCSB2.AD1E = ((pstcAdcmpConfig->bFrt2Adc1StartZeroDetection) == TRUE) ? 1 : 0 ;
    unTCSB2.AD2E = ((pstcAdcmpConfig->bFrt2Adc2StartZeroDetection) == TRUE) ? 1 : 0 ;    
  } // if (pstcAdcmpConfig != NULL)
  
  // PPG
  if (pstcPpgConfig != NULL)
  {
    // PPG0/2/4 or PPG8/10/12 - Compare Clock Prescaler
    switch(pstcPpgConfig->enPpgCompareClock)
    {
      case MftPpgPclk2:
        unTTCRn.CS00 = 0;
        unTTCRn.CS01 = 0;
        break;
      case MftPpgPclk8:
        unTTCRn.CS00 = 1;
        unTTCRn.CS01 = 0;
        break;
      case MftPpgPclk32:
        unTTCRn.CS00 = 0;
        unTTCRn.CS01 = 1;
        break;
      case MftPpgPclk64:
        unTTCRn.CS00 = 1;
        unTTCRn.CS01 = 1;
        break;
      default:
        return ErrorInvalidParameter;         
    }
    
    // PPG0 - Mode
    switch(pstcPpgConfig->enPpg0Mode)
    {
      case MftPpg8Bit:
        unPPGC0.MD0 = 0;
        unPPGC0.MD1 = 0;
        break;
      case MftPpg8plus8Bit:
        unPPGC0.MD0 = 1;
        unPPGC0.MD1 = 0;
        break;
      case MftPpg16Bit:
        unPPGC0.MD0 = 0;
        unPPGC0.MD1 = 1;
        break;
      default:              // 16 + 16 mode not implemented yet!
        return ErrorInvalidParameter;         
    }
    
    // PPG2 - Mode
    switch(pstcPpgConfig->enPpg2Mode)
    {
      case MftPpg8Bit:
        unPPGC2.MD0 = 0;
        unPPGC2.MD1 = 0;
        break;
      case MftPpg8plus8Bit:
        unPPGC2.MD0 = 1;
        unPPGC2.MD1 = 0;
        break;
      case MftPpg16Bit:
        unPPGC2.MD0 = 0;
        unPPGC2.MD1 = 1;
        break;
      default:              // 16 + 16 mode not implemented yet!
        return ErrorInvalidParameter;         
    }    
    
    // PPG4 - Mode
    switch(pstcPpgConfig->enPpg4Mode)
    {
      case MftPpg8Bit:
        unPPGC4.MD0 = 0;
        unPPGC4.MD1 = 0;
        break;
      case MftPpg8plus8Bit:
        unPPGC4.MD0 = 1;
        unPPGC4.MD1 = 0;
        break;
      case MftPpg16Bit:
        unPPGC4.MD0 = 0;
        unPPGC4.MD1 = 1;
        break;
      default:              // 16 + 16 mode not implemented yet!
        return ErrorInvalidParameter;         
    }       
    
    // PPG0 - Clock Prescaler
    switch(pstcPpgConfig->enPpg0Clock)
    {
      case MftPpgPclk2:
        unPPGC0.PCS0 = 0;
        unPPGC0.PCS1 = 0;
        break;
      case MftPpgPclk8:
        unPPGC0.PCS0 = 1;
        unPPGC0.PCS1 = 0;
        break;
      case MftPpgPclk32:
        unPPGC0.PCS0 = 0;
        unPPGC0.PCS1 = 1;
        break;
      case MftPpgPclk64:
        unPPGC0.PCS0 = 1;
        unPPGC0.PCS1 = 1;
        break;
      default:
        return ErrorInvalidParameter;         
    }   
    
    // PPG2 - Clock Prescaler
    switch(pstcPpgConfig->enPpg2Clock)
    {
      case MftPpgPclk2:
        unPPGC2.PCS0 = 0;
        unPPGC2.PCS1 = 0;
        break;
      case MftPpgPclk8:
        unPPGC2.PCS0 = 1;
        unPPGC2.PCS1 = 0;
        break;
      case MftPpgPclk32:
        unPPGC2.PCS0 = 0;
        unPPGC2.PCS1 = 1;
        break;
      case MftPpgPclk64:
        unPPGC2.PCS0 = 1;
        unPPGC2.PCS1 = 1;
        break;
      default:
        return ErrorInvalidParameter;         
    }
    
    // PPG4 - Clock Prescaler
    switch(pstcPpgConfig->enPpg4Clock)
    {
      case MftPpgPclk2:
        unPPGC4.PCS0 = 0;
        unPPGC4.PCS1 = 0;
        break;
      case MftPpgPclk8:
        unPPGC4.PCS0 = 1;
        unPPGC4.PCS1 = 0;
        break;
      case MftPpgPclk32:
        unPPGC4.PCS0 = 0;
        unPPGC4.PCS1 = 1;
        break;
      case MftPpgPclk64:
        unPPGC4.PCS0 = 1;
        unPPGC4.PCS1 = 1;
        break;
      default:
        return ErrorInvalidParameter;         
    }  
    
    // PPG0 trigger
    switch (pstcPpgConfig->enPpg0Trigger)
    {
    case MftPpgTimingGenerator:
      unPPGC0.TTRG   = 1;
      unGATEC0.STRG0 = 0;
      break;
    case MftPpgGateSignal:
      unPPGC0.TTRG   = 0;
      unGATEC0.STRG0 = 1;
      break;
    case MftPpgTriggerRegister:
      unPPGC0.TTRG   = 0;
      unGATEC0.STRG0 = 0;
      break;
    }

    // PPG2 trigger
    switch (pstcPpgConfig->enPpg2Trigger)
    {
    case MftPpgTimingGenerator:
      unPPGC2.TTRG   = 1;
      unGATEC0.STRG2 = 0;
      break;
    case MftPpgGateSignal:
      unPPGC2.TTRG   = 0;
      unGATEC0.STRG2 = 1;
      break;
    case MftPpgTriggerRegister:
      unPPGC2.TTRG   = 0;
      unGATEC0.STRG2 = 0;
      break;
    }
    
    // PPG4 trigger
    switch (pstcPpgConfig->enPpg4Trigger)
    {
    case MftPpgTimingGenerator:
      unPPGC4.TTRG   = 1;
      unGATEC4.STRG4 = 0;
      break;
    case MftPpgGateSignal:
      unPPGC4.TTRG   = 0;
      unGATEC4.STRG4 = 1;
      break;
    case MftPpgTriggerRegister:
      unPPGC4.TTRG   = 0;
      unGATEC4.STRG4 = 0;
      break;
    }
    
    // PPG - Start GATE level
    unGATEC0.EDGE0 = ((pstcPpgConfig->bPpg0StartLowLevel) == TRUE) ? 1 : 0 ;
    unGATEC0.EDGE2 = ((pstcPpgConfig->bPpg2StartLowLevel) == TRUE) ? 1 : 0 ;
    unGATEC4.EDGE4 = ((pstcPpgConfig->bPpg4StartLowLevel) == TRUE) ? 1 : 0 ;
    
    // PPG - Invert Polarity
    switch (i8MftInstanceNumber)
    {
      case 0:    
        unREVC0.REV00 = ((pstcPpgConfig->bPpg0InvertPolarity) == TRUE) ? 1 : 0;
        unREVC0.REV02 = ((pstcPpgConfig->bPpg2InvertPolarity) == TRUE) ? 1 : 0;
        unREVC0.REV04 = ((pstcPpgConfig->bPpg4InvertPolarity) == TRUE) ? 1 : 0;
        break;
      case 1:    
        unREVC0.REV08 = ((pstcPpgConfig->bPpg0InvertPolarity) == TRUE) ? 1 : 0;
        unREVC0.REV10 = ((pstcPpgConfig->bPpg2InvertPolarity) == TRUE) ? 1 : 0;
        unREVC0.REV12 = ((pstcPpgConfig->bPpg4InvertPolarity) == TRUE) ? 1 : 0;
        break;
      default:  // MFT2 PPGs to be added here
        break;
    }
    
    // PPG - Interrupts
    unPPGC0.PIE = ((pstcPpgConfig->bPpg0InterruptEnable) == TRUE) ? 1 : 0 ;
    unPPGC2.PIE = ((pstcPpgConfig->bPpg2InterruptEnable) == TRUE) ? 1 : 0 ;
    unPPGC4.PIE = ((pstcPpgConfig->bPpg4InterruptEnable) == TRUE) ? 1 : 0 ;
    
    // PPG - Interrupt Mode
    unPPGC0.INTM = ((pstcPpgConfig->bPpg0InterruptMode) == TRUE) ? 1 : 0 ;
    unPPGC2.INTM = ((pstcPpgConfig->bPpg2InterruptMode) == TRUE) ? 1 : 0 ;
    unPPGC4.INTM = ((pstcPpgConfig->bPpg4InterruptMode) == TRUE) ? 1 : 0 ;
    
    // PPG - Callback Pointers
    pstcMftInternData->pfnPpg0Callback = pstcPpgConfig->pfnPpg0Callback;
    pstcMftInternData->pfnPpg2Callback = pstcPpgConfig->pfnPpg2Callback;
    pstcMftInternData->pfnPpg4Callback = pstcPpgConfig->pfnPpg4Callback;
    
    if (((pstcPpgConfig->bPpg0InterruptEnable)  == TRUE) ||
        ((pstcPpgConfig->bPpg2InterruptEnable)  == TRUE) ||
        ((pstcPpgConfig->bPpg4InterruptEnable)  == TRUE)
       )
    {
      MftInitPpgInterrupt();
    }
  } // if (pstcPpgConfig != NULL)
  
  // Update hardware
  pstcMft->u16TCSA0 = unTCSA0;
  pstcMft->u16TCSA1 = unTCSA1;
  pstcMft->u16TCSA2 = unTCSA2;
  pstcMft->TCDT0 = 0; // Init counter
  pstcMft->TCDT1 = 0; // Init counter
  pstcMft->TCDT2 = 0; // Init counter
  pstcMft->TCCP0 = pstcFrtOcuConfig->u16Frt0Cycle;
  pstcMft->TCCP1 = pstcFrtOcuConfig->u16Frt1Cycle;
  pstcMft->TCCP2 = pstcFrtOcuConfig->u16Frt2Cycle;
  
  pstcMft->OCCP0 = pstcFrtOcuConfig->u16Ocu0CompareValue;
  pstcMft->OCCP1 = pstcFrtOcuConfig->u16Ocu1CompareValue;
  pstcMft->OCCP2 = pstcFrtOcuConfig->u16Ocu2CompareValue;
  pstcMft->OCCP3 = pstcFrtOcuConfig->u16Ocu3CompareValue;
  pstcMft->OCCP4 = pstcFrtOcuConfig->u16Ocu4CompareValue;
  pstcMft->OCCP5 = pstcFrtOcuConfig->u16Ocu5CompareValue;
  pstcMft->u8OCSA10 = unOCSA10;
  pstcMft->u8OCSA32 = unOCSA32;
  pstcMft->u8OCSA54 = unOCSA54;
  pstcMft->u8OCSB10 = unOCSB10;
  pstcMft->u8OCSB32 = unOCSB32;
  pstcMft->u8OCSB54 = unOCSB54;
  pstcMft->u8OCSC   = unOCSC;
  
  pstcMft->u16WFSA10 = unWFSA10;
  pstcMft->u16WFSA32 = unWFSA32;
  pstcMft->u16WFSA54 = unWFSA54;
  pstcMft->WFTM10 = u16WFTM10;
  pstcMft->WFTM32 = u16WFTM32;
  pstcMft->WFTM54 = u16WFTM54;
  
  pstcMft->u16NZCL = unNZCL;
  
  pstcMft->u8ICFS10 = unICFS10;
  pstcMft->u8ICFS32 = unICFS32;
  pstcMft->u8ICSA10 = unICSA10;
  pstcMft->u8ICSA32 = unICSA32;
  
  pstcMft->u16ACSA = unACSA;
  pstcMft->u16ATSA = unATSA;
  pstcMft->u16TCSB0 = unTCSB0;
  pstcMft->u16TCSB1 = unTCSB1;
  pstcMft->u16TCSB2 = unTCSB2;
  
  if (pstcAdcmpConfig != NULL)
  {
    pstcMft->ACCP0 = pstcAdcmpConfig->u16Adcmp0Accp;
    pstcMft->ACCP1 = pstcAdcmpConfig->u16Adcmp1Accp;
    pstcMft->ACCP2 = pstcAdcmpConfig->u16Adcmp2Accp;
    
    pstcMft->ACCPDN0 = pstcAdcmpConfig->u16Adcmp0Accpdn;
    pstcMft->ACCPDN1 = pstcAdcmpConfig->u16Adcmp1Accpdn;
    pstcMft->ACCPDN2 = pstcAdcmpConfig->u16Adcmp2Accpdn;
  }
  else
  {
    pstcMft->ACCP0 = 0;
    pstcMft->ACCP1 = 0;
    pstcMft->ACCP2 = 0;
    
    pstcMft->ACCPDN0 = 0;
    pstcMft->ACCPDN1 = 0;
    pstcMft->ACCPDN2 = 0;
  }
  
  if (pstcPpgConfig != NULL)
  {
    // PPG0(1) cycles depending on mode
    switch(pstcPpgConfig->enPpg0Mode)
    {
      case MftPpg8Bit:
        pstcMftPpg->PRLL0 = pstcPpgConfig->u8Ppg0Low;
        pstcMftPpg->PRLH0 = pstcPpgConfig->u8Ppg0High;
        break;
      case MftPpg8plus8Bit:
        pstcMftPpg->PRLL0 = pstcPpgConfig->u8Ppg0Low;
        pstcMftPpg->PRLH0 = pstcPpgConfig->u8Ppg0High;
        pstcMftPpg->PRLL1 = pstcPpgConfig->u8Ppg0PrescalerLow;
        pstcMftPpg->PRLH1 = pstcPpgConfig->u8Ppg0PrescalerHigh;        
        break;
      case MftPpg16Bit:
        pstcMftPpg->PRL0 = pstcPpgConfig->u16Ppg0Low;
        pstcMftPpg->PRL1 = pstcPpgConfig->u16Ppg0High;
        
#if L3_PPG_1616_MODE_AVAILABLE == L3_ON
      case MftPpg16plus16Bit:
        pstcMftPpg->PRL0 = pstcPpgConfig->u16Ppg0Low;
        pstcMftPpg->PRL1 = pstcPpgConfig->u16Ppg0High;
        pstcMftPpg->PRL2 = pstcPpgConfig->u16Ppg2Low;   // Prescaler
        pstcMftPpg->PRL3 = pstcPpgConfig->u16Ppg2High;  // Prescaler
#endif
        
      default:
        break;
    }
    
    // PPG2(3) cycles depending on mode
    switch(pstcPpgConfig->enPpg2Mode)
    {
      case MftPpg8Bit:
        pstcMftPpg->PRLL2 = pstcPpgConfig->u8Ppg2Low;
        pstcMftPpg->PRLH2 = pstcPpgConfig->u8Ppg2High;
        break;
      case MftPpg8plus8Bit:
        pstcMftPpg->PRLL2 = pstcPpgConfig->u8Ppg2Low;
        pstcMftPpg->PRLH2 = pstcPpgConfig->u8Ppg2High;
        pstcMftPpg->PRLL3 = pstcPpgConfig->u8Ppg2PrescalerLow;
        pstcMftPpg->PRLH3 = pstcPpgConfig->u8Ppg2PrescalerHigh;        
        break;
      case MftPpg16Bit:
        pstcMftPpg->PRL2 = pstcPpgConfig->u16Ppg2Low;
        pstcMftPpg->PRL3 = pstcPpgConfig->u16Ppg2High;
      default:
        break;
    } 
    
    // PPG4(5) cycles depending on mode
    switch(pstcPpgConfig->enPpg4Mode)
    {
      case MftPpg8Bit:
        pstcMftPpg->PRLL4 = pstcPpgConfig->u8Ppg4Low;
        pstcMftPpg->PRLH4 = pstcPpgConfig->u8Ppg4High;
        break;
      case MftPpg8plus8Bit:
        pstcMftPpg->PRLL4 = pstcPpgConfig->u8Ppg4Low;
        pstcMftPpg->PRLH4 = pstcPpgConfig->u8Ppg4High;
        pstcMftPpg->PRLL5 = pstcPpgConfig->u8Ppg4PrescalerLow;
        pstcMftPpg->PRLH5 = pstcPpgConfig->u8Ppg4PrescalerHigh;        
        break;
      case MftPpg16Bit:
        pstcMftPpg->PRL4 = pstcPpgConfig->u16Ppg4Low;
        pstcMftPpg->PRL5 = pstcPpgConfig->u16Ppg4High;
#if L3_PPG_1616_MODE_AVAILABLE == L3_ON
      case MftPpg16plus16Bit:
        pstcMftPpg->PRL4 = pstcPpgConfig->u16Ppg4Low;
        pstcMftPpg->PRL5 = pstcPpgConfig->u16Ppg4High;
        pstcMftPpg->PRL6 = pstcPpgConfig->u16Ppg6Low;   // Prescaler
        pstcMftPpg->PRL7 = pstcPpgConfig->u16Ppg6High;  // Prescaler
#endif
      default:
        break;
    } 
    
    switch (i8MftInstanceNumber)
    {
      case 0:
        pstcMftPpgBase->COMP0 = pstcPpgConfig->u8Ppg0Compare;
        pstcMftPpgBase->COMP2 = pstcPpgConfig->u8Ppg2Compare;
        pstcMftPpgBase->COMP4 = pstcPpgConfig->u8Ppg4Compare;
        
        pstcMftPpgBase->u8TTCR0  = unTTCRn;
        pstcMftPpgBase->u16REVC0 = unREVC0;
        break;
      case 1:
        pstcMftPpgBase->COMP1 = pstcPpgConfig->u8Ppg0Compare;
        pstcMftPpgBase->COMP3 = pstcPpgConfig->u8Ppg2Compare;
        pstcMftPpgBase->COMP5 = pstcPpgConfig->u8Ppg4Compare;   
        
        pstcMftPpgBase->u8TTCR1  = unTTCRn;
        pstcMftPpgBase->u16REVC0 = unREVC0;
        break;
      // TTCR2, COMP8-14 for MFT2, REVC1 to be added here
      default:
        break;
    }
    
    // PPG Operation Mode Control
    pstcMftPpg->u8PPGC0  = unPPGC0;
    pstcMftPpg->u8PPGC2  = unPPGC2;
    pstcMftPpg->u8PPGC4  = unPPGC4;
    
    // PPG Gate Function Control
    pstcMftPpg->u8GATEC0 = unGATEC0;
    pstcMftPpg->u8GATEC4 = unGATEC4;
  }
  else // if (pstcPpgConfig != NULL)
  {
    pstcMftPpg->PRL0 = 0;
    pstcMftPpg->PRL2 = 0;
    pstcMftPpg->PRL4 = 0;
  }
  
  return Ok;
} // Mft_Init()

/**
 ******************************************************************************
 ** \brief Starts FRTs of an MFT instance
 **
 ** \pre MFTn must be initialized properly before calling this function!
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  bStartFrt0        TRUE: Starts FRT0, FLASE: Don't touch FRT0
 ** \param [in]  bStartFrt1        TRUE: Starts FRT1, FLASE: Don't touch FRT1
 ** \param [in]  bStartFrt2        TRUE: Starts FRT2, FLASE: Don't touch FRT2
 ** \retval Ok                     Initialisation of MFT module 
 **                                successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **
 ******************************************************************************/
en_result_t Mft_FrtStart(stc_mftn_t*  pstcMft,
                         boolean_t    bStartFrt0,
                         boolean_t    bStartFrt1,
                         boolean_t    bStartFrt2
                        )
{
  stc_mft_intern_data_t* pstcMftInternData;   // Pointer to internal data structure
  
  // Check for NULL Pointer
  if (pstcMft == NULL)
  {
    return ErrorInvalidParameter ;
  }

  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if (pstcMftInternData == NULL)
  {
    return ErrorInvalidParameter;
  }  
  
  if (TRUE == bStartFrt0)
  {
     pstcMft->u16TCSA0.STOP = 0;
     pstcMft->u16TCSA0.SCLR = 1;
  }
  
  if (TRUE == bStartFrt1)
  {
     pstcMft->u16TCSA1.STOP = 0;
     pstcMft->u16TCSA1.SCLR = 1;
  }
  
  if (TRUE == bStartFrt2)
  {
     pstcMft->u16TCSA2.STOP = 0;
     pstcMft->u16TCSA2.SCLR = 1;
  }
  
  return Ok;
} // Mft_FrtStart

/**
 ******************************************************************************
 ** \brief Stops FRTs of an MFT instance
 **
 ** \pre MFTn must be initialized properly before calling this function!
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  bStopFrt0         TRUE: Stops FRT0, FLASE: Don't touch FRT0
 ** \param [in]  bStopFrt1         TRUE: Stops FRT1, FLASE: Don't touch FRT1
 ** \param [in]  bStopFrt2         TRUE: Stops FRT2, FLASE: Don't touch FRT2
 ** \retval Ok                     Initialisation of MFT module 
 **                                successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **
 ******************************************************************************/
en_result_t Mft_FrtStop(stc_mftn_t*  pstcMft,
                        boolean_t    bStopFrt0,
                        boolean_t    bStopFrt1,
                        boolean_t    bStopFrt2
                       )
{
  stc_mft_intern_data_t* pstcMftInternData;   // Pointer to internal data structure
  
  // Check for NULL Pointer
  if (pstcMft == NULL)
  {
    return ErrorInvalidParameter ;
  }

  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if (pstcMftInternData == NULL)
  {
    return ErrorInvalidParameter;
  }  
  
  if (TRUE == bStopFrt0)
  {
     pstcMft->u16TCSA0.STOP = 1;
     pstcMft->u16TCSA0.SCLR = 0;
  }
  
  if (TRUE == bStopFrt1)
  {
     pstcMft->u16TCSA1.STOP = 1;
     pstcMft->u16TCSA1.SCLR = 0;
  }
  
  if (TRUE == bStopFrt2)
  {
     pstcMft->u16TCSA2.STOP = 1;
     pstcMft->u16TCSA2.SCLR = 0;
  }
  
  return Ok;
} // Mft_FrtStart

/**
 ******************************************************************************
 ** \brief Reads out the current count of a FRT channel of an MFT instance
 **
 ** Note, that 0 is returned, if pstMft == NULL and u8FrtChannel > 3 and
 ** does not mean the counter value 0!
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  u8FrtChannel      0-3: FRT Channel Number
 **
 ** \return Current count
 **
 ******************************************************************************/
uint16_t Mft_FrtRead(stc_mftn_t*  pstcMft,
                     uint8_t      u8FrtChannel
                    )
{
  if (pstcMft == NULL)
  {
    return 0 ;
  }
  
  switch (u8FrtChannel)
  {
    case 0:
      return pstcMft->TCDT0;
    case 1:
      return pstcMft->TCDT1;
    case 2:
      return pstcMft->TCDT2;
    default:
      break;
  }
  
  return 0;
} // Mft_FrtRead

/**
 ******************************************************************************
 ** \brief Start or Stops OCU operation by configuration
 **
 ** The operation of a OCU channel is cotrolled by the FRT/OCU configurations.
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  pstcFrtOcuConfig  Pointer to FRT/OCU configuration
 **
 ** \retval Ok                     New values successfully set
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcFrtOcuConfig == NULL
 **
 ******************************************************************************/
en_result_t Mft_OcuStartStop( stc_mftn_t*                pstcMft, 
                              stc_mft_frt_ocu_config_t*  pstcFrtOcuConfig
                            )
{
  uint8_t u8Ocsa01Mask = 0;
  uint8_t u8Ocsa23Mask = 0;
  uint8_t u8Ocsa45Mask = 0; 
  uint8_t u8DummyRead;
  
  // Check for NULL Pointers
  if ((pstcMft          == NULL) ||
      (pstcFrtOcuConfig == NULL)
     )
  {
    return ErrorInvalidParameter ;
  }
  
  u8Ocsa01Mask  = ((pstcFrtOcuConfig->bOcu0Enable) == TRUE) ? 0x01 : 0 ;
  u8Ocsa01Mask |= ((pstcFrtOcuConfig->bOcu1Enable) == TRUE) ? 0x02 : 0 ;
  u8Ocsa23Mask  = ((pstcFrtOcuConfig->bOcu2Enable) == TRUE) ? 0x01 : 0 ;
  u8Ocsa23Mask |= ((pstcFrtOcuConfig->bOcu3Enable) == TRUE) ? 0x02 : 0 ;
  u8Ocsa45Mask  = ((pstcFrtOcuConfig->bOcu4Enable) == TRUE) ? 0x01 : 0 ;
  u8Ocsa45Mask |= ((pstcFrtOcuConfig->bOcu5Enable) == TRUE) ? 0x02 : 0 ;
  
  // Update hardware
  u8DummyRead = pstcMft->OCSA10 & 0xFC;
  pstcMft->OCSA10 = u8DummyRead | u8Ocsa01Mask;

  u8DummyRead = pstcMft->OCSA32 & 0xFC;
  pstcMft->OCSA32 = u8DummyRead | u8Ocsa23Mask;

  u8DummyRead = pstcMft->OCSA54 & 0xFC;
  pstcMft->OCSA54 = u8DummyRead | u8Ocsa45Mask;

  return Ok;  
} // Mft_OcuStartStop

/**
 ******************************************************************************
 ** \brief Start or Stops ICU operation by configuration
 **
 ** The operation of a ICU channel is cotrolled by the #en_mft_icu_edge_t
 ** configurations.
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  pstcIcuConfig     Pointer to ICU configuration
 **
 ** \retval Ok                     New values successfully set
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcIcuConfig == NULL
 **
 ******************************************************************************/
en_result_t Mft_IcuStartStop( stc_mftn_t*            pstcMft, 
                              stc_mft_icu_config_t*  pstcIcuConfig
                            )
{
  stc_mft_icu_icsa10_field_t unICSA10 = {0};
  stc_mft_icu_icsa32_field_t unICSA32 = {0};
  
  // Check for NULL Pointers
  if ((pstcMft       == NULL) ||
      (pstcIcuConfig == NULL)
     )
  {
    return ErrorInvalidParameter ;
  }
  
  // ICU0 - Edge Detection Selection
  switch(pstcIcuConfig->enIcu0Edge)
  {
    case MftIcuDisable:
      unICSA10.EG00 = 0;
      unICSA10.EG01 = 0;
      break;
    case MftIcuRisingEdge:
      unICSA10.EG00 = 1;
      unICSA10.EG01 = 0;
      break;
    case MftIcuFallingEdge:
      unICSA10.EG00 = 0;
      unICSA10.EG01 = 1;
      break;
    case MftIcuBothEdges:
      unICSA10.EG00 = 1;
      unICSA10.EG01 = 1;
      break;
    default:
      return ErrorInvalidParameter; 
  }
  
  // ICU1 - Edge Detection Selection
  switch(pstcIcuConfig->enIcu1Edge)
  {
    case MftIcuDisable:
      unICSA10.EG10 = 0;
      unICSA10.EG11 = 0;
      break;
    case MftIcuRisingEdge:
      unICSA10.EG10 = 1;
      unICSA10.EG11 = 0;
      break;
    case MftIcuFallingEdge:
      unICSA10.EG10 = 0;
      unICSA10.EG11 = 1;
      break;
    case MftIcuBothEdges:
      unICSA10.EG10 = 1;
      unICSA10.EG11 = 1;
      break;
    default:
      return ErrorInvalidParameter; 
  }
  
  // ICU2 - Edge Detection Selection
  switch(pstcIcuConfig->enIcu2Edge)
  {
    case MftIcuDisable:
      unICSA32.EG20 = 0;
      unICSA32.EG21 = 0;
      break;
    case MftIcuRisingEdge:
      unICSA32.EG20 = 1;
      unICSA32.EG21 = 0;
      break;
    case MftIcuFallingEdge:
      unICSA32.EG20 = 0;
      unICSA32.EG21 = 1;
      break;
    case MftIcuBothEdges:
      unICSA32.EG20 = 1;
      unICSA32.EG21 = 1;
      break;
    default:
      return ErrorInvalidParameter; 
  }
  
  // ICU3 - Edge Detection Selection
  switch(pstcIcuConfig->enIcu3Edge)
  {
    case MftIcuDisable:
      unICSA32.EG30 = 0;
      unICSA32.EG31 = 0;
      break;
    case MftIcuRisingEdge:
      unICSA32.EG30 = 1;
      unICSA32.EG31 = 0;
      break;
    case MftIcuFallingEdge:
      unICSA32.EG30 = 0;
      unICSA32.EG31 = 1;
      break;
    case MftIcuBothEdges:
      unICSA32.EG30 = 1;
      unICSA32.EG31 = 1;
      break;
    default:
      return ErrorInvalidParameter; 
  } 
  
  // Update hardware
  pstcMft->u8ICSA10 = unICSA10;
  pstcMft->u8ICSA32 = unICSA32;
  
  return Ok;
} // Mft_IcuStartStop

/**
 ******************************************************************************
 ** \brief Reads out the capture count of a ICU channel of an MFT instance
 **
 ** Note, that 0 is returned, if pstMft == NULL and u8IcuChannel > 3 and
 ** does not mean the counter value 0!
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  u8IcuChannel      0-3: FRT Channel Number
 **
 ** \return Capured ICU count
 **
 ******************************************************************************/
uint16_t Mft_IcuRead(stc_mftn_t*  pstcMft,
                     uint8_t      u8IcuChannel
                    )
{
  if (pstcMft == NULL)
  {
    return 0 ;
  }
  
  switch (u8IcuChannel)
  {
    case 0:
      return pstcMft->ICCP0;
    case 1:
      return pstcMft->ICCP1;
    case 2:
      return pstcMft->ICCP2;
    case 3:
      return pstcMft->ICCP3;
    default:
      break;
  }
  
  return 0;
} // Mft_FrtRead

/**
 ******************************************************************************
 ** \brief Triggers PPG channels of an MFT instance by Software
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  bPpg0             TRUE: Trigger PPG0, FALSE: Do not touch PPG0
 ** \param [in]  bPpg2             TRUE: Trigger PPG2, FALSE: Do not touch PPG2
 ** \param [in]  bPpg4             TRUE: Trigger PPG4, FALSE: Do not touch PPG4
 **
 ** \retval Ok                     Trigger of PPG modules successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **             - Intance number of internal data out of range (corrupted)
 **
 ******************************************************************************/
en_result_t Mft_PpgSwTrigger(stc_mftn_t*  pstcMft,
                             boolean_t    bPpg0,
                             boolean_t    bPpg2,
                             boolean_t    bPpg4
                            )
{
  volatile stc_ppg_base_t*   pstcMftPpgBase = &PPG;  // Pointer to PPG base registers
  stc_mft_intern_data_t*     pstcMftInternData;      // Pointer to internal data structure
  uint16_t                   u16TriggerMask;         // Trigger mask for PPG numbers

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if ( pstcMftInternData == NULL )
  {
    return ErrorInvalidParameter;
  }
  
  switch(pstcMftInternData->i8MftInstanceNumber)
  {
    // MFT0 PPG
    case 0:
      u16TriggerMask  = (bPpg0 == TRUE) ? 0x0001 : 0 ; // TRG0.PEN00
      u16TriggerMask |= (bPpg2 == TRUE) ? 0x0004 : 0 ; // TRG0.PEN02
      u16TriggerMask |= (bPpg4 == TRUE) ? 0x0010 : 0 ; // TRG0.PEN04
      pstcMftPpgBase->TRG0 |= u16TriggerMask;
      break;
    // MFT1 PPG
    case 1:
      u16TriggerMask  = (bPpg0 == TRUE) ? 0x0100 : 0 ; // TRG0.PEN08
      u16TriggerMask |= (bPpg2 == TRUE) ? 0x0400 : 0 ; // TRG0.PEN10
      u16TriggerMask |= (bPpg4 == TRUE) ? 0x1000 : 0 ; // TRG0.PEN12
      pstcMftPpgBase->TRG0 |= u16TriggerMask;
      break;
    // Future: MFT2 PPG -> TRG1.PENxy
    default:
      return ErrorInvalidParameter; // should never be happen!
  }
  
  return Ok;
} // Mft_PpgSwTrigger

/**
 ******************************************************************************
 ** \brief Stops PPG channels of an MFT instance by Software
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  bPpg0             TRUE: Stop PPG0, FALSE: Do not touch PPG0
 ** \param [in]  bPpg2             TRUE: Stop PPG2, FALSE: Do not touch PPG2
 ** \param [in]  bPpg4             TRUE: Stop PPG4, FALSE: Do not touch PPG4
 **
 ** \retval Ok                     Stop of PPG modules successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **             - Intance number of internal data out of range (corrupted)
 **
 ******************************************************************************/
en_result_t Mft_PpgSwStop(stc_mftn_t*  pstcMft,
                          boolean_t    bPpg0,
                          boolean_t    bPpg2,
                          boolean_t    bPpg4
                         )
{
  volatile stc_ppg_base_t*   pstcMftPpgBase = &PPG;  // Pointer to PPG base registers
  stc_mft_intern_data_t*     pstcMftInternData;      // Pointer to internal data structure
  uint16_t                   u16TriggerMask;         // Trigger mask for PPG numbers

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if ( pstcMftInternData == NULL )
  {
    return ErrorInvalidParameter;
  }
  
  switch(pstcMftInternData->i8MftInstanceNumber)
  {
    // MFT0 PPG
    case 0:
      u16TriggerMask  = (bPpg0 == TRUE) ? 0xFFFE : 0xFFFF ; // TRG0.PEN00
      u16TriggerMask &= (bPpg2 == TRUE) ? 0xFFFB : 0xFFFF ; // TRG0.PEN02
      u16TriggerMask &= (bPpg4 == TRUE) ? 0xFFEF : 0xFFFF ; // TRG0.PEN04
      pstcMftPpgBase->TRG0 &= u16TriggerMask;
      break;
    // MFT1 PPG
    case 1:
      u16TriggerMask  = (bPpg0 == TRUE) ? 0xFEFF : 0xFFFF ; // TRG0.PEN08
      u16TriggerMask &= (bPpg2 == TRUE) ? 0xFBFF : 0xFFFF ; // TRG0.PEN10
      u16TriggerMask &= (bPpg4 == TRUE) ? 0xEFFF : 0xFFFF ; // TRG0.PEN12
      pstcMftPpgBase->TRG0 &= u16TriggerMask;
      break;
    // Future: MFT2 PPG -> TRG1.PENxy
    default:
      return ErrorInvalidParameter; // should never be happen!
  }
  
  return Ok;
} // Mft_PpgSwStop

/**
 ******************************************************************************
 ** \brief Start all 3 PPG timer counter of an MFT instance by Software
 **
 ** Start PPG0/2/4 (PPG8/10/12) timer synchronously.
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 **
 ** \retval Ok                     Trigger of PPG modules successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **             - Intance number of internal data out of range (corrupted)
 **
 ******************************************************************************/
en_result_t Mft_PpgTimerStart(stc_mftn_t*  pstcMft)
{
  volatile stc_ppg_base_t*   pstcMftPpgBase = &PPG;  // Pointer to PPG base registers
  stc_mft_intern_data_t*     pstcMftInternData;      // Pointer to internal data structure

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if ( pstcMftInternData == NULL )
  {
    return ErrorInvalidParameter;
  }
  
  switch(pstcMftInternData->i8MftInstanceNumber)
  {
    // MFT0 PPG0/2/4
    case 0:
      pstcMftPpgBase->TTCR0 |= 1;
      break;
    // MFT1 PPG8/10/12
    case 1:
      pstcMftPpgBase->TTCR1 |= 1;
      break;
    // Future: TTCR2
    default:
      return ErrorInvalidParameter; // should never be happen!
  } 
  
  return Ok;
} // Mft_PpgTimerStart

/**
 ******************************************************************************
 ** \brief Stops PPG timer counter channels of an MFT instance by Software
 **
 ** Stop PPG0/2/4 (PPG8/10/12) timer individually.
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  bPpg0             TRUE: Stop PPG0, FALSE: Do not touch PPG0
 ** \param [in]  bPpg2             TRUE: Stop PPG2, FALSE: Do not touch PPG2
 ** \param [in]  bPpg4             TRUE: Stop PPG4, FALSE: Do not touch PPG4
 **
 ** \retval Ok                     Stop of PPG modules successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - pstcMftInternData == NULL (invalid or disabled MFT unit
 **                                         (L3_PERIPHERAL_ENABLE_MFT)) 
 **             - Intance number of internal data out of range (corrupted)
 **
 ******************************************************************************/
en_result_t Mft_PpgTimerStop(stc_mftn_t*  pstcMft,
                             boolean_t    bPpg0,
                             boolean_t    bPpg2,
                             boolean_t    bPpg4
                            )
{
  volatile stc_ppg_base_t*   pstcMftPpgBase = &PPG;  // Pointer to PPG base registers
  stc_mft_intern_data_t*     pstcMftInternData;      // Pointer to internal data structure
  uint8_t                    u8StopMask;             // Stop mask for PPG numbers

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }
  
  // Get ptr to internal data struct ...
  pstcMftInternData = MftGetInternDataPtr(pstcMft);
  // ... and check
  if ( pstcMftInternData == NULL )
  {
    return ErrorInvalidParameter;
  }
  
  u8StopMask  = (bPpg0 == TRUE) ? 0x10 : 0 ; // TRG0O, TRG1O
  u8StopMask |= (bPpg2 == TRUE) ? 0x20 : 0 ; // TRG2O, TRG3O
  u8StopMask |= (bPpg4 == TRUE) ? 0x40 : 0 ; // TRG4O, TRG5O
      
  switch(pstcMftInternData->i8MftInstanceNumber)
  {
    // MFT0 PPG0/2/4
    case 0:
      pstcMftPpgBase->TTCR0 |= u8StopMask;
      break;
    // MFT1 PPG8/10/12
    case 1:
      pstcMftPpgBase->TTCR1 |= u8StopMask;
      break;
    // Future: TTCR2
    default:
      return ErrorInvalidParameter; // should never be happen!
  } 
  
  return Ok;  
}

/**
 ******************************************************************************
 ** \brief Set new 8-bit PPG values of an MFT instance
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  u8PpgNumber       PPG number
 ** \param [in]  u8LowValue        New Low pulse value
 ** \param [in]  u8HighValue       New High pulse value
 **
 ** \retval Ok                     New values successfully set
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - PPG number out of range
 **
 ******************************************************************************/
en_result_t Mft_PpgSetValue8(stc_mftn_t*  pstcMft,
                             uint8_t      u8PpgNumber,
                             uint8_t      u8LowValue,
                             uint8_t      u8HighValue
                            )
{
  volatile stc_ppgn_t*  pstcMftPpg;  // Pointer to PPG registers

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }

  // Get pointer to dedicated PPG registers
  pstcMftPpg = MftGetPpgPointer(pstcMft);
  
  switch (u8PpgNumber)
  {
    case 0:
      pstcMftPpg->PRLL0 = u8LowValue;
      pstcMftPpg->PRLH0 = u8HighValue;
      break;
    case 2:
      pstcMftPpg->PRLL2 = u8LowValue;
      pstcMftPpg->PRLH2 = u8HighValue;
      break; 
    case 4:
      pstcMftPpg->PRLL4 = u8LowValue;
      pstcMftPpg->PRLH4 = u8HighValue;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  return Ok;
} // Mft_PpgSetValue8

/**
 ******************************************************************************
 ** \brief Set new 16-bit PPG values of an MFT instance
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  u8PpgNumber       PPG number
 ** \param [in]  u16LowValue       New Low pulse value
 ** \param [in]  u16HighValue      New High pulse value
 **
 ** \retval Ok                     New values successfully set
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - PPG number out of range
 **
 ******************************************************************************/
en_result_t Mft_PpgSetValue16(stc_mftn_t*  pstcMft,
                              uint8_t      u8PpgNumber,
                              uint16_t     u16LowValue,
                              uint16_t     u16HighValue
                             )
{
  volatile stc_ppgn_t*  pstcMftPpg;  // Pointer to PPG registers

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }

  // Get pointer to dedicated PPG registers
  pstcMftPpg = MftGetPpgPointer(pstcMft);
  
  switch (u8PpgNumber)
  {
    case 0:
      pstcMftPpg->PRL0 = u16LowValue;
      pstcMftPpg->PRL1 = u16HighValue;
      break;
    case 2:
      pstcMftPpg->PRL2 = u16LowValue;
      pstcMftPpg->PRL3 = u16HighValue;
      break; 
    case 4:
      pstcMftPpg->PRL4 = u16LowValue;
      pstcMftPpg->PRL5 = u16HighValue;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  return Ok;
} // Mft_PpgSetValue16

/**
 ******************************************************************************
 ** \brief Set new 8+8-bit PPG values of an MFT instance
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  u8PpgNumber       PPG number
 ** \param [in]  u8PscLowValue     New Prescaler Low value
 ** \param [in]  u8PscHighValue    New Prescaler High value
 ** \param [in]  u8LowValue        New Low pulse value
 ** \param [in]  u8HighValue       New High pulse value
 **
 ** \retval Ok                     New values successfully set
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **             - PPG number out of range
 **
 ******************************************************************************/
en_result_t Mft_PpgSetValue8plus8(stc_mftn_t*  pstcMft,
                                  uint8_t      u8PpgNumber,
                                  uint8_t      u8PscLowValue,
                                  uint8_t      u8PscHighValue,
                                  uint8_t      u8LowValue,
                                  uint8_t      u8HighValue
                                 )
{
  volatile stc_ppgn_t*  pstcMftPpg;  // Pointer to PPG registers

  // Check for NULL Pointer
  if (pstcMft == NULL )
  {
    return ErrorInvalidParameter ;
  }

  // Get pointer to dedicated PPG registers
  pstcMftPpg = MftGetPpgPointer(pstcMft);
  
  switch (u8PpgNumber)
  {
    case 0:
      pstcMftPpg->PRLL0 = u8LowValue;
      pstcMftPpg->PRLH0 = u8HighValue;
      pstcMftPpg->PRLL1 = u8PscLowValue;
      pstcMftPpg->PRLH1 = u8PscHighValue;      
      break;
    case 2:
      pstcMftPpg->PRLL2 = u8LowValue;
      pstcMftPpg->PRLH2 = u8HighValue;
      pstcMftPpg->PRLL3 = u8PscLowValue;
      pstcMftPpg->PRLH3 = u8PscHighValue;  
      break; 
    case 4:
      pstcMftPpg->PRLL4 = u8LowValue;
      pstcMftPpg->PRLH4 = u8HighValue;
      pstcMftPpg->PRLL5 = u8PscLowValue;
      pstcMftPpg->PRLH5 = u8PscHighValue;  
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  return Ok;
} // Mft_PpgSetValue8plus8

/**
 ******************************************************************************
 ** \brief De-Inits an MFT instance
 **
 ** \param [in]  pstcMft           Pointer to MFT instance register area
 ** \param [in]  pstcFrtOcuConfig  Configuration of FRT and OCU of MFT instance
 ** \param [in]  pstcWfgConfig     Configuration of WFG of MFT instance
 ** \param [in]  pstcNzclConfig    Configuration of NZCL of MFT instance
 ** \param [in]  pstcIcuConfig     Configuration of ICU of MFT instance
 ** \param [in]  pstcAdcmpConfig   Configuration of ADCMP of MFT instance
 ** \param [in]  pstcPpgConfig     Configuration of PPG of MFT instance
 **
 ** \retval Ok                     De-Initialisation of MFT module 
 **                                successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMft == NULL
 **
 ******************************************************************************/
en_result_t Mft_DeInit( stc_mftn_t*                pstcMft, 
                        stc_mft_frt_ocu_config_t*  pstcFrtOcuConfig,
                        stc_mft_wfg_config_t*      pstcWfgConfig,
                        stc_mft_nzcl_config_t*     pstcNzclConfig,
                        stc_mft_icu_config_t*      pstcIcuConfig,
                        stc_mft_adcmp_config_t*    pstcAdcmpConfig,
                        stc_mft_ppg_config_t*      pstcPpgConfig
                      )
{
  volatile stc_ppgn_t*       pstcMftPpg;             // Pointer to dedicated PPG registers
  volatile stc_ppg_base_t*   pstcMftPpgBase = &PPG;  // Pointer to PPG base registers
  int8_t                     i8MftInstanceNumber;
  
  // Check for NULL Pointer
  if (pstcMft == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  // First de-init Interupts (if configuration is != NULL)
  MftDeInitOcuInterrupt();
  MftDeInitFrtInterrupt();
  
  if (pstcWfgConfig != NULL)
  {
    MftDeInitWfgInterrupt();
  }
  
  if (pstcIcuConfig != NULL)
  {
    MftDeInitIcuInterrupt();
  }
  
  if (pstcPpgConfig != NULL)
  {
    MftDeInitPpgInterrupt();
  }
  
  // Get MFT instance number ...
  i8MftInstanceNumber = MftGetInstanceNumber(pstcMft);
  // ... and check
  if (i8MftInstanceNumber < 0)
  {
    return ErrorInvalidParameter ;
  }
  
  // Get pointer to dedicated PPG registers
  pstcMftPpg = MftGetPpgPointer(pstcMft);
  
  // Clear all registers to initial values
  pstcMft->TCSA0 = 0x0040; // Stop FRT0
  pstcMft->TCSA1 = 0x0040; // Stop FRT1
  pstcMft->TCSA2 = 0x0040; // Stop FRT2
  
  pstcMft->TCCP0 = 0xFFFF;
  pstcMft->TCCP1 = 0xFFFF;
  pstcMft->TCCP2 = 0xFFFF;  

  pstcMft->TCDT0 = 0;
  pstcMft->TCDT1 = 0;
  pstcMft->TCDT2 = 0;  

  pstcMft->OCFS10 = 0;
  pstcMft->OCFS32 = 0;
  pstcMft->OCFS54 = 0; 
  
  pstcMft->OCSA10 = 0x0C;
  pstcMft->OCSA32 = 0x0C;
  pstcMft->OCSA54 = 0x0C;  

  pstcMft->OCSB10 = 0x60;
  pstcMft->OCSB32 = 0x60;
  pstcMft->OCSB54 = 0x60;  

  pstcMft->OCSC = 0;
  
  pstcMft->OCCP0 = 0;
  pstcMft->OCCP1 = 0;
  pstcMft->OCCP2 = 0;
  pstcMft->OCCP3 = 0;
  pstcMft->OCCP4 = 0;
  pstcMft->OCCP5 = 0;
  
  if (pstcWfgConfig != NULL)
  {
    pstcMft->WFSA10 = 0;
    pstcMft->WFSA32 = 0;
    pstcMft->WFSA54 = 0;
    
    pstcMft->WFTM10 = 0;
    pstcMft->WFTM32 = 0;
    pstcMft->WFTM54 = 0;
    
    pstcMft->WFIR = 0;    
  }
  
  if (pstcNzclConfig != NULL)
  {  
    pstcMft->NZCL = 0;
  }
  
  if (pstcIcuConfig != NULL)
  {
    pstcMft->ICFS10 = 0;
    pstcMft->ICFS32 = 0;
    
    pstcMft->ICSA10 = 0;
    pstcMft->ICSA32 = 0;
    
    pstcMft->ICSB10 = 0;
    pstcMft->ICSB32 = 0;
  }
  
  if (pstcAdcmpConfig != NULL)
  {
    pstcMft->ACSA = 0;
    pstcMft->ACSB = 0x07;
    
    pstcMft->ACCP0 = 0;
    pstcMft->ACCP1 = 0;
    pstcMft->ACCP2 = 0;
    
    pstcMft->ACCPDN0 = 0;
    pstcMft->ACCPDN1 = 0;
    pstcMft->ACCPDN2 = 0;
    
    pstcMft->ATSA = 0;
  
    pstcMft->TCSB0 = 0;
    pstcMft->TCSB1 = 0;
    pstcMft->TCSB2 = 0; 
  }
  
  if (pstcPpgConfig != NULL)
  {
    switch (i8MftInstanceNumber)
    {
      case 0:
        pstcMftPpgBase->TTCR0  = 0xF0;   
        pstcMftPpgBase->TRG0  &= 0xFF00;
        pstcMftPpgBase->REVC0 &= 0xFF00;
        
        pstcMftPpgBase->COMP0 = 0;
        pstcMftPpgBase->COMP2 = 0;
        pstcMftPpgBase->COMP4 = 0;
        break;
      case 1:
        pstcMftPpgBase->TTCR1  = 0xF0;   
        pstcMftPpgBase->TRG0  &= 0x00FF;
        pstcMftPpgBase->REVC0 &= 0x00FF;
        
        pstcMftPpgBase->COMP1 = 0;
        pstcMftPpgBase->COMP3 = 0;
        pstcMftPpgBase->COMP5 = 0;
        break;
      // COMP8-14 for MFT2 to be added here
      default:
        return ErrorInvalidParameter ;
    }
    
    pstcMftPpg->PPGC0 = 0;
    pstcMftPpg->PPGC1 = 0;
    pstcMftPpg->PPGC2 = 0;
    pstcMftPpg->PPGC3 = 0;        
    pstcMftPpg->PPGC4 = 0;
    pstcMftPpg->PPGC5 = 0;
    pstcMftPpg->PPGC6 = 0;
    pstcMftPpg->PPGC7 = 0;

    pstcMftPpg->PRL0 = 0;
    pstcMftPpg->PRL1 = 0;
    pstcMftPpg->PRL2 = 0;
    pstcMftPpg->PRL3 = 0;
    pstcMftPpg->PRL4 = 0;
    pstcMftPpg->PRL5 = 0;
    pstcMftPpg->PRL6 = 0;
    pstcMftPpg->PRL7 = 0;
    
    pstcMftPpg->GATEC0 = 0;
    pstcMftPpg->GATEC4 = 0;
  }
    
  return Ok;
}

#if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Init PPG IGBT module (only Type7 devices)
 **
 ** \pre MFTn PPG must be initialized properly before calling this function!
 **
 ** \param [in]  pstcConfig        Pointer to PPG-IGBT configuration
 **
 ** \retval Ok                     Initialisation of PPG-IGBT module 
 **                                successfully done
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **                                - pstcConfig == NULL
 **                                - pstcConfig->enNoiseFilter undefined
 **                                  enumerator
 ******************************************************************************/
en_result_t Mft_PpgIgbt_Init( stc_mft_ppg_igbt_config_t* pstcConfig )
{
  volatile stc_igbtn_t*  pstcMftPpgIgbt = &PPG_IGBT;  // Pointer to PPG-IGBT register
  stc_ppg_igbtc_t        enIGBTC;                     // Local IGBT mirror register variable
  
  // Check for NULL Pointer
  if (pstcConfig == NULL)
  {
    return ErrorInvalidParameter ;
  }
  
  enIGBTC.IGATIH  = ((pstcConfig->bStopProhibMode)  == TRUE) ? 1 : 0 ;
  enIGBTC.IGOSEL0 = ((pstcConfig->bInvertedOutput0) == TRUE) ? 1 : 0 ;
  enIGBTC.IGOSEL1 = ((pstcConfig->bInvertedOutput1) == TRUE) ? 1 : 0 ;
  enIGBTC.IGTRGLV = ((pstcConfig->bInvertedInput)   == TRUE) ? 1 : 0 ;
  
  switch (pstcConfig->enNoiseFilter)
  {
    case MftPpgIgbtNoFilter :
      enIGBTC.IGNFW = 0;
      break;
    case MftPpgIgbt4PclkFilter :
      enIGBTC.IGNFW = 1;
      break;
    case MftPpgIgbt8PclkFilter :
      enIGBTC.IGNFW = 2;
      break;
    case MftPpgIgbt16PclkFilter :
      enIGBTC.IGNFW = 3;
      break;
    case MftPpgIgbt32PclkFilter :
      enIGBTC.IGNFW = 4;
      break;
    default: 
      return ErrorInvalidParameter ;
  }
  
  // Finally setup hardware
  pstcMftPpgIgbt->u8IGBTC = enIGBTC;
  
  return Ok;
} // Mft_PpgIgbt_Init
     
/**
 ******************************************************************************
 ** \brief Enable PPG IGBT module (only Type7 devices)
 **
 ** \pre MFTn PPG must be initialized properly before calling this function!
 **
 ** \retval Ok     PPG-IGBT modele enabled
 ******************************************************************************/
en_result_t Mft_PpgIgbt_Enable(void)
{
  volatile stc_igbtn_t*  pstcMftPpgIgbt = &PPG_IGBT;  // Pointer to PPG-IGBT register 
  
  pstcMftPpgIgbt->u8IGBTC.IGBTMD = 1;
   
  return Ok;
} // Mft_PpgIgbt_Enable

/**
 ******************************************************************************
 ** \brief Disable PPG IGBT module (only Type7 devices)
 **
 ** \pre MFTn PPG must be initialized properly before calling this function!
 **
 ** \retval Ok     PPG-IGBT modele enabled
 ******************************************************************************/
en_result_t Mft_PpgIgbt_Disable(void)
{
  volatile stc_igbtn_t*  pstcMftPpgIgbt = &PPG_IGBT;  // Pointer to PPG-IGBT register  
  
  pstcMftPpgIgbt->u8IGBTC.IGBTMD = 0;
   
  return Ok;
} // Mft_PpgIgbt_Disable

/**
 ******************************************************************************
 ** \brief De-Init PPG IGBT module (only Type7 devices)
 **
 ** \pre MFTn PPG must be initialized properly before calling this function!
 **
 ** \retval Ok     PPG-IGBT modele de-initialized
 ******************************************************************************/
en_result_t Mft_PpgIgbt_DeInit(void)
{
  volatile stc_igbtn_t*  pstcMftPpgIgbt = &PPG_IGBT;  // Pointer to PPG-IGBT register  
  
  pstcMftPpgIgbt->IGBTC = 0;
   
  return Ok;
} // Mft_PpgIgbt_DeInit

#endif // #if (L3_DEVICE_TYPE == L3_TYPE7)

//@} // MftGroup

#endif // #if (defined(L3_PERIPHERAL_MFT_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
