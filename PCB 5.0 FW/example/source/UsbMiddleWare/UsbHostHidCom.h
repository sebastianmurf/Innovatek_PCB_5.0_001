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
/** \file UsbHostHidCom.h
 **
 ** Part of FSEU USB Host Driver Module for use with Fujitsu HID Com Device
 **
 ** History:
 **   - 2012-07-18    1.0  MSc  First version 
 *****************************************************************************/

#ifndef __USBHOSTHIDCOM_H__
#define __USBHOSTHIDCOM_H__

#include "mcu.h"
#include "base_types_l3.h"
#include "UsbHost.h"
#ifndef USB_DISBALE_HOST_FUNCTIONALITY

void UsbHostHidCom_DataReceivedHandler(stc_usbn_t* pstcUsb);
void UsbHostHidCom_DataSentHandler(stc_usbn_t* pstcUsb);
void UsbHostHidCom_SetDataSentHandler(void (*_UsbHostHidComDataSentHandler)(void));
void UsbHostHidCom_SetDataReceivedHandler(void (*_UsbHostHidComDataReceivedHandler)(uint8_t* pu8Buffer));
void UsbHostHidCom_InitiateSending(uint8_t* pu8Buffer);
void UsbHostHidCom_SendPolled(uint8_t* pu8Buffer);
uint8_t* UsbHostHidCom_ReceivePolled(void);
boolean_t UsbHostHidCom_IsSent(void);
boolean_t UsbHostHidCom_IsSending(void);
boolean_t UsbHostHidCom_IsReceived(void);
boolean_t UsbHostHidCom_DriverActive(void);

/* USED FOR USBCLASSSUPERVISOR */
boolean_t UsbHostHidCom_InitHandler(stc_usbn_t* pstcUsb, uint8_t* pu8Configuration, uint32_t u32Length);
boolean_t UsbHostHidCom_DeinitHandler(stc_usbn_t* pstcUsb);

#endif
#endif
