#ifndef __PAE_I2C_H__
#define __PAE_I2C_H__

#include "common.h"
//port 0
#define P0B	(1<<11)	//PAE_I2C_SCL
#define P0A (1<<10)	//PAE_I2C_SDA

#define PAE_I2C_SCL_INIT_AS_OUTPUT_HIGH	FM3_GPIO->PFR0 &=~P0B; FM3_GPIO->PDOR0 |= P0B; FM3_GPIO->DDR0 |= P0B;
#define PAE_I2C_SCL_INIT_AS_OUTPUT_LOW	FM3_GPIO->PFR0 &=~P0B; FM3_GPIO->PDOR0 &= ~P0B; FM3_GPIO->DDR0 |= P0B;
#define PAE_I2C_SCL_LOW FM3_GPIO->PDOR0 &= ~P0B
#define PAE_I2C_SCL_HIGH FM3_GPIO->PDOR0 |= P0B
#define PAE_I2C_SCL_TOG FM3_GPIO->PDOR0 ^= P0B

#define PAE_I2C_SCL_INIT_AS_INPUT	FM3_GPIO->PFR0 &=~P0B; FM3_GPIO->PCR0 |= P0B; FM3_GPIO->DDR0 &= ~P0B;
#define CHECK_PAE_I2C_SCL (FM3_GPIO->PDIR0 & P0B)

#define PAE_I2C_SDA_INIT_AS_OUTPUT_HIGH	FM3_GPIO->PFR0 &=~P0A; FM3_GPIO->PDOR0 |= P0A; FM3_GPIO->DDR0 |= P0A;
#define PAE_I2C_SDA_INIT_AS_OUTPUT_LOW	FM3_GPIO->PFR0 &=~P0A; FM3_GPIO->PDOR0 &= ~P0A; FM3_GPIO->DDR0 |= P0A;
#define PAE_I2C_SDA_LOW FM3_GPIO->PDOR0 &= ~P0A
#define PAE_I2C_SDA_HIGH FM3_GPIO->PDOR0 |= P0A
#define PAE_I2C_SDA_TOG FM3_GPIO->PDOR0 ^= P0A

#define PAE_I2C_SDA_INIT_AS_INPUT  FM3_GPIO->PFR0 &= ~P0A; FM3_GPIO->PCR0 |= P0A; FM3_GPIO->DDR0 &= ~P0A;
#define CHECK_PAE_I2C_SDA (FM3_GPIO->PDIR0 & P0A)

/*###########################################################################################################*/
/********* VARIABLE LOCAL TO THIS FILE BUT IS TO BE USED BY OTHER FILES DECLERATION ***********/
extern boolean_t PAE_I2C_ErrorFlag;
/*************************************************************/
/*###########################################################################################################*/
/********* VARIABLE EXTERNAL TO THIS FILE DECLERATION ***********/

/*************************************************************/
/*###########################################################################################################*/
/**********************FUNCTION LOCAL TO THE FILE BUT IS ALLOWED TO BE USE BY OTHER FILES*************/
/**** FUNCTIONS RELATED TO I2C PROTOCOL ****/
extern void PAE_I2C_Clock();
extern void PAE_I2C_Start(void);
extern void PAE_I2C_Stop(void);
extern void PAE_I2C_Nack(void);
extern void PAE_I2C_Ack(void);
extern void PAE_I2C_Send(char);
extern int PAE_I2C_Get(void);
/*###########################################################################################################*/
/*EXTERNAL FUNCTIONS REQUIRED FOR TESTING PURPOSE*/

/*************************************************************/
/*###########################################################################################################*/

#endif