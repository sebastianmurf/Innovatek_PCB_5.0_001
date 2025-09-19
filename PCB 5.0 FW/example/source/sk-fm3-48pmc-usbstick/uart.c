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
 *************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "uart.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


#define UARTREG(x,y)    (*(((volatile uint8_t*)UART_BASE[x]) + y))
/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/* global variables */
char line_buffer[255];
char *line_buffer_ptr;

/* constants */
const char ASCII[] = "0123456789ABCDEF";
static uint8_t u8DefaultUart = 0;



#ifndef UART_USE_L3
    #error UART_USE_L3 is missing
#endif

#if (USE_PRINTF == 1)
    char_t tbuf;

    static   long        brk_siz = 0;
    #if    HEAP_SIZE
        typedef  int         _heap_t;
        #define ROUNDUP(s)   (((s)+sizeof(_heap_t)-1)&~(sizeof(_heap_t)-1))
        static   _heap_t     _heap[ROUNDUP(HEAP_SIZE)/sizeof(_heap_t)];
        #define              _heap_size       ROUNDUP(HEAP_SIZE)
    #else
        extern  char         *_heap;
        extern  long         _heap_size;
    #endif
#endif
#if ((UART_USE_L3 == 1) && (USE_PRINTF == 1))
    #error Printf with L3 currently not supported
#endif        

#if (UART_USE_L3 == 0)
FM3_MFS03_UART_TypeDef* UART_BASE[] = {
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS0_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS1_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS2_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS0_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS3_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS0_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS5_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS0_UART_BASE),
        ((FM3_MFS03_UART_TypeDef *)FM3_MFS0_UART_BASE)};
#endif
/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/

#if (UART_USE_L3 == 0)


boolean_t Uart_Init(uint8_t u8Uart, uint32_t Baudrate)
{
    UART_BASE[u8Uart]->SCR =  0x80;
    UART_BASE[u8Uart]->SMR =  0x01;
    UART_BASE[u8Uart]->SSR =  0x00;
    UART_BASE[u8Uart]->ESCR = 0x00;
    UART_BASE[u8Uart]->BGR = (uint16_t)Baudrate; //(uint16_t)BGRTemp;
    UART_BASE[u8Uart]->SCR = 0x03;
    if (u8Uart == 0)
    {
        FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02);
        FM3_GPIO->EPFR07 |= (1 << 6);
        FM3_GPIO->ADE &= ~(1 << 7);
    }
    u8DefaultUart = u8Uart;
    return TRUE;
}

void Uart_Putch(uint8_t u8Uart, char_t c)
{
    while (!(UART_BASE[u8Uart]->SSR & (1 << 1)));		/* wait for transmit buffer empty 	*/
    UART_BASE[u8Uart]->RDR = c;            /* put ch into buffer	*/
}

char_t Uart_Getch(uint8_t u8Uart)
{
    while(!(UART_BASE[u8Uart]->SSR & (1 << 2)));			/* wait for data in buffer */
    if (UART_BASE[u8Uart]->SSR & 0x28) 
    {	/* overrun or parity error */
        return 0;//(-1);
    } 
    else
    {
	return (UART_BASE[u8Uart]->RDR);
    }
}

#else /*  WITH L3 */

static stc_mfs_config_t stcConfig ; 

static stc_mfsn_t* pstcMfs = NULL;

static char_t pcRxBuffer[8];
static char_t pcTxBuffer[8];

boolean_t Uart_Init(uint8_t u8Uart, uint32_t Baudrate)
{
    switch (u8Uart)
    {
    case 0:
          FM3_GPIO->PFR2 |= (1 << 0x01) | (1 << 0x02);
          FM3_GPIO->EPFR07 |= (1 << 6);
          FM3_GPIO->ADE &= ~(1 << 7);
          stcConfig.enMode         = MfsAsynchronous0;
          stcConfig.enClockSource  = MfsUseInternalBaudRateGenerator;
          stcConfig.u32DataRate                  = 115200;
          stcConfig.enParity                     = MfsParityNone;
          stcConfig.enStartStopBit               = MfsOneStartOneStop;
          stcConfig.u8CharLength                 = MfsEightBits;
          stcConfig.bBitDirection                = 0; // LSB first
          stcConfig.bSyncClockInversion          = FALSE;
          stcConfig.bSyncClockOutputDelay        = FALSE;
          stcConfig.pcTxBuffer                   = pcTxBuffer;
          stcConfig.u16TxBufferSize              = 8;
          stcConfig.pcRxBuffer                   = pcRxBuffer;
          stcConfig.u16RxBufferSize              = 8;
          stcConfig.enTxCallbackMode             = MfsOnTxBufferEmpty;
          stcConfig.pfnTxCallback                = NULL;
          stcConfig.u16RxCallbackBufferFillLevel = 1;
          stcConfig.pfnRxCallback                = NULL; 
          Mfs_Init((stc_mfsn_t*)&MFS0,&stcConfig);
          pstcMfs = (stc_mfsn_t*)&MFS0 ; 
          u8DefaultUart = u8Uart;
          break;
    default:
          return FALSE;
    }
    return TRUE;
}

void Uart_Putch(uint8_t u8Uart, char_t c)
{
    Mfs_Write(pstcMfs, (char_t*)&c,1, TRUE);
}

char_t Uart_Getch(uint8_t u8Uart)
{
    uint16_t u16DataRead;
    char_t c = -1;
    Mfs_Read(pstcMfs, (char_t*)c,&u16DataRead,1,TRUE);
    return c;
}

#endif /* WITHOUT L3 */



void putch(char_t c)
{
    Uart_Putch(u8DefaultUart, c);
}

char_t getch()
{
    return Uart_Getch(u8DefaultUart);
}

void Uart_Puts(uint8_t u8Uart, char_t* String)
{
	while (*String != '\0')
	{ 
        if(*String == '\n') Uart_Putch(u8Uart,'\r');
        Uart_Putch(u8Uart,*String++);        /* send every char of string */
    }
}

int puts(const char_t* String)
{
    Uart_Puts(u8DefaultUart,(char_t*)String);
    return 0;
}

/*****************************************************************************
 *  DESCRIPTION:    sends a x-digit Hex-number (as ASCII charcaters)
 *                  to terminal via ext. UART
 *
 *  PARAMETERS:     Value and number of Hex-digits (e.g. FF = 2 Hex digits)
 *
 *  RETURNS:        NONE
 *****************************************************************************/
void puthex(uint32_t n, uint8_t digits)
{
	unsigned char i,ch,div=0;

	puts("0x");				/* hex string */
	div=(digits-1) << 2;	/* init shift divisor */

	for (i=0; i<digits; i++) {
	  ch = (n >> div) & 0xF;/* get hex-digit value */
	  putch(ASCII[ch]);		/* prompt to terminal as ASCII */
	  div-=4;				/* next digit shift */
   }
}

/*****************************************************************************
 *  DESCRIPTION:    send a x-digit Dec-number (as ASCII charcaters)
 *                  to terminal via ext. UART
 *
 *  PARAMETERS:     integer value
 *
 *  RETURNS:        None
 *****************************************************************************/
void putdec(uint32_t x)
{
	int i;
	char buf[9];
	
	buf[8]='\0';				/* end sign of string */
	
	for (i=8; i>0; i--) 
	{
       buf[i-1] = ASCII[x % 10];
	   x = x/10;

	}

	for (i=0; buf[i]=='0'; i++) // no print of zero 
	{
	   buf[i] = ' ';
    }
	puts(buf);					/* send string */
}


/*****************************************************************************
 *  DESCRIPTION:    Converts ASCII inputs (0..8,a..f,A..F) to num. value
 *
 *  PARAMETERS:     ASCII input
 *
 *  RETURNS:        value or -1 (error)
 *****************************************************************************/
unsigned long ASCIItobin(unsigned char k)
{
  char d=(char) -1;
  if ((k > 47) & (k < 58)) d = k - 48;  /* 0..9 */
  if ((k > 64) & (k < 71)) d = k - 55;  /* A..F */
  if ((k > 96) & (k < 103)) d = k - 87; /* a..f */
  return d;
}

/*****************************************************************************
 *  DESCRIPTION:    Receives a string until CR from ext UART
 *
 *  PARAMETERS:     (stored in global variable "line_buffer")
 *
 *  RETURNS:        None
 *****************************************************************************/
void receive_line(void)
{
  unsigned char ch;
  unsigned short i=0;

	do {
		ch = getch();             
		if((ch == '\r') | (ch=='\n')) break;
		line_buffer[i++] = ch;
	} while(1);
   
   line_buffer[i]='\0';
   line_buffer_ptr = line_buffer + i;
}

/*****************************************************************************
 *  DESCRIPTION:    Receives a string until CR from ext UART + Echo
 *
 *  PARAMETERS:     (stored in global variable "line_buffer")
 *
 *  RETURNS:        None
 *****************************************************************************/
int receive_line_echo(int *cnt)
{
  
  unsigned char ch;
  unsigned short i=0;
  memset(line_buffer,0,sizeof(line_buffer));

	do {
		ch = getch();             
        putch(ch);
        if((ch == 13) | (ch==27)) break;
        line_buffer[i++] = ch;
   } while(1);

   line_buffer[i]='\0';
   line_buffer_ptr = line_buffer + i;
   if (cnt) *cnt = i;
   //BufferPtr = (char)  &line_buffer;
   return (int) &line_buffer;
}

/***********************************************************************
 * DESCRIPTION: Scans the string 'line_buffer' for the string 'string'.
 *              Both strings must be null-terminated. 'line_buffer_ptr'
 *              is set appropriately.
 * RETURN VALUE: 1, if string 'string' is contained in string 'line_buffer'
 *                  at its beginning. 'line_buffer_ptr' points to the
 *                  next character of 'line_buffer' after the equal field.
 *               2, if string 'string' was not found in string 'line_buffer'.
 *               0, if strings are identical to the final null character.
 ************************************************************************/
int scan_line(char *str) 
{
	line_buffer_ptr = line_buffer;

    while((int)*line_buffer_ptr==(int)*str)
	{
		if((int)*str=='\0') return(0);
		line_buffer_ptr++;
		str++;
	}
	
	if((int)*str=='\0') return(1);
	return(2);
}

/*****************************************************************************
 *  DESCRIPTION:    Inputs a x-digit Hex-number (ASCII characters expected)
 *                      ESC aborts input
 *
 *  PARAMETERS:     Number of Hex-digits (e.g. FF = 2 Hex digits)
 *
 *  RETURNS:        Input value or -1 (abort)
 *****************************************************************************/
unsigned long Inputhex(unsigned char digits)
{
   unsigned long number=0,digit=0;
   unsigned char abort=0,mlt=0,i,key;

   mlt=(4*(digits-1));  /* 20 for 6 hex-digit numers, 4 for 2 hex-digits */
   for (i=0;i<digits;i++)
   {
     digit = (char)-1;     
     while ((digit==(char)-1) & (!abort))     /* input next valid digit */
     {
       key = getch();             /* wait for next key input (ASCII) */
       putch(key);
       if (key == 27) abort=1;    /* ESC aborts */
       digit = ASCIItobin(key);   /* convert to number */ 
       if (digit == (char)-1) putch(8); /* backspace if not valid */
     }
     number+= (digit << mlt);     /* add digit value to number */
     mlt-=4;                      /* next digit shift */
   }

   if (!abort) 
      return number;             /* return input value */
   else
   {
      puts("\n\n input cancled \n");
      return (char)-1;                /* return abort indicator */
   }
}

char getkey(char LKey, char HKey)
{
   char key;

   do                           /* input next valid digit */
   {
     key = upcase(getch());     /* wait for next key input (0-9,A-Z,a-z) */
     if (key == 27)
     {
       puts("\r>>> cancel input \n");
       return (char)-1;            /* return with ESC aborts */
     }

     if ( (key < LKey) || (key > HKey) )
     {
       /* undefinded key pressed */
       puts("\r>>> key not defined \r");
     }
     else
     {
       puts("\r>                   \r");
       putch(key);
       return key;              /* return input value */
     }

   } while(1);
}

char upcase(char k)
{
  char d=(char) -1;
  if ((k > 47) & (k < 58))  d = k;      /* 0..9 */
  if ((k > 64) & (k < 71))  d = k;      /* A..F */
  if ((k > 96) & (k < 123)) d = k - 32; /* a..f */
  if (k == 27) d = k;                   /* ESC  */ 
  return d;
}

#if (USE_PRINTF == 1)
/*********************@FUNCTION_HEADER_START*************************
*@FUNCTION NAME:	write()                                         *
*                                                                   *
*@DESCRIPTION:		Low-Level function for printf()                 *
*                                                                   *
*@PARAMETER:		file no., pointer to buffer, size of data       *
*                                                                   *
*@RETURN:			size or error(-1)                               *
*                                                                   *
***********************@FUNCTION_HEADER_END*************************/
int __write(int fileno, char *buf, unsigned int size)
{
    unsigned int cnt;
    switch(fileno)
    {
        case 0 :  
            //return(0);                            /* stdin */
        case 1 :  
            for(cnt = 0;size > cnt;cnt++)         /* stdout */
            {
                tbuf = *buf++;
                if(tbuf == '\n')                      //add CR to newline for terminal output
                {
                    while((UART_BASE[u8DefaultUart]->SSR & 0x02) == 0)
                    {
                    }
                    UART_BASE[u8DefaultUart]->RDR = '\r';                        //send carriage return
                }
                while((UART_BASE[u8DefaultUart]->SSR & 0x02) == 0)
                {
                }
                UART_BASE[u8DefaultUart]->RDR = tbuf;

            }
            return(cnt);                          /* successful output */
        case 2 :  
            return(-1);                           /* stderr */
        default:  
            return(-1);                           /* should never happend */
    }
}

/*****************************************************************************
 *  DESCRIPTION:    low level read function, read characters via UART1
 *                  carrige return (ASCII:13) is translated into line feed
 *                  and carrige returmn (ASCII: 10 and 13)
 *
 *  PARAMETERS:     file number, buffer pointer, maximal buffer size
 *
 *  RETURNS:        successfull: number of read characters
 *                  error: -1
 *****************************************************************************/

int __read( int fileno, char *buf, unsigned int size) {

	unsigned int cnt;
	unsigned char helpchar;
    switch(fileno)
    {
        case 0 :  
            for(cnt = 0;size > cnt;cnt++)         /* stdin */
            {
                while((UART_BASE[u8DefaultUart]->SSR & 0x04) == 0)
                {
                }
                if ((UART_BASE[u8DefaultUart]->SSR & 0x28) > 0)
                {
                    return (char_t)-1;
                }
                helpchar = UART_BASE[u8DefaultUart]->RDR;
                if (helpchar == 13) 
                {
                    *buf=10;
		       		return(cnt+1);			/* successful input */
                }
                *buf=helpchar;
                buf++;
            }
            return(cnt);                          /* successful output */
        case 1 :  
            return(0);                            /* stdout */
        case 2 :  
            return(-1);                           /* stderr */
        default:  
            return(-1);                           /* should never happend */
    }
}
/*********************@FUNCTION_HEADER_START*************************
*@FUNCTION NAME:	close()                                         *
*                                                                   *
*@DESCRIPTION:		Low-Level function to close specific file       *
*                                                                   *
*@PARAMETER:		file number                                     *
*                                                                   *
*@RETURN:			successful or error (-1)                        *
*                                                                   *
***********************@FUNCTION_HEADER_END*************************/
int __close(int fileno)
{
    if((fileno >= 0) && (fileno <= 2))
    {
        return(0);
    }
    else
    {
        return(-1);
    }
}

extern  char   *sbrk(int size)
{
   if (brk_siz + size > _heap_size || brk_siz + size < 0)
        return((char*)-1);
   brk_siz += size;
   return( (char *)_heap + brk_siz - size);
}


#endif /* (USE_PRINTF == 1) */
