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
/** \file UsbClassSupervisor.c
 **
 ** Part of FSEU USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version
 **   - 2012-06-01    2.0  MSc  New Version for use with M3 L3 USB driver
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#define __USBCLASSSUPERVISOR_C__
#include "UsbClassSupervisor.h"

#ifndef USB_DISBALE_HOST_FUNCTIONALITY


/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


/*****************************************************************************/
/* function */
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


/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

/// Macro to return the number of enabled MFS instances
#define USBCLASSSUPERVISOR_INSTANCE_COUNT (uint32_t)(sizeof(m_astcUsbClassSupervisorInstanceDataLut) / sizeof(m_astcUsbClassSupervisorInstanceDataLut[0]))

/// Look-up table for all enabled MFS instances and their internal data
stc_usbclasssupervisorn_instance_data_t m_astcUsbClassSupervisorInstanceDataLut[] =
{
    #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)) && (L3_PERIPHERAL_ENABLE_USB0_HOST == L3_ON))
    { &USB0,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)) && (L3_PERIPHERAL_ENABLE_USB1_HOST == L3_ON))
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
static stc_usbclasssupervisor_intern_data_t* UsbClassSupervisorGetInternDataPtr(stc_usbn_t* pstcUsb) 
{
    volatile uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < USBCLASSSUPERVISOR_INSTANCE_COUNT; u32Instance++)
    {
        if ((uint32_t)pstcUsb == (uint32_t)(m_astcUsbClassSupervisorInstanceDataLut[u32Instance].pstcInstance))
        {
            return &m_astcUsbClassSupervisorInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}

void UsbClassSupervisor_Init(stc_usbn_t* pstcUsb)
{
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    
    pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_IDLE;
    pstcUsbClassSupervisorIntern->CurrentDriver = 0;
    pstcUsbClassSupervisorIntern->InternalErrorHandler = 0;
    Usb_RemoveTimeOut(pstcUsb, UsbClassSupervisor_SysTickHandler);
    Usb_AddTimeOut(pstcUsb, UsbClassSupervisor_SysTickHandler,200);
}

void UsbClassSupervisor_Deinit(stc_usbn_t* pstcUsb)
{
	  volatile uint8_t i;
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    pstcUsbClassSupervisorIntern->CurrentDriver = 0;
    for(i=0;i<MAX_CLASSHANDLERS;i++)
    {
        if (UsbSupervisor_UsbClassMatch((UsbClassHandler_t*)&UsbClassHandlers[i],&pstcUsbClassSupervisorIntern->stcMatchClass))
        {
            if (UsbClassHandlers[i].DeviceDisconnection != NULL)
            {
                UsbClassHandlers[i].DeviceDisconnection(pstcUsb);
                break;
            }
        }
    }
}

void UsbClassSupervisor_SysTickHandler(stc_usbn_t* pstcUsb)
{
	  volatile uint8_t i;
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    
    if ((UsbHost_GetDeviceStatus(pstcUsb) == USBHOST_DEVICE_ADDRESSED) && (UsbHost_GetDeviceStatus(pstcUsb) != USBHOST_DEVICE_CONFIGURED))
    {
        switch(pstcUsbClassSupervisorIntern->u8SupervisorState)
        {
            case USBCLASSSVR_IDLE:
               USBDBG("Requesting Class Informations\n");
               pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_REQUEST_DESCRIPTORS;
               UsbHost_SetupRequest(pstcUsb,(stc_usb_request_t *)&stcDeviceDescriptorRequest,ReturnedDeviceDescriptor);
               break;
            case USBCLASSSVR_CLASSCHECK:
               pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_IDLE;
               for(i=0;i<MAX_CLASSHANDLERS;i++)
               {
                   if (UsbSupervisor_UsbClassMatch((UsbClassHandler_t*)&UsbClassHandlers[i],&pstcUsbClassSupervisorIntern->stcMatchClass))
                   {
                       if (UsbClassHandlers[i].InitializeClass != NULL)
                       {
                           if (UsbClassHandlers[i].InitializeClass(pstcUsb,pstcUsbClassSupervisorIntern->pu8Configuration,pstcUsbClassSupervisorIntern->u32ConfigurationSize) == TRUE)
                           {
                               USBDBG("Setting Configuration (1)\n");
                               UsbHost_SetConfigurationDescriptor(pstcUsb,1);
                               pstcUsbClassSupervisorIntern->CurrentDriver = UsbClassHandlers[i].stcDriverType;
                               break;
                           }
                       }
                   }
               }
               if (pstcUsbClassSupervisorIntern->CurrentDriver == 0)
               {
                   pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_ERROR;
                   if (pstcUsbClassSupervisorIntern->InternalErrorHandler != 0)
                   {
                       pstcUsbClassSupervisorIntern->InternalErrorHandler(pstcUsb,pstcUsbClassSupervisorIntern->stcMatchClass);
                   }
               }
               break;
            default:
               break;
        }

    }
    if (UsbHost_GetDeviceStatus(pstcUsb) != USBHOST_DEVICE_CONFIGURED)
    {
        Usb_AddTimeOut(pstcUsb,UsbClassSupervisor_SysTickHandler,100);
    }
}

void UsbClassSupervisor_SetErrorHandler(stc_usbn_t* pstcUsb,void (*Handler)(stc_usbn_t* pstcUsb,UsbClassHandler_t stcMatchClass))
{
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    pstcUsbClassSupervisorIntern->InternalErrorHandler = Handler;
}

UsbClassDriver_t UsbClassSupervisor_GetActiveDriver(stc_usbn_t* pstcUsb)
{
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    return pstcUsbClassSupervisorIntern->CurrentDriver;
}

void ReturnedDeviceDescriptor(stc_usbn_t* pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataSize)
{
    stc_usb_request_t stcSetup = {0x80,0x06,0x0200,0x00,0x09};
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    Usb_RemoveTimeOut(pstcUsb,UsbClassSupervisor_SysTickHandler);
    Usb_AddTimeOut(pstcUsb,UsbClassSupervisor_SysTickHandler,200);
    pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_IDLE;
    pstcUsbClassSupervisorIntern->stcMatchClass.u16IdVendor = (uint16_t)((uint16_t)pu8Buffer[8] + ((uint16_t)pu8Buffer[9] << 8));
    pstcUsbClassSupervisorIntern->stcMatchClass.u16IdProduct = (uint16_t)((uint16_t)pu8Buffer[10] + ((uint16_t)pu8Buffer[11] << 8));
    USBDBG("Device Descriptor Returned\n");
    if (pu8Buffer[4] != 0)
    {
        pstcUsbClassSupervisorIntern->stcMatchClass.u8MatchDeviceClass    = pu8Buffer[4];
        pstcUsbClassSupervisorIntern->stcMatchClass.u8MatchDeviceSubClass = pu8Buffer[5];
        pstcUsbClassSupervisorIntern->stcMatchClass.u8MatchDeviceProtocol = pu8Buffer[6];
    }
    //stcSetup.wValue |= 1;
    USBDBG("Requesting 9 byte of Configuration\n");
    UsbHost_SetupRequest(pstcUsb,(stc_usb_request_t *)&stcSetup,ReturnedConfigurationDescriptor);
}

void ReturnedConfigurationDescriptor(stc_usbn_t* pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataSize)
{
    uint8_t i;
    uint8_t u8NumberOfInterfaces;
    uint8_t* pu8Pointer;
    stc_usb_request_t stcSetup = {0x80,0x06,0x0200,0x00,9};
    stc_usbclasssupervisor_intern_data_t* pstcUsbClassSupervisorIntern;
    pstcUsbClassSupervisorIntern = UsbClassSupervisorGetInternDataPtr(pstcUsb);
    Usb_RemoveTimeOut(pstcUsb,UsbClassSupervisor_SysTickHandler);
    Usb_AddTimeOut(pstcUsb,UsbClassSupervisor_SysTickHandler,200);
    pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_IDLE;
    
    if (pu8Buffer[1] != USB_CONFIGURATION_DESCRIPTOR_TYPE) 
    {
        USBDBG("Error while receiving configuration!\n");
        USBDBG("Requesting 9 byte of Configuration\n");
        UsbHost_SetupRequest(pstcUsb,(stc_usb_request_t *)&stcSetup,ReturnedConfigurationDescriptor);
        return;
    }
    if (u32DataSize == 9)
    {
        USBDBG("Requesting 9 byte of Configuration received\n");
        //stcSetup.wValue |= 1;
        stcSetup.wLength = ((uint8_t)pu8Buffer[2]) | (((uint8_t)pu8Buffer[3]) << 8);
        USBDBG("Requesting full Configuration\n");
        UsbHost_SetupRequest(pstcUsb,(stc_usb_request_t *)&stcSetup,ReturnedConfigurationDescriptor);
        return;
    }
   
    USBDBG("Configuration received.\n");
    pstcUsbClassSupervisorIntern->pu8Configuration = pu8Buffer;
    pstcUsbClassSupervisorIntern->u32ConfigurationSize = u32DataSize;
    u8NumberOfInterfaces = pu8Buffer[4];
    for(i = 0;i<u8NumberOfInterfaces;i++)
    {
        pu8Pointer = UsbHost_GetUsbInterfaceDescriptor(pu8Buffer,(uint16_t)u32DataSize,i);
        if (pu8Pointer != 0)
        {
            if (pu8Pointer[5] != 0)
            {
                pstcUsbClassSupervisorIntern->stcMatchClass.u8MatchInterfaceClass = pu8Pointer[5];
                pstcUsbClassSupervisorIntern->stcMatchClass.u8MatchInterfaceSubClass = pu8Pointer[6];
                pstcUsbClassSupervisorIntern->stcMatchClass.u8MatchInterfaceProtocol = pu8Pointer[7];
                pstcUsbClassSupervisorIntern->u8SupervisorState = USBCLASSSVR_CLASSCHECK;
                return;
            }
        }
    }
}

boolean_t UsbSupervisor_UsbClassMatch(UsbClassHandler_t* stcA, UsbClassHandler_t* stcB)
{
    if ((stcA->u16IdVendor != 0) && (stcB->u16IdVendor != 0))
    {
       if (stcA->u16IdVendor != stcB->u16IdVendor)
       {
          return FALSE;
       }
    }
    
    if ((stcA->u16IdProduct != 0) && (stcB->u16IdProduct != 0))
    {
       if (stcA->u16IdProduct != stcB->u16IdProduct)
       {
          return FALSE;
       }
    }
    
    if ((stcA->u8MatchDeviceClass != 0) && (stcB->u8MatchDeviceClass != 0))
    {
       if (stcA->u8MatchDeviceClass != stcB->u8MatchDeviceClass)
       {
          return FALSE;
       }
    }
    

    if ((stcA->u8MatchDeviceSubClass != 0) && (stcB->u8MatchDeviceSubClass != 0))
    {
       if (stcA->u8MatchDeviceSubClass != stcB->u8MatchDeviceSubClass)
       {
          return FALSE;
       }
    }
    
    
    if ((stcA->u8MatchDeviceProtocol != 0) && (stcB->u8MatchDeviceProtocol != 0))
    {
       if (stcA->u8MatchDeviceProtocol != stcB->u8MatchDeviceProtocol)
       {
          return FALSE;
       }
    }


    if ((stcA->u8MatchInterfaceClass != 0) && (stcB->u8MatchInterfaceClass != 0))
    {
       if (stcA->u8MatchInterfaceClass != stcB->u8MatchInterfaceClass)
       {
          return FALSE;
       }
    }
    

    if ((stcA->u8MatchInterfaceSubClass != 0) && (stcB->u8MatchInterfaceSubClass != 0))
    {
       if (stcA->u8MatchInterfaceSubClass != stcB->u8MatchInterfaceSubClass)
       {
          return FALSE;
       }
    }
    

    if ((stcA->u8MatchInterfaceProtocol != 0) && (stcB->u8MatchInterfaceProtocol != 0))
    {
       if (stcA->u8MatchInterfaceProtocol != stcB->u8MatchInterfaceProtocol)
       {
          return FALSE;
       }
    }
    return TRUE;
}

#endif /* (USE_USB_HOST == 1) */

