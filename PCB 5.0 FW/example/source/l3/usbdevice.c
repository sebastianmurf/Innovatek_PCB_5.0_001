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
/** \file usbdevice.c
 **
 **
 ** A detailed description is available at 
 ** @link UsbDeviceGroup USB Device Module description @endlink
 **
 ** History:
 **   - 2012-08-28  2.0  MSc  First version (starting at version 2.0).
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/


#include "usbdevice.h"
#include "variable.h"
#include <stdio.h>

#if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))



/**
 ******************************************************************************
 ** \ingroup UsbDeviceGroup
 ******************************************************************************/
//@{

#if (USE_USBDEVICEHW_H == 1)
#include "usbdevicehw.h"
#endif


static stc_usbdevice_intern_data_t* UsbDeviceGetInternDataPtr(const stc_usbn_t* pstcUsb) ;
static void EndpointContinueTxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint);
static void EndpointContinueRxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint);
static void ControlRxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint);
static void ControlTxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint);
static void ConfigChangeCallback(stc_usbn_t* pstcUsb);
static void SetupRxCallback(stc_usbn_t* pstcUsb);





/// Macro to return the number of enabled MFS instances
#define USBDEVICE_INSTANCE_COUNT (uint32_t)(sizeof(m_astcUsbDeviceInstanceDataLut) / sizeof(m_astcUsbDeviceInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled MFS instances and their internal data
stc_usbdevicen_instance_data_t m_astcUsbDeviceInstanceDataLut[] =
{
    #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)) && (L3_PERIPHERAL_ENABLE_USB0_DEVICE == L3_ON))
    { &USB0,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)) && (L3_PERIPHERAL_ENABLE_USB1_DEVICE == L3_ON))
    { &USB1,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    }
    #endif
};

/**
 ******************************************************************************
 ** \brief Return the internal data for a certain USB instance.
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_usbdevice_intern_data_t* UsbDeviceGetInternDataPtr(const stc_usbn_t* pstcUsb) 
{
    volatile uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < USBDEVICE_INSTANCE_COUNT; u32Instance++)
    {
        if ((uint32_t)pstcUsb == (uint32_t)(m_astcUsbDeviceInstanceDataLut[u32Instance].pstcInstance))
        {
            return &m_astcUsbDeviceInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}


/**
 ******************************************************************************
 ** \brief Callback function, called after sending via endpoints (IN direction)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcEndpoint Pointer to endpoint instance
 **
 ** \return none
 **
 ******************************************************************************/
static void EndpointContinueTxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint)
{
    uint16_t u16DataSend = 0;
    uint32_t u32TransferredData;
    
    USBDBG(">TxClbk\n");
    
    u32TransferredData = (((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8BufferPos) - ((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8Buffer));
    USBDBGVAL16("Trsfrd: ",u32TransferredData);
    USBDBGVAL16("Sze: ",pstcEndpoint->pstcEndpointBuffer->u32DataSize);
    
    if (u32TransferredData >= ((uint32_t)pstcEndpoint->pstcEndpointBuffer->u32DataSize))
    {
      USBDBG("EOT\n");
      BITMASK_CLEAR(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
      if (pstcEndpoint->pfnRxTxBlockCallback != NULL)
      {
          if ((pstcEndpoint->u8EndpointAddress == 0x80) && (pstcEndpoint->pstcEndpointBuffer->u32DataSize % 64 == 0))
          {
              USBDBG("NULL");
              Usb_HalSend(pstcEndpoint,pstcEndpoint->pstcEndpointBuffer->pu8Buffer,0,&u16DataSend);
          }
          pstcEndpoint->pfnRxTxBlockCallback(pstcUsb, pstcEndpoint);
      }
      return;
      //Data Transfer End
    }
    BITMASK_CLEAR(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
    Usb_HalSend(pstcEndpoint,pstcEndpoint->pstcEndpointBuffer->pu8Buffer,pstcEndpoint->pstcEndpointBuffer->u32DataSize - u32TransferredData,&u16DataSend);
    pstcEndpoint->pstcEndpointBuffer->pu8BufferPos += u16DataSend;
    BITMASK_SET(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
}

/**
 ******************************************************************************
 ** \brief Callback function, called after receiving via endpoints (OUT direction)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcEndpoint Pointer to endpoint instance
 **
 ** \return none
 **
 ******************************************************************************/
static void EndpointContinueRxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint)
{
    uint16_t u16DataReceived = 0;
    Usb_HalReceive(pstcEndpoint,pstcEndpoint->pstcEndpointBuffer->pu8BufferPos,&u16DataReceived);
    BITMASK_CLEAR(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
    pstcEndpoint->pstcEndpointBuffer->u32DataSize += u16DataReceived;
    pstcEndpoint->pstcEndpointBuffer->pu8BufferPos += u16DataReceived;
    if (((((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8BufferPos) - ((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8Buffer)) >= ((uint32_t)pstcEndpoint->pstcEndpointBuffer->u32BufferSize)) || (u16DataReceived < pstcEndpoint->u16EndpointSize))
    {
      pstcEndpoint->pfnRxTxCallback = NULL;
      if (pstcEndpoint->pfnRxTxBlockCallback != NULL)
      {
          pstcEndpoint->pfnRxTxBlockCallback(pstcUsb, pstcEndpoint);
      }
      return;
      //Data Transfer End
    }
    BITMASK_SET(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
}


/**
 ******************************************************************************
 ** \brief Initiate receiving data (OUT direction)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcEndpoint Pointer to endpoint instance
 **
 ** \param pu8Buffer Pointer to data buffer
 **
 ** \param u32DataSize Size of data to be received
 **
 ** \param enMode Mode how to send data (polled, irq driven or dma driven)
 **
 ** \return none
 **
 ******************************************************************************/
en_result_t UsbDevice_ReceiveData(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint, uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode)
{
    uint16_t u16DataReceived = 0;
    if (pstcEndpoint->pstcEndpointBuffer == NULL)
    {
        return ErrorInvalidParameter;
    }
    pstcEndpoint->pstcEndpointBuffer->pu8Buffer = pu8Buffer;
    pstcEndpoint->pstcEndpointBuffer->pu8BufferPos = pu8Buffer;
    pstcEndpoint->pstcEndpointBuffer->u32BufferSize = u32DataSize;
    pstcEndpoint->pstcEndpointBuffer->u32DataSize = 0;
    switch(enMode)
    {
      case UsbPOLL:
        while((((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8BufferPos) - ((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8Buffer)) < ((uint32_t)pstcEndpoint->pstcEndpointBuffer->u32BufferSize))
        {
            while (BITMASK_ISCLEARED(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQ));
            Usb_HalReceive(pstcEndpoint,pstcEndpoint->pstcEndpointBuffer->pu8BufferPos,&u16DataReceived);
            pstcEndpoint->pstcEndpointBuffer->u32DataSize += u16DataReceived;
            pstcEndpoint->pstcEndpointBuffer->pu8BufferPos += u16DataReceived;
            if (u16DataReceived < pstcEndpoint->u16EndpointSize)
            {
                return Ok;
            }
        }
        break;
      case UsbIRQ:
        pstcEndpoint->pfnRxTxCallback = EndpointContinueRxCallback;
        BITMASK_SET(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
        break;
      case UsbDMA:
        break;
    }
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Initiate sending data (IN direction)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcEndpoint Pointer to endpoint instance
 **
 ** \param pu8Buffer Pointer to data buffer
 **
 ** \param u32DataSize Size of data to be send
 **
 ** \param enMode Mode how to send data (polled, irq driven or dma driven)
 **
 ** \return none
 **
 ******************************************************************************/
en_result_t UsbDevice_SendData(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint, uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode)
{
    uint16_t u16DataSend = 0;
    
    USBDBGVAL8("New ",(uint32_t)pstcEndpoint->u8EndpointAddress);
    USBDBGVAL8("Sze ",(uint32_t)u32DataSize);
	
	if (pstcEndpoint->pstcEndpointBuffer == NULL)
    {
        USBDBG("\nERROR: Buffer == NULL!");
		return ErrorInvalidParameter;
    }
    //if(*pu8Buffer == '\n')//nabeen
	//  return Ok;//nabeen
    pstcEndpoint->pstcEndpointBuffer->pu8Buffer = pu8Buffer;
    pstcEndpoint->pstcEndpointBuffer->pu8BufferPos = pu8Buffer;
    pstcEndpoint->pstcEndpointBuffer->u32BufferSize = u32DataSize;
    pstcEndpoint->pstcEndpointBuffer->u32DataSize = u32DataSize;
    
    switch(enMode)
    {
      case UsbPOLL:
        while((((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8BufferPos) - ((uint32_t)pstcEndpoint->pstcEndpointBuffer->pu8Buffer)) < ((uint32_t)pstcEndpoint->pstcEndpointBuffer->u32DataSize))
        {
            while (BITMASK_ISCLEARED(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQ));
            Usb_HalSend(pstcEndpoint,pu8Buffer,u32DataSize,&u16DataSend);
            pstcEndpoint->pstcEndpointBuffer->pu8BufferPos += u16DataSend;
        }
        break;
      case UsbIRQ:
        pstcEndpoint->pfnRxTxCallback = EndpointContinueTxCallback;
        Usb_HalSend(pstcEndpoint,pu8Buffer,u32DataSize,&u16DataSend);
        pstcEndpoint->pstcEndpointBuffer->pu8BufferPos += u16DataSend;
        BITMASK_SET(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
        break;
      case UsbDMA:
        break;
    }
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Callback function, called after receiving via control endpoint 0 (OUT direction)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcEndpoint Pointer to endpoint instance
 **
 ** \return none
 **
 ******************************************************************************/
static void ControlRxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint)
{
    uint8_t pu8Buffer[64];
    uint16_t u16BytesCount;
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    
    Usb_HalReceive(pstcEndpoint,pu8Buffer,&u16BytesCount);
    pstcEndpoint->pstcEndpointBuffer->pu8Buffer = pu8Buffer;
    pstcEndpoint->pstcEndpointBuffer->u32DataSize = u16BytesCount;
    if (u16BytesCount == 0)
    {
        if (pstcUsbDeviceIntern->enControlStage == StatusStage)
        {
            pstcUsbDeviceIntern->enControlStage = ControlStageUnknown;
        }
        if (pstcUsbDeviceIntern->pfnControlTransferred != NULL)
        {
            pstcUsbDeviceIntern->pfnControlTransferred(pstcUsb, pstcEndpoint);
            pstcUsbDeviceIntern->pfnControlTransferred = NULL;
        }
        //UsbDevice_SendData(pstcUsb, pstcUsbDeviceIntern->pstcEp0IN, pu8Buffer, 0, UsbIRQ);
    }
    pstcEndpoint->pstcEndpointBuffer->pu8Buffer = NULL;
    pstcEndpoint->pstcEndpointBuffer->u32DataSize = 0;
}

/**
 ******************************************************************************
 ** \brief Callback function, called after sending via control endpoint 0 (IN direction)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcEndpoint Pointer to endpoint instance
 **
 ** \return none
 **
 ******************************************************************************/
static void ControlTxCallback(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint)
{
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    pstcUsbDeviceIntern->enControlStage = StatusStage;
}

/**
 ******************************************************************************
 ** \brief Callback function, called after device was configured
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return none
 **
 ******************************************************************************/
static void ConfigChangeCallback(stc_usbn_t* pstcUsb)
{
    uint8_t u8i;
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb); // get internal i
    
    if (Usb_GetConfiguration(pstcUsb) == 0)            // Check configuration
    {
        pstcUsbDeviceIntern->enStatus = UsbConnected;  // If it is 0, set status connected
    }
    else
    {
        pstcUsbDeviceIntern->enStatus = UsbConfigured; // If it is not 0, set status configured
    }
    for(u8i = 0;u8i < USBDEVICE_MAXCLASSES;u8i++)
    {
        if (pstcUsbDeviceIntern->stcUsbClasses[u8i].pfnConfCallback != NULL)
        {
            pstcUsbDeviceIntern->stcUsbClasses[u8i].pfnConfCallback(pstcUsb);
        }
    }
}

/**
 ******************************************************************************
 ** \brief Callback function, called after setup was received
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return none
 **
 ******************************************************************************/
static void SetupRxCallback(stc_usbn_t* pstcUsb)
{
    uint8_t u8DataToSend[255];
	  uint16_t u16BytesCount;
    uint16_t u16Length;
    uint8_t pu8Buffer[8];
    uint8_t u8Size;
    stc_usb_request_t stcSetup;
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);

    Usb_HalReceive(pstcUsbDeviceIntern->pstcEp0OUT,pu8Buffer,&u16BytesCount);
    
    pstcUsbDeviceIntern->enControlStage = SetupStage;
    
    /* Decode Setup Package */
    stcSetup.bmRequestType = pu8Buffer[0];
    stcSetup.bRequest = pu8Buffer[1];
    stcSetup.wValue = pu8Buffer[3] << 8;
    stcSetup.wValue += pu8Buffer[2];
    stcSetup.wIndex = pu8Buffer[5] << 8;
    stcSetup.wIndex += pu8Buffer[4];    
    stcSetup.wLength = pu8Buffer[7] << 8;
    stcSetup.wLength += pu8Buffer[6];  
    
    if (stcSetup.bmRequestType_f.DataPhaseTransferDirection == 1)
    {
        pstcUsbDeviceIntern->enControlStage = DataStageIN;
    }
    else
    {
        pstcUsbDeviceIntern->enControlStage = DataStageOUT;
    }
    
    if ((pu8Buffer[0] & 0x60) == USB_DEVREQ_CLSTYPE)
    {
        if(pu8Buffer[1] == USB_DEVREQ_SET_IDLE)
        {
            /*** recive SET_IDLE ***/
            BITMASK_CLEAR(pstcUsb->EP0IS,_EP0IS_DRQI);    /* DRQi <- 0 */
            return;
        }
    }
    
    switch (stcSetup.bmRequestType_f.Type)
    {
    case 0: // Device
      switch(stcSetup.bRequest)
      {
        case GET_DESCRIPTOR:
            switch((stcSetup.wValue) >> 8)
            {
                case 1:    // Device Descriptor
                  u16Length = stcSetup.wLength;
                  USBDBG("Req Dev Desc\n");
                  USBDBGVAL16("Len ",u16Length);
                  if (u16Length > pstcUsbDeviceIntern->pu8DeviceDescriptor[0]) u16Length = pstcUsbDeviceIntern->pu8DeviceDescriptor[0];
                  USBDBGVAL16("Len New ",u16Length);
                  pstcUsbDeviceIntern->pstcEp0IN->pfnRxTxBlockCallback = ControlTxCallback;
                  UsbDevice_SendData(pstcUsb, pstcUsbDeviceIntern->pstcEp0IN, pstcUsbDeviceIntern->pu8DeviceDescriptor, u16Length, UsbIRQ);
                  break;
                case 2:    // Config Descriptor
                  u16Length = stcSetup.wLength;
                  USBDBG("Req Conf Desc\n");
                  USBDBGVAL16("Len ",u16Length);
                  if (u16Length > (pstcUsbDeviceIntern->pu8ConfigDescriptor[2] + pstcUsbDeviceIntern->pu8ConfigDescriptor[3]*256)) u16Length = (pstcUsbDeviceIntern->pu8ConfigDescriptor[2] + pstcUsbDeviceIntern->pu8ConfigDescriptor[3]*256);
                  USBDBGVAL16("Len New ",u16Length);
                  pstcUsbDeviceIntern->pstcEp0IN->pfnRxTxBlockCallback = ControlTxCallback;
                  UsbDevice_SendData(pstcUsb, pstcUsbDeviceIntern->pstcEp0IN, pstcUsbDeviceIntern->pu8ConfigDescriptor, u16Length, UsbIRQ);
                  break;
                case 3:    // String Descriptor 
                  u16Length = stcSetup.wLength;
                  u8DataToSend[0] = 0;
                  switch ((stcSetup.wValue) & 0xFF)
                  {
                      case 0:
                          u8DataToSend[0] = 0x04;
                          u8DataToSend[1] = 0x03;
                          u8DataToSend[2] = 0x09;
                          u8DataToSend[3] = 0x04;
                          break;
                      default:
                          if (((stcSetup.wValue) & 0xFF) <= pstcUsbDeviceIntern->u8StringDescriptorCount)
                          {
                              if (pstcUsbDeviceIntern->pstcStringDescriptors[(stcSetup.wValue) & 0xFF].pu8String != NULL)
                              {
                                  u8DataToSend[0] = 2;
                                  u8DataToSend[1] = 0x03;
                                  for(u8Size=0;(pstcUsbDeviceIntern->pstcStringDescriptors[((stcSetup.wValue) & 0xFF)-1].pu8String[u8Size] != 0);u8Size++)
                                  {
                                      u8DataToSend[2 + u8Size * 2] = pstcUsbDeviceIntern->pstcStringDescriptors[((stcSetup.wValue) & 0xFF)-1].pu8String[u8Size];
                                      u8DataToSend[3 + u8Size * 2] = 0;
                                  }
                                  u8DataToSend[0] = u8DataToSend[0] + 2 * u8Size;
                              } else
                              {
                                  /* tbd UNICODE */
                              }
                          }
                          
                  }
                  UsbDevice_SendData(pstcUsb, pstcUsbDeviceIntern->pstcEp0IN, u8DataToSend, u8DataToSend[0], UsbIRQ);
                  break;
                case 0x22: // Report Descriptor
                  u16Length = stcSetup.wLength;
                  USBDBG("Req Rep Desc\n");
                  USBDBGVAL16("Len ",u16Length);
                  if (u16Length > pstcUsbDeviceIntern->u16ReportDescriptorSize) u16Length = pstcUsbDeviceIntern->u16ReportDescriptorSize;
                  USBDBGVAL16("Len New ",u16Length);
                  UsbDevice_SendData(pstcUsb, pstcUsbDeviceIntern->pstcEp0IN, pstcUsbDeviceIntern->pu8ReportDescriptor, u16Length, UsbIRQ);
                default:
                  USBDBGVAL8("USB Request Unknown Descriptor: \n",(stcSetup.wValue) >> 8);
                  break;
            }
            break;
      default:
        USBDBGVAL8("USB Request Unknown: ",stcSetup.bRequest);
      }
      break;
    case 1: // Class
      if (stcSetup.wIndex < 3)
      {
          if (pstcUsbDeviceIntern->stcUsbClasses[stcSetup.wIndex].pfnSetupRequestCallback != NULL)
          {
              pstcUsbDeviceIntern->stcUsbClasses[stcSetup.wIndex].pfnSetupRequestCallback(pstcUsb, &stcSetup);
          }
      }
      break;
    case 2: // Vendor
      break;
    }
    if ((pu8Buffer[0] & 0x60) == USB_DEVREQ_CLSTYPE)
    {
        if(pu8Buffer[1] == USB_DEVREQ_SET_IDLE)
        {
            /*** recive SET_IDLE ***/
            BITMASK_CLEAR(pstcUsb->EP0IS,_EP0IS_DRQI);    /* DRQi <- 0 */
            return;
        }
    }
    u16BytesCount = u16BytesCount;
}

/**
 ******************************************************************************
 ** \brief Send control data
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pu8Buffer Buffer
 **
 ** \param u32DataLength Data length
 **
 ** \return none
 **
 ******************************************************************************/
void UsbDevice_SendDataControl(stc_usbn_t * pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataLength)
{
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    pstcUsbDeviceIntern->pstcEp0IN->pfnRxTxBlockCallback = ControlTxCallback;
    UsbDevice_SendData(pstcUsb, pstcUsbDeviceIntern->pstcEp0IN, pu8Buffer, u32DataLength, UsbIRQ);
}

/**
 ******************************************************************************
 ** \brief Receive control data
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pfnCompletionCallback Data received callback
 **
 ** \return none
 **
 ******************************************************************************/
void UsbDevice_ReceiveDataControl(stc_usbn_t * pstcUsb, usbdevice_controltranfered_func_ptr_t pfnCompletionCallback)
{
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    pstcUsbDeviceIntern->pfnControlTransferred = pfnCompletionCallback;
}

/**
 ******************************************************************************
 ** \brief Register USB class
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcUsbClassConfig configuration of USB class
 **
 ** \param ppu8Buffer Pointer to buffer where to return configuration buffer pointer
 **
 ** \param pu8InterfaceLength Pointer to where to return length of configuration buffer
 **
 ** \return Ok if registered, Error if error occured
 **
 ******************************************************************************/
en_result_t UsbDevice_RegisterVendorClass(stc_usbn_t* pstcUsb, stc_usbdevice_class_config_t* pstcUsbClassConfig, uint8_t** ppu8Buffer, uint8_t* pu8InterfaceLength)
{
    uint16_t u16i;
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    
    for(u16i = 0;u16i < (pstcUsbDeviceIntern->pu8ConfigDescriptor[2] + pstcUsbDeviceIntern->pu8ConfigDescriptor[3]*256);u16i++)
    {
        if (pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+1] == USBDESCR_INTERFACE)
        {
            if  (((pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+5] == pstcUsbClassConfig->u8InterfaceClass) &&
                (pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+6] == pstcUsbClassConfig->u8InterfaceSubClass) &&
                (pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+7] == pstcUsbClassConfig->u8InterfaceProtocoll)) && 
                ((pstcUsbClassConfig->u8InterfaceNumber == 0xFF) || (pstcUsbClassConfig->u8InterfaceNumber == pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2])))
            {
                *ppu8Buffer = (pstcUsbDeviceIntern->pu8ConfigDescriptor + u16i);
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].u8InterfaceClass = pstcUsbClassConfig->u8InterfaceClass;
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].u8InterfaceSubClass = pstcUsbClassConfig->u8InterfaceSubClass;
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].u8InterfaceProtocoll = pstcUsbClassConfig->u8InterfaceProtocoll;
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].pfnSetupRequestCallback = pstcUsbClassConfig->pfnSetupRequestCallback;
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].pfnConnectionCallback = pstcUsbClassConfig->pfnConnectionCallback;
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].pfnDisconnectionCallback = pstcUsbClassConfig->pfnDisconnectionCallback;
                pstcUsbDeviceIntern->stcUsbClasses[pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+2]].pfnConfCallback = pstcUsbClassConfig->pfnConfCallback;
                while(u16i < (pstcUsbDeviceIntern->pu8ConfigDescriptor[2] + pstcUsbDeviceIntern->pu8ConfigDescriptor[3]*256))
                {
                    u16i += pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i];    // go to next descriptor
                    if (pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i+1] == USBDESCR_INTERFACE) 
                    {
                        break;
                    }
                }
                *pu8InterfaceLength = (uint8_t)((uint32_t)u16i - (uint32_t)(((uint32_t)*ppu8Buffer - (uint32_t)pstcUsbDeviceIntern->pu8ConfigDescriptor)));
                return Ok;
            }
        }
        u16i += pstcUsbDeviceIntern->pu8ConfigDescriptor[u16i] - 1;    // go to next descriptor
    }
    return Error;
}
       
/**
 ******************************************************************************
 ** \brief Get Usb Device status
 **
 ** \param pstcUsb Pointer to USB instance
 **
 **
 ** \return status as en_usb_status_t
 **
 ******************************************************************************/
en_usb_status_t UsbDevice_GetStatus(stc_usbn_t* pstcUsb)
{
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb); 
    /*switch(pstcUsbDeviceIntern->enStatus)
	{	case UsbDisconnected: PutStringPc("\n\rUSB is disconnected");break;
		case UsbConnected:    PutStringPc("\n\rUSB is connected");break;
		case UsbAddressed:    PutStringPc("\n\rUSB is addressed");break;
		case UsbConfigured:		PutStringPc("\n\rUSB is configured");break;
	}*/
	return pstcUsbDeviceIntern->enStatus; 
}

/**
 ******************************************************************************
 ** \brief Initialization handled from Usb Lowlevel Driver (usb.c)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcUsbConfig Usb Configuration
 **
 ******************************************************************************/
void UsbDevice_InitCallback(stc_usbn_t* pstcUsb, stc_usb_config_t* pstcUsbConfig)
{
    uint16_t i;
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    for(i = 0;i<(pstcUsbDeviceIntern->pu8ConfigDescriptor[2] + pstcUsbDeviceIntern->pu8ConfigDescriptor[3]*256);i++)
    {
        if (pstcUsbDeviceIntern->pu8ConfigDescriptor[i+1] == USBDESCR_ENDPOINT)
        {
            Usb_SetupHalEndpoint(pstcUsb,pstcUsbDeviceIntern->pu8ConfigDescriptor[i+2], pstcUsbDeviceIntern->pu8ConfigDescriptor[i+4] | (pstcUsbDeviceIntern->pu8ConfigDescriptor[i+5] << 8),(en_usb_ep_type_t)(pstcUsbDeviceIntern->pu8ConfigDescriptor[i+3] & 0x03),FALSE);
        }
        i += pstcUsbDeviceIntern->pu8ConfigDescriptor[i] - 1;    // go to next descriptor
    }
    pstcUsbConfig->pfnSetpCallback = &SetupRxCallback;
    pstcUsbConfig->pfnConfCallback = &ConfigChangeCallback;
    if (pstcUsbDeviceIntern->pfnInitClassesCallback != NULL)
    {
        pstcUsbDeviceIntern->pfnInitClassesCallback(pstcUsb);
    }
}

/**
 ******************************************************************************
 ** \brief Initialization handled from Usb Lowlevel Driver (usb.c)
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcUsbConfig Usb Configuration To Export Data
 **
 ** \param pstcConfig Usb Device Config
 **
 ******************************************************************************/
void UsbDevice_Init(stc_usbn_t* pstcUsb, stc_usb_config_t* pstcUsbConfig, stc_usbdevice_config_t* pstcConfig)
{
    stc_usbdevice_intern_data_t* pstcUsbDeviceIntern;
    pstcUsbDeviceIntern = UsbDeviceGetInternDataPtr(pstcUsb);
    
    pstcUsbConfig->enMode |= UsbDeviceEnabled;
    pstcUsbConfig->pu8DeviceDescriptor = pstcConfig->pu8DeviceDescriptor;
    pstcUsbConfig->pu8ConfigDescriptor = pstcConfig->pu8ConfigDescriptor; 
    pstcUsbConfig->pfnDeviceInit = UsbDevice_InitCallback;
    //pstcUsbConfig->pstcStringDescriptors = pstcConfig->pstcStringDescriptors;
    //pstcUsbConfig->u8StringDescriptorCount = pstcConfig->u8StringDescriptorCount;
      
    pstcUsbDeviceIntern->pu8DeviceDescriptor = pstcConfig->pu8DeviceDescriptor;
    pstcUsbDeviceIntern->pu8ConfigDescriptor = pstcConfig->pu8ConfigDescriptor;
    pstcUsbDeviceIntern->pu8ReportDescriptor = pstcConfig->pu8ReportDescriptor;
    pstcUsbDeviceIntern->u16ReportDescriptorSize = pstcConfig->u16ReportDescriptorSize;
    pstcUsbDeviceIntern->pstcStringDescriptors = pstcConfig->pstcStringDescriptors;
    pstcUsbDeviceIntern->u8StringDescriptorCount = pstcConfig->u8StringDescriptorCount;
    
    pstcUsbDeviceIntern->pfnInitClassesCallback = pstcConfig->pfnInitClassesCallback;
    
    
    pstcUsbDeviceIntern->pstcEp0IN = Usb_GetEndpointPtr(pstcUsb,0x80);
    pstcUsbDeviceIntern->pstcEp0OUT = Usb_GetEndpointPtr(pstcUsb,0x00);

    pstcUsbDeviceIntern->pstcEp0IN->pstcEndpointBuffer = &(pstcUsbDeviceIntern->stcEndpointBuffer);
    pstcUsbDeviceIntern->pstcEp0OUT->pstcEndpointBuffer = &(pstcUsbDeviceIntern->stcEndpointBuffer);
        
    pstcUsbDeviceIntern->pstcEp0IN->pfnRxTxCallback = &ControlTxCallback;
    pstcUsbDeviceIntern->pstcEp0OUT->pfnRxTxCallback = &ControlRxCallback;
}


/**
 ******************************************************************************
 ** \brief Setup Endpoint
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcConfig Pointer of endpoint configuration
 **
 ** \return pointer of endpoint handle #stc_usbn_endpoint_data_t
 **
 ******************************************************************************/
stc_usbn_endpoint_data_t* UsbDevice_SetupEndpoint(stc_usbn_t* pstcUsb, stc_usbdevice_endpoint_config_t* pstcConfig)
{
    stc_usbn_endpoint_data_t* pstcEndpointHandle;
    pstcEndpointHandle = Usb_GetEndpointPtr(pstcUsb, pstcConfig->u8EndpointAddress);
    pstcEndpointHandle->pfnRxTxBlockCallback = pstcConfig->pfnRxTxCallback;
    pstcEndpointHandle->pstcEndpointBuffer = pstcConfig->pstcEndpointBuffer;
    return pstcEndpointHandle;
}

//@} // UsbDeviceGroup
#endif /* !USB_DISBALE_DEVICE_FUNCTIONALITY */
