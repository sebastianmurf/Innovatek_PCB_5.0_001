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
/** \file ppg.h
 **
 ** for SK-FM3-48PMC-USBSTICK
 **
 ** History:
 **   2012-02-03  1.0  MSc  First version for SK-FM3-48PMC-USBSTICK
 **   2012-07-17  1.1  MSc  base_type.h -> base_types_l3.h
 **   2012-08-31  1.2  MSc  support for L3 rename ppg.c --> ledpwm.c
 *****************************************************************************/

#ifndef __PPG_H__
#define __PPG_H__
#include "mcu.h"
#include "base_types_l3.h"

#define LEDPWM_USE_L3   1

#define PWM_CYCLE 255 //3125 

#if LEDPWM_USE_L3 == 1
#include "l3.h"
#include "bt.h"
#endif
   
void LedPwm_Init(void);
void LedPwm_SetDuty(uint8_t u8Channel, uint8_t u8Duty);
void LedPwm_SetCycle(uint8_t u8Channel, uint8_t u8Cycle);

#endif /* __PPG_H__ */