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
/** \file UsbDevice.h
 **
 ** A detailed description is available at 
 ** @link UsbDeviceGroup USB Device Module description @endlink
 **
 ** History:
 **   - 2012-08-28  2.0  MSc  First version (starting at version 2.0).
 **
 ******************************************************************************/

#ifndef __USBDEVICE_H__
#define __USBDEVICE_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup UsbDeviceGroup USB Device Mode Functionality
 **
 ** Provided functions of USB module:
 ** 
 ** - UsbDevice_ReceiveData()
 ** - UsbDevice_SendData()
 ** - UsbDevice_SendDataControl()
 ** - UsbDevice_ReceiveDataControl()
 ** - UsbDevice_RegisterVendorClass()
 ** - UsbDevice_GetStatus()
 ** - UsbDevice_InitCallback()
 ** - UsbDevice_Init()
 ** - UsbDevice_SetupEndpoint()
 **
 ** Used to transfer data with EP0..EP5, to do enumeration process and support different USB class drivers.
 **
 ******************************************************************************/
//@{


#include "usb.h"

#define USBDEVICE_MAXCLASSES 5

/**
 ******************************************************************************
 ** \brief setup request callback handle
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcSetup Pointer to setup request
 **
 ******************************************************************************/
typedef void (*usbdevice_setuprequest_func_ptr_t)(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup);

/**
 ******************************************************************************
 ** \brief control data transferred
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcEndpoint Endpoint handle
 **
 ******************************************************************************/
typedef void (*usbdevice_controltranfered_func_ptr_t)(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint);

/**
 ******************************************************************************
 ** \brief init class callback
 **
 ** \param pstcUsb USB handle
 **
 ******************************************************************************/
typedef void (*usbdevice_initclasses_func_ptr_t)(stc_usbn_t* pstcUsb);



/**
 ******************************************************************************
 ** \brief USB device class configuration
 **
 ** used to setup USB classes
 **
 ******************************************************************************/
typedef struct stc_usbdevice_class_config
{
    uint8_t u8InterfaceNumber;                                  ///< Interface Number, set to 0xFF, if automatically search for
    uint8_t u8InterfaceClass;                                   ///< Interface Class
    uint8_t u8InterfaceSubClass;                                ///< Interface Sub Class
    uint8_t u8InterfaceProtocoll;                               ///< Interface Protocoll
    usbdevice_setuprequest_func_ptr_t pfnSetupRequestCallback;  ///< Callback for setup requests
    usb_connect_func_ptr_t pfnConnectionCallback;               ///< Callback on device connect
    usb_disconnect_func_ptr_t pfnDisconnectionCallback;         ///< Callback on device disconnect
    usb_conf_isr_func_ptr_t pfnConfCallback;                    ///< Callback on device configuration changed
} stc_usbdevice_class_config_t;

/**
 ******************************************************************************
 ** \brief USB device class configuration
 **
 ** used to setup USB classes
 **
 ******************************************************************************/
typedef struct stc_usbdevice_intern_data
{
  en_usb_status_t enStatus;                                              ///< USB HAL status
  uint8_t* pu8DeviceDescriptor;                                          ///< Pointer to USB device descriptor
  uint8_t* pu8ConfigDescriptor;                                          ///< Pointer to USB configuration descriptor
  uint8_t* pu8ReportDescriptor;                                          ///< Pointer to USB report descriptor
  uint16_t u16ReportDescriptorSize;                                      ///< Size of USB report descriptor
  stc_usbdevice_stringdescriptor_t* pstcStringDescriptors;               ///< Pointer to USB string descriptors
  uint8_t u8StringDescriptorCount;                                       ///< Number of USB string descriptors
  stc_usbn_endpoint_data_t* pstcEp0OUT;                                  ///< Pointer of Endpoint 0 OUT handle
  stc_usbn_endpoint_data_t* pstcEp0IN;                                   ///< Pointer of Endpoint 0 IN handle
  en_usb_control_stage_t enControlStage;                                 ///< Control endpoint stage
  uint8_t pu8ControlReceiveBuffer[64];                                   ///< Buffer for control endpoint
  stc_usbn_endpoint_buffer_t stcEndpointBuffer;                          ///< Buffer struct of endpoint buffer
  stc_usbdevice_class_config_t stcUsbClasses[USBDEVICE_MAXCLASSES];      ///< USB class configurations
  usbdevice_initclasses_func_ptr_t pfnInitClassesCallback;               ///< Callback to initialize classes
  usbdevice_controltranfered_func_ptr_t pfnControlTransferred;           ///< Callback control data transferred
} stc_usbdevice_intern_data_t;

/**
 ******************************************************************************
 ** \brief USB device configuration
 **
 ******************************************************************************/
typedef struct stc_usbdevice_config
{
    uint8_t* pu8DeviceDescriptor;                            ///< Pointer to USB device descriptor
    uint8_t* pu8ConfigDescriptor;                            ///< Pointer to USB configuration descriptor
    uint8_t* pu8ReportDescriptor;                            ///< Pointer to USB report descriptor
    uint16_t u16ReportDescriptorSize;                        ///< Report descriptor size
    stc_usbdevice_stringdescriptor_t* pstcStringDescriptors; ///< Pointer to USB string descriptor
    uint8_t u8StringDescriptorCount;                         ///< Number of string descriptors
    usbdevice_initclasses_func_ptr_t pfnInitClassesCallback; ///< Class initialization callback
} stc_usbdevice_config_t;


/**
 ******************************************************************************
 ** \brief USB device endpoint configuration
 **
 ******************************************************************************/
typedef struct stc_usbdevice_endpoint_config
{
  uint8_t u8EndpointAddress;                                        ///< Endpointaddress
  volatile usb_endpoint_datatransferred_func_ptr_t pfnRxTxCallback; ///< Data transferred callback
  stc_usbn_endpoint_buffer_t* pstcEndpointBuffer;                   ///< Pointer to endpoint buffer struct
} stc_usbdevice_endpoint_config_t;



/**
 ******************************************************************************
 ** \brief USB device instance
 **
 ******************************************************************************/
typedef struct stc_usbdevicen_instance_data
{
    volatile stc_usbn_t*  pstcInstance;  ///< pointer to registers of an instance
    stc_usbdevice_intern_data_t stcInternData; ///< module internal data of instance
} stc_usbdevicen_instance_data_t;

/**
 ******************************************************************************
 ** \brief USB device sending mode
 **
 ******************************************************************************/
typedef enum en_usbsend_mode
{
  UsbPOLL  = 0,  ///< send polled
  UsbIRQ   = 1,  ///< send IRQ
  UsbDMA   = 2,  ///< send DMA
} en_usbsend_mode_t;




en_result_t UsbDevice_ReceiveData(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint, uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode);

en_result_t UsbDevice_SendData(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint, uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode);

void UsbDevice_SendDataControl(stc_usbn_t * pstcUsb,uint8_t* pu8Buffer, uint32_t u32DataLength);

void UsbDevice_ReceiveDataControl(stc_usbn_t * pstcUsb, usbdevice_controltranfered_func_ptr_t pfnCompletionCallback);

en_result_t UsbDevice_RegisterVendorClass(stc_usbn_t* pstcUsb, stc_usbdevice_class_config_t* pstcUsbClassConfig, uint8_t** ppu8Buffer, uint8_t* pu8InterfaceLength);

en_usb_status_t UsbDevice_GetStatus(stc_usbn_t* pstcUsb);

void UsbDevice_InitCallback(stc_usbn_t* pstcUsb, stc_usb_config_t* pstcUsbConfig);

void UsbDevice_Init(stc_usbn_t* pstcUsb, stc_usb_config_t* pstcUsbConfig, stc_usbdevice_config_t* pstcConfig);

stc_usbn_endpoint_data_t* UsbDevice_SetupEndpoint(stc_usbn_t* pstcUsb, stc_usbdevice_endpoint_config_t* pstcConfig);


#if (L3_PERIPHERAL_ENABLE_USB0 == L3_ON)
    void UsbDevice_Usb0ExintCallback(void);
#endif
#if (L3_PERIPHERAL_ENABLE_USB1 == L3_ON)
    void UsbDevice_Usb1ExintCallback(void);
#endif
#ifdef __cplusplus
}
#endif
//@} // UsbDeviceGroup
#endif
