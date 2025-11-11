/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)           */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user's obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FSEU does not warrant that the deliverables do not infringe any      */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FSEU disclaims all */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FSEU's liability  */
/* is restricted to intentional misconduct and gross negligence.        */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.5)                                                               */
/************************************************************************/
/** \file usbconfig.h
 **
 ** Part of FSEU USB Driver Module
 **
 ** History:
 **   - 2012-07-17    1.0  MSc  New Version for use with M3 L3 USB driver
 *****************************************************************************/

#ifndef __USBCONFIG_H__
#define __USBCONFIG_H__

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USEUSBDBG            0
#define USBWARNLEVEL         0  //0: no warnings, 1: hard warnings, 2: all warnings
#define USE_USBDESCRIPTORS_H 1  //<- 1 use usbdescriptors.h file for USB descriptors settings, 0 for disabling
#define USE_USBDEVICEHW_H    1  //<- 1 use usbdevicehw.h file for gpio settings, 0 for disabling
#define USE_USBHOSTHW_H      0  //<- 1 use usbhosthw.h file for gpio settings, 0 for disabling
#define USB_USE_L3 1  //<- 1 use as part of L3 library, 0 for using without L3 library

/* only used if USB is used without L3 */
#define USB0_HOST_ENABLED 1
#define USB0_DEVICE_ENABLED 1
#define USB1_HOST_ENABLED 0
#define USB1_DEVICE_ENABLED 0
#define USE_EXT_INT          1
#define IRQ_LEVEL_USB0       3  
#define IRQ_LEVEL_USB1       3  


#define USB0DEVICE_CLASS_INIT UsbDeviceCdcCom_Init(pstcUSB);

#define USB1DEVICE_CLASS_INIT UsbDeviceCdcCom_Init(pstcUSB);


void UsbConfig_UsbInit(void);
void UsbConfig_SwitchMode(void);

#endif
