/************************************************************************
 * $Id$ / $Rev$ / 2012/9/3
 * http://emea.fujitsu.com
 ************************************************************************/
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
/** \file UsbDeviceCdcCom.c
 **
 ** Add description here...
 **
 ** History:
 **   - 2012-9-3  x.yy  MSc  Automatically Created by FME USB Descriptors Manager 
 *****************************************************************************/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "base_types_l3.h"
#include "UsbDeviceCdcCom.h"
#include "usbdevice.h"
#include "usb.h"


/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/



/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/



/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/

uint8_t DataReceivedFlags = 0; //Status for received data
uint8_t DataSentFlags = 0; //Status for sent data


static stc_usbn_endpoint_data_t* pstcEpIN2;
stc_usbn_endpoint_buffer_t stcEpBuffer2; //struct for custom buffer EP2v

static uint32_t u32LastReceivedSize3; // last received size EP3
static stc_usbn_endpoint_buffer_t stcEpBuffer3; //struct for custom buffer EP3
static uint8_t pu8BufferEp3[BUFFER_SIZE_ENDPOINT3]; // custom buffer EP3
static stc_usbn_endpoint_data_t* pstcEpOUT3;

static stc_usbn_endpoint_data_t* pstcEpIN1;
stc_usbn_endpoint_buffer_t stcEpBuffer1; //struct for custom buffer EP1v


/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/



static void UsbDeviceCdcCom_TxCallbackEp2(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint);


static void UsbDeviceCdcCom_RxCallbackEp3(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint);

static void UsbDeviceCdcCom_TxCallbackEp1(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint);


/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

void UsbDeviceCdcCom_Init(stc_usbn_t* pstcUsb)
{
    stc_usbdevice_class_config_t stcUsbClassConfig;
    stc_usbdevice_endpoint_config_t stcEndpointConfig;
    uint8_t* pu8Interface = NULL;
    uint8_t u8InterfaceLength = 0;

    stcUsbClassConfig.u8InterfaceNumber          = 0;       
    stcUsbClassConfig.u8InterfaceClass           = 0x02;        
    stcUsbClassConfig.u8InterfaceSubClass        = 0x02;                    
    stcUsbClassConfig.u8InterfaceProtocoll       = 0x01;                    
    stcUsbClassConfig.pfnSetupRequestCallback    = ClassSetupRequest0;                    
    stcUsbClassConfig.pfnConnectionCallback      = NULL;                    
    stcUsbClassConfig.pfnDisconnectionCallback   = NULL;                    
    stcUsbClassConfig.pfnConfCallback            = NULL;  
    UsbDevice_RegisterVendorClass(pstcUsb,&stcUsbClassConfig,&pu8Interface,&u8InterfaceLength);

    stcEndpointConfig.u8EndpointAddress = 0x80 | 2;
    stcEndpointConfig.pfnRxTxCallback = UsbDeviceCdcCom_TxCallbackEp2;
    stcEndpointConfig.pstcEndpointBuffer = &stcEpBuffer2;
    pstcEpIN2 = UsbDevice_SetupEndpoint(pstcUsb, &stcEndpointConfig);

    stcUsbClassConfig.u8InterfaceNumber          = 1;       
    stcUsbClassConfig.u8InterfaceClass           = 0x0A;        
    stcUsbClassConfig.u8InterfaceSubClass        = 0x00;                    
    stcUsbClassConfig.u8InterfaceProtocoll       = 0x00;                    
    stcUsbClassConfig.pfnSetupRequestCallback    = ClassSetupRequest1;                    
    stcUsbClassConfig.pfnConnectionCallback      = NULL;                    
    stcUsbClassConfig.pfnDisconnectionCallback   = NULL;                    
    stcUsbClassConfig.pfnConfCallback            = NULL;  
    UsbDevice_RegisterVendorClass(pstcUsb,&stcUsbClassConfig,&pu8Interface,&u8InterfaceLength);

    stcEndpointConfig.u8EndpointAddress = 3;
    stcEndpointConfig.pfnRxTxCallback = UsbDeviceCdcCom_RxCallbackEp3;
    stcEndpointConfig.pstcEndpointBuffer = &stcEpBuffer3;
    pstcEpOUT3 = UsbDevice_SetupEndpoint(pstcUsb, &stcEndpointConfig);
    UsbDevice_ReceiveData(pstcUsb, pstcEpOUT3, pu8BufferEp3, BUFFER_SIZE_ENDPOINT3, UsbIRQ);

    stcEndpointConfig.u8EndpointAddress = 0x80 | 1;
    stcEndpointConfig.pfnRxTxCallback = UsbDeviceCdcCom_TxCallbackEp1;
    stcEndpointConfig.pstcEndpointBuffer = &stcEpBuffer1;
    pstcEpIN1 = UsbDevice_SetupEndpoint(pstcUsb, &stcEndpointConfig);

}

/**
 ******************************************************************************
 ** Class Setup Request handling
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcSetup Received Setup
 **
 *****************************************************************************/
static void ClassSetupRequest0(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup)
{
    
}

/**
 ******************************************************************************
 ** Is called from main application to send data via endpoint 2
 **
 ** \param pu8Buffer Buffer to send
 ** \param u32DataSize Buffersize
 ** \param u8PollinMode 1: polled sending; 0: interrupt sending;
 **
 ** \return 1: if succesful, 0: if usb was not ready
 *****************************************************************************/
boolean_t UsbDeviceCdcCom_SendDataVia2(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode)
{
    if (UsbDevice_GetStatus(pstcUsb) == UsbConfigured)
    {
        DataSentFlags &= ~(1<<2);
        
        UsbDevice_SendData(pstcUsb, pstcEpIN2, pu8Buffer, u32DataSize, enMode);
        if (enMode == UsbPOLL)
        {
            DataSentFlags |= (1<<2);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 ******************************************************************************
 ** Is called from UsbFunctions.c when a endpoint buffer 2 was sent
 **
 ** \param stcEvent Event containing all neccesary information
 *****************************************************************************/
static void UsbDeviceCdcCom_TxCallbackEp2(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint)
{
    DataSentFlags |= (1<<2);
}
/**
 ******************************************************************************
 ** Is used to use get the sent status of endpoint 2
 **
 *****************************************************************************/
uint8_t UsbDeviceCdcCom_DataSent2(void)
{
    if ((DataSentFlags & (1<<2)) > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 ******************************************************************************
 ** Class Setup Request handling
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcSetup Received Setup
 **
 *****************************************************************************/
static void ClassSetupRequest1(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup)
{
    
}

/**
 ******************************************************************************
 ** Is called from UsbFunctions.c when a endpoint buffer 3 was received
 **
 ** \param stcEvent Event containing all neccesary information for receiving data
 *****************************************************************************/
static void UsbDeviceCdcCom_RxCallbackEp3(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint)
{
    uint8_t* pu8Buffer = pstcEndpoint->pstcEndpointBuffer->pu8Buffer;
    uint32_t u32DataSize = pstcEndpoint->pstcEndpointBuffer->u32DataSize;
    u32LastReceivedSize3 = u32DataSize;
    DataReceivedFlags |= (1<<3); // setting data received flag

    //Add your code here to process the received buffer
    UsbDevice_ReceiveData(pstcUsb, pstcEpOUT3, pu8BufferEp3, BUFFER_SIZE_ENDPOINT3, UsbIRQ);
}


/**
 ******************************************************************************
 ** Is used to use received data from endpoint 3
 **
 ** \param pu8Buffer pointer to buffer
 *****************************************************************************/
uint32_t UsbDeviceCdcCom_GetReceivedDataEndpoint3(uint8_t** ppu8Buffer)
{
    if ((DataReceivedFlags & (1<<3)) == 0)
    {
        return 0; // nothing to receive
    }
    DataReceivedFlags -= (1<<3);
    *ppu8Buffer = pstcEpOUT3->pstcEndpointBuffer->pu8Buffer;
    return u32LastReceivedSize3;
}

/**
 ******************************************************************************
 ** Is called from main application to send data via endpoint 1
 **
 ** \param pu8Buffer Buffer to send
 ** \param u32DataSize Buffersize
 ** \param u8PollinMode 1: polled sending; 0: interrupt sending;
 **
 ** \return 1: if succesful, 0: if usb was not ready
 *****************************************************************************/
boolean_t UsbDeviceCdcCom_SendDataVia1(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode)
{
    if (UsbDevice_GetStatus(pstcUsb) == UsbConfigured)
    {
        DataSentFlags &= ~(1<<1);
        
        UsbDevice_SendData(pstcUsb, pstcEpIN1, pu8Buffer, u32DataSize, enMode);
        if (enMode == UsbPOLL)
        {
            DataSentFlags |= (1<<1);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 ******************************************************************************
 ** Is called from UsbFunctions.c when a endpoint buffer 1 was sent
 **
 ** \param stcEvent Event containing all neccesary information
 *****************************************************************************/
static void UsbDeviceCdcCom_TxCallbackEp1(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint)
{
    DataSentFlags |= (1<<1);
}
/**
 ******************************************************************************
 ** Is used to use get the sent status of endpoint 1
 **
 *****************************************************************************/
uint8_t UsbDeviceCdcCom_DataSent1(void)
{
    if ((DataSentFlags & (1<<1)) > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


