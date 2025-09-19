/************************************************************************
 * $Id$ / 1.0 / 2012/9/3
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
/** \file UsbDescriptors.h
 **
 ** USB Descriptors File
 **
 ** History:
 **   - 2012-9-3  1.0  MSc  Automatically Created by FME USB Descriptors Manager 
 *****************************************************************************/

#ifndef __USBDESCRIPTORS_H__
#define __USBDESCRIPTORS_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/



/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USB_DEVDESC_SIZE 18
#define USB_CNFGDESC_SIZE 48
#define USB_RPTDESC_SIZE 0    /// only used with HID
#define USB_FUNC_EP0_SIZE 64
#define CLASSNAME "UsbDeviceCdcCom"


/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

#define USBDESCRIPTORS_STRINGDESCRIPTOR_COUNT (uint32_t)(sizeof(pstcStringDescriptors) / sizeof(pstcStringDescriptors[0]))
#define USBDESCRIPTORS_REPORTDESCRIPTOR_LENGTH (uint32_t)(sizeof(au8ReportDescriptor) / sizeof(au8ReportDescriptor[0]))




/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

const uint8_t au8DeviceDescriptor[18] = 
{
    ///DEVICE DESCRIPTOR
    0x12,    ///bLength: Length of this descriptor
    0x01,    ///bDescriptorType: Device Descriptor Type
    0x01,    ///bcdUSB: USB Version
    0x01,    ///bcdUSB: USB Version
    0x02,    ///bDeviceClass: Class Code: COMMUNICATIONS_AND_CDC_CONTROL
    0x00,    ///bDeviceSubClass: Sub Class Code
    0x00,    ///bDeviceProtocol: Protocol Code
    0x40,    ///bMaxPacketSize0: Maximum size of endpoint 0
    0x55,    ///idVendor: Vendor ID
    0x1F,    ///idVendor: Vendor ID
    0x04,    ///idProduct: Product ID
    0x00,    ///idProduct: Product ID
    0x00,    ///bcdDevice: Release Number
    0x02,    ///bcdDevice: Release Number
    0x01,    ///iManufacture: String-Index of Manufacture
    0x02,    ///iProduct: String-Index of Product
    0x03,    ///iSerialNumber: String-Index of Serial Number
    0x01    ///bNumConfigurations: Number of possible configurations
};

const uint8_t au8ConfigDescriptor[48] =
{        ///NEW CONFIG DESCRIPTOR(1)
        0x09,    ///bLength: Length of this descriptor
        0x02,    ///bDescriptorType: Config Descriptor Type
        0x30,    ///wTotalLength: Total Length with all interface- and endpoint descriptors
        0x00,    ///wTotalLength: Total Length with all interface- and endpoint descriptors
        0x02,    ///bNumInterfaces: Number of interfaces
        0x01,    ///iConfigurationValue: Number of this configuration
        0x00,    ///iConfiguration: String index of this configuration
        0xC0,    ///bmAttributes: Bus-Powered, Remote-Wakeup not supported
        0xFA,    ///MaxPower: (in 2mA)
            ///NEW INTERFACE DESCRIPTOR(0)
            0x09,    ///bLength: Length of this descriptor
            0x04,    ///bDescriptorType: Interface Descriptor Type
            0x00,    ///bInterfaceNumber: Interface Number
            0x00,    ///bAlternateSetting: Alternate setting for this interface
            0x01,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
            0x02,    ///iInterfaceClass: Class Code: COMMUNICATIONS_AND_CDC_CONTROL
            0x02,    ///iInterfaceSubClass: SubClass Code
            0x01,    ///bInterfaceProtocol: Protocol Code
            0x00,    ///iInterface: String index
                ///NEW ENDPOINT DESCRIPTOR(0)
                0x07,    ///bLength: Length of this descriptor
                0x05,    ///bDescriptorType: Endpoint Descriptor Type
                0x82,    ///bEndpointAddress: Endpoint address (IN,EP2)
                0x03,    ///bmAttributes: Transfer Type: INTERRUPT_TRANSFER
                0x40,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///wMaxPacketSize: Endpoint Size
                0xFF,    ///bIntervall: Polling Intervall
            ///NEW INTERFACE DESCRIPTOR(1)
            0x09,    ///bLength: Length of this descriptor
            0x04,    ///bDescriptorType: Interface Descriptor Type
            0x01,    ///bInterfaceNumber: Interface Number
            0x00,    ///bAlternateSetting: Alternate setting for this interface
            0x02,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
            0x0A,    ///iInterfaceClass: Class Code: CDC_DATA
            0x00,    ///iInterfaceSubClass: SubClass Code
            0x00,    ///bInterfaceProtocol: Protocol Code
            0x00,    ///iInterface: String index
                ///NEW ENDPOINT DESCRIPTOR(0)
                0x07,    ///bLength: Length of this descriptor
                0x05,    ///bDescriptorType: Endpoint Descriptor Type
                0x03,    ///bEndpointAddress: Endpoint address (OUT,EP3)
                0x02,    ///bmAttributes: Transfer Type: BULK_TRANSFER
                0x40,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///bIntervall: Polling Intervall
                ///NEW ENDPOINT DESCRIPTOR(1)
                0x07,    ///bLength: Length of this descriptor
                0x05,    ///bDescriptorType: Endpoint Descriptor Type
                0x81,    ///bEndpointAddress: Endpoint address (IN,EP1)
                0x02,    ///bmAttributes: Transfer Type: BULK_TRANSFER
                0x40,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///wMaxPacketSize: Endpoint Size
                0x00    ///bIntervall: Polling Intervall
};

const uint8_t au8ReportDescriptor[1]; // Not used

const stc_usbdevice_stringdescriptor_t pstcStringDescriptors[] =
{
{"Fujitsu Microelectronics",NULL},    //Manufacturer String
{"Virtual Comm Port",NULL},    //Product String
{"1.0",NULL},    //Serial Number String
};


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/



#endif /* __USBDESCRIPTORS_H__ */
