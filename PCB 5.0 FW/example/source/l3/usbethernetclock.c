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
/** \file usbethernetclock.h
 **
 ** Headerfile for USB / Ethernet functions
 ** @link UsbEthernetGroup USB/Ethernet Clock Module description @endlink
 **
 ** History:
 **   - 2012-08-10  1.0  MSc  First version.
 **   - 2012-08-23  1.1  MSc  GNU compiler compatible
 **   - 2012-08-30  1.2  MSc  Correction in USB 0/1 clock enable
 **
 ******************************************************************************/

#include "usbethernetclock.h"
#include "base_types_l3.h"

/**
 ******************************************************************************
 ** \ingroup UsbEthernetGroup
 ******************************************************************************/
//@{

/*****************************************************************************/
/* Static variable declarations ('static', definition in C source)           */
/*****************************************************************************/

static boolean_t bUsbEthernetClockEnabled = FALSE;

/*****************************************************************************/
/* Global function  ('extern', definition in C source)                       */
/*****************************************************************************/

/**
 ******************************************************************************
 ** \brief Initalize PLL for USB / Ethernet Clock.
 **
 ******************************************************************************/
void UsbEthernetClock_Init(void)
{
    if (bUsbEthernetClockEnabled)
    {
        return;
    }
    #ifdef FM3_USBCLK_BASE
        FM3_USBCLK->UCCR = 0;
        bFM3_USBCLK_USBEN_USBEN = 0;
        FM3_USBCLK->UPCR1 = 0x00; /* PLL Disable */
    #endif
    #ifdef FM3_USBETHERNETCLK_BASE
        FM3_USBETHERNETCLK->UCCR = 0;
        #ifdef bFM3_USBCLK_USBEN0_USBEN
        bFM3_USBCLK_USBEN0_USBEN = 0;
        #endif
        #ifdef bFM3_USBETHERNETCLK_UCCR_UCEN0
        bFM3_USBETHERNETCLK_UCCR_UCEN0 = 0;
        #endif
        
        #ifdef bFM3_USBCLK_USBEN1_USBEN
        bFM3_USBCLK_USBEN1_USBEN = 0;
        #endif
        #ifdef bFM3_USBETHERNETCLK_UCCR_UCEN1
        bFM3_USBETHERNETCLK_UCCR_UCEN1 = 0;
        #endif
        FM3_USBETHERNETCLK->UPCR1 = 0x00; /* PLL Disable */
    #endif
        
        
    #ifndef USBETHERNETCLOCK_UCSEL
        #define USBETHERNETCLOCK_UCSEL 0x00
    #endif
    
    #ifndef USBETHERNETCLOCK_ECSEL
        #define USBETHERNETCLOCK_ECSEL 0x00
    #endif
    
    #if USBETHERNETCLOCK_PLL_K > 63
        #error USB ETHERNET CLOCK: (K - 1) must be < 64
    #endif

    #if ((FM3MCUTYPE == FM3MCUTYPE_TYPE6) || (FM3MCUTYPE == FM3MCUTYPE_TYPE0))
        #if USBETHERNETCLOCK_PLL_N == 0
            #error USB ETHERNET CLOCK: (N - 1) can't be 0 
        #endif
        #if USBETHERNETCLOCK_PLL_N > 63
            #error USB ETHERNET CLOCK: (N - 1) can't be > 63 
        #endif
    #endif
    #if ((FM3MCUTYPE == FM3MCUTYPE_TYPE1) || (FM3MCUTYPE == FM3MCUTYPE_TYPE4) || (FM3MCUTYPE == FM3MCUTYPE_TYPE5))
        #if USBETHERNETCLOCK_PLL_N < 13
            #error USB ETHERNET CLOCK: (N - 1) must be > 12
        #endif
        #if USBETHERNETCLOCK_PLL_N > 99
            #error USB ETHERNET CLOCK: (N - 1) can't be > 99 
        #endif
    #endif
        
    #if ((FM3MCUTYPE == FM3MCUTYPE_TYPE1) || (FM3MCUTYPE == FM3MCUTYPE_TYPE4) || (FM3MCUTYPE == FM3MCUTYPE_TYPE5))
        #if USBETHERNETCLOCK_PLL_M > 15
            #error USB ETHERNET CLOCK: (M - 1) can't be > 15 
        #endif
    #endif    
        
    #if ((FM3MCUTYPE == FM3MCUTYPE_TYPE6))
        #if USBETHERNETCLOCK_PLL_M > 15
            #error USB ETHERNET CLOCK: (M - 1) can't be > 15 
        #endif
    #endif    
        

    #ifdef FM3_USBETHERNETCLK_BASE
        FM3_USBETHERNETCLK->UPCR1 = 0x00; /* PLL Disable */
        FM3_USBETHERNETCLK->UPCR2 = 0x03; /* wait 1ms */
        
        #ifdef USBETHERNETCLOCK_PLL_K
            FM3_USBETHERNETCLK->UPCR3 = USBETHERNETCLOCK_PLL_K - 1;
        #endif
        #ifdef USBETHERNETCLOCK_PLL_K
            FM3_USBETHERNETCLK->UPCR4 = USBETHERNETCLOCK_PLL_N - 1;
        #endif
        #ifdef USBETHERNETCLOCK_PLL_M
            FM3_USBETHERNETCLK->UPCR5 = USBETHERNETCLOCK_PLL_M - 1;
        #endif          
    #else
        FM3_USBCLK->UPCR1 = 0x00; /* PLL Disable */
        FM3_USBCLK->UPCR2 = 0x03; /* wait 1ms */
        #ifdef USBETHERNETCLOCK_PLL_K
            FM3_USBCLK->UPCR3 = USBETHERNETCLOCK_PLL_K - 1;
        #endif
        #ifdef USBETHERNETCLOCK_PLL_K
            FM3_USBCLK->UPCR4 = USBETHERNETCLOCK_PLL_N - 1;
        #endif
        #ifdef USBETHERNETCLOCK_PLL_M
            FM3_USBCLK->UPCR5 = USBETHERNETCLOCK_PLL_M - 1;
        #endif          
    #endif
            
            
    #ifdef bFM3_USBCLK_UPCR1_UPLLEN
        bFM3_USBCLK_UPCR1_UPLLEN = 1;
        while(bFM3_USBCLK_UP_STR_UPRDY == 0);
        FM3_USBCLK->UCCR = 0x01 | (USBETHERNETCLOCK_UCSEL << 1);
        bFM3_USBCLK_USBEN_USBEN = 1;
    #endif
    #ifdef bFM3_USBETHERNETCLK_UPCR1_UPLLEN
        #ifdef USBETHERNETCLOCK_UBSR
            FM3_USBETHERNETCLK->UPCR6 = (USBETHERNETCLOCK_UBSR & 0x0F);
        #endif
        bFM3_USBETHERNETCLK_UPCR1_UPLLEN = 1;
        while(bFM3_USBETHERNETCLK_UP_STR_UPRDY == 0);
        #if ((L3_PERIPHERAL_ENABLE_ETHERNET_MAC0 == L3_ON) || (L3_PERIPHERAL_ENABLE_ETHERNET_MAC1 == L3_ON))
             bFM3_USBETHERNETCLK_UCCR_ECEN = 1;
        #endif  
        FM3_USBETHERNETCLK->UCCR = 0;
        #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)))
            FM3_USBETHERNETCLK->UCCR |= (USBETHERNETCLOCK_UCSEL << 1) | (USBETHERNETCLOCK_ECSEL << 5);
            
            #ifdef bFM3_USBETHERNETCLK_UCCR_UCEN0
            bFM3_USBETHERNETCLK_UCCR_UCEN0 = 1;
            #endif
            
            
            #ifdef bFM3_USBCLK_USBEN0_USBEN
            bFM3_USBCLK_USBEN0_USBEN = 1;
            #endif
            #ifdef bFM3_USBETHERNETCLK_USBEN0_USBEN0
            bFM3_USBETHERNETCLK_USBEN0_USBEN0 = 1;
            #endif
        #endif
        #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)))
            FM3_USBETHERNETCLK->UCCR |= (USBETHERNETCLOCK_UCSEL << 1) | (USBETHERNETCLOCK_ECSEL << 5);
            
            #ifdef bFM3_USBETHERNETCLK_UCCR_UCEN1
            bFM3_USBETHERNETCLK_UCCR_UCEN1 = 1;
            #endif
            
            
            #ifdef bFM3_USBCLK_USBEN1_USBEN
            bFM3_USBCLK_USBEN1_USBEN = 1;
            #endif
            #ifdef bFM3_USBETHERNETCLK_USBEN1_USBEN1
            bFM3_USBETHERNETCLK_USBEN1_USBEN1 = 1;
            #endif
        #endif
    #endif
    bUsbEthernetClockEnabled = TRUE;
}

/**
 ******************************************************************************
 ** \brief Deinitalize PLL for USB / Ethernet Clock.
 **
 ******************************************************************************/
void UsbEthernetClock_Deinit(void)
{
    #ifdef FM3_USBCLK_BASE
        FM3_USBCLK->UCCR = 0;
        bFM3_USBCLK_USBEN_USBEN = 0;
        FM3_USBCLK->UPCR1 = 0x00; /* PLL Disable */
    #endif
    #ifdef FM3_USBETHERNETCLK_BASE
        FM3_USBETHERNETCLK->UCCR = 0;
        #ifdef bFM3_USBCLK_USBEN0_USBEN
        bFM3_USBCLK_USBEN0_USBEN = 0;
        #endif
        #ifdef bFM3_USBETHERNETCLK_UCCR_UCEN0
        bFM3_USBETHERNETCLK_UCCR_UCEN0 = 0;
        #endif
        
        #ifdef bFM3_USBCLK_USBEN1_USBEN
        bFM3_USBCLK_USBEN1_USBEN = 0;
        #endif
        #ifdef bFM3_USBETHERNETCLK_UCCR_UCEN1
        bFM3_USBETHERNETCLK_UCCR_UCEN1 = 0;
        #endif
        FM3_USBETHERNETCLK->UPCR1 = 0x00; /* PLL Disable */
    #endif
    #ifdef FM3_USBETHERNETCLK_BASE
        FM3_USBETHERNETCLK->UPCR1 = 0x00; /* PLL Disable */
    #else
        FM3_USBCLK->UPCR1 = 0x00; /* PLL Disable */
    #endif
    bUsbEthernetClockEnabled = FALSE;
}
//@} // UsbEthernetClockGroup


