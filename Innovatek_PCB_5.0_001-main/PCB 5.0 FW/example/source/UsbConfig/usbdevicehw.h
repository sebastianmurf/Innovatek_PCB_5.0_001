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
/** \file usbdevicehw.h
 **
 ** - See README.TXT for project description
 ** - pre release for a simple universal usb function library
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX,FR80)
 **   - 2011-03-30    1.1  MSc  New HW description style
 *****************************************************************************/

#ifndef __USBDEVICEHW_H__
#define __USBDEVICEHW_H__

#define DEVICE0VBUS_DISABLEISR bFM3_EXTI_ENIR_EN15 = 0
#define DEVICE0VBUS_ENABLEISR bFM3_EXTI_ENIR_EN15 = 1
#define DEVICE0VBUS_ISRISSET bFM3_EXTI_EIRR_ER15 == 1
#define DEVICE0VBUS_CLEARISRFLAG bFM3_EXTI_EICL_ECL15 = 0
#define DEVICE0VBUS_SETLOWDETECT bFM3_EXTI_ELVR_LA15 = 0
#define DEVICE0VBUS_SETHIGHDETECT bFM3_EXTI_ELVR_LA15 = 1
#define DEVICE0VBUS_HIGHDETECT (FM3_GPIO->PDIR6 & 0x01) > 0
#define DEVICE0VBUS_INIT bFM3_GPIO_PFR6_P0  = 1; bFM3_GPIO_DDR6_P0 = 0; bFM3_GPIO_EPFR06_EINT15S1 = 1; //NVIC_ClearPendingIRQ(EXINT8_15_IRQn); NVIC_EnableIRQ(EXINT8_15_IRQn); NVIC_SetPriority(EXINT8_15_IRQn,1)
#define DEVICE0VBUS_DEINIT
#define DEVICE0VBUS_ENABLED 1

#define DEVICE0HCONX_INIT 
#define DEVICE0HCONX_SET 
#define DEVICE0HCONX_CLEAR 
#define DEVICE0HCONX_ENABLED 1

#define DEVICE1VBUS_DISABLEISR 
#define DEVICE1VBUS_ENABLEISR 
#define DEVICE1VBUS_ISRISSET 0
#define DEVICE1VBUS_CLEARISRFLAG 
#define DEVICE1VBUS_SETLOWDETECT 
#define DEVICE1VBUS_SETHIGHDETECT 
#define DEVICE1VBUS_HIGHDETECT 0
#define DEVICE1VBUS_INIT 
#define DEVICE1VBUS_DEINIT
#define DEVICE1VBUS_ENABLED 1

#define DEVICE1HCONX_INIT 
#define DEVICE1HCONX_SET 
#define DEVICE1HCONX_CLEAR 
#define DEVICE1HCONX_ENABLED 1


/* ORIGINAL P13
#define HOSTVBUS_INIT FM3_GPIO->DDR1 |= (1 << 3)
#define HOSTVBUS_SET FM3_GPIO->PDOR1 |= (1 << 3)
#define HOSTVBUS_CLEAR FM3_GPIO->PDOR1 &= ~(1 << 3)
#define HOSTVBUS_ENABLED 1
*/
/* NABEEN P62*/
#define HOSTVBUS_INIT FM3_GPIO->DDR6 |= (1 << 2)
#define HOSTVBUS_SET FM3_GPIO->PDOR6 |= (1 << 2)
#define HOSTVBUS_CLEAR FM3_GPIO->PDOR6 &= ~(1 << 2)
#define HOSTVBUS_ENABLED 1

/* ORIGINAL P12
#define HOSTOTGPULLDOWN_INIT FM3_GPIO->DDR1 |= (1 << 0x02)
#define HOSTOTGPULLDOWN_SET FM3_GPIO->PDOR1 |= (1 << 0x02)
#define HOSTOTGPULLDOWN_CLEAR FM3_GPIO->PDOR1 &= (1 << 0x02)
#define HOSTOTGPULLDOWN_ENABLED 1
*/
/* NABEEN P0F*/
#define HOSTOTGPULLDOWN_INIT FM3_GPIO->DDR0 |= (1 << 0x0F)
#define HOSTOTGPULLDOWN_SET FM3_GPIO->PDOR0 |= (1 << 0x0F)
#define HOSTOTGPULLDOWN_CLEAR FM3_GPIO->PDOR0 &= (1 << 0x0F)
#define HOSTOTGPULLDOWN_ENABLED 1

/* ORIGINAL INT02_1 P11
#define HOSTOVERCURRENT_DISABLEISR bFM3_EXTI_ENIR_EN2 = 0
#define HOSTOVERCURRENT_ENABLEISR bFM3_EXTI_ENIR_EN2 = 1
#define HOSTOVERCURRENT_CLEARISRFLAG bFM3_EXTI_EICL_ECL2 = 0
#define HOSTOVERCURRENT_ISRISSET bFM3_EXTI_EIRR_ER2 == 1
#define HOSTOVERCURRENT_SETLOWDETECT bFM3_EXTI_ELVR_LA2 = 0
#define HOSTOVERCURRENT_SETHIGHDETECT bFM3_EXTI_ELVR_LA2 = 1
#define HOSTOVERCURRENT_HIGHDETECT (FM3_GPIO->PDIR1 & 0x02) > 0
#define HOSTOVERCURRENT_INIT FM3_GPIO->DDR1 &= ~0x02
#define HOSTOVERCURRENT_ENABLED 1
*/
/* NABEEN INT04_0 P33*/
#define HOSTOVERCURRENT_DISABLEISR bFM3_EXTI_ENIR_EN4 = 0
#define HOSTOVERCURRENT_ENABLEISR bFM3_EXTI_ENIR_EN4 = 1
#define HOSTOVERCURRENT_CLEARISRFLAG bFM3_EXTI_EICL_ECL4 = 0
#define HOSTOVERCURRENT_ISRISSET bFM3_EXTI_EIRR_ER4 == 1
#define HOSTOVERCURRENT_SETLOWDETECT bFM3_EXTI_ELVR_LA4 = 0
#define HOSTOVERCURRENT_SETHIGHDETECT bFM3_EXTI_ELVR_LA4 = 1
#define HOSTOVERCURRENT_HIGHDETECT (FM3_GPIO->PDIR3 & 0x08) > 0
#define HOSTOVERCURRENT_INIT FM3_GPIO->DDR3 &= ~0x08
#define HOSTOVERCURRENT_ENABLED 1



#endif
