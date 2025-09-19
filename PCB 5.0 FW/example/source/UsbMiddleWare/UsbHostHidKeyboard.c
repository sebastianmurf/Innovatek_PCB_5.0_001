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
/** \file UsbHostHidKeyboard.c
 **
 ** Part of FSEU USB Host Driver Module
 **
 ** History:
 **   - 2010-12-13    1.0  MSc  First version  (works with 16FX)
 **   - 2010-01-05    1.1  MSc  API Updates
 **   - 2011-03-30    1.2  MSc  Public Release
 **   - 2011-08-24    1.2  MSc  Bug fixes
 **   - 2012-06-05    1.4  MSc  New verison for use with new USB driver for FM3 L3
 **                             Rename HidKeyboard.c -> UsbHostHidKeyboard.c
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#define __USBHOSTHIDKEYBOARD_C__

#include "Usb.h"

#ifndef USB_DISBALE_HOST_FUNCTIONALITY
#include "UsbHostHidKeyboard.h"


/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


/*****************************************************************************/
/* function */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/
static stc_host_endpoint_t *EndpointIN, *EndpointOUT;
static uint8_t        u8MouseData[8];
static uint8_t        u8EndpointIN, u8EndpointOUT;
static uint16_t       u16MaxPackageSizeIN, u16MaxPackageSizeOUT;
static KeyboardData_t stcKeyboardData;
static uint8_t pu8OldData[8];
static boolean_t bKeyboardChanged = FALSE;
static stc_usbn_t* pstcUsbHandle = NULL;
void   (*KeyEventHandler)(KeyboardData_t *stcData);
static void UsbHostHidKeyboard_DataReceivedCallback(stc_usbn_t* pstcUsb);
const uint8_t u8Keycodes[] =
{ 
        0,    // 00
        0,    // 01
        0,    // 02
        0,    // 03
        'a',    // 04
        'b',    // 05
        'c',    // 06
        'd',    // 07
        'e',    // 08
        'f',    // 09
        'g',    // 0A
        'h',    // 0B
        'i',    // 0C
        'j',    // 0D
        'k',    // 0E
        'l',    // 0F
        'm',    // 10
        'n',    // 11
        'o',    // 12
        'p',    // 13
        'q',    // 14
        'r',    // 15
        's',    // 16
        't',    // 17
        'u',    // 18
        'v',    // 19
        'w',    // 1A
        'x',    // 1B
        'y',    // 1C
        'z',    // 1D
        '1',    // 1E
        '2',    // 1F
        '3',    // 20
        '4',    // 21
        '5',    // 22
        '6',    // 23
        '7',    // 24
        '8',    // 25
        '9',    // 26
        '0',    // 27
        10,    // 28    (RETURN)
        0x1B,    // 29  (ESC)
        0x08,    // 2A  (BACKSPACE)
        0x09,    // 2B  (TAB)
        ' ',    // 2C
        '-',    // 2D
        '=',    // 2E
        0,    // 2F
        0,    // 30
        0,    // 31
        0,    // 32
        0,    // 33
        0,    // 34
        '`',    // 35
        '<',    // 36
        '>',    // 37
        '/',    // 38
        0,    // 39
        0,    // 3A
        0,    // 3B
        0,    // 3C
        0,    // 3D
        0,    // 3E
        0,    // 3F
        0,    // 40
        0,    // 41
        0,    // 42
        0,    // 43
        0,    // 44
        0,    // 45
        0,    // 46
        0,    // 47
        0,    // 48
        0,    // 49
        0,    // 4A
        0,    // 4B
        0,    // 4C
        0,    // 4D
        0,    // 4E
        0,    // 4F (RIGHT)
        0,    // 50 (LEFT)
        0,    // 51 (DOWN)
        0,    // 52 (UP)
        0,    // 53 (NUM)
        '/',    // 54 
        '*',    // 55
        '-',    // 56
        '+',    // 57
        10,    // 58 (RETURN)
        '1',    // 59
        '2',    // 5A
        '3',    // 5B
        '4',    // 5C
        '5',    // 5D
        '6',    // 5E
        '7',    // 5F
        '8',    // 60
        '9',    // 61
        '0',    // 62
        '.',    // 63
        0,    // 64
        0,    // 65
        0,    // 66
        0,    // 67
        0,    // 68
        0,    // 69
        0,    // 6A
        0,    // 6B
        0,    // 6C
        0,    // 6D
        0,    // 6E
        0,    // 6F
        0,    // 70
        0,    // 71
        0,    // 72
        0,    // 73
        0,    // 74
        0,    // 75
        0,    // 76
        0,    // 77
        0,    // 78
        0,    // 79
        0,    // 7A
        0,    // 7B
        0,    // 7C
        0,    // 7D
        0,    // 7E
        0,    // 7F
        0,    // 80
        0,    // 81
        0,    // 82
        0,    // 83
        0,    // 84
        0,    // 85
        0,    // 86
        0,    // 87
        0,    // 88
        0,    // 89
        0,    // 8A
        0,    // 8B
        0,    // 8C
        0,    // 8D
        0,    // 8E
        0,    // 8F
        0,    // 90
        0,    // 91
        0,    // 92
        0,    // 93
        0,    // 94
        0,    // 95
        0,    // 96
        0,    // 97
        0,    // 98
        0,    // 99
        0,    // 9A
        0,    // 9B
        0,    // 9C
        0,    // 9D
        0,    // 9E
        0,    // 9F
        0,    // A0
        0,    // A1
        0,    // A2
        0,    // A3
        0,    // A4
        0,    // A5
        0,    // A6
        0,    // A7
        0,    // A8
        0,    // A9
        0,    // AA
        0,    // AB
        0,    // AC
        0,    // AD
        0,    // AE
        0,    // AF
        0,    // B0
        0,    // B1
        0,    // B2
        0,    // B3
        0,    // B4
        0,    // B5
        0,    // B6
        0,    // B7
        0,    // B8
        0,    // B9
        0,    // BA
        0,    // BB
        0,    // BC
        0,    // BD
        0,    // BE
        0,    // BF
        0,    // C0
        0,    // C1
        0,    // C2
        0,    // C3
        0,    // C4
        0,    // C5
        0,    // C6
        0,    // C7
        0,    // C8
        0,    // C9
        0,    // CA
        0,    // CB
        0,    // CC
        0,    // CD
        0,    // CE
        0,    // CF
        0,    // D0
        0,    // D1
        0,    // D2
        0,    // D3
        0,    // D4
        0,    // D5
        0,    // D6
        0,    // D7
        0,    // D8
        0,    // D9
        0,    // DA
        0,    // DB
        0,    // DC
        0,    // DD
        0,    // DE
        0,    // DF
        0,    // E0
        0,    // E1
        0,    // E2
        0,    // E3
        0,    // E4
        0,    // E5
        0,    // E6
        0,    // E7
        0,    // E8
        0,    // E9
        0,    // EA
        0,    // EB
        0,    // EC
        0,    // ED
        0,    // EE
        0,    // EF
        0,    // F0
        0,    // F1
        0,    // F2
        0,    // F3
        0,    // F4
        0,    // F5
        0,    // F6
        0,    // F7
        0,    // F8
        0,    // F9
        0,    // FA
        0,    // FB
        0,    // FC
        0,    // FD
        0,    // FE
        0     // FF
    };
     
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

boolean_t UsbHostHidKeyboard_InitHandler(stc_usbn_t* pstcUsb, uint8_t* pu8Configuration, uint32_t u32Length)
{
    uint8_t* pu8Buffer;
    uint8_t u8NumberOfInterfaces;
    uint8_t u8NumberOfEndpoints;
    uint8_t u8EndpointNumber;
    uint8_t u8InterfaceNumber;
    uint8_t u8IntervalIN, u8IntervalOUT;
    if (pstcUsbHandle != NULL)
    {
        return FALSE;
    }
    pstcUsbHandle = pstcUsb;
    
    u8EndpointIN = 0;
    u8EndpointOUT = 0;
    USBDBG("Yeah, called if a HID Device was connected ;-)\n");
    
    u8NumberOfInterfaces=pu8Configuration[4];
    for(u8InterfaceNumber=0;u8InterfaceNumber < u8NumberOfInterfaces;u8InterfaceNumber++)
    {
        USBDBGVAL8("Interface Index: ",u8InterfaceNumber);
        pu8Buffer = UsbHost_GetUsbInterfaceDescriptor(pu8Configuration,u32Length,u8InterfaceNumber);
        if (pu8Buffer == 0)
        {
            USBDBG("Error: Could not Request Interface Descriptor.\n");
            return FALSE;
        }
        u8NumberOfEndpoints = pu8Buffer[4];
        
        if ((pu8Buffer[6] == 0x01) && (pu8Buffer[7] == 0x1)) //SubClass & Protocol
        {
            for(u8EndpointNumber=1;u8EndpointNumber<=u8NumberOfEndpoints;u8EndpointNumber++)
            {
                USBDBGVAL8("Endpoint Index: ",u8EndpointNumber);
                pu8Buffer = UsbHost_GetUsbEndpointDescriptor(pu8Configuration,u32Length,u8InterfaceNumber,u8EndpointNumber);
                if (pu8Buffer[2] & USB_IN_DIRECTION)
                {
                    u8EndpointIN = pu8Buffer[2];
                    u16MaxPackageSizeIN = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                    u8IntervalIN = pu8Buffer[6]; 
                }
                else
                {
                    u8EndpointOUT = pu8Buffer[2];
                    u16MaxPackageSizeOUT = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                    u8IntervalOUT = pu8Buffer[6]; 
                }
                if ((u8EndpointIN != 0) && (u8EndpointOUT != 0))
                {
                    break;
                }
            }
            if (u8EndpointIN == 0)
            {
                USBDBG("Hm, error while resolving endpoints...");
                return FALSE;
            }


            if (u8EndpointIN != 0)
            {
                UsbHost_AddHostEndpoint(pstcUsbHandle, &EndpointIN,u8EndpointIN,u16MaxPackageSizeIN,u8IntervalIN,UsbHostHidKeyboard_DataReceivedCallback);
                EndpointIN->pu8Buffer = u8MouseData;
                EndpointIN->u32DataSize = 8;
            } 
            if (u8EndpointOUT != 0)
            {
                UsbHost_AddHostEndpoint(pstcUsbHandle, &EndpointOUT,u8EndpointOUT,u16MaxPackageSizeOUT,u8IntervalOUT,0);
            }       
            USBDBGVAL8("Endpoint-IN  Addr: ",u8EndpointIN);
            USBDBGVAL8("Endpoint-IN  Size: ",u16MaxPackageSizeIN);
            break;
        }
    }     
    if (u8EndpointIN == 0)
    {
        USBDBG("Hm, error while resolving endpoints...");
        return FALSE;
    }
    return TRUE;                       
}

boolean_t UsbHostHidKeyboard_DeinitHandler(stc_usbn_t* pstcUsb)
{
    pstcUsbHandle = NULL;
    return TRUE;
}


boolean_t UsbHostHidKeyboard_DriverActive(void)
{
    return (pstcUsbHandle != NULL);
}

static void UsbHostHidKeyboard_DataReceivedCallback(stc_usbn_t* pstcUsb)
{
    uint8_t i = 0;
    boolean_t bExecuteEvent = FALSE;
    for(i=0;i<8;i++)
    {
        if ((bExecuteEvent == FALSE) && ((EndpointIN->pu8Buffer[i] != 0) || ((EndpointIN->pu8Buffer[i] == 0) && (pu8OldData[i] != EndpointIN->pu8Buffer[i]))))
        {
             bExecuteEvent = TRUE;
        }
        pu8OldData[i] = EndpointIN->pu8Buffer[i];
    }

    stcKeyboardData.u8ModifierKeys = EndpointIN->pu8Buffer[0];
    stcKeyboardData.u8Reserved     = EndpointIN->pu8Buffer[1];
    stcKeyboardData.u8KeyCode1     = EndpointIN->pu8Buffer[2];
    stcKeyboardData.u8KeyCode2     = EndpointIN->pu8Buffer[3];
    stcKeyboardData.u8KeyCode3     = EndpointIN->pu8Buffer[4];
    stcKeyboardData.u8KeyCode4     = EndpointIN->pu8Buffer[5];
    stcKeyboardData.u8KeyCode5     = EndpointIN->pu8Buffer[6];
    stcKeyboardData.u8KeyCode6     = EndpointIN->pu8Buffer[7];
    bKeyboardChanged = bExecuteEvent;
    if ((bExecuteEvent == TRUE) && (KeyEventHandler != 0))
    {
        KeyEventHandler(&stcKeyboardData);
    }
    EndpointIN->CompletionHandler = UsbHostHidKeyboard_DataReceivedCallback;
    
}

void UsbHostHidKeyboard_SetCallback(void (*Handler)(KeyboardData_t *stcData))
{
    KeyEventHandler = Handler;
}

void UsbHostHidKeyboard_RemoveCallback(void)
{
        KeyEventHandler = 0;
}

char_t UsbHostHidKeyboard_GetCh(volatile uint32_t u32TimeOut)
{
    char_t c;
    while ((bKeyboardChanged == FALSE) && (u32TimeOut > 0))
    {
        u32TimeOut--;
        if ((UsbHost_GetDeviceStatus(pstcUsbHandle) & USBHOST_DEVICE_CONFIGURED) == 0)
        {
            return 0;
        }
    }
    if (u32TimeOut == 0) return 0;
        
    while((stcKeyboardData.u8KeyCode1 == 0) && (u32TimeOut > 0))
    {
        u32TimeOut--;
        if ((UsbHost_GetDeviceStatus(pstcUsbHandle) & USBHOST_DEVICE_CONFIGURED) == 0)
        {
            return 0;
        }
    }
    if (u32TimeOut == 0) return 0;
    
    c = (char_t)KEYCODE_ASCII(stcKeyboardData.u8KeyCode1);
    if (stcKeyboardData.u8KeyCode2 != 0)
    {
        c = (char_t)KEYCODE_ASCII(stcKeyboardData.u8KeyCode2);
        while((stcKeyboardData.u8KeyCode2 != 0) && (u32TimeOut > 0))
        {
            u32TimeOut--;
            if ((UsbHost_GetDeviceStatus(pstcUsbHandle) & USBHOST_DEVICE_CONFIGURED) == 0)
            {
                return 0;
            }
        }
        if (u32TimeOut == 0) return 0;
    }
    if ((stcKeyboardData.u8ModifierKeys & 0x22))
    {
        if ((c >= 'a') && (c <= 'z'))
        {
            c += 'A' - 'a';
        }
        while((stcKeyboardData.u8ModifierKeys & 0x22) && (u32TimeOut > 0))
        {
            u32TimeOut--;
            if ((UsbHost_GetDeviceStatus(pstcUsbHandle) & USBHOST_DEVICE_CONFIGURED) == 0)
            {
                return 0;
            }
        }
        if (u32TimeOut == 0) return 0;
    }
    bKeyboardChanged = FALSE;
    return c;
}




/* NOT USABLE 
uint8_t OutData[4];

void HidKeyboard_SetLeds(uint8_t u8Data)
{
    UsbRequest_t stcRequst;
    stcRequst.bmRequestType = 0x21;
    stcRequst.bRequest = 0x09;
    stcRequst.wValue = 0x0002;
    stcRequst.wIndex = 0x00;
    stcRequst.wLength = 0x0004;
    OutData[0] = u8Data;
    UsbHost_SetupRequestWithData(&stcRequst,OutData,0);
}
*/
#endif /* (USE_USB_HOST == 1) */
