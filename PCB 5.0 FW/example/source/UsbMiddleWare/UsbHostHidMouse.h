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
/** \file UsbHostHidMouse.h
 **
 ** Part of FSEU USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version 
 **   - 2011-08-24    1.1  MSc  Some fixes in X/Y calculation
 **   - 2012-06-05    1.2  MSc  New verison for use with new USB driver for FM3 L3
 **                             Rename HidMouse.h -> UsbHostHidMouse.h
 *****************************************************************************/


#ifndef __USBHOSTHIDMOUSE_H__
#define __USBHOSTHIDMOUSE_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "UsbHost.h"

#ifndef USB_DISBALE_HOST_FUNCTIONALITY

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/
typedef uint8_t buttons_t;

typedef struct stc_point
{
    uint32_t u32X;
    uint32_t u32Y;
} stc_point_t;

typedef struct stc_pointdifference
{
    int8_t i8X;
    int8_t i8Y;
} stc_pointdifference_t;

typedef struct stc_mousedata
{
    stc_point_t   stcPosition;
    buttons_t tButtons;
    stc_pointdifference_t    stcDiffXY;
    uint32_t  u32ScrollPosition;
    int8_t    i8Scrolling;    
} stc_mousedata_t;


typedef uint8_t MouseEventType_t;
#define MOUSEEVENT_POSITIONX_CHANGED 1
#define MOUSEEVENT_POSITIONY_CHANGED 2
#define MOUSEEVENT_BUTTON_CHANGED    4
#define MOUSEEVENT_SCROLLING_CHANGED  8
#define MOUSEEVENT_ANY_CHANGE        0xFF
/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/



/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

boolean_t UsbHostHidMouse_InitHandler(stc_usbn_t* pstcUsb, uint8_t* pu8Configuration, uint32_t u32Length);
boolean_t UsbHostHidMouse_DeinitHandler(stc_usbn_t* pstcUsb);
void UsbHostHidMouse_SetCallback(void (*Handler)(MouseEventType_t stcEventType, stc_mousedata_t stcInternalMouseData));
void UsbHostHidMouse_RemoveCallback(void);
stc_point_t UsbHostHidMouse_GetCurrentPosition(void);
void UsbHostHidMouse_SetCurrentPosition(uint32_t u32X, uint32_t u32Y);
uint32_t UsbHostHidMouse_GetCurrentScrollPosition(void);
void UsbHostHidMouse_SetCurrentScrollPosition(uint32_t u32ScrollPosition);
buttons_t UsbHostHidMouse_GetButtons(void);
boolean_t UsbHostHidMouse_Moved(void);
boolean_t UsbHostHidMouse_DriverActive(void);

#endif /* (USE_USB_HOST == 1) */
#endif /* __HIDMOUSE_H__*/
