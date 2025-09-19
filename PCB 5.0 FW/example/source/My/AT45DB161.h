/*******************************************************************************
FILE NAME			: 	AT45DB161.h
AUTHOR				:	NABEEN LAL AMATYA
MAIL TO AUTHOR		:	nabeenamatya@hotmail.com
CREATED DATE		:	2/3/2014
MOTHER VERSION NO	:	1.0
LAST MODIFIED DATE	:	2/3/2014
CURRENT VERSION NO	:	1.0
*******************************************************************************/
#ifndef __AT45DB161_H__
#define __AT45DB161_H__
/* other header file necessary to use this file */ 
#include "common.h"
#include"myfun.h"
/************************************************/

//FLASH MEMORY HARDWARE CONFIG
#define MEM_WP_PIN    (1UL<<9)  //P4.9
#define MEM_CS_PIN    (1UL<<10) //P4.B
#define MEM_RESET_PIN (1UL<<11) //P4.A
#define MEM_SCLK_PIN  (1UL<<12) //P4.C
#define MEM_SI_PIN    (1UL<<13) //P4.D
#define MEM_SO_PIN    (1UL<<14) //P4.E
#define MEM_MASK      (MEM_RESET_PIN | MEM_WP_PIN | MEM_CS_PIN | MEM_SI_PIN | MEM_SCLK_PIN)

#define MEM_PFR   FM3_GPIO->PFR4
#define MEM_PCR   FM3_GPIO->PCR4
#define MEM_PDIR  FM3_GPIO->PDIR4
#define MEM_DDR   FM3_GPIO->DDR4
#define MEM_PDOR  FM3_GPIO->PDOR4

#define FM_CS_LOW  MEM_PDOR &= ~MEM_CS_PIN 
#define FM_CS_HIGH MEM_PDOR |= MEM_CS_PIN
//#define FM_CS_TOG  MEM_PDOR ^= MEM_CS_PIN

#define FM_RESET_LOW  	MEM_PDOR &= ~MEM_RESET_PIN 
#define FM_RESET_HIGH 	MEM_PDOR |= MEM_RESET_PIN
//#define FM_RESET_TOG 	MEM_PDOR ^= MEM_RESET_PIN

#define FM_WP_LOW  	MEM_PDOR &= ~MEM_WP_PIN 
#define FM_WP_HIGH 	MEM_PDOR |= MEM_WP_PIN
//#define FM_WP_TOG 	MEM_PDOR ^= MEM_WP_PIN

#define FM_SCLK_LOW   	MEM_PDOR &= ~MEM_SCLK_PIN
#define FM_SCLK_HIGH  	MEM_PDOR |= MEM_SCLK_PIN
//#define FM_SCLK_TOG		MEM_PDOR ^= MEM_SCLK_PIN

#define FM_SI_LOW   MEM_PDOR &= ~MEM_SI_PIN
#define FM_SI_HIGH  MEM_PDOR |= MEM_SI_PIN
//#define FM_SI_TOG 	MEM_PDOR ^= MEM_SI_PIN

//TESTING PURPOSE ONLY 
//#define FM_SO_LOW 	MEM_PDOR &= ~MEM_SO_PIN
//#define FM_SO_HIGH 	MEM_PDOR |= MEM_SO_PIN
//#define FM_SO_TOG 	MEM_PDOR ^= MEM_SO_PIN

//TIME CONSTANTS 80MHz Tstate = 0.125nSec
#define TCS 500	//Minimum CS High Time = min 50nS
#define TCSS 50	//CS Setup Time = min 5nS
#define TCSH 50	//CS Hold Time = min 5nS i.e sclk high to low to CS high time
#define TWH 70	//CLOCK HIGH TIME = min 6.8 nS
#define TSU 20	//Data In Setup Time = min 2nS
#define TP 500	//Page Programming Time = max 6ms

//CONSTANTS
#define MEM_READ 0
#define MEM_WRITE 1
//MEMORY ORGANIZATION
#define BYTE_PER_PAGE   528
#define PAGE_PER_BLOCK  8
#define NO_OF_BLOCK     512
#define TOTAL_NO_OF_PAGE  4096

//COMMAND
#define RD_FM_ID 0x9F
#define RD_FM_SR 0xD7
#define CONTI_READ 0x0B
#define RD_BUFF1 0xD4
#define RD_BUFF2 0xD6
#define WR_BUFF1 0x84
#define WR_BUFF2 0x87
#define BUFF1_TO_PAGE	0x83
#define BUFF2_TO_PAGE	0x86
#define PAGE_TO_BUFF1	0x53
#define PAGE_TO_BUFF2	0x55
#define READ_UID 		0x77

// VARIABLES LOCAL TO THE FILE STARTS //////////////////////////////////////////
extern int page_in_buff1;	//page no that is loaded in buffer 1
extern int page_in_buff2;	//page no that is loaded in buffer 2

// FUNCTIONS LOCAL TO THE FILE STARTS //////////////////////////////////////////
extern void BufferToPage(uc buff_1_2, ui address);
extern void ChipErase();
extern void DirectMemRead(uli address, char * buff,ui no_of_bytes);
extern void FMemInit();
extern void PageErase(uint32_t page);
extern void PageToBuffer(ui page, uc buff_1_2);
extern void RandomMemRead(ui address,char *buff,int no_of_byte);
extern void RandomMemWrite(ui address,char *buff,int no_of_byte);
extern long long ReadSerialCode();
extern void ReadSerialInfoInCopyBuff();
extern unsigned char ReadIdentification();		//done
extern long long ReadUniqueIdentification();
extern void ReadMemReg(uc opcode, uc* buff,ui no_of_bytes);  //ok tested
extern void ReadWriteMemBuffer(boolean_t rw, ui address, char* buff, ui no_of_bytes);
extern uc spi_send(uc Byte);
extern void WriteValidSerialInfoFromCopyBuffToSecurityRegister();

// FUNCTIONS LOCAL TO THE FILE ENDS ////////////////////////////////////////////

// FUNCTIONS EXTERNALL TO THE FILE STARTS //////////////////////////////////////
// FUNCTIONS EXTERNAL TO THE FILE ENDS /////////////////////////////////////////
#endif