/******************************************************************************
 * $Id$ / 1.0 / 2009.12.18
 * http://emea.fujitsu.com
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
/** \file UsbHostMassStorage.h
 **
 ** USB Descriptors File
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 **   - 2011-08-11    1.1  MSc  Several Bug fixes
 **   - 2011-08-24    1.2  MSc  Timeout Bug fixes
 *****************************************************************************/

#ifndef __USBMASSSTORAGE_H__
#define __USBMASSSTORAGE_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "mcu.h"
#include "base_types_l3.h"
#include "Usb.h"
#ifndef USB_DISBALE_HOST_FUNCTIONALITY
#include "UsbHost.h"
#include "diskio.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USBMASSSTORAGE_FLAG_TRANSFERDONE     (1<<0)
#define USBMASSSTORAGE_FLAG_TRANSFERDONE_IN  (1<<1)
#define USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT (1<<2)

#define FORMAT_UNIT							0x04
#define INQUIRY_MASS_STORAGE				0x12
#define MODE_SELECT							0x55
#define MODE_SENSE							0x5A
#define PREVENT_ALLOW_MEDIUM_REMOVAL		0x1E
#define READ_MASS_STORAGE  				    0x28
#define READ_MASS_STORAGE_CAPACITY			0x25	
#define READ_FORMAT_CAPACITIES				0x23
#define MASS_STORAGE_REQUEST_SENSE			0x03
#define REZERO								0x01
#define SEEK10								0x2B
#define SEND_DIAGNOSTIC						0x1D
#define START_STOP_UNIT						0x1B
#define TEST_UNIT_READY						0x00
#define VERIFY								0x2F
#define WRITE_MASS_STORAGE   				0x2A
#define WRITE12								0xAA
#define WRITE_AND_VERIFY					0x2E

#define CBW_TAG                       0x1A2B3C4D 

#define LITTLEENDIAN 1
#define BIGENDIAN 2
#define ENDIANTYPE BIGENDIAN

#define LENDIAN_BUFFER_TO_U32(pu8Buffer)\
(uint32_t)( (uint32_t)*(uint8_t*)(pu8Buffer)                                   \
         |  ((((uint32_t)*(((uint8_t*)(pu8Buffer))+1)) <<  8  ) & 0xFF00UL    )\
         |  ((((uint32_t)*(((uint8_t*)(pu8Buffer))+2)) << 16  ) & 0xFF0000UL  )\
         |  ((((uint32_t)*(((uint8_t*)(pu8Buffer))+3)) << 24  ) & 0xFF000000UL)\
         )

#define U32_TO_LENDIAN_BUFFER(pu8Buffer,u32Data)\
    { *((pu8Buffer))  =(uint8_t)((uint32_t)(u32Data)      );\
      *((pu8Buffer)+1)=(uint8_t)((uint32_t)(u32Data) >>  8);\
      *((pu8Buffer)+2)=(uint8_t)((uint32_t)(u32Data) >> 16);\
      *((pu8Buffer)+3)=(uint8_t)((uint32_t)(u32Data) >> 24);\
    }

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/
typedef struct cbw
{
   uint32_t dCBWSignature;
   uint32_t dCBWTag;
   uint32_t dCBWDataTransferLength;
   uint8_t  bCBWFlags;
   uint8_t  bCBWLUN;
   uint8_t  bCBWCBLength;
   uint8_t  CBWCB[16];
} cbw_t;

typedef struct csw
{
   uint32_t CSWSignatur;
   uint32_t CSWTag;
   uint32_t CSWDataResidue;
   uint8_t CSWStatus;
} csw_t;

// Response struct
typedef struct InquiryResponse
{
   uint8_t Peripheral;
   uint8_t Removable;
   uint8_t Version;
   uint8_t ResponseDataFormat;
   uint8_t AdditionalLength;
   uint8_t Sccstp;
   uint8_t Bqueetc;
   uint8_t CmdQue;
   uint8_t VendorID[8];
   uint8_t ProductID[16];
   uint8_t ProductRev[4];
} InquiryResponse_t;


// 8 byte capacity value buffer
typedef struct read_capacity
{
   uint32_t SectorCount;
   uint32_t SectorSize;
} read_capacity_t;


// Functions
typedef struct RequestSenseResponse
{
    uint16_t ResponseCode :7;
    uint16_t VALID :1;
    uint8_t Obsolete;
    uint16_t SenseKey :4;
    uint16_t Resv :1;
    uint16_t ILI :1;
    uint16_t EOM :1;
    uint16_t FILEMARK :1;
    uint32_t Information;
    uint8_t AddSenseLen;
    uint32_t CmdSpecificInfo;
    uint8_t ASC;
    uint8_t ASCQ;
    uint8_t FRUC;
    uint8_t SenseKeySpecific[3];
} RequestSenseResponse_t;

typedef struct StorageInfo
{
    uint8_t STORAGE_CMD;
    uint16_t  BUFFER_SIZE;
    uint8_t*  BUFFER;
    uint16_t  IN_EP;
    uint16_t  OUT_EP;
    uint32_t BLOCK_START;
    uint8_t BLOCK_CNT;
    uint16_t  STATUS;
    uint8_t SEND_CBW;
    uint8_t GET_CSW;
    uint8_t CSW_STATUS;
    uint16_t  BYTES_TRANSFERRED;
    uint16_t  ERROR_CODE;
    uint8_t MAX_LUN;
} StorageInfo_t;


typedef struct inquiry_response
{
    uint8_t Peripheral;
    uint8_t Removable;
    uint8_t Version;
    uint8_t ResponseDataFormat;
    uint8_t AdditionalLength;
    uint8_t Sccstp;
    uint8_t Bqueetc;
    uint8_t CmdQue;
    uint8_t VendorID[8];
    uint8_t ProductID[16];
    uint8_t ProductRev[4];
} inquiry_response_t;

typedef struct ModeSenseResponse
{  
    uint8_t ModeDataLength;   
    uint8_t MediumType;     
    uint8_t Reserved1     :4;
    uint8_t DPOFUA        :1;
    uint8_t Reserved2     :2;
    uint8_t WriteProtect  :1; 
    uint8_t BlockDescriptorLength;
} ModeSenseResponse_t;

typedef struct StorageReadWrite
{
    uint8_t data[512];
} StorageReadWrite_t;


// Struct for write data
typedef struct StorageWrite
{
    uint8_t data[10];
} StorageWrite_t;


/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/



/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

/* Storage API */
int UsbHostMassStorage_StorageApiRead(BYTE *buff, DWORD sector, BYTE count);
int UsbHostMassStorage_StorageApiWrite(BYTE *buff, DWORD sector, BYTE count);
int UsbHostMassStorage_StorageApiReadCapacity(void (*Buff));
int UsbHostMassStorage_StorageApiInit(void);
int UsbHostMassStorage_StorageApiModeSense(void);
/* END Storage API */

void UsbHostMassStorage_Init(void);
boolean_t UsbHostMassStorage_DeinitHandler(stc_usbn_t* pstcUsb);
boolean_t UsbHostMassStorage_InitHandler(stc_usbn_t* pstcUsb,uint8_t* pu8Configuration, uint32_t u32Length);
void UsbHostMassStorage_BulkInCompletion(stc_usbn_t* pstcUsbHandle);
void UsbHostMassStorage_BulkOutCompletion(stc_usbn_t* pstcUsbHandle);
void UsbHostMassStorage_InTransferDone(void);
void UsbHostMassStorage_OutTransferDone(void);
void UsbHostMassStorage_CheckForCompletion(volatile uint8_t *NAKDetected);

StorageInfo_t* UsbHostMassStorage_GetGlobalStorageInfo(void);
int16_t UsbHostMassStorage_StorageCommandSend(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_SendCBW(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_Transmit_MSDC_Data(uint8_t *pu8Buffer, StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_Transmit_Data(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_GetCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_CheckCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_BulkOnlyMSReset(void);
int16_t UsbHostMassStorage_StorageStatus(void);
int16_t UsbHostMassStorage_ResetRecovery(void);
int16_t UsbHostMassStorage_ClearInEndpoint(void);
int16_t UsbHostMassStorage_ClearOutEndpoint(void);
void UsbHostMassStorage_GetEPStatus(uint8_t* InEpStatus, uint8_t* OutEpStatus);
int UsbHostMassStorage_TestUnitReady(StorageInfo_t *pstcStorageInfo);

void MassStorageWaitForSetupComplete(void);

boolean_t UsbHostMassStorage_DriverActive(void);

/*
void UsbHostMassStorage_BulkInCompletion(UMH_STATUS Status,unsigned int BytesTransferred,void* Context);
void UsbHostMassStorage_BulkOutCompletion(UMH_STATUS Status,unsigned int BytesTransferred,void* Context);


void UsbHostMassStorage_Init();
StorageInfo_t* UsbHostMassStorage_GetGlobalStorageInfo();

void UsbHostMassStorage_CheckForCompletion(uint8_t *NAKDetected);
void UsbHostMassStorage_InTransferDone(void);
void UsbHostMassStorage_OutTransferDone(void);
int16_t UsbHostMassStorage_StorageCommandSend(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_SendCBW(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_Transmit_MSDC_Data(uint8_t *pu8Buffer, StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_Transmit_Data(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_GetCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_CheckCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbHostMassStorage_BulkOnlyMSReset(void);
int16_t UsbHostMassStorage_StorageStatus(void);
int16_t UsbHostMassStorage_ResetRecovery(void);
int16_t UsbHostMassStorage_ClearInEndpoint(void);
int16_t UsbHostMassStorage_ClearOutEndpoint(void);
int UsbHostMassStorage_TestUnitReady(StorageInfo_t *pstcStorageInfo);
void UsbHostMassStorage_GetEPStatus(uint8_t* InEpStatus, uint8_t* OutEpStatus);


void UMH_StallExecutionUserCallback(unsigned long time);
void Idle(void);
*/
#endif /* (USE_USB_HOST == 1) */
#endif /* __USBMASSSTORAGE_H__*/
