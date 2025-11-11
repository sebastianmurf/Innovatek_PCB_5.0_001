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
/** \file uart.c
 **
 ** uart file
 **
 ** History:
 **   - 2011-03-30  1.0  MSc  First Version for FM3
 **   - 2012-07-17    1.1  MSc  base_type.h -> base_types_l3.h
 *************************************************************************/

#ifndef __UART_H__
#define __UART_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "mcu.h"
#include "base_types_l3.h"
#include <string.h>

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define USE_PRINTF       0
#define UART_USE_L3      1

#if (USE_PRINTF == 1)
    #include <stdio.h>
#endif
#define  HEAP_SIZE     16

#ifndef CLOCK_FREQ
    #ifdef __PLLCLK
        #define CLOCK_FREQ __PLLCLK
    #else
        #define CLOCK_FREQ 40000000UL
    #endif
#endif

#if (UART_USE_L3 == 0)
    #define UART_BAUDRATE(x)  (uint16_t)((CLOCK_FREQ/((uint32_t)x - 1))/2)
#else
    #define UART_BAUDRATE(x)  x
#endif

#if UART_USE_L3 == 1
    #include "mfs.h"
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

boolean_t Uart_Init(uint8_t u8Uart, uint32_t Baudrate);
void Uart_Putch(uint8_t u8Uart, char_t c);
void putch(char_t c);
char_t Uart_Getch(uint8_t u8Uart);
char_t getch();
void Uart_Puts(uint8_t u8Uart, char_t* String);
int puts(const char_t* String);
void puthex(uint32_t n, uint8_t digits);
void putdec(uint32_t x);
unsigned long ASCIItobin(uint8_t k);
void receive_line(void);
int receive_line_echo(int *cnt);
int scan_line(char_t *str) ;
unsigned long Inputhex(uint8_t digits);
char upcase(char_t k);

/* Low-Level functions */
#if (USE_PRINTF == 1)
    int __write(int , char *, unsigned int);
    int __close(int);
    int __read(int , char *, unsigned int);
#endif /* (USE_PRINTF == 1) */

#endif /* __UART_H__ */
