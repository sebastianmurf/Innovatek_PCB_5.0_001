/******************************************************************************
 * $Id$ / $Rev$ / $Date$
 * $URL$
 *****************************************************************************/
/*               (C) Fujitsu Microelectronics Europe GmbH               */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user�s obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FME does not warrant that the deliverables do not infringe any       */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FME disclaims all  */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FME�s liability   */
/* is restricted to intention and gross negligence.                     */
/* FME is not liable for consequential damages.                         */
/*                                                                      */
/* (V1.3)                                                               */
/*****************************************************************************/
/** \file UsbHostMassStorage.c
 **
 ** - See README.TXT for project description
 ** - first release for a simple universal usb function library
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 **   - 2011-08-11    1.1  MSc  Several Bug fixes
 **   - 2011-08-24    1.2  MSc  Timeout Bug fixes
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "Usb.h" 

#ifndef USB_DISBALE_HOST_FUNCTIONALITY
#include "UsbHostMassStorage.h"


/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


/*****************************************************************************/
/* function */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/
/*
UMH_HANDLE     umhInHandle;
UMH_HANDLE     umhOutHandle;
*/

static stc_host_endpoint_t *EndpointOUT;
static stc_host_endpoint_t *EndpointIN;
static uint8_t        u8EndpointIN, u8EndpointOUT;
static uint16_t       u16MaxPackageSizeIN, u16MaxPackageSizeOUT;
volatile static uint8_t        u8Status = 0;
volatile static boolean_t      bSetupComplete = FALSE;
StorageInfo_t  stcGlobalStorageInfo;
static StorageInfo_t* pstcStorageInfo;
static stc_usb_request_t setup;
static stc_usbn_t* pstcUsbHandle = NULL;
/*SETUP_DATA     setup; 
StorageInfo_t  stcGlobalStorageInfo;
*/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/



/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/
static void WaitForSetupComplete(void);
static void SetupComplete(stc_usbn_t* pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataSize);
static void convBufferToSenseResponse(uint8_t *buffer, struct RequestSenseResponse *sr) ;
static void convBufferToModeSenseResponse(uint8_t *buffer, struct ModeSenseResponse *msr);
static void convBufferToInquiryResponse(uint8_t *buffer, InquiryResponse_t *ir);
static void convCBWtoBuffer(cbw_t *cbw, uint8_t *Buffer);
static int
convBufferToCSW(
    uint8_t *buffer,      // buffer that contains a status wrapper block  
    int length,                 // length of the buffer
    csw_t *csw
);
/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/
/* Storage API */
int UsbHostMassStorage_StorageApiRead(BYTE *buff, DWORD sector, BYTE count)
{  
   unsigned int status;
   unsigned char NAKDetected = 0;
   pstcStorageInfo = UsbHostMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   pstcStorageInfo->STORAGE_CMD = READ_MASS_STORAGE;
   pstcStorageInfo->BUFFER = buff;
   pstcStorageInfo->BLOCK_START = sector;
   pstcStorageInfo->BLOCK_CNT = count;

   status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
   if (status)
   {
       return status;
   }
   if (pstcStorageInfo->CSW_STATUS == 1)
   {
      struct RequestSenseResponse SenseResponse;
      uint8_t RSRBuffer[sizeof(SenseResponse)];
      ZERO_STRUCT(*pstcStorageInfo);
      ZERO_STRUCT(SenseResponse);
      pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
      pstcStorageInfo->BUFFER = RSRBuffer; //(uint8_t*) &SenseResponse;
      status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
      UsbHostMassStorage_CheckForCompletion(&NAKDetected);
      convBufferToSenseResponse(RSRBuffer,&SenseResponse);
      if(status)
      {
         return status;
      }
      status = SenseResponse.SenseKey;
      return status;
   }
  
   return status;
}

int UsbHostMassStorage_StorageApiWrite(BYTE *buff, DWORD sector, BYTE count)
{
   unsigned int status;
   pstcStorageInfo = UsbHostMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   pstcStorageInfo->STORAGE_CMD = WRITE_MASS_STORAGE;
   pstcStorageInfo->BUFFER = buff;
   pstcStorageInfo->BLOCK_START = sector;
   pstcStorageInfo->BLOCK_CNT = count;
   
   status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);

   if (pstcStorageInfo->CSW_STATUS == 1)
   {
      RequestSenseResponse_t SenseResponse;
      uint8_t NAKDetected;
      uint8_t RSRBuffer[sizeof(SenseResponse)];
      ZERO_STRUCT(*pstcStorageInfo);
      ZERO_STRUCT(SenseResponse);
      pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
      pstcStorageInfo->BUFFER = RSRBuffer; //(unsigned char *) &SenseResponse;
      status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
      
      UsbHostMassStorage_CheckForCompletion(&NAKDetected);
      convBufferToSenseResponse(RSRBuffer,&SenseResponse);
      if (status)
      {
         return status;
      }
      
      if(SenseResponse.SenseKey == 0x7)
      {
         return 0x02;  // Write protect
      }
      
      status = SenseResponse.SenseKey;
      return status;      
   }
   
   return status; 
}

int UsbHostMassStorage_StorageApiReadCapacity(void (*Buff))
{
   unsigned int status;
   pstcStorageInfo = UsbHostMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   pstcStorageInfo->STORAGE_CMD = READ_MASS_STORAGE_CAPACITY;
   pstcStorageInfo->BUFFER = Buff;
   
   status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);

   if (pstcStorageInfo->CSW_STATUS == 1)
   {
      RequestSenseResponse_t SenseResponse;
      uint8_t NAKDetected;
      uint8_t RSRBuffer[sizeof(SenseResponse)];
      ZERO_STRUCT(*pstcStorageInfo);
      ZERO_STRUCT(SenseResponse);
      pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
      pstcStorageInfo->BUFFER = RSRBuffer; //(unsigned char *)&SenseResponse;
      status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
      
      UsbHostMassStorage_CheckForCompletion(&NAKDetected);
      convBufferToSenseResponse(RSRBuffer,&SenseResponse);
      if (status)
      {
         return status;
      }
      status = SenseResponse.SenseKey;
      return status;
   }
   
   return status; 
}


int UsbHostMassStorage_StorageApiInit(void)
{
   unsigned char cnt=0;
   unsigned char CheckAgain = 1; 
   unsigned char NAKDetected = 0; 
   InquiryResponse_t InquiryResponse;
   uint8_t IRbuffer[sizeof(InquiryResponse)];
   unsigned int status=0;
   pstcStorageInfo = UsbHostMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   ZERO_STRUCT(InquiryResponse);
   pstcStorageInfo->STORAGE_CMD = INQUIRY_MASS_STORAGE;
   pstcStorageInfo->BUFFER = IRbuffer;
   //(unsigned char *)&InquiryResponse;
      
   status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
   UsbHostMassStorage_CheckForCompletion(&NAKDetected);
   convBufferToInquiryResponse(IRbuffer,&InquiryResponse);  
   
   if (status)
   {
      return status;
   }
   
   while(CheckAgain)
   {
      if(cnt >= 2)
      {
         return USBH_STATUS_ERROR;
      }
      
      ZERO_STRUCT(*pstcStorageInfo);
      pstcStorageInfo->STORAGE_CMD = TEST_UNIT_READY;      

      status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
      
      if (status)
      {
         return status;
      }   
      
      if (pstcStorageInfo->CSW_STATUS == 1)
      {
         struct RequestSenseResponse SenseResponse;
         uint8_t RSRBuffer[sizeof(SenseResponse)];
         ZERO_STRUCT(*pstcStorageInfo);
         ZERO_STRUCT(SenseResponse);
         pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
         pstcStorageInfo->BUFFER = RSRBuffer;
         //(uint8_t *)&SenseResponse;
         status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
         UsbHostMassStorage_CheckForCompletion(&NAKDetected);
         convBufferToSenseResponse(RSRBuffer,&SenseResponse);
         if (status)
         {
            return status;
         }
         if (SenseResponse.SenseKey != 0x06)
         {
            CheckAgain = 0;        
         }
         cnt++;
         status = SenseResponse.SenseKey;         
      }
      else
      {
         CheckAgain = 0;
      }
   } // end of while 
   status = UsbHostMassStorage_StorageStatus();   
   return status;
}

int UsbHostMassStorage_StorageApiModeSense(void)
{
   uint16_t status;
   struct ModeSenseResponse ModeSense;
   uint8_t MSRBuffer[sizeof(ModeSense)];
   uint8_t NAKDetected;
   
   pstcStorageInfo = UsbHostMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   ZERO_STRUCT(ModeSense);
   pstcStorageInfo->STORAGE_CMD = MODE_SENSE;
   pstcStorageInfo->BUFFER = MSRBuffer; //(uint8_t*)&ModeSense;   
   status = UsbHostMassStorage_StorageCommandSend(pstcStorageInfo);
   UsbHostMassStorage_CheckForCompletion(&NAKDetected);
   convBufferToModeSenseResponse(MSRBuffer,&ModeSense);
   if (ModeSense.WriteProtect == 1)
   {
      return USBH_STATUS_ERROR;
   }      
   return status; 
}

/* End of Storage API */

boolean_t UsbHostMassStorage_InitHandler(stc_usbn_t* pstcUsb, uint8_t* pu8Configuration, uint32_t u32Length)
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
    
    u8NumberOfInterfaces=pu8Configuration[4];
    for(u8InterfaceNumber=0;u8InterfaceNumber < u8NumberOfInterfaces;u8InterfaceNumber++)
    {

        pu8Buffer = UsbHost_GetUsbInterfaceDescriptor(pu8Configuration,u32Length,u8InterfaceNumber);
        if (pu8Buffer == 0)
        {

            return FALSE;
        }
        u8NumberOfEndpoints = pu8Buffer[4];
        
        if ((pu8Buffer[6] == 0x06) && (pu8Buffer[7] == 0x50)) //SubClass & Protocol
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

            
            UsbHost_AddHostEndpoint(pstcUsbHandle,&EndpointOUT,u8EndpointOUT,u16MaxPackageSizeOUT,0,0);
            UsbHost_AddHostEndpoint(pstcUsbHandle,&EndpointIN,u8EndpointIN,u16MaxPackageSizeIN,0,0);
             

            break;
        }
    }
    if ((u8EndpointIN == 0) || (u8EndpointOUT == 0))
    {
        return FALSE;
    }       
    return TRUE;               
}

boolean_t UsbHostMassStorage_DeinitHandler(stc_usbn_t* pstcUsb)
{
    pstcUsbHandle = NULL;
    return TRUE;
}

static void SetupComplete(stc_usbn_t* pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataSize)
{
    bSetupComplete = TRUE;
}

static void WaitForSetupComplete()
{
    volatile uint32_t u32Timeout = 1000000;
    while((bSetupComplete == FALSE) && (u32Timeout > 5))
    {
        u32Timeout--;
    }
}
void UsbHostMassStorage_BulkInCompletion(stc_usbn_t* pstcUsbHandle)
{
    //SwapBuffer(EndpointIN->pu8Buffer,EndpointIN->u32DataSize);
    stcGlobalStorageInfo.STATUS = ((EndpointIN->u8Status) & 0xF0);
    stcGlobalStorageInfo.BYTES_TRANSFERRED = EndpointIN->u32DataSize;
    UsbHostMassStorage_InTransferDone();
}

void UsbHostMassStorage_BulkOutCompletion(stc_usbn_t* pstcUsbHandle)
{
    stcGlobalStorageInfo.STATUS = ((EndpointOUT->u8Status) & 0xF0);
    stcGlobalStorageInfo.BYTES_TRANSFERRED = EndpointOUT->u32DataSize;
    UsbHostMassStorage_OutTransferDone();
}

// transfer done is called from the transfer completion function
void UsbHostMassStorage_InTransferDone(void)
{
    CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
}

void UsbHostMassStorage_OutTransferDone(void)
{
    CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT);
}



StorageInfo_t* UsbHostMassStorage_GetGlobalStorageInfo()
{
    return &stcGlobalStorageInfo;
}



read_capacity_t ReadCapacity;
csw_t csw;
cbw_t cbw;
uint8_t CswBuffer[13];
uint8_t CbwBuffer[31];

static void convBufferToSenseResponse(uint8_t *buffer, struct RequestSenseResponse *sr) 
{
    sr->ResponseCode = ((buffer[0] & 0xFE) >> 1);
    sr->VALID = (buffer[0] & 0x01);
    sr->Obsolete = buffer[1];
    sr->SenseKey = ((buffer[2] & 0xF0) >> 4);
    sr->ILI = ((buffer[2] & 0x04) >> 2);
    sr->EOM = ((buffer[2] & 0x02) >> 1);
    sr->FILEMARK = (buffer[2] & 0x01);
    sr->Information =  LENDIAN_BUFFER_TO_U32(buffer+3);
    sr->AddSenseLen = buffer[7];
    sr->CmdSpecificInfo =  LENDIAN_BUFFER_TO_U32(buffer+8);
    sr->ASC = buffer[12];
    sr->ASCQ = buffer[13];
    sr->FRUC = buffer[14];
    sr->SenseKeySpecific[0] = buffer[15];
    sr->SenseKeySpecific[1] = buffer[16];
    sr->SenseKeySpecific[2] = buffer[17];
}
static void convBufferToModeSenseResponse(uint8_t *buffer, struct ModeSenseResponse *msr) {
    msr->ModeDataLength = buffer[0];
    msr->MediumType = buffer[1];
    msr->Reserved1 = ((buffer[2] & 0xF0) >> 4);
    msr->DPOFUA = ((buffer[2] & 0x08) >> 3); 
    msr->Reserved2 = ((buffer[2] & 0x06) >> 1);
    msr->WriteProtect = (buffer[2] & 0x01); 
    msr->BlockDescriptorLength = buffer[3];
}
static void convBufferToInquiryResponse(uint8_t *buffer, InquiryResponse_t *ir) {
    unsigned char i;
    ir->Peripheral = buffer[0];
    ir->Removable = buffer[1];
    ir->Version = buffer[2];
    ir->ResponseDataFormat = buffer[3];
    ir->AdditionalLength = buffer[4];
    ir->Sccstp = buffer[5];
    ir->Bqueetc = buffer[6];
    ir->CmdQue = buffer[7];
    for(i=0;i<8;i++) 
    {
        ir->VendorID[i] = buffer[8+i];
    }
    for(i=0;i<16;i++) 
    {
        ir->ProductID[i] = buffer[16+i];
    }
}
static void convCBWtoBuffer(cbw_t *cbw, uint8_t *Buffer)
{
    unsigned char i;
    U32_TO_LENDIAN_BUFFER(Buffer,cbw->dCBWSignature);            // index 0:Signature
    U32_TO_LENDIAN_BUFFER(Buffer+4,cbw->dCBWTag);                // index:4 Tag
    U32_TO_LENDIAN_BUFFER(Buffer+8,cbw->dCBWDataTransferLength); // index:8 DataTransferLength
    Buffer[12]=cbw->bCBWFlags;
    Buffer[13]=cbw->bCBWLUN;
    Buffer[14]=cbw->bCBWCBLength;
    for(i=0;i<16;i++)
    {
        Buffer[15 + i] = cbw->CBWCB[i];
    }
}

static int
convBufferToCSW(
    uint8_t *buffer,      // buffer that contains a status wrapper block  
    int length,                 // length of the buffer
    csw_t *csw
)
{
    if( length < 13)
    {
        return USBH_STATUS_LENGTH;
    }
    csw->CSWSignatur=LENDIAN_BUFFER_TO_U32(buffer);
    csw->CSWTag=LENDIAN_BUFFER_TO_U32(buffer+4);    // 4: tag Same as original command
    csw->CSWDataResidue=LENDIAN_BUFFER_TO_U32(buffer+8);// 8: residue, amount of bytes not transferred
    csw->CSWStatus=(uint32_t)*(buffer+12);//12:status
    return USBH_STATUS_SUCCESS;
}


static void ErrorHandler(uint16_t errCode, uint8_t *clrEponly )
{
    //uint8_t InEpStatus=0, OutEpStatus=0;
    if (errCode & USBHOST_ENDPOINTSTATUS_STALL)                  
    {

        if(*clrEponly)
        {
                //UsbHostMassStorage_GetEPStatus(&InEpStatus, &OutEpStatus);
           if((EndpointIN->u8Status) & USBHOST_ENDPOINTSTATUS_STALL)
           {

               bSetupComplete = FALSE;
               UsbHostMassStorage_ClearInEndpoint();     
               EndpointIN->u8Status &= ~USBHOST_ENDPOINTSTATUS_STALL;
           }
           if((EndpointOUT->u8Status) & USBHOST_ENDPOINTSTATUS_STALL)
           {
               UsbHostMassStorage_ClearOutEndpoint();
               EndpointOUT->u8Status &= ~USBHOST_ENDPOINTSTATUS_STALL;
           }
           *clrEponly=0;
       }
       return;
       //UsbHostMassStorage_ResetRecovery();
   }   
   //UsbHostMassStorage_ResetRecovery();
}

void UsbHostMassStorage_CheckForCompletion(volatile uint8_t *NAKDetected)
{
   volatile uint32_t u32Timeout = 20000000;
   *NAKDetected = 0;

   while((IS_SET(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT)) || (IS_SET(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN))) // Wait for status information of previous transmission
   {  
      if ((u32Timeout < 5) || (UsbHost_GetDeviceStatus(pstcUsbHandle) == USBHOST_DEVICE_IDLE))  // if there is no Handshake, something went wrong!
      {
         CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
         CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT);
         EndpointOUT->bAbortTransfer = TRUE;
         EndpointIN->bAbortTransfer = TRUE;
         *NAKDetected = 1;   // There must be a circular NAK Handshake
         u32Timeout = 1000000;
         while((EndpointIN->bAbortTransfer) && (EndpointOUT->bAbortTransfer) && (u32Timeout > 5) && (UsbHost_GetDeviceStatus(pstcUsbHandle) != USBHOST_DEVICE_IDLE)) u32Timeout--;

      }
      u32Timeout--;
   } 

}



int16_t UsbHostMassStorage_StorageCommandSend(StorageInfo_t *pstcStorageInfo)
{
    uint16_t status;
    volatile uint8_t NAKDetected=0;
    uint8_t clrEponly=0;
    uint16_t cnt=0;
    cnt = cnt;

    UsbHostMassStorage_CheckForCompletion(&NAKDetected);
    SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT); // Block transfer 
    status = UsbHostMassStorage_SendCBW(pstcStorageInfo);
    if(status)
    {
        return status;
    }
    UsbHostMassStorage_CheckForCompletion(&NAKDetected);
    if(pstcStorageInfo->STATUS || NAKDetected)
    { 
        ErrorHandler(pstcStorageInfo->STATUS, &clrEponly );      
        return USBH_STATUS_ERROR;  
    }

    if(cbw.dCBWDataTransferLength != 0) // Jump in, if there is a data Transfer
    {    

        if(pstcStorageInfo->STORAGE_CMD == WRITE_MASS_STORAGE)
        {
            SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT);
        }
        else
        {
            SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
        }   
        status = UsbHostMassStorage_Transmit_Data(pstcStorageInfo);
        if(status)
        {
            return status;
        }
        UsbHostMassStorage_CheckForCompletion(&NAKDetected);

        if((pstcStorageInfo->STATUS) || (NAKDetected))
        {
             clrEponly=1;
             ErrorHandler(pstcStorageInfo->STATUS, &clrEponly) ;
             NAKDetected=0;       
        } 
    }   
    SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
    status = UsbHostMassStorage_GetCSW(pstcStorageInfo);
    if(status)
    {
        return status;
    }
   
    UsbHostMassStorage_CheckForCompletion(&NAKDetected);

    if((pstcStorageInfo->STATUS) || (NAKDetected))
    {
        
        clrEponly=1;
        ErrorHandler(pstcStorageInfo->STATUS, &clrEponly);
        NAKDetected=0;
        status = UsbHostMassStorage_GetCSW(pstcStorageInfo);
        if(status)
        {
            return status;
        }
        UsbHostMassStorage_CheckForCompletion(&NAKDetected);
        if(pstcStorageInfo->STATUS || NAKDetected)
        {
            ErrorHandler(pstcStorageInfo->STATUS, &clrEponly);
            return USBH_STATUS_ERROR;  
        }
    }
    status = UsbHostMassStorage_CheckCSW(pstcStorageInfo);    
    return status;
}

int16_t UsbHostMassStorage_SendCBW(StorageInfo_t *pstcStorageInfo)
{  
    uint16_t status;
    ZERO_STRUCT(cbw);
    pstcStorageInfo->SEND_CBW = 1;
    pstcStorageInfo->BUFFER_SIZE = 31; // CBW Size must always be 31 byte
 
    cbw.dCBWSignature = 0x43425355; 
    cbw.dCBWTag = CBW_TAG; 
    cbw.bCBWLUN = 0x00; 
    cbw.CBWCB[1] = 0x00;
    cbw.CBWCB[2] = (0xff & (pstcStorageInfo->BLOCK_START >> 24));
    cbw.CBWCB[3] = (0xff & (pstcStorageInfo->BLOCK_START >> 16));
    cbw.CBWCB[4] = (0xff & (pstcStorageInfo->BLOCK_START >> 8));
    cbw.CBWCB[5] = (0xff & (pstcStorageInfo->BLOCK_START));
    cbw.CBWCB[6] = 0x00;
    cbw.CBWCB[7] = (uint8_t)(((uint16_t)(pstcStorageInfo->BLOCK_CNT) >> 8) & 0xff);
    cbw.CBWCB[8] = (uint8_t)(((uint16_t)(pstcStorageInfo->BLOCK_CNT)  & 0xff));
    cbw.CBWCB[9] = 0x00;
   
    switch(pstcStorageInfo->STORAGE_CMD)                  
    {
        case READ_MASS_STORAGE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000200;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x0A; 
            break;
      
        case WRITE_MASS_STORAGE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000200;
            cbw.bCBWFlags = 0x00;
            cbw.bCBWCBLength = 0x0A; 
            break;
      
        case INQUIRY_MASS_STORAGE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000024;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x06;
            cbw.CBWCB[4] = 0x24; 
            break;
      
        case READ_MASS_STORAGE_CAPACITY:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000008;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x0A ;
            break;

        case TEST_UNIT_READY:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000000;
            cbw.bCBWFlags = 0x00;
            cbw.bCBWCBLength = 0x06;
            break;
            
        case MASS_STORAGE_REQUEST_SENSE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000012;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x0C;
            cbw.CBWCB[4] = 0x12; 
            break;
      
        case SEND_DIAGNOSTIC:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.CBWCB[1] = 0x04;
            cbw.dCBWDataTransferLength = 0x00000000;
            cbw.bCBWCBLength = 0x0A;
            break;
      
        case MODE_SENSE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000004;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x06;
            cbw.CBWCB[2] = 0x3F;
            cbw.CBWCB[4] = 0x04;
            break;
   }
   #if (ENDIANTYPE == LITTLEENDIAN)
       status = UsbHostMassStorage_Transmit_MSDC_Data((uint8_t*)&cbw, pstcStorageInfo); 
   #else
       convCBWtoBuffer(&cbw,CbwBuffer);
       status = UsbHostMassStorage_Transmit_MSDC_Data((uint8_t*)CbwBuffer, pstcStorageInfo); 
       if (status)
       {
           //status = UsbHostMassStorage_Transmit_MSDC_Data((uint8_t*)CbwBuffer, pstcStorageInfo); 
       }
       
   #endif 
   pstcStorageInfo->SEND_CBW = 0;
   return status;
   
}

int16_t UsbHostMassStorage_Transmit_MSDC_Data(uint8_t *pu8Buffer, StorageInfo_t *pstcStorageInfo)
{   
   if (pstcStorageInfo == 0)
   {

      return 2;
   }
   //SwapBuffer(pu8Buffer,pstcStorageInfo->BUFFER_SIZE);
   if((EndpointOUT != 0) && (EndpointIN != 0))                 
   {
      
      if(pstcStorageInfo->SEND_CBW)
      {

         UsbHost_TransferData(pstcUsbHandle,EndpointOUT,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbHostMassStorage_BulkOutCompletion);
         return USBH_STATUS_SUCCESS;
      }
      
      if(pstcStorageInfo->GET_CSW)
      {

         UsbHost_TransferData(pstcUsbHandle,EndpointIN,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbHostMassStorage_BulkInCompletion);
         return USBH_STATUS_SUCCESS;
      }
       
      switch(pstcStorageInfo->STORAGE_CMD)                  
      {

         case WRITE_MASS_STORAGE:
         case TEST_UNIT_READY:

         UsbHost_TransferData(pstcUsbHandle,EndpointOUT,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbHostMassStorage_BulkOutCompletion);      
         break; 
         

         case MASS_STORAGE_REQUEST_SENSE:
         case MODE_SENSE:
         case READ_MASS_STORAGE_CAPACITY:
         case INQUIRY_MASS_STORAGE:     
         case READ_MASS_STORAGE:     

         UsbHost_TransferData(pstcUsbHandle,EndpointIN,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbHostMassStorage_BulkInCompletion);
         break;
       }   
       return USBH_STATUS_SUCCESS;     
   }
   

   return USBH_STATUS_ERROR;   
         
}

int16_t UsbHostMassStorage_Transmit_Data(StorageInfo_t *pstcStorageInfo)
{
    uint16_t status;
    pstcStorageInfo->BUFFER_SIZE = cbw.dCBWDataTransferLength;

    status = UsbHostMassStorage_Transmit_MSDC_Data(pstcStorageInfo->BUFFER, pstcStorageInfo);
    return status;
}

int16_t UsbHostMassStorage_GetCSW(StorageInfo_t *pstcStorageInfo)
{  
    uint16_t status;
    volatile uint8_t NAKDetected=0;
    ZERO_STRUCT(csw);
    pstcStorageInfo->GET_CSW = 1;
    pstcStorageInfo->BUFFER_SIZE = 13; //CBW Size must always be 13 byte
    #if (ENDIANTYPE == LITTLEENDIAN)
        status = UsbHostMassStorage_Transmit_MSDC_Data((uint8_t*)&csw, pstcStorageInfo);
    #else 
        status = UsbHostMassStorage_Transmit_MSDC_Data((uint8_t*)CswBuffer, pstcStorageInfo); 
        UsbHostMassStorage_CheckForCompletion(&NAKDetected);
        if ((status == 0) && (NAKDetected == 1))
        {
            status = 3;
        }
        if (!status) 
        {
            status = convBufferToCSW(CswBuffer,13,&csw);
        }
    #endif
    pstcStorageInfo->GET_CSW = 0;
    return status;
}

int16_t UsbHostMassStorage_CheckCSW(StorageInfo_t *pstcStorageInfo)
{  // Check if CSW is valid
    uint8_t clrEponly=0;

    if((csw.CSWTag != CBW_TAG) || (csw.CSWSignatur != 0x53425355) ||((pstcStorageInfo->BYTES_TRANSFERRED) != 13 ))
    {
        ErrorHandler((uint16_t)NULL, &clrEponly);
        return USBH_STATUS_ERROR;
    }  // Check if CSW is meaningful
    if((csw.CSWStatus == 2) || (csw.CSWDataResidue >> cbw.dCBWDataTransferLength)) 
    {

        ErrorHandler((uint16_t)NULL, &clrEponly);
        return USBH_STATUS_ERROR;
    }
    if(csw.CSWStatus == 1)
    {
        pstcStorageInfo->CSW_STATUS = 1;   
    }
    return USBH_STATUS_SUCCESS;
}


int16_t UsbHostMassStorage_BulkOnlyMSReset(void)
{
    ZERO_STRUCT(setup);
    setup.bmRequestType=0x21;  
    setup.bRequest=0xFF;
    setup.wValue=0;
    setup.wIndex=0;
    setup.wLength=0;
    UsbHost_SetupRequest(pstcUsbHandle,&setup,SetupComplete);
    WaitForSetupComplete();
    return USBH_STATUS_SUCCESS;
}

int16_t UsbHostMassStorage_StorageStatus(void)
{  
    if( UsbHost_GetDeviceStatus(pstcUsbHandle) == USBHOST_DEVICE_CONFIGURED)
    {

        return 0;
    }
    else
    {

        return 1;
    }
} 

int16_t UsbHostMassStorage_ResetRecovery(void)
{
    uint16_t status;
    status = UsbHostMassStorage_BulkOnlyMSReset();
    status = UsbHostMassStorage_ClearOutEndpoint();
    status = UsbHostMassStorage_ClearInEndpoint();
    status = UsbHostMassStorage_TestUnitReady(&stcGlobalStorageInfo);
    return status; 

}

int16_t UsbHostMassStorage_ClearInEndpoint(void)
{
    ZERO_STRUCT(setup);
    setup.bmRequestType=0x02;  
    setup.bRequest=0x01;
    setup.wValue=0x00;
    setup.wIndex = USB_IN_DIRECTION | u8EndpointIN;
    setup.wLength=0x00;
    //UMH_AbortTransfer(umhInHandle); TBD
    UsbHost_SetupRequest(pstcUsbHandle,&setup,SetupComplete);
    WaitForSetupComplete();
    //UMH_ResetDataToggleBit(umhInHandle); TBD
    return USBH_STATUS_SUCCESS;
}

int16_t UsbHostMassStorage_ClearOutEndpoint(void)
{
    ZERO_STRUCT(setup);
    setup.bmRequestType=0x02;  
    setup.bRequest=0x01;
    setup.wValue=0x00;  
    setup.wIndex=USB_OUT_DIRECTION | u8EndpointOUT;
    setup.wLength=0x00;
    //UMH_AbortTransfer(umhOutHandle);  TBD
    UsbHost_SetupRequest(pstcUsbHandle,&setup,SetupComplete);
    WaitForSetupComplete();
    //UMH_ResetDataToggleBit(umhOutHandle);  TBD   
    return USBH_STATUS_SUCCESS;
}

int UsbHostMassStorage_TestUnitReady(StorageInfo_t *pstcStorageInfo)
{  
    uint16_t status;
    ZERO_STRUCT(pstcStorageInfo);
    pstcStorageInfo->STORAGE_CMD = TEST_UNIT_READY;

    status = UsbHostMassStorage_SendCBW(pstcStorageInfo);
    //UMH_StallExecutionUserCallback(200);
    UsbHostMassStorage_GetCSW(pstcStorageInfo);

    return status;
}

/*
void UsbHostMassStorage_GetEPStatus(uint8_t* InEpStatus, uint8_t* OutEpStatus)
{
    SETUP_DATA setup;
    int16_t status;
    uint16_t transf=0;
  
    ZERO_STRUCT(setup);
    setup.Bytes.Type=0x82;  
    setup.Bytes.Request=0x00;
    setup.Words.Value=0x00;  
    setup.Words.Index=USB_IN_DIRECTION | u8EndpointIN;
    setup.Words.Length=0x02;
    status = UMH_SetupRequest((uint8_t*)&setup,(uint8_t*)InEpStatus,(unsigned int*)&transf);     
  
    ZERO_STRUCT(setup);
    setup.Bytes.Type=0x82;  
    setup.Bytes.Request=0x00;
    setup.Words.Value=0x00;  
    setup.Words.Index=USB_OUT_DIRECTION | u8EndpointOUT;
    setup.Words.Length=0x02;
    status = UMH_SetupRequest((uint8_t*)&setup,(uint8_t*)OutEpStatus,(unsigned int*)&transf);     
}
*/

// max. execution time is 100us
/*static void Idle(void)
{
  return;
}*/

// this function is called from the library to wait 
// a specified time. This function must be implemented by the user
// with the fixed function name UmhStallExecutionCallback.
// Return: nothing
// parameter: time in 0.1 milliseconds units

/*void UMH_StallExecutionUserCallback(unsigned long time)
{ 
  unsigned long startTime=GetCurrentTime();

  while(1)
  {
    if( IsTimeOver(time,startTime)  )
      break;
    Idle();
  }
}*/

boolean_t UsbHostMassStorage_DriverActive(void)
{
    return (pstcUsbHandle != NULL);
}

#endif /* (USE_USB_HOST == 1) */
