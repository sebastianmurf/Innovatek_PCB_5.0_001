#ifndef __I2C_H__
#define __I2C_H__

#include "common.h"
//port 1 & 2
#define P0F	(1<<15)	//I2C_SCL
#define P62 (1<<2)	//I2C_SDA

#define I2C_SCL_INIT_AS_OUTPUT_HIGH	FM3_GPIO->PFR0 &=~P0F; FM3_GPIO->PDOR0 |= P0F; FM3_GPIO->DDR0 |= P0F;
#define I2C_SCL_INIT_AS_OUTPUT_LOW	FM3_GPIO->PFR0 &=~P0F; FM3_GPIO->PDOR0 &= ~P0F; FM3_GPIO->DDR0 |= P0F;
#define I2C_SCL_LOW FM3_GPIO->PDOR0 &= ~P0F
#define I2C_SCL_HIGH FM3_GPIO->PDOR0 |= P0F
#define I2C_SCL_TOG FM3_GPIO->PDOR0 ^= P0F

#define I2C_SCL_INIT_AS_INPUT	FM3_GPIO->PFR0 &=~P0F; FM3_GPIO->PCR0 |= P0F; FM3_GPIO->DDR0 &= ~P0F;
#define CHECK_I2C_SCL (FM3_GPIO->PDIR0 & P0F)

#define I2C_SDA_INIT_AS_OUTPUT_HIGH	FM3_GPIO->PFR6 &=~P62; FM3_GPIO->PDOR6 |= P62; FM3_GPIO->DDR6 |= P62;
#define I2C_SDA_INIT_AS_OUTPUT_LOW	FM3_GPIO->PFR6 &=~P62; FM3_GPIO->PDOR6 &= ~P62; FM3_GPIO->DDR6 |= P62;
#define I2C_SDA_LOW FM3_GPIO->PDOR6 &= ~P62
#define I2C_SDA_HIGH FM3_GPIO->PDOR6 |= P62
#define I2C_SDA_TOG FM3_GPIO->PDOR6 ^= P62

#define I2C_SDA_INIT_AS_INPUT  FM3_GPIO->PFR6 &= ~P62; FM3_GPIO->PCR6 |= P62; FM3_GPIO->DDR6 &= ~P62;
#define CHECK_I2C_SDA (FM3_GPIO->PDIR6 & P62)

/*###########################################################################################################*/
/********* VARIABLE LOCAL TO THIS FILE BUT IS TO BE USED BY OTHER FILES DECLERATION ***********/
extern boolean_t I2C_ErrorFlag;
extern uc I2C_ComErrorCounter;
/*************************************************************/
/*###########################################################################################################*/
/********* VARIABLE EXTERNAL TO THIS FILE DECLERATION ***********/

/*************************************************************/
/*###########################################################################################################*/
/**********************FUNCTION LOCAL TO THE FILE BUT IS ALLOWED TO BE USE BY OTHER FILES*************/
/**** FUNCTIONS RELATED TO I2C PROTOCOL ****/
extern void I2C_Clock();
extern void I2C_Start(void);
extern void I2C_Stop(void);
extern void I2C_Nack(void);
extern void I2C_Ack(void);
extern void I2C_Send(char);
extern int I2C_Get(void);
/*###########################################################################################################*/
/*EXTERNAL FUNCTIONS REQUIRED FOR TESTING PURPOSE*/

/*************************************************************/
/*###########################################################################################################*/

#endif