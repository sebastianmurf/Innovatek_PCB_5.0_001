#include "MyUsb.h"
#include "mcu.h"
#include "usbconfig.h"
#include "usb.h"
#include "GprsUart.h"
#ifndef USB_DISBALE_DEVICE_FUNCTIONALITY
	#include "usbdevice.h"
	#include "UsbDeviceCdcCom.h"
#endif
#ifndef USB_DISBALE_HOST_FUNCTIONALITY
	#include "usbhost.h"
	#include "usbclasssupervisor.h"
#endif 
#include "l3.h"
#include "ff.h"
#include "board.h"
#include "uart.h"
#include "usbdevicehw.h"
#include "usbhosthw.h"

#include "mmu.h"

#include "gprs.h"	//for debuging at server end

//char UsbBuff[USB_BUFF_SIZE];
ui UsbCrcCodeOut;
boolean_t UsbCommandFlag = FALSE;
boolean_t WaitForConnectionFlag = FALSE;//IF DURING POWER ON USB IS DETECTED
unsigned int UsbReceiveCounter;


void Main_AppHost(void);
void Main_AppDevice(void);

boolean_t wasConfigured;
    
extern boolean_t UsbDeviceCdcCom_IsConnected(void); 
extern void UsbDeviceCdcCom_SetSeparator(const char_t cNewSeperator);
extern void UsbDeviceCdcCom_SetEchomode(boolean_t bEchoOnOff); 
extern uint8_t UsbDeviceCdcCom_SendByte(char_t c); 
extern uint8_t UsbDeviceCdcCom_SendString(char_t *pcBuffer); 
extern uint32_t UsbDeviceCdcCom_ReceivedLength(void);
extern uint32_t UsbDeviceCdcCom_ReceiveBuffer(uint8_t *pu8Buffer); 
extern void PutStringUsb(const char * msg);

/* START ADDITIONAL PROCEDURES   */
void delay(uint32_t u32TimeOut)
{
   uint32_t i;
   u32TimeOut = u32TimeOut * 100;
   while(u32TimeOut--)
   {
       for(i=10;i>0;i--)
       {
       }
   }
}
/* END ADDITIONAL PROCEDURES   */




/* START FILESYSTEM USAGE */
#ifdef __USBMASSSTORAGE_H__
FATFS fatfs[1];      // used for USB MassStorage & Filesystem
FATFS *fs;           // used for USB MassStorage & Filesystem
char_t* path = "0:"; // used for USB MassStorage & Filesystem
FILINFO finfo;       // used for USB MassStorage & Filesystem
FIL myFile;          // used for USB MassStorage & Filesystem
UINT bytesWritten;   // used for USB MassStorage & Filesystem
DIR dirs;            // used for USB MassStorage & Filesystem
DWORD P1 = 0;        // used for USB MassStorage & Filesystem
DWORD clust = 0;     // used for USB MassStorage & Filesystem
DWORD total = 0;     // used for USB MassStorage & Filesystem
DWORD fre = 0;
//DWORD free = 0;      // used for USB MassStorage & Filesystem
#endif
/* END FILESYSTEM USAGE */

/* START USB HOST EXAMPLE USAGE VARIABLES SECTION */
boolean_t bHostMouseConnected = FALSE;
boolean_t bHostKeyboardConnected = FALSE;
boolean_t bHostMassStorageConnected = FALSE;
/* END USB HOST EXAMPLE USAGE VARIABLES SECTION */
  


/*      START GLOBAL VARIABLES FOR VIRTUAL COM PORT EXAMPLE       */

 char_t UsbReceiveBuffer[512];
 uint32_t u32DeviceCdcReceiveSize;
 boolean_t bDeviceCdcComConnected;
/*      END GLOBAL VARIABLES FOR VIRTUAL COM PORT EXAMPLE        */


void CheckUsbStage()
{	static char UsbStage = USB_NOT_INITIALIZED;
  	volatile uint32_t u32Timeout;
  	switch(UsbStage)
	{	case USB_NOT_INITIALIZED:
	  		UsbConfig_UsbInit();
			Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchToDevice,0);
			bFM3_GPIO_PDOR6_P0 = 0;
  			bFM3_GPIO_DDR6_P0 = 1;
  			u32Timeout = 1000000;
			while((u32Timeout > 0))
			{
				u32Timeout--;
			}
			UsbStage = USB_SWITCH_TO_DEVICE;
		//	break;
		case USB_SWITCH_TO_DEVICE:
			Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchToDevice,0);
			if (DEVICE0VBUS_HIGHDETECT)
			{
				PutStringUsb("Loading USB Device Application...\n");
				u32Timeout = 1500000;
				while(UsbDevice_GetStatus((stc_usbn_t*)&USB0) == UsbDisconnected)
				{
					 u32Timeout--;
				}
				//boolean_t wasConfigured;
				wasConfigured = FALSE;
				UsbDeviceCdcCom_SetSeparator(']');    // there is the possibility to set end of buffer by a seperator
				UsbDeviceCdcCom_SetEchomode(FALSE); // all input shall be echoed
				UsbStage = USB_MAIN_APP_AS_A_DEVICE;  
				WaitForConnectionFlag = TRUE;
			}
			break;
		case USB_MAIN_APP_AS_A_DEVICE:
		  	Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchDependingDeviceVbus,0);
			if (UsbDevice_GetStatus((stc_usbn_t*)&USB0) == UsbConfigured)
			{
			 	wasConfigured = TRUE;
			}
			else
			{
				if (wasConfigured)
				{
					UsbStage = USB_SWITCH_TO_DEVICE;
					// return; // USB was disconnected
				   	break;
				}
			}
	
		   /**************************************************************************************************/
		   /*                                                                                                */
		   /* START VIRTUAL COM PORT EXAMPLE USAGE                                                           */
		   /*                                                                                                */
		   /**************************************************************************************************/
		   #ifndef USB_DISBALE_DEVICE_FUNCTIONALITY
		   /* waiting for a connection */
		   if (bDeviceCdcComConnected != UsbDeviceCdcCom_IsConnected())
		   {
			   bDeviceCdcComConnected = UsbDeviceCdcCom_IsConnected();
			   if (bDeviceCdcComConnected == TRUE)
			   {
				   /* sending welcome message after connection*/
				   UsbDeviceCdcCom_SendString("\r\n");
				   UsbDeviceCdcCom_SendString("*Hello Nabeen Lal Amatya. Welcome to iCORE Virtual Comm Port *\r\n");
				   WaitForConnectionFlag = FALSE; //CONNECTION MADE. NO NEED TO WAIT NOW
			   }
			   else
			   {
			   }
		   }
		   
		   if (UsbDeviceCdcCom_IsConnected()) 
		   {
			   if (UsbDeviceCdcCom_ReceivedLength() > 0) {
				   /* receive data from buffer */
				   u32DeviceCdcReceiveSize = UsbDeviceCdcCom_ReceiveBuffer((uint8_t *)UsbReceiveBuffer); //this clears also the receive buffer
				   UsbReceiveBuffer[u32DeviceCdcReceiveSize-1] = '\0';    //adding zero termination to string
			 
				   /* print16_t out UsbReceiveBuffer through Virtual Comm Port */
				   /*UsbDeviceCdcCom_SendString("\r\nMain Received String 1: ");
				   UsbDeviceCdcCom_SendString(UsbReceiveBuffer);//problem when size is more than 64 byte
					PutStringUsb("\r\nMain Received String 2: ");
				   PutStringUsb(UsbReceiveBuffer);//no problem when size is more than 64 byte
				   UsbDeviceCdcCom_SendString("\r\nSend Complete\r\n"); 
				   */
				   /*strcpy(UsbBuff,UsbReceiveBuffer);
				   UsbReceiveCounter = strlen(UsbBuff);
				   UsbBuff[UsbReceiveCounter-1] = '\0';
				   */
				   UsbCommandFlag = TRUE;
			   }  
		   }  
		   #endif
		   break;
		case USB_SWITCH_TO_HOST:
			  /*else
			  {
				 Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchToDevice,0);
				 Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchToHost,0);
				 Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchToHost,0);
				 u32Timeout = 1500000;
				 while((UsbClassSupervisor_GetActiveDriver((stc_usbn_t*)&USB0) == 0) && (u32Timeout > 0))
				 {
					 u32Timeout--;
				 }
				 if (UsbClassSupervisor_GetActiveDriver((stc_usbn_t*)&USB0) != 0)
				 {
					 //puts("Loading USB Host Application...\n");
					 PutStringUsb("Loading USB Host Application...\n");
					 //LED_DUTY(LED_RED,0);
					 //LED_DUTY(LED_GREEN,0);
					 //LED_DUTY(LED_BLUE,0);
					 Main_AppHost();
				 }
				 Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchToDevice,0);
				 bFM3_GPIO_PDOR6_P0 = 0;
				 bFM3_GPIO_DDR6_P0 = 1;
				 HOST0VBUS_CLEAR;
				 u32Timeout = 1500000;
				 while((u32Timeout > 0) && (DEVICE0VBUS_HIGHDETECT))
				 {
					 u32Timeout--;
				 }
				 bFM3_GPIO_DDR6_P0 = 0;
			  }*/
		  	break;
			  
	}
}
/*
void Main_AppDevice(void)
{	
  	boolean_t wasConfigured;
     wasConfigured = FALSE;
     UsbDeviceCdcCom_SetSeparator('\r');    // there is the possibility to set end of buffer by a seperator
     UsbDeviceCdcCom_SetEchomode(TRUE); // all input shall be echoed
     PutStringUsb("\n\rInside Main_AppDevice");
	 while(UsbDevice_GetStatus((stc_usbn_t*)&USB0) != UsbDisconnected) 
     {	FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
		FM3_SWWDT->WDOGINTCLR = 0x11111111;
		FM3_SWWDT->WDOGLOCK = 0;
	
         Usb_SwitchUsb((stc_usbn_t*)&USB0,UsbSwitchDependingDeviceVbus,0);
         if (UsbDevice_GetStatus((stc_usbn_t*)&USB0) == UsbConfigured)
         {
	     wasConfigured = TRUE;
         }
         else
         {
             if (wasConfigured)
             {
                 return; // USB was disconnected
             }
         }

       //#################################################################################################
       //                                                                                                
       // START VIRTUAL COM PORT EXAMPLE USAGE                                                           
       //                                                                                                
       //#################################################################################################
       #ifndef USB_DISBALE_DEVICE_FUNCTIONALITY
       // waiting for a connection 
       if (bDeviceCdcComConnected != UsbDeviceCdcCom_IsConnected())
       {
           bDeviceCdcComConnected = UsbDeviceCdcCom_IsConnected();
           if (bDeviceCdcComConnected == TRUE)
           {
               // sending welcome message after connection
               UsbDeviceCdcCom_SendString("\r\n");
               UsbDeviceCdcCom_SendString("Welcome to FSEU Virtual Comm Port Example for FM3!\r\n");
               UsbDeviceCdcCom_SendString("waiting for your message:\r\n");
           }
           else
           {
           }
       }
	   
       if (UsbDeviceCdcCom_IsConnected()) 
       {
           if (UsbDeviceCdcCom_ReceivedLength() > 0) {
               // receive data from buffer 
               u32DeviceCdcReceiveSize = UsbDeviceCdcCom_ReceiveBuffer((uint8_t *)UsbReceiveBuffer); //this clears also the receive buffer
               UsbReceiveBuffer[u32DeviceCdcReceiveSize] = '\0';    //adding zero termination to string
         
               // print16_t out UsbReceiveBuffer through Virtual Comm Port 
               PutStringUsb(UsbReceiveBuffer);
			   UsbDeviceCdcCom_SendString("Main Received String: ");
               UsbDeviceCdcCom_SendString(UsbReceiveBuffer);//problem when size is more than 64 byte
				PutStringUsb(UsbReceiveBuffer);
               UsbDeviceCdcCom_SendString("\r\n"); 
           }  
       }  
       #endif
       //#################################################################################################
       //                                                                                                
       // END VIRTUAL COM PORT EXAMPLE USAGE                                                             
       //                                                                                                
       //#################################################################################################

    } // End Loop Forever 

}
*/
void Main_AppHost(void)
{
    bHostMouseConnected = FALSE;
    bHostKeyboardConnected = FALSE;
    bHostMassStorageConnected = FALSE;
    while(UsbClassSupervisor_GetActiveDriver((stc_usbn_t*)&USB0) != 0)  
    {
        #ifdef __USBHOSTHIDMOUSE_H__
        /* USB Host Mouse Driver API */
        if (UsbHostHidMouse_DriverActive())
        {
            stc_point_t stcPos;
            if (bHostMouseConnected == FALSE)
            {
                //LED_DUTY(LED_RED,0);
                //LED_DUTY(LED_GREEN,255);
                //LED_DUTY(LED_BLUE,0);
                bHostMouseConnected = TRUE;
                puts("\n>> Mouse Connected <<\n");
            }
            if (UsbHostHidMouse_Moved())
            {
                  stcPos = UsbHostHidMouse_GetCurrentPosition();
                  puts("X:");
                  putdec(stcPos.u32X);
                  //LED_DUTY(LED_RED,stcPos.u32X / 32);
                  puts(", Y:");
                  putdec(stcPos.u32Y);
                  //LED_DUTY(LED_BLUE,stcPos.u32Y / 32);
                  puts(", Scroll: ");
                  putdec(UsbHostHidMouse_GetCurrentScrollPosition());
                  //LED_DUTY(LED_GREEN,UsbHostHidMouse_GetCurrentScrollPosition() * 4);
                  puts(", Buttons: ");
                  puthex((uint32_t)UsbHostHidMouse_GetButtons(),2);
                  puts("\n");
           }
        } else
        {
            if (bHostMouseConnected == TRUE)
            {
                bHostMouseConnected = FALSE;
                puts("\n>> Mouse Disconnected <<\n");
            }
        } /* end if .. else UsbHostHidMouse_DriverActive() */
        #endif


        #ifdef __USBHOSTHIDKEYBOARD_H__
        /* USB Host Keyboard Driver API */
        if (UsbHostHidKeyboard_DriverActive())
        {
            char_t c;
            if (bHostKeyboardConnected == FALSE)
            {
                //LED_DUTY(LED_RED,0);
                //LED_DUTY(LED_GREEN,255);
                //LED_DUTY(LED_BLUE,0);
                bHostKeyboardConnected = TRUE;
                puts("\n>> Keyboard Connected <<\n");
            }
            c = UsbHostHidKeyboard_GetCh(100);
            if (c != 0)
            {
                putch(c);
                switch(c)
                  {
                  case '1':
                     //LED_DUTY(LED_RED,255);
                     //LED_DUTY(LED_GREEN,0);
                     //LED_DUTY(LED_BLUE,0);
                     break;
                  case '2':
                     //LED_DUTY(LED_RED,0);
                     //LED_DUTY(LED_GREEN,255);
                     //LED_DUTY(LED_BLUE,0);
                     break;
                  case '3':
                     //LED_DUTY(LED_RED,0);
                     //LED_DUTY(LED_GREEN,0);
                     //LED_DUTY(LED_BLUE,255);
                     break;
                  default:
                    //LED_DUTY(LED_RED,c);
                    //LED_DUTY(LED_GREEN,c);
                    //LED_DUTY(LED_BLUE,c);
                    break;
                  }
            }
        } else
        {
            if (bHostKeyboardConnected == TRUE)
            {
                bHostKeyboardConnected = FALSE;
                puts("\n>> Keyboard Disconnected <<\n");
            }
        } /* end if .. else UsbHostHidKeyboard_DriverActive() */
        #endif
        
        
        #ifdef __USBMASSSTORAGE_H__
        /* USB Host MassStorage Driver API */
        if (UsbHostMassStorage_DriverActive())
        {

            if (bHostMassStorageConnected == FALSE)
            {
              //LED_DUTY(LED_RED,255);
              //LED_DUTY(LED_GREEN,0);
              //LED_DUTY(LED_BLUE,0);
              bHostMassStorageConnected = TRUE;
              puts("\n>> MassStorage Connected <<\n");
                
              puts("Loading USB Mass Storage Application\n");
              puts("\nContents:\n");
              puts("-----------------------------------------\n");
              delay(10000); // wait a moment
              while((UsbHostMassStorage_StorageApiInit() != 0) && (UsbHostMassStorage_DriverActive()));
	          if (UsbHostMassStorage_DriverActive())
              {
                  f_mount(P1,fatfs);
                  if (f_opendir(&dirs, path) == FR_OK) 
                    {
                        //i = strlen(path);
                        while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
                        {
                            if (finfo.fattrib & AM_DIR)
                            {
                            
                                puts("[DIR] ");
                                puts((char*)finfo.fname);
                                puts("\n");
                   
                            } 
                               else 
                               {
                                  puts("      ");
                                  puts((char*)finfo.fname);
                                  puts("     ");
                                  putdec(finfo.fsize);
                                  puts(" ");   
                                  puts("byte");               
                                  puts("\n");
                               }
                        }
                    }
                    f_getfree (path, &clust, &fs);
                    total = ((fs->max_clust - 2) * fs->csize) / 2;
                    fre = (clust * fs->csize) / 2;
                    puts("\n");
                    puts("-----------------------------------------\n");
                    putdec(total / 1024);
                    puts(" MB total;");
                    putdec(fre / 1024);
                    puts(" MB free\n");  
                    puts("\n\n\n"); 
                    f_open(&myFile,"0:fujitsu.txt",FA_CREATE_ALWAYS | FA_WRITE);
                    f_write(&myFile, "SK-FM3-48PMC-USBSTICK - http://mcu.emea.fujitsu.com", 51, &bytesWritten);
                    f_close(&myFile);
                    
                    //LED_DUTY(LED_RED,0);
                    //LED_DUTY(LED_GREEN,255);
                    //LED_DUTY(LED_BLUE,0);
                  
                      puts("\n>>> Please remove now the USB Device <<<\n");
              }
            }
        } else
        {
            if (bHostMassStorageConnected == TRUE)
            {
                bHostMassStorageConnected = FALSE;
                puts("\n>> MassStorage Disconnected <<\n");
            }
        } /* end if .. else UsbHostMassStorage_DriverActive() */
        #endif
        /**************************************************************************************************/
        /*                                                                                                */
        /* END USB HOST EXAMPLE USAGE                                                                     */
        /*                                                                                                */
        /**************************************************************************************************/
    }
}

//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a single character (no timeout!)
 *  PARAMETERS:     Character
 *  RETURNS:        None
 *****************************************************************************/
void PutCharUsb(uint8_t u8Char)
{	UsbDeviceCdcCom_SendByte(u8Char);
	UsbCrcCodeOut += u8Char;
}
//------------------------------------------------------------------------------
/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *  RETURNS:        None
 *****************************************************************************/
void PutStringUsb(const char *pu8Buffer)              
{	char* buff;
	buff = (char*)pu8Buffer;
  	while (*buff != '\0')
  	{ 	PutCharUsb(*buff++);	// send every char of string
  	}
	//if(ActiveServer == 0 && MobileStage == MOBILE_STAGE_WAITING_SERVER_RESPONSE)
	//	PutStringGprs(pu8Buffer);
}
//------------------------------------------------------------------------------
/*void PutStringUsb(const char * msg)
{	int i;
  	//UsbDeviceCdcCom_SendString("Received String: ");
    for(i=0;msg[i]!='\0';i++)
	{	UsbDeviceCdcCom_SendByte(msg[i]); 
	}
}
*/