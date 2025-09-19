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
/** \file usb.c
 **
 ** A detailed description is available at 
 ** @link UsbGroup USB Module description @endlink
 **
 ** History:
 **   - 2012-08-28  2.0  MSc  First version (starting at version 2.0)
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "usb.h"
#include "usbethernetclock.h"

#if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) || (L3_PERIPHERAL_ENABLE_USB1 == L3_ON))

/**
 ******************************************************************************
 ** \ingroup UsbGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

#if (L3_PERIPHERAL_ENABLE_USB0 == L3_ON)
   #ifndef USB0
       #error USB0 is not existing in the MCUs header file, maybe this MCU does not support USB, please change L3_PERIPHERAL_ENABLE_USB0
   #endif
#endif

#if (L3_PERIPHERAL_ENABLE_USB1 == L3_ON)
   #ifndef USB1
       #error USB1 is not existing in the MCUs header file, maybe this MCU does not support more than one USB, please change L3_PERIPHERAL_ENABLE_USB1
   #endif
#endif

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

static boolean_t UsbInitDone = FALSE;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

static stc_usb_intern_data_t* UsbGetInternDataPtr(stc_usbn_t* pstcUsb);
static en_result_t Usb_InitIrq(stc_usbn_t* pstcUsb); 
static en_result_t Usb_DeinitIrq(stc_usbn_t* pstcUsb);
#if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
static en_result_t Usb_InitDeviceMode(stc_usbn_t* pstcUsb);
static en_result_t Usb_DeinitDeviceMode(stc_usbn_t* pstcUsb);
#endif
#if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
static en_result_t Usb_InitHostMode(stc_usbn_t* pstcUsb);
static en_result_t Usb_DeinitHostMode(stc_usbn_t* pstcUsb);
#endif
static void Usb_TimeoutHandler(stc_usbn_t* pstcUsb);
extern void UsbHost_DisconnectionCallback(stc_usbn_t* pstcUsb);

/// Macro to return the number of enabled USB instances
#define USB_INSTANCE_COUNT (uint32_t)(sizeof(m_astcUsbInstanceDataLut) / sizeof(m_astcUsbInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled USB instances and their internal data
stc_usbn_instance_data_t m_astcUsbInstanceDataLut[] =
{
    #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)))
    { &USB0,  // pstcInstance
      0    // stcInternData (not initialized yet)
    },
    #endif
    #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)))
    { &USB1,  // pstcInstance
      0    // stcInternData (not initialized yet)
    },
    #endif
};
/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Return the internal data for a certain USB instance.
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_usb_intern_data_t* UsbGetInternDataPtr(stc_usbn_t* pstcUsb) 
{
    volatile uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < USB_INSTANCE_COUNT; u32Instance++)
    {
        if ((uint32_t)pstcUsb == (uint32_t)(m_astcUsbInstanceDataLut[u32Instance].pstcInstance))
        {
            return &m_astcUsbInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}

/**
 ******************************************************************************
 ** \brief Device dependent initialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return Ok     Successful initialization
 **
 ******************************************************************************/
static en_result_t Usb_InitIrq(stc_usbn_t* pstcUsb) 
{
  #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)))
  if (pstcUsb == &USB0)
  {
    #if (defined(_MB9A110K_H_) || defined(_MB9A110L_H_) || defined(_MB9A110M_H_) || defined(_MB9A110N_H_) || \
         defined(_MB9A130K_H_) || defined(_MB9A130L_H_) || defined(_MB9A130M_H_) || defined(_MB9A130N_H_) || \
                                  defined(_MB9A310L_H_) || defined(_MB9A310M_H_) || defined(_MB9A310N_H_) || \
                                                           defined(_MB9B500N_H_) || defined(_MB9B500R_H_))                           
        #define USB0F_IRQn USBF_IRQn
        #define USB0F_USB0H_IRQn USBF_USBH_IRQn
    #endif
    
    // USB0 interrupt settings
    USBDBG("INIT ISR\n");
    NVIC_ClearPendingIRQ(USB0F_IRQn); 
    NVIC_EnableIRQ(USB0F_IRQn);
    NVIC_SetPriority(USB0F_IRQn,L3_IRQ_LEVEL_USB0);
    
    NVIC_ClearPendingIRQ(USB0F_USB0H_IRQn); 
    NVIC_EnableIRQ(USB0F_USB0H_IRQn);
    NVIC_SetPriority(USB0F_USB0H_IRQn,L3_IRQ_LEVEL_USB0);
  }
  #endif
  
  #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)))
  if (pstcUsb == &USB1)
  {
    // USB1 interrupt settings
    NVIC_ClearPendingIRQ(USB1F_IRQn); 
    NVIC_EnableIRQ(USB1F_IRQn);
    NVIC_SetPriority(USB1F_IRQn,L3_IRQ_LEVEL_USB1);
    
    NVIC_ClearPendingIRQ(USB1F_USB1H_IRQn); 
    NVIC_EnableIRQ(USB1F_USB1H_IRQn);
    NVIC_SetPriority(USB1F_USB1H_IRQn,L3_IRQ_LEVEL_USB1);
  }
  #endif

  return Ok;
}


/**
 ******************************************************************************
 ** \brief Device dependent deinitialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \return Ok     Successful deinitialization
 **
 ******************************************************************************/
static en_result_t Usb_DeinitIrq(stc_usbn_t* pstcUsb) 
{
  #if (L3_PERIPHERAL_ENABLE_USB0 == L3_ON)
  if (pstcUsb == &USB0)
  {
    #if (defined(_MB9A110K_H_) || defined(_MB9A110L_H_) || defined(_MB9A110M_H_) || defined(_MB9A110N_H_) || \
         defined(_MB9A130K_H_) || defined(_MB9A130L_H_) || defined(_MB9A130M_H_) || defined(_MB9A130N_H_) || \
                                  defined(_MB9A310L_H_) || defined(_MB9A310M_H_) || defined(_MB9A310N_H_) || \
                                                           defined(_MB9B500N_H_) || defined(_MB9B500R_H_))                           
        #define USB0F_IRQn USBF_IRQn
        #define USB0F_USB0H_IRQn USBF_USBH_IRQn
    #endif
    
    // USB0 interrupt settings
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    NVIC_ClearPendingIRQ(USB0F_IRQn); 
    NVIC_DisableIRQ(USB0F_IRQn);
    NVIC_SetPriority(USB0F_IRQn,15);
    #endif
    
    NVIC_ClearPendingIRQ(USB0F_USB0H_IRQn); 
    NVIC_DisableIRQ(USB0F_USB0H_IRQn);
    NVIC_SetPriority(USB0F_USB0H_IRQn,15);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_USB1 == L3_ON)
  if (pstcUsb == &USB1)
  {
    // USB1 interrupt settings
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    NVIC_ClearPendingIRQ(USB1F_IRQn); 
    NVIC_DisableIRQ(USB1F_IRQn);
    NVIC_SetPriority(USB1F_IRQn,15);
    #endif
    
    NVIC_ClearPendingIRQ(USB1F_USB1H_IRQn); 
    NVIC_DisableIRQ(USB1F_USB1H_IRQn);
    NVIC_SetPriority(USB1F_USB1H_IRQn,15);
  }
  #endif

  return Ok;
}


/**
 ******************************************************************************
 ** \brief Initialize USB in general
 **
 ** \param pstcUsb      USB handle
 **
 ** \param pstcConfig   USB configuration
 **
 ** \return #en_result_t
 **
 ******************************************************************************/
en_result_t Usb_Configure(stc_usbn_t* pstcUsb, stc_usb_config_t* pstcConfig)
{
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    if (pstcUsbIntern->bUsbInstanceSet == FALSE)
    {
        Usb_InitInstance(pstcUsb);
    }
    if (UsbInitDone == FALSE)
    {
        Usb_Init();
    }
    if (pstcConfig->bUseInterrupts == TRUE)
    {
        Usb_InitIrq(pstcUsb);
    }
    else
    {
        Usb_DeinitIrq(pstcUsb);
    }
    
    pstcUsbIntern->bDeviceEnabled = TRUE;
    pstcUsbIntern->bHostEnabled = TRUE;
    
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    pstcUsbIntern->pu8DeviceDescriptor = pstcConfig->pu8DeviceDescriptor;
    pstcUsbIntern->pu8ConfigDescriptor = pstcConfig->pu8ConfigDescriptor;
    #endif
    
    pstcUsbIntern->pfnSetpCallback = pstcConfig->pfnSetpCallback;
    pstcUsbIntern->pfnConfCallback = pstcConfig->pfnConfCallback;
    pstcUsbIntern->pfnSofCallback = pstcConfig->pfnSofCallback;
    pstcUsbIntern->pfnDeviceVbus = pstcConfig->pfnDeviceVbus;
    pstcUsbIntern->pfnHostVbus = pstcConfig->pfnHostVbus;
    pstcUsbIntern->pfnHostPullDownHostEnable = pstcConfig->pfnHostPullDownHostEnable;
    pstcUsbIntern->pfnHostOvercurrent = pstcConfig->pfnHostOvercurrent;
    pstcUsbIntern->pfnDeviceInit = pstcConfig->pfnDeviceInit;
    pstcUsbIntern->pfnHostInit = pstcConfig->pfnHostInit;
    
    if (pstcUsbIntern->pfnHostVbus != NULL)
    {
        pstcUsbIntern->pfnHostVbus(UsbGpioInit);
        pstcUsbIntern->pfnHostVbus(UsbGpioClear);
    }
    if (pstcUsbIntern->pfnHostPullDownHostEnable != NULL)
    {
        pstcUsbIntern->pfnHostPullDownHostEnable(UsbGpioInit);
        pstcUsbIntern->pfnHostPullDownHostEnable(UsbGpioClear);
    }
    if (pstcUsbIntern->pfnHostOvercurrent != NULL)
    {
        pstcUsbIntern->pfnHostOvercurrent(UsbExtIntInit);
    }
    if (pstcUsbIntern->pfnDeviceVbus != NULL)
    {
        pstcUsbIntern->pfnDeviceVbus(UsbExtIntInit);
    }
      
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Initialize USB in general
 **
 ** \return #en_result_t
 **
 ******************************************************************************/
en_result_t Usb_Init( void )
{
    volatile uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < USB_INSTANCE_COUNT; u32Instance++)
    {
        memset(&m_astcUsbInstanceDataLut[u32Instance].stcInternData,0,sizeof(m_astcUsbInstanceDataLut[u32Instance].stcInternData));
    }
    
    #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)))
    Usb_InitInstance((stc_usbn_t*)&USB0);
    #endif
    
    #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)))
    Usb_InitInstance((stc_usbn_t*)&USB1);
    #endif

    #if (USB_USE_L3 == 0)    
        #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)))
            bFM3_GPIO_SPSR_USB0C = 1;
            #ifdef bFM3_GPIO_EPFR00_USBP0E
            bFM3_GPIO_EPFR00_USBP0E = 1;
            #endif
            #ifdef bFM3_GPIO_EPFR00_USB0PE
            bFM3_GPIO_EPFR00_USB0PE = 1;
            #endif
            #ifdef bFM3_GPIO_PFR6_P1 
            /* USB Pull-up : P61 as UHCONX */
            bFM3_GPIO_PFR6_P1 = 1;
            #endif
        #endif
        #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)))
            bFM3_GPIO_SPSR_USB1C = 1;
            #ifdef bFM3_GPIO_EPFR00_USBP1E
            bFM3_GPIO_EPFR00_USBP1E = 1;
            #endif
            #ifdef bFM3_GPIO_EPFR00_USB1PE
            bFM3_GPIO_EPFR00_USB1PE = 1;
            #endif
            #ifdef bFM3_GPIO_PFR6_P1 
            /* USB Pull-up : P61 as UHCONX */
            bFM3_GPIO_PFR2_P0 = 1;
            #endif
        #endif    
    #else
        #if ((L3_PERIPHERAL_ENABLE_USB0 == L3_ON) && (defined(USB0)))
            bL3_FM3_GPIO_SPSR_USB0C = 1;
            bL3_FM3_GPIO_EPFR00_USB0PE = 1;
            bL3_FM3_GPIO_PFR6_P1 = 1;
        #endif
        #if ((L3_PERIPHERAL_ENABLE_USB1 == L3_ON) && (defined(USB1)))
            bL3_FM3_GPIO_SPSR_USB1C = 1;
            bL3_FM3_GPIO_EPFR00_USB1PE = 1;
            bL3_FM3_GPIO_PFR2_P0 = 1;
        #endif   
    #endif        
    UsbEthernetClock_Init();
    UsbInitDone = TRUE;
    
    return Ok;
}


/**
 ******************************************************************************
 ** \brief Setup USB HAL endpoint
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param u8EndpointAddress Endpoint address
 **
 ** \param u16FifoSize Endpoint FIFO size
 **
 ** \param enType Type of endpoint, see also #en_usb_ep_type_t
 **
 ** \param bInterruptsEnabled Use interrupts
 **
 ** \return #en_result_t
 **
 ******************************************************************************/
en_result_t Usb_SetupHalEndpoint(stc_usbn_t* pstcUsb,uint8_t u8EndpointAddress , uint16_t u16FifoSize, en_usb_ep_type_t enType, boolean_t bInterruptsEnabled)
{
     stc_usbn_endpoint_data_t* pstcEndpoint;
     stc_usb_intern_data_t* pstcUsbIntern;
     pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
     
     pstcEndpoint = &(pstcUsbIntern->astcEndpoints[(u8EndpointAddress & 0x1F) + 1]);
       
     *(pstcEndpoint->pstcEpControlRegister) = u16FifoSize & 0x1FF;
     pstcEndpoint->u8EndpointAddress = u8EndpointAddress;
     pstcEndpoint->u16EndpointSize = (u16FifoSize & 0x1FF);
     *(pstcEndpoint->pstcEpControlRegister) |= (((u8EndpointAddress & 0x80) > 0) << 12);
     *(pstcEndpoint->pstcEpControlRegister) |= enType << 13;
     if ((bInterruptsEnabled == TRUE) && ((u8EndpointAddress & 0x80) == 0))
     {
         BITMASK_SET(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
     }
     else
     {
         BITMASK_CLEAR(*(pstcEndpoint->pstcEpStatusRegister),_EPNS_DRQIE);
     }
     BITMASK_SET(*(pstcEndpoint->pstcEpControlRegister),_EPNC_EPEN);
     return Ok;
}

/**
 ******************************************************************************
 ** \brief Switch "Task" between host / device mode
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param enType Type of switching, see also #en_usb_switch_t
 **
 ** \param u32SwitchDelay optional delay
 **
 ** \return #en_result_t
 **
 ******************************************************************************/
en_result_t Usb_SwitchUsb(stc_usbn_t* pstcUsb, en_usb_switch_t enType, uint32_t u32SwitchDelay)
{
    static volatile uint32_t u32InternalSwitchDelay;
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    if (u32InternalSwitchDelay != 0) 
    {
        u32InternalSwitchDelay--;
        return ErrorNotReady;
    }
    switch(enType)
    {
        case UsbSwitchDependingDeviceVbus:
            if (pstcUsbIntern->pfnDeviceVbus != NULL)
            {
                if (pstcUsbIntern->pfnDeviceVbus(UsbExtIntGetLevel) == TRUE)
                {
                    #if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
                    if ((pstcUsbIntern->bHostActive == TRUE) && (pstcUsbIntern->bHostEnabled == TRUE))
                    {
                        pstcUsbIntern->bHostActive = !(Usb_DeinitHostMode(pstcUsb) == 0);
                        u32InternalSwitchDelay = u32SwitchDelay;
                        return ErrorNotReady;
                    }
                    #endif

                    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
                    if ((pstcUsbIntern->bDeviceActive == FALSE) && (pstcUsbIntern->bDeviceEnabled == TRUE))
                    {
                        pstcUsbIntern->bDeviceActive = (Usb_InitDeviceMode(pstcUsb) == 0);
                        return (en_result_t)pstcUsbIntern->bDeviceActive;
                    }
                    #endif
                } else
                {
                    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
                    if ((pstcUsbIntern->bDeviceActive == TRUE) && (pstcUsbIntern->bDeviceEnabled == TRUE))
                    {
                        pstcUsbIntern->bDeviceActive = !(Usb_DeinitDeviceMode(pstcUsb) == 0);
                        u32InternalSwitchDelay = u32SwitchDelay;
                        return ErrorNotReady;
                    }
                    #endif
                    #if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
                    if ((pstcUsbIntern->bHostActive == FALSE) && (pstcUsbIntern->bHostEnabled == TRUE))
                    {
                        pstcUsbIntern->bHostActive = (Usb_InitHostMode(pstcUsb) == 0);
                        return (en_result_t)pstcUsbIntern->bHostActive;
                    }
                    #endif
                }
            }
            break;
        case UsbSwitchToDevice:
            #if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
            if ((pstcUsbIntern->bHostActive == TRUE) && (pstcUsbIntern->bHostEnabled == TRUE))
            {
                pstcUsbIntern->bHostActive = !(Usb_DeinitHostMode(pstcUsb) == 0);
                u32InternalSwitchDelay = u32SwitchDelay;
                return ErrorNotReady;
            }
            #endif
            #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
            if ((pstcUsbIntern->bDeviceActive == FALSE) && (pstcUsbIntern->bDeviceEnabled == TRUE))
            {
                pstcUsbIntern->bDeviceActive = (Usb_InitDeviceMode(pstcUsb) == 0);
                return (en_result_t)pstcUsbIntern->bDeviceActive;
            }
            #endif
            break;
        case UsbSwitchToHost:
          #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
          if ((pstcUsbIntern->bDeviceActive == TRUE) && (pstcUsbIntern->bDeviceEnabled == TRUE))
          {
              pstcUsbIntern->bDeviceActive = !(Usb_DeinitDeviceMode(pstcUsb) == 0);
              u32InternalSwitchDelay = u32SwitchDelay;
              return ErrorNotReady;
          }
          #endif
          #if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
          if ((pstcUsbIntern->bHostActive == FALSE) && (pstcUsbIntern->bHostEnabled == TRUE))
          {
              pstcUsbIntern->bHostActive = (Usb_InitHostMode(pstcUsb) == 0);
              return (en_result_t)pstcUsbIntern->bHostActive;
          }
          #endif
          break;
    }
    return Ok;
}

#if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
/**
 ******************************************************************************
 ** \brief Init USB device
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
static en_result_t Usb_InitDeviceMode(stc_usbn_t* pstcUsb)
{
    volatile uint32_t i;
    stc_usb_config_t stcConfig;
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    
    if ((pstcUsbIntern->pu8DeviceDescriptor == NULL) || (pstcUsbIntern->pfnDeviceInit == NULL))
    {
        return ErrorUninitialized;
    }
    if (pstcUsbIntern->pfnHostPullDownHostEnable != NULL)
    {
        pstcUsbIntern->pfnHostPullDownHostEnable(UsbGpioClear);
    }
    if (pstcUsbIntern->pfnHostVbus != NULL)
    {
        pstcUsbIntern->pfnHostVbus(UsbGpioClear);
    }
    
    
    pstcUsb->UDCC_f.RST = 1;    // Reset USB HAL
    pstcUsb->HCNT = 0;          // Switch off host
    pstcUsb->HCNT_f.URST = 1;   // Switch off host
    pstcUsb->UDCC_f.USTP = 0;
    pstcUsb->UDCC_f.RESUM  = 0;  
    pstcUsb->UDCC_f.RFBK  = 0;  

    if (pstcUsbIntern->pu8ConfigDescriptor[7] == USBATTR_SELFPOWER) 
    {
        pstcUsb->UDCC_f.PWC = 1;
    }
    else
    {
        pstcUsb->UDCC_f.PWC = 0;
    }
    
    pstcUsb->EP0C = pstcUsbIntern->pu8DeviceDescriptor[7];
    
    pstcUsb->UDCC_f.RST = 0;
    
    pstcUsbIntern->pfnDeviceInit(pstcUsb,&stcConfig);
    
    
     
    pstcUsbIntern->pfnSetpCallback = stcConfig.pfnSetpCallback;
    pstcUsbIntern->pfnConfCallback = stcConfig.pfnConfCallback;
    
    for (i = pstcUsbIntern->u8NumberOfEndpoints; i > 0;i--)
    {
        BITMASK_CLEAR(*((uint16_t*)pstcUsbIntern->astcEndpoints[i - 1].pstcEpStatusRegister),_EPNS_BFINI);
    }
     
    pstcUsb->UDCIE_f.SUSPIE  = 1;
    pstcUsb->UDCIE_f.SOFIE   = 0;
    pstcUsb->UDCIE_f.BRSTIE  = 1;
    pstcUsb->UDCIE_f.WKUPIE  = 1;
    pstcUsb->UDCIE_f.CONFIE  = 1;
    pstcUsb->UDCIE_f.CONFN   = 0;
    
    pstcUsb->EP0IS_f.DRQIIE  = 0;
    pstcUsb->EP0OS_f.DRQOIE  = 1;
    
    pstcUsb->UDCC_f.HCONX = 0;
    
    return Ok;
}
#endif

#if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
/**
 ******************************************************************************
 ** \brief Deinit USB device
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
static en_result_t Usb_DeinitDeviceMode(stc_usbn_t* pstcUsb)
{
    uint8_t i;
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    for (i = 0;i < pstcUsbIntern->u8NumberOfEndpoints;i++)
    {
        BITMASK_CLEAR(*((uint16_t*)pstcUsbIntern->astcEndpoints[i - 1].pstcEpStatusRegister),_EPNC_EPEN);
        BITMASK_CLEAR(*((uint16_t*)pstcUsbIntern->astcEndpoints[i - 1].pstcEpStatusRegister),_EPNS_DRQIE);
    }
    
    pstcUsb->UDCIE_f.CONFN   = 0;
    pstcUsb->UDCC_f.HCONX  = 1;
    

    
    pstcUsb->UDCIE = 0;
    
    pstcUsb->EP0IS_f.DRQIIE  = 0;
    pstcUsb->EP0OS_f.DRQOIE  = 0;
    
    pstcUsb->HCNT_f.URST  = 1;  // Hold host in reset
    
    if (pstcUsbIntern->pfnConfCallback != NULL)
    {
        pstcUsbIntern->pfnConfCallback(pstcUsb);
    }
    return Ok;
}
#endif

#if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
/**
 ******************************************************************************
 ** \brief Init USB host
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
static en_result_t Usb_InitHostMode(stc_usbn_t* pstcUsb)
{
    stc_usb_config_t stcConfig;
    stc_usb_intern_data_t* pstcUsbIntern;
	  USB_ZERO_STRUCT(stcConfig);
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    
    
    pstcUsb->HCNT_f.HOST = 0;
    pstcUsb->UDCC_f.HCONX  = 1; // disable HCONX
    pstcUsb->UDCC_f.RST = 1;    // Reset USB HAL
    pstcUsb->EP1C_f.EPEN = 0;
    pstcUsb->EP2C_f.EPEN = 0;
    
    pstcUsb->EP0IS_f.DRQIIE  = 0;
    pstcUsb->EP0OS_f.DRQOIE  = 0;    
    pstcUsb->EP1C = 0x4000  | 256;
    pstcUsb->EP2C = 0x4000  | 64;
    pstcUsb->EP3C = 0x4000;
    pstcUsb->EP4C = 0x4000;
    pstcUsb->EP5C = 0x4000;
    
    Usb_InitInstance(pstcUsb);
    //Usb_SetupHalEndpoint(pstcUsb,0x81, 256, EpTypeHost, FALSE); // EP 1 for IN transfers   
    //Usb_SetupHalEndpoint(pstcUsb,0x02, 64,  EpTypeHost, FALSE); // EP 2 for OUT transfers
    
    pstcUsb->EP1C_f.DIR = 0;    // EP1 is a HOST IN endpoint
    pstcUsb->EP2C_f.DIR = 1;    // EP2 is a HOST OUT endpoint

    pstcUsb->EP1C_f.EPEN = 1;   // enable endpoint 1
    pstcUsb->EP2C_f.EPEN = 1;   // enable endpoint 2
    
    pstcUsb->HFCOMP = 0;
    pstcUsb->HRTIMER0 = 0;
    pstcUsb->HRTIMER1 = 0;
    pstcUsb->HRTIMER2 = 0;
    pstcUsb->HSTATE_f.CSTAT = 0;
    
    pstcUsbIntern->pfnHostInit(pstcUsb,&stcConfig);
    pstcUsbIntern->pfnSofCallback = stcConfig.pfnSofCallback;
    pstcUsbIntern->pfnDirqCallback = stcConfig.pfnDirqCallback;
    pstcUsbIntern->pfnCnnirqCallback = stcConfig.pfnCnnirqCallback;
    pstcUsbIntern->pfnCmpirqCallback = stcConfig.pfnCmpirqCallback;
    pstcUsbIntern->pfnUrirqCallback = stcConfig.pfnUrirqCallback;
    pstcUsbIntern->pfnRwkirqCallback = stcConfig.pfnRwkirqCallback;
    pstcUsbIntern->pfnTcanCallback = stcConfig.pfnTcanCallback;
    
    pstcUsb->HADR = 0;  // Device Address = 0
    pstcUsb->HCNT_f.HOST = 1; // Enable Host
    pstcUsb->HCNT_f.HOST = 1; // Enable Host
    
    pstcUsb->HIRQ = 0;  /* IRQ clear */
    
    pstcUsb->HRTIMER0 = 0;
    pstcUsb->HRTIMER1 = 0;
    pstcUsb->HRTIMER2 = 0;
    
    pstcUsb->HCNT_f.RETRY = 0;
    
    pstcUsb->HEOF = 0x2c9; // Set the time where token are allowed in a frame
    
    pstcUsb->EP1S_f.BFINI = 1;
    pstcUsb->EP1S_f.BFINI = 0;
    
    pstcUsb->EP2S_f.BFINI = 1;
    pstcUsb->EP2S_f.BFINI = 0;
    
    pstcUsb->HFCOMP = 0x00;  /* SOF interrupt frame No. for INTERRUPT-IN transfer */
    
    pstcUsb->HCNT_f.HOST = 1;    // Enable Host 
    pstcUsb->HCNT_f.CNNIRE = 1;  // connection interrupt 
    pstcUsb->HCNT_f.DIRE = 1;    // disconnect interrupt 
    pstcUsb->HCNT_f.CMPIRE = 1;  // token completion interrupt 
    pstcUsb->HCNT_f.SOFIRE = 1;  // SOF interrupt 
    pstcUsb->HCNT_f.HOST = 1;    // Enable Host 
    
    if (pstcUsbIntern->pfnHostPullDownHostEnable != NULL)
    {
        pstcUsbIntern->pfnHostPullDownHostEnable(UsbGpioSet);
    }
    if (pstcUsbIntern->pfnHostVbus != NULL)
    {
        pstcUsbIntern->pfnHostVbus(UsbGpioSet);
    }

    return Ok;
}
#endif

#if (!defined(USB_DISBALE_HOST_FUNCTIONALITY))
/**
 ******************************************************************************
 ** \brief Deinit USB host
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
static en_result_t Usb_DeinitHostMode(stc_usbn_t* pstcUsb)
{
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    UsbHost_DisconnectionCallback(pstcUsb);
    
    pstcUsb->UDCC_f.HCONX  = 1; // disable HCONX
    pstcUsb->UDCC_f.RST = 1;    // Reset USB HAL
    pstcUsb->EP1C = 0x4000;
    pstcUsb->EP2C = 0x4000;
    pstcUsb->EP3C = 0x4000;
    pstcUsb->EP4C = 0x4000;
    pstcUsb->EP5C = 0x4000;
    
    if (pstcUsbIntern->pfnHostPullDownHostEnable != NULL)
    {
        pstcUsbIntern->pfnHostPullDownHostEnable(UsbGpioClear);
    }
    if (pstcUsbIntern->pfnHostVbus != NULL)
    {
        pstcUsbIntern->pfnHostVbus(UsbGpioClear);
    }
    pstcUsb->HCNT = 0;
    
    return Ok;
}
#endif


/**
 ******************************************************************************
 ** \brief Init USB instance
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ******************************************************************************/
en_result_t Usb_InitInstance(stc_usbn_t* pstcUsb) 
{
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    pstcUsbIntern->bUsbInstanceSet = TRUE;
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    pstcUsbIntern->u8NumberOfEndpoints = 7;
    #else
    pstcUsbIntern->u8NumberOfEndpoints = 2;
    #endif
    
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    pstcUsbIntern->astcEndpoints[0].u8EndpointAddress = 0x80;
    pstcUsbIntern->astcEndpoints[0].u16EndpointSize   = 64;
    pstcUsbIntern->astcEndpoints[0].pstcEpStatusRegister   = &(pstcUsb->EP0IS);
    pstcUsbIntern->astcEndpoints[0].pstcEpControlRegister  = &(pstcUsb->EP0C);
    pstcUsbIntern->astcEndpoints[0].pstcEpDataRegister     = &(pstcUsb->EP0DT);
    pstcUsbIntern->astcEndpoints[0].pfnRxTxCallback = NULL;

    pstcUsbIntern->astcEndpoints[1].u8EndpointAddress = 0x00;
    pstcUsbIntern->astcEndpoints[1].u16EndpointSize   = 64;
    pstcUsbIntern->astcEndpoints[1].pstcEpStatusRegister   = &(pstcUsb->EP0OS);
    pstcUsbIntern->astcEndpoints[1].pstcEpControlRegister  = &(pstcUsb->EP0C);
    pstcUsbIntern->astcEndpoints[1].pstcEpDataRegister     = &(pstcUsb->EP0DT);
    pstcUsbIntern->astcEndpoints[1].pfnRxTxCallback = NULL;
    
    pstcUsbIntern->astcEndpoints[2].u8EndpointAddress = 0x01;
    pstcUsbIntern->astcEndpoints[2].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[2].pstcEpStatusRegister   = &(pstcUsb->EP1S);
    pstcUsbIntern->astcEndpoints[2].pstcEpControlRegister  = &(pstcUsb->EP1C);
    pstcUsbIntern->astcEndpoints[2].pstcEpDataRegister     = &(pstcUsb->EP1DT);
    pstcUsbIntern->astcEndpoints[2].pfnRxTxCallback = NULL;

    pstcUsbIntern->astcEndpoints[3].u8EndpointAddress = 0x02;
    pstcUsbIntern->astcEndpoints[3].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[3].pstcEpStatusRegister   = &(pstcUsb->EP2S);
    pstcUsbIntern->astcEndpoints[3].pstcEpControlRegister  = &(pstcUsb->EP2C);
    pstcUsbIntern->astcEndpoints[3].pstcEpDataRegister     = &(pstcUsb->EP2DT);
    pstcUsbIntern->astcEndpoints[3].pfnRxTxCallback = NULL;

    pstcUsbIntern->astcEndpoints[4].u8EndpointAddress = 0x03;
    pstcUsbIntern->astcEndpoints[4].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[4].pstcEpStatusRegister   = &(pstcUsb->EP3S);
    pstcUsbIntern->astcEndpoints[4].pstcEpControlRegister  = &(pstcUsb->EP3C);
    pstcUsbIntern->astcEndpoints[4].pstcEpDataRegister     = &(pstcUsb->EP3DT);
    pstcUsbIntern->astcEndpoints[4].pfnRxTxCallback = NULL;    

    pstcUsbIntern->astcEndpoints[5].u8EndpointAddress = 0x04;
    pstcUsbIntern->astcEndpoints[5].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[5].pstcEpStatusRegister   = &(pstcUsb->EP4S);
    pstcUsbIntern->astcEndpoints[5].pstcEpControlRegister  = &(pstcUsb->EP4C);
    pstcUsbIntern->astcEndpoints[5].pstcEpDataRegister     = &(pstcUsb->EP4DT);
    pstcUsbIntern->astcEndpoints[5].pfnRxTxCallback = NULL; 
    
    pstcUsbIntern->astcEndpoints[6].u8EndpointAddress = 0x05;
    pstcUsbIntern->astcEndpoints[6].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[6].pstcEpStatusRegister   = &(pstcUsb->EP5S);
    pstcUsbIntern->astcEndpoints[6].pstcEpControlRegister  = &(pstcUsb->EP5C);
    pstcUsbIntern->astcEndpoints[6].pstcEpDataRegister     = &(pstcUsb->EP5DT);
    pstcUsbIntern->astcEndpoints[6].pfnRxTxCallback = NULL; 
    #else
    pstcUsbIntern->astcEndpoints[0].u8EndpointAddress = 0x01;
    pstcUsbIntern->astcEndpoints[0].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[0].pstcEpStatusRegister   = &(pstcUsb->EP1S);
    pstcUsbIntern->astcEndpoints[0].pstcEpControlRegister  = &(pstcUsb->EP1C);
    pstcUsbIntern->astcEndpoints[0].pstcEpDataRegister     = &(pstcUsb->EP1DT);
    pstcUsbIntern->astcEndpoints[0].pfnRxTxCallback = NULL;

    pstcUsbIntern->astcEndpoints[1].u8EndpointAddress = 0x02;
    pstcUsbIntern->astcEndpoints[1].u16EndpointSize   = 0x00;
    pstcUsbIntern->astcEndpoints[1].pstcEpStatusRegister   = &(pstcUsb->EP2S);
    pstcUsbIntern->astcEndpoints[1].pstcEpControlRegister  = &(pstcUsb->EP2C);
    pstcUsbIntern->astcEndpoints[1].pstcEpDataRegister     = &(pstcUsb->EP2DT);
    pstcUsbIntern->astcEndpoints[1].pfnRxTxCallback = NULL;
    #endif
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Get endpoint pointer from endpointaddress
 **
 ** \param pstcUsb Pointer to USB instance
 **
 ** \param u8EndpointAddress u8EndpointAddress
 ** 
 ** \return Pointer to endpoint, NULL if no enpoint was found
 **
 ******************************************************************************/
stc_usbn_endpoint_data_t* Usb_GetEndpointPtr(stc_usbn_t* pstcUsb, uint8_t u8EndpointAddress)
{
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    if ((u8EndpointAddress & 0x1F) == 0)
    {
        if ((u8EndpointAddress & 0x80) > 0)
        {
            return &(pstcUsbIntern->astcEndpoints[0]);
        }
        else
        {
            return &(pstcUsbIntern->astcEndpoints[1]);
        }
    }
    else
    {
        if ((u8EndpointAddress & 0x1F) < (pstcUsbIntern->u8NumberOfEndpoints - 2))
        {
            if ((pstcUsbIntern->astcEndpoints[(u8EndpointAddress & 0x1F) + 1].u8EndpointAddress) == u8EndpointAddress) 
            {
                return &(pstcUsbIntern->astcEndpoints[(u8EndpointAddress & 0x1F) + 1]);
            }
        }
    }
    return NULL;
}

/**
 ******************************************************************************
 ** \brief Send data via endpoint.
 **
 ** \param pstcEpHandle Pointer to endpoint instance
 **
 ** \param pu8Data Data to write
 **
 ** \param u16Size Data size
 **
 ** \param pu16ByteCount Pointer to data written
 ** 
 ** \return Ok on success
 **
 ******************************************************************************/
en_result_t Usb_HalSend(stc_usbn_endpoint_data_t* pstcEpHandle, uint8_t* pu8Data, uint16_t u16Size, uint16_t* pu16ByteCount)
{
    boolean_t bEvenData = 0;
    USBDBGVAL8("HAL Snd ",pstcEpHandle->u8EndpointAddress);
    USBDBGVAL8("Sze ",u16Size);
    if ( pu16ByteCount != NULL )
    {
        *pu16ByteCount = 0;
    }
    
    if ( (pstcEpHandle == NULL) || (((pu8Data  == NULL) || (pu16ByteCount == NULL)) && (u16Size != 0)) )
    {
      return ErrorInvalidParameter ;
    }
    
    if (BITMASK_ISCLEARED(*(pstcEpHandle->pstcEpStatusRegister),_EPNS_DRQ)) 
    {
        return ErrorNotReady;
    }
    
    if (u16Size > pstcEpHandle->u16EndpointSize)
    {
        u16Size = pstcEpHandle->u16EndpointSize;
    }
    USBDBGVAL8("New Sze ",u16Size);
    bEvenData = ((u16Size % 2) == 0);
    
    if (u16Size == 0) 
    {
        bEvenData = TRUE;
    } 
    else
    {
        for(*pu16ByteCount = 0;*pu16ByteCount < (u16Size - 1);*pu16ByteCount = *pu16ByteCount + 2)
        {
            *(pstcEpHandle->pstcEpDataRegister) = *((uint16_t*)pu8Data);
            pu8Data += 2;
        }
    }
    
    if (!bEvenData)
    {
        *pu16ByteCount = *pu16ByteCount + 1;
        *((__IO uint8_t*)(pstcEpHandle->pstcEpDataRegister)) = *pu8Data;
    }
    
    BITMASK_CLEAR(*(pstcEpHandle->pstcEpStatusRegister), _EPNS_DRQ);
    return Ok;
}


/**
 ******************************************************************************
 ** \brief Read data via endpoint.
 **
 ** \param pstcEpHandle Pointer to endpoint instance
 **
 ** \param pu8Data Databuffer to write in
 **
 ** \param pu16ByteCount Pointer to data read
 ** 
 ** \return Ok on success
 **
 ******************************************************************************/
en_result_t Usb_HalReceive(stc_usbn_endpoint_data_t* pstcEpHandle, uint8_t* pu8Data, uint16_t* pu16ByteCount)
{
    boolean_t bEvenData = 0;
    
    uint16_t u16i;
    
    if ( pu16ByteCount != NULL )
    {
        *pu16ByteCount = 0;
    }
    
    if (pstcEpHandle == NULL)
    {
      return ErrorInvalidParameter ;
    }
    
    if (BITMASK_ISCLEARED(*(pstcEpHandle->pstcEpStatusRegister),_EPNS_DRQ)) 
    {
        return ErrorNotReady;
    }
    
    *pu16ByteCount = (*(pstcEpHandle->pstcEpStatusRegister)) & 0x7F;
    
    bEvenData = (((*pu16ByteCount) % 2) == 0);
    
    if (*pu16ByteCount == 0) 
    {
        bEvenData = TRUE;
    } 
    else
    {
        for(u16i = 0;u16i < (*pu16ByteCount - 1);u16i += 2)
        {
            *((uint16_t*)pu8Data) = *(pstcEpHandle->pstcEpDataRegister);
            pu8Data += 2;
        }
    }
    
    if (!bEvenData)
    {
        *pu8Data = *((__IO uint8_t*)(pstcEpHandle->pstcEpDataRegister));
    }
    BITMASK_CLEAR(*(pstcEpHandle->pstcEpStatusRegister), _EPNS_DRQ);
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Get information if the specified enpoint is ready
 **
 ** \param pstcEpHandle Endpoint handle 
 **
 ** \return TRUE if ready
 **
 ******************************************************************************/
boolean_t Usb_HalEpReady(stc_usbn_endpoint_data_t* pstcEpHandle)
{
    if (pstcEpHandle == NULL)
    {
        return FALSE;
    }
    
    if (BITMASK_ISCLEARED(*(pstcEpHandle->pstcEpControlRegister),_EPNC_EPEN))
    {
        return FALSE;
    }
    
    if (BITMASK_ISCLEARED(*(pstcEpHandle->pstcEpStatusRegister), _EPNS_DRQ)) 
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/**
 ******************************************************************************
 ** Usb Device get current configuration
 ** 
 ** \param pstcUsb USB handle
 **
 *****************************************************************************/
uint8_t Usb_GetConfiguration(stc_usbn_t* pstcUsb)
{
    if (pstcUsb->UDCC_f.HCONX == 1)
    {
        return 0;
    }
    return pstcUsb->UDCIE_f.CONFN;
}


/**
 ******************************************************************************
 ** SOF callback - Realize Timeouts
 ** 
 ** \param pstcUsb USB handle
 **
 *****************************************************************************/
static void Usb_TimeoutHandler(stc_usbn_t* pstcUsb)
{
    volatile uint8_t i;
    boolean_t bNothingToBeDone = TRUE;
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    for(i = 0;i<5;i++)
    {
        if (pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler != 0)
        {
            bNothingToBeDone = FALSE;
            if (pstcUsbIntern->pstcUsbTimeOutHandler[i].u16TimeOut == 0)
            {
                pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler(pstcUsb);
                pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler = 0;
                return;
            }
            else
            {
                pstcUsbIntern->pstcUsbTimeOutHandler[i].u16TimeOut--;
            }
        }
    }
    if (bNothingToBeDone == TRUE)
    {
         pstcUsbIntern->bSofTimeoutEnabled = FALSE;
    }
}

/**
 ******************************************************************************
 ** Add a timeout handler
 **
 ** \param pstcUsb USB handle
 ** 
 ** \param Handler handler executed after timeout
 ** 
 ** \param u16TimeOut timeout in ms
 **
 ** \return TRUE if command was successful
 **
 *****************************************************************************/
boolean_t Usb_AddTimeOut(stc_usbn_t* pstcUsb, void (* Handler)(stc_usbn_t* pstcUsb), uint16_t u16TimeOut)
{
    volatile uint8_t i;
    boolean_t bAdded = FALSE;
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    for(i = 0;i<5;i++)
    {
        if (pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler == 0)
        {
            pstcUsbIntern->bSofTimeoutEnabled = FALSE;
            pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler = Handler;
            pstcUsbIntern->pstcUsbTimeOutHandler[i].u16TimeOut = u16TimeOut;
            pstcUsbIntern->bSofTimeoutEnabled = TRUE;
            bAdded = TRUE;
            break;
        }
    }
    pstcUsbIntern->bSofTimeoutEnabled = TRUE;
    return bAdded;
}

/**
 ******************************************************************************
 ** Remove a timeout handler
 **
 ** \param pstcUsb USB handle
 ** 
 ** \param Handler handler executed after timeout
 ** 
 ** \return none
 **
 *****************************************************************************/
void Usb_RemoveTimeOut(stc_usbn_t* pstcUsb, void (* Handler)(stc_usbn_t* pstcUsb))
{
    volatile uint8_t i;
    stc_usb_intern_data_t* pstcUsbIntern;
    pstcUsbIntern = UsbGetInternDataPtr(pstcUsb);
    for(i = 0;i<5;i++)
    {
        if (pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler == Handler)
        {
            pstcUsbIntern->pstcUsbTimeOutHandler[i].Handler = 0;
            pstcUsbIntern->pstcUsbTimeOutHandler[i].u16TimeOut = 0;
            break;
        }
    }
}

/******************************************************************************/
/* Interrupt Handling                                                         */
/******************************************************************************/


                  
#ifdef WITHOUT_L3
  #if (L3_PERIPHERAL_ENABLE_USB0 == L3_ON)
  
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    /**
     ******************************************************************************
     ** USB 0 function interrupt handler (only used in standalone mode)
     **
     *****************************************************************************/
    void USBF_Handler(void)
    {
        UsbIrqHandlerF((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
    }
    #endif
    
    /**
     ******************************************************************************
     ** USB 0 interrupt handler (only used in standalone mode)
     **
     *****************************************************************************/
    void USB_Handler(void)
    {
        UsbIrqHandler((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
    }
    
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
    /**
     ******************************************************************************
     ** USB 0 function interrupt handler (only used in standalone mode)
     **
     *****************************************************************************/
    void USB0F_Handler(void)
    {
        UsbIrqHandlerF((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
    }
    #endif
    
    /**
     ******************************************************************************
     ** USB 0 interrupt handler (only used in standalone mode)
     **
     *****************************************************************************/
    void USB0_Handler(void)
    {
        UsbIrqHandler((stc_usbn_t*)&USB0, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb0].stcInternData));
    }
  #endif
  #if (L3_PERIPHERAL_ENABLE_USB1 == L3_ON)
    #if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))

    /**
     ******************************************************************************
     ** USB 1 function interrupt handler (only used in standalone mode)
     **
     *****************************************************************************/
    void USB1F_Handler(void)
    {
        UsbIrqHandlerF((stc_usbn_t*)&USB1, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb1].stcInternData));
    }
    #endif
    
    
    /**
     ******************************************************************************
     ** USB 1 interrupt handler (only used in standalone mode)
     **
     *****************************************************************************/
    void USB1_Handler(void)
    {
        UsbIrqHandler((stc_usbn_t*)&USB1, &(m_astcUsbInstanceDataLut[UsbInstanceIndexUsb1].stcInternData));
    }
  #endif
#endif

#if (!defined(USB_DISBALE_DEVICE_FUNCTIONALITY))
/**
 ******************************************************************************
 ** USB function interrupt handler (used for HAL endpoint interrupts)
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcUsbInternData Internal data USB handle 
 **
 *****************************************************************************/
void UsbIrqHandlerF(stc_usbn_t* pstcUsb, stc_usb_intern_data_t* pstcUsbInternData)
{ 
    volatile uint32_t i;
    uint32_t temp;

    for(i = 2;i < pstcUsbInternData->u8NumberOfEndpoints;i++)
    {
        if ((BITMASK_ISSET(*(pstcUsbInternData->astcEndpoints[i].pstcEpStatusRegister),_EPNS_DRQ)) && (BITMASK_ISSET(*(pstcUsbInternData->astcEndpoints[i].pstcEpStatusRegister),_EPNS_DRQIE)))
        {
            if (pstcUsbInternData->astcEndpoints[i].pfnRxTxCallback != NULL)
            {
                temp = i; //needed to avoid warning, because of unknown volatile access
                pstcUsbInternData->astcEndpoints[(uint8_t)temp].pfnRxTxCallback(pstcUsb,&(pstcUsbInternData->astcEndpoints[(uint8_t)temp]));
            }
        }
    }
}
#endif


/**
 ******************************************************************************
 ** USB interrupt handler
 **
 ** \param pstcUsb USB handle
 **
 ** \param pstcUsbInternData Internal data USB handle 
 **
 *****************************************************************************/
void UsbIrqHandler(stc_usbn_t* pstcUsb, stc_usb_intern_data_t* pstcUsbInternData)
{
    volatile uint32_t i;
    
    if (pstcUsbInternData->bDeviceActive)
    {
        if (pstcUsb->UDCS_f.CONF == 1)
        {
            pstcUsb->UDCS_f.CONF = 0;
            if (pstcUsbInternData->pfnConfCallback != NULL)
            {
                pstcUsbInternData->pfnConfCallback(pstcUsb);
            }
        }
        
        
        if (pstcUsb->EP0C_f.STAL == 1)
        {
            pstcUsb->EP0C_f.STAL = 0;
        }
        
        if ((pstcUsb->EP0OS_f.DRQO == 1) && (pstcUsb->EP0OS_f.DRQOIE == 1))
        {
            if (pstcUsb->UDCS_f.SETP == 1)
            {
                pstcUsb->UDCS_f.SETP = 0;
                if (pstcUsbInternData->pfnSetpCallback != NULL)
                {
                    USBDBG(">> SETP\n");
                    pstcUsbInternData->pfnSetpCallback(pstcUsb);
                }
            }
            else
            {
                if (pstcUsbInternData->astcEndpoints[1].pfnRxTxCallback != NULL)
                {
                    pstcUsbInternData->astcEndpoints[1].pfnRxTxCallback(pstcUsb,&(pstcUsbInternData->astcEndpoints[1]));
                }
                else
                {
                    USBDBG("EP0");
                    if ((pstcUsb->EP0OS & 0x1F) == 0)
                    {
                       USBDBG(" zerro package "); 
                       // Zerro Package Received
                    }
                    pstcUsb->EP0OS_f.DRQO = 0;
                    USBDBG("received (not handled)\n");
                    
                }
            }
        }
    
        if ((pstcUsb->EP0IS_f.DRQI == 1) && (pstcUsb->EP0IS_f.DRQIIE == 1))
        {
            if (pstcUsbInternData->astcEndpoints[0].pfnRxTxCallback != NULL)
            {
                pstcUsbInternData->astcEndpoints[0].pfnRxTxCallback(pstcUsb,&(pstcUsbInternData->astcEndpoints[0]));
            }
        }
        
        if (pstcUsb->UDCS_f.WKUP == 1)
        {
            pstcUsb->UDCS_f.WKUP = 0;
            if (pstcUsbInternData->pfnWkupCallback != NULL)
            {
                pstcUsbInternData->pfnWkupCallback(pstcUsb);
            }
        }
        if (pstcUsb->UDCS_f.BRST == 1)
        {
            pstcUsb->UDCS_f.BRST = 0;
            for (i = pstcUsbInternData->u8NumberOfEndpoints; i > 0;i--)
            {
                BITMASK_SET(*((uint16_t*)pstcUsbInternData->astcEndpoints[i - 1].pstcEpStatusRegister),_EPNS_BFINI);
                BITMASK_CLEAR(*((uint16_t*)pstcUsbInternData->astcEndpoints[i - 1].pstcEpStatusRegister),_EPNS_BFINI);
            }
            pstcUsb->EP0IS_f.DRQIIE  = 0;
            pstcUsb->EP0OS_f.DRQOIE  = 1;
            if (pstcUsbInternData->pfnBrstCallback != NULL)
            {
                pstcUsbInternData->pfnBrstCallback(pstcUsb);
            }
        }
        if (pstcUsb->UDCS_f.SOF == 1)
        {
            pstcUsb->UDCS_f.SOF = 0;
            if (pstcUsbInternData->bSofTimeoutEnabled == TRUE)
            {
                Usb_TimeoutHandler(pstcUsb);
            }
            
            if (pstcUsbInternData->pfnSofCallback != NULL)
            {
                pstcUsbInternData->pfnSofCallback(pstcUsb);
            }
        }
        if (pstcUsb->UDCS_f.SUSP == 1)
        {
            pstcUsb->UDCS_f.SUSP = 0;
            if (pstcUsbInternData->pfnSuspCallback != NULL)
            {
                pstcUsbInternData->pfnSuspCallback(pstcUsb);
            }
        }
    }
    if (pstcUsbInternData->bHostActive)
    {
        if (pstcUsb->HIRQ_f.SOFIRQ == 1)
        {
            pstcUsb->HIRQ_f.SOFIRQ = 0;
            
            if (pstcUsbInternData->bSofTimeoutEnabled == TRUE)
            {
                Usb_TimeoutHandler(pstcUsb);
            }
            
            if (pstcUsbInternData->pfnSofCallback != NULL)
            {
                pstcUsbInternData->pfnSofCallback(pstcUsb);
            }
        }
        if (pstcUsb->HIRQ_f.DIRQ == 1)
        {
            pstcUsb->HIRQ_f.DIRQ = 0;
            if (pstcUsbInternData->pfnDirqCallback != NULL)
            {
                pstcUsbInternData->pfnDirqCallback(pstcUsb);
            }
        }
        if (pstcUsb->HIRQ_f.CNNIRQ == 1)
        {
            if (pstcUsbInternData->pfnCnnirqCallback != NULL)
            {
                pstcUsbInternData->pfnCnnirqCallback(pstcUsb);
            }
            else
            {
                pstcUsb->HIRQ_f.CNNIRQ = 0;
            }
        }
        if (pstcUsb->HIRQ_f.CMPIRQ == 1)
        {
            pstcUsb->HIRQ_f.CMPIRQ = 0;
            if (pstcUsbInternData->pfnCmpirqCallback != NULL)
            {
                pstcUsbInternData->pfnCmpirqCallback(pstcUsb);
            }
        }
        if (pstcUsb->HIRQ_f.URIRQ == 1)
        {
            if (pstcUsbInternData->pfnUrirqCallback != NULL)
            {
                pstcUsbInternData->pfnUrirqCallback(pstcUsb);
            }
            else
            {
                pstcUsb->HIRQ_f.URIRQ = 0;
            }
        }
        if (pstcUsb->HIRQ_f.RWKIRQ == 1)
        {
            pstcUsb->HIRQ_f.RWKIRQ = 0;
            if (pstcUsbInternData->pfnRwkirqCallback != NULL)
            {
                pstcUsbInternData->pfnRwkirqCallback(pstcUsb);
            }
        }
        if (pstcUsb->HIRQ_f.TCAN == 1)
        {
            pstcUsb->HIRQ_f.TCAN = 0;
            if (pstcUsbInternData->pfnTcanCallback != NULL)
            {
                pstcUsbInternData->pfnTcanCallback(pstcUsb);
            }
        }
    }
}

//@} // UsbGroup



#endif // #if (defined(L3_PERIPHERAL_USB_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

