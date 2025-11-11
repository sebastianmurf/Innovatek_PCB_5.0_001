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
/** \file UsbClassSupervisor.h
 **
 ** Part of FSEU USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version
 **   - 2012-06-01    2.0  MSc  New Version for use with M3 L3 USB driver
 *****************************************************************************/
#ifndef __USBCLASSSUPERVISOR_H__
#define __USBCLASSSUPERVISOR_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/


#include "Usb.h"
#include "UsbHost.h"

#ifndef USB_DISBALE_HOST_FUNCTIONALITY


/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USBCLASSSVR_IDLE 0
#define USBCLASSSVR_REQUEST_DESCRIPTORS 1
#define USBCLASSSVR_CLASSCHECK 2
#define USBCLASSSVR_ERROR 3

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

typedef uint8_t UsbClassDriver_t;


typedef struct UsbClassHandler
{
    boolean_t (*InitializeClass)(stc_usbn_t* pstcUsb, uint8_t* pu8Configuration, uint32_t u32Length);
    boolean_t (*DeviceDisconnection)(stc_usbn_t* pstcUsb);
    uint16_t u16IdVendor;
    uint16_t u16IdProduct;
    uint8_t u8MatchDeviceClass;
    uint8_t u8MatchDeviceSubClass;
    uint8_t u8MatchDeviceProtocol;
    uint8_t u8MatchInterfaceClass;
    uint8_t u8MatchInterfaceSubClass;
    uint8_t u8MatchInterfaceProtocol;
    UsbClassDriver_t stcDriverType;
} UsbClassHandler_t;

typedef struct stc_usbclasssupervisor_intern_data
{
    UsbClassHandler_t stcMatchClass;
    uint8_t u8SupervisorState;
    uint8_t* pu8Configuration;
    uint32_t u32ConfigurationSize;
    UsbClassDriver_t CurrentDriver;
    void (*InternalErrorHandler)(stc_usbn_t* pstcUsb,UsbClassHandler_t stcMatchClass);  
} stc_usbclasssupervisor_intern_data_t;

typedef struct stc_usbclasssupervisorn_instance_data
{
    volatile stc_usbn_t*  pstcInstance;  ///< pointer to registers of an instance
    stc_usbclasssupervisor_intern_data_t stcInternData; ///< module internal data of instance
} stc_usbclasssupervisorn_instance_data_t;

#include "ClassDriverTable.h"

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/
extern const stc_usb_request_t stcDeviceDescriptorRequest;

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/
void UsbClassSupervisor_Init(stc_usbn_t* pstcUsb);
void UsbClassSupervisor_Deinit(stc_usbn_t* pstcUsb);
void UsbClassSupervisor_SysTickHandler(stc_usbn_t* pstcUsb);
boolean_t UsbSupervisor_UsbClassMatch(UsbClassHandler_t* stcA, UsbClassHandler_t* stcB);
void UsbClassSupervisor_SetErrorHandler(stc_usbn_t* pstcUsb,void (*Handler)(stc_usbn_t* pstcUsb,UsbClassHandler_t stcMatchClass));
UsbClassDriver_t UsbClassSupervisor_GetActiveDriver(stc_usbn_t* pstcUsb);

void ReturnedDeviceDescriptor(stc_usbn_t* pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataSize);
void ReturnedConfigurationDescriptor(stc_usbn_t* pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataSize);

#endif /* (USE_USB_HOST == 1) */
#endif /* __USBCLASSSUPERVISOR_H__ */

