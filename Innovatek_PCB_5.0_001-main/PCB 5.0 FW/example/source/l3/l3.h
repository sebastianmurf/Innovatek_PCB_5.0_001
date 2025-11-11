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
/** \file l3.h
 **
 ** Headerfile for FM3 Low Level Library 
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-05-10  1.1  MWi  Parameter check for preprocessor switches added,
 **                           Package definititon added; peripheral base
 **                           addresses added to get more independent from
 **                           MCU's header files; Reset Cause added
 **   - 2012-07-05  1.2  MWi  Devices added for Type calculation; RTC, DSM
 **                           added
 **   - 2012-08-10  1.3  MWi  Osolete RTC_AVAILABLE check removed
 **
 *****************************************************************************/

#ifndef __L3_H__
#define __L3_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "base_types_l3.h"

/* C binding of definitions if building with C++ compiler                     */
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/

/**
 ******************************************************************************
 ** All definitions needed for l3_user.h are stated here 
 ******************************************************************************/ 
  
#define L3_ON  1    ///< Switches a feature on.
#define L3_OFF 0    ///< Switches a feature off.

/**
 ******************************************************************************
 ** Global Device type defintions
 ** Note that an existing defintion does not implicitly means full device
 ** type support of this libarary!
 ******************************************************************************/  
#define L3_TYPE0 0  ///< FM3 device type0
#define L3_TYPE1 1  ///< FM3 device type1
#define L3_TYPE2 2  ///< FM3 device type2
#define L3_TYPE3 3  ///< FM3 device type3
#define L3_TYPE4 4  ///< FM3 device type4
#define L3_TYPE5 5  ///< FM3 device type5
#define L3_TYPE6 6  ///< FM3 device type6
#define L3_TYPE7 7  ///< FM3 device type7

/**
 ******************************************************************************
 ** Global User Device Choice List
 ******************************************************************************/
#define L3_DEVICE_TYPE_MB9AF10X   0
#define L3_DEVICE_TYPE_MB9AF11X  10
#define L3_DEVICE_TYPE_MB9AF13X  20
#define L3_DEVICE_TYPE_MB9AF14X  25
#define L3_DEVICE_TYPE_MB9AF31X  30
#define L3_DEVICE_TYPE_MB9AF34X  33
#define L3_DEVICE_TYPE_MB9AFA3X  34
#define L3_DEVICE_TYPE_MB9AFA4X  36
#define L3_DEVICE_TYPE_MB9AFB4X  37
#define L3_DEVICE_TYPE_MB9BF10X  40
#define L3_DEVICE_TYPE_MB9BF11X  45
#define L3_DEVICE_TYPE_MB9BF21X  55
#define L3_DEVICE_TYPE_MB9BF30X  60
#define L3_DEVICE_TYPE_MB9BF31X  65
#define L3_DEVICE_TYPE_MB9BF40X  70
#define L3_DEVICE_TYPE_MB9BF41X  75
#define L3_DEVICE_TYPE_MB9BF50X  80
#define L3_DEVICE_TYPE_MB9BF51X  85
#define L3_DEVICE_TYPE_MB9BF61X 100
#define L3_DEVICE_TYPE_MB9BFD1X 110

/**
 ******************************************************************************
 ** Global User Package Choice List
 ******************************************************************************/ 
#define L3_DEVICE_PACKAGE_K  10
#define L3_DEVICE_PACKAGE_L  20
#define L3_DEVICE_PACKAGE_M  30
#define L3_DEVICE_PACKAGE_N  40
#define L3_DEVICE_PACKAGE_R  50
#define L3_DEVICE_PACKAGE_S  60
#define L3_DEVICE_PACKAGE_T  70

/******************************************************************************/
/* User Setting Include file                                                  */
/******************************************************************************/
#include "l3_user.h"  // MUST be included here!

/**
 ******************************************************************************
 ** Global device family definition
 ******************************************************************************/
#if (L3_MCU == L3_DEVICE_TYPE_MB9AF10X)
  #define L3_DEVICE_MB9AF10X L3_ON
#else
  #define L3_DEVICE_MB9AF10X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AF11X)
  #define L3_DEVICE_MB9AF11X L3_ON
#else
  #define L3_DEVICE_MB9AF11X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AF13X)
  #define L3_DEVICE_MB9AF13X L3_ON
#else
  #define L3_DEVICE_MB9AF13X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AF14X)
  #define L3_DEVICE_MB9AF14X L3_ON
#else
  #define L3_DEVICE_MB9AF14X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AF31X)
  #define L3_DEVICE_MB9AF31X L3_ON
#else
  #define L3_DEVICE_MB9AF31X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AF34X)
  #define L3_DEVICE_MB9AF34X L3_ON
#else
  #define L3_DEVICE_MB9AF34X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AFA3X)
  #define L3_DEVICE_MB9AFA3X L3_ON
#else
  #define L3_DEVICE_MB9AFA3X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AFA4X)
  #define L3_DEVICE_MB9AFA4X L3_ON
#else
  #define L3_DEVICE_MB9AFA4X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9AFB4X)
  #define L3_DEVICE_MB9AFB4X L3_ON
#else
  #define L3_DEVICE_MB9AFB4X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF10X)
  #define L3_DEVICE_MB9BF10X L3_ON
#else
  #define L3_DEVICE_MB9BF10X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF11X)
  #define L3_DEVICE_MB9BF11X L3_ON
#else
  #define L3_DEVICE_MB9BF11X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF21X)
  #define L3_DEVICE_MB9BF21X L3_ON
#else
  #define L3_DEVICE_MB9BF21X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF30X)
  #define L3_DEVICE_MB9BF30X L3_ON
#else
  #define L3_DEVICE_MB9BF30X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF31X)
  #define L3_DEVICE_MB9BF31X L3_ON
#else
  #define L3_DEVICE_MB9BF31X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF40X)
  #define L3_DEVICE_MB9BF40X L3_ON
#else
  #define L3_DEVICE_MB9BF40X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF41X)
  #define L3_DEVICE_MB9BF41X L3_ON
#else
  #define L3_DEVICE_MB9BF41X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF50X)
  #define L3_DEVICE_MB9BF50X L3_ON
#else
  #define L3_DEVICE_MB9BF50X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF51X)
  #define L3_DEVICE_MB9BF51X L3_ON
#else
  #define L3_DEVICE_MB9BF51X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BF61X)
  #define L3_DEVICE_MB9BF61X L3_ON
#else
  #define L3_DEVICE_MB9BF61X L3_OFF
#endif

#if (L3_MCU == L3_DEVICE_TYPE_MB9BFD1X)
  #define L3_DEVICE_MB9BFD1X L3_ON
#else
  #define L3_DEVICE_MB9BFD1X L3_OFF
#endif

/**
 ******************************************************************************
 ** Global device type definition
 ******************************************************************************/

#if (L3_DEVICE_MB9AF10X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE0
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AF11X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE1
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_K)
    #define L3_DEVICE_TYPE L3_TYPE5
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AF13X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_K) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_L)
    #define L3_DEVICE_TYPE L3_TYPE3
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE7
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AF14X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE6
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AF31X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE1
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_K)
    #define L3_DEVICE_TYPE L3_TYPE5
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AF34X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE6
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AFA3X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE7
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AFA4X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE6
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9AFB4X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_L) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_M) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_N)
    #define L3_DEVICE_TYPE L3_TYPE6
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF10X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE0
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF11X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE4
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF21X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF30X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE0
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF31X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE4
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF40X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE0
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF41X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE4
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF50X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE0
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF51X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_N) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_R)
    #define L3_DEVICE_TYPE L3_TYPE4
  #elif (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BF61X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#elif (L3_DEVICE_MB9BFD1X == L3_ON)
  #if   (L3_PACKAGE == L3_DEVICE_PACKAGE_S) || \
        (L3_PACKAGE == L3_DEVICE_PACKAGE_T)
    #define L3_DEVICE_TYPE L3_TYPE2
  #else
    #error Device Type not defined!
  #endif

#else
  #error Device not found!
#endif
  
/**
 ******************************************************************************
 ** This hook is part of wait loops. By default this hook clears the watchdog.
 ******************************************************************************/
extern void L3_WAIT_LOOP_HOOK(void);

/**
 ******************************************************************************
 ** Defines for L3 interrupt handling
 **
 ** The user of the L3 can adjust here the resources to be actived and the
 ** corresponding interrupt level.
 **
 ** \note 
 **       - If L3_INTERRUPT_ENABLE is set to L3_OFF it is not guaranteed that
 **         any implemented function will work correctly, except of those which
 **         are especially noted as working without interrupts.
 **       - Recently only Type0 devices are supported
 **
 ******************************************************************************/

///< Use L3 interrupt handling. Possible values: L3_ON, L3_OFF. L3_OFF is not implemented yet!
#define L3_INTERRUPT_ENABLE        L3_ON   

///< Default Interrupt Level (lowest priority, used for De-Init functions)
#define L3_DEFAULT_INTERRUPT_LEVEL 0x0F

/**
 ******************************************************************************
 ** L3 resource enable check
 ** 
 ** \note It does not check, if a device has actually any in l3.h enabled
 **       resource and all instances available!
 **
 ******************************************************************************/
// Activate code for dma.c
#if L3_PERIPHERAL_ENABLE_DMA == L3_ON
    #define L3_PERIPHERAL_DMA_ACTIVE
#endif

// Activate code in adc.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_ADC0 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_ADC1 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_ADC2 == L3_ON)
    #define L3_PERIPHERAL_ADC_ACTIVE
#endif

// Activate code in bt.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_BT0 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT1 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT2 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT3 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT4 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT5 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT6 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_BT7 == L3_ON)
    #define L3_PERIPHERAL_BT_ACTIVE
#endif

// Activate code in can.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_CAN0 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_CAN1 == L3_ON)
    #define L3_PERIPHERAL_CAN_ACTIVE
#endif

// Activate code in crc.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_CRC0 == L3_ON)
    #define L3_PERIPHERAL_CRC_ACTIVE
#endif

// Activate code in clk.c if set to L3_ON or WC enabled
#if (L3_PERIPHERAL_ENABLE_CLK == L3_ON)
    #define L3_PERIPHERAL_CLK_ACTIVE
#endif

// Activate code in crtrim.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_CRTRIM == L3_ON)
    #define L3_PERIPHERAL_CRTRIM_ACTIVE
#endif

// Activate code in csv.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_CSV == L3_ON)
    #define L3_PERIPHERAL_CSV_ACTIVE
#endif

// Activate code in dac.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_DAC0 == L3_ON)
    #define L3_PERIPHERAL_DAC_ACTIVE
#endif

// Activate code in dsm.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_DSM == L3_ON)
    #define L3_PERIPHERAL_DSM_ACTIVE
#endif

// Activate code in dt.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_DT0 == L3_ON)
    #define L3_PERIPHERAL_DT_ACTIVE
#endif

// Activate code in exint.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_EXINT0_7  == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_EXINT8_15 == L3_ON)
    #define L3_PERIPHERAL_EXINT_ACTIVE
#endif

// Activate code in extif.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_EXTIF == L3_ON)
    #define L3_PERIPHERAL_EXTIF_ACTIVE
#endif

// Activate code in flash.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_FLASH        == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_CRTRIM_FLASH == L3_ON)
    #define L3_PERIPHERAL_FLASH_ACTIVE
#endif

// Activate code in lvd.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_LVD == L3_ON)
    #define L3_PERIPHERAL_LVD_ACTIVE
#endif

// Activate code in mfs.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)
    #define L3_PERIPHERAL_MFS_ACTIVE
#endif


// Activate code in mft.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    #define L3_PERIPHERAL_MFT_ACTIVE
#endif

// Activate code in qprc.c if set to L3_ON
#if (L3_PERIPHERAL_ENABLE_QPRC0 == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_QPRC1 == L3_ON)
    #define L3_PERIPHERAL_QPRC_ACTIVE
#endif

// Reset Cause
#if (L3_PERIPHERAL_ENABLE_RESET == L3_ON)
    #define L3_PERIPHERAL_RESET_ACTIVE
#endif

// Real Time Clock
#if (L3_PERIPHERAL_ENABLE_RTC0 == L3_ON)
    #define L3_PERIPHERAL_RTC_ACTIVE
#endif

// Activate code in wc.c if one or more are set to L3_ON
#if (L3_PERIPHERAL_ENABLE_WC0 == L3_ON)
    #define L3_PERIPHERAL_WC_ACTIVE
#endif

// Activate code in wdg.c is set to L3_ON
#if (L3_PERIPHERAL_ENABLE_HWWDG == L3_ON) || \
    (L3_PERIPHERAL_ENABLE_SWWDG == L3_ON)
    #define L3_PERIPHERAL_WDG_ACTIVE
#endif

/******************************************************************************/
/* Parameter check of l3_user.h settings                                      */
/******************************************************************************/

// L3_ON / L3_OFF check

#if (L3_PPG_1616_MODE_AVAILABLE != L3_ON) && (L3_PPG_1616_MODE_AVAILABLE != L3_OFF)
  #error L3_PPG_1616_MODE_AVAILABLE in l3_user.h is not defined as either L3_ON or L3_OFF!
#endif

#if (L3_ECC_FLASH != L3_ON) && (L3_ECC_FLASH != L3_OFF)
  #error L3_ECC_FLASH in l3_user.h is not defined as either L3_ON or L3_OFF!
#endif

#if (L3_WORK_FLASH != L3_ON) && (L3_WORK_FLASH != L3_OFF)
  #error L3_WORK_FLASH in l3_user.h is not defined as either L3_ON or L3_OFF!
#endif

#if (L3_NO_FLASH_RAMCODE != L3_ON) && (L3_NO_FLASH_RAMCODE != L3_OFF)
  #error L3_NO_FLASH_RAMCODE in l3_user.h is not defined as either L3_ON or L3_OFF!
#endif

// Value check

#if ((L3_ADC_CHANNELS < 1) || (L3_ADC_CHANNELS > 16))
  #error L3_ADC_CHANNELS in l3_user.h value out of range!
#endif

#if ((L3_ADC_FIFO_DEPTH < 1) || (L3_ADC_FIFO_DEPTH > 16))
  #error L3_ADC_FIFO_DEPTH in l3_user.h value out of range!
#endif

#if ((L3_DMA_CHANNELS < 1) || (L3_DMA_CHANNELS > 8))
  #error L3_DMA_CHANNELS in l3_user.h value out of range!
#endif

#if ((L3_EXINT_CHANNELS < 1) || (L3_EXINT_CHANNELS > 16))
  #error L3_EXINT_CHANNELS in l3_user.h value out of range!
#endif

/******************************************************************************/
/* Feature check                                                              */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief NAND Flash support
 ** 
 ** NAND Flash support check
 **
 ** Result:
 ** L3_NAND_SUPPORT ==  L3_ON  => Device supports NAND Flash
 ** L3_NAND_SUPPORT ==  L3_OFF => Device does not support NAND Flash
 **
 ******************************************************************************/
#if (((L3_DEVICE_TYPE == L3_TYPE0) && (L3_PACKAGE == L3_DEVICE_PACKAGE_N)) || \
      (L3_DEVICE_TYPE == L3_TYPE1) || \
      (L3_DEVICE_TYPE == L3_TYPE3) || \
     ((L3_DEVICE_TYPE == L3_TYPE4) && (L3_PACKAGE == L3_DEVICE_PACKAGE_N)) || \
      (L3_DEVICE_TYPE == L3_TYPE5) || \
      (L3_DEVICE_TYPE == L3_TYPE7))
  #define L3_NAND_SUPPORT L3_OFF
#else
  #define L3_NAND_SUPPORT L3_ON
#endif

/**
 ******************************************************************************
 ** \brief RTC support
 ** 
 ** RTC support check
 **
 ** Result:
 ** L3_RTC_SUPPORT ==  L3_ON  => Device supports RTC
 ** L3_RTC_SUPPORT ==  L3_OFF => Device does not support RTC
 **
 ******************************************************************************/
#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
     (L3_DEVICE_TYPE == L3_TYPE4) || \
     (L3_DEVICE_TYPE == L3_TYPE5) || \
     (L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7))
  #define L3_RTC_SUPPORT L3_ON
#else
  #define L3_RTC_SUPPORT L3_OFF
#endif

/**
 ******************************************************************************
 ** \brief Deep Stadby Mode support
 ** 
 ** Deep Stadby Mode support check
 **
 ** Result:
 ** L3_DSM_SUPPORT ==  L3_ON  => Device supports Deep Standby Mode
 ** L3_DSM_SUPPORT ==  L3_OFF => Device does not support Deep Standby Mode
 **
 ******************************************************************************/
#if ((L3_DEVICE_TYPE == L3_TYPE3) || \
     (L3_DEVICE_TYPE == L3_TYPE5) || \
     (L3_DEVICE_TYPE == L3_TYPE6) || \
     (L3_DEVICE_TYPE == L3_TYPE7))
  #define L3_DSM_SUPPORT L3_ON
#else
  #define L3_DSM_SUPPORT L3_OFF
#endif

/**
 ******************************************************************************
 ** Global macro address defintions
 ******************************************************************************/
#define L3_FLASH_BASE        (0x00000000UL)                // Flash Base
#define L3_WORKFLASH_BASE    (0x200C0000UL)                // Work Flash Base
#define L3_PERIPH_BASE       (0x40000000UL)                // Peripheral Base

#define L3_WORKFLASH_IF_BASE (L3_WORKFLASH_BASE + 0x20000UL) // Work Flash interface registers 
#define L3_FLASH_IF_BASE     (L3_PERIPH_BASE + 0x00000UL)  // Flash interface registers 
#define L3_CRG_BASE          (L3_PERIPH_BASE + 0x10000UL)  // Clock and reset registers 
#define L3_HWWDT_BASE        (L3_PERIPH_BASE + 0x11000UL)  // Hardware watchdog registers 
#define L3_SWWDT_BASE        (L3_PERIPH_BASE + 0x12000UL)  // Software watchdog registers
#define L3_DT0_BASE          (L3_PERIPH_BASE + 0x15000UL)  // Dual timer 1/2 registers 
#define L3_MFT0_BASE         (L3_PERIPH_BASE + 0x20000UL)  // Multifunction Timer unit 0 registers
#define L3_MFT1_BASE         (L3_PERIPH_BASE + 0x21000UL)  // Multifunction Timer unit 1 registers
#define L3_MFT2_BASE         (L3_PERIPH_BASE + 0x22000UL)  // Multifunction Timer unit 2 registers
#define L3_MFT_PPG_BASE      (L3_PERIPH_BASE + 0x24000UL)  // Multifunction Timer PPG registers
#define L3_BT0_BASE          (L3_PERIPH_BASE + 0x25000UL)  // Base Timer 0 registers
#define L3_BT1_BASE          (L3_PERIPH_BASE + 0x25040UL)  // Base Timer 1 registers
#define L3_BT2_BASE          (L3_PERIPH_BASE + 0x25080UL)  // Base Timer 2 registers
#define L3_BT3_BASE          (L3_PERIPH_BASE + 0x250C0UL)  // Base Timer 3 registers
#define L3_BT4_BASE          (L3_PERIPH_BASE + 0x25200UL)  // Base Timer 4 registers
#define L3_BT5_BASE          (L3_PERIPH_BASE + 0x25240UL)  // Base Timer 5 registers
#define L3_BT6_BASE          (L3_PERIPH_BASE + 0x25280UL)  // Base Timer 6 registers
#define L3_BT7_BASE          (L3_PERIPH_BASE + 0x252C0UL)  // Base Timer 7 registers
#define L3_BTIOSEL03_BASE    (L3_PERIPH_BASE + 0x25100UL)  // Base Timer I/O selector channel 0 - channel 3 registers
#define L3_BTIOSEL47_BASE    (L3_PERIPH_BASE + 0x25300UL)  // Base Timer I/O selector channel 4 - channel 7 registers
#define L3_SBSSR_BASE        (L3_PERIPH_BASE + 0x25FFCUL)  // Software based Simulation Startup (Base Timer) register
#define L3_QPRC0_BASE        (L3_PERIPH_BASE + 0x26000UL)  // Quad position and revolution counter channel 0 registers
#define L3_QPRC1_BASE        (L3_PERIPH_BASE + 0x26040UL)  // Quad position and revolution counter channel 1 registers
#define L3_QPRC2_BASE        (L3_PERIPH_BASE + 0x26080UL)  // Quad position and revolution counter channel 2 registers
#define L3_ADC0_BASE         (L3_PERIPH_BASE + 0x27000UL)  // 12-bit ADC unit 0 registers
#define L3_ADC1_BASE         (L3_PERIPH_BASE + 0x27100UL)  // 12-bit ADC unit 1 registers
#define L3_ADC2_BASE         (L3_PERIPH_BASE + 0x27200UL)  // 12-bit ADC unit 2 registers
#define L3_DAC_BASE          (L3_PERIPH_BASE + 0x28000UL)  // 10-bit DAC uint
#define L3_CRTRIM_BASE       (L3_PERIPH_BASE + 0x2E000UL)  // CR trimming registers 
#define L3_EXTI_BASE         (L3_PERIPH_BASE + 0x30000UL)  // External interrupt registers
#define L3_INTREQ_BASE       (L3_PERIPH_BASE + 0x31000UL)  // Interrupt request read registers
#define L3_GPIO_BASE         (L3_PERIPH_BASE + 0x33000UL)  // General purpose I/O registers
#define L3_LVD_BASE          (L3_PERIPH_BASE + 0x35000UL)  // Low voltage detection registers
#define L3_DSM_BASE          (L3_PERIPH_BASE + 0x35100UL)  // Deep Standby Modes registers 
#define L3_USBCLK_BASE       (L3_PERIPH_BASE + 0x36000UL)  // USB clock registers
#define L3_CANPRES_BASE      (L3_PERIPH_BASE + 0x37000UL)  // CAN prescaler register
#define L3_MFS0_BASE         (L3_PERIPH_BASE + 0x38000UL)  // MFS 0 registers
#define L3_MFS1_BASE         (L3_PERIPH_BASE + 0x38100UL)  // MFS 1 registers
#define L3_MFS2_BASE         (L3_PERIPH_BASE + 0x38200UL)  // MFS 2 registers
#define L3_MFS3_BASE         (L3_PERIPH_BASE + 0x38300UL)  // MFS 3 registers
#define L3_MFS4_BASE         (L3_PERIPH_BASE + 0x38400UL)  // MFS 4 registers
#define L3_MFS5_BASE         (L3_PERIPH_BASE + 0x38500UL)  // MFS 5 registers
#define L3_MFS6_BASE         (L3_PERIPH_BASE + 0x38600UL)  // MFS 6 registers
#define L3_MFS7_BASE         (L3_PERIPH_BASE + 0x38700UL)  // MFS 7 registers
#define L3_MFS_NFC_BASE      (L3_PERIPH_BASE + 0x38800UL)  // MFS Noise Filter register
#define L3_CRC_BASE          (L3_PERIPH_BASE + 0x39000UL)  // CRC registers
#define L3_WC_BASE           (L3_PERIPH_BASE + 0x3A000UL)  // Watch counter registers
#define L3_RTC_BASE          (L3_PERIPH_BASE + 0x3B000UL)  // Real time clock registers
#define L3_EXBUS_BASE        (L3_PERIPH_BASE + 0x3F000UL)  // External bus interface registers
#define L3_USB0_BASE         (L3_PERIPH_BASE + 0x42100UL)  // USB channel 0 registers
#define L3_USB1_BASE         (L3_PERIPH_BASE + 0x52100UL)  // USB channel 1 registers
#define L3_DMAC_BASE         (L3_PERIPH_BASE + 0x60000UL)  // DMA controller
#define L3_CAN0_BASE         (L3_PERIPH_BASE + 0x62000UL)  // CAN channel 0 registers
#define L3_CAN1_BASE         (L3_PERIPH_BASE + 0x63000UL)  // CAN channel 1 registers

/******************************************************************************/
/* Global type definitions ('typedef')                                        */
/******************************************************************************/

// Error codes in base_types_l3.h

/**
 ******************************************************************************
 ** \brief Level
 ** 
 ** Specifies levels.
 **
 ******************************************************************************/
typedef enum en_level
{
    L3Low      = 0,  ///< Low level '0'
    L3High     = 1   ///< High level '1'
} en_level_t;

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

/******************************************************************************
 * Global function protoypes
 ******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* __L3_H__ */

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

