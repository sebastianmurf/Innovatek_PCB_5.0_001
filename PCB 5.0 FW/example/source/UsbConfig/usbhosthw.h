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
/** \file UsbHostHW.h
 **
 ** USB hardware specific settings
 **
 ** History:
 **   - 2010-10-14    1.0  MSc  First version  (works with 16FX,FR80)
 **   - 2011-03-30    1.1  MSc  Compatible with FSEU Host drivers 2011-03-30
 *****************************************************************************/

#ifndef __USBHOSTHW_H__
#define __USBHOSTHW_H__

#define HOST0VBUS_DEINIT 
#define HOST0VBUS_INIT FM3_GPIO->DDR1 |= (1 << 3)
#define HOST0VBUS_SET FM3_GPIO->PDOR1 |= (1 << 3)
#define HOST0VBUS_CLEAR FM3_GPIO->PDOR1 &= ~(1 << 3)
#define HOST0VBUS_GET ((FM3_GPIO->PDORC & 0x0400) > 0)
#define HOST0VBUS_ENABLED 1

#define HOST0OTGPULLDOWN_DEINIT 
#define HOST0OTGPULLDOWN_INIT FM3_GPIO->DDR1 |= (1 << 0x02)
#define HOST0OTGPULLDOWN_SET FM3_GPIO->PDOR1 |= (1 << 0x02)
#define HOST0OTGPULLDOWN_CLEAR FM3_GPIO->PDOR1 &= ~(1 << 0x02)
#define HOST0OTGPULLDOWN_GET ((FM3_GPIO->PDORC & 0x0200) > 0)
#define HOST0OTGPULLDOWN_ENABLED 1

#define HOST0OVERCURRENT_DISABLEISR bFM3_EXTI_ENIR_EN2 = 0
#define HOST0OVERCURRENT_ENABLEISR bFM3_EXTI_ENIR_EN2 = 1
#define HOST0OVERCURRENT_CLEARISRFLAG bFM3_EXTI_EICL_ECL2 = 0
#define HOST0OVERCURRENT_ISRISSET bFM3_EXTI_EIRR_ER2 == 1
#define HOST0OVERCURRENT_SETLOWDETECT bFM3_EXTI_ELVR_LA2 = 0
#define HOST0OVERCURRENT_SETHIGHDETECT bFM3_EXTI_ELVR_LA2 = 1
#define HOST0OVERCURRENT_HIGHDETECT (FM3_GPIO->PDIR1 & 0x02) > 0
#define HOST0OVERCURRENT_INIT FM3_GPIO->DDR1 &= ~0x02
#define HOST0OVERCURRENT_DEINIT 
#define HOST0OVERCURRENT_ENABLED 1


#define HOST1VBUS_DEINIT
#define HOST1VBUS_INIT
#define HOST1VBUS_SET
#define HOST1VBUS_CLEAR
#define HOST1VBUS_GET 1
#define HOST1VBUS_ENABLED 1

#define HOST1OTGPULLDOWN_DEINIT 
#define HOST1OTGPULLDOWN_INIT 
#define HOST1OTGPULLDOWN_SET 
#define HOST1OTGPULLDOWN_CLEAR 
#define HOST1OTGPULLDOWN_GET 1
#define HOST1OTGPULLDOWN_ENABLED 1

#define HOST1OVERCURRENT_DISABLEISR 
#define HOST1OVERCURRENT_ENABLEISR 
#define HOST1OVERCURRENT_CLEARISRFLAG 
#define HOST1OVERCURRENT_ISRISSET 0
#define HOST1OVERCURRENT_SETLOWDETECT 
#define HOST1OVERCURRENT_SETHIGHDETECT 
#define HOST1OVERCURRENT_HIGHDETECT 0
#define HOST1OVERCURRENT_INIT 
#define HOST1OVERCURRENT_DEINIT
#define HOST0OVERCURRENT_ENABLED 1


#endif
