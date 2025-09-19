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
/** \file usbhost.c
 **
 **
 ** History:
 **   - 2012-08-28  2.0  MSc  First version (starting at version 2.0).
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/


#include "usbhost.h"

#if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))

/**
 ******************************************************************************
 ** \ingroup UsbHostGroup
 ******************************************************************************/
//@{

#include "usbclasssupervisor.h"
#if (USE_USBHOSTHW_H == 1)
#include "usbhosthw.h"
#endif


const stc_usb_request_t stcDeviceDescriptorRequest = {0x80,0x06,0x0100,0x00,18};
const stc_usb_request_t stcShortDeviceDescriptorRequest = {0x80,0x06,0x0100,0x00,8};
const stc_usb_request_t stcClearStallEp0 = {0x02,0x01,0x0000,0x00,0x00};


static stc_usbhost_intern_data_t* UsbHostGetInternDataPtr(stc_usbn_t* pstcUsb) ;
static void EnumerationStateMachine(stc_usbn_t* pstcUsb);
static void ConvertRequestToByteArray(uint8_t* pu8Buffer, stc_usb_request_t* pstcSetup);
static void StallCompletionEp0(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint32_t u32Size);
static void UsbHost_SetupComplete(stc_usbn_t* pstcUsb);
static void SchedulerFinishing(stc_usbn_t* pstcUsb,uint8_t u8NextToken,stc_host_endpoint_t* pstcEndpointHandle);




/// Macro to return the number of enabled MFS instances
#define USBHOST_INSTANCE_COUNT (uint32_t)(sizeof(m_astcUsbHostInstanceDataLut) / sizeof(m_astcUsbHostInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled MFS instances and their internal data
stc_usbhostn_instance_data_t m_astcUsbHostInstanceDataLut[] =
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
static stc_usbhost_intern_data_t* UsbHostGetInternDataPtr(stc_usbn_t* pstcUsb) 
{
    volatile uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < USBHOST_INSTANCE_COUNT; u32Instance++)
    {
        if ((uint32_t)pstcUsb == (uint32_t)(m_astcUsbHostInstanceDataLut[u32Instance].pstcInstance))
        {
            return &m_astcUsbHostInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}


/**
 ******************************************************************************
 ** \brief Add new endpoint
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcHandler returns pointer to stc_host_endpoint_t (pointer of pointer)
 **
 ** \param u8EndpointAddress Endpointaddress
 **
 ** \param u16FifoSize Fifosize
 **
 ** \param u8Interval Interval (for interrupt transfers)
 **
 ** \param CompletionHandler Transfer completion callback routine
 **
 ** \return USBHOST_SUCCESS at success
 **
 ******************************************************************************/
uint8_t UsbHost_AddHostEndpoint(stc_usbn_t* pstcUsb, stc_host_endpoint_t** pstcHandler, uint8_t u8EndpointAddress, uint16_t u16FifoSize, uint8_t u8Interval, void(* CompletionHandler)(stc_usbn_t* pstcUsb))
{
	 uint8_t i;
   stc_usbhost_intern_data_t* pstcUsbHostIntern;
   pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
   pstcUsbHostIntern->bSchedulerLock = TRUE;
   pstcUsbHostIntern->bSchedulerLock = TRUE;
   pstcUsbHostIntern->bSchedulerLock = TRUE;
   for(i=0;i<MAX_HOST_ENDPOINTS;i++)
   {
      if ((pstcUsbHostIntern->stc_host_endpoints[i].u16FifoSize == 0) || ((&pstcUsbHostIntern->stc_host_endpoints[i] == *pstcHandler) && (*pstcHandler != 0)))
      {
          if ((&pstcUsbHostIntern->stc_host_endpoints[i] == *pstcHandler) && (*pstcHandler != 0))
          {
             USBDBG("    |->Endpoint exists in list...\n");
          }
          USBDBGVAL8("    |->New EP, Address: ",u8EndpointAddress);
          
          pstcUsbHostIntern->stc_host_endpoints[i].u16FifoSize = u16FifoSize;
          pstcUsbHostIntern->stc_host_endpoints[i].u8Address = u8EndpointAddress;
          pstcUsbHostIntern->stc_host_endpoints[i].u8Interval = u8Interval;
          pstcUsbHostIntern->stc_host_endpoints[i].u8IntervalCount = u8Interval;
          pstcUsbHostIntern->stc_host_endpoints[i].CompletionHandler = CompletionHandler;
          *pstcHandler = &pstcUsbHostIntern->stc_host_endpoints[i];
          USBDBGVAL8("    |->New EP, Index: ",i);
          USBDBGVAL32("   |->New EP, Handler: ",*pstcHandler);
          pstcUsbHostIntern->u8EndpointListEnd = i;
          pstcUsbHostIntern->bSchedulerLock = FALSE;
          pstcUsbHostIntern->bSchedulerLock = FALSE;
          pstcUsbHostIntern->bSchedulerLock = FALSE;
          return USBHOST_SUCCESS;
      }
   }
   pstcUsbHostIntern->bSchedulerLock = FALSE;
   pstcUsbHostIntern->bSchedulerLock = FALSE;
   pstcUsbHostIntern->bSchedulerLock = FALSE;
   return USBHOST_ERROR; 
}

/**
 ******************************************************************************
 ** \brief Get pointzer to host endpoint via endpoint address
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param u8EndpointNumber Endpointaddress
 **
 ** \return Pointer to stc_host_endpoint_t
 **
 ******************************************************************************/
stc_host_endpoint_t* UsbHost_EndpointFromNumber(stc_usbn_t* pstcUsb, uint8_t u8EndpointNumber)
{
   uint8_t i;
   stc_usbhost_intern_data_t* pstcUsbHostIntern;
   pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
   
   for(i=0;i<=pstcUsbHostIntern->u8EndpointListEnd;i++)
   {
       if ((pstcUsbHostIntern->stc_host_endpoints[i].u8Address & 0x0F) == u8EndpointNumber)
       {
           return &pstcUsbHostIntern->stc_host_endpoints[i];
       }
   }
   return NULL;
}

/**
 ******************************************************************************
 ** \brief Enumeration Statemachine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 **
 ******************************************************************************/
static void EnumerationStateMachine(stc_usbn_t* pstcUsb)
{
    uint8_t* pu8SetupBuffer;
    uint32_t u32Size = UsbHost_ReceiveSetupData(pstcUsb, &pu8SetupBuffer);
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    pstcUsbHostIntern->Ep0IN->CompletionHandler = 0;
	
    USBDBG("ENUM STATEMACH\n");
    if (pu8SetupBuffer != 0)
    {
        if (u32Size > 0)
        {
            if (pu8SetupBuffer[1] == 0x02)
            {
                USBDBG("Received Configuration\n");
            }
            if (pu8SetupBuffer[1] == 0x01)
            {
                USBDBG("Enumeration Process:\n");
                USBDBG("  ->Received Device Descriptor\n");
                USBDBGVAL16("  ->New EP0 buffersize: ",(uint16_t)((uint8_t)pu8SetupBuffer[7]));
                pstcUsbHostIntern->Ep0IN->u16FifoSize = (uint16_t)((uint8_t)pu8SetupBuffer[7]);
                pstcUsbHostIntern->Ep0OUT->u16FifoSize = (uint16_t)((uint8_t)pu8SetupBuffer[7]);
            }
        }
        else
        {
            if (pstcUsbHostIntern->u8DeviceStatus == USBHOST_DEVICE_ADDRESSING)
            {
                   pstcUsbHostIntern->u8EnumerationCounter = 0;
                   pstcUsb->HADR = pstcUsbHostIntern->u8DeviceAddress;
                   USBDBG("Enumeration Process:\n");
                   USBDBGVAL8("  ->Addressed:  ",pstcUsbHostIntern->u8DeviceAddress);
                   pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_ADDRESSED;
                   UsbClassSupervisor_Init(pstcUsb);
            }
        }
    }
    else
    {
        if (pstcUsbHostIntern->Ep0IN->u16FifoSize == 20)
        {
            USBDBG("Enumeration Process:\n");
            USBDBG("  -> Endpoint 0 has no Fifo size specified.\n"); 
            USBDBG("     Requesting Device Descriptor.\n");
            UsbHost_SetupRequest(pstcUsb,(stc_usb_request_t *)&stcShortDeviceDescriptorRequest,0);
        }
        else
        {            
            if (pstcUsbHostIntern->u8DeviceStatus == USBHOST_DEVICE_IDLE)
            {
                USBDBG("Enumeration Process:\n");
                pstcUsbHostIntern->Ep0IN->CompletionHandler = EnumerationStateMachine;
                pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_ADDRESSING;
                UsbHost_AddressDevice(pstcUsb, 2);
                pstcUsbHostIntern->u8EnumerationCounter = 0;
            }
        }
    }
    if (pstcUsbHostIntern->u8DeviceStatus != USBHOST_DEVICE_ADDRESSED)
    {
        pstcUsbHostIntern->u8EnumerationCounter++;
        if (pstcUsbHostIntern->u8EnumerationCounter == 40)
        {
            pstcUsbHostIntern->u32ConnectionDelay += 100;
            //UsbHost_DeInit();
            //UsbHost_Init();
        }
        
        if (pstcUsbHostIntern->u8EnumerationCounter > 50)
        {
            /*stc_usb_request_t stcSetup;
            pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_IDLE; 
            USBDBG("Enumeration timed out...\n");
            pstcUsbHostIntern->u8EnumerationCounter = 0;
            USBDBG("ENUMERATION FAILED - SYSTEM HALT\n");
            stcSetup.bmRequestType=0x82;  
            stcSetup.bRequest=0x00;
            stcSetup.wValue=0x00;  
            stcSetup.wIndex=0x00;
            stcSetup.wLength=0x02;
            UsbHost_SetupRequest(pstcUsb, (stc_usb_request_t *)&stcShortDeviceDescriptorRequest,EndpointInStatusUpdate);*/
        }
        else
        {
            Usb_AddTimeOut(pstcUsb, EnumerationStateMachine,100);
        }
    }
}

/**
 ******************************************************************************
 ** \brief Convert stc_usb_request_t to byte array
 **
 ** \param pu8Buffer pointer to byte array
 **
 ** \param pstcSetup Setup request
 **
 ******************************************************************************/
static void ConvertRequestToByteArray(uint8_t* pu8Buffer, stc_usb_request_t* pstcSetup)
{
    pu8Buffer[0] = pstcSetup->bmRequestType;
    pu8Buffer[1] = pstcSetup->bRequest;
    pu8Buffer[2] = (uint8_t)(pstcSetup->wValue & 0xFF);
    pu8Buffer[3] = (uint8_t)((pstcSetup->wValue >> 8) & 0xFF);
    pu8Buffer[4] = (uint8_t)(pstcSetup->wIndex & 0xFF);
    pu8Buffer[5] = (uint8_t)((pstcSetup->wIndex >> 8) & 0xFF);
    pu8Buffer[6] = (uint8_t)(pstcSetup->wLength & 0xFF);
    pu8Buffer[7] = (uint8_t)((pstcSetup->wLength >> 8) & 0xFF);
}

/**
 ******************************************************************************
 ** \brief Address device
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param u8NewAddress New address
 **
 ******************************************************************************/
void UsbHost_AddressDevice(stc_usbn_t* pstcUsb, uint8_t u8NewAddress)
{
    stc_usb_request_t stcSetup = {0x00,0x05,0x02,0x00,0x00};
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    
    USBDBGVAL8("Start Addressing:  ",u8NewAddress);
    stcSetup.wValue = u8NewAddress;
    pstcUsbHostIntern->u8DeviceAddress = u8NewAddress;  // Device Address = NewAddress
    pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_ADDRESSING;
    UsbHost_SetupRequest(pstcUsb, &stcSetup,NULL);
}

/**
 ******************************************************************************
 ** \brief Receive data of setup request
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pu8Buffer pointer returned
 **
 ** \return length
 **
 ******************************************************************************/
uint32_t UsbHost_ReceiveSetupData(stc_usbn_t* pstcUsb, uint8_t** pu8Buffer)
{
    uint32_t u32Size;
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    
    if (pstcUsbHostIntern->Ep0IN->u8Status & USBHOST_ENDPOINTSTATUS_DATA)
    {
        pstcUsbHostIntern->Ep0IN->u8Status &= ~USBHOST_ENDPOINTSTATUS_DATA;
        *pu8Buffer = pstcUsbHostIntern->Ep0IN->pu8Buffer;
        u32Size = (pstcUsbHostIntern->Ep0IN->u32DataSize);
        return u32Size;
    }
    *pu8Buffer = 0;
    return 0;
}

/**
 ******************************************************************************
 ** \brief Add data package to setup package
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcSetup setup request
 **
 ** \param pu8Data data to send
 **
 ** \param SetupCompletion completion callback routine
 **
 ******************************************************************************/
void UsbHost_SetupRequestWithData(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup, uint8_t* pu8Data, void(*SetupCompletion)(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint32_t u32DataSize))
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    pstcUsbHostIntern->pu8SetupData = pu8Data;
    UsbHost_SetupRequest(pstcUsb, pstcSetup,SetupCompletion);
}

/**
 ******************************************************************************
 ** \brief Send setup package
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcSetup setup request
 **
 ** \param SetupCompletion completion callback routine
 **
 ******************************************************************************/
void UsbHost_SetupRequest(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup, void(*SetupCompletion)(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint32_t u32DataSize))
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    pstcUsbHostIntern->bSchedulerLock = TRUE;
    pstcUsbHostIntern->SetupCompletionHandler = SetupCompletion;
    if ((((pstcUsbHostIntern->Ep0IN->u8Status) & USBHOST_ENDPOINTSTATUS_STALL) == 0) && (((pstcUsbHostIntern->Ep0OUT->u8Status) & USBHOST_ENDPOINTSTATUS_STALL) == 0))
    {
        memcpy((uint8_t*)&pstcUsbHostIntern->stcLastSetup,(uint8_t*)pstcSetup,sizeof(pstcUsbHostIntern->stcLastSetup));
    }
    ConvertRequestToByteArray(pstcUsbHostIntern->pu8Setup,pstcSetup);
    USBDBGVAL8("Setup [0]",pstcUsbHostIntern->pu8Setup[0]);
    USBDBGVAL8("[1]",pstcUsbHostIntern->pu8Setup[1]);
    USBDBGVAL8("[2]",pstcUsbHostIntern->pu8Setup[2]);
    USBDBGVAL8("[3]",pstcUsbHostIntern->pu8Setup[3]);
    USBDBGVAL8("[4]",pstcUsbHostIntern->pu8Setup[4]);
    USBDBGVAL8("[5]",pstcUsbHostIntern->pu8Setup[5]);
    USBDBGVAL8("[6]",pstcUsbHostIntern->pu8Setup[6]);
    USBDBGVAL8("[7]",pstcUsbHostIntern->pu8Setup[7]);

    UsbHost_TransferDataToFifo(pstcUsb, pstcUsbHostIntern->pu8Setup,8,pstcUsbHostIntern->Ep0OUT->u16FifoSize);
    
    pstcUsbHostIntern->Ep0OUT->u8Stage = USBHOST_STAGE_SETUP;
    pstcUsbHostIntern->Ep0IN->u8Stage = USBHOST_STAGE_SETUP;
    
    pstcUsbHostIntern->Ep0IN->pu8BufferPos = pstcUsbHostIntern->Ep0IN->pu8Buffer;
    pstcUsbHostIntern->Ep0IN->pu8BufferPosNextPackage = pstcUsbHostIntern->Ep0IN->pu8Buffer;
    pstcUsbHostIntern->Ep0IN->u8Status &= ~USBHOST_ENDPOINTSTATUS_DATA;
    pstcUsbHostIntern->Ep0OUT->bToggle = FALSE;
    pstcUsbHostIntern->Ep0OUT->u32DataSize = 8;
    pstcUsbHostIntern->Ep0OUT->pu8Buffer = pstcUsbHostIntern->pu8Setup;
    pstcUsbHostIntern->Ep0OUT->pu8BufferPos = pstcUsbHostIntern->pu8Setup;
    pstcUsbHostIntern->Ep0OUT->u32BufferSize = 8;
    pstcUsbHostIntern->Ep0IN->CompletionHandler = UsbHost_SetupComplete;
    pstcUsbHostIntern->Ep0OUT->CompletionHandler = UsbHost_SetupComplete;
    

    pstcUsbHostIntern->Ep0OUT->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
    pstcUsbHostIntern->Ep0OUT->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
    pstcUsbHostIntern->Ep0IN->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
    pstcUsbHostIntern->Ep0IN->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
    pstcUsbHostIntern->u8SetupDelay = 3;
    pstcUsbHostIntern->ActiveEndpoint=pstcUsbHostIntern->Ep0OUT;
    //UsbHost_TransferData(Ep0OUT,u8Setup,8,SetupComplete);
    
    // synchronous token (sent after next SOF)
    
    pstcUsbHostIntern->Ep0OUT->u8LastToken = CREATE_TOKEN(pstcUsbHostIntern->Ep0OUT->bToggle,HTOKEN_SETUP,pstcUsbHostIntern->Ep0OUT->u8Address & 0x0F);  
    pstcUsbHostIntern->ActiveEndpoint = pstcUsbHostIntern->Ep0OUT; 
    pstcUsbHostIntern->u8LastToken = pstcUsbHostIntern->Ep0OUT->u8LastToken; 
    pstcUsbHostIntern->u8SyncToken = pstcUsbHostIntern->Ep0OUT->u8LastToken;
                  
    pstcUsbHostIntern->bSchedulerLock = FALSE;
}

/**
 ******************************************************************************
 ** \brief Stall completion routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pu8Buffer data returned
 **
 ** \param u32Size size of data
 **
 ******************************************************************************/
static void StallCompletionEp0(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint32_t u32Size)
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    USBDBG("Resending last Setup\n");
    UsbHost_SetupRequest(pstcUsb, &pstcUsbHostIntern->stcLastSetupBeforeStall, pstcUsbHostIntern->SetupCompletionHandler);
}

/**
 ******************************************************************************
 ** \brief Stall completion routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pu8Buffer data returned
 **
 ** \param u32Size size of data
 **
 ******************************************************************************/
static void UsbHost_SetupComplete(stc_usbn_t* pstcUsb)
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    if (pstcUsbHostIntern->SetupCompletionHandler != 0)
    {
        pstcUsbHostIntern->SetupCompletionHandler(pstcUsb,(pstcUsbHostIntern->Ep0IN->pu8Buffer),(pstcUsbHostIntern->Ep0IN->u32DataSize));
    }
}

/**
 ******************************************************************************
 ** \brief Connection Callback Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
void ConnectionCallback(stc_usbn_t* pstcUsb)
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    if (pstcUsbHostIntern->u32ConTimeout == 0)
    {
        if ((pstcUsb->HIRQ_f.URIRQ == 0) || (pstcUsbHostIntern->bBusResetSend == FALSE)) 
        {
            pstcUsb->UDCC_f.RST = 1;
            if (pstcUsb->HSTATE_f.TMODE == 1)  /* TMODE : Full-Speed */
            { 
                USBDBG("Full-Speed Device\n");
                pstcUsb->HSTATE_f.ALIVE = 0;
                pstcUsb->HSTATE_f.CLKSEL = 1;
                pstcUsbHostIntern->bFullSpeed = TRUE;
            }
            else /* Low-Speed */
            {       
                USBDBG("Low-Speed Device\n"); 
                pstcUsbHostIntern->bFullSpeed = FALSE;
                pstcUsb->HSTATE_f.ALIVE = 1;
                pstcUsb->HSTATE_f.CLKSEL = 0;
            }
            pstcUsb->UDCC_f.RST = 0;
        }
        USBDBG("\n### Device Connection - wait...\n");
        if (pstcUsb->HSTATE_f.TMODE == 1)  /* TMODE : Full-Speed */
        {
            pstcUsbHostIntern->u32ConTimeout = pstcUsbHostIntern->u32ConnectionDelay;
        }
        else
        {
            pstcUsbHostIntern->u32ConTimeout = pstcUsbHostIntern->u32ConnectionDelay;
        }
        return;
    }
    if (pstcUsbHostIntern->u32ConTimeout > 1)
    {
        pstcUsbHostIntern->u32ConTimeout = pstcUsbHostIntern->u32ConTimeout - 1;
        return;
    }
    USBDBG("\n### Device Connection...\n");
    pstcUsb->HIRQ_f.CNNIRQ = 0;
    if ((pstcUsb->HIRQ_f.URIRQ == 0) || (pstcUsbHostIntern->bBusResetSend == FALSE))
    {
        pstcUsbHostIntern->bBusResetSend = TRUE;          
        if (pstcUsb->HSTATE_f.CSTAT == 1)
        {
            USBDBG(">> Exec Bus Reset...\n");
            
            pstcUsb->HADR = 0;  // Device Address = 0
            
            pstcUsb->EP1S_f.BFINI = 1;
            pstcUsb->EP1S_f.BFINI = 0;
            pstcUsb->EP2S_f.BFINI = 1;
            pstcUsb->EP2S_f.BFINI = 0;
            
            pstcUsb->HCNT_f.URST = 1; // initiate bus reset
        }
    }
    else
    {
        USBDBG("    |->Bus Resetted\n");
        pstcUsb->HIRQ_f.URIRQ = 0;
        
        UsbHost_AddHostEndpoint(pstcUsb,&pstcUsbHostIntern->Ep0IN ,USBHOST_EPADDR0_IN,20,0,0);
        UsbHost_AddHostEndpoint(pstcUsb,&pstcUsbHostIntern->Ep0OUT,USBHOST_EPADDR0_OUT,20,0,0);
        pstcUsbHostIntern->Ep0IN->u32BufferSize = 255;
        pstcUsbHostIntern->Ep0OUT->u32BufferSize = 255;

        pstcUsbHostIntern->Ep0IN->pu8Buffer  = pstcUsbHostIntern->pu8GlobalBuffer;
        pstcUsbHostIntern->Ep0OUT->pu8Buffer = pstcUsbHostIntern->pu8GlobalBuffer;

        USBDBG(">> Starting SOF\n");
        pstcUsb->HFRAME = 0;
        pstcUsb->HEOF = 0x2c9; // Set the time where token are allowed in a frame
    
        pstcUsb->HCNT_f.SOFSTEP = 1; 
    
        pstcUsbHostIntern->Ep0OUT->bToggle = FALSE;
    
        
        pstcUsb->HTOKEN = CREATE_TOKEN(0,HTOKEN_SOF,0);
        /*SET_TOKEN_NOW(,HTOKEN_SOF);*/

        pstcUsbHostIntern->u8EnumerationCounter = 0;
        Usb_AddTimeOut(pstcUsb, EnumerationStateMachine,500);
    }
}

/**
 ******************************************************************************
 ** \brief Clear all endpoints
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
void UsbHost_ClearAllEndpoints(stc_usbn_t* pstcUsb)
{
   uint8_t i;
   stc_usbhost_intern_data_t* pstcUsbHostIntern;
   pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
   pstcUsbHostIntern->u8EndpointListEnd = 0;
   for(i=0;i<MAX_HOST_ENDPOINTS;i++)
   {
       memset((uint8_t*)&pstcUsbHostIntern->stc_host_endpoints[i],0,sizeof(pstcUsbHostIntern->stc_host_endpoints[i]));
   }
}

/**
 ******************************************************************************
 ** \brief Disconnection Callback Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
void UsbHost_DisconnectionCallback(stc_usbn_t* pstcUsb)
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    USBDBG("\n### Device Disconnection...\n");
    pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_IDLE; 
    pstcUsbHostIntern->u32BusResetTimeout = 0;
    pstcUsbHostIntern->u32ConnectionDelay = 1000;
    pstcUsbHostIntern->u32ConTimeout = 0;
    pstcUsbHostIntern->bBusResetSend = FALSE;
    UsbClassSupervisor_Deinit(pstcUsb);
    UsbHost_ClearAllEndpoints(pstcUsb);
}

/**
 ******************************************************************************
 ** \brief Completion Callback Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
void UsbHost_CompletionCallback(stc_usbn_t* pstcUsb)
{
    uint8_t u8NextToken = 0;
    stc_host_endpoint_t* pstcEndpointHandle = 0;
    uint32_t u32DataSize = 0;
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    
    pstcUsbHostIntern->u8SchedulerState = USBHOST_SCHEDULER_IDLE;
    pstcEndpointHandle = pstcUsbHostIntern->ActiveEndpoint;
    
    USBDBGVAL8("EP: ",pstcEndpointHandle->u8Address);
    
        
    if (pstcEndpointHandle == 0)
    {
      USBDBG("WARNING: pstcEndpointHandle == 0\n");
      return;
    }
    
    USBDBGVAL8("HERR: ",pstcUsb->HERR);
    USBDBGVAL8("TKN: ",(pstcEndpointHandle->u8LastToken));
    if ((pstcUsb->HERR & 0x03) == HERR_ACK)
    {
       USBDBG("ACK\n");
       pstcEndpointHandle->bToggle = ~(pstcEndpointHandle->bToggle);
       
       if (((pstcEndpointHandle->u8Address) & 0x0F) == 0) // handler == endpoint 0?
       {
           if (((pstcEndpointHandle->u8LastToken) & 0x70) == HTOKEN_SETUP) // type == SETUP token?
           {
               if (((pstcUsbHostIntern->stcLastSetup.bmRequestType) & 0x80) || (pstcUsbHostIntern->stcLastSetup.wLength == 0))
               {
                   pstcEndpointHandle = pstcUsbHostIntern->Ep0IN;
               }
               pstcEndpointHandle->bToggle = TRUE;
               pstcEndpointHandle->pu8BufferPos = (pstcEndpointHandle->pu8Buffer);
               pstcEndpointHandle->u32DataSize = 0;
               pstcUsbHostIntern->ActiveEndpoint = pstcEndpointHandle;
               if ((pstcUsbHostIntern->stcLastSetup.wLength == 0)) // || ((stcLastSetup.wLength <= (Ep0IN->u16FifoSize)) && (Handler == Ep0IN)))
               {
                   pstcUsbHostIntern->Ep0OUT->u8Stage = USBHOST_STAGE_STATUS;
                   pstcUsbHostIntern->Ep0IN->u8Stage = USBHOST_STAGE_STATUS;
               }
               else
               { 
                   pstcUsbHostIntern->Ep0OUT->u8Stage = USBHOST_STAGE_DATA;
                   pstcUsbHostIntern->Ep0IN->u8Stage = USBHOST_STAGE_DATA;
               }
           
               pstcEndpointHandle->u32DataSize = pstcUsbHostIntern->stcLastSetup.wLength;
               if (((pstcUsbHostIntern->stcLastSetup.bmRequestType) & 0x80) || (pstcUsbHostIntern->stcLastSetup.wLength == 0))
               {
                   USBDBG("IT\n");
                   u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_IN,((pstcEndpointHandle->u8Address) & 0x0F));
               }
               else
               {
                   USBDBG("OT\n");
                   pstcEndpointHandle->pu8BufferPosNextPackage = pstcUsbHostIntern->pu8SetupData;
                   pstcEndpointHandle->pu8BufferPos = pstcUsbHostIntern->pu8SetupData;
                   pstcEndpointHandle->pu8Buffer = pstcUsbHostIntern->pu8SetupData;
                   pstcEndpointHandle->u32BufferSize = pstcUsbHostIntern->stcLastSetup.wLength;
                   pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
                   pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
                   
                   u32DataSize = (pstcEndpointHandle->u32DataSize);
                   u32DataSize -= (uint32_t)((pstcEndpointHandle->pu8BufferPos) - (pstcEndpointHandle->pu8Buffer));
                   if (u32DataSize > (pstcEndpointHandle->u16FifoSize))
                   {
                       u32DataSize = pstcEndpointHandle->u16FifoSize;
                   }
                  if (u32DataSize > 0)
                  {
                       USBDBGVAL32("DPKG l=",u32DataSize);
                       pstcEndpointHandle->pu8BufferPosNextPackage += u32DataSize;
                       
                       UsbHost_TransferDataToFifo(pstcUsb,pstcEndpointHandle->pu8BufferPos, (uint16_t)u32DataSize, pstcEndpointHandle->u16FifoSize);
                  } 
                  else
                  {
                       UsbHost_TransferDataToFifo(pstcUsb,pstcEndpointHandle->pu8BufferPos, (uint16_t)u32DataSize, pstcEndpointHandle->u16FifoSize);
                  }
                  pstcEndpointHandle = pstcUsbHostIntern->Ep0OUT;
                  pstcEndpointHandle->u8LastToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_OUT,pstcEndpointHandle->u8Address & 0x0F);  
                  pstcUsbHostIntern->ActiveEndpoint = pstcEndpointHandle; 
                  pstcUsbHostIntern->u8LastToken = pstcEndpointHandle->u8LastToken; 
                  pstcUsbHostIntern->u8SyncToken = pstcEndpointHandle->u8LastToken;
                   
               }
           } /* END: type == SETUP token? */
           else
           {
               if (pstcUsbHostIntern->u8DeviceStatus == USBHOST_DEVICE_ADDRESSING)
               {
                   pstcUsbHostIntern->u8EnumerationCounter = 0;
                   pstcUsb->HADR = pstcUsbHostIntern->u8DeviceAddress;
                   USBDBG("Enumeration Process:\n");
                   USBDBGVAL8("  ->Addressed:  ",pstcUsbHostIntern->u8DeviceAddress);
                   pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_ADDRESSED;
                   UsbClassSupervisor_Init(pstcUsb);
               }
               if (pstcUsbHostIntern->u8DeviceStatus == USBHOST_DEVICE_CONFIGURATING)
               {
                   USBDBG("-->Configured\n");
                   pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_CONFIGURED;
               }
           }
       } /* END: handler == endpoint 0? */
       
       if ((u8NextToken == 0) && (pstcUsbHostIntern->u8SyncToken == 0))
       {
           pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
           if (((pstcEndpointHandle->u8LastToken) & 0x70) == HTOKEN_IN)
           {
               uint32_t tmp;
               u32DataSize = (uint32_t)((uint16_t)UsbHost_TransferFifoToBuffer(pstcUsb,pstcEndpointHandle->pu8BufferPos));
               
               (pstcEndpointHandle->pu8BufferPos) = (pstcEndpointHandle->pu8BufferPos) + u32DataSize;
               tmp = ((uint32_t)((uint32_t)(pstcEndpointHandle->pu8BufferPos) - (uint32_t)(pstcEndpointHandle->pu8Buffer)));
               USBDBGVAL8("EP#",(uint32_t)(pstcEndpointHandle->u8Address));
               USBDBGVAL32("#",(uint32_t)(pstcEndpointHandle->pu8Buffer));
               USBDBGVAL32("DSz ", u32DataSize);
               USBDBGVAL32("Sz ", tmp);
               USBDBGVAL32("mx: ",(pstcEndpointHandle->u32DataSize));
               if (((u32DataSize == (pstcEndpointHandle->u16FifoSize)) && ((pstcEndpointHandle->u32DataSize) == 0)) || (((pstcEndpointHandle->u32DataSize) > 0) && ((pstcEndpointHandle->u32DataSize) > tmp)))
               {
                   USBDBG("NXT\n");
                   u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_IN,((pstcEndpointHandle->u8Address) & 0x0F));
               }
               else
               {
                   pstcEndpointHandle->u32DataSize = (uint32_t)((uint32_t)(pstcEndpointHandle->pu8BufferPos) - (uint32_t)(pstcEndpointHandle->pu8Buffer));
                   USBDBGVAL32("RX-Size: ", (pstcEndpointHandle->u32DataSize));
                   pstcEndpointHandle->pu8BufferPos = (pstcEndpointHandle->pu8Buffer);
                   pstcEndpointHandle->pu8BufferPosNextPackage = (pstcEndpointHandle->pu8Buffer);
                   pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_DATA;
                   pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
                   
                   if ((((pstcEndpointHandle->u8Address) & 0x0F) == 0) && ((pstcEndpointHandle->u8Stage) == USBHOST_STAGE_DATA))
                   {
                       pstcUsbHostIntern->Ep0IN->u8Stage = USBHOST_STAGE_STATUS;
                       pstcUsbHostIntern->Ep0OUT->u8Stage = USBHOST_STAGE_STATUS;
                       pstcUsbHostIntern->Ep0OUT->bToggle = TRUE;
                       pstcUsbHostIntern->Ep0OUT->u32DataSize = 0;
                       pstcUsbHostIntern->Ep0OUT->pu8BufferPos = (pstcUsbHostIntern->Ep0OUT->pu8Buffer);
                       pstcUsbHostIntern->Ep0OUT->pu8BufferPosNextPackage = (pstcUsbHostIntern->Ep0OUT->pu8BufferPos);
                       pstcUsbHostIntern->Ep0OUT->CompletionHandler = pstcUsbHostIntern->Ep0IN->CompletionHandler;
                       UsbHost_TransferDataToFifo(pstcUsb,pstcEndpointHandle->pu8BufferPos, (uint16_t)0, pstcEndpointHandle->u16FifoSize);

                       pstcUsbHostIntern->Ep0OUT->u8LastToken = CREATE_TOKEN(pstcUsbHostIntern->Ep0OUT->bToggle,HTOKEN_OUT,pstcUsbHostIntern->Ep0OUT->u8Address & 0x0F);  
                       pstcUsbHostIntern->ActiveEndpoint = pstcUsbHostIntern->Ep0OUT; 
                       pstcUsbHostIntern->u8LastToken = pstcUsbHostIntern->Ep0OUT->u8LastToken; 
                       pstcUsbHostIntern->u8SyncToken = pstcUsbHostIntern->Ep0OUT->u8LastToken;
                   }
                   else
                   {
                       if ((pstcEndpointHandle->CompletionHandler) != 0)
                       {
                           USBDBG("Exec(IN)\n");
                           pstcEndpointHandle->CompletionHandler(pstcUsb);
                       }
                       pstcUsbHostIntern->ActiveEndpoint = 0;
                   }
               }
           }
           else // Sending Data
           {
               pstcEndpointHandle->pu8BufferPos = (pstcEndpointHandle->pu8BufferPosNextPackage);
               u32DataSize = (pstcEndpointHandle->u32DataSize);
               u32DataSize -= (uint32_t)((pstcEndpointHandle->pu8BufferPos) - (pstcEndpointHandle->pu8Buffer));
               if (u32DataSize > (pstcEndpointHandle->u16FifoSize))
               {
                   u32DataSize = pstcEndpointHandle->u16FifoSize;
               }
               if (u32DataSize > 0)
               {
                   pstcEndpointHandle->pu8BufferPosNextPackage += u32DataSize;
                   
                   UsbHost_TransferDataToFifo(pstcUsb,pstcEndpointHandle->pu8BufferPos, (uint16_t)u32DataSize, pstcEndpointHandle->u16FifoSize);
                   u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_OUT,((pstcEndpointHandle->u8Address) & 0x0F));
               }
               else
               {
                   u8NextToken = 0;
                   pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
                   if ((((pstcEndpointHandle->u8Address) & 0x0F) == 0) && ((pstcEndpointHandle->u8Stage) == USBHOST_STAGE_DATA))
                   {
                       pstcUsbHostIntern->Ep0IN->u8Stage = USBHOST_STAGE_STATUS;
                       pstcUsbHostIntern->Ep0OUT->u8Stage = USBHOST_STAGE_STATUS;
                       pstcUsbHostIntern->Ep0IN->CompletionHandler = pstcUsbHostIntern->Ep0OUT->CompletionHandler;
                       pstcUsbHostIntern->Ep0IN->bToggle = TRUE;
                       
                       // synchronous token (sent after next SOF)
                       pstcUsbHostIntern->Ep0IN->u8LastToken = CREATE_TOKEN(pstcUsbHostIntern->Ep0IN->bToggle,HTOKEN_IN,pstcUsbHostIntern->Ep0IN->u8Address & 0x0F);  
                       pstcUsbHostIntern->ActiveEndpoint = pstcUsbHostIntern->Ep0IN; 
                       pstcUsbHostIntern->u8LastToken = pstcUsbHostIntern->Ep0IN->u8LastToken; 
                       pstcUsbHostIntern->u8SyncToken = pstcUsbHostIntern->Ep0IN->u8LastToken;
                       
                        
                       

                       //u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_OUT,((pstcEndpointHandle->u8Address) & 0x0F));
                   }
                   else
                   {
                       if (pstcEndpointHandle->u8Interval != 0)
                       {
                           pstcEndpointHandle->u32DataSize = 0;
                       }
                       if ((pstcEndpointHandle->CompletionHandler) != 0)
                       {
                           pstcEndpointHandle->CompletionHandler(pstcUsb);
                       }
                   }
                   pstcUsbHostIntern->ActiveEndpoint = 0;
               }
    
           }
       } /* END: ((u8NextToken == 0) && (u8SyncToken == 0)) */
    } /* END: ((HERR & 0x03) == HERR_ACK) */
    
    if ((pstcUsb->HERR & 0x08) > 0)
    {
       USBDBG("Toggle Error\n");
       pstcUsb->HERR_f.TGERR = 0;
       //pstcEndpointHandle->bToggle = ~pstcEndpointHandle->bToggle;
    }
    if ((pstcUsb->HERR & 0x03) == HERR_NAK)
    {
       USBDBG("NAK\n");
       if (((pstcEndpointHandle->u8Interval) > 0) && ((pstcEndpointHandle->pu8Buffer) == (pstcEndpointHandle->pu8BufferPos)))
       {
           USBDBG("Cancel\n");
           pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
           pstcEndpointHandle = 0;
           pstcUsbHostIntern->ActiveEndpoint = 0;
       }
       else
       {   
           USBDBG("RTY\n");
           pstcEndpointHandle = pstcUsbHostIntern->ActiveEndpoint;
    
           if (pstcEndpointHandle->bAbortTransfer == TRUE)
           {
               USBDBG("-> CANCELED\n");
               pstcEndpointHandle->u32DataSize = 0;
               pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
               pstcEndpointHandle->bAbortTransfer = FALSE;
               //pstcEndpointHandle->bToggle = ~pstcEndpointHandle->bToggle;
               pstcUsbHostIntern->ActiveEndpoint = 0;
               return;
               /*if ((pstcEndpointHandle->CompletionHandler) != 0)
               {
                  pstcEndpointHandle->CompletionHandler();
               }
               Handler = 0;*/
               
           }
           else
           {
               //u8NextToken = pstcEndpointHandle->u8LastToken;
               pstcUsbHostIntern->u8SyncToken = pstcEndpointHandle->u8LastToken;
               pstcEndpointHandle = 0;
           }
       }
    }
    if ((pstcUsb->HERR & 0x03) == HERR_NULL)
    {
       USBDBG("NULL\n");
       USBDBGVAL8("HERR: ",pstcUsb->HERR);
    }
    if ((pstcUsb->HERR & 0x03) == HERR_STALL)
    {
       USBDBG("STALL\n");
       pstcEndpointHandle = pstcUsbHostIntern->ActiveEndpoint;
       pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_STALL;
       if (((pstcEndpointHandle->u8Address) & 0x0F) == 0)
       {
           USBDBG("Clearing STALL EP0\n");
           pstcUsbHostIntern->StallCompletionHandler = pstcEndpointHandle->CompletionHandler;
           memcpy((uint8_t*)&pstcUsbHostIntern->stcLastSetupBeforeStall,(uint8_t*)&pstcUsbHostIntern->stcLastSetup,sizeof(pstcUsbHostIntern->stcLastSetup));
           UsbHost_SetupRequest(pstcUsb, (stc_usb_request_t*)&stcClearStallEp0,StallCompletionEp0);
       }
       else
       {
           if ((pstcEndpointHandle->CompletionHandler) != 0)
           {
               pstcEndpointHandle->CompletionHandler(pstcUsb);
           }
       }
    }
    SchedulerFinishing(pstcUsb,u8NextToken,pstcEndpointHandle);
}


/**
 ******************************************************************************
 ** \brief Start Of Frame Callback Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
void UsbHost_SofCallback(stc_usbn_t* pstcUsb)
{
     uint8_t i;
     uint8_t u8Priority = 255;
     uint8_t u8NextToken = 0;
     stc_host_endpoint_t* pstcEndpointHandle = 0;
     stc_usbhost_intern_data_t* pstcUsbHostIntern;
     pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
     if (pstcUsbHostIntern->bSchedulerLock == TRUE)
     {
         USBDBG("SOF LOCK!\n");
         return;
     }
     
     if (pstcUsbHostIntern->u8SetupDelay > 0)
     {
         pstcUsbHostIntern->u8SetupDelay--;
         USBDBG("SETUP WAIT\n");
     }
     else
     {
         if (pstcUsbHostIntern->u8SyncToken != 0)
         {
            if ((pstcUsbHostIntern->u8SyncToken & 0x0F) != 0)
            {
               pstcEndpointHandle = UsbHost_EndpointFromNumber(pstcUsb,pstcUsbHostIntern->u8SyncToken & 0x0F);
            }
            else
            {
               if ((pstcUsbHostIntern->u8SyncToken & 0x70) == HTOKEN_IN)
               {
                   pstcEndpointHandle = pstcUsbHostIntern->Ep0IN;
               }
               else
               {
                   pstcEndpointHandle = pstcUsbHostIntern->Ep0OUT;
               }
            }
            pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
            u8NextToken = pstcUsbHostIntern->u8SyncToken;
            pstcUsbHostIntern->u8SyncToken = 0;
            u8Priority = 0;
        }
    }
    pstcUsbHostIntern->bSofToken = TRUE;
    if ((u8NextToken == 0) && (pstcEndpointHandle == 0))
    {
       for(i=0;i<=pstcUsbHostIntern->u8EndpointListEnd;i++)
       {
           if (pstcUsbHostIntern->stc_host_endpoints[i].u8Interval != 0)
           {
               if (pstcUsbHostIntern->stc_host_endpoints[i].u8IntervalCount == 0)
               {
                   if (pstcUsbHostIntern->stc_host_endpoints[i].u8Interval < u8Priority)
                   {
                       u8Priority = pstcUsbHostIntern->stc_host_endpoints[i].u8Interval;
                       if (pstcUsbHostIntern->ActiveEndpoint == 0)
                       {
                           pstcEndpointHandle = &pstcUsbHostIntern->stc_host_endpoints[i];
                       }
                   }
               }
               else
               {
                   pstcUsbHostIntern->stc_host_endpoints[i].u8IntervalCount--;
               }
           }
       }
    }
    if (pstcEndpointHandle == 0)
    {
       if ((pstcUsbHostIntern->stc_host_endpoints[pstcUsbHostIntern->u8CurrentEndpointPosition].u8Interval) == 0)
       {
           pstcEndpointHandle = &pstcUsbHostIntern->stc_host_endpoints[pstcUsbHostIntern->u8CurrentEndpointPosition];
       }
    } 
    else
    {
       if (!((pstcEndpointHandle->u8Status) & USBHOST_ENDPOINTSTATUS_INUSE))
       {
           if (((pstcEndpointHandle->u8Address) & 0x80))
           {
               pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_INITIATE;
           }
           else
           {
               if (((pstcEndpointHandle->u32DataSize) != 0) && ((pstcEndpointHandle->pu8BufferPos) == (pstcEndpointHandle->pu8Buffer)))
               {
                   pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_INITIATE;
               }
           }
       }
    }
    SchedulerFinishing(pstcUsb,u8NextToken,pstcEndpointHandle);
}

/**
 ******************************************************************************
 ** \brief Scheduler Finishing Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param u8NextToken Planned Token
 **
 ** \param pstcEndpointHandle Planned Endpoint
 **
 ******************************************************************************/
static void SchedulerFinishing(stc_usbn_t* pstcUsb,uint8_t u8NextToken,stc_host_endpoint_t* pstcEndpointHandle)
{
	 uint32_t u32DataSize = 0;
   stc_usbhost_intern_data_t* pstcUsbHostIntern;
   pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
   if ((pstcEndpointHandle != 0) && (pstcEndpointHandle->bAbortTransfer == TRUE))
   {
       USBDBG("-> CANCELED\n");
       pstcEndpointHandle->u32DataSize = 0;
       pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
       pstcEndpointHandle->bAbortTransfer = FALSE;
       pstcUsbHostIntern->ActiveEndpoint = 0;
       return; 
   }
   if ((pstcEndpointHandle != 0) && ((pstcEndpointHandle->u8Status) & USBHOST_ENDPOINTSTATUS_INITIATE) && (u8NextToken == 0) && (pstcUsbHostIntern->u8SchedulerState == USBHOST_SCHEDULER_IDLE))
   {
       USBDBGVAL8("Init Token, Endpoint: ", pstcEndpointHandle->u8Address);
       pstcEndpointHandle->pu8BufferPos = (pstcEndpointHandle->pu8Buffer);
       pstcEndpointHandle->pu8BufferPosNextPackage = (pstcEndpointHandle->pu8Buffer);
       pstcEndpointHandle->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
       pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
       pstcEndpointHandle->u8IntervalCount = (pstcEndpointHandle->u8Interval);
       if ((pstcEndpointHandle->u8Address) & 0x80)
       {
           //pstcEndpointHandle->u32DataSize = 0;
           u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_IN,((pstcEndpointHandle->u8Address) & 0x0F));
           //SET_TOKEN(Handler,HTOKEN_IN);
       }
       else
       {
           u32DataSize = (pstcEndpointHandle->u32DataSize);
           if (u32DataSize > (pstcEndpointHandle->u16FifoSize))
           {
               u32DataSize = pstcEndpointHandle->u16FifoSize;
           }
           if (u32DataSize > 0)
           {
               pstcEndpointHandle->pu8BufferPosNextPackage += u32DataSize;
               
               UsbHost_TransferDataToFifo(pstcUsb,pstcEndpointHandle->pu8BufferPos, (uint16_t)u32DataSize, pstcEndpointHandle->u16FifoSize);
               u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_OUT,((pstcEndpointHandle->u8Address) & 0x0F));

           }
           else
           {
               USBDBG("Sending 0 byte\n");
               UsbHost_TransferDataToFifo(pstcUsb,pstcEndpointHandle->pu8BufferPos, (uint16_t)u32DataSize, pstcEndpointHandle->u16FifoSize);
               u8NextToken = CREATE_TOKEN(pstcEndpointHandle->bToggle,HTOKEN_OUT,((pstcEndpointHandle->u8Address) & 0x0F));
           }
       }
   }
   
   pstcUsbHostIntern->u8CurrentEndpointPosition++;
   if (pstcUsbHostIntern->u8CurrentEndpointPosition > pstcUsbHostIntern->u8EndpointListEnd) 
   {
       pstcUsbHostIntern->u8CurrentEndpointPosition = 0;
   }
   
   if ((u8NextToken != 0) && (pstcEndpointHandle != 0))
   {
       USBDBG("<T>");
       USBDBGVAL8("",u8NextToken);
       USBDBGVAL8("DATA",(u8NextToken & 0x80) > 0);
       pstcUsbHostIntern->bSofToken = FALSE;
       pstcUsbHostIntern->u8LastToken = u8NextToken;
       pstcUsbHostIntern->ActiveEndpoint = pstcEndpointHandle;
       pstcEndpointHandle->u8LastToken = u8NextToken;
       pstcUsbHostIntern->u8SchedulerState = USBHOST_SCHEDULER_BUSY;
       pstcEndpointHandle->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
       pstcUsb->HTOKEN = u8NextToken;
   }
}


/**
 ******************************************************************************
 ** \brief Bus Reset Callback Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
void UsbHost_BusresetCallback(stc_usbn_t* pstcUsb)
{

}

/**
 ******************************************************************************
 ** \brief Host Init Callback Routine
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param  pstcConfig USB Config
 **
 ******************************************************************************/
void UsbHost_InitCallback(stc_usbn_t* pstcUsb,struct stc_usb_config* pstcConfig)
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    pstcUsbHostIntern->bSofToken = FALSE;
    pstcUsbHostIntern->u32ConTimeout = 0;
    pstcUsbHostIntern->u32BusResetTimeout = 0;
    pstcUsbHostIntern->u8CurrentEndpointPosition = 2;
    pstcUsbHostIntern->u8EndpointListEnd = 0;
    pstcUsbHostIntern->bSchedulerLock = FALSE;
    pstcUsbHostIntern->bBusResetSend = FALSE;
    //pstcUsbHostIntern->SetupCompletionHandler = 0;
    pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_IDLE;
    pstcUsbHostIntern->u8DeviceAddress = 0;
    pstcUsbHostIntern->u8EnumerationCounter = 0;
    pstcConfig->pfnCnnirqCallback = ConnectionCallback;
    pstcConfig->pfnDirqCallback = UsbHost_DisconnectionCallback;
    pstcConfig->pfnCmpirqCallback = UsbHost_CompletionCallback;
    pstcConfig->pfnSofCallback = UsbHost_SofCallback;
    pstcUsbHostIntern->u32ConnectionDelay = 1000;
    pstcUsbHostIntern->pstcUsbHalEndpointIN = Usb_GetEndpointPtr(pstcUsb,0x81);
    pstcUsbHostIntern->pstcUsbHalEndpointOUT = Usb_GetEndpointPtr(pstcUsb,0x02);
    
}

/**
 ******************************************************************************
 ** \brief Transfer OUT token data
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pu8Buffer Source buffer
 **
 ** \param u16Size data size
 **
 ** \param u16FifoSize FIFO data size
 **
 ******************************************************************************/
void UsbHost_TransferDataToFifo(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer, uint16_t u16Size, uint16_t u16FifoSize)
{
    boolean_t Odd = u16Size & 1;
    //stc_usbhost_intern_data_t* pstcUsbHostIntern;
    //pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    USBDBGVAL8("TX: ",u16Size);
    USBDBGVAL32("#",pu8Buffer);
    pstcUsb->EP2S = (pstcUsb->EP2S & (~0x1F)) | u16FifoSize;

    u16Size = u16Size / 2; // transfer 16 Bit words;
    while(u16Size--)	
    {
	    //EP2DT = *(uint16_t*)pu8Buffer;
	    pstcUsb->EP2DTL = *pu8Buffer++;
	    pstcUsb->EP2DTH = *pu8Buffer++;
	    //pu8Buffer += 2;
    }
    if(Odd == TRUE)  {
        pstcUsb->EP2DTL= *(uint8_t*)pu8Buffer; // transfer the first or last byte
    }
    pstcUsb->EP2S_f.DRQ = 0;	 // now the OUT FIFO is valid for the next transfer
}

/**
 ******************************************************************************
 ** \brief Transfer IN token data
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pu8Buffer Destination buffer
 **
 ** \return received size
 **
 ******************************************************************************/
uint16_t UsbHost_TransferFifoToBuffer(stc_usbn_t* pstcUsb, uint8_t* pu8Buffer)
{
    uint16_t u16Size = 0;
    uint16_t u16ReceivedSize;
    //stc_usbhost_intern_data_t* pstcUsbHostIntern;
    //pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    u16Size = (uint16_t)(pstcUsb->EP1S & 0x00FF);
    u16ReceivedSize = u16Size;
    USBDBGVAL16("RX: ",u16Size);
    USBDBGVAL32("#",pu8Buffer);
    u16Size = u16Size / 2; // transfer 16 Bit words;
    while(u16Size--)	
    {
	    //*(uint16_t*)pu8Buffer = EP1DT;
	    *pu8Buffer++ = pstcUsb->EP1DTL;
	    *pu8Buffer++ = pstcUsb->EP1DTH;
	    //pu8Buffer += 2;
    }
    if(u16ReceivedSize & 1 == TRUE)  {
        *(uint8_t*)pu8Buffer = pstcUsb->EP1DTL; // transfer the first or last byte
    }	
    pstcUsb->EP1S_f.DRQ = 0;	 // now the IN FIFO is valid for the next transfer
    return u16ReceivedSize;
}

/**
 ******************************************************************************
 ** \brief Transfer data via endpoint
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param Handler Endpoint handle
 **
 ** \param pu8Buffer buffer
 **
 ** \param u32BufferSize size of data to be transferred
 **
 ** \param CompletionHandler callback handle
 **
 ******************************************************************************/
void UsbHost_TransferData(stc_usbn_t* pstcUsb, stc_host_endpoint_t* Handler,uint8_t* pu8Buffer, uint32_t u32BufferSize, void (* CompletionHandler)(stc_usbn_t* pstcUsb))
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    pstcUsbHostIntern->bSchedulerLock = TRUE;
    (Handler->u8Status) &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
    if ((Handler->u8Interval) == 0)
    {
        (Handler->u8Status) |= USBHOST_ENDPOINTSTATUS_INUSE;
    }
    Handler->bAbortTransfer = FALSE;
    (Handler->u32DataSize) = u32BufferSize;
    (Handler->pu8Buffer) = pu8Buffer;
    (Handler->pu8BufferPos) = pu8Buffer;
    (Handler->u32BufferSize) = u32BufferSize;
    (Handler->CompletionHandler) = CompletionHandler;
    (Handler->u8Status) &= ~USBHOST_ENDPOINTSTATUS_INUSE;


    if ((Handler->u8Interval) == 0)
    {
        (Handler->u8Status) |= USBHOST_ENDPOINTSTATUS_INITIATE;
    }
    pstcUsbHostIntern->bSchedulerLock = FALSE;
    //Handler->u8Status |= USBHOST_ENDPOINTSTATUS_DATA;
}

/**
 ******************************************************************************
 ** \brief Write host relevant initializations into pstcUsbConfig
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param pstcUsbConfig Write host relevant initializations into pstcUsbConfig
 **
 ******************************************************************************/
void UsbHost_Init(stc_usbn_t* pstcUsb, stc_usb_config_t* pstcUsbConfig)
{
    //stc_usbhost_intern_data_t* pstcUsbHostIntern;
    //pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    
    pstcUsbConfig->enMode |= UsbHostEnabled;
    pstcUsbConfig->pfnHostInit = UsbHost_InitCallback;
    

}

/**
 ******************************************************************************
 ** \brief Get status of current enumerated device
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return USB device status
 **         #USBHOST_DEVICE_IDLE          = 1,
 **         #USBHOST_DEVICE_ADDRESSING    = 2,
 **         #USBHOST_DEVICE_ADDRESSED     = 3,
 **         #USBHOST_DEVICE_ENUMERATED    = 4,
 **         #USBHOST_DEVICE_CONFIGURATING = 5,
 **         #USBHOST_DEVICE_CONFIGURED    = 6
 **
 ******************************************************************************/
uint8_t UsbHost_GetDeviceStatus(stc_usbn_t* pstcUsb)
{
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    return pstcUsbHostIntern->u8DeviceStatus;
}


/**
 ******************************************************************************
 ** \brief Set configuration
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param u8Configuration number of configuration
 **
 ******************************************************************************/
void UsbHost_SetConfigurationDescriptor(stc_usbn_t* pstcUsb, uint8_t u8Configuration)
{
    stc_usb_request_t stcSetup = {0x00,0x09,0x00,0x00,0x00};
    stc_usbhost_intern_data_t* pstcUsbHostIntern;
    pstcUsbHostIntern = UsbHostGetInternDataPtr(pstcUsb);
    stcSetup.wValue |= u8Configuration;
    USBDBGVAL8("Setting Configuration: ",u8Configuration);
    pstcUsbHostIntern->u8DeviceStatus = USBHOST_DEVICE_CONFIGURATING;
    UsbHost_SetupRequest(pstcUsb,&stcSetup,0);
}

/**
 ******************************************************************************
 ** \brief Get endpoint descriptor from buffer of configuration
 **
 ** \param pu8Buffer Buffer
 **
 ** \param u16Size Size of buffer
 **
 ** \param u8InterfaceNumber Interface number
 **
 ** \param u8EndpointNumber Number of endpoint
 **
 ** \return NULL, if endpoint was not found in the configuration, else pointer to endpoint descriptor (as buffer)
 **
 ******************************************************************************/
uint8_t* UsbHost_GetUsbEndpointDescriptor(uint8_t* pu8Buffer, uint16_t u16Size, uint8_t u8InterfaceNumber, uint8_t u8EndpointNumber)
{
    uint16_t u16Position = 0;
    uint8_t u8DescriptorLength = 0;
    uint8_t u8DescriptorType = 0;
    uint8_t u8CurrentInterface = 0xFF;
    uint8_t u8CurrentEndpoint = 0;
    uint16_t u16wTotalLength;
    

    if (pu8Buffer[1] != USB_CONFIGURATION_DESCRIPTOR_TYPE) 
    {
        return 0;
    }
    
    u16wTotalLength = (uint16_t)pu8Buffer[2] + (uint16_t)(pu8Buffer[3] << 8);
    if (u16Size < u16wTotalLength)
    {
       u16wTotalLength = u16Size;
    }
    
    while(u16Position < u16wTotalLength)
    {
       u8DescriptorLength = pu8Buffer[u16Position];
       u8DescriptorType = pu8Buffer[u16Position + 1];
       if (u8DescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE)
       {
           u8CurrentEndpoint = 0;
           u8CurrentInterface = pu8Buffer[u16Position + 2];
       }
       if (u8DescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE)
       {
           u8CurrentEndpoint = u8CurrentEndpoint + 1;
       }
       if (((u8CurrentEndpoint == u8EndpointNumber) || (u8EndpointNumber == 0)) && (u8InterfaceNumber == u8CurrentInterface))
       {
           return (uint8_t*)(pu8Buffer + u16Position);
       }
       u16Position += u8DescriptorLength;
    }
    return 0;
}

//@} // UsbHostGroup

#endif
