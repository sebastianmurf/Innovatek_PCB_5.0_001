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
/** \file UsbDeviceCdcCom.c
 **
 ** CONST USB_ASSISTANT_OFF
 **
 **
 ** History:
 **   - 2009-08-31    1.0  MSc  First version  (works with 16FX)
 **   - 2010-04-16    1.1  MSc  new USB library added - API had changed
 **   - 2011-08-30    1.2  MSc  bug fixes while echo data
 **                             added UsbDeviceCdcCom_SetEchoAtSeperatorOnly for
 **                             echo data only after seperator received
 **   - 2012-07-24    1.3  MSc  Version for USB Library >= FM3 
 *****************************************************************************/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "UsbDeviceCdcCom.h"

#ifndef USB_DISBALE_DEVICE_FUNCTIONALITY

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/



/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

static const char_t pcASCII[] = "0123456789ABCDEF";

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/



/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/

static volatile uint8_t DataReceivedFlags = 0; //Status for received data
static volatile uint8_t DataSentFlags = 0; //Status for sent data
static uint8_t pu8VcommBuffer[USBDEVICECDCCOM_MAX_RECEIVE_BUFFER];
static stc_cdc_linecoding_t stcLineCoding = {9600,USBCLASSCDC_CHARFORMAT_1STOPBIT,USBCLASSCDC_PARITYTYPE_NONE,8};
static uint8_t pu8LineCoding[8];
static boolean_t bEchoMode = FALSE;
static boolean_t bEchoAtSeperatorOnly = FALSE;
static boolean_t bVcommActive = FALSE;
static boolean_t bSeperatorFlag = FALSE;
static char_t cSeperator = '\0';
static volatile uint32_t u32ReceivedData = 0;
static uint8_t pu8IngoingBuffer[USBDEVICECDCCOM_EPOUT_BUFFERSIZE];
static boolean_t bReady = FALSE;
static stc_usbn_t* pstcUsbHandle = NULL;
static stc_usbn_endpoint_data_t* pstcEndpointIN;
static stc_usbn_endpoint_data_t* pstcEndpointOUT;
static stc_usbn_endpoint_buffer_t stcEndpointBufferIN;
static stc_usbn_endpoint_buffer_t stcEndpointBufferOUT;
static usbdevicecdccom_linecodingchange_func_ptr_t pfnLineCodingChange;
/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/
static void ConfCallback(stc_usbn_t* pstcUsb);
static void RxTxCallback(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint);
static boolean_t SendData(uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode);
static void DecodeSetupRequest(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup);
static void SetLineCoding(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint);
static void SendLineCoding(void);
static uint32_t ASCIItobin(uint8_t k);
/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

/**
 ******************************************************************************
 ** \brief Configuration change callback, used to see if configuration is set
 **        or cleared
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return none     
 **
 ******************************************************************************/
static void ConfCallback(stc_usbn_t* pstcUsb)
{
    if (UsbDevice_GetStatus(pstcUsb) == UsbConfigured)
    {
        bReady = TRUE;
    } else
    {
        bReady = FALSE;
    }
}

/**
 ******************************************************************************
 ** \brief Receive or transmitted data callback
 **
 ** \param pstcUsb         Pointer to USB instance
 **
 ** \param pstcEndpoint    Pointer to endpoint instance
 **
 ** \return none     
 **
 ******************************************************************************/
static void RxTxCallback(stc_usbn_t* pstcUsb, struct stc_usbn_endpoint_data* pstcEndpoint)
{
    volatile uint32_t u32i;
    uint8_t* pu8Buffer = pstcEndpoint->pstcEndpointBuffer->pu8Buffer;
    uint32_t u32DataSize = pstcEndpoint->pstcEndpointBuffer->u32DataSize;
    
    if ((pstcEndpoint->u8EndpointAddress) & 0x80)
    {
        DataSentFlags |= (1<<1);
    } 
    else
    {
        DataReceivedFlags |= (1<<3); // setting data received flag

            
        if ((bEchoMode) && (bVcommActive) && (bEchoAtSeperatorOnly == FALSE))
        {
            UsbDevice_SendData(pstcUsbHandle, pstcEndpointIN, pu8Buffer, u32DataSize, UsbPOLL);
        }
        for(u32i=0;u32i<u32DataSize;u32i++)
        {
            if ((cSeperator != '\0') && (*pu8Buffer == cSeperator)) {
                bSeperatorFlag = TRUE;
            }
            if (u32ReceivedData < USBDEVICECDCCOM_MAX_RECEIVE_BUFFER) 
            {
                pu8VcommBuffer[u32ReceivedData++] = *pu8Buffer++; 
            }
            if ((cSeperator != '\0') && (bEchoMode) && (bVcommActive) && (bEchoAtSeperatorOnly) && (bSeperatorFlag))
            {
                UsbDevice_SendData(pstcUsbHandle, pstcEndpointIN, (uint8_t *)pu8VcommBuffer, u32ReceivedData, UsbPOLL);
            }
        }
        UsbDevice_ReceiveData(pstcUsb, pstcEndpoint, pstcEndpoint->pstcEndpointBuffer->pu8Buffer, pstcEndpoint->pstcEndpointBuffer->u32BufferSize, UsbIRQ);
    }
}


/**
 ******************************************************************************
 ** \brief Initialisation callback
 **
 ** \param pstcUsb         Pointer to USB instance
 **
 ** \return none     
 **
 ******************************************************************************/
void UsbDeviceCdcCom_Init(stc_usbn_t* pstcUsb)
{
    stc_usbdevice_class_config_t stcUsbClassConfig;
    stc_usbdevice_endpoint_config_t stcEndpointConfig;
    uint8_t* pu8Interface = NULL;
    uint8_t u8InterfaceLength = 0;
    uint8_t u8i = 0;
    uint8_t u8NumEndpoints = 0;
    pstcUsbHandle = pstcUsb;
    
    stcUsbClassConfig.u8InterfaceNumber          = 0xFF;                          // do not use fix interface number, choose by class, subclass, protocol
    stcUsbClassConfig.u8InterfaceClass           = 0x02;                          // CDC Class
    stcUsbClassConfig.u8InterfaceSubClass        = 0x02;                          // Custom Sub Class
    stcUsbClassConfig.u8InterfaceProtocoll       = 0x01;                          // Custom Protocol
    stcUsbClassConfig.pfnSetupRequestCallback    = DecodeSetupRequest;            // setup requests handled
    stcUsbClassConfig.pfnConnectionCallback      = NULL;                          // No connection callback handled
    stcUsbClassConfig.pfnDisconnectionCallback   = NULL;                          // No disconnection callback handled
    stcUsbClassConfig.pfnConfCallback            = ConfCallback;                  // Callback for configuration set
    UsbDevice_RegisterVendorClass(pstcUsb,&stcUsbClassConfig,&pu8Interface,&u8InterfaceLength);

    stcUsbClassConfig.u8InterfaceNumber          = 0xFF;                          // do not use fix interface number, choose by class, subclass, protocol
    stcUsbClassConfig.u8InterfaceClass           = 0x0A;                          // CDC Class
    stcUsbClassConfig.u8InterfaceSubClass        = 0x00;                          // Custom Sub Class
    stcUsbClassConfig.u8InterfaceProtocoll       = 0x00;                          // Custom Protocol
    stcUsbClassConfig.pfnSetupRequestCallback    = DecodeSetupRequest;            // setup requests handled
    stcUsbClassConfig.pfnConnectionCallback      = NULL;                          // No connection callback handled
    stcUsbClassConfig.pfnDisconnectionCallback   = NULL;                          // No disconnection callback handled
    stcUsbClassConfig.pfnConfCallback            = ConfCallback;                  // Callback for configuration set
    UsbDevice_RegisterVendorClass(pstcUsb,&stcUsbClassConfig,&pu8Interface,&u8InterfaceLength);
        
    for(u8i = 0;u8i < u8InterfaceLength;)
    {
        u8i += pu8Interface[u8i];
        if (USBDESCR_ENDPOINT == pu8Interface[u8i + 1])
        {
            stcEndpointConfig.u8EndpointAddress = pu8Interface[u8i + 2];
            stcEndpointConfig.pfnRxTxCallback = RxTxCallback;
            if (((stcEndpointConfig.u8EndpointAddress) & 0x80) == 0)
            {
            
                stcEndpointConfig.pstcEndpointBuffer = &stcEndpointBufferOUT;
                pstcEndpointOUT = UsbDevice_SetupEndpoint(pstcUsb, &stcEndpointConfig);

                UsbDevice_ReceiveData(pstcUsb, pstcEndpointOUT, pu8IngoingBuffer, USBDEVICECDCCOM_EPOUT_BUFFERSIZE, UsbIRQ);
            }
            else
            {
                stcEndpointConfig.pstcEndpointBuffer = &stcEndpointBufferIN;
                pstcEndpointIN = UsbDevice_SetupEndpoint(pstcUsb, &stcEndpointConfig);
            }
            u8NumEndpoints++;
        }
    }
    
}


/**
 ******************************************************************************
 ** Is called from main application to send data via endpoint 1
 **
 ** \param pu8Buffer Buffer to send
 ** \param u32DataSize Buffersize
 ** \param enMode Sending mode: UsbPOLL, UsbIRQ or UsbDMA;
 **
 ** \return TRUE: if succesful, FALSE: if usb was not ready
 *****************************************************************************/
static boolean_t SendData(uint8_t* pu8Buffer, uint32_t u32DataSize, en_usbsend_mode_t enMode)
{
    if (UsbDevice_GetStatus(pstcUsbHandle) == UsbConfigured)
    {
        DataSentFlags &= ~(1<<1);
		
	UsbDevice_SendData(pstcUsbHandle, pstcEndpointIN, pu8Buffer, u32DataSize, enMode);
        if (enMode == UsbPOLL)
        {
            DataSentFlags |= (1<<1);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 ******************************************************************************
 ** Check if data was sent
 **
 ** \return TRUE if data was sent
 *****************************************************************************/
boolean_t UsbDeviceCdcCom_DataSent(void)
{
    if ((DataSentFlags & (1<<1)) > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 ******************************************************************************
 ** Set callback function for LineCoding changed event
 **
 ** \param pfnLineCodingChangeCallback  Callback handler
 *****************************************************************************/
void UsbDeviceCdcCom_SetCallbackLineCodingChange(usbdevicecdccom_linecodingchange_func_ptr_t pfnLineCodingChangeCallback)
{
    pfnLineCodingChange = pfnLineCodingChangeCallback;
}

/**
 ******************************************************************************
 ** Decode setup request (class specific) handler
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcSetup pointer of setup request
 *****************************************************************************/
static void DecodeSetupRequest(stc_usbn_t* pstcUsb, stc_usb_request_t* pstcSetup)
{
    switch (pstcSetup->bRequest)
    {
        case SET_LINE_CODING:
            UsbDevice_ReceiveDataControl(pstcUsb,SetLineCoding);
            return;
        case GET_LINE_CODING:
            SendLineCoding();
            UsbDevice_SendDataControl(pstcUsbHandle,(uint8_t *)NULL, 0);
            return;
        case SET_CONTROL_LINE_STATE:
            if (pstcSetup->wValue & 0x01)
            {
                bVcommActive = TRUE;
            } 
            else 
            {
                bVcommActive = FALSE;
            }  
            if (pstcSetup->wValue & 0x02) 
            {

            } 
            else
            {

            }
            UsbDevice_SendDataControl(pstcUsbHandle,(uint8_t *)NULL, 0);
            return;
       default:
            return;
    }   
}

/**
 ******************************************************************************
 ** callback for data package of setup request "SET_LINE_CODING"
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcEndpoint endpoint handle of received data
 **
 ** \return none
 *****************************************************************************/
static void SetLineCoding(stc_usbn_t* pstcUsb, stc_usbn_endpoint_data_t* pstcEndpoint)
{
   uint8_t* pu8Buffer = pstcEndpoint->pstcEndpointBuffer->pu8Buffer;
   stcLineCoding.dwDTERate = 0;
   stcLineCoding.dwDTERate += ((uint32_t)pu8Buffer[3]) << 24;
   stcLineCoding.dwDTERate += ((uint32_t)pu8Buffer[2]) << 16;
   stcLineCoding.dwDTERate += ((uint32_t)pu8Buffer[1]) <<  8;
   stcLineCoding.dwDTERate += ((uint32_t)pu8Buffer[0]) <<  0;
   stcLineCoding.bCharFormat = pu8Buffer[4];
   stcLineCoding.bParityType = pu8Buffer[5];
   stcLineCoding.bDataBits   = pu8Buffer[6];
   if (pfnLineCodingChange != NULL)
   {
       pfnLineCodingChange(&stcLineCoding);
   }
}
/**
 ******************************************************************************
 ** answer for request "GET_LINE_CODING"
 **
 ** \param none
 **
 ** \return none
 *****************************************************************************/
static void SendLineCoding(void) 
{
    pu8LineCoding[3] = (uint8_t)((stcLineCoding.dwDTERate >> 24) & 0xFF);
    pu8LineCoding[2] = (uint8_t)((stcLineCoding.dwDTERate >> 16) & 0xFF);
    pu8LineCoding[1] = (uint8_t)((stcLineCoding.dwDTERate >>  8) & 0xFF);
    pu8LineCoding[0] = (uint8_t)((stcLineCoding.dwDTERate >>  0) & 0xFF);
    pu8LineCoding[4] = stcLineCoding.bCharFormat;
    pu8LineCoding[5] = stcLineCoding.bParityType;
    pu8LineCoding[6] = stcLineCoding.bDataBits;
    UsbDevice_SendDataControl(pstcUsbHandle,(uint8_t *)pu8LineCoding, 8);
}



/**
 ******************************************************************************
 ** getting vcomm connection status
 **
 ** \param none
 **
 ** \return TRUE: active / FALSE: inactive
 *****************************************************************************/
boolean_t UsbDeviceCdcCom_IsConnected(void) 
{
    return (bVcommActive && bReady);
}


/**
 ******************************************************************************
 ** sending byte / char_t
 **
 ** \param c byte / char_t to send
 **
 ** \return 1: error vcomm inactive / 0: successful sent
 *****************************************************************************/

uint8_t UsbDeviceCdcCom_SendByte(char_t c) 
{
    if (bVcommActive) 
    {	
	  	SendData((uint8_t*)&c,1,UsbPOLL);
        return 0;
    } 
    return 1;
}


/**
 ******************************************************************************
 ** sending a null terminated string
 **
 ** \param pcBuffer string to send
 **
 ** \return 1: error vcomm inactive / 0: successful sent
 *****************************************************************************/

uint8_t UsbDeviceCdcCom_SendString(char_t *pcBuffer) 
{
    volatile uint32_t u32Size;
	if (bVcommActive) 
    {
        for(u32Size=0;(pcBuffer[u32Size] != '\0');u32Size++); //calculating size
		SendData((uint8_t *)pcBuffer,u32Size,UsbPOLL);
        return 0;
    } 
    return 1;
}


/**
 ******************************************************************************
 ** sending a buffer
 **
 ** \param pu8Buffer buffer to send
 ** \param u32Size   buffersize
 **
 ** \return 1: error vcomm inactive / 0: successful sent
 *****************************************************************************/

uint8_t UsbDeviceCdcCom_SendBuffer(uint8_t *pu8Buffer, uint32_t u32Size) 
{
    if (bVcommActive) 
    {
        SendData((uint8_t *)pu8Buffer,u32Size,UsbPOLL);
        return 0;
    } 
    return 1;
}


/**
 ******************************************************************************
 ** splits the buffer at a defined character
 **
 ** \param split buffer every newSeparator
 **        if zerro, ignore splitting (default) 
 **
 ** \return 1: error vcomm inactive / 0: successful sent
 *****************************************************************************/

void UsbDeviceCdcCom_SetSeparator(const char_t cNewSeperator) 
{
    cSeperator = cNewSeperator;
}


/**
 ******************************************************************************
 ** echoes every received char
 **
 ** \param bEchoOnOff switch echo on/off
 **
 ** \return none
 *****************************************************************************/

void UsbDeviceCdcCom_SetEchomode(boolean_t bEchoOnOff) 
{
    bEchoMode = bEchoOnOff;
}

/**
 ******************************************************************************
 ** echoes every received char after receiving defined seperator
 **
 ** \param bEchoOnOff switch echo on/off
 **
 ** \return none
 *****************************************************************************/

void UsbDeviceCdcCom_SetEchoAtSeperatorOnly(boolean_t bEchoAtSeperatorOnlyOnOff) 
{
    bEchoAtSeperatorOnly = bEchoAtSeperatorOnlyOnOff;
}

/**
 ******************************************************************************
 ** returns number of received bytes in buffer
 **
 ** \param none
 **
 ** \return number of received bytes in buffer
 *****************************************************************************/

uint32_t UsbDeviceCdcCom_ReceivedLength(void) 
{
    if ((cSeperator == '\0') || (bSeperatorFlag == TRUE))
    {
        return u32ReceivedData;
    }
    return 0;   
}



/**
 ******************************************************************************
 ** transfer receive buffer and clear it
 **
 ** \param pu8Buffer buffer to write in 
 **
 ** \return buffer size
 *****************************************************************************/
 
uint32_t UsbDeviceCdcCom_ReceiveBuffer(uint8_t *pu8Buffer) 
{
    uint32_t u32ReturnValue;
    for(u32ReturnValue=0;u32ReturnValue<u32ReceivedData;u32ReturnValue++) 
    {
        pu8Buffer[u32ReturnValue] = pu8VcommBuffer[u32ReturnValue];
    }
    u32ReceivedData = 0;
    bSeperatorFlag = FALSE;
    return u32ReturnValue;
}


/**
 ******************************************************************************
 ** transfer one byte from the buffer 
 **
 ** \param none 
 **
 ** \return received byte
 *****************************************************************************/
 
uint8_t UsbDeviceCdcCom_ReceiveByte(void) 
{
    uint32_t i;
    uint8_t c;
    while (u32ReceivedData == 0);
    c = pu8VcommBuffer[0];
    /* shift buffer by 1 byte */
    for(i=0;i<(u32ReceivedData - 1);i++) 
    {    
        pu8VcommBuffer[i] = pu8VcommBuffer[i + 1];    
    }
    u32ReceivedData--;
    return c;
}


/**
 ******************************************************************************
 ** transfer last received byte without touching the buffer 
 **
 ** \param none 
 **
 ** \return received byte
 *****************************************************************************/
 
uint8_t UsbDeviceCdcCom_ReceiveLastByte(void) 
{
    if (u32ReceivedData > 0)
    {
        return (char_t)pu8VcommBuffer[u32ReceivedData - 1];
    }
    return 0;
}


/**
 ******************************************************************************
 ** sends a x-digit Hex-number (as ASCII charcaters) 
 **
 ** \param n value
 ** \param digits number of hex-digits 
 **
 ** \return 1: error vcomm inactive / 0: successful sent 
 *****************************************************************************/
 
uint8_t UsbDeviceCdcCom_SendHex(uint32_t n, uint8_t digits)
{
    uint8_t i,ch,div=0;

    if (UsbDeviceCdcCom_SendString("0x")) return 1;                /* hex string */
    div=(digits-1) << 2;    /* init shift divisor */

    for (i=0; i<digits; i++) {
      ch = (n >> div) & 0xF;/* get hex-digit value */
      if (UsbDeviceCdcCom_SendByte(pcASCII[ch])) return 1;        /* prompt to terminal as ASCII */
      div-=4;                /* next digit shift */
   }
   return 0;
}


/**
 ******************************************************************************
 ** sends a decimal-digit 
 **
 ** \param x value
 **
 ** \return 1: error vcomm inactive / 0: successful sent 
 *****************************************************************************/
 
uint8_t UsbDeviceCdcCom_SendDec(uint32_t x)
{
    int16_t i;
    char_t buf[9];
    if (x == 0) 
    {
      if (UsbDeviceCdcCom_SendString("       0")) return 1;
      return 0;
    }
    buf[8]='\0';                /* end sign of string */
    
    for (i=8; i>0; i--) 
    {
       buf[i-1] = pcASCII[x % 10];
       x = x/10;

    }

    for (i=0; buf[i]=='0'; i++) // no print16_t of zero 
    {
       buf[i] = ' ';
    }
    if (UsbDeviceCdcCom_SendString(buf)) return 1;                    /* send string */
    return 0;
}


/**
 ******************************************************************************
 ** 4-bit bin to ascii function
 **
 ** \param a value
 ** \param base type: HEX / DEC
 ** \param fill fill char
 **
 ** \return none 
 *****************************************************************************/
 
void UsbDeviceCdcCom_SendBinAsci4(uint16_t a, uint8_t base, char_t fill)                
{                            
  uint8_t val;
  a &= 0x0F;    

  switch(base)
    {    
        case HEX:    if (a <= 9) UsbDeviceCdcCom_SendByte(a + '0');                        
                  else        UsbDeviceCdcCom_SendByte(a + 'A' - 10);
                break;

        case DEC:    val = a / 10;
                if( val ) UsbDeviceCdcCom_SendByte( a / 10 + '0');
                    else  UsbDeviceCdcCom_SendByte( fill );
                UsbDeviceCdcCom_SendByte( a % 10 + '0');
                break;

        default:    UsbDeviceCdcCom_SendString(" <format not supported> ");
    }
}


/**
 ******************************************************************************
 ** 8-bit bin to ascii function
 **
 ** \param a value
 ** \param base type: HEX / DEC
 ** \param fill fill char
 **
 ** \return none 
 *****************************************************************************/
 
void UsbDeviceCdcCom_SendBinAsci8(uint16_t a, uint8_t base, char_t fill)
{
  uint16_t rem,val,d;
  uint8_t filling=1;
                        
    switch(base)
    {    
        case HEX:    UsbDeviceCdcCom_SendBinAsci4(a >> 4, HEX, fill);                         
                  UsbDeviceCdcCom_SendBinAsci4(a, HEX, fill);                        
                break;

        case DEC:    rem = a;
                for( d=100; d >= 10; d = d / 10)
                { 
                  val = rem / d; rem = rem % d;
                  if( val ) filling=0; 
                  if( filling ) UsbDeviceCdcCom_SendByte( fill );
                    else UsbDeviceCdcCom_SendByte( val + '0' );
                }
                UsbDeviceCdcCom_SendByte( rem + '0');
                break;

        default:    UsbDeviceCdcCom_SendString(" <format not supported> ");
    }

}


/**
 ******************************************************************************
 ** bin to ascii function
 **
 ** \param a value
 **
 ** \return none 
 *****************************************************************************/
 
void UsbDeviceCdcCom_SendBin2DotDec99(uint8_t a)
{
  
        uint8_t rem, val;
        UsbDeviceCdcCom_SendByte('.');

        rem = a;

        val = rem / 10; rem = rem % 10;
        UsbDeviceCdcCom_SendByte( val + '0' );
        
        UsbDeviceCdcCom_SendByte( rem + '0');


}


/**
 ******************************************************************************
 ** receive hex value
 **
 ** \param digits number of digits
 **
 ** \return value 
 *****************************************************************************/
 
uint32_t UsbDeviceCdcCom_ReceiveHex(uint8_t digits)
{
   uint32_t number=0;
   signed digit=0;
   uint8_t abort=0,mlt=0,i,key;
   UsbDeviceCdcCom_SetSeparator('\0');
   UsbDeviceCdcCom_SetEchomode(TRUE);
   mlt=(4*(digits-1));  /* 20 for 6 hex-digit numers, 4 for 2 hex-digits */
   for (i=0;i<digits;i++)
   {
     digit = -1;     
     while ((digit==-1) & (!abort))     /* input next valid digit */
     {
       key = UsbDeviceCdcCom_ReceiveByte();             /* wait for next key input (ASCII) */
       if (key == 27) abort=1;    /* ESC aborts */
       digit = ASCIItobin(key);   /* convert to number */ 
       if (digit == -1) UsbDeviceCdcCom_SendByte(8); /* backspace if not valid */
     }
     number+= (digit << mlt);     /* add digit value to number */
     mlt-=4;                      /* next digit shift */
   }
   UsbDeviceCdcCom_SetEchomode(FALSE);
   if (!abort) 
      return number;             /* return input value */
   else
   {
      UsbDeviceCdcCom_SendString("\n\n input cancled \n");
      return 0;                /* return abort indicator */
   }
}

/* Internal */
static uint32_t ASCIItobin(uint8_t k)
{
  char_t d=(char_t) -1;
  if ((k > 47) & (k < 58)) d = k - 48;  /* 0..9 */
  if ((k > 64) & (k < 71)) d = k - 55;  /* A..F */
  if ((k > 96) & (k < 103)) d = k - 87; /* a..f */
  return d;
}
#endif




