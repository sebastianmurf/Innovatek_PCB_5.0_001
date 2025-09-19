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
/* It is the user’s obligation to fully test the software in its        */
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
/* To the maximum extent permitted by applicable law, FME’s liability   */
/* is restricted to intention and gross negligence.                     */
/* FME is not liable for consequential damages.                         */
/*                                                                      */
/* (V1.3)                                                               */
/*****************************************************************************/
/** \file UsbDeviceCdcCom.h
 **
 ** CONST USB_ASSISTANT_OFF
 **
 ** USB Descriptors File
 **
 ** History:
 **   - 2009-08-31    1.0  MSc  First version  (works with 16FX)
 **   - 2010-04-16    1.1  MSc  new USB library added - API had changed
 **   - 2011-08-30    1.2  MSc  bug fixes while echo data
 **                             added UsbDeviceCdcCom_SetEchoAtSeperatorOnly for
 **                             echo data only after seperator received
 **   - 2012-07-24    1.3  MSc  Version for USB Library >= FM3 
 *****************************************************************************/

#ifndef __USBDEVICECDCCOM_H__
#define __USBDEVICECDCCOM_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "usb.h"

#ifndef USB_DISBALE_DEVICE_FUNCTIONALITY

#include "base_types_l3.h"
#include "UsbDevice.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define  USBDEVICECDCCOM_EPOUT_BUFFERSIZE   128  //buffer Size by default 512 bytes
#define  USBDEVICECDCCOM_MAX_RECEIVE_BUFFER 512

#define  USBDEVICECDCCOM_USE_UARTCOMMANDS    0


/* CDC Management Element Requests */
#define SEND_ENCAPSULATED_COMMAND 0x00
#define GET_ENCAPSULATED_RESPONSE 0x01
#define SET_COMM_FEATURE 0x02
#define GET_COMM_FEATURE 0x03
#define CLEAR_COMM_FEATURE 0x04
#define SET_AUX_LINE_STATE 0x10
#define SET_HOOK_STATE 0x11
#define PULSE_SETUP 0x12
#define SEND_PULSE 0x13
#define SET_PULSE_TIME 0x14
#define RING_AUX_JACK 0x15
#define SET_LINE_CODING 0x20
#define GET_LINE_CODING 0x21
#define SET_CONTROL_LINE_STATE 0x22
#define SEND_BREAK 0x23
#define SET_RINGER_PARMS 0x30
#define GET_RINGER_PARMS 0x31
#define SET_OPERATION_PARMS 0x32
#define GET_OPERATION_PARMS 0x33
#define SET_LINE_PARMS 0x34
#define GET_LINE_PARMS 0x35
#define DIAL_DIGITS 0x36
#define SET_UNIT_PARAMETER 0x37
#define GET_UNIT_PARAMETER 0x38
#define CLEAR_UNIT_PARAMETER 0x39
#define GET_PROFILE 0x3A
#define SET_ETHERNET_MULTICAST_FILTERS 0x40
#define SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x41
#define GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x42
#define SET_ETHERNET_PACKET_FILTER 0x43
#define GET_ETHERNET_STATISTIC 0x44
#define SET_ATM_DATA_FORMAT 0x50
#define GET_ATM_DEVICE_STATISTICS 0x51
#define SET_ATM_DEFAULT_VC 0x52
#define GET_ATM_VC_STATISTICS 0x53


#define DEC 0
#define HEX 1


#if (USBDEVICECDCCOM_USE_UARTCOMMANDS == 1)
   #define puts(x)    UsbDeviceCdcCom_SendString(x)
   #define putch(x)    UsbDeviceCdcCom_SendByte((uint8_t)x)
   #define puthex(x,y)    UsbDeviceCdcCom_SendHex(x,y)
   #define putdec(x)    UsbDeviceCdcCom_SendDec(x)
   #define getch()    ((char_t)UsbDeviceCdcCom_ReceiveByte())
   #define Inputhex(x) UsbDeviceCdcCom_ReceiveHex(x)
   #define binasci8(x,y,z)    UsbDeviceCdcCom_SendBinAsci8(x,y,z);
   #define binasci4(x,y,z)    UsbDeviceCdcCom_SendBinAsci4(x,y,z);
   #define bin2_dot_dec99(x)    UsbDeviceCdcCom_Send_Bin2DotDec99(x);
#endif

#define USBCLASSCDC_CHARFORMAT_1STOPBIT   0
#define USBCLASSCDC_CHARFORMAT_1_5STOPBIT 1
#define USBCLASSCDC_CHARFORMAT_2STOPBIT   2

#define USBCLASSCDC_PARITYTYPE_NONE       0
#define USBCLASSCDC_PARITYTYPE_ODD        1
#define USBCLASSCDC_PARITYTYPE_EVEN       2
#define USBCLASSCDC_PARITYTYPE_MARK       3
#define USBCLASSCDC_PARITYTYPE_SPACE      4

typedef struct stc_cdc_linecoding
{
    volatile uint32_t dwDTERate;
    volatile uint8_t bCharFormat;
    volatile uint8_t bParityType;
    volatile uint8_t bDataBits;
} stc_cdc_linecoding_t ;

typedef void (*usbdevicecdccom_linecodingchange_func_ptr_t)(stc_cdc_linecoding_t* pstcLineCoding);

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/



/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/



/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/



void UsbDeviceCdcCom_Init(stc_usbn_t* pstcUsb);
boolean_t UsbDeviceCdcCom_DataSent(void);
boolean_t UsbDeviceCdcCom_IsConnected(void); 
uint8_t UsbDeviceCdcCom_SendByte(char_t c); 
uint8_t UsbDeviceCdcCom_SendString(char_t *pcBuffer); 
uint8_t UsbDeviceCdcCom_SendBuffer(uint8_t *pu8Buffer, uint32_t u32Size); 
void UsbDeviceCdcCom_SetSeparator(const char_t cNewSeperator);
void UsbDeviceCdcCom_SetEchomode(boolean_t bEchoOnOff); 
void UsbDeviceCdcCom_SetEchoAtSeperatorOnly(boolean_t bEchoAtSeperatorOnlyOnOff);
uint32_t UsbDeviceCdcCom_ReceivedLength(void);
uint32_t UsbDeviceCdcCom_ReceiveBuffer(uint8_t *pu8Buffer); 
uint8_t UsbDeviceCdcCom_ReceiveByte(void);
uint8_t UsbDeviceCdcCom_ReceiveLastByte(void); 
uint8_t UsbDeviceCdcCom_SendHex(uint32_t n, uint8_t digits);
uint8_t UsbDeviceCdcCom_SendDec(uint32_t x);
void UsbDeviceCdcCom_SendBinAsci4(uint16_t a, uint8_t base, char_t fill);  
void UsbDeviceCdcCom_SendBinAsci8(uint16_t a, uint8_t base, char_t fill);
void UsbDeviceCdcCom_SendBin2DotDec99(uint8_t a);
uint32_t UsbDeviceCdcCom_ReceiveHex(uint8_t digits);


#endif /* USB_DISBALE_DEVICE_FUNCTIONALITY */

#endif /* __USBCLASS_H__*/
