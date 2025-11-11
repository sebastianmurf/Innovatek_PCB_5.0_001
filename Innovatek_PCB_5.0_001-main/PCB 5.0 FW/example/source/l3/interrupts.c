/*****************************************************************************/
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
/*****************************************************************************/
/** \file interrupts.c
 **
 ** L3 Interrupt Handler
 **
 ** A detailed description is available at 
 ** @link L3Interrupts L3 Interrupt handling description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi       First version.
 **   - 2012-04-12  1.1  MWi       IRQMON bit mask defines for QPDR and DT added;
 **                                stc_dtn_t type for DT only ISR call corrected;
 **                                L3_TYPE6 and L3_TYPE7 added
 **   - 2012-06-27  1.2  MWi       Interrupt Types added, RTC added
 **   - 2012-08-30  1.3  MWi, MSc  USB added
 **
 *****************************************************************************/
/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "interrupts.h"
#include "common.h"
#include "GpsUart.h"
/**
 ******************************************************************************
 ** \addtogroup L3Interrupts
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/******************************************************************************/
/* Device specific interrupt service routines and driver interrupt service    */
/* calls                                                                      */
/******************************************************************************/

 // DMA
#if (L3_PERIPHERAL_ENABLE_DMA == L3_ON)
  void DMAC0_Handler(void)
  {
    DmaIrqHandler(0);
  }
  void DMAC1_Handler(void)
  {
    DmaIrqHandler(1);
  }
  void DMAC2_Handler(void)
  {
    DmaIrqHandler(2);
  }
  void DMAC3_Handler(void)
  {
    DmaIrqHandler(3);
  }
  void DMAC4_Handler(void)
  {
    DmaIrqHandler(4);
  }
  void DMAC5_Handler(void)
  {
    DmaIrqHandler(5);
  }
  void DMAC6_Handler(void)
  {
    DmaIrqHandler(6);
  }
  void DMAC7_Handler(void)
  {
    DmaIrqHandler(7);
  }
#endif

// ISRs for Device TYPE0, 1, 2, 3, 4, 5, 6, 7 (all)
#if (L3_DEVICE_TYPE == L3_TYPE0) || \
    (L3_DEVICE_TYPE == L3_TYPE1) || \
    (L3_DEVICE_TYPE == L3_TYPE2) || \
    (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE4) || \
    (L3_DEVICE_TYPE == L3_TYPE5) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  // Non shared ADC ISRs
  #if (L3_PERIPHERAL_ENABLE_ADC0 == L3_ON)
  void ADC0_IRQHandler(void)
  {
    //AdcIrqHandler((stc_adcn_t*)&ADC0, &(m_astcAdcInstanceDataLut[AdcInstanceIndexAdc0].stcInternData));
  }
  #endif

  #if (L3_PERIPHERAL_ENABLE_ADC1 == L3_ON)
  void ADC1_IRQHandler(void)
  {
    AdcIrqHandler((stc_adcn_t*)&ADC1, &(m_astcAdcInstanceDataLut[AdcInstanceIndexAdc1].stcInternData));
  }
  #endif

  #if (L3_PERIPHERAL_ENABLE_ADC2 == L3_ON)
  void ADC2_IRQHandler(void)
  {
    AdcIrqHandler((stc_adcn_t*)&ADC2, &(m_astcAdcInstanceDataLut[AdcInstanceIndexAdc2].stcInternData));
  }
  #endif

  // BT0-7 ISRs
  #if defined(L3_PERIPHERAL_BT_ACTIVE)
  void BT_IRQHandler(void)
  {
    BtIrqHandler();
  }
  #endif

  // DT ISR not shared with QPRC
  #if ((L3_PERIPHERAL_ENABLE_QPRC0 != L3_ON)  && \
       (L3_PERIPHERAL_ENABLE_QPRC1 != L3_ON)) && \
       (L3_PERIPHERAL_ENABLE_DT0   == L3_ON)
  void DT_Handler(void)
  {
    DtIrqHandler0((stc_dtn_t*)&DT0, &(m_astcDtInstanceDataLut[DtInstanceIndexDt0].stcInternData));
  }
  
  // QPRC ISR not shared with DT
  #elif ((L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)  || \
         (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)) && \
         (L3_PERIPHERAL_ENABLE_DT0   != L3_ON)
  void DT_Handler(void)
  {
    #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ06MON & IRQ06MON_QPRC0_BITMASK_A) != 0)
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ41MON & IRQ41MON_QPRC0_BITMASK_A) != 0)
      #else
        #error Device does not support a QPRC!
      #endif
    {
      QprcIrqHandler((stc_qprcn_t*)&QPRC0, &(m_astcQprcInstanceDataLut[QprcInstanceIndexQprc0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
    
    #if (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ06MON & IRQ06MON_QPRC1_BITMASK_A) != 0)
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ41MON & IRQ41MON_QPRC1_BITMASK_A) != 0)
      #else
        #error Device does not support a QPRC!
      #endif
    {
      QprcIrqHandler((stc_qprcn_t*)&QPRC1, &(m_astcQprcInstanceDataLut[QprcInstanceIndexQprc0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
  }

  // DT and QPRC have shared ISR
  #elif ((L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)  || \
         (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)) && \
         (L3_PERIPHERAL_ENABLE_DT0   == L3_ON)
  void DT_Handler(void)
  {
    // Check for DT interrupt 
    #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ06MON & IRQ06MON_DT0_BITMASK_A) != 0)
    #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ14MON & IRQ14MON_DT0_BITMASK_B) != 0)
    #else
      #error Device does not support a Dual Timer!
    #endif
    {
      DtIrqHandler0((stc_dtn_t*)&DT0, &(m_astcDtInstanceDataLut[DtInstanceIndexDt0].stcInternData));
    }
    
    #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ06MON & IRQ06MON_QPRC0_BITMASK_A) != 0)
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ41MON & IRQ41MON_QPRC0_BITMASK_B) != 0)
      #else
        #error Device does not support a QPRC!
      #endif
    {
      QprcIrqHandler((stc_qprcn_t*)&QPRC0, &(m_astcQprcInstanceDataLut[QprcInstanceIndexQprc0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
    
    #if (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ06MON & IRQ06MON_QPRC1_BITMASK_A) != 0)
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ41MON & IRQ41MON_QPRC1_BITMASK_A) != 0)
      #else
        #error Device does not support a QPRC!
      #endif
    {
      QprcIrqHandler((stc_qprcn_t*)&QPRC1, &(m_astcQprcInstanceDataLut[QprcInstanceIndexQprc0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON)
  }
  #endif

  // None-shared CAN INT ISRs
  #if (L3_PERIPHERAL_ENABLE_CAN0 == L3_ON)
  void CAN0_IRQHandler(void)
  {
    CanIrqHandler((stc_cann_t*)&CAN0, &(m_astcCanInstanceDataLut[CanInstanceIndexCan0].stcInternData));
  }
  #endif

  #if (L3_PERIPHERAL_ENABLE_CAN1 == L3_ON)
  void CAN1_IRQHandler(void)
  {
    CanIrqHandler((stc_cann_t*)&CAN1, &(m_astcCanInstanceDataLut[CanInstanceIndexCan1].stcInternData));
  }
  #endif

  // None-shared CSV INT ISR
  #if (L3_PERIPHERAL_ENABLE_CSV == L3_ON)
  void CSV_Handler(void)
  {
    CsvIrqHandler();
  }
  #endif

  // None-shared EXT INT ISRs
  #if (L3_PERIPHERAL_ENABLE_EXINT0_7 == L3_ON)
  void INT0_7_Handler(void)
  {
    ExintIrqHandler0_7();
  }
  #endif

  #if (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
  void INT8_15_Handler(void)
  {
    ExintIrqHandler8_15();
  }
  #endif

  // None-shared LVD ISR
  #if (L3_PERIPHERAL_ENABLE_LVD == L3_ON)
  void LVD_Handler(void)
  {
    LvdIrqHandler();
  }
  #endif

  // None-shared MFSn ISRs
  #if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
  void MFS0TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS0, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs0].stcInternData));
  }
  /*
  void MFS0RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS0, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs0].stcInternData));
  }*/
  #endif // #if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON)
  void MFS1TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS1, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs1].stcInternData));
  }
  
  void MFS1RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS1, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs1].stcInternData));
  }
  #endif // #if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON)
  void MFS2TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS2, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs2].stcInternData));
  }
  
  void MFS2RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS2, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs2].stcInternData));
  }
  #endif // #if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON)
  void MFS3TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS3, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs3].stcInternData));
  }
  
  void MFS3RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS3, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs3].stcInternData));
  }
  #endif // #if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON)
  void MFS4TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS4, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs4].stcInternData));
  }
  
  void MFS4RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS4, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs4].stcInternData));
  }
  #endif // #if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON)
  void MFS5TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS5, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs5].stcInternData));
  }
  
  void MFS5RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS5, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs5].stcInternData));
  }
  #endif // #if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON)
  void MFS6TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS6, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs6].stcInternData));
  }
  
  void MFS6RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS6, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs6].stcInternData));
  }
  #endif //  #if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON)
  
  #if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)
  void MFS7TX_IRQHandler(void)
  {
    MfsIrqHandlerTx((stc_mfsn_t*)&MFS7, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs7].stcInternData));
  }
  
  void MFS7RX_IRQHandler(void)
  {
    MfsIrqHandlerRx((stc_mfsn_t*)&MFS7, &(m_astcMfsInstanceDataLut[MfsInstanceIndexMfs7].stcInternData));
  }
  #endif //  #if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)

  // Multi Function Timers
  #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON) || \
      (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)

    #if (L3_INTERRUPT_TYPE != L3_INTERRUPT_TYPE_A)
      #error LLL does not support 
    #endif

  void MFT_WG_IRQHandler(void)
  {
    #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
      #if (L3_DEVICE_TYPE != L3_TYPE3) && (L3_DEVICE_TYPE != L3_TYPE7)
    if ((FM3_INTREQ->IRQ03MON & IRQ03MON_MFT0_WFG_BITMASK_A) != 0)  // WAE0INT bit(s) active?
      #else
    if ((FM3_INTREQ->IRQ03MON & IRQ03MON_MFT_WFG_BITMASK_A) != 0)  // WAEINT bit(s) active? 
      #endif
    {
      MftWfgIrqHandler((stc_mftn_t*)&MFT0, &(m_astcMftInstanceDataLut[MftInstanceIndexMft0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    if ((FM3_INTREQ->IRQ03MON & IRQ03MON_MFT1_WFG_BITMASK_A) != 0)  // WAE1INT bit(s) active?
    {
      MftWfgIrqHandler((stc_mftn_t*)&MFT1, &(m_astcMftInstanceDataLut[MftInstanceIndexMft1].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
  }

  void MFT_FRT_IRQHandler(void)
  {
    #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
      #if (L3_DEVICE_TYPE != L3_TYPE3) && (L3_DEVICE_TYPE != L3_TYPE7)
    if ((FM3_INTREQ->IRQ28MON & IRQ28MON_MFT0_FRT_BITMASK_A) != 0)  // FRT0INT bit(s) active?
      #else
    if ((FM3_INTREQ->IRQ25MON & IRQ25MON_MFT_FRT_BITMASK_A) != 0)  // FRTINT bit(s) active? 
      #endif
    {
      MftFrtIrqHandler((stc_mftn_t*)&MFT0, &(m_astcMftInstanceDataLut[MftInstanceIndexMft0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    if ((FM3_INTREQ->IRQ28MON & IRQ28MON_MFT1_FRT_BITMASK) != 0)  // FRT1INT bit(s) active?
    {
      MftFrtIrqHandler((stc_mftn_t*)&MFT1, &(m_astcMftInstanceDataLut[MftInstanceIndexMft1].stcInternData));
    }  
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
  }

  void MFT_IPC_IRQHandler(void)
  {
    #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
      #if (L3_DEVICE_TYPE != L3_TYPE3) && (L3_DEVICE_TYPE != L3_TYPE7)
    if ((FM3_INTREQ->IRQ29MON & IRQ29MON_MFT0_ICU_BITMASK_A) != 0)  // ICU0INT bit(s) active?
      #else
    if ((FM3_INTREQ->IRQ26MON & IRQ26MON_MFT_ICU_BITMASK_A) != 0)  // ICUINT bit(s) active? 
      #endif
    {
      MftIcuIrqHandler((stc_mftn_t*)&MFT0, &(m_astcMftInstanceDataLut[MftInstanceIndexMft0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    if ((FM3_INTREQ->IRQ29MON & IRQ29MON_MFT1_ICU_BITMASK) != 0)  // ICU1INT bit(s) active?
    {  
      MftIcuIrqHandler((stc_mftn_t*)&MFT1, &(m_astcMftInstanceDataLut[MftInstanceIndexMft1].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
  }
  
  void MFT_OPC_IRQHandler(void)
  {
    #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
      #if (L3_DEVICE_TYPE != L3_TYPE3) && (L3_DEVICE_TYPE != L3_TYPE7)
    if ((FM3_INTREQ->IRQ30MON & IRQ30MON_MFT0_OCU_BITMASK_A) != 0)  // OCU0INT bit(s) active?
      #else
    if ((FM3_INTREQ->IRQ27MON & IRQ27MON_MFT_OCU_BITMASK_A) != 0)  // OCUINT bit(s) active? 
      #endif
    {
      MftOcuIrqHandler((stc_mftn_t*)&MFT0, &(m_astcMftInstanceDataLut[MftInstanceIndexMft0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    if ((FM3_INTREQ->IRQ30MON & IRQ30MON_MFT1_OCU_BITMASK) != 0)  // OCU1INT bit(s) active?
    {
      MftOcuIrqHandler((stc_mftn_t*)&MFT1, &(m_astcMftInstanceDataLut[MftInstanceIndexMft1].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
  }

  void PPG_Handler(void)
  {
    #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
      #if (L3_DEVICE_TYPE != L3_TYPE3) && (L3_DEVICE_TYPE != L3_TYPE7)
    if ((FM3_INTREQ->IRQ23MON & IRQ23MON_MFT0_PPG_BITMASK_A) != 0)  // PPG0INT bit(s) active?
      #else
    if ((FM3_INTREQ->IRQ22MON & IRQ22MON_MFT_PPG_BITMASK_A) != 0)  // PPGINT bit(s) active? 
      #endif
    {
      MftPpgIrqHandler((stc_ppgn_t*)&PPG_MFT0, &(m_astcMftInstanceDataLut[MftInstanceIndexMft0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    if ((FM3_INTREQ->IRQ23MON & IRQ23MON_MFT1_PPG_BITMASK_A) != 0)  // PPG1INT bit(s) active?
    {
      MftPpgIrqHandler((stc_ppgn_t*)&PPG_MFT1, &(m_astcMftInstanceDataLut[MftInstanceIndexMft1].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)     
  }
  #endif //  #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON) || (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)

  #if (L3_PERIPHERAL_ENABLE_CLK  == L3_ON) || \
      (L3_PERIPHERAL_ENABLE_WC0  == L3_ON) || \
      (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
  void TIM_IRQHandler(void)
  {
    // Real Time Clock
    #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A) 
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_RTC0_INT_BITMASK_A) != 0)
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_RTC0_INT_BITMASK_B) != 0)  
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_C)
    if ((FM3_INTREQ->IRQ23MON & IRQ23MON_RTC0_INT_BITMASK_C) != 0)
      #else
        #error Unknown Interrupt Type!
      #endif
    {
      RtcIrqHander((stc_rtcn_t*)&RTC0, &(m_astcRtcInstanceDataLut[RtcInstanceIndexRtc0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
    
    // Watch Counter
    #if (L3_PERIPHERAL_ENABLE_WC0 == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_WC0_INT_BITMASK_A) != 0)  // Clock interrupts active?
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_WC0_INT_BITMASK_B) != 0)  // Clock interrupts active?
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_C)
        #error Device does not support a Watch Counter!
      #else
        #error Unknown Interrupt Type!
      #endif
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_WC0_INT_BITMASK) != 0)  // Watch Counter active?
    {
      WcIrqHandler0((stc_wcn_t*)&WC0, &(m_astcWcInstanceDataLut[WcInstanceIndexWc0].stcInternData));
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_WC0 == L3_ON)

    // Clock Stabilization
    #if (L3_PERIPHERAL_ENABLE_CLK == L3_ON)
      #if (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_A)
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_CLK_INT_BITMASK_A) != 0)  // Clock interrupts active?
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_B)
    if ((FM3_INTREQ->IRQ24MON & IRQ24MON_CLK_INT_BITMASK_B) != 0)  // Clock interrupts active?
      #elif (L3_INTERRUPT_TYPE == L3_INTERRUPT_TYPE_C)
    if ((FM3_INTREQ->IRQ23MON & IRQ23MON_CLK_INT_BITMASK_C) != 0)  // Clock interrupts active? 
      #else
        #error Unknown Interrupt Type!
      #endif
    {
      ClkIrqHandler();
    }
    #endif // #if (L3_PERIPHERAL_ENABLE_CLK == L3_ON)
  }
  #endif // #if (L3_PERIPHERAL_ENABLE_CLK  == L3_ON) || (L3_PERIPHERAL_ENABLE_WC0  == L3_ON) || (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON

  // Software Wachdog Timer
  #if (L3_PERIPHERAL_ENABLE_SWWDG == L3_ON)
  void SWDT_Handler(void)
  {
    WdgIrqHandlerSwWdg();
  }
  #endif

  // Shared Hardware Watchdog Timer and NMI
  #if (L3_PERIPHERAL_ENABLE_HWWDG == L3_ON) && \
      (L3_PERIPHERAL_ENABLE_NMI   == L3_OFF)
  void NMI_Handler(void)
  {
    if (FM3_HWWDT->WDG_RIS_f.RIS == 1)
    {
      WdgIrqHandlerHwWdg();
    }
  }
  #elif (L3_PERIPHERAL_ENABLE_HWWDG == L3_OFF) && \
        (L3_PERIPHERAL_ENABLE_NMI   == L3_ON)
  void NMI_Handler(void)  
  {
    if (FM3_INTREQ->EXC02MON.NMI == 1)
    {
      ExintNMIrqHandler();
    }
  }
  #elif (L3_PERIPHERAL_ENABLE_HWWDG == L3_ON) && \
        (L3_PERIPHERAL_ENABLE_NMI   == L3_ON)
  void NMI_Handler(void)  
  { 
    if (FM3_HWWDT->WDG_RIS_f.RIS == 1)
    {
      WdgIrqHandlerHwWdg();
    }
    
    if (FM3_INTREQ->EXC02MON.NMI == 1)
    {
      ExintNMIrqHandler();
    }
  }
  #endif

  #if (L3_PERIPHERAL_ENABLE_USB0_DEVICE == L3_ON)
  void USBF_Handler(void)
  {
      UsbIrqHandlerF((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
  }

  void USB0F_Handler(void)
  {
      UsbIrqHandlerF((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
  }
  #endif

  #if ((L3_PERIPHERAL_ENABLE_USB0_DEVICE == L3_ON) || (L3_PERIPHERAL_ENABLE_USB0_HOST == L3_ON))
  void USB_Handler(void)
  {
      UsbIrqHandler((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
  }

  void USB0_Handler(void)
  {
      UsbIrqHandler((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
  }
  #endif

  #if (L3_PERIPHERAL_ENABLE_USB1_DEVICE == L3_ON)
  void USB1F_Handler(void)
  {
      UsbIrqHandlerF((stc_usbn_t*)&USB1, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb1].stcInternData));
  }
  #endif

  #if ((L3_PERIPHERAL_ENABLE_USB1_DEVICE == L3_ON) || (L3_PERIPHERAL_ENABLE_USB1_HOST == L3_ON))
  void USB1_Handler(void)
  {
      UsbIrqHandler((stc_usbn_t*)&USB1, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb1].stcInternData));
  }
  #endif


#else
  #error Device Type not supported!
#endif // #if (L3_DEVICE_TYPE == L3_TYPE0) || ...

//@} // L3Interrupts

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
