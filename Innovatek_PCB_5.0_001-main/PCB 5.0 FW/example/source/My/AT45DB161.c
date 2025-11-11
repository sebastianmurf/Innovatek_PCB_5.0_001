/*******************************************************************************
FILE NAME			: 	AT45DB161.C
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	2/3/2014
MOTHER VERSION NO	:	1.0
CURRENT VERSION NO	:	1.0

LAST MODIFIED DATE	:	04/11/2020
1. Chip Erase (max of 60 sec) time added as per datasheet

*******************************************************************************/
#include"AT45DB161.h"
#include"delay.h"
#include"variable.h"

#include"constants.c"
#include"DualTimer.h"
#include"MyUsb.h"

int page_in_buff1 = -1;	//page no that is loaded in buffer 1
int page_in_buff2 = -1;	//page no that is loaded in buffer 2

/******************************************************************************/

void PageErase(uint32_t page)
{   FM_CS_LOW;
  	Delay(TCSS);//CS Setup Time = minimum 5 nS
  	spi_send(0x81);
  	spi_send((page >> 16 ) & 0x0F);
  	spi_send((page >> 8) & 0x0F);
  	spi_send(page & 0x0F);
	FM_CS_HIGH;
    DelayInSec(60);//MAX CHIP ERASE TIME
}
////////////////////////////////////////////////////////////////////////////////
void ChipErase()
{	FM_CS_LOW;
  	Delay(TCSS);//CS Setup Time = minimum 5 nS
  	spi_send(0xC7);
  	spi_send(0x94);
  	spi_send(0x80);
  	spi_send(0x9A);
	FM_CS_HIGH;
    DelayInSec(60);//MAX CHIP ERASE TIME
}
////////////////////////////////////////////////////////////////////////////////
//	DESCRIPTION: Initializes Flash memory GPIO 
//	STATUS : OK TESTED
void FMemInit()
{ //FOR TESTING PURPOSE ONLY
  /*MEM_PFR &= ~MEM_MASK;    //Select CPIO function
  MEM_PDOR |= MEM_MASK;     //Set Pin to turn off leds
  MEM_DDR |= MEM_MASK;      //Make led pins outputs
  MEM_PFR &= ~MEM_SO_PIN;    //Select CPIO function
  MEM_PDOR |= MEM_SO_PIN;     //Set Pin to turn off leds
  MEM_DDR |= MEM_SO_PIN;      //Make led pins outputs
  */
  
  // Initialize ports for CSIO3
  //make flash memory serial data out pin as input
  //Select CPIO function
  MEM_PFR &= ~MEM_SO_PIN;
  //Set CPIO Pull-Up function
  MEM_PCR |= MEM_SO_PIN;
  //Make button pins inputs
  MEM_DDR &= ~MEM_SO_PIN;
  
  //make memory CLOCK pin as output
  MEM_PFR &= ~MEM_SCLK_PIN;    //Select CPIO function
  MEM_PDOR |= MEM_SCLK_PIN;     //Set Pin to turn off leds
  MEM_DDR |= MEM_SCLK_PIN;      //Make led pins outputs
  //make memory Serial Data IN pin as output
  MEM_PFR &= ~MEM_SI_PIN;    //Select CPIO function
  MEM_PDOR |= MEM_SI_PIN;     //Set Pin to turn off leds
  MEM_DDR |= MEM_SI_PIN;      //Make led pins outputs
  //make memory chip select pin as output
  MEM_PFR &= ~MEM_CS_PIN;    //Select CPIO function
  MEM_PDOR |= MEM_CS_PIN;     //Set Pin to turn off leds
  MEM_DDR |= MEM_CS_PIN;      //Make led pins outputs
  //make memory reset pin as output
  MEM_PFR &= ~MEM_RESET_PIN;    //Select CPIO function
  MEM_PDOR |= MEM_RESET_PIN;     //Set Pin to turn off leds
  MEM_DDR |= MEM_RESET_PIN;      //Make led pins outputs
  //make memory write protect pin as output
  MEM_PFR &= ~MEM_WP_PIN;    //Select CPIO function
  MEM_PDOR |= MEM_WP_PIN;     //Set Pin to turn off leds
  MEM_DDR |= MEM_WP_PIN;      //Make led pins outputs 
  
  FM_RESET_HIGH;  
  FM_WP_HIGH;
}
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
//	DESCRIPTION: Sends Byte through SI pin and reads return_val (1 Byte) through SO pin 
//	parameter 1: Byte to send
//	return : return_val ( 1 byte data)
//	STATUS : OK TESTED
////////////////////////////////////////////////////////////////////////////////
uc spi_send(uc Byte)            //ok tested
{ uc i, return_val = 0x00;
  for(i=0;i<8;i++)
  { FM_SCLK_LOW;
    while((MEM_PDOR & MEM_SCLK_PIN) > 0);
    //write
    if(Byte & 0x80)
      FM_SI_HIGH;
    else
      FM_SI_LOW;
    Byte <<= 1;
    Delay(TSU);//data setup time min 2ns
    //read
    return_val <<= 1;
    if((MEM_PDIR & MEM_SO_PIN) > 0)
    { return_val |= 0X01;
    }
    FM_SCLK_HIGH;
    while((MEM_PDOR & MEM_SCLK_PIN) == 0);
    Delay(TWH);	//CLOCK HIGH TIME = min 6.8 nS
  }
  return return_val;
}
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
//	DESCRIPTION: Random Memory Read, it will also use memory internal buffer
//	Parameter1:	memory address from where to start reading
//	Parameter2: variable where read data to place
//	Parameter3: number of byte to read
//	STATUS : OK 
//	CAUTION: Please take care of memory interanl buffer to prevent data loss
////////////////////////////////////////////////////////////////////////////////
void RandomMemRead(ui address,char *buff,int no_of_byte)	//OK TESTED
{	PageToBuffer((address / BYTE_PER_PAGE),PAGE_TO_BUFF1);
	if((address/BYTE_PER_PAGE) != ((address + no_of_byte)/BYTE_PER_PAGE))
		PageToBuffer(((address + no_of_byte) / BYTE_PER_PAGE),PAGE_TO_BUFF2);
	ReadWriteMemBuffer(MEM_READ,address,buff,no_of_byte);
}
/******************************************************************************/
//	DESCRIPTION: Random Memory Write (read change write), it will also use memory internal buffer
//	Parameter1:	memory address from where to start writing
//	Parameter2: variable where memory data to be changed/write 
//	Parameter3: number of byte to change/write
//	STATUS : OK TESTED. 
//	CAUTION: Please take care of memory interanl buffer to prevent data loss
////////////////////////////////////////////////////////////////////////////////
void RandomMemWrite(ui address,char *buff,int no_of_byte)	//OK TESTED
{	PageToBuffer((address/BYTE_PER_PAGE),PAGE_TO_BUFF1);
	if((address/BYTE_PER_PAGE) != ((address + no_of_byte)/BYTE_PER_PAGE))
		PageToBuffer(((address + no_of_byte) / BYTE_PER_PAGE),PAGE_TO_BUFF2);
	ReadWriteMemBuffer(MEM_WRITE,address,buff,no_of_byte);
	BufferToPage(BUFF1_TO_PAGE,(address/BYTE_PER_PAGE));
	if((address/BYTE_PER_PAGE) != ((address + no_of_byte)/BYTE_PER_PAGE))
		BufferToPage(BUFF2_TO_PAGE,((address + no_of_byte) / BYTE_PER_PAGE));
}
/******************************************************************************/
//	STATUS : OK TESTED
////////////////////////////////////////////////////////////////////////////////
void ReadMemReg(uc opcode, uc* buff,ui no_of_bytes)  //ok tested
{ FM_CS_LOW;
  Delay(TCSS);//CS Setup Time = minimum 5 nS
  spi_send(opcode);
  while(no_of_bytes > 0)
  { *buff = spi_send(0xff);
    no_of_bytes--;
    buff++;
  }
  FM_CS_HIGH;
  Delay(TCS);//min CS high time 50
}
/******************************************************************************/
//	DESCRIPTION: Direct Memory Read, it will not use memory internal buffer
//	Parameter1:	memory long address from where to start reading
//	Parameter2: variable where memory data read to be copied 
//	Parameter3: number of byte to read
//	STATUS :	OK
////////////////////////////////////////////////////////////////////////////////
void DirectMemRead(uli address, char * buff,ui no_of_bytes)
{ 	uc status_reg;
  	do
  	{ 	ReadMemReg(RD_FM_SR, &status_reg,1);
  	}	
	while((status_reg & 0x80) == 0);
	FM_CS_LOW;
  	Delay(TCSS);//CS Setup Time = minimum 5 nS
  	spi_send(CONTI_READ);	//command
  	//decoding address														|------- 1 byte --------||------ 2 byte ------||----- 3 byte --|
	spi_send((address/BYTE_PER_PAGE)>>6);									//23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
	spi_send(((address/BYTE_PER_PAGE)<<2) | ((address%BYTE_PER_PAGE)>>8));	//x  x  P  P  P  P  P  P  P  P  P  P  P  P  B B B B B B B B B B
	spi_send(address%BYTE_PER_PAGE);
	spi_send(0xFF);//dummy byte
  	while(no_of_bytes > 0)
  	{ 	*buff = spi_send(0xFF);
    	no_of_bytes--;
    	buff++;
  	}
  	FM_CS_HIGH;
  	Delay(TCS);//min CS high time 50
}
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
//	DESCRIPTION: READS / WRITES BUFFER 1 and / or buffer 2, it always starts read/write from buffer 1.
//				While read/write, if address exceeds BYTE_PER_PAGE byte it starts read/write from buffer 2 address 0 and upwards
//	parameter 1: 	buff_1_2	read or write buffer 1 or 2
//	parameter 2: 	address		Memory actual address that will be converted into buffer address leter 0-527
//	parameter 3:	buff		data to read to or write from
//	parameter 4:	no_of_bytes	number of bytes to read or write from / to buffer
//	STATUS : OK TESTED
//	CAUTION: Please take care of memory interanl buffer 2 to prevent data loss
////////////////////////////////////////////////////////////////////////////////
void ReadWriteMemBuffer(boolean_t rw, ui address, char* buff, ui no_of_bytes)
{ 	uc status_reg,buff_1_2;
	if(rw == MEM_READ)
		buff_1_2 = RD_BUFF1;
	else
	  	buff_1_2 = WR_BUFF1;
	
	address = address%BYTE_PER_PAGE;//ADDRESS THAT REFLECTS INTO BUFFER
	
	while(no_of_bytes > 0)
	{
		do
		{ 	ReadMemReg(RD_FM_SR, &status_reg,1);
		}
		while((status_reg & 0x80) == 0);
	  
		FM_CS_LOW;
		Delay(TCSS);//CS Setup Time = minimum 5 nS
	  //decoding address
		spi_send(buff_1_2);
		spi_send(0xff);
		spi_send(address>>8);
		spi_send(address&0xff);
		if(buff_1_2 == RD_BUFF1 || buff_1_2 == RD_BUFF2)
			spi_send(0xff); //dummy byte - time required to read by memory
	  
		while(no_of_bytes > 0)
		{ 	if(rw == MEM_READ)
				*buff = spi_send(0xFF);
			else
			  	spi_send(*buff);
			no_of_bytes--;
			buff++;
			address++;
			if((address % BYTE_PER_PAGE) == 0)
			{	FM_CS_HIGH;
				Delay(TCS);//min CS high time 50
				address = 0;
				//PutStringUsb("\r\nPage break encountered");
				if(rw == MEM_READ)
					buff_1_2 = RD_BUFF2;
				else
					buff_1_2 = WR_BUFF2;
				break;
  			}
		}
  	}
	FM_CS_HIGH;
  	Delay(TCS);//min CS high time 50
}
/******************************************************************************/
//	DESCRIPTION: writes buffer 1 or 2 into the memory page
//	Parameter1:	use buffer 1 or 2 to write
//	Parameter2: page number in memory where to write
//	STATUS : OK TESTED
////////////////////////////////////////////////////////////////////////////////
void BufferToPage(uc buff_1_2, ui page)
{ 	uc status_reg;
  	do
  	{	ReadMemReg(RD_FM_SR, &status_reg,1);
  	}
	while((status_reg & 0x80) == 0);
  
  	FM_CS_LOW;
  	Delay(TCSS);//CS Setup Time = minimum 5 nS
  	spi_send(buff_1_2);//command 	 
  	//decoding address						|------- 1 byte --------||------ 2 byte ------||----- 3 byte --|
	spi_send(page>>6);//6);					//23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
	spi_send(page<<2);//2));				//x  x  p  p  p  p  p  p  p  p  p  p  p  p  x x x x x x x x x x
	spi_send(0xFF);
	
	FM_CS_HIGH;
  	Delay(TCS);//min CS high time 50
}
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// 	Description : Reads Flash memory page to buffer 1 OR 2
//	Parameter1: page = page no to read from FM
//	Parameter2: use buffer 1 or 2 keep read page
//	STATUS : OK TESTED
////////////////////////////////////////////////////////////////////////////////
void PageToBuffer(ui page, uc buff_1_2)
{	uc status_reg;
  	do
  	{ 	ReadMemReg(RD_FM_SR, &status_reg,1);
  	}
	while((status_reg & 0x80) == 0);
  
  	FM_CS_LOW;
  	Delay(TCSS);//CS Setup Time = minimum 5 nS
  	spi_send(buff_1_2);//command 	 
  	//decoding address						|------- 1 byte --------||------ 2 byte -------||---- 3 byte --|
	spi_send(page>>6);						//23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
	spi_send((page<<2));					//x  x  p  p  p  p  p  p  p  p  p  p  p  p  x x x x x x x x x x
	spi_send(0xFF);
	
	FM_CS_HIGH;
  	Delay(TCS);//min CS high time 50
}
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// 	Description : reads flash memory identification number (0x1F260000)
//	STATUS : OK TESTED
////////////////////////////////////////////////////////////////////////////////
unsigned char ReadIdentification()		//done
{	uc ReturnVal;
  	FM_CS_LOW;
  	DelayInMilliSec(5);
  	spi_send(RD_FM_ID);
  	if(spi_send(0xff) == 0x1F)
  	{ 	if(spi_send(0xff) == 0x26)
    	{ 	if(spi_send(0xff) == 0x00)
      		{ 	ReturnVal = 0;		//success
				/*if(spi_send(0xff) == 0x00)
        		{ 	ReturnVal = 0;		//success
				}
				else
	  				ReturnVal = -1;	  //fail*/
      		}
      		else
        		ReturnVal = -1;	 //fail
    	}
    	else
      		ReturnVal = -1;		//fail
  	}
  	else
    	ReturnVal = -1; //fail
  	FM_CS_HIGH;
  	Delay(TCS);//min CS high time 50
  	return ReturnVal;   
}
/*******************************************************************************/
///////////////////////////////// Read Unique Identification ///////////////////
// Description: reads flash memory unique identification number; every chip has its own unique 64 byte id no.
// STATUS : PROGRAMMING
//////////////////////////////////////////////////////////////////////////////////
void ReadSerialInfoInCopyBuff()
{	uc loc;
	
	FM_CS_LOW;
	Delay(TCSS); // CS setup time min 5 ns
	spi_send(READ_UID);
	spi_send(0xFF);
	spi_send(0xFF);
	spi_send(0xFF);
	//read first 64 byte as Serial information
	for(loc =0;loc<13;loc++)
	{	CopyBuff[loc] = spi_send(0xFF);
	}
	FM_CS_HIGH;
	Delay(TCS);// min CS high time 50
}

void WriteValidSerialInfoFromCopyBuffToSecurityRegister()
{	uc loc;
	FM_CS_LOW;
	Delay(TCSS); // CS setup time min 5 ns
	spi_send(0x9B);
	spi_send(0x00);
	spi_send(0x00);
	spi_send(0x00);
	for(loc=0;loc<13;loc++)
	{	
		spi_send(CopyBuff[loc]);
	}
	FM_CS_HIGH;
	Delay(TP);
}
long long ReadUniqueIdentification()
{
	uc temp;
	long long temp1;
	union union_temp temp2;
	
	FM_CS_LOW;
	Delay(TCSS); // CS setup time min 5 ns
	spi_send(READ_UID);
	spi_send(0xFF);
	spi_send(0xFF);
	spi_send(0xFF);
	for(temp =0; temp<64;temp++)
	  	spi_send(0xFF);
	for(temp =0,temp1 =0; temp<64;temp++)
	{	
	  	temp2.c[7-(temp%8)] = spi_send(0xFF);
		if((temp+1)%8 == 0)
		{
		  	temp1 ^=temp2.ll;
			//sprintf(CopyBuff,"-%lld",u.ll);
		  	//PutStringUsb(CopyBuff);
		}
		
	}
	FM_CS_HIGH;
	Delay(TCS);// min CS high time 50
	return temp1;	
}
/******************************************************************************/
