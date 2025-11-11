/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH                  */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user�s obligation to fully test the software in its      */
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
/* To the maximum extent permitted by applicable law, FSEU�s liability  */
/* is restricted to intention and gross negligence.                     */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.4)                                                               */
/************************************************************************/
/** \file board.h
 **
 ** for SK-FM3-48PMC-USBSTICK
 **
 ** History:
 **   - 2012-02-03  1.0  MSc  First Version
 *****************************************************************************/

#ifndef __LED_H__
#define __LED_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "mcu.h"
#include "base_types_l3.h"
#include "ppg.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define LEDS_DIMMABLE 0

#if (LEDS_DIMMABLE == 1)
  #define LED_RED       (1<<10)
  #define LED_GREEN     (1<<12)
  #define LED_BLUE      (1<<15)
	
  #define LED_MASK    (LED_RED  | LED_GREEN | LED_BLUE )

  #define LED_INIT     FM3_GPIO->PFR3 &=~LED_MASK; FM3_GPIO->PDOR3 &= ~LED_MASK; FM3_GPIO->DDR3 |= LED_MASK;

  #define LED_ON(x)     Ppg_SetDuty(x,255);
  #define LED_OFF(x)    Ppg_SetDuty(x,0);
  #define LED_TOG(x)	FM3_GPIO->PDOR3 ^= x
  #define LED_DUTY(x,y) Ppg_SetDuty(x,y);

#else
  #define LED_RED       (1<<10)
  #define LED_GREEN     (1<<12)
  #define LED_BLUE      (1<<15)

  #define LED_MASK    (LED_RED  | LED_GREEN | LED_BLUE )

  #define LED_INIT     FM3_GPIO->PFR3 &=~LED_MASK; FM3_GPIO->PDOR3 &= ~LED_MASK; FM3_GPIO->DDR3 |= LED_MASK;

  #define LED_OFF(x)    FM3_GPIO->PDOR3 |= x
  #define LED_ON(x)    	FM3_GPIO->PDOR3 &= ~x
  #define LED_TOG(x)	FM3_GPIO->PDOR3 ^= x

#endif

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/


#endif /* __LED_H__ */
