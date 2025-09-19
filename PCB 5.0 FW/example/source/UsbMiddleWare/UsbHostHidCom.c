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
/** \file UsbHostHidCom.c
 **
 ** Part of FSEU USB Host Driver Module for use with Fujitsu HID Com Device
 **
 ** History:
 **   - 2012-07-18    1.0  MSc  First version 
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "usbhosthidcom.h"

#ifndef USB_DISBALE_HOST_FUNCTIONALITY

static stc_host_endpoint_t *EndpointOUT;
static stc_host_endpoint_t *EndpointIN;
static uint8_t        u8EndpointIN, u8EndpointOUT;
static uint16_t       u16MaxPackageSizeIN, u16MaxPackageSizeOUT;
static volatile boolean_t bDataSent = FALSE;
static volatile boolean_t bDataSending = FALSE;
static volatile boolean_t bDataReceived = FALSE;
static stc_usbn_t* pstcUsbHandle = NULL;
volatile static uint8_t        u8Status = 0;
void (*UsbHostHidComDataSentHandler)(void) = 0;
void (*UsbHostHidComDataReceivedHandler)(uint8_t* pu8Buffer) = 0;

static uint8_t pu8DataReceived[64];

void UsbHostHidCom_DataReceivedHandler(stc_usbn_t* pstcUsb)
{
    bDataReceived = TRUE;
    if (UsbHostHidComDataReceivedHandler != 0)
    {
        UsbHostHidComDataReceivedHandler(pu8DataReceived);
        bDataReceived = FALSE;
    }
}

void UsbHostHidCom_DataSentHandler(stc_usbn_t* pstcUsb)
{
    bDataSent = TRUE;
    bDataSending = FALSE;
    if (UsbHostHidComDataSentHandler != 0)
    {
        UsbHostHidComDataSentHandler();
        bDataSent = FALSE;
    }
}

void UsbHostHidCom_SetDataSentHandler(void (*_UsbHostHidComDataSentHandler)(void))
{
    UsbHostHidComDataSentHandler = _UsbHostHidComDataSentHandler;
}

void UsbHostHidCom_SetDataReceivedHandler(void (*_UsbHostHidComDataReceivedHandler)(uint8_t* pu8Buffer))
{
    UsbHostHidComDataReceivedHandler = _UsbHostHidComDataReceivedHandler;
}

void UsbHostHidCom_InitiateSending(uint8_t* pu8Buffer)
{
    bDataSent = FALSE;
    bDataSending = TRUE;
    UsbHost_TransferData(pstcUsbHandle,EndpointOUT,pu8Buffer, 64, UsbHostHidCom_DataSentHandler);
}

void UsbHostHidCom_SendPolled(uint8_t* pu8Buffer)
{
    bDataSent = FALSE;
    bDataSending = TRUE;
    UsbHost_TransferData(pstcUsbHandle,EndpointOUT,pu8Buffer, 64, UsbHostHidCom_DataSentHandler);
    while(bDataSent == FALSE);
}

uint8_t* UsbHostHidCom_ReceivePolled(void)
{
    while(bDataReceived == FALSE);
    bDataReceived = FALSE;
    return pu8DataReceived;
}

boolean_t UsbHostHidCom_IsSent(void)
{
    return bDataSent;
}

boolean_t UsbHostHidCom_IsSending(void)
{
    return bDataSending;
}

boolean_t UsbHostHidCom_IsReceived(void)
{
    return bDataReceived;
}



boolean_t UsbHostHidCom_InitHandler(stc_usbn_t* pstcUsb, uint8_t* pu8Configuration, uint32_t u32Length)
{
    uint8_t* pu8Buffer;
    uint8_t u8NumberOfInterfaces;
    uint8_t u8NumberOfEndpoints;
    uint8_t u8EndpointNumber;
    uint8_t u8InterfaceNumber;
    
    if (pstcUsbHandle != NULL)
    {
        return FALSE;
    }
    pstcUsbHandle = pstcUsb;
    bDataSent = FALSE;
    bDataSending = FALSE;
    bDataReceived = FALSE;
    u8NumberOfInterfaces=pu8Configuration[4];
    for(u8InterfaceNumber=0;u8InterfaceNumber < u8NumberOfInterfaces;u8InterfaceNumber++)
    {
        pu8Buffer = UsbHost_GetUsbInterfaceDescriptor(pu8Configuration,u32Length,u8InterfaceNumber);
        if (pu8Buffer == 0)
        {
            return FALSE;
        }
        u8NumberOfEndpoints = pu8Buffer[4];
        
        if ((pu8Buffer[5] == 0x03) && (pu8Buffer[6] == 0x00) && (pu8Buffer[7] == 0x0)) //Class & SubClass & Protocol
        {
            for(u8EndpointNumber=1;u8EndpointNumber<=u8NumberOfEndpoints;u8EndpointNumber++)
            {
                pu8Buffer = UsbHost_GetUsbEndpointDescriptor(pu8Configuration,u32Length,u8InterfaceNumber,u8EndpointNumber);
                if (pu8Buffer[2] & USB_IN_DIRECTION)
                {
                    u8EndpointIN = pu8Buffer[2];
                    u16MaxPackageSizeIN = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                }
                else
                {
                    u8EndpointOUT = pu8Buffer[2];
                    u16MaxPackageSizeOUT = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                }
                if ((u8EndpointIN != 0) && (u8EndpointOUT != 0))
                {
                    break;
                }
            }
            if ((u8EndpointIN == 0) && (u8EndpointOUT == 0))
            {

                return FALSE;
            }

            
            UsbHost_AddHostEndpoint(pstcUsbHandle,&EndpointOUT,u8EndpointOUT,u16MaxPackageSizeOUT,10,UsbHostHidCom_DataSentHandler);
            UsbHost_AddHostEndpoint(pstcUsbHandle,&EndpointIN,u8EndpointIN,u16MaxPackageSizeIN,10,UsbHostHidCom_DataReceivedHandler);
            EndpointIN->pu8Buffer = pu8DataReceived;
            EndpointIN->u32DataSize = 64; 
            break;
        }
    }
    if ((u8EndpointIN == 0) || (u8EndpointOUT == 0))
    {
        return FALSE;
    }       
    return TRUE;               
}

boolean_t UsbHostHidCom_DeinitHandler(stc_usbn_t* pstcUsb)
{
    pstcUsbHandle = NULL;
    bDataSent = FALSE;
    bDataSending = FALSE;
    bDataReceived = FALSE;
    u8Status = 0;
    return TRUE;
}

boolean_t UsbHostHidCom_DriverActive(void)
{
    return (pstcUsbHandle != NULL);
}

#endif
