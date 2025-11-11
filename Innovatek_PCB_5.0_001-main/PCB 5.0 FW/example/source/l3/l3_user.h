/******************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)                */
/*                                                                           */
/* The following software deliverable is intended for and must only be       */
/* used for reference and in an evaluation laboratory environment.           */
/* It is provided on an as-is basis without charge and is subject to         */
/* alterations.                                                              */
/* It is the user�s obligation to fully test the software in its             */
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
/* To the maximum extent permitted by applicable law, FSEU�s liability       */
/* is restricted to intentional misconduct and gross negligence.             */
/* FSEU is not liable for consequential damages.                             */
/*                                                                           */
/* (V1.5)                                                                    */
/******************************************************************************/
/** \file l3_user.h
 **
 ** User settings headerfile for FM3 Low Level Library.
 **
 ** \note This file MUST only be included in l3.h!
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-05-10  1.1  MWi  Parameter check for preprocessor switches added,
 **                           Package definititon added, Reset defintion added
 **   - 2012-05-16  1.2  MWi  RTC activator added; DSM activator added
 **   - 2012-07-09  1.3  MWi  L3_DMA_HALT_RAMCODE,
 **                           L3_NMI_RAM_VECTOR_HANDLE,
 **                           L3_NMI_RAM_VECTOR_ADDRESS added
 **
 ******************************************************************************/

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/

/**
 ******************************************************************************
 ** Global device definition
 **
 ** See l3.h line 91 for choice list.
 **
 ** \note This definition is used for GPIO settings, interrupt vector,
 **       enumeration handling, and Type(0, 1, 3, ...) Device handling.
 ******************************************************************************/
#define L3_MCU L3_DEVICE_TYPE_MB9AF31X

/**
 ******************************************************************************
 ** Global package definition
 **
 ** See l3.h line 119 for choice list.
 **
 ** \note This definition is used for device type settings
 ******************************************************************************/
#define L3_PACKAGE L3_DEVICE_PACKAGE_K
  
/**
 ******************************************************************************
 ** User Defines for L3 resource enable
 **
 ** Possible definitions are L3_ON and L3_OFF.
 **
 ** \note Only active resources and instances, which are actually available
 **       on the used MCU! Refer to the MCU's documentation (e.g. data sheet).
 ******************************************************************************/
// DMA
#define L3_PERIPHERAL_ENABLE_DMA L3_OFF

// ADCs
#define L3_PERIPHERAL_ENABLE_ADC0 L3_ON
#define L3_PERIPHERAL_ENABLE_ADC1 L3_OFF
#define L3_PERIPHERAL_ENABLE_ADC2 L3_OFF

// Base Timers
#define L3_PERIPHERAL_ENABLE_BT0 L3_ON
#define L3_PERIPHERAL_ENABLE_BT1 L3_OFF
#define L3_PERIPHERAL_ENABLE_BT2 L3_ON
#define L3_PERIPHERAL_ENABLE_BT3 L3_OFF
#define L3_PERIPHERAL_ENABLE_BT4 L3_ON
#define L3_PERIPHERAL_ENABLE_BT5 L3_OFF
#define L3_PERIPHERAL_ENABLE_BT6 L3_OFF
#define L3_PERIPHERAL_ENABLE_BT7 L3_OFF

// CAN
#define L3_PERIPHERAL_ENABLE_CAN0 L3_OFF
#define L3_PERIPHERAL_ENABLE_CAN1 L3_OFF

// CRC
#define L3_PERIPHERAL_ENABLE_CRC0 L3_OFF

// Clock
#define L3_PERIPHERAL_ENABLE_CLK L3_OFF

// DAC
#define L3_PERIPHERAL_ENABLE_DAC0 L3_OFF

// DSM
#define L3_PERIPHERAL_ENABLE_DSM L3_OFF

// HS-CR Trimming
#define L3_PERIPHERAL_ENABLE_CRTRIM L3_OFF

// HS-CR Trimming with Flash update
//   Note: Does not work w/o activation of L3_PERIPHERAL_ENABLE_CRTRIM!
//         See crtrim.h for details.
#define L3_PERIPHERAL_ENABLE_CRTRIM_FLASH L3_OFF

// Clock Supervisor
#define L3_PERIPHERAL_ENABLE_CSV L3_OFF

// External Interrupts
#define L3_PERIPHERAL_ENABLE_EXINT0_7  L3_OFF
#define L3_PERIPHERAL_ENABLE_EXINT5_15 L3_OFF

// External Bus Interface
#define L3_PERIPHERAL_ENABLE_EXTIF  L3_OFF

// Flash routines
#define L3_PERIPHERAL_ENABLE_FLASH  L3_OFF

// Dual Timer(s)
#define L3_PERIPHERAL_ENABLE_DT0 L3_OFF

// Low Voltage Detection
#define L3_PERIPHERAL_ENABLE_LVD L3_OFF

// Multi Function Serial Interfaces
#define L3_PERIPHERAL_ENABLE_MFS0 L3_ON
#define L3_PERIPHERAL_ENABLE_MFS1 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFS2 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFS3 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFS4 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFS5 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFS6 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFS7 L3_OFF

// Multi Function Timer Interfaces
#define L3_PERIPHERAL_ENABLE_MFT0 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFT1 L3_OFF
#define L3_PERIPHERAL_ENABLE_MFT2 L3_OFF

// Quad Decoder
#define L3_PERIPHERAL_ENABLE_QPRC0 L3_OFF
#define L3_PERIPHERAL_ENABLE_QPRC1 L3_OFF

// Reset Cause
#define L3_PERIPHERAL_ENABLE_RESET L3_OFF

// Real Time Clock
#define L3_PERIPHERAL_ENABLE_RTC0 L3_OFF

// USB
#define L3_PERIPHERAL_ENABLE_USB0_DEVICE L3_ON
#define L3_PERIPHERAL_ENABLE_USB0_HOST L3_ON
#define L3_PERIPHERAL_ENABLE_USB1_DEVICE L3_OFF
#define L3_PERIPHERAL_ENABLE_USB1_HOST L3_OFF

// Watch Counter
#define L3_PERIPHERAL_ENABLE_WC0 L3_OFF

// Watchdog Timers
#define L3_PERIPHERAL_ENABLE_HWWDG L3_OFF
#define L3_PERIPHERAL_ENABLE_SWWDG L3_OFF

/**
 ******************************************************************************
 ** User Interrupt level settings
 **
 ** Possible values are 0 (high priority) to 15 (low priority)
 ******************************************************************************/
// DMA end transfer interrupts
#define L3_DMA0_LEVEL             15
#define L3_DMA1_LEVEL             15
#define L3_DMA2_LEVEL             15
#define L3_DMA3_LEVEL             15
#define L3_DMA4_LEVEL             15
#define L3_DMA5_LEVEL             15
#define L3_DMA6_LEVEL             15
#define L3_DMA7_LEVEL             15

// CAN
#define L3_IRQ_LEVEL_CAN0         15
#define L3_IRQ_LEVEL_CAN1         15

// Analog Digital Converters
#define L3_IRQ_LEVEL_ADC0         15
#define L3_IRQ_LEVEL_ADC1         15
#define L3_IRQ_LEVEL_ADC2         15

// Base Timers
#define L3_IRQ_LEVEL_BT           15

// Clock Supervisor
#define L3_IRQ_LEVEL_CSV          15

// Dual Timer(s)
#define L3_IRQ_LEVEL_DT0          15

// External Interrupts + NMI
#define L3_IRQ_LEVEL_EXINT0_7     15
#define L3_IRQ_LEVEL_EXINT8_15    15
#define L3_IRQ_LEVEL_NMI          15

// Low Voltage Detection Interrupt
#define L3_IRQ_LEVEL_LVD          15

// Multi Function Serial Interfaces
#define L3_IRQ_LEVEL_MFS0_TX      3
#define L3_IRQ_LEVEL_MFS0_RX      3
#define L3_IRQ_LEVEL_MFS1_TX      15
#define L3_IRQ_LEVEL_MFS1_RX      15
#define L3_IRQ_LEVEL_MFS2_TX      15
#define L3_IRQ_LEVEL_MFS2_RX      15
#define L3_IRQ_LEVEL_MFS3_TX      15
#define L3_IRQ_LEVEL_MFS3_RX      15
#define L3_IRQ_LEVEL_MFS4_TX      15
#define L3_IRQ_LEVEL_MFS4_RX      15
#define L3_IRQ_LEVEL_MFS5_TX      15
#define L3_IRQ_LEVEL_MFS5_RX      15
#define L3_IRQ_LEVEL_MFS6_TX      15
#define L3_IRQ_LEVEL_MFS6_RX      15
#define L3_IRQ_LEVEL_MFS7_TX      15
#define L3_IRQ_LEVEL_MFS7_RX      15

// Multi Function Timer Interrupts
#define L3_IRQ_LEVEL_MFT_WFG_DTIF 15
#define L3_IRQ_LEVEL_MFT_FRT      15
#define L3_IRQ_LEVEL_MFT_ICU      15
#define L3_IRQ_LEVEL_MFT_OCU      15
#define L3_IRQ_LEVEL_MFT_PPG      15

// Quad Decoder and Revolution Counter
#define L3_IRQ_LEVEL_QPRC0        15

// Real Time Clock
#define L3_IRQ_LEVEL_RTC0         15

// Software Watchdog
#define L3_IRQ_LEVEL_SWWDG        15

// USB
#define L3_IRQ_LEVEL_USB0 3
#define L3_IRQ_LEVEL_USB1 15

// Watch Counter / Clock Stabilization Irq
#define L3_IRQ_LEVEL_CLK_WC0      15

// Watchdog Timers
#define L3_IRQ_LEVEL_HWWDG        15
#define L3_IRQ_LEVEL_SWWDG        15

/******************************************************************************
 ** User hardware setting
 **
 ** Not all settings can be done via device definition. This section allows the
 ** user to adjust the library for individual device settings, such e.g. number
 ** of available ADC channels, which depend on the package size, etc.
 **
 ** The L3_ON/L3_OFF parameters are checked in l3.h!
 ** 
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief User 16+16 Bit activation for PPG configuration
 **
 ** Possible values are:
 ** - L3_ON   PPG driver supports 16+16 bit mode
 ** - L3_OFF  PPG driver does not support 16+16 bit mode
 ******************************************************************************/
#define L3_PPG_1616_MODE_AVAILABLE L3_ON

/**
 ******************************************************************************
 ** \brief User ADC input configuration
 **
 ** Number of analog input pins (1 to 16)
 ******************************************************************************/
#define L3_ADC_CHANNELS          16 

/**
 ******************************************************************************
 ** \brief User ADC FIFO depth configuration
 **
 ** ADC Scan conversion FIFO depth (1 to 16)
 ******************************************************************************/
#define L3_ADC_FIFO_DEPTH        16 

/**
 ******************************************************************************
 ** \brief User Number of DMA channels. If a device does not support DMA it
 ** is not needed to set this value to 0.
 **
 ** Number of available DMA channels (1 to 8)
 ******************************************************************************/
#define L3_DMA_CHANNELS          8 

/**
 ******************************************************************************
 ** \brief User Number of external interrupt channels
 **
 ** Number of available EXINT channels (1 to 16)
 ******************************************************************************/
#define L3_EXINT_CHANNELS        16

/**
 ******************************************************************************
 ** \brief User ECC Flash availability
 **
 ** Possible values are:
 ** - L3_ON   Device supports ECC Flash
 ** - L3_OFF  Device does not support ECC Flash
 ******************************************************************************/
#define L3_ECC_FLASH             L3_OFF

/**
 ******************************************************************************
 ** \brief User Work Flash availability
 **
 ** Possible values are:
 ** - L3_ON   Device supports Work Flash
 ** - L3_OFF  Device does not support Work Flash
 ******************************************************************************/
#define L3_WORK_FLASH            L3_ON

/**
 ******************************************************************************
 ** \brief User does not need Flash routines in RAMCODE
 **
 ** Possible values are:
 ** - L3_ON   Flash routines in RAMCODE are not compiled
 ** - L3_OFF  Flash routines in RAMCODE active
 ******************************************************************************/
#define L3_NO_FLASH_RAMCODE      L3_OFF

/**
 ******************************************************************************
 ** \brief Global DMA halt during Flash routines
 **
 ** Possible values are:
 ** - L3_ON   DMA is globally halted during Flash routine executions
 ** - L3_OFF  DMA is not touched in Flash routines
 ******************************************************************************/
#define L3_FLASH_DMA_HALT        L3_ON

/**
 ******************************************************************************
 ** \brief Global NMI service during Flash routines. Only used, if RAMCODE is
 **        used and L3_FLASH_DMA_HALT == L3_ON
 **
 ** Possible values are:
 ** - L3_ON   NMI is pre-serviced in RAM vector code
 ** - L3_OFF  NMI is not handled in RAM routines
 ******************************************************************************/
#define L3_NMI_RAM_VECTOR_HANDLE  L3_ON

/**
 ******************************************************************************
 ** \brief NMI RAM vector address. Only used, if L3_NMI_RAM_VECTOR_HANDLE ==
 **        L3_ON, L3_FLASH_DMA_HALT == L3_ON, and RAMCODE used.
 **
 ** Used for remapping VTOR to RAM area. Must be defined by user. Note that the
 ** last 6 bits are always treated as '0'. A global varibale
 ** #u32L3NmiVectorAddress for the RAM NMI vector is placed at this address + 8.
 **
 ** \attention Be careful by choosing the address! It must not interfere with
 **            the RAM vector table, if 'Debug' build is used. The address must
 **            be less than 0x20000000 and bigger than the bottom of the RAM.
 **            There is no automatic plausibility check!
 ******************************************************************************/
#define L3_RAM_VECTOR_TABLE_ADDRESS  0x1FFFFF00

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
