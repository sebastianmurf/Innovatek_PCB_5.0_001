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
/** \file interrupts.h
 **
 ** L3 Interrupt Handler
 **
 ** History:
 **   - 2012-02-21  1.0  MWi       First version.
 **   - 2012-04-05  1.1  MWi       IRQMON bit mask for QPDR and DT added;
 **                                L3_TYPE7 added
 **   - 2012-06-27  1.2  MWi       Interrupt types added, RTC added
 **   - 2012-08-30  1.3  MWi, MSc  USB added
 **
 ******************************************************************************/
#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "bt.h"
#include "can.h"
#include "clk.h"
#include "csv.h"
#include "dma.h"
#include "exint.h"
#include "adc.h"
#include "dt.h"
#include "lvd.h"
#include "mfs.h"
#include "mft.h"
#include "qprc.h"
#include "rtc.h"
#include "usb.h"
#include "wc.h"
#include "wdg.h"
   
/* C binding of definitions if building with C++ compiler                     */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup L3Interrupts L3 Interrupt handling
 **
 ** \brief Description of the L3 Interrupt handling
 **
 ** Each device group of the FM3 MCUs may have individual interrupt service
 ** routine vector names. For this reason this file defines the device
 ** depenent vectors, if it corresponding resource is set to active in l3.h.
 **
 ** The user has to check by himself, if an enabled resource is actually
 ** available on the used device!
 **
 ** Interrupt and Callback flow in L3:
 ** \image html Interrupt_flow.png
 **
 ** \attention IRQCMODER functionality is not implemented yet! Therfor the LLL
 **            only handles Interrupt A fully. Using other Interrupt types may
 **            cause in compilation errors!
 **
 ******************************************************************************/
//@{
  
/**
 ******************************************************************************
 ** Defines for MFTn IRQMON Bit Masks for ISR preselection in interrupts.c
 ******************************************************************************/  
#define L3_INTERRUPT_TYPE_A 0
#define L3_INTERRUPT_TYPE_B 1
#define L3_INTERRUPT_TYPE_C 2
  
#define L3_INTERRUPT_TYPE L3_INTERRUPT_TYPE_A  ///< Do not change!
  
// L3_INTERRUPT_TYPE_A
#define IRQ03MON_MFT0_WFG_BITMASK_A   0x0000000F
#define IRQ03MON_MFT1_WFG_BITMASK_A   0x000000F0
#define IRQ03MON_MFT2_WFG_BITMASK_A   0x00000F00

#define IRQ06MON_DT0_BITMASK_A        0x00000003

#define IRQ06MON_QPRC0_BITMASK_A      0x000000FC
#define IRQ06MON_QPRC1_BITMASK_A      0x00003F00
#define IRQ06MON_QPRC2_BITMASK_A      0x000FC000

#define IRQ23MON_MFT0_PPG_BITMASK_A   0x00000007
#define IRQ23MON_MFT1_PPG_BITMASK_A   0x00000038
#define IRQ23MON_MFT2_PPG_BITMASK_A   0x000001E0

#define IRQ24MON_CLK_INT_BITMASK_A    0x00000007  // UPLLINT not defined here!
#define IRQ24MON_WC0_INT_BITMASK_A    0x00000010
#define IRQ24MON_RTC0_INT_BITMASK_A   0x00000020

#define IRQ28MON_MFT0_FRT_BITMASK_A   0x0000003F
#define IRQ28MON_MFT1_FRT_BITMASK_A   0x00000FC0
#define IRQ28MON_MFT2_FRT_BITMASK_A   0x0003F000 

#define IRQ29MON_MFT0_ICU_BITMASK_A   0x0000000F
#define IRQ29MON_MFT1_ICU_BITMASK_A   0x000000F0
#define IRQ29MON_MFT2_ICU_BITMASK_A   0x00000F00

#define IRQ30MON_MFT0_OCU_BITMASK_A   0x0000003F
#define IRQ30MON_MFT1_OCU_BITMASK_A   0x00000FC0
#define IRQ30MON_MFT2_OCU_BITMASK_A   0x0003F000

// L3_INTERRUPT_TYPE_B
#define IRQ11MON_MFT0_WFG_BITMASK_B   0x0000000F
#define IRQ39MON_MFT1_WFG_BITMASK_B   0x0000000F
#define IRQ40MON_MFT2_WFG_BITMASK_B   0x0000000F

#define IRQ14MON_DT0_BITMASK_B        0x00000003

#define IRQ41MON_QPRC0_BITMASK_B      0x000000FC
#define IRQ41MON_QPRC1_BITMASK_B      0x0000003F
#define IRQ47MON_QPRC2_BITMASK_B      0x000007E0

#define IRQ23MON_MFT0_PPG_BITMASK_B   0x00000007
#define IRQ23MON_MFT1_PPG_BITMASK_B   0x00000038
#define IRQ23MON_MFT2_PPG_BITMASK_B   0x000001E0

#define IRQ24MON_CLK_INT_BITMASK_B    0x00000007  // UPLLINT not defined here!
#define IRQ24MON_WC0_INT_BITMASK_B    0x00000010
#define IRQ24MON_RTC0_INT_BITMASK_B   0x00000020

#define IRQ28MON_MFT0_FRT_BITMASK_B   0x0000003F
#define IRQ29MON_MFT1_FRT_BITMASK_B   0x0000003F
#define IRQ30MON_MFT2_FRT_BITMASK_B   0x0000003F

#define IRQ28MON_MFT0_ICU_BITMASK_B   0x000003C0
#define IRQ29MON_MFT1_ICU_BITMASK_B   0x000003C0
#define IRQ30MON_MFT2_ICU_BITMASK_B   0x000003C0

#define IRQ28MON_MFT0_OCU_BITMASK_B   0x0000FC00
#define IRQ29MON_MFT1_OCU_BITMASK_B   0x0000FC00
#define IRQ30MON_MFT2_OCU_BITMASK_B   0x0000FC00  

// L3_INTERRUPT_TYPE_C
#define IRQ03MON_MFT0_WFG_BITMASK_C   0x0000000F

#define IRQ22MON_MFT0_PPG_BITMASK_C   0x00000007

#define IRQ23MON_CLK_INT_BITMASK_C    0x00000007
#define IRQ23MON_RTC0_INT_BITMASK_C   0x00000020

#define IRQ25MON_MFT0_FRT_BITMASK_C   0x0000003F  

#define IRQ26MON_MFT0_ICU_BITMASK_C   0x0000000F

#define IRQ28MON_MFT0_OCU_BITMASK_C   0x0000003F
  
//@} // L3Interrupts
  
#ifdef __cplusplus
}
#endif  
  
#endif /* __INTERRUPTS_H__ */

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
