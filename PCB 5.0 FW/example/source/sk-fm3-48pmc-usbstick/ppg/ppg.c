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
/** \file ppg.c
 **
 ** for SK-FM3-48PMC-USBSTICK
 **
 ** History:
 **   2012-02-03  1.0  MSc  First version for SK-FM3-48PMC-USBSTICK
 *****************************************************************************/
#include "ppg.h"


unsigned char PwmDuty=0,PwmCycle=200; //variable that stores pwm current duty & cycle values

void Ppg_Init(void)
{
  /*Enable peripherial ports to be used with PWM*/
  FM3_GPIO->PFR3   = 0x8000;//P3F AS PWM FUNCTION. OK TESTED
  FM3_GPIO->EPFR04 = 0x08080800;//OK TESTED
  FM3_GPIO->EPFR05 = 0x00000800;//TIOA5_1 AS PWM OUTPUT PIN. OK TESTED
  //FM3_GPIO->PFR3   = 0x4000;//P3E AS PWM FUNCTION. NOT OK
  //FM3_GPIO->EPFR04 = 0x08080800;//NOT OK
  //FM3_GPIO->EPFR05 = 0x0000008;//TIOA4_1 AS PWM OUTPUT PIN. NOT OK
  
  /* bit11:RTGEN=0b0 reboot by triger disable */
  /* bit10:PMSK=0b0 output mask disable */
  /* bit9,8:EGS1,0=0b00 triger input none  */
  /* bit7:T32=0b0 16bit mode */
  /* bit6~4:FMD2~0=0b001 PWM timer mode */
  /* bit3:OSEL=0b0 output level normal */
  /* bit2:MDSE=0b0 output mode continue */
  //FM3_BT0_PWM->TMCR = 0x3010;
  //FM3_BT2_PWM->TMCR = 0x3010;
  //FM3_BT4_PWM->TMCR = 0x3010;
  FM3_BT5_PWM->TMCR = 0x3010;
  
  /* bit0+TMCR.bit14~12:CKS3~0=0b0011 count clock prescaler PCLK1/128 */
  FM3_BT0_PWM->TMCR2 = 0x00;
  FM3_BT2_PWM->TMCR2 = 0x00;
  FM3_BT4_PWM->TMCR2 = 0x00;
  
 /* bit6:TGIE=0b0 triger interrupt disable */
 /* bit5:DTIE=0b0 duty match interrupt disable */
 /* bit4:UDIE=0b0 under flow interrupt disable */
  //FM3_BT0_PWM->STC   = 0x00;
  //FM3_BT2_PWM->STC   = 0x00;
  //FM3_BT4_PWM->STC   = 0x00;
  FM3_BT5_PWM->STC   = 0x00;
  
  /* Down count*/
  //Ppg_SetCycle(0,PWM_CYCLE);
  //Ppg_SetCycle(2,PWM_CYCLE);
  //Ppg_SetCycle(4,PWM_CYCLE);
  Ppg_SetCycle(5,PWM_CYCLE);
  
  /* bit1:CTEN=0b1 down counter enable */
  /* bit0:STRG=0b1 started by software */
  //FM3_BT0_PWM->TMCR |= 0x03;
  //FM3_BT2_PWM->TMCR |= 0x03;
  //FM3_BT4_PWM->TMCR |= 0x03;
  FM3_BT5_PWM->TMCR |= 0x03;
}

void Ppg_SetDuty(uint8_t u8Channel, uint8_t u8Duty)
{
  switch(u8Channel)
  {
  case 0:
      FM3_BT0_PWM->PDUT = u8Duty;
      break;
  case 1:
      FM3_BT1_PWM->PDUT = u8Duty;
      break;
  case 2:
      FM3_BT2_PWM->PDUT = u8Duty;
      break;
  case 3:
      FM3_BT3_PWM->PDUT = u8Duty;
      break;
  case 4:
      FM3_BT4_PWM->PDUT = u8Duty;
      break;
  case 5:
      FM3_BT5_PWM->PDUT = u8Duty;
      break;
  default:
    break;
  }
}

void Ppg_SetCycle(uint8_t u8Channel, uint8_t u8Cycle)
{
  switch(u8Channel)
  {
  case 0:
      FM3_BT0_PWM->PCSR = u8Cycle;
      break;
  case 1:
      FM3_BT1_PWM->PCSR = u8Cycle;
      break;
  case 2:
      FM3_BT2_PWM->PCSR = u8Cycle;
      break;
  case 3:
      FM3_BT3_PWM->PCSR = u8Cycle;
      break;
  case 4:
      FM3_BT4_PWM->PCSR = u8Cycle;
      break;
  case 5:
      FM3_BT5_PWM->PCSR = u8Cycle;
      break;
  default:
    break;
  }
}