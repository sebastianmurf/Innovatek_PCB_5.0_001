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
/** \file adc.h
 **
 ** for SK-FM3-48PMC-USBSTICK
 **
 ** History:
 **   2012-02-03  1.0  MSc  First version for SK-FM3-48PMC-USBSTICK
 **   2012-07-17  1.1  MSc  base_type.h -> base_types_l3.h
 **   2012-08-31  1.2  MSc  l3 support, rename adc -> sensoradc
 *****************************************************************************/

#ifndef __SENSORADC_H__
#define __SENSORADC_H__
#include "mcu.h"
#include "base_types_l3.h"

#define SENSORADC_USE_L3 1

#if SENSORADC_USE_L3 == 1
#include "l3.h"
#include "adc.h"
#endif

#define CALC_OHM(x)  ((1000UL*x)/(x))
#define CALC_TEMP(x) (int8_t)((double)3977/(double)(log((double)x/(double)(1024-x))+(double)3977/(double)298)-273)
void SensorAdc_AdcHardwareInit(void);
uint16_t SensorAdc_Read(uint8_t u8Channel);
uint32_t SensorAdc_ReadSensorOhm(uint8_t u8Channel);

#endif /* __ADC_H__ */
