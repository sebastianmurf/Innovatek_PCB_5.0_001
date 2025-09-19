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

#ifndef __USBETHERNETCLOCK_H__
#define __USBETHERNETCLOCK_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/

#include "mcu.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif
  
/**
 ******************************************************************************
 ** \defgroup UsbEthernetGroup USB/Ethernet Clock Module description
 **
 ** Provided functions of USB Ethernet Clock module:
 ** 
 ** - UsbEthernetClock_Init()
 ** - UsbEthernetClock_Deinit()
 **
 ** Used to initialize the USB Ethernet Clock PLL, devider and source selector
 **
 ******************************************************************************/
//@{

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USBETHERNETCLOCK_USE_USB      1
#define USBETHERNETCLOCK_USE_ETHERNET 0 
#define USBETHERNETCLOCK_USE_L3       0  

#define ETHERNETCLOCK_OUTFREQ FREQ_25MHZ  //< Set required source clock frequency for ethernet (25MHZ or 50MHZ)

#if USBETHERNETCLOCK_USE_USB == 1
    #include "usb.h" /* if error happens, set USBETHERNETCLOCK_USE_USB to 0 */
#endif

#if USBETHERNETCLOCK_USE_ETHERNET == 1
    //#include "ethernet.h" /* if error happens, set USBETHERNETCLOCK_USE_ETHERNET to 0 */
#endif

#if USBETHERNETCLOCK_USE_L3 == 1
    #include "l3.h" /* if error happens, set USBETHERNETCLOCK_USE_L3 to 0 */
#endif
  
#ifndef L3_OFF
    #define L3_OFF 0
#endif

#ifndef L3_ON
    #define L3_ON 1
#endif
      
#ifndef FREQ_4MHZ
    #define FREQ_4MHZ  ( 4000000UL)
#endif

#ifndef FREQ_8MHZ
    #define FREQ_8MHZ  ( 8000000UL)
#endif
#ifndef FREQ_16MHZ
    #define FREQ_16MHZ (16000000UL)
#endif
#ifndef FREQ_24MHZ  
    #define FREQ_24MHZ (24000000UL)
#endif

#ifndef FREQ_48MHZ  
    #define FREQ_48MHZ (48000000UL)
#endif

#ifndef FREQ_25MHZ  
    #define FREQ_25MHZ (25000000UL)
#endif

#ifndef FREQ_50MHZ  
    #define FREQ_50MHZ (50000000UL)
#endif

#ifndef FREQ_96MHZ  
    #define FREQ_96MHZ (96000000UL)
#endif

#ifndef FREQ_144MHZ  
    #define FREQ_144MHZ (144000000UL)
#endif
  
//#define MAINCRYSTALFREQ 4MHZ   // <<< External   4MHz Crystal

#if ((!defined(__CLKMO)) && (!defined(MAINCRYSTALFREQ)))
    #error __CLKMO does not exist, please specify crystal freqency in MAINCRYSTALFREQ
#endif

#if !defined(MAINCRYSTALFREQ)
    #define MAINCRYSTALFREQ __CLKMO
#endif


#ifndef FM3MCUTYPE_TYPE0
    #define FM3MCUTYPE_TYPE0 0
#endif
#ifndef FM3MCUTYPE_TYPE1
    #define FM3MCUTYPE_TYPE1 1
#endif
#ifndef FM3MCUTYPE_TYPE2
    #define FM3MCUTYPE_TYPE2 2
#endif
#ifndef FM3MCUTYPE_TYPE3
    #define FM3MCUTYPE_TYPE3 3
#endif
#ifndef FM3MCUTYPE_TYPE4
    #define FM3MCUTYPE_TYPE4 4
#endif
#ifndef FM3MCUTYPE_TYPE5
    #define FM3MCUTYPE_TYPE5 5
#endif
#ifndef FM3MCUTYPE_TYPE6
    #define FM3MCUTYPE_TYPE6 6
#endif
#ifndef FM3MCUTYPE_TYPE7
    #define FM3MCUTYPE_TYPE7 7
#endif


#ifdef FM3_DEVICE_TYPE 
    #ifndef FM3MCUTYPE
         #define FM3MCUTYPE FM3_DEVICE_TYPE
    #endif
#endif

#ifndef FM3MCUTYPE
#ifdef _MB9B120K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B320K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B420K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B520K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B120L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B320L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B420L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B520L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B120M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B320M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B420M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B520M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE9
#endif
#ifdef _MB9B500N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B400N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B300N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B100N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B500R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B400R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B300R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9B100R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9BD10S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B610S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B510S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B410S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B310S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B210S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B110S_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9BD10T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B610T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B210T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B510T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B410T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B310T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B110T_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE2
#endif
#ifdef _MB9B510N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B410N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B310N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B110N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B510R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B410R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B310R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9B110R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE4
#endif
#ifdef _MB9A310K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE5
#endif
#ifdef _MB9A110K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE5
#endif
#ifdef _MB9A310L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE1
#endif
#ifdef _MB9A110L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE1
#endif
#ifdef _MB9A310M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE1
#endif
#ifdef _MB9A110M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE1
#endif
#ifdef _MB9A310N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE1
#endif
#ifdef _MB9A100N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9A110N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE1
#endif
#ifdef _MB9A100R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE0
#endif
#ifdef _MB9AB40L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9AA40L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A340L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A140L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9AB40M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9AA40M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A340M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A140M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9AB40N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9AA40N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A340N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A140N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE6
#endif
#ifdef _MB9A150M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE8
#endif
#ifdef _MB9A150N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE8
#endif
#ifdef _MB9A150R_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE8
#endif
#ifdef _MB9A130K_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE3
#endif
#ifdef _MB9A130L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE3
#endif
#ifdef _MB9AA30L_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE7
#endif
#ifdef _MB9AA30M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE7
#endif
#ifdef _MB9A130M_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE7
#endif
#ifdef _MB9AA30N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE7
#endif
#ifdef _MB9A130N_H_
    #define FM3MCUTYPE FM3MCUTYPE_TYPE7
#endif
#endif

#if FM3MCUTYPE == FM3MCUTYPE_TYPE0
    #if   MAINCRYSTALFREQ == FREQ_4MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 24
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_8MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 12
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_16MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 6
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_24MHZ
        #define USBETHERNETCLOCK_PLL_K 2
        #define USBETHERNETCLOCK_PLL_N 8
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_48MHZ
        #define USBETHERNETCLOCK_UCSEL 0x00 // CLKMO
    #else
        #error NOT SUPPORTED CRYSTAL / PLL SETTING FOR USB / ETHERNET CLOCK
    #endif
#elif ((FM3MCUTYPE == FM3MCUTYPE_TYPE1) || ((FM3MCUTYPE == FM3MCUTYPE_TYPE4) || (FM3MCUTYPE == FM3MCUTYPE_TYPE5)))
    #if   MAINCRYSTALFREQ == FREQ_4MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 60
        #define USBETHERNETCLOCK_PLL_M 5
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_8MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 30
        #define USBETHERNETCLOCK_PLL_M 5
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_16MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 15
        #define USBETHERNETCLOCK_PLL_M 5
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_24MHZ
        #define USBETHERNETCLOCK_PLL_K 2
        #define USBETHERNETCLOCK_PLL_N 20
        #define USBETHERNETCLOCK_PLL_M 5
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_48MHZ
        #define USBETHERNETCLOCK_UCSEL 0x00 // CLKMO
    #else
        #error NOT SUPPORTED CRYSTAL / PLL SETTING FOR USB / ETHERNET CLOCK
    #endif
#elif (FM3MCUTYPE == FM3MCUTYPE_TYPE6)
    #if   MAINCRYSTALFREQ == FREQ_4MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 24
        #define USBETHERNETCLOCK_PLL_M 1
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_8MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 12
        #define USBETHERNETCLOCK_PLL_M 1
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_16MHZ
        #define USBETHERNETCLOCK_PLL_K 1
        #define USBETHERNETCLOCK_PLL_N 6
        #define USBETHERNETCLOCK_PLL_M 1
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_24MHZ
        #define USBETHERNETCLOCK_PLL_K 2
        #define USBETHERNETCLOCK_PLL_N 8
        #define USBETHERNETCLOCK_PLL_M 1
        #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
    #elif MAINCRYSTALFREQ == FREQ_48MHZ
        #define USBETHERNETCLOCK_UCSEL 0x00 // CLKMO
    #else
        #error NOT SUPPORTED CRYSTAL / PLL SETTING FOR USB / ETHERNET CLOCK
    #endif
#elif (FM3MCUTYPE == FM3MCUTYPE_TYPE2)
    #if ((USBETHERNETCLOCK_USE_USB == 1) && (USBETHERNETCLOCK_USE_ETHERNET == 0))
        #if   MAINCRYSTALFREQ == FREQ_4MHZ
            #define USBETHERNETCLOCK_PLL_K 1
            #define USBETHERNETCLOCK_PLL_N 60
            #define USBETHERNETCLOCK_PLL_M 5
            #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
        #elif MAINCRYSTALFREQ == FREQ_8MHZ
            #define USBETHERNETCLOCK_PLL_K 1
            #define USBETHERNETCLOCK_PLL_N 30
            #define USBETHERNETCLOCK_PLL_M 5
            #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
        #elif MAINCRYSTALFREQ == FREQ_16MHZ
            #define USBETHERNETCLOCK_PLL_K 1
            #define USBETHERNETCLOCK_PLL_N 15
            #define USBETHERNETCLOCK_PLL_M 5
            #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
        #elif MAINCRYSTALFREQ == FREQ_24MHZ
            #define USBETHERNETCLOCK_PLL_K 2
            #define USBETHERNETCLOCK_PLL_N 20
            #define USBETHERNETCLOCK_PLL_M 5
            #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
        #elif MAINCRYSTALFREQ == FREQ_25MHZ
            #define USBETHERNETCLOCK_PLL_K 2
            #define USBETHERNETCLOCK_PLL_N 48
            #define USBETHERNETCLOCK_PLL_M 5
            #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
        #elif MAINCRYSTALFREQ == FREQ_48MHZ
            #define USBETHERNETCLOCK_UCSEL 0x00 // CLKMO
        #elif MAINCRYSTALFREQ == FREQ_50MHZ
            #define USBETHERNETCLOCK_PLL_K 10
            #define USBETHERNETCLOCK_PLL_N 48
            #define USBETHERNETCLOCK_PLL_M 5
            #define USBETHERNETCLOCK_UCSEL 0x01 // USB/Ethernet-PLL oscillation clock
        #else
            #error NOT SUPPORTED CRYSTAL / PLL SETTING FOR USB / ETHERNET CLOCK
        #endif
    #else
        #if ETHERNETCLOCK_OUTFREQ == FREQ_25MHZ
            #if   MAINCRYSTALFREQ == FREQ_4MHZ
                #define USBETHERNETCLOCK_PLL_K 1
                #define USBETHERNETCLOCK_PLL_N 50
                #define USBETHERNETCLOCK_PLL_M 8
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_8MHZ
                #define USBETHERNETCLOCK_PLL_K 1
                #define USBETHERNETCLOCK_PLL_N 25
                #define USBETHERNETCLOCK_PLL_M 8
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_16MHZ
                #define USBETHERNETCLOCK_PLL_K 2
                #define USBETHERNETCLOCK_PLL_N 25
                #define USBETHERNETCLOCK_PLL_M 8
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_24MHZ
                #define USBETHERNETCLOCK_PLL_K 6
                #define USBETHERNETCLOCK_PLL_N 50
                #define USBETHERNETCLOCK_PLL_M 8
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_50MHZ 
                #define USBETHERNETCLOCK_ECSEL 0x00 // CLKMO
            #elif MAINCRYSTALFREQ == FREQ_48MHZ
                #define USBETHERNETCLOCK_PLL_K 6
                #define USBETHERNETCLOCK_PLL_N 25
                #define USBETHERNETCLOCK_PLL_M 8
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_50MHZ
                #define USBETHERNETCLOCK_PLL_K 5
                #define USBETHERNETCLOCK_PLL_N 20
                #define USBETHERNETCLOCK_PLL_M 8
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #else
                #error NOT SUPPORTED CRYSTAL / PLL SETTING FOR USB / ETHERNET CLOCK
            #endif
        #elif ETHERNETCLOCK_OUTFREQ == FREQ_50MHZ
            #if   MAINCRYSTALFREQ == FREQ_4MHZ
                #define USBETHERNETCLOCK_PLL_K 1
                #define USBETHERNETCLOCK_PLL_N 50
                #define USBETHERNETCLOCK_PLL_M 3
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_8MHZ
                #define USBETHERNETCLOCK_PLL_K 1
                #define USBETHERNETCLOCK_PLL_N 25
                #define USBETHERNETCLOCK_PLL_M 4
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_16MHZ
                #define USBETHERNETCLOCK_PLL_K 2
                #define USBETHERNETCLOCK_PLL_N 25
                #define USBETHERNETCLOCK_PLL_M 4
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_24MHZ
                #define USBETHERNETCLOCK_PLL_K 3
                #define USBETHERNETCLOCK_PLL_N 50
                #define USBETHERNETCLOCK_PLL_M 4
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_25MHZ
                #define USBETHERNETCLOCK_PLL_K 5
                #define USBETHERNETCLOCK_PLL_N 40
                #define USBETHERNETCLOCK_PLL_M 4
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #elif MAINCRYSTALFREQ == FREQ_48MHZ
                #define USBETHERNETCLOCK_PLL_K 6
                #define USBETHERNETCLOCK_PLL_N 25
                #define USBETHERNETCLOCK_PLL_M 4
                #define USBETHERNETCLOCK_ECSEL 0x01 // USB/Ethernet-PLL oscillation clock
            #else
                #error NOT SUPPORTED CRYSTAL / PLL SETTING FOR USB / ETHERNET CLOCK
            #endif
        #else
            #error ETHERNETCLOCK must be 25MHz or 50MHz
        #endif
 
        #if MAINCRYSTALFREQ == FREQ_48MHZ
            #define USBETHERNETCLOCK_UCSEL 0x00 // CLKMO
        #elif __PLLCLK == FREQ_144MHZ
            #define USBETHERNETCLOCK_UCSEL 0x02    // CLKPLL division clock
            #define USBETHERNETCLOCK_UBSR  (3 - 1) // Clock divided by 3
        #elif __PLLCLK == FREQ_96MHz
            #define USBETHERNETCLOCK_UCSEL 0x02    // CLKPLL division clock
            #define USBETHERNETCLOCK_UBSR  (2 - 1) // Clock divided by 3
        #else
            #if (USBETHERNETCLOCK_USE_USB == 1)
                #error USB Clock Setting not supported
            #endif
        #endif
    #endif
#else
    #error FM3 MCU TYPE UNKNOWN
#endif
  
#ifndef L3_PERIPHERAL_ENABLE_USB0 
    #define L3_PERIPHERAL_ENABLE_USB0 L3_OFF
#endif
  
#ifndef L3_PERIPHERAL_ENABLE_USB1
    #define L3_PERIPHERAL_ENABLE_USB1 L3_OFF
#endif


  
/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

void UsbEthernetClock_Init(void);
void UsbEthernetClock_Deinit(void);
#ifdef __cplusplus
}
#endif

//@} // UsbEthernetClockGroup

#endif /* __USBETHERNETCLOCK_H__ */

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/



