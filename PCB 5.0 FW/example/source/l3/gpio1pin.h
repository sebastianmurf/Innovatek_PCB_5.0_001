/******************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)                */
/*                                                                           */
/* The following software deliverable is intended for and must only be       */
/* used for reference and in an evaluation laboratory environment.           */
/* It is provided on an as-is basis without charge and is subject to         */
/* alterations.                                                              */
/* It is the user’s obligation to fully test the software in its             */
/* environment and to ensure proper functionality, qualification and         */
/* compliance with component specifications.                                 */
/*                                                                           */
/* In the event the software deliverable includes the use of open            */
/* source components, the provisions of the governing open source            */
/* license agreement shall apply with respect to such software               */
/* deliverable.                                                              */
/* FSEU does not warrant that the deliverables do not infringe any           */
/* third party intellectual property right (IPR). In the event that          */
/* the deliverables infringe a third party IPR it is the sole                */
/* responsibility of the customer to obtain necessary licenses to            */
/* continue the usage of the deliverable.                                    */
/*                                                                           */
/* To the maximum extent permitted by applicable law FSEU disclaims all      */
/* warranties, whether express or implied, in particular, but not            */
/* limited to, warranties of merchantability and fitness for a               */
/* particular purpose for which the deliverable is not designated.           */
/*                                                                           */
/* To the maximum extent permitted by applicable law, FSEU’s liability       */
/* is restricted to intentional misconduct and gross negligence.             */
/* FSEU is not liable for consequential damages.                             */
/*                                                                           */
/* (V1.5)                                                                    */
/******************************************************************************/
/** \file gpio1pin.h
 **
 ** Headerfile for GPIO1PIN functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  AAr    First version.
 **
 ******************************************************************************/

#ifndef __GPIO1PIN_H__
#define __GPIO1PIN_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "l3.h"
#include "mcu.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup Gpio1PinGroup GPIO Defintions (GPIO)
 ******************************************************************************/
//@{  
  
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/


#define GPIO1PIN_CONCAT(x,y)           GPIO1PIN_CONCAT1(x,y)
#define GPIO1PIN_CONCAT1(x,y)          GPIO1PIN_CONCAT2(x,y)
#define GPIO1PIN_CONCAT2(x,y)          GPIO1PIN_CONCAT3(x,y)
#define GPIO1PIN_CONCAT3(x,y)          x##y


#define Gpio1pin_InitIn(p,settings)    do{ stc_gpio1pin_init_t __v__;\
                                         __v__.bPullup=0;__v__.bInitVal=0;\
                                         settings;\
                                         GPIO1PIN_CONCAT( p, _INITIN )( __v__ ); }while(0)


#define Gpio1pin_InitOut(p,settings)   do{ stc_gpio1pin_init_t __v__;\
                                         __v__.bPullup=0;__v__.bInitVal=0;\
                                         settings;\
                                         GPIO1PIN_CONCAT( p, _INITOUT )(__v__); }while(0)

#define Gpio1pin_Init(p,settings)      do{ stc_gpio1pin_init_t __v__;__v__.bOutput=0;\
                                         __v__.bPullup=0;__v__.bInitVal=0;\
                                         settings;\
                                         GPIO1PIN_CONCAT( p, _INIT )( __v__ ); }while(0)


#define Gpio1pin_InitDirectionInput    (__v__.bOutput=0)
#define Gpio1pin_InitDirectionOutput   (__v__.bOutput=1)
#define Gpio1pin_InitPullup(v)         (__v__.bPullup=(v))
#define Gpio1pin_InitVal(v)            (__v__.bInitVal=(v))
		


#define Gpio1pin_Get(p)	               GPIO1PIN_CONCAT( p, _GET )
#define Gpio1pin_Put(p,v)              (GPIO1PIN_CONCAT( p, _PUT ) (v))





/************************************************************
   GPIO
*************************************************************/



/*---- GPIO bit P00 ----*/

#define GPIO1PIN_P00_GET             ( bFM3_GPIO_PDIR0_P0 )

#define GPIO1PIN_P00_PUT(v)          ( bFM3_GPIO_PDOR0_P0=(v) )

#define GPIO1PIN_P00_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P00_INITIN(v) \
                                                        : GPIO1PIN_P00_INITOUT(v) )

#define GPIO1PIN_P00_INITIN(v)       ( bFM3_GPIO_PCR0_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P0=0, \
                                       bFM3_GPIO_PFR0_P0=0 )

#define GPIO1PIN_P00_INITOUT(v)      ( bFM3_GPIO_PDOR0_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P0=1, \
                                       bFM3_GPIO_PFR0_P0=0 )


/*---- GPIO bit NP00 ----*/

#define GPIO1PIN_NP00_GET            ( !(bFM3_GPIO_PDIR0_P0) )

#define GPIO1PIN_NP00_PUT(v)         ( bFM3_GPIO_PDOR0_P0=!(v) )

#define GPIO1PIN_NP00_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP00_INITIN(v) \
                                                        : GPIO1PIN_NP00_INITOUT(v) )

#define GPIO1PIN_NP00_INITIN(v)      ( bFM3_GPIO_PCR0_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P0=0, \
                                       bFM3_GPIO_PFR0_P0=0 )

#define GPIO1PIN_NP00_INITOUT(v)     ( bFM3_GPIO_PDOR0_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P0=1, \
                                       bFM3_GPIO_PFR0_P0=0 )

/*---- GPIO bit P01 ----*/

#define GPIO1PIN_P01_GET             ( bFM3_GPIO_PDIR0_P1 )

#define GPIO1PIN_P01_PUT(v)          ( bFM3_GPIO_PDOR0_P1=(v) )

#define GPIO1PIN_P01_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P01_INITIN(v) \
                                                        : GPIO1PIN_P01_INITOUT(v) )

#define GPIO1PIN_P01_INITIN(v)       ( bFM3_GPIO_PCR0_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P1=0, \
                                       bFM3_GPIO_PFR0_P1=0 )

#define GPIO1PIN_P01_INITOUT(v)      ( bFM3_GPIO_PDOR0_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P1=1, \
                                       bFM3_GPIO_PFR0_P1=0 )


/*---- GPIO bit NP01 ----*/

#define GPIO1PIN_NP01_GET            ( !(bFM3_GPIO_PDIR0_P1) )

#define GPIO1PIN_NP01_PUT(v)         ( bFM3_GPIO_PDOR0_P1=!(v) )

#define GPIO1PIN_NP01_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP01_INITIN(v) \
                                                        : GPIO1PIN_NP01_INITOUT(v) )

#define GPIO1PIN_NP01_INITIN(v)      ( bFM3_GPIO_PCR0_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P1=0, \
                                       bFM3_GPIO_PFR0_P1=0 )

#define GPIO1PIN_NP01_INITOUT(v)     ( bFM3_GPIO_PDOR0_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P1=1, \
                                       bFM3_GPIO_PFR0_P1=0 )

/*---- GPIO bit P02 ----*/

#define GPIO1PIN_P02_GET             ( bFM3_GPIO_PDIR0_P2 )

#define GPIO1PIN_P02_PUT(v)          ( bFM3_GPIO_PDOR0_P2=(v) )

#define GPIO1PIN_P02_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P02_INITIN(v) \
                                                        : GPIO1PIN_P02_INITOUT(v) )

#define GPIO1PIN_P02_INITIN(v)       ( bFM3_GPIO_PCR0_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P2=0, \
                                       bFM3_GPIO_PFR0_P2=0 )

#define GPIO1PIN_P02_INITOUT(v)      ( bFM3_GPIO_PDOR0_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P2=1, \
                                       bFM3_GPIO_PFR0_P2=0 )


/*---- GPIO bit NP02 ----*/

#define GPIO1PIN_NP02_GET            ( !(bFM3_GPIO_PDIR0_P2) )

#define GPIO1PIN_NP02_PUT(v)         ( bFM3_GPIO_PDOR0_P2=!(v) )

#define GPIO1PIN_NP02_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP02_INITIN(v) \
                                                        : GPIO1PIN_NP02_INITOUT(v) )

#define GPIO1PIN_NP02_INITIN(v)      ( bFM3_GPIO_PCR0_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P2=0, \
                                       bFM3_GPIO_PFR0_P2=0 )

#define GPIO1PIN_NP02_INITOUT(v)     ( bFM3_GPIO_PDOR0_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P2=1, \
                                       bFM3_GPIO_PFR0_P2=0 )

/*---- GPIO bit P03 ----*/

#define GPIO1PIN_P03_GET             ( bFM3_GPIO_PDIR0_P3 )

#define GPIO1PIN_P03_PUT(v)          ( bFM3_GPIO_PDOR0_P3=(v) )

#define GPIO1PIN_P03_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P03_INITIN(v) \
                                                        : GPIO1PIN_P03_INITOUT(v) )

#define GPIO1PIN_P03_INITIN(v)       ( bFM3_GPIO_PCR0_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P3=0, \
                                       bFM3_GPIO_PFR0_P3=0 )

#define GPIO1PIN_P03_INITOUT(v)      ( bFM3_GPIO_PDOR0_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P3=1, \
                                       bFM3_GPIO_PFR0_P3=0 )


/*---- GPIO bit NP03 ----*/

#define GPIO1PIN_NP03_GET            ( !(bFM3_GPIO_PDIR0_P3) )

#define GPIO1PIN_NP03_PUT(v)         ( bFM3_GPIO_PDOR0_P3=!(v) )

#define GPIO1PIN_NP03_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP03_INITIN(v) \
                                                        : GPIO1PIN_NP03_INITOUT(v) )

#define GPIO1PIN_NP03_INITIN(v)      ( bFM3_GPIO_PCR0_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P3=0, \
                                       bFM3_GPIO_PFR0_P3=0 )

#define GPIO1PIN_NP03_INITOUT(v)     ( bFM3_GPIO_PDOR0_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P3=1, \
                                       bFM3_GPIO_PFR0_P3=0 )

/*---- GPIO bit P04 ----*/

#define GPIO1PIN_P04_GET             ( bFM3_GPIO_PDIR0_P4 )

#define GPIO1PIN_P04_PUT(v)          ( bFM3_GPIO_PDOR0_P4=(v) )

#define GPIO1PIN_P04_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P04_INITIN(v) \
                                                        : GPIO1PIN_P04_INITOUT(v) )

#define GPIO1PIN_P04_INITIN(v)       ( bFM3_GPIO_PCR0_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P4=0, \
                                       bFM3_GPIO_PFR0_P4=0 )

#define GPIO1PIN_P04_INITOUT(v)      ( bFM3_GPIO_PDOR0_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P4=1, \
                                       bFM3_GPIO_PFR0_P4=0 )


/*---- GPIO bit NP04 ----*/

#define GPIO1PIN_NP04_GET            ( !(bFM3_GPIO_PDIR0_P4) )

#define GPIO1PIN_NP04_PUT(v)         ( bFM3_GPIO_PDOR0_P4=!(v) )

#define GPIO1PIN_NP04_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP04_INITIN(v) \
                                                        : GPIO1PIN_NP04_INITOUT(v) )

#define GPIO1PIN_NP04_INITIN(v)      ( bFM3_GPIO_PCR0_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P4=0, \
                                       bFM3_GPIO_PFR0_P4=0 )

#define GPIO1PIN_NP04_INITOUT(v)     ( bFM3_GPIO_PDOR0_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P4=1, \
                                       bFM3_GPIO_PFR0_P4=0 )

/*---- GPIO bit P05 ----*/

#define GPIO1PIN_P05_GET             ( bFM3_GPIO_PDIR0_P5 )

#define GPIO1PIN_P05_PUT(v)          ( bFM3_GPIO_PDOR0_P5=(v) )

#define GPIO1PIN_P05_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P05_INITIN(v) \
                                                        : GPIO1PIN_P05_INITOUT(v) )

#define GPIO1PIN_P05_INITIN(v)       ( bFM3_GPIO_PCR0_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P5=0, \
                                       bFM3_GPIO_PFR0_P5=0 )

#define GPIO1PIN_P05_INITOUT(v)      ( bFM3_GPIO_PDOR0_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P5=1, \
                                       bFM3_GPIO_PFR0_P5=0 )


/*---- GPIO bit NP05 ----*/

#define GPIO1PIN_NP05_GET            ( !(bFM3_GPIO_PDIR0_P5) )

#define GPIO1PIN_NP05_PUT(v)         ( bFM3_GPIO_PDOR0_P5=!(v) )

#define GPIO1PIN_NP05_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP05_INITIN(v) \
                                                        : GPIO1PIN_NP05_INITOUT(v) )

#define GPIO1PIN_NP05_INITIN(v)      ( bFM3_GPIO_PCR0_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P5=0, \
                                       bFM3_GPIO_PFR0_P5=0 )

#define GPIO1PIN_NP05_INITOUT(v)     ( bFM3_GPIO_PDOR0_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P5=1, \
                                       bFM3_GPIO_PFR0_P5=0 )

/*---- GPIO bit P06 ----*/

#define GPIO1PIN_P06_GET             ( bFM3_GPIO_PDIR0_P6 )

#define GPIO1PIN_P06_PUT(v)          ( bFM3_GPIO_PDOR0_P6=(v) )

#define GPIO1PIN_P06_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P06_INITIN(v) \
                                                        : GPIO1PIN_P06_INITOUT(v) )

#define GPIO1PIN_P06_INITIN(v)       ( bFM3_GPIO_PCR0_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P6=0, \
                                       bFM3_GPIO_PFR0_P6=0 )

#define GPIO1PIN_P06_INITOUT(v)      ( bFM3_GPIO_PDOR0_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P6=1, \
                                       bFM3_GPIO_PFR0_P6=0 )


/*---- GPIO bit NP06 ----*/

#define GPIO1PIN_NP06_GET            ( !(bFM3_GPIO_PDIR0_P6) )

#define GPIO1PIN_NP06_PUT(v)         ( bFM3_GPIO_PDOR0_P6=!(v) )

#define GPIO1PIN_NP06_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP06_INITIN(v) \
                                                        : GPIO1PIN_NP06_INITOUT(v) )

#define GPIO1PIN_NP06_INITIN(v)      ( bFM3_GPIO_PCR0_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P6=0, \
                                       bFM3_GPIO_PFR0_P6=0 )

#define GPIO1PIN_NP06_INITOUT(v)     ( bFM3_GPIO_PDOR0_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P6=1, \
                                       bFM3_GPIO_PFR0_P6=0 )

/*---- GPIO bit P07 ----*/

#define GPIO1PIN_P07_GET             ( bFM3_GPIO_PDIR0_P7 )

#define GPIO1PIN_P07_PUT(v)          ( bFM3_GPIO_PDOR0_P7=(v) )

#define GPIO1PIN_P07_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P07_INITIN(v) \
                                                        : GPIO1PIN_P07_INITOUT(v) )

#define GPIO1PIN_P07_INITIN(v)       ( bFM3_GPIO_PCR0_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P7=0, \
                                       bFM3_GPIO_PFR0_P7=0 )

#define GPIO1PIN_P07_INITOUT(v)      ( bFM3_GPIO_PDOR0_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P7=1, \
                                       bFM3_GPIO_PFR0_P7=0 )


/*---- GPIO bit NP07 ----*/

#define GPIO1PIN_NP07_GET            ( !(bFM3_GPIO_PDIR0_P7) )

#define GPIO1PIN_NP07_PUT(v)         ( bFM3_GPIO_PDOR0_P7=!(v) )

#define GPIO1PIN_NP07_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP07_INITIN(v) \
                                                        : GPIO1PIN_NP07_INITOUT(v) )

#define GPIO1PIN_NP07_INITIN(v)      ( bFM3_GPIO_PCR0_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P7=0, \
                                       bFM3_GPIO_PFR0_P7=0 )

#define GPIO1PIN_NP07_INITOUT(v)     ( bFM3_GPIO_PDOR0_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P7=1, \
                                       bFM3_GPIO_PFR0_P7=0 )

/*---- GPIO bit P08 ----*/

#define GPIO1PIN_P08_GET             ( bFM3_GPIO_PDIR0_P8 )

#define GPIO1PIN_P08_PUT(v)          ( bFM3_GPIO_PDOR0_P8=(v) )

#define GPIO1PIN_P08_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P08_INITIN(v) \
                                                        : GPIO1PIN_P08_INITOUT(v) )

#define GPIO1PIN_P08_INITIN(v)       ( bFM3_GPIO_PCR0_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P8=0, \
                                       bFM3_GPIO_PFR0_P8=0 )

#define GPIO1PIN_P08_INITOUT(v)      ( bFM3_GPIO_PDOR0_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P8=1, \
                                       bFM3_GPIO_PFR0_P8=0 )


/*---- GPIO bit NP08 ----*/

#define GPIO1PIN_NP08_GET            ( !(bFM3_GPIO_PDIR0_P8) )

#define GPIO1PIN_NP08_PUT(v)         ( bFM3_GPIO_PDOR0_P8=!(v) )

#define GPIO1PIN_NP08_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP08_INITIN(v) \
                                                        : GPIO1PIN_NP08_INITOUT(v) )

#define GPIO1PIN_NP08_INITIN(v)      ( bFM3_GPIO_PCR0_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P8=0, \
                                       bFM3_GPIO_PFR0_P8=0 )

#define GPIO1PIN_NP08_INITOUT(v)     ( bFM3_GPIO_PDOR0_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P8=1, \
                                       bFM3_GPIO_PFR0_P8=0 )

/*---- GPIO bit P09 ----*/

#define GPIO1PIN_P09_GET             ( bFM3_GPIO_PDIR0_P9 )

#define GPIO1PIN_P09_PUT(v)          ( bFM3_GPIO_PDOR0_P9=(v) )

#define GPIO1PIN_P09_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P09_INITIN(v) \
                                                        : GPIO1PIN_P09_INITOUT(v) )

#define GPIO1PIN_P09_INITIN(v)       ( bFM3_GPIO_PCR0_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P9=0, \
                                       bFM3_GPIO_PFR0_P9=0 )

#define GPIO1PIN_P09_INITOUT(v)      ( bFM3_GPIO_PDOR0_P9=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_P9=1, \
                                       bFM3_GPIO_PFR0_P9=0 )


/*---- GPIO bit NP09 ----*/

#define GPIO1PIN_NP09_GET            ( !(bFM3_GPIO_PDIR0_P9) )

#define GPIO1PIN_NP09_PUT(v)         ( bFM3_GPIO_PDOR0_P9=!(v) )

#define GPIO1PIN_NP09_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP09_INITIN(v) \
                                                        : GPIO1PIN_NP09_INITOUT(v) )

#define GPIO1PIN_NP09_INITIN(v)      ( bFM3_GPIO_PCR0_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR0_P9=0, \
                                       bFM3_GPIO_PFR0_P9=0 )

#define GPIO1PIN_NP09_INITOUT(v)     ( bFM3_GPIO_PDOR0_P9=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_P9=1, \
                                       bFM3_GPIO_PFR0_P9=0 )

/*---- GPIO bit P0A ----*/

#define GPIO1PIN_P0A_GET             ( bFM3_GPIO_PDIR0_PA )

#define GPIO1PIN_P0A_PUT(v)          ( bFM3_GPIO_PDOR0_PA=(v) )

#define GPIO1PIN_P0A_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P0A_INITIN(v) \
                                                        : GPIO1PIN_P0A_INITOUT(v) )

#define GPIO1PIN_P0A_INITIN(v)       ( bFM3_GPIO_PCR0_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PA=0, \
                                       bFM3_GPIO_PFR0_PA=0 )

#define GPIO1PIN_P0A_INITOUT(v)      ( bFM3_GPIO_PDOR0_PA=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_PA=1, \
                                       bFM3_GPIO_PFR0_PA=0 )


/*---- GPIO bit NP0A ----*/

#define GPIO1PIN_NP0A_GET            ( !(bFM3_GPIO_PDIR0_PA) )

#define GPIO1PIN_NP0A_PUT(v)         ( bFM3_GPIO_PDOR0_PA=!(v) )

#define GPIO1PIN_NP0A_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP0A_INITIN(v) \
                                                        : GPIO1PIN_NP0A_INITOUT(v) )

#define GPIO1PIN_NP0A_INITIN(v)      ( bFM3_GPIO_PCR0_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PA=0, \
                                       bFM3_GPIO_PFR0_PA=0 )

#define GPIO1PIN_NP0A_INITOUT(v)     ( bFM3_GPIO_PDOR0_PA=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_PA=1, \
                                       bFM3_GPIO_PFR0_PA=0 )

/*---- GPIO bit P0B ----*/

#define GPIO1PIN_P0B_GET             ( bFM3_GPIO_PDIR0_PB )

#define GPIO1PIN_P0B_PUT(v)          ( bFM3_GPIO_PDOR0_PB=(v) )

#define GPIO1PIN_P0B_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P0B_INITIN(v) \
                                                        : GPIO1PIN_P0B_INITOUT(v) )

#define GPIO1PIN_P0B_INITIN(v)       ( bFM3_GPIO_PCR0_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PB=0, \
                                       bFM3_GPIO_PFR0_PB=0 )

#define GPIO1PIN_P0B_INITOUT(v)      ( bFM3_GPIO_PDOR0_PB=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_PB=1, \
                                       bFM3_GPIO_PFR0_PB=0 )


/*---- GPIO bit NP0B ----*/

#define GPIO1PIN_NP0B_GET            ( !(bFM3_GPIO_PDIR0_PB) )

#define GPIO1PIN_NP0B_PUT(v)         ( bFM3_GPIO_PDOR0_PB=!(v) )

#define GPIO1PIN_NP0B_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP0B_INITIN(v) \
                                                        : GPIO1PIN_NP0B_INITOUT(v) )

#define GPIO1PIN_NP0B_INITIN(v)      ( bFM3_GPIO_PCR0_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PB=0, \
                                       bFM3_GPIO_PFR0_PB=0 )

#define GPIO1PIN_NP0B_INITOUT(v)     ( bFM3_GPIO_PDOR0_PB=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_PB=1, \
                                       bFM3_GPIO_PFR0_PB=0 )

/*---- GPIO bit P0C ----*/

#define GPIO1PIN_P0C_GET             ( bFM3_GPIO_PDIR0_PC )

#define GPIO1PIN_P0C_PUT(v)          ( bFM3_GPIO_PDOR0_PC=(v) )

#define GPIO1PIN_P0C_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P0C_INITIN(v) \
                                                        : GPIO1PIN_P0C_INITOUT(v) )

#define GPIO1PIN_P0C_INITIN(v)       ( bFM3_GPIO_PCR0_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PC=0, \
                                       bFM3_GPIO_PFR0_PC=0 )

#define GPIO1PIN_P0C_INITOUT(v)      ( bFM3_GPIO_PDOR0_PC=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_PC=1, \
                                       bFM3_GPIO_PFR0_PC=0 )


/*---- GPIO bit NP0C ----*/

#define GPIO1PIN_NP0C_GET            ( !(bFM3_GPIO_PDIR0_PC) )

#define GPIO1PIN_NP0C_PUT(v)         ( bFM3_GPIO_PDOR0_PC=!(v) )

#define GPIO1PIN_NP0C_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP0C_INITIN(v) \
                                                        : GPIO1PIN_NP0C_INITOUT(v) )

#define GPIO1PIN_NP0C_INITIN(v)      ( bFM3_GPIO_PCR0_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PC=0, \
                                       bFM3_GPIO_PFR0_PC=0 )

#define GPIO1PIN_NP0C_INITOUT(v)     ( bFM3_GPIO_PDOR0_PC=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_PC=1, \
                                       bFM3_GPIO_PFR0_PC=0 )

/*---- GPIO bit P0D ----*/

#define GPIO1PIN_P0D_GET             ( bFM3_GPIO_PDIR0_PD )

#define GPIO1PIN_P0D_PUT(v)          ( bFM3_GPIO_PDOR0_PD=(v) )

#define GPIO1PIN_P0D_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P0D_INITIN(v) \
                                                        : GPIO1PIN_P0D_INITOUT(v) )

#define GPIO1PIN_P0D_INITIN(v)       ( bFM3_GPIO_PCR0_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PD=0, \
                                       bFM3_GPIO_PFR0_PD=0 )

#define GPIO1PIN_P0D_INITOUT(v)      ( bFM3_GPIO_PDOR0_PD=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_PD=1, \
                                       bFM3_GPIO_PFR0_PD=0 )


/*---- GPIO bit NP0D ----*/

#define GPIO1PIN_NP0D_GET            ( !(bFM3_GPIO_PDIR0_PD) )

#define GPIO1PIN_NP0D_PUT(v)         ( bFM3_GPIO_PDOR0_PD=!(v) )

#define GPIO1PIN_NP0D_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP0D_INITIN(v) \
                                                        : GPIO1PIN_NP0D_INITOUT(v) )

#define GPIO1PIN_NP0D_INITIN(v)      ( bFM3_GPIO_PCR0_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PD=0, \
                                       bFM3_GPIO_PFR0_PD=0 )

#define GPIO1PIN_NP0D_INITOUT(v)     ( bFM3_GPIO_PDOR0_PD=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_PD=1, \
                                       bFM3_GPIO_PFR0_PD=0 )

/*---- GPIO bit P0E ----*/

#define GPIO1PIN_P0E_GET             ( bFM3_GPIO_PDIR0_PE )

#define GPIO1PIN_P0E_PUT(v)          ( bFM3_GPIO_PDOR0_PE=(v) )

#define GPIO1PIN_P0E_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P0E_INITIN(v) \
                                                        : GPIO1PIN_P0E_INITOUT(v) )

#define GPIO1PIN_P0E_INITIN(v)       ( bFM3_GPIO_PCR0_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PE=0, \
                                       bFM3_GPIO_PFR0_PE=0 )

#define GPIO1PIN_P0E_INITOUT(v)      ( bFM3_GPIO_PDOR0_PE=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_PE=1, \
                                       bFM3_GPIO_PFR0_PE=0 )


/*---- GPIO bit NP0E ----*/

#define GPIO1PIN_NP0E_GET            ( !(bFM3_GPIO_PDIR0_PE) )

#define GPIO1PIN_NP0E_PUT(v)         ( bFM3_GPIO_PDOR0_PE=!(v) )

#define GPIO1PIN_NP0E_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP0E_INITIN(v) \
                                                        : GPIO1PIN_NP0E_INITOUT(v) )

#define GPIO1PIN_NP0E_INITIN(v)      ( bFM3_GPIO_PCR0_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PE=0, \
                                       bFM3_GPIO_PFR0_PE=0 )

#define GPIO1PIN_NP0E_INITOUT(v)     ( bFM3_GPIO_PDOR0_PE=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_PE=1, \
                                       bFM3_GPIO_PFR0_PE=0 )

/*---- GPIO bit P0F ----*/

#define GPIO1PIN_P0F_GET             ( bFM3_GPIO_PDIR0_PF )

#define GPIO1PIN_P0F_PUT(v)          ( bFM3_GPIO_PDOR0_PF=(v) )

#define GPIO1PIN_P0F_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P0F_INITIN(v) \
                                                        : GPIO1PIN_P0F_INITOUT(v) )

#define GPIO1PIN_P0F_INITIN(v)       ( bFM3_GPIO_PCR0_PF=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PF=0, \
                                       bFM3_GPIO_PFR0_PF=0 )

#define GPIO1PIN_P0F_INITOUT(v)      ( bFM3_GPIO_PDOR0_PF=(v).bInitVal, \
                                       bFM3_GPIO_DDR0_PF=1, \
                                       bFM3_GPIO_PFR0_PF=0 )


/*---- GPIO bit NP0F ----*/

#define GPIO1PIN_NP0F_GET            ( !(bFM3_GPIO_PDIR0_PF) )

#define GPIO1PIN_NP0F_PUT(v)         ( bFM3_GPIO_PDOR0_PF=!(v) )

#define GPIO1PIN_NP0F_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP0F_INITIN(v) \
                                                        : GPIO1PIN_NP0F_INITOUT(v) )

#define GPIO1PIN_NP0F_INITIN(v)      ( bFM3_GPIO_PCR0_PF=(v).bPullup, \
                                       bFM3_GPIO_DDR0_PF=0, \
                                       bFM3_GPIO_PFR0_PF=0 )

#define GPIO1PIN_NP0F_INITOUT(v)     ( bFM3_GPIO_PDOR0_PF=!((v).bInitVal), \
                                       bFM3_GPIO_DDR0_PF=1, \
                                       bFM3_GPIO_PFR0_PF=0 )

/*---- GPIO bit P10 ----*/

#define GPIO1PIN_P10_GET             ( bFM3_GPIO_PDIR1_P0 )

#define GPIO1PIN_P10_PUT(v)          ( bFM3_GPIO_PDOR1_P0=(v) )

#define GPIO1PIN_P10_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P10_INITIN(v) \
                                                        : GPIO1PIN_P10_INITOUT(v) )

#define GPIO1PIN_P10_INITIN(v)       ( bFM3_GPIO_PCR1_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P0=0, \
                                       bFM3_GPIO_PFR1_P0=0, \
                                       bFM3_GPIO_ADE_AN0=0 )

#define GPIO1PIN_P10_INITOUT(v)      ( bFM3_GPIO_PDOR1_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P0=1, \
                                       bFM3_GPIO_PFR1_P0=0, \
                                       bFM3_GPIO_ADE_AN0=0 )


/*---- GPIO bit NP10 ----*/

#define GPIO1PIN_NP10_GET            ( !(bFM3_GPIO_PDIR1_P0) )

#define GPIO1PIN_NP10_PUT(v)         ( bFM3_GPIO_PDOR1_P0=!(v) )

#define GPIO1PIN_NP10_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP10_INITIN(v) \
                                                        : GPIO1PIN_NP10_INITOUT(v) )

#define GPIO1PIN_NP10_INITIN(v)      ( bFM3_GPIO_PCR1_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P0=0, \
                                       bFM3_GPIO_PFR1_P0=0, \
                                       bFM3_GPIO_ADE_AN0=0 )

#define GPIO1PIN_NP10_INITOUT(v)     ( bFM3_GPIO_PDOR1_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P0=1, \
                                       bFM3_GPIO_PFR1_P0=0, \
                                       bFM3_GPIO_ADE_AN0=0 )

/*---- GPIO bit P11 ----*/

#define GPIO1PIN_P11_GET             ( bFM3_GPIO_PDIR1_P1 )

#define GPIO1PIN_P11_PUT(v)          ( bFM3_GPIO_PDOR1_P1=(v) )

#define GPIO1PIN_P11_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P11_INITIN(v) \
                                                        : GPIO1PIN_P11_INITOUT(v) )

#define GPIO1PIN_P11_INITIN(v)       ( bFM3_GPIO_PCR1_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P1=0, \
                                       bFM3_GPIO_PFR1_P1=0, \
                                       bFM3_GPIO_ADE_AN1=0 )

#define GPIO1PIN_P11_INITOUT(v)      ( bFM3_GPIO_PDOR1_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P1=1, \
                                       bFM3_GPIO_PFR1_P1=0, \
                                       bFM3_GPIO_ADE_AN1=0 )


/*---- GPIO bit NP11 ----*/

#define GPIO1PIN_NP11_GET            ( !(bFM3_GPIO_PDIR1_P1) )

#define GPIO1PIN_NP11_PUT(v)         ( bFM3_GPIO_PDOR1_P1=!(v) )

#define GPIO1PIN_NP11_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP11_INITIN(v) \
                                                        : GPIO1PIN_NP11_INITOUT(v) )

#define GPIO1PIN_NP11_INITIN(v)      ( bFM3_GPIO_PCR1_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P1=0, \
                                       bFM3_GPIO_PFR1_P1=0, \
                                       bFM3_GPIO_ADE_AN1=0 )

#define GPIO1PIN_NP11_INITOUT(v)     ( bFM3_GPIO_PDOR1_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P1=1, \
                                       bFM3_GPIO_PFR1_P1=0, \
                                       bFM3_GPIO_ADE_AN1=0 )

/*---- GPIO bit P12 ----*/

#define GPIO1PIN_P12_GET             ( bFM3_GPIO_PDIR1_P2 )

#define GPIO1PIN_P12_PUT(v)          ( bFM3_GPIO_PDOR1_P2=(v) )

#define GPIO1PIN_P12_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P12_INITIN(v) \
                                                        : GPIO1PIN_P12_INITOUT(v) )

#define GPIO1PIN_P12_INITIN(v)       ( bFM3_GPIO_PCR1_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P2=0, \
                                       bFM3_GPIO_PFR1_P2=0, \
                                       bFM3_GPIO_ADE_AN2=0 )

#define GPIO1PIN_P12_INITOUT(v)      ( bFM3_GPIO_PDOR1_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P2=1, \
                                       bFM3_GPIO_PFR1_P2=0, \
                                       bFM3_GPIO_ADE_AN2=0 )


/*---- GPIO bit NP12 ----*/

#define GPIO1PIN_NP12_GET            ( !(bFM3_GPIO_PDIR1_P2) )

#define GPIO1PIN_NP12_PUT(v)         ( bFM3_GPIO_PDOR1_P2=!(v) )

#define GPIO1PIN_NP12_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP12_INITIN(v) \
                                                        : GPIO1PIN_NP12_INITOUT(v) )

#define GPIO1PIN_NP12_INITIN(v)      ( bFM3_GPIO_PCR1_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P2=0, \
                                       bFM3_GPIO_PFR1_P2=0, \
                                       bFM3_GPIO_ADE_AN2=0 )

#define GPIO1PIN_NP12_INITOUT(v)     ( bFM3_GPIO_PDOR1_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P2=1, \
                                       bFM3_GPIO_PFR1_P2=0, \
                                       bFM3_GPIO_ADE_AN2=0 )

/*---- GPIO bit P13 ----*/

#define GPIO1PIN_P13_GET             ( bFM3_GPIO_PDIR1_P3 )

#define GPIO1PIN_P13_PUT(v)          ( bFM3_GPIO_PDOR1_P3=(v) )

#define GPIO1PIN_P13_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P13_INITIN(v) \
                                                        : GPIO1PIN_P13_INITOUT(v) )

#define GPIO1PIN_P13_INITIN(v)       ( bFM3_GPIO_PCR1_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P3=0, \
                                       bFM3_GPIO_PFR1_P3=0, \
                                       bFM3_GPIO_ADE_AN3=0 )

#define GPIO1PIN_P13_INITOUT(v)      ( bFM3_GPIO_PDOR1_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P3=1, \
                                       bFM3_GPIO_PFR1_P3=0, \
                                       bFM3_GPIO_ADE_AN3=0 )


/*---- GPIO bit NP13 ----*/

#define GPIO1PIN_NP13_GET            ( !(bFM3_GPIO_PDIR1_P3) )

#define GPIO1PIN_NP13_PUT(v)         ( bFM3_GPIO_PDOR1_P3=!(v) )

#define GPIO1PIN_NP13_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP13_INITIN(v) \
                                                        : GPIO1PIN_NP13_INITOUT(v) )

#define GPIO1PIN_NP13_INITIN(v)      ( bFM3_GPIO_PCR1_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P3=0, \
                                       bFM3_GPIO_PFR1_P3=0, \
                                       bFM3_GPIO_ADE_AN3=0 )

#define GPIO1PIN_NP13_INITOUT(v)     ( bFM3_GPIO_PDOR1_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P3=1, \
                                       bFM3_GPIO_PFR1_P3=0, \
                                       bFM3_GPIO_ADE_AN3=0 )

/*---- GPIO bit P14 ----*/

#define GPIO1PIN_P14_GET             ( bFM3_GPIO_PDIR1_P4 )

#define GPIO1PIN_P14_PUT(v)          ( bFM3_GPIO_PDOR1_P4=(v) )

#define GPIO1PIN_P14_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P14_INITIN(v) \
                                                        : GPIO1PIN_P14_INITOUT(v) )

#define GPIO1PIN_P14_INITIN(v)       ( bFM3_GPIO_PCR1_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P4=0, \
                                       bFM3_GPIO_PFR1_P4=0, \
                                       bFM3_GPIO_ADE_AN4=0 )

#define GPIO1PIN_P14_INITOUT(v)      ( bFM3_GPIO_PDOR1_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P4=1, \
                                       bFM3_GPIO_PFR1_P4=0, \
                                       bFM3_GPIO_ADE_AN4=0 )


/*---- GPIO bit NP14 ----*/

#define GPIO1PIN_NP14_GET            ( !(bFM3_GPIO_PDIR1_P4) )

#define GPIO1PIN_NP14_PUT(v)         ( bFM3_GPIO_PDOR1_P4=!(v) )

#define GPIO1PIN_NP14_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP14_INITIN(v) \
                                                        : GPIO1PIN_NP14_INITOUT(v) )

#define GPIO1PIN_NP14_INITIN(v)      ( bFM3_GPIO_PCR1_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P4=0, \
                                       bFM3_GPIO_PFR1_P4=0, \
                                       bFM3_GPIO_ADE_AN4=0 )

#define GPIO1PIN_NP14_INITOUT(v)     ( bFM3_GPIO_PDOR1_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P4=1, \
                                       bFM3_GPIO_PFR1_P4=0, \
                                       bFM3_GPIO_ADE_AN4=0 )

/*---- GPIO bit P15 ----*/

#define GPIO1PIN_P15_GET             ( bFM3_GPIO_PDIR1_P5 )

#define GPIO1PIN_P15_PUT(v)          ( bFM3_GPIO_PDOR1_P5=(v) )

#define GPIO1PIN_P15_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P15_INITIN(v) \
                                                        : GPIO1PIN_P15_INITOUT(v) )

#define GPIO1PIN_P15_INITIN(v)       ( bFM3_GPIO_PCR1_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P5=0, \
                                       bFM3_GPIO_PFR1_P5=0, \
                                       bFM3_GPIO_ADE_AN5=0 )

#define GPIO1PIN_P15_INITOUT(v)      ( bFM3_GPIO_PDOR1_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P5=1, \
                                       bFM3_GPIO_PFR1_P5=0, \
                                       bFM3_GPIO_ADE_AN5=0 )


/*---- GPIO bit NP15 ----*/

#define GPIO1PIN_NP15_GET            ( !(bFM3_GPIO_PDIR1_P5) )

#define GPIO1PIN_NP15_PUT(v)         ( bFM3_GPIO_PDOR1_P5=!(v) )

#define GPIO1PIN_NP15_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP15_INITIN(v) \
                                                        : GPIO1PIN_NP15_INITOUT(v) )

#define GPIO1PIN_NP15_INITIN(v)      ( bFM3_GPIO_PCR1_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P5=0, \
                                       bFM3_GPIO_PFR1_P5=0, \
                                       bFM3_GPIO_ADE_AN5=0 )

#define GPIO1PIN_NP15_INITOUT(v)     ( bFM3_GPIO_PDOR1_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P5=1, \
                                       bFM3_GPIO_PFR1_P5=0, \
                                       bFM3_GPIO_ADE_AN5=0 )

/*---- GPIO bit P16 ----*/

#define GPIO1PIN_P16_GET             ( bFM3_GPIO_PDIR1_P6 )

#define GPIO1PIN_P16_PUT(v)          ( bFM3_GPIO_PDOR1_P6=(v) )

#define GPIO1PIN_P16_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P16_INITIN(v) \
                                                        : GPIO1PIN_P16_INITOUT(v) )

#define GPIO1PIN_P16_INITIN(v)       ( bFM3_GPIO_PCR1_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P6=0, \
                                       bFM3_GPIO_PFR1_P6=0, \
                                       bFM3_GPIO_ADE_AN6=0 )

#define GPIO1PIN_P16_INITOUT(v)      ( bFM3_GPIO_PDOR1_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P6=1, \
                                       bFM3_GPIO_PFR1_P6=0, \
                                       bFM3_GPIO_ADE_AN6=0 )


/*---- GPIO bit NP16 ----*/

#define GPIO1PIN_NP16_GET            ( !(bFM3_GPIO_PDIR1_P6) )

#define GPIO1PIN_NP16_PUT(v)         ( bFM3_GPIO_PDOR1_P6=!(v) )

#define GPIO1PIN_NP16_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP16_INITIN(v) \
                                                        : GPIO1PIN_NP16_INITOUT(v) )

#define GPIO1PIN_NP16_INITIN(v)      ( bFM3_GPIO_PCR1_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P6=0, \
                                       bFM3_GPIO_PFR1_P6=0, \
                                       bFM3_GPIO_ADE_AN6=0 )

#define GPIO1PIN_NP16_INITOUT(v)     ( bFM3_GPIO_PDOR1_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P6=1, \
                                       bFM3_GPIO_PFR1_P6=0, \
                                       bFM3_GPIO_ADE_AN6=0 )

/*---- GPIO bit P17 ----*/

#define GPIO1PIN_P17_GET             ( bFM3_GPIO_PDIR1_P7 )

#define GPIO1PIN_P17_PUT(v)          ( bFM3_GPIO_PDOR1_P7=(v) )

#define GPIO1PIN_P17_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P17_INITIN(v) \
                                                        : GPIO1PIN_P17_INITOUT(v) )

#define GPIO1PIN_P17_INITIN(v)       ( bFM3_GPIO_PCR1_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P7=0, \
                                       bFM3_GPIO_PFR1_P7=0, \
                                       bFM3_GPIO_ADE_AN7=0 )

#define GPIO1PIN_P17_INITOUT(v)      ( bFM3_GPIO_PDOR1_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P7=1, \
                                       bFM3_GPIO_PFR1_P7=0, \
                                       bFM3_GPIO_ADE_AN7=0 )


/*---- GPIO bit NP17 ----*/

#define GPIO1PIN_NP17_GET            ( !(bFM3_GPIO_PDIR1_P7) )

#define GPIO1PIN_NP17_PUT(v)         ( bFM3_GPIO_PDOR1_P7=!(v) )

#define GPIO1PIN_NP17_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP17_INITIN(v) \
                                                        : GPIO1PIN_NP17_INITOUT(v) )

#define GPIO1PIN_NP17_INITIN(v)      ( bFM3_GPIO_PCR1_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P7=0, \
                                       bFM3_GPIO_PFR1_P7=0, \
                                       bFM3_GPIO_ADE_AN7=0 )

#define GPIO1PIN_NP17_INITOUT(v)     ( bFM3_GPIO_PDOR1_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P7=1, \
                                       bFM3_GPIO_PFR1_P7=0, \
                                       bFM3_GPIO_ADE_AN7=0 )

/*---- GPIO bit P18 ----*/

#define GPIO1PIN_P18_GET             ( bFM3_GPIO_PDIR1_P8 )

#define GPIO1PIN_P18_PUT(v)          ( bFM3_GPIO_PDOR1_P8=(v) )

#define GPIO1PIN_P18_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P18_INITIN(v) \
                                                        : GPIO1PIN_P18_INITOUT(v) )

#define GPIO1PIN_P18_INITIN(v)       ( bFM3_GPIO_PCR1_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P8=0, \
                                       bFM3_GPIO_PFR1_P8=0, \
                                       bFM3_GPIO_ADE_AN8=0 )

#define GPIO1PIN_P18_INITOUT(v)      ( bFM3_GPIO_PDOR1_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P8=1, \
                                       bFM3_GPIO_PFR1_P8=0, \
                                       bFM3_GPIO_ADE_AN8=0 )


/*---- GPIO bit NP18 ----*/

#define GPIO1PIN_NP18_GET            ( !(bFM3_GPIO_PDIR1_P8) )

#define GPIO1PIN_NP18_PUT(v)         ( bFM3_GPIO_PDOR1_P8=!(v) )

#define GPIO1PIN_NP18_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP18_INITIN(v) \
                                                        : GPIO1PIN_NP18_INITOUT(v) )

#define GPIO1PIN_NP18_INITIN(v)      ( bFM3_GPIO_PCR1_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P8=0, \
                                       bFM3_GPIO_PFR1_P8=0, \
                                       bFM3_GPIO_ADE_AN8=0 )

#define GPIO1PIN_NP18_INITOUT(v)     ( bFM3_GPIO_PDOR1_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P8=1, \
                                       bFM3_GPIO_PFR1_P8=0, \
                                       bFM3_GPIO_ADE_AN8=0 )

/*---- GPIO bit P19 ----*/

#define GPIO1PIN_P19_GET             ( bFM3_GPIO_PDIR1_P9 )

#define GPIO1PIN_P19_PUT(v)          ( bFM3_GPIO_PDOR1_P9=(v) )

#define GPIO1PIN_P19_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P19_INITIN(v) \
                                                        : GPIO1PIN_P19_INITOUT(v) )

#define GPIO1PIN_P19_INITIN(v)       ( bFM3_GPIO_PCR1_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P9=0, \
                                       bFM3_GPIO_PFR1_P9=0, \
                                       bFM3_GPIO_ADE_AN9=0 )

#define GPIO1PIN_P19_INITOUT(v)      ( bFM3_GPIO_PDOR1_P9=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_P9=1, \
                                       bFM3_GPIO_PFR1_P9=0, \
                                       bFM3_GPIO_ADE_AN9=0 )


/*---- GPIO bit NP19 ----*/

#define GPIO1PIN_NP19_GET            ( !(bFM3_GPIO_PDIR1_P9) )

#define GPIO1PIN_NP19_PUT(v)         ( bFM3_GPIO_PDOR1_P9=!(v) )

#define GPIO1PIN_NP19_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP19_INITIN(v) \
                                                        : GPIO1PIN_NP19_INITOUT(v) )

#define GPIO1PIN_NP19_INITIN(v)      ( bFM3_GPIO_PCR1_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR1_P9=0, \
                                       bFM3_GPIO_PFR1_P9=0, \
                                       bFM3_GPIO_ADE_AN9=0 )

#define GPIO1PIN_NP19_INITOUT(v)     ( bFM3_GPIO_PDOR1_P9=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_P9=1, \
                                       bFM3_GPIO_PFR1_P9=0, \
                                       bFM3_GPIO_ADE_AN9=0 )

/*---- GPIO bit P1A ----*/

#define GPIO1PIN_P1A_GET             ( bFM3_GPIO_PDIR1_PA )

#define GPIO1PIN_P1A_PUT(v)          ( bFM3_GPIO_PDOR1_PA=(v) )

#define GPIO1PIN_P1A_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P1A_INITIN(v) \
                                                        : GPIO1PIN_P1A_INITOUT(v) )

#define GPIO1PIN_P1A_INITIN(v)       ( bFM3_GPIO_PCR1_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PA=0, \
                                       bFM3_GPIO_PFR1_PA=0, \
                                       bFM3_GPIO_ADE_ANA=0 )

#define GPIO1PIN_P1A_INITOUT(v)      ( bFM3_GPIO_PDOR1_PA=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_PA=1, \
                                       bFM3_GPIO_PFR1_PA=0, \
                                       bFM3_GPIO_ADE_ANA=0 )


/*---- GPIO bit NP1A ----*/

#define GPIO1PIN_NP1A_GET            ( !(bFM3_GPIO_PDIR1_PA) )

#define GPIO1PIN_NP1A_PUT(v)         ( bFM3_GPIO_PDOR1_PA=!(v) )

#define GPIO1PIN_NP1A_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP1A_INITIN(v) \
                                                        : GPIO1PIN_NP1A_INITOUT(v) )

#define GPIO1PIN_NP1A_INITIN(v)      ( bFM3_GPIO_PCR1_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PA=0, \
                                       bFM3_GPIO_PFR1_PA=0, \
                                       bFM3_GPIO_ADE_ANA=0 )

#define GPIO1PIN_NP1A_INITOUT(v)     ( bFM3_GPIO_PDOR1_PA=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_PA=1, \
                                       bFM3_GPIO_PFR1_PA=0, \
                                       bFM3_GPIO_ADE_ANA=0 )

/*---- GPIO bit P1B ----*/

#define GPIO1PIN_P1B_GET             ( bFM3_GPIO_PDIR1_PB )

#define GPIO1PIN_P1B_PUT(v)          ( bFM3_GPIO_PDOR1_PB=(v) )

#define GPIO1PIN_P1B_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P1B_INITIN(v) \
                                                        : GPIO1PIN_P1B_INITOUT(v) )

#define GPIO1PIN_P1B_INITIN(v)       ( bFM3_GPIO_PCR1_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PB=0, \
                                       bFM3_GPIO_PFR1_PB=0, \
                                       bFM3_GPIO_ADE_ANB=0 )

#define GPIO1PIN_P1B_INITOUT(v)      ( bFM3_GPIO_PDOR1_PB=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_PB=1, \
                                       bFM3_GPIO_PFR1_PB=0, \
                                       bFM3_GPIO_ADE_ANB=0 )


/*---- GPIO bit NP1B ----*/

#define GPIO1PIN_NP1B_GET            ( !(bFM3_GPIO_PDIR1_PB) )

#define GPIO1PIN_NP1B_PUT(v)         ( bFM3_GPIO_PDOR1_PB=!(v) )

#define GPIO1PIN_NP1B_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP1B_INITIN(v) \
                                                        : GPIO1PIN_NP1B_INITOUT(v) )

#define GPIO1PIN_NP1B_INITIN(v)      ( bFM3_GPIO_PCR1_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PB=0, \
                                       bFM3_GPIO_PFR1_PB=0, \
                                       bFM3_GPIO_ADE_ANB=0 )

#define GPIO1PIN_NP1B_INITOUT(v)     ( bFM3_GPIO_PDOR1_PB=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_PB=1, \
                                       bFM3_GPIO_PFR1_PB=0, \
                                       bFM3_GPIO_ADE_ANB=0 )

/*---- GPIO bit P1C ----*/

#define GPIO1PIN_P1C_GET             ( bFM3_GPIO_PDIR1_PC )

#define GPIO1PIN_P1C_PUT(v)          ( bFM3_GPIO_PDOR1_PC=(v) )

#define GPIO1PIN_P1C_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P1C_INITIN(v) \
                                                        : GPIO1PIN_P1C_INITOUT(v) )

#define GPIO1PIN_P1C_INITIN(v)       ( bFM3_GPIO_PCR1_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PC=0, \
                                       bFM3_GPIO_PFR1_PC=0, \
                                       bFM3_GPIO_ADE_ANC=0 )

#define GPIO1PIN_P1C_INITOUT(v)      ( bFM3_GPIO_PDOR1_PC=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_PC=1, \
                                       bFM3_GPIO_PFR1_PC=0, \
                                       bFM3_GPIO_ADE_ANC=0 )


/*---- GPIO bit NP1C ----*/

#define GPIO1PIN_NP1C_GET            ( !(bFM3_GPIO_PDIR1_PC) )

#define GPIO1PIN_NP1C_PUT(v)         ( bFM3_GPIO_PDOR1_PC=!(v) )

#define GPIO1PIN_NP1C_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP1C_INITIN(v) \
                                                        : GPIO1PIN_NP1C_INITOUT(v) )

#define GPIO1PIN_NP1C_INITIN(v)      ( bFM3_GPIO_PCR1_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PC=0, \
                                       bFM3_GPIO_PFR1_PC=0, \
                                       bFM3_GPIO_ADE_ANC=0 )

#define GPIO1PIN_NP1C_INITOUT(v)     ( bFM3_GPIO_PDOR1_PC=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_PC=1, \
                                       bFM3_GPIO_PFR1_PC=0, \
                                       bFM3_GPIO_ADE_ANC=0 )

/*---- GPIO bit P1D ----*/

#define GPIO1PIN_P1D_GET             ( bFM3_GPIO_PDIR1_PD )

#define GPIO1PIN_P1D_PUT(v)          ( bFM3_GPIO_PDOR1_PD=(v) )

#define GPIO1PIN_P1D_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P1D_INITIN(v) \
                                                        : GPIO1PIN_P1D_INITOUT(v) )

#define GPIO1PIN_P1D_INITIN(v)       ( bFM3_GPIO_PCR1_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PD=0, \
                                       bFM3_GPIO_PFR1_PD=0, \
                                       bFM3_GPIO_ADE_AND=0 )

#define GPIO1PIN_P1D_INITOUT(v)      ( bFM3_GPIO_PDOR1_PD=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_PD=1, \
                                       bFM3_GPIO_PFR1_PD=0, \
                                       bFM3_GPIO_ADE_AND=0 )


/*---- GPIO bit NP1D ----*/

#define GPIO1PIN_NP1D_GET            ( !(bFM3_GPIO_PDIR1_PD) )

#define GPIO1PIN_NP1D_PUT(v)         ( bFM3_GPIO_PDOR1_PD=!(v) )

#define GPIO1PIN_NP1D_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP1D_INITIN(v) \
                                                        : GPIO1PIN_NP1D_INITOUT(v) )

#define GPIO1PIN_NP1D_INITIN(v)      ( bFM3_GPIO_PCR1_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PD=0, \
                                       bFM3_GPIO_PFR1_PD=0, \
                                       bFM3_GPIO_ADE_AND=0 )

#define GPIO1PIN_NP1D_INITOUT(v)     ( bFM3_GPIO_PDOR1_PD=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_PD=1, \
                                       bFM3_GPIO_PFR1_PD=0, \
                                       bFM3_GPIO_ADE_AND=0 )

/*---- GPIO bit P1E ----*/

#define GPIO1PIN_P1E_GET             ( bFM3_GPIO_PDIR1_PE )

#define GPIO1PIN_P1E_PUT(v)          ( bFM3_GPIO_PDOR1_PE=(v) )

#define GPIO1PIN_P1E_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P1E_INITIN(v) \
                                                        : GPIO1PIN_P1E_INITOUT(v) )

#define GPIO1PIN_P1E_INITIN(v)       ( bFM3_GPIO_PCR1_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PE=0, \
                                       bFM3_GPIO_PFR1_PE=0, \
                                       bFM3_GPIO_ADE_ANE=0 )

#define GPIO1PIN_P1E_INITOUT(v)      ( bFM3_GPIO_PDOR1_PE=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_PE=1, \
                                       bFM3_GPIO_PFR1_PE=0, \
                                       bFM3_GPIO_ADE_ANE=0 )


/*---- GPIO bit NP1E ----*/

#define GPIO1PIN_NP1E_GET            ( !(bFM3_GPIO_PDIR1_PE) )

#define GPIO1PIN_NP1E_PUT(v)         ( bFM3_GPIO_PDOR1_PE=!(v) )

#define GPIO1PIN_NP1E_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP1E_INITIN(v) \
                                                        : GPIO1PIN_NP1E_INITOUT(v) )

#define GPIO1PIN_NP1E_INITIN(v)      ( bFM3_GPIO_PCR1_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PE=0, \
                                       bFM3_GPIO_PFR1_PE=0, \
                                       bFM3_GPIO_ADE_ANE=0 )

#define GPIO1PIN_NP1E_INITOUT(v)     ( bFM3_GPIO_PDOR1_PE=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_PE=1, \
                                       bFM3_GPIO_PFR1_PE=0, \
                                       bFM3_GPIO_ADE_ANE=0 )

/*---- GPIO bit P1F ----*/

#define GPIO1PIN_P1F_GET             ( bFM3_GPIO_PDIR1_PF )

#define GPIO1PIN_P1F_PUT(v)          ( bFM3_GPIO_PDOR1_PF=(v) )

#define GPIO1PIN_P1F_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P1F_INITIN(v) \
                                                        : GPIO1PIN_P1F_INITOUT(v) )

#define GPIO1PIN_P1F_INITIN(v)       ( bFM3_GPIO_PCR1_PF=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PF=0, \
                                       bFM3_GPIO_PFR1_PF=0, \
                                       bFM3_GPIO_ADE_ANF=0 )

#define GPIO1PIN_P1F_INITOUT(v)      ( bFM3_GPIO_PDOR1_PF=(v).bInitVal, \
                                       bFM3_GPIO_DDR1_PF=1, \
                                       bFM3_GPIO_PFR1_PF=0, \
                                       bFM3_GPIO_ADE_ANF=0 )


/*---- GPIO bit NP1F ----*/

#define GPIO1PIN_NP1F_GET            ( !(bFM3_GPIO_PDIR1_PF) )

#define GPIO1PIN_NP1F_PUT(v)         ( bFM3_GPIO_PDOR1_PF=!(v) )

#define GPIO1PIN_NP1F_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP1F_INITIN(v) \
                                                        : GPIO1PIN_NP1F_INITOUT(v) )

#define GPIO1PIN_NP1F_INITIN(v)      ( bFM3_GPIO_PCR1_PF=(v).bPullup, \
                                       bFM3_GPIO_DDR1_PF=0, \
                                       bFM3_GPIO_PFR1_PF=0, \
                                       bFM3_GPIO_ADE_ANF=0 )

#define GPIO1PIN_NP1F_INITOUT(v)     ( bFM3_GPIO_PDOR1_PF=!((v).bInitVal), \
                                       bFM3_GPIO_DDR1_PF=1, \
                                       bFM3_GPIO_PFR1_PF=0, \
                                       bFM3_GPIO_ADE_ANF=0 )

/*---- GPIO bit P20 ----*/

#define GPIO1PIN_P20_GET             ( bFM3_GPIO_PDIR2_P0 )

#define GPIO1PIN_P20_PUT(v)          ( bFM3_GPIO_PDOR2_P0=(v) )

#define GPIO1PIN_P20_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P20_INITIN(v) \
                                                        : GPIO1PIN_P20_INITOUT(v) )

#define GPIO1PIN_P20_INITIN(v)       ( bFM3_GPIO_PCR2_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P0=0, \
                                       bFM3_GPIO_PFR2_P0=0 )

#define GPIO1PIN_P20_INITOUT(v)      ( bFM3_GPIO_PDOR2_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P0=1, \
                                       bFM3_GPIO_PFR2_P0=0 )


/*---- GPIO bit NP20 ----*/

#define GPIO1PIN_NP20_GET            ( !(bFM3_GPIO_PDIR2_P0) )

#define GPIO1PIN_NP20_PUT(v)         ( bFM3_GPIO_PDOR2_P0=!(v) )

#define GPIO1PIN_NP20_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP20_INITIN(v) \
                                                        : GPIO1PIN_NP20_INITOUT(v) )

#define GPIO1PIN_NP20_INITIN(v)      ( bFM3_GPIO_PCR2_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P0=0, \
                                       bFM3_GPIO_PFR2_P0=0 )

#define GPIO1PIN_NP20_INITOUT(v)     ( bFM3_GPIO_PDOR2_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P0=1, \
                                       bFM3_GPIO_PFR2_P0=0 )

/*---- GPIO bit P21 ----*/

#define GPIO1PIN_P21_GET             ( bFM3_GPIO_PDIR2_P1 )

#define GPIO1PIN_P21_PUT(v)          ( bFM3_GPIO_PDOR2_P1=(v) )

#define GPIO1PIN_P21_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P21_INITIN(v) \
                                                        : GPIO1PIN_P21_INITOUT(v) )

#define GPIO1PIN_P21_INITIN(v)       ( bFM3_GPIO_PCR2_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P1=0, \
                                       bFM3_GPIO_PFR2_P1=0 )

#define GPIO1PIN_P21_INITOUT(v)      ( bFM3_GPIO_PDOR2_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P1=1, \
                                       bFM3_GPIO_PFR2_P1=0 )


/*---- GPIO bit NP21 ----*/

#define GPIO1PIN_NP21_GET            ( !(bFM3_GPIO_PDIR2_P1) )

#define GPIO1PIN_NP21_PUT(v)         ( bFM3_GPIO_PDOR2_P1=!(v) )

#define GPIO1PIN_NP21_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP21_INITIN(v) \
                                                        : GPIO1PIN_NP21_INITOUT(v) )

#define GPIO1PIN_NP21_INITIN(v)      ( bFM3_GPIO_PCR2_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P1=0, \
                                       bFM3_GPIO_PFR2_P1=0 )

#define GPIO1PIN_NP21_INITOUT(v)     ( bFM3_GPIO_PDOR2_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P1=1, \
                                       bFM3_GPIO_PFR2_P1=0 )

/*---- GPIO bit P22 ----*/

#define GPIO1PIN_P22_GET             ( bFM3_GPIO_PDIR2_P2 )

#define GPIO1PIN_P22_PUT(v)          ( bFM3_GPIO_PDOR2_P2=(v) )

#define GPIO1PIN_P22_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P22_INITIN(v) \
                                                        : GPIO1PIN_P22_INITOUT(v) )

#define GPIO1PIN_P22_INITIN(v)       ( bFM3_GPIO_PCR2_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P2=0, \
                                       bFM3_GPIO_PFR2_P2=0 )

#define GPIO1PIN_P22_INITOUT(v)      ( bFM3_GPIO_PDOR2_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P2=1, \
                                       bFM3_GPIO_PFR2_P2=0 )


/*---- GPIO bit NP22 ----*/

#define GPIO1PIN_NP22_GET            ( !(bFM3_GPIO_PDIR2_P2) )

#define GPIO1PIN_NP22_PUT(v)         ( bFM3_GPIO_PDOR2_P2=!(v) )

#define GPIO1PIN_NP22_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP22_INITIN(v) \
                                                        : GPIO1PIN_NP22_INITOUT(v) )

#define GPIO1PIN_NP22_INITIN(v)      ( bFM3_GPIO_PCR2_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P2=0, \
                                       bFM3_GPIO_PFR2_P2=0 )

#define GPIO1PIN_NP22_INITOUT(v)     ( bFM3_GPIO_PDOR2_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P2=1, \
                                       bFM3_GPIO_PFR2_P2=0 )

/*---- GPIO bit P23 ----*/

#define GPIO1PIN_P23_GET             ( bFM3_GPIO_PDIR2_P3 )

#define GPIO1PIN_P23_PUT(v)          ( bFM3_GPIO_PDOR2_P3=(v) )

#define GPIO1PIN_P23_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P23_INITIN(v) \
                                                        : GPIO1PIN_P23_INITOUT(v) )

#define GPIO1PIN_P23_INITIN(v)       ( bFM3_GPIO_PCR2_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P3=0, \
                                       bFM3_GPIO_PFR2_P3=0 )

#define GPIO1PIN_P23_INITOUT(v)      ( bFM3_GPIO_PDOR2_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P3=1, \
                                       bFM3_GPIO_PFR2_P3=0 )


/*---- GPIO bit NP23 ----*/

#define GPIO1PIN_NP23_GET            ( !(bFM3_GPIO_PDIR2_P3) )

#define GPIO1PIN_NP23_PUT(v)         ( bFM3_GPIO_PDOR2_P3=!(v) )

#define GPIO1PIN_NP23_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP23_INITIN(v) \
                                                        : GPIO1PIN_NP23_INITOUT(v) )

#define GPIO1PIN_NP23_INITIN(v)      ( bFM3_GPIO_PCR2_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P3=0, \
                                       bFM3_GPIO_PFR2_P3=0 )

#define GPIO1PIN_NP23_INITOUT(v)     ( bFM3_GPIO_PDOR2_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P3=1, \
                                       bFM3_GPIO_PFR2_P3=0 )

/*---- GPIO bit P24 ----*/

#define GPIO1PIN_P24_GET             ( bFM3_GPIO_PDIR2_P4 )

#define GPIO1PIN_P24_PUT(v)          ( bFM3_GPIO_PDOR2_P4=(v) )

#define GPIO1PIN_P24_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P24_INITIN(v) \
                                                        : GPIO1PIN_P24_INITOUT(v) )

#define GPIO1PIN_P24_INITIN(v)       ( bFM3_GPIO_PCR2_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P4=0, \
                                       bFM3_GPIO_PFR2_P4=0 )

#define GPIO1PIN_P24_INITOUT(v)      ( bFM3_GPIO_PDOR2_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P4=1, \
                                       bFM3_GPIO_PFR2_P4=0 )


/*---- GPIO bit NP24 ----*/

#define GPIO1PIN_NP24_GET            ( !(bFM3_GPIO_PDIR2_P4) )

#define GPIO1PIN_NP24_PUT(v)         ( bFM3_GPIO_PDOR2_P4=!(v) )

#define GPIO1PIN_NP24_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP24_INITIN(v) \
                                                        : GPIO1PIN_NP24_INITOUT(v) )

#define GPIO1PIN_NP24_INITIN(v)      ( bFM3_GPIO_PCR2_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P4=0, \
                                       bFM3_GPIO_PFR2_P4=0 )

#define GPIO1PIN_NP24_INITOUT(v)     ( bFM3_GPIO_PDOR2_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P4=1, \
                                       bFM3_GPIO_PFR2_P4=0 )

/*---- GPIO bit P25 ----*/

#define GPIO1PIN_P25_GET             ( bFM3_GPIO_PDIR2_P5 )

#define GPIO1PIN_P25_PUT(v)          ( bFM3_GPIO_PDOR2_P5=(v) )

#define GPIO1PIN_P25_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P25_INITIN(v) \
                                                        : GPIO1PIN_P25_INITOUT(v) )

#define GPIO1PIN_P25_INITIN(v)       ( bFM3_GPIO_PCR2_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P5=0, \
                                       bFM3_GPIO_PFR2_P5=0 )

#define GPIO1PIN_P25_INITOUT(v)      ( bFM3_GPIO_PDOR2_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P5=1, \
                                       bFM3_GPIO_PFR2_P5=0 )


/*---- GPIO bit NP25 ----*/

#define GPIO1PIN_NP25_GET            ( !(bFM3_GPIO_PDIR2_P5) )

#define GPIO1PIN_NP25_PUT(v)         ( bFM3_GPIO_PDOR2_P5=!(v) )

#define GPIO1PIN_NP25_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP25_INITIN(v) \
                                                        : GPIO1PIN_NP25_INITOUT(v) )

#define GPIO1PIN_NP25_INITIN(v)      ( bFM3_GPIO_PCR2_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P5=0, \
                                       bFM3_GPIO_PFR2_P5=0 )

#define GPIO1PIN_NP25_INITOUT(v)     ( bFM3_GPIO_PDOR2_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P5=1, \
                                       bFM3_GPIO_PFR2_P5=0 )

/*---- GPIO bit P26 ----*/

#define GPIO1PIN_P26_GET             ( bFM3_GPIO_PDIR2_P6 )

#define GPIO1PIN_P26_PUT(v)          ( bFM3_GPIO_PDOR2_P6=(v) )

#define GPIO1PIN_P26_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P26_INITIN(v) \
                                                        : GPIO1PIN_P26_INITOUT(v) )

#define GPIO1PIN_P26_INITIN(v)       ( bFM3_GPIO_PCR2_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P6=0, \
                                       bFM3_GPIO_PFR2_P6=0 )

#define GPIO1PIN_P26_INITOUT(v)      ( bFM3_GPIO_PDOR2_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P6=1, \
                                       bFM3_GPIO_PFR2_P6=0 )


/*---- GPIO bit NP26 ----*/

#define GPIO1PIN_NP26_GET            ( !(bFM3_GPIO_PDIR2_P6) )

#define GPIO1PIN_NP26_PUT(v)         ( bFM3_GPIO_PDOR2_P6=!(v) )

#define GPIO1PIN_NP26_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP26_INITIN(v) \
                                                        : GPIO1PIN_NP26_INITOUT(v) )

#define GPIO1PIN_NP26_INITIN(v)      ( bFM3_GPIO_PCR2_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P6=0, \
                                       bFM3_GPIO_PFR2_P6=0 )

#define GPIO1PIN_NP26_INITOUT(v)     ( bFM3_GPIO_PDOR2_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P6=1, \
                                       bFM3_GPIO_PFR2_P6=0 )

/*---- GPIO bit P27 ----*/

#define GPIO1PIN_P27_GET             ( bFM3_GPIO_PDIR2_P7 )

#define GPIO1PIN_P27_PUT(v)          ( bFM3_GPIO_PDOR2_P7=(v) )

#define GPIO1PIN_P27_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P27_INITIN(v) \
                                                        : GPIO1PIN_P27_INITOUT(v) )

#define GPIO1PIN_P27_INITIN(v)       ( bFM3_GPIO_PCR2_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P7=0, \
                                       bFM3_GPIO_PFR2_P7=0 )

#define GPIO1PIN_P27_INITOUT(v)      ( bFM3_GPIO_PDOR2_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P7=1, \
                                       bFM3_GPIO_PFR2_P7=0 )


/*---- GPIO bit NP27 ----*/

#define GPIO1PIN_NP27_GET            ( !(bFM3_GPIO_PDIR2_P7) )

#define GPIO1PIN_NP27_PUT(v)         ( bFM3_GPIO_PDOR2_P7=!(v) )

#define GPIO1PIN_NP27_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP27_INITIN(v) \
                                                        : GPIO1PIN_NP27_INITOUT(v) )

#define GPIO1PIN_NP27_INITIN(v)      ( bFM3_GPIO_PCR2_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P7=0, \
                                       bFM3_GPIO_PFR2_P7=0 )

#define GPIO1PIN_NP27_INITOUT(v)     ( bFM3_GPIO_PDOR2_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P7=1, \
                                       bFM3_GPIO_PFR2_P7=0 )

/*---- GPIO bit P28 ----*/

#define GPIO1PIN_P28_GET             ( bFM3_GPIO_PDIR2_P8 )

#define GPIO1PIN_P28_PUT(v)          ( bFM3_GPIO_PDOR2_P8=(v) )

#define GPIO1PIN_P28_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P28_INITIN(v) \
                                                        : GPIO1PIN_P28_INITOUT(v) )

#define GPIO1PIN_P28_INITIN(v)       ( bFM3_GPIO_PCR2_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P8=0, \
                                       bFM3_GPIO_PFR2_P8=0 )

#define GPIO1PIN_P28_INITOUT(v)      ( bFM3_GPIO_PDOR2_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR2_P8=1, \
                                       bFM3_GPIO_PFR2_P8=0 )


/*---- GPIO bit NP28 ----*/

#define GPIO1PIN_NP28_GET            ( !(bFM3_GPIO_PDIR2_P8) )

#define GPIO1PIN_NP28_PUT(v)         ( bFM3_GPIO_PDOR2_P8=!(v) )

#define GPIO1PIN_NP28_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP28_INITIN(v) \
                                                        : GPIO1PIN_NP28_INITOUT(v) )

#define GPIO1PIN_NP28_INITIN(v)      ( bFM3_GPIO_PCR2_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR2_P8=0, \
                                       bFM3_GPIO_PFR2_P8=0 )

#define GPIO1PIN_NP28_INITOUT(v)     ( bFM3_GPIO_PDOR2_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR2_P8=1, \
                                       bFM3_GPIO_PFR2_P8=0 )

/*---- GPIO bit P30 ----*/

#define GPIO1PIN_P30_GET             ( bFM3_GPIO_PDIR3_P0 )

#define GPIO1PIN_P30_PUT(v)          ( bFM3_GPIO_PDOR3_P0=(v) )

#define GPIO1PIN_P30_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P30_INITIN(v) \
                                                        : GPIO1PIN_P30_INITOUT(v) )

#define GPIO1PIN_P30_INITIN(v)       ( bFM3_GPIO_PCR3_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P0=0, \
                                       bFM3_GPIO_PFR3_P0=0 )

#define GPIO1PIN_P30_INITOUT(v)      ( bFM3_GPIO_PDOR3_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P0=1, \
                                       bFM3_GPIO_PFR3_P0=0 )


/*---- GPIO bit NP30 ----*/

#define GPIO1PIN_NP30_GET            ( !(bFM3_GPIO_PDIR3_P0) )

#define GPIO1PIN_NP30_PUT(v)         ( bFM3_GPIO_PDOR3_P0=!(v) )

#define GPIO1PIN_NP30_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP30_INITIN(v) \
                                                        : GPIO1PIN_NP30_INITOUT(v) )

#define GPIO1PIN_NP30_INITIN(v)      ( bFM3_GPIO_PCR3_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P0=0, \
                                       bFM3_GPIO_PFR3_P0=0 )

#define GPIO1PIN_NP30_INITOUT(v)     ( bFM3_GPIO_PDOR3_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P0=1, \
                                       bFM3_GPIO_PFR3_P0=0 )

/*---- GPIO bit P31 ----*/

#define GPIO1PIN_P31_GET             ( bFM3_GPIO_PDIR3_P1 )

#define GPIO1PIN_P31_PUT(v)          ( bFM3_GPIO_PDOR3_P1=(v) )

#define GPIO1PIN_P31_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P31_INITIN(v) \
                                                        : GPIO1PIN_P31_INITOUT(v) )

#define GPIO1PIN_P31_INITIN(v)       ( bFM3_GPIO_PCR3_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P1=0, \
                                       bFM3_GPIO_PFR3_P1=0 )

#define GPIO1PIN_P31_INITOUT(v)      ( bFM3_GPIO_PDOR3_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P1=1, \
                                       bFM3_GPIO_PFR3_P1=0 )


/*---- GPIO bit NP31 ----*/

#define GPIO1PIN_NP31_GET            ( !(bFM3_GPIO_PDIR3_P1) )

#define GPIO1PIN_NP31_PUT(v)         ( bFM3_GPIO_PDOR3_P1=!(v) )

#define GPIO1PIN_NP31_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP31_INITIN(v) \
                                                        : GPIO1PIN_NP31_INITOUT(v) )

#define GPIO1PIN_NP31_INITIN(v)      ( bFM3_GPIO_PCR3_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P1=0, \
                                       bFM3_GPIO_PFR3_P1=0 )

#define GPIO1PIN_NP31_INITOUT(v)     ( bFM3_GPIO_PDOR3_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P1=1, \
                                       bFM3_GPIO_PFR3_P1=0 )

/*---- GPIO bit P32 ----*/

#define GPIO1PIN_P32_GET             ( bFM3_GPIO_PDIR3_P2 )

#define GPIO1PIN_P32_PUT(v)          ( bFM3_GPIO_PDOR3_P2=(v) )

#define GPIO1PIN_P32_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P32_INITIN(v) \
                                                        : GPIO1PIN_P32_INITOUT(v) )

#define GPIO1PIN_P32_INITIN(v)       ( bFM3_GPIO_PCR3_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P2=0, \
                                       bFM3_GPIO_PFR3_P2=0 )

#define GPIO1PIN_P32_INITOUT(v)      ( bFM3_GPIO_PDOR3_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P2=1, \
                                       bFM3_GPIO_PFR3_P2=0 )


/*---- GPIO bit NP32 ----*/

#define GPIO1PIN_NP32_GET            ( !(bFM3_GPIO_PDIR3_P2) )

#define GPIO1PIN_NP32_PUT(v)         ( bFM3_GPIO_PDOR3_P2=!(v) )

#define GPIO1PIN_NP32_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP32_INITIN(v) \
                                                        : GPIO1PIN_NP32_INITOUT(v) )

#define GPIO1PIN_NP32_INITIN(v)      ( bFM3_GPIO_PCR3_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P2=0, \
                                       bFM3_GPIO_PFR3_P2=0 )

#define GPIO1PIN_NP32_INITOUT(v)     ( bFM3_GPIO_PDOR3_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P2=1, \
                                       bFM3_GPIO_PFR3_P2=0 )

/*---- GPIO bit P33 ----*/

#define GPIO1PIN_P33_GET             ( bFM3_GPIO_PDIR3_P3 )

#define GPIO1PIN_P33_PUT(v)          ( bFM3_GPIO_PDOR3_P3=(v) )

#define GPIO1PIN_P33_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P33_INITIN(v) \
                                                        : GPIO1PIN_P33_INITOUT(v) )

#define GPIO1PIN_P33_INITIN(v)       ( bFM3_GPIO_PCR3_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P3=0, \
                                       bFM3_GPIO_PFR3_P3=0 )

#define GPIO1PIN_P33_INITOUT(v)      ( bFM3_GPIO_PDOR3_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P3=1, \
                                       bFM3_GPIO_PFR3_P3=0 )


/*---- GPIO bit NP33 ----*/

#define GPIO1PIN_NP33_GET            ( !(bFM3_GPIO_PDIR3_P3) )

#define GPIO1PIN_NP33_PUT(v)         ( bFM3_GPIO_PDOR3_P3=!(v) )

#define GPIO1PIN_NP33_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP33_INITIN(v) \
                                                        : GPIO1PIN_NP33_INITOUT(v) )

#define GPIO1PIN_NP33_INITIN(v)      ( bFM3_GPIO_PCR3_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P3=0, \
                                       bFM3_GPIO_PFR3_P3=0 )

#define GPIO1PIN_NP33_INITOUT(v)     ( bFM3_GPIO_PDOR3_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P3=1, \
                                       bFM3_GPIO_PFR3_P3=0 )

/*---- GPIO bit P34 ----*/

#define GPIO1PIN_P34_GET             ( bFM3_GPIO_PDIR3_P4 )

#define GPIO1PIN_P34_PUT(v)          ( bFM3_GPIO_PDOR3_P4=(v) )

#define GPIO1PIN_P34_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P34_INITIN(v) \
                                                        : GPIO1PIN_P34_INITOUT(v) )

#define GPIO1PIN_P34_INITIN(v)       ( bFM3_GPIO_PCR3_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P4=0, \
                                       bFM3_GPIO_PFR3_P4=0 )

#define GPIO1PIN_P34_INITOUT(v)      ( bFM3_GPIO_PDOR3_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P4=1, \
                                       bFM3_GPIO_PFR3_P4=0 )


/*---- GPIO bit NP34 ----*/

#define GPIO1PIN_NP34_GET            ( !(bFM3_GPIO_PDIR3_P4) )

#define GPIO1PIN_NP34_PUT(v)         ( bFM3_GPIO_PDOR3_P4=!(v) )

#define GPIO1PIN_NP34_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP34_INITIN(v) \
                                                        : GPIO1PIN_NP34_INITOUT(v) )

#define GPIO1PIN_NP34_INITIN(v)      ( bFM3_GPIO_PCR3_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P4=0, \
                                       bFM3_GPIO_PFR3_P4=0 )

#define GPIO1PIN_NP34_INITOUT(v)     ( bFM3_GPIO_PDOR3_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P4=1, \
                                       bFM3_GPIO_PFR3_P4=0 )

/*---- GPIO bit P35 ----*/

#define GPIO1PIN_P35_GET             ( bFM3_GPIO_PDIR3_P5 )

#define GPIO1PIN_P35_PUT(v)          ( bFM3_GPIO_PDOR3_P5=(v) )

#define GPIO1PIN_P35_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P35_INITIN(v) \
                                                        : GPIO1PIN_P35_INITOUT(v) )

#define GPIO1PIN_P35_INITIN(v)       ( bFM3_GPIO_PCR3_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P5=0, \
                                       bFM3_GPIO_PFR3_P5=0 )

#define GPIO1PIN_P35_INITOUT(v)      ( bFM3_GPIO_PDOR3_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P5=1, \
                                       bFM3_GPIO_PFR3_P5=0 )


/*---- GPIO bit NP35 ----*/

#define GPIO1PIN_NP35_GET            ( !(bFM3_GPIO_PDIR3_P5) )

#define GPIO1PIN_NP35_PUT(v)         ( bFM3_GPIO_PDOR3_P5=!(v) )

#define GPIO1PIN_NP35_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP35_INITIN(v) \
                                                        : GPIO1PIN_NP35_INITOUT(v) )

#define GPIO1PIN_NP35_INITIN(v)      ( bFM3_GPIO_PCR3_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P5=0, \
                                       bFM3_GPIO_PFR3_P5=0 )

#define GPIO1PIN_NP35_INITOUT(v)     ( bFM3_GPIO_PDOR3_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P5=1, \
                                       bFM3_GPIO_PFR3_P5=0 )

/*---- GPIO bit P36 ----*/

#define GPIO1PIN_P36_GET             ( bFM3_GPIO_PDIR3_P6 )

#define GPIO1PIN_P36_PUT(v)          ( bFM3_GPIO_PDOR3_P6=(v) )

#define GPIO1PIN_P36_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P36_INITIN(v) \
                                                        : GPIO1PIN_P36_INITOUT(v) )

#define GPIO1PIN_P36_INITIN(v)       ( bFM3_GPIO_PCR3_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P6=0, \
                                       bFM3_GPIO_PFR3_P6=0 )

#define GPIO1PIN_P36_INITOUT(v)      ( bFM3_GPIO_PDOR3_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P6=1, \
                                       bFM3_GPIO_PFR3_P6=0 )


/*---- GPIO bit NP36 ----*/

#define GPIO1PIN_NP36_GET            ( !(bFM3_GPIO_PDIR3_P6) )

#define GPIO1PIN_NP36_PUT(v)         ( bFM3_GPIO_PDOR3_P6=!(v) )

#define GPIO1PIN_NP36_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP36_INITIN(v) \
                                                        : GPIO1PIN_NP36_INITOUT(v) )

#define GPIO1PIN_NP36_INITIN(v)      ( bFM3_GPIO_PCR3_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P6=0, \
                                       bFM3_GPIO_PFR3_P6=0 )

#define GPIO1PIN_NP36_INITOUT(v)     ( bFM3_GPIO_PDOR3_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P6=1, \
                                       bFM3_GPIO_PFR3_P6=0 )

/*---- GPIO bit P37 ----*/

#define GPIO1PIN_P37_GET             ( bFM3_GPIO_PDIR3_P7 )

#define GPIO1PIN_P37_PUT(v)          ( bFM3_GPIO_PDOR3_P7=(v) )

#define GPIO1PIN_P37_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P37_INITIN(v) \
                                                        : GPIO1PIN_P37_INITOUT(v) )

#define GPIO1PIN_P37_INITIN(v)       ( bFM3_GPIO_PCR3_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P7=0, \
                                       bFM3_GPIO_PFR3_P7=0 )

#define GPIO1PIN_P37_INITOUT(v)      ( bFM3_GPIO_PDOR3_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P7=1, \
                                       bFM3_GPIO_PFR3_P7=0 )


/*---- GPIO bit NP37 ----*/

#define GPIO1PIN_NP37_GET            ( !(bFM3_GPIO_PDIR3_P7) )

#define GPIO1PIN_NP37_PUT(v)         ( bFM3_GPIO_PDOR3_P7=!(v) )

#define GPIO1PIN_NP37_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP37_INITIN(v) \
                                                        : GPIO1PIN_NP37_INITOUT(v) )

#define GPIO1PIN_NP37_INITIN(v)      ( bFM3_GPIO_PCR3_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P7=0, \
                                       bFM3_GPIO_PFR3_P7=0 )

#define GPIO1PIN_NP37_INITOUT(v)     ( bFM3_GPIO_PDOR3_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P7=1, \
                                       bFM3_GPIO_PFR3_P7=0 )

/*---- GPIO bit P38 ----*/

#define GPIO1PIN_P38_GET             ( bFM3_GPIO_PDIR3_P8 )

#define GPIO1PIN_P38_PUT(v)          ( bFM3_GPIO_PDOR3_P8=(v) )

#define GPIO1PIN_P38_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P38_INITIN(v) \
                                                        : GPIO1PIN_P38_INITOUT(v) )

#define GPIO1PIN_P38_INITIN(v)       ( bFM3_GPIO_PCR3_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P8=0, \
                                       bFM3_GPIO_PFR3_P8=0 )

#define GPIO1PIN_P38_INITOUT(v)      ( bFM3_GPIO_PDOR3_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P8=1, \
                                       bFM3_GPIO_PFR3_P8=0 )


/*---- GPIO bit NP38 ----*/

#define GPIO1PIN_NP38_GET            ( !(bFM3_GPIO_PDIR3_P8) )

#define GPIO1PIN_NP38_PUT(v)         ( bFM3_GPIO_PDOR3_P8=!(v) )

#define GPIO1PIN_NP38_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP38_INITIN(v) \
                                                        : GPIO1PIN_NP38_INITOUT(v) )

#define GPIO1PIN_NP38_INITIN(v)      ( bFM3_GPIO_PCR3_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P8=0, \
                                       bFM3_GPIO_PFR3_P8=0 )

#define GPIO1PIN_NP38_INITOUT(v)     ( bFM3_GPIO_PDOR3_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P8=1, \
                                       bFM3_GPIO_PFR3_P8=0 )

/*---- GPIO bit P39 ----*/

#define GPIO1PIN_P39_GET             ( bFM3_GPIO_PDIR3_P9 )

#define GPIO1PIN_P39_PUT(v)          ( bFM3_GPIO_PDOR3_P9=(v) )

#define GPIO1PIN_P39_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P39_INITIN(v) \
                                                        : GPIO1PIN_P39_INITOUT(v) )

#define GPIO1PIN_P39_INITIN(v)       ( bFM3_GPIO_PCR3_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P9=0, \
                                       bFM3_GPIO_PFR3_P9=0 )

#define GPIO1PIN_P39_INITOUT(v)      ( bFM3_GPIO_PDOR3_P9=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_P9=1, \
                                       bFM3_GPIO_PFR3_P9=0 )


/*---- GPIO bit NP39 ----*/

#define GPIO1PIN_NP39_GET            ( !(bFM3_GPIO_PDIR3_P9) )

#define GPIO1PIN_NP39_PUT(v)         ( bFM3_GPIO_PDOR3_P9=!(v) )

#define GPIO1PIN_NP39_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP39_INITIN(v) \
                                                        : GPIO1PIN_NP39_INITOUT(v) )

#define GPIO1PIN_NP39_INITIN(v)      ( bFM3_GPIO_PCR3_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR3_P9=0, \
                                       bFM3_GPIO_PFR3_P9=0 )

#define GPIO1PIN_NP39_INITOUT(v)     ( bFM3_GPIO_PDOR3_P9=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_P9=1, \
                                       bFM3_GPIO_PFR3_P9=0 )

/*---- GPIO bit P3A ----*/

#define GPIO1PIN_P3A_GET             ( bFM3_GPIO_PDIR3_PA )

#define GPIO1PIN_P3A_PUT(v)          ( bFM3_GPIO_PDOR3_PA=(v) )

#define GPIO1PIN_P3A_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P3A_INITIN(v) \
                                                        : GPIO1PIN_P3A_INITOUT(v) )

#define GPIO1PIN_P3A_INITIN(v)       ( bFM3_GPIO_PCR3_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PA=0, \
                                       bFM3_GPIO_PFR3_PA=0 )

#define GPIO1PIN_P3A_INITOUT(v)      ( bFM3_GPIO_PDOR3_PA=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_PA=1, \
                                       bFM3_GPIO_PFR3_PA=0 )


/*---- GPIO bit NP3A ----*/

#define GPIO1PIN_NP3A_GET            ( !(bFM3_GPIO_PDIR3_PA) )

#define GPIO1PIN_NP3A_PUT(v)         ( bFM3_GPIO_PDOR3_PA=!(v) )

#define GPIO1PIN_NP3A_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP3A_INITIN(v) \
                                                        : GPIO1PIN_NP3A_INITOUT(v) )

#define GPIO1PIN_NP3A_INITIN(v)      ( bFM3_GPIO_PCR3_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PA=0, \
                                       bFM3_GPIO_PFR3_PA=0 )

#define GPIO1PIN_NP3A_INITOUT(v)     ( bFM3_GPIO_PDOR3_PA=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_PA=1, \
                                       bFM3_GPIO_PFR3_PA=0 )

/*---- GPIO bit P3B ----*/

#define GPIO1PIN_P3B_GET             ( bFM3_GPIO_PDIR3_PB )

#define GPIO1PIN_P3B_PUT(v)          ( bFM3_GPIO_PDOR3_PB=(v) )

#define GPIO1PIN_P3B_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P3B_INITIN(v) \
                                                        : GPIO1PIN_P3B_INITOUT(v) )

#define GPIO1PIN_P3B_INITIN(v)       ( bFM3_GPIO_PCR3_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PB=0, \
                                       bFM3_GPIO_PFR3_PB=0 )

#define GPIO1PIN_P3B_INITOUT(v)      ( bFM3_GPIO_PDOR3_PB=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_PB=1, \
                                       bFM3_GPIO_PFR3_PB=0 )


/*---- GPIO bit NP3B ----*/

#define GPIO1PIN_NP3B_GET            ( !(bFM3_GPIO_PDIR3_PB) )

#define GPIO1PIN_NP3B_PUT(v)         ( bFM3_GPIO_PDOR3_PB=!(v) )

#define GPIO1PIN_NP3B_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP3B_INITIN(v) \
                                                        : GPIO1PIN_NP3B_INITOUT(v) )

#define GPIO1PIN_NP3B_INITIN(v)      ( bFM3_GPIO_PCR3_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PB=0, \
                                       bFM3_GPIO_PFR3_PB=0 )

#define GPIO1PIN_NP3B_INITOUT(v)     ( bFM3_GPIO_PDOR3_PB=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_PB=1, \
                                       bFM3_GPIO_PFR3_PB=0 )

/*---- GPIO bit P3C ----*/

#define GPIO1PIN_P3C_GET             ( bFM3_GPIO_PDIR3_PC )

#define GPIO1PIN_P3C_PUT(v)          ( bFM3_GPIO_PDOR3_PC=(v) )

#define GPIO1PIN_P3C_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P3C_INITIN(v) \
                                                        : GPIO1PIN_P3C_INITOUT(v) )

#define GPIO1PIN_P3C_INITIN(v)       ( bFM3_GPIO_PCR3_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PC=0, \
                                       bFM3_GPIO_PFR3_PC=0 )

#define GPIO1PIN_P3C_INITOUT(v)      ( bFM3_GPIO_PDOR3_PC=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_PC=1, \
                                       bFM3_GPIO_PFR3_PC=0 )


/*---- GPIO bit NP3C ----*/

#define GPIO1PIN_NP3C_GET            ( !(bFM3_GPIO_PDIR3_PC) )

#define GPIO1PIN_NP3C_PUT(v)         ( bFM3_GPIO_PDOR3_PC=!(v) )

#define GPIO1PIN_NP3C_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP3C_INITIN(v) \
                                                        : GPIO1PIN_NP3C_INITOUT(v) )

#define GPIO1PIN_NP3C_INITIN(v)      ( bFM3_GPIO_PCR3_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PC=0, \
                                       bFM3_GPIO_PFR3_PC=0 )

#define GPIO1PIN_NP3C_INITOUT(v)     ( bFM3_GPIO_PDOR3_PC=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_PC=1, \
                                       bFM3_GPIO_PFR3_PC=0 )

/*---- GPIO bit P3D ----*/

#define GPIO1PIN_P3D_GET             ( bFM3_GPIO_PDIR3_PD )

#define GPIO1PIN_P3D_PUT(v)          ( bFM3_GPIO_PDOR3_PD=(v) )

#define GPIO1PIN_P3D_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P3D_INITIN(v) \
                                                        : GPIO1PIN_P3D_INITOUT(v) )

#define GPIO1PIN_P3D_INITIN(v)       ( bFM3_GPIO_PCR3_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PD=0, \
                                       bFM3_GPIO_PFR3_PD=0 )

#define GPIO1PIN_P3D_INITOUT(v)      ( bFM3_GPIO_PDOR3_PD=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_PD=1, \
                                       bFM3_GPIO_PFR3_PD=0 )


/*---- GPIO bit NP3D ----*/

#define GPIO1PIN_NP3D_GET            ( !(bFM3_GPIO_PDIR3_PD) )

#define GPIO1PIN_NP3D_PUT(v)         ( bFM3_GPIO_PDOR3_PD=!(v) )

#define GPIO1PIN_NP3D_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP3D_INITIN(v) \
                                                        : GPIO1PIN_NP3D_INITOUT(v) )

#define GPIO1PIN_NP3D_INITIN(v)      ( bFM3_GPIO_PCR3_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PD=0, \
                                       bFM3_GPIO_PFR3_PD=0 )

#define GPIO1PIN_NP3D_INITOUT(v)     ( bFM3_GPIO_PDOR3_PD=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_PD=1, \
                                       bFM3_GPIO_PFR3_PD=0 )

/*---- GPIO bit P3E ----*/

#define GPIO1PIN_P3E_GET             ( bFM3_GPIO_PDIR3_PE )

#define GPIO1PIN_P3E_PUT(v)          ( bFM3_GPIO_PDOR3_PE=(v) )

#define GPIO1PIN_P3E_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P3E_INITIN(v) \
                                                        : GPIO1PIN_P3E_INITOUT(v) )

#define GPIO1PIN_P3E_INITIN(v)       ( bFM3_GPIO_PCR3_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PE=0, \
                                       bFM3_GPIO_PFR3_PE=0 )

#define GPIO1PIN_P3E_INITOUT(v)      ( bFM3_GPIO_PDOR3_PE=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_PE=1, \
                                       bFM3_GPIO_PFR3_PE=0 )


/*---- GPIO bit NP3E ----*/

#define GPIO1PIN_NP3E_GET            ( !(bFM3_GPIO_PDIR3_PE) )

#define GPIO1PIN_NP3E_PUT(v)         ( bFM3_GPIO_PDOR3_PE=!(v) )

#define GPIO1PIN_NP3E_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP3E_INITIN(v) \
                                                        : GPIO1PIN_NP3E_INITOUT(v) )

#define GPIO1PIN_NP3E_INITIN(v)      ( bFM3_GPIO_PCR3_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PE=0, \
                                       bFM3_GPIO_PFR3_PE=0 )

#define GPIO1PIN_NP3E_INITOUT(v)     ( bFM3_GPIO_PDOR3_PE=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_PE=1, \
                                       bFM3_GPIO_PFR3_PE=0 )

/*---- GPIO bit P3F ----*/

#define GPIO1PIN_P3F_GET             ( bFM3_GPIO_PDIR3_PF )

#define GPIO1PIN_P3F_PUT(v)          ( bFM3_GPIO_PDOR3_PF=(v) )

#define GPIO1PIN_P3F_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P3F_INITIN(v) \
                                                        : GPIO1PIN_P3F_INITOUT(v) )

#define GPIO1PIN_P3F_INITIN(v)       ( bFM3_GPIO_PCR3_PF=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PF=0, \
                                       bFM3_GPIO_PFR3_PF=0 )

#define GPIO1PIN_P3F_INITOUT(v)      ( bFM3_GPIO_PDOR3_PF=(v).bInitVal, \
                                       bFM3_GPIO_DDR3_PF=1, \
                                       bFM3_GPIO_PFR3_PF=0 )


/*---- GPIO bit NP3F ----*/

#define GPIO1PIN_NP3F_GET            ( !(bFM3_GPIO_PDIR3_PF) )

#define GPIO1PIN_NP3F_PUT(v)         ( bFM3_GPIO_PDOR3_PF=!(v) )

#define GPIO1PIN_NP3F_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP3F_INITIN(v) \
                                                        : GPIO1PIN_NP3F_INITOUT(v) )

#define GPIO1PIN_NP3F_INITIN(v)      ( bFM3_GPIO_PCR3_PF=(v).bPullup, \
                                       bFM3_GPIO_DDR3_PF=0, \
                                       bFM3_GPIO_PFR3_PF=0 )

#define GPIO1PIN_NP3F_INITOUT(v)     ( bFM3_GPIO_PDOR3_PF=!((v).bInitVal), \
                                       bFM3_GPIO_DDR3_PF=1, \
                                       bFM3_GPIO_PFR3_PF=0 )

/*---- GPIO bit P40 ----*/

#define GPIO1PIN_P40_GET             ( bFM3_GPIO_PDIR4_P0 )

#define GPIO1PIN_P40_PUT(v)          ( bFM3_GPIO_PDOR4_P0=(v) )

#define GPIO1PIN_P40_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P40_INITIN(v) \
                                                        : GPIO1PIN_P40_INITOUT(v) )

#define GPIO1PIN_P40_INITIN(v)       ( bFM3_GPIO_PCR4_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P0=0, \
                                       bFM3_GPIO_PFR4_P0=0 )

#define GPIO1PIN_P40_INITOUT(v)      ( bFM3_GPIO_PDOR4_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P0=1, \
                                       bFM3_GPIO_PFR4_P0=0 )


/*---- GPIO bit NP40 ----*/

#define GPIO1PIN_NP40_GET            ( !(bFM3_GPIO_PDIR4_P0) )

#define GPIO1PIN_NP40_PUT(v)         ( bFM3_GPIO_PDOR4_P0=!(v) )

#define GPIO1PIN_NP40_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP40_INITIN(v) \
                                                        : GPIO1PIN_NP40_INITOUT(v) )

#define GPIO1PIN_NP40_INITIN(v)      ( bFM3_GPIO_PCR4_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P0=0, \
                                       bFM3_GPIO_PFR4_P0=0 )

#define GPIO1PIN_NP40_INITOUT(v)     ( bFM3_GPIO_PDOR4_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P0=1, \
                                       bFM3_GPIO_PFR4_P0=0 )

/*---- GPIO bit P41 ----*/

#define GPIO1PIN_P41_GET             ( bFM3_GPIO_PDIR4_P1 )

#define GPIO1PIN_P41_PUT(v)          ( bFM3_GPIO_PDOR4_P1=(v) )

#define GPIO1PIN_P41_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P41_INITIN(v) \
                                                        : GPIO1PIN_P41_INITOUT(v) )

#define GPIO1PIN_P41_INITIN(v)       ( bFM3_GPIO_PCR4_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P1=0, \
                                       bFM3_GPIO_PFR4_P1=0 )

#define GPIO1PIN_P41_INITOUT(v)      ( bFM3_GPIO_PDOR4_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P1=1, \
                                       bFM3_GPIO_PFR4_P1=0 )


/*---- GPIO bit NP41 ----*/

#define GPIO1PIN_NP41_GET            ( !(bFM3_GPIO_PDIR4_P1) )

#define GPIO1PIN_NP41_PUT(v)         ( bFM3_GPIO_PDOR4_P1=!(v) )

#define GPIO1PIN_NP41_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP41_INITIN(v) \
                                                        : GPIO1PIN_NP41_INITOUT(v) )

#define GPIO1PIN_NP41_INITIN(v)      ( bFM3_GPIO_PCR4_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P1=0, \
                                       bFM3_GPIO_PFR4_P1=0 )

#define GPIO1PIN_NP41_INITOUT(v)     ( bFM3_GPIO_PDOR4_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P1=1, \
                                       bFM3_GPIO_PFR4_P1=0 )

/*---- GPIO bit P42 ----*/

#define GPIO1PIN_P42_GET             ( bFM3_GPIO_PDIR4_P2 )

#define GPIO1PIN_P42_PUT(v)          ( bFM3_GPIO_PDOR4_P2=(v) )

#define GPIO1PIN_P42_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P42_INITIN(v) \
                                                        : GPIO1PIN_P42_INITOUT(v) )

#define GPIO1PIN_P42_INITIN(v)       ( bFM3_GPIO_PCR4_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P2=0, \
                                       bFM3_GPIO_PFR4_P2=0 )

#define GPIO1PIN_P42_INITOUT(v)      ( bFM3_GPIO_PDOR4_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P2=1, \
                                       bFM3_GPIO_PFR4_P2=0 )


/*---- GPIO bit NP42 ----*/

#define GPIO1PIN_NP42_GET            ( !(bFM3_GPIO_PDIR4_P2) )

#define GPIO1PIN_NP42_PUT(v)         ( bFM3_GPIO_PDOR4_P2=!(v) )

#define GPIO1PIN_NP42_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP42_INITIN(v) \
                                                        : GPIO1PIN_NP42_INITOUT(v) )

#define GPIO1PIN_NP42_INITIN(v)      ( bFM3_GPIO_PCR4_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P2=0, \
                                       bFM3_GPIO_PFR4_P2=0 )

#define GPIO1PIN_NP42_INITOUT(v)     ( bFM3_GPIO_PDOR4_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P2=1, \
                                       bFM3_GPIO_PFR4_P2=0 )

/*---- GPIO bit P43 ----*/

#define GPIO1PIN_P43_GET             ( bFM3_GPIO_PDIR4_P3 )

#define GPIO1PIN_P43_PUT(v)          ( bFM3_GPIO_PDOR4_P3=(v) )

#define GPIO1PIN_P43_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P43_INITIN(v) \
                                                        : GPIO1PIN_P43_INITOUT(v) )

#define GPIO1PIN_P43_INITIN(v)       ( bFM3_GPIO_PCR4_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P3=0, \
                                       bFM3_GPIO_PFR4_P3=0 )

#define GPIO1PIN_P43_INITOUT(v)      ( bFM3_GPIO_PDOR4_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P3=1, \
                                       bFM3_GPIO_PFR4_P3=0 )


/*---- GPIO bit NP43 ----*/

#define GPIO1PIN_NP43_GET            ( !(bFM3_GPIO_PDIR4_P3) )

#define GPIO1PIN_NP43_PUT(v)         ( bFM3_GPIO_PDOR4_P3=!(v) )

#define GPIO1PIN_NP43_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP43_INITIN(v) \
                                                        : GPIO1PIN_NP43_INITOUT(v) )

#define GPIO1PIN_NP43_INITIN(v)      ( bFM3_GPIO_PCR4_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P3=0, \
                                       bFM3_GPIO_PFR4_P3=0 )

#define GPIO1PIN_NP43_INITOUT(v)     ( bFM3_GPIO_PDOR4_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P3=1, \
                                       bFM3_GPIO_PFR4_P3=0 )

/*---- GPIO bit P44 ----*/

#define GPIO1PIN_P44_GET             ( bFM3_GPIO_PDIR4_P4 )

#define GPIO1PIN_P44_PUT(v)          ( bFM3_GPIO_PDOR4_P4=(v) )

#define GPIO1PIN_P44_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P44_INITIN(v) \
                                                        : GPIO1PIN_P44_INITOUT(v) )

#define GPIO1PIN_P44_INITIN(v)       ( bFM3_GPIO_PCR4_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P4=0, \
                                       bFM3_GPIO_PFR4_P4=0 )

#define GPIO1PIN_P44_INITOUT(v)      ( bFM3_GPIO_PDOR4_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P4=1, \
                                       bFM3_GPIO_PFR4_P4=0 )


/*---- GPIO bit NP44 ----*/

#define GPIO1PIN_NP44_GET            ( !(bFM3_GPIO_PDIR4_P4) )

#define GPIO1PIN_NP44_PUT(v)         ( bFM3_GPIO_PDOR4_P4=!(v) )

#define GPIO1PIN_NP44_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP44_INITIN(v) \
                                                        : GPIO1PIN_NP44_INITOUT(v) )

#define GPIO1PIN_NP44_INITIN(v)      ( bFM3_GPIO_PCR4_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P4=0, \
                                       bFM3_GPIO_PFR4_P4=0 )

#define GPIO1PIN_NP44_INITOUT(v)     ( bFM3_GPIO_PDOR4_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P4=1, \
                                       bFM3_GPIO_PFR4_P4=0 )

/*---- GPIO bit P45 ----*/

#define GPIO1PIN_P45_GET             ( bFM3_GPIO_PDIR4_P5 )

#define GPIO1PIN_P45_PUT(v)          ( bFM3_GPIO_PDOR4_P5=(v) )

#define GPIO1PIN_P45_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P45_INITIN(v) \
                                                        : GPIO1PIN_P45_INITOUT(v) )

#define GPIO1PIN_P45_INITIN(v)       ( bFM3_GPIO_PCR4_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P5=0, \
                                       bFM3_GPIO_PFR4_P5=0 )

#define GPIO1PIN_P45_INITOUT(v)      ( bFM3_GPIO_PDOR4_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P5=1, \
                                       bFM3_GPIO_PFR4_P5=0 )


/*---- GPIO bit NP45 ----*/

#define GPIO1PIN_NP45_GET            ( !(bFM3_GPIO_PDIR4_P5) )

#define GPIO1PIN_NP45_PUT(v)         ( bFM3_GPIO_PDOR4_P5=!(v) )

#define GPIO1PIN_NP45_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP45_INITIN(v) \
                                                        : GPIO1PIN_NP45_INITOUT(v) )

#define GPIO1PIN_NP45_INITIN(v)      ( bFM3_GPIO_PCR4_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P5=0, \
                                       bFM3_GPIO_PFR4_P5=0 )

#define GPIO1PIN_NP45_INITOUT(v)     ( bFM3_GPIO_PDOR4_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P5=1, \
                                       bFM3_GPIO_PFR4_P5=0 )

/*---- GPIO bit P46 ----*/

#define GPIO1PIN_P46_GET             ( bFM3_GPIO_PDIR4_P6 )

#define GPIO1PIN_P46_PUT(v)          ( bFM3_GPIO_PDOR4_P6=(v) )

#define GPIO1PIN_P46_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P46_INITIN(v) \
                                                        : GPIO1PIN_P46_INITOUT(v) )

#define GPIO1PIN_P46_INITIN(v)       ( bFM3_GPIO_PCR4_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P6=0, \
                                       bFM3_GPIO_PFR4_P6=0, \
                                       bFM3_SPSR_SUBXC = 0 )

#define GPIO1PIN_P46_INITOUT(v)      ( bFM3_GPIO_PDOR4_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P6=1, \
                                       bFM3_GPIO_PFR4_P6=0, \
                                       bFM3_SPSR_SUBXC = 0 )


/*---- GPIO bit NP46 ----*/

#define GPIO1PIN_NP46_GET            ( !(bFM3_GPIO_PDIR4_P6) )

#define GPIO1PIN_NP46_PUT(v)         ( bFM3_GPIO_PDOR4_P6=!(v) )

#define GPIO1PIN_NP46_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP46_INITIN(v) \
                                                        : GPIO1PIN_NP46_INITOUT(v) )

#define GPIO1PIN_NP46_INITIN(v)      ( bFM3_GPIO_PCR4_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P6=0, \
                                       bFM3_GPIO_PFR4_P6=0, \
                                       bFM3_SPSR_SUBXC = 0 )

#define GPIO1PIN_NP46_INITOUT(v)     ( bFM3_GPIO_PDOR4_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P6=1, \
                                       bFM3_GPIO_PFR4_P6=0, \
                                       bFM3_SPSR_SUBXC = 0 )

/*---- GPIO bit P47 ----*/

#define GPIO1PIN_P47_GET             ( bFM3_GPIO_PDIR4_P7 )

#define GPIO1PIN_P47_PUT(v)          ( bFM3_GPIO_PDOR4_P7=(v) )

#define GPIO1PIN_P47_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P47_INITIN(v) \
                                                        : GPIO1PIN_P47_INITOUT(v) )

#define GPIO1PIN_P47_INITIN(v)       ( bFM3_GPIO_PCR4_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P7=0, \
                                       bFM3_GPIO_PFR4_P7=0, \
                                       bFM3_SPSR_SUBXC = 0 )

#define GPIO1PIN_P47_INITOUT(v)      ( bFM3_GPIO_PDOR4_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P7=1, \
                                       bFM3_GPIO_PFR4_P7=0, \
                                       bFM3_SPSR_SUBXC = 0 )


/*---- GPIO bit NP47 ----*/

#define GPIO1PIN_NP47_GET            ( !(bFM3_GPIO_PDIR4_P7) )

#define GPIO1PIN_NP47_PUT(v)         ( bFM3_GPIO_PDOR4_P7=!(v) )

#define GPIO1PIN_NP47_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP47_INITIN(v) \
                                                        : GPIO1PIN_NP47_INITOUT(v) )

#define GPIO1PIN_NP47_INITIN(v)      ( bFM3_GPIO_PCR4_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P7=0, \
                                       bFM3_GPIO_PFR4_P7=0, \
                                       bFM3_SPSR_SUBXC = 0 )

#define GPIO1PIN_NP47_INITOUT(v)     ( bFM3_GPIO_PDOR4_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P7=1, \
                                       bFM3_GPIO_PFR4_P7=0, \
                                       bFM3_SPSR_SUBXC = 0 )

/*---- GPIO bit P48 ----*/

#define GPIO1PIN_P48_GET             ( bFM3_GPIO_PDIR4_P8 )

#define GPIO1PIN_P48_PUT(v)          ( bFM3_GPIO_PDOR4_P8=(v) )

#define GPIO1PIN_P48_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P48_INITIN(v) \
                                                        : GPIO1PIN_P48_INITOUT(v) )

#define GPIO1PIN_P48_INITIN(v)       ( bFM3_GPIO_PCR4_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P8=0, \
                                       bFM3_GPIO_PFR4_P8=0 )

#define GPIO1PIN_P48_INITOUT(v)      ( bFM3_GPIO_PDOR4_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P8=1, \
                                       bFM3_GPIO_PFR4_P8=0 )


/*---- GPIO bit NP48 ----*/

#define GPIO1PIN_NP48_GET            ( !(bFM3_GPIO_PDIR4_P8) )

#define GPIO1PIN_NP48_PUT(v)         ( bFM3_GPIO_PDOR4_P8=!(v) )

#define GPIO1PIN_NP48_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP48_INITIN(v) \
                                                        : GPIO1PIN_NP48_INITOUT(v) )

#define GPIO1PIN_NP48_INITIN(v)      ( bFM3_GPIO_PCR4_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P8=0, \
                                       bFM3_GPIO_PFR4_P8=0 )

#define GPIO1PIN_NP48_INITOUT(v)     ( bFM3_GPIO_PDOR4_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P8=1, \
                                       bFM3_GPIO_PFR4_P8=0 )

/*---- GPIO bit P49 ----*/

#define GPIO1PIN_P49_GET             ( bFM3_GPIO_PDIR4_P9 )

#define GPIO1PIN_P49_PUT(v)          ( bFM3_GPIO_PDOR4_P9=(v) )

#define GPIO1PIN_P49_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P49_INITIN(v) \
                                                        : GPIO1PIN_P49_INITOUT(v) )

#define GPIO1PIN_P49_INITIN(v)       ( bFM3_GPIO_PCR4_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P9=0, \
                                       bFM3_GPIO_PFR4_P9=0 )

#define GPIO1PIN_P49_INITOUT(v)      ( bFM3_GPIO_PDOR4_P9=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_P9=1, \
                                       bFM3_GPIO_PFR4_P9=0 )


/*---- GPIO bit NP49 ----*/

#define GPIO1PIN_NP49_GET            ( !(bFM3_GPIO_PDIR4_P9) )

#define GPIO1PIN_NP49_PUT(v)         ( bFM3_GPIO_PDOR4_P9=!(v) )

#define GPIO1PIN_NP49_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP49_INITIN(v) \
                                                        : GPIO1PIN_NP49_INITOUT(v) )

#define GPIO1PIN_NP49_INITIN(v)      ( bFM3_GPIO_PCR4_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR4_P9=0, \
                                       bFM3_GPIO_PFR4_P9=0 )

#define GPIO1PIN_NP49_INITOUT(v)     ( bFM3_GPIO_PDOR4_P9=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_P9=1, \
                                       bFM3_GPIO_PFR4_P9=0 )

/*---- GPIO bit P4A ----*/

#define GPIO1PIN_P4A_GET             ( bFM3_GPIO_PDIR4_PA )

#define GPIO1PIN_P4A_PUT(v)          ( bFM3_GPIO_PDOR4_PA=(v) )

#define GPIO1PIN_P4A_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P4A_INITIN(v) \
                                                        : GPIO1PIN_P4A_INITOUT(v) )

#define GPIO1PIN_P4A_INITIN(v)       ( bFM3_GPIO_PCR4_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PA=0, \
                                       bFM3_GPIO_PFR4_PA=0 )

#define GPIO1PIN_P4A_INITOUT(v)      ( bFM3_GPIO_PDOR4_PA=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_PA=1, \
                                       bFM3_GPIO_PFR4_PA=0 )


/*---- GPIO bit NP4A ----*/

#define GPIO1PIN_NP4A_GET            ( !(bFM3_GPIO_PDIR4_PA) )

#define GPIO1PIN_NP4A_PUT(v)         ( bFM3_GPIO_PDOR4_PA=!(v) )

#define GPIO1PIN_NP4A_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP4A_INITIN(v) \
                                                        : GPIO1PIN_NP4A_INITOUT(v) )

#define GPIO1PIN_NP4A_INITIN(v)      ( bFM3_GPIO_PCR4_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PA=0, \
                                       bFM3_GPIO_PFR4_PA=0 )

#define GPIO1PIN_NP4A_INITOUT(v)     ( bFM3_GPIO_PDOR4_PA=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_PA=1, \
                                       bFM3_GPIO_PFR4_PA=0 )

/*---- GPIO bit P4B ----*/

#define GPIO1PIN_P4B_GET             ( bFM3_GPIO_PDIR4_PB )

#define GPIO1PIN_P4B_PUT(v)          ( bFM3_GPIO_PDOR4_PB=(v) )

#define GPIO1PIN_P4B_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P4B_INITIN(v) \
                                                        : GPIO1PIN_P4B_INITOUT(v) )

#define GPIO1PIN_P4B_INITIN(v)       ( bFM3_GPIO_PCR4_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PB=0, \
                                       bFM3_GPIO_PFR4_PB=0 )

#define GPIO1PIN_P4B_INITOUT(v)      ( bFM3_GPIO_PDOR4_PB=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_PB=1, \
                                       bFM3_GPIO_PFR4_PB=0 )


/*---- GPIO bit NP4B ----*/

#define GPIO1PIN_NP4B_GET            ( !(bFM3_GPIO_PDIR4_PB) )

#define GPIO1PIN_NP4B_PUT(v)         ( bFM3_GPIO_PDOR4_PB=!(v) )

#define GPIO1PIN_NP4B_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP4B_INITIN(v) \
                                                        : GPIO1PIN_NP4B_INITOUT(v) )

#define GPIO1PIN_NP4B_INITIN(v)      ( bFM3_GPIO_PCR4_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PB=0, \
                                       bFM3_GPIO_PFR4_PB=0 )

#define GPIO1PIN_NP4B_INITOUT(v)     ( bFM3_GPIO_PDOR4_PB=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_PB=1, \
                                       bFM3_GPIO_PFR4_PB=0 )

/*---- GPIO bit P4C ----*/

#define GPIO1PIN_P4C_GET             ( bFM3_GPIO_PDIR4_PC )

#define GPIO1PIN_P4C_PUT(v)          ( bFM3_GPIO_PDOR4_PC=(v) )

#define GPIO1PIN_P4C_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P4C_INITIN(v) \
                                                        : GPIO1PIN_P4C_INITOUT(v) )

#define GPIO1PIN_P4C_INITIN(v)       ( bFM3_GPIO_PCR4_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PC=0, \
                                       bFM3_GPIO_PFR4_PC=0 )

#define GPIO1PIN_P4C_INITOUT(v)      ( bFM3_GPIO_PDOR4_PC=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_PC=1, \
                                       bFM3_GPIO_PFR4_PC=0 )


/*---- GPIO bit NP4C ----*/

#define GPIO1PIN_NP4C_GET            ( !(bFM3_GPIO_PDIR4_PC) )

#define GPIO1PIN_NP4C_PUT(v)         ( bFM3_GPIO_PDOR4_PC=!(v) )

#define GPIO1PIN_NP4C_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP4C_INITIN(v) \
                                                        : GPIO1PIN_NP4C_INITOUT(v) )

#define GPIO1PIN_NP4C_INITIN(v)      ( bFM3_GPIO_PCR4_PC=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PC=0, \
                                       bFM3_GPIO_PFR4_PC=0 )

#define GPIO1PIN_NP4C_INITOUT(v)     ( bFM3_GPIO_PDOR4_PC=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_PC=1, \
                                       bFM3_GPIO_PFR4_PC=0 )

/*---- GPIO bit P4D ----*/

#define GPIO1PIN_P4D_GET             ( bFM3_GPIO_PDIR4_PD )

#define GPIO1PIN_P4D_PUT(v)          ( bFM3_GPIO_PDOR4_PD=(v) )

#define GPIO1PIN_P4D_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P4D_INITIN(v) \
                                                        : GPIO1PIN_P4D_INITOUT(v) )

#define GPIO1PIN_P4D_INITIN(v)       ( bFM3_GPIO_PCR4_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PD=0, \
                                       bFM3_GPIO_PFR4_PD=0 )

#define GPIO1PIN_P4D_INITOUT(v)      ( bFM3_GPIO_PDOR4_PD=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_PD=1, \
                                       bFM3_GPIO_PFR4_PD=0 )


/*---- GPIO bit NP4D ----*/

#define GPIO1PIN_NP4D_GET            ( !(bFM3_GPIO_PDIR4_PD) )

#define GPIO1PIN_NP4D_PUT(v)         ( bFM3_GPIO_PDOR4_PD=!(v) )

#define GPIO1PIN_NP4D_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP4D_INITIN(v) \
                                                        : GPIO1PIN_NP4D_INITOUT(v) )

#define GPIO1PIN_NP4D_INITIN(v)      ( bFM3_GPIO_PCR4_PD=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PD=0, \
                                       bFM3_GPIO_PFR4_PD=0 )

#define GPIO1PIN_NP4D_INITOUT(v)     ( bFM3_GPIO_PDOR4_PD=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_PD=1, \
                                       bFM3_GPIO_PFR4_PD=0 )

/*---- GPIO bit P4E ----*/

#define GPIO1PIN_P4E_GET             ( bFM3_GPIO_PDIR4_PE )

#define GPIO1PIN_P4E_PUT(v)          ( bFM3_GPIO_PDOR4_PE=(v) )

#define GPIO1PIN_P4E_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P4E_INITIN(v) \
                                                        : GPIO1PIN_P4E_INITOUT(v) )

#define GPIO1PIN_P4E_INITIN(v)       ( bFM3_GPIO_PCR4_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PE=0, \
                                       bFM3_GPIO_PFR4_PE=0 )

#define GPIO1PIN_P4E_INITOUT(v)      ( bFM3_GPIO_PDOR4_PE=(v).bInitVal, \
                                       bFM3_GPIO_DDR4_PE=1, \
                                       bFM3_GPIO_PFR4_PE=0 )


/*---- GPIO bit NP4E ----*/

#define GPIO1PIN_NP4E_GET            ( !(bFM3_GPIO_PDIR4_PE) )

#define GPIO1PIN_NP4E_PUT(v)         ( bFM3_GPIO_PDOR4_PE=!(v) )

#define GPIO1PIN_NP4E_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP4E_INITIN(v) \
                                                        : GPIO1PIN_NP4E_INITOUT(v) )

#define GPIO1PIN_NP4E_INITIN(v)      ( bFM3_GPIO_PCR4_PE=(v).bPullup, \
                                       bFM3_GPIO_DDR4_PE=0, \
                                       bFM3_GPIO_PFR4_PE=0 )

#define GPIO1PIN_NP4E_INITOUT(v)     ( bFM3_GPIO_PDOR4_PE=!((v).bInitVal), \
                                       bFM3_GPIO_DDR4_PE=1, \
                                       bFM3_GPIO_PFR4_PE=0 )

/*---- GPIO bit P50 ----*/

#define GPIO1PIN_P50_GET             ( bFM3_GPIO_PDIR5_P0 )

#define GPIO1PIN_P50_PUT(v)          ( bFM3_GPIO_PDOR5_P0=(v) )

#define GPIO1PIN_P50_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P50_INITIN(v) \
                                                        : GPIO1PIN_P50_INITOUT(v) )

#define GPIO1PIN_P50_INITIN(v)       ( bFM3_GPIO_PCR5_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P0=0, \
                                       bFM3_GPIO_PFR5_P0=0 )

#define GPIO1PIN_P50_INITOUT(v)      ( bFM3_GPIO_PDOR5_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P0=1, \
                                       bFM3_GPIO_PFR5_P0=0 )


/*---- GPIO bit NP50 ----*/

#define GPIO1PIN_NP50_GET            ( !(bFM3_GPIO_PDIR5_P0) )

#define GPIO1PIN_NP50_PUT(v)         ( bFM3_GPIO_PDOR5_P0=!(v) )

#define GPIO1PIN_NP50_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP50_INITIN(v) \
                                                        : GPIO1PIN_NP50_INITOUT(v) )

#define GPIO1PIN_NP50_INITIN(v)      ( bFM3_GPIO_PCR5_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P0=0, \
                                       bFM3_GPIO_PFR5_P0=0 )

#define GPIO1PIN_NP50_INITOUT(v)     ( bFM3_GPIO_PDOR5_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P0=1, \
                                       bFM3_GPIO_PFR5_P0=0 )

/*---- GPIO bit P51 ----*/

#define GPIO1PIN_P51_GET             ( bFM3_GPIO_PDIR5_P1 )

#define GPIO1PIN_P51_PUT(v)          ( bFM3_GPIO_PDOR5_P1=(v) )

#define GPIO1PIN_P51_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P51_INITIN(v) \
                                                        : GPIO1PIN_P51_INITOUT(v) )

#define GPIO1PIN_P51_INITIN(v)       ( bFM3_GPIO_PCR5_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P1=0, \
                                       bFM3_GPIO_PFR5_P1=0 )

#define GPIO1PIN_P51_INITOUT(v)      ( bFM3_GPIO_PDOR5_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P1=1, \
                                       bFM3_GPIO_PFR5_P1=0 )


/*---- GPIO bit NP51 ----*/

#define GPIO1PIN_NP51_GET            ( !(bFM3_GPIO_PDIR5_P1) )

#define GPIO1PIN_NP51_PUT(v)         ( bFM3_GPIO_PDOR5_P1=!(v) )

#define GPIO1PIN_NP51_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP51_INITIN(v) \
                                                        : GPIO1PIN_NP51_INITOUT(v) )

#define GPIO1PIN_NP51_INITIN(v)      ( bFM3_GPIO_PCR5_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P1=0, \
                                       bFM3_GPIO_PFR5_P1=0 )

#define GPIO1PIN_NP51_INITOUT(v)     ( bFM3_GPIO_PDOR5_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P1=1, \
                                       bFM3_GPIO_PFR5_P1=0 )

/*---- GPIO bit P52 ----*/

#define GPIO1PIN_P52_GET             ( bFM3_GPIO_PDIR5_P2 )

#define GPIO1PIN_P52_PUT(v)          ( bFM3_GPIO_PDOR5_P2=(v) )

#define GPIO1PIN_P52_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P52_INITIN(v) \
                                                        : GPIO1PIN_P52_INITOUT(v) )

#define GPIO1PIN_P52_INITIN(v)       ( bFM3_GPIO_PCR5_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P2=0, \
                                       bFM3_GPIO_PFR5_P2=0 )

#define GPIO1PIN_P52_INITOUT(v)      ( bFM3_GPIO_PDOR5_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P2=1, \
                                       bFM3_GPIO_PFR5_P2=0 )


/*---- GPIO bit NP52 ----*/

#define GPIO1PIN_NP52_GET            ( !(bFM3_GPIO_PDIR5_P2) )

#define GPIO1PIN_NP52_PUT(v)         ( bFM3_GPIO_PDOR5_P2=!(v) )

#define GPIO1PIN_NP52_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP52_INITIN(v) \
                                                        : GPIO1PIN_NP52_INITOUT(v) )

#define GPIO1PIN_NP52_INITIN(v)      ( bFM3_GPIO_PCR5_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P2=0, \
                                       bFM3_GPIO_PFR5_P2=0 )

#define GPIO1PIN_NP52_INITOUT(v)     ( bFM3_GPIO_PDOR5_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P2=1, \
                                       bFM3_GPIO_PFR5_P2=0 )

/*---- GPIO bit P53 ----*/

#define GPIO1PIN_P53_GET             ( bFM3_GPIO_PDIR5_P3 )

#define GPIO1PIN_P53_PUT(v)          ( bFM3_GPIO_PDOR5_P3=(v) )

#define GPIO1PIN_P53_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P53_INITIN(v) \
                                                        : GPIO1PIN_P53_INITOUT(v) )

#define GPIO1PIN_P53_INITIN(v)       ( bFM3_GPIO_PCR5_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P3=0, \
                                       bFM3_GPIO_PFR5_P3=0 )

#define GPIO1PIN_P53_INITOUT(v)      ( bFM3_GPIO_PDOR5_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P3=1, \
                                       bFM3_GPIO_PFR5_P3=0 )


/*---- GPIO bit NP53 ----*/

#define GPIO1PIN_NP53_GET            ( !(bFM3_GPIO_PDIR5_P3) )

#define GPIO1PIN_NP53_PUT(v)         ( bFM3_GPIO_PDOR5_P3=!(v) )

#define GPIO1PIN_NP53_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP53_INITIN(v) \
                                                        : GPIO1PIN_NP53_INITOUT(v) )

#define GPIO1PIN_NP53_INITIN(v)      ( bFM3_GPIO_PCR5_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P3=0, \
                                       bFM3_GPIO_PFR5_P3=0 )

#define GPIO1PIN_NP53_INITOUT(v)     ( bFM3_GPIO_PDOR5_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P3=1, \
                                       bFM3_GPIO_PFR5_P3=0 )

/*---- GPIO bit P54 ----*/

#define GPIO1PIN_P54_GET             ( bFM3_GPIO_PDIR5_P4 )

#define GPIO1PIN_P54_PUT(v)          ( bFM3_GPIO_PDOR5_P4=(v) )

#define GPIO1PIN_P54_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P54_INITIN(v) \
                                                        : GPIO1PIN_P54_INITOUT(v) )

#define GPIO1PIN_P54_INITIN(v)       ( bFM3_GPIO_PCR5_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P4=0, \
                                       bFM3_GPIO_PFR5_P4=0 )

#define GPIO1PIN_P54_INITOUT(v)      ( bFM3_GPIO_PDOR5_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P4=1, \
                                       bFM3_GPIO_PFR5_P4=0 )


/*---- GPIO bit NP54 ----*/

#define GPIO1PIN_NP54_GET            ( !(bFM3_GPIO_PDIR5_P4) )

#define GPIO1PIN_NP54_PUT(v)         ( bFM3_GPIO_PDOR5_P4=!(v) )

#define GPIO1PIN_NP54_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP54_INITIN(v) \
                                                        : GPIO1PIN_NP54_INITOUT(v) )

#define GPIO1PIN_NP54_INITIN(v)      ( bFM3_GPIO_PCR5_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P4=0, \
                                       bFM3_GPIO_PFR5_P4=0 )

#define GPIO1PIN_NP54_INITOUT(v)     ( bFM3_GPIO_PDOR5_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P4=1, \
                                       bFM3_GPIO_PFR5_P4=0 )

/*---- GPIO bit P55 ----*/

#define GPIO1PIN_P55_GET             ( bFM3_GPIO_PDIR5_P5 )

#define GPIO1PIN_P55_PUT(v)          ( bFM3_GPIO_PDOR5_P5=(v) )

#define GPIO1PIN_P55_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P55_INITIN(v) \
                                                        : GPIO1PIN_P55_INITOUT(v) )

#define GPIO1PIN_P55_INITIN(v)       ( bFM3_GPIO_PCR5_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P5=0, \
                                       bFM3_GPIO_PFR5_P5=0 )

#define GPIO1PIN_P55_INITOUT(v)      ( bFM3_GPIO_PDOR5_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P5=1, \
                                       bFM3_GPIO_PFR5_P5=0 )


/*---- GPIO bit NP55 ----*/

#define GPIO1PIN_NP55_GET            ( !(bFM3_GPIO_PDIR5_P5) )

#define GPIO1PIN_NP55_PUT(v)         ( bFM3_GPIO_PDOR5_P5=!(v) )

#define GPIO1PIN_NP55_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP55_INITIN(v) \
                                                        : GPIO1PIN_NP55_INITOUT(v) )

#define GPIO1PIN_NP55_INITIN(v)      ( bFM3_GPIO_PCR5_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P5=0, \
                                       bFM3_GPIO_PFR5_P5=0 )

#define GPIO1PIN_NP55_INITOUT(v)     ( bFM3_GPIO_PDOR5_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P5=1, \
                                       bFM3_GPIO_PFR5_P5=0 )

/*---- GPIO bit P56 ----*/

#define GPIO1PIN_P56_GET             ( bFM3_GPIO_PDIR5_P6 )

#define GPIO1PIN_P56_PUT(v)          ( bFM3_GPIO_PDOR5_P6=(v) )

#define GPIO1PIN_P56_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P56_INITIN(v) \
                                                        : GPIO1PIN_P56_INITOUT(v) )

#define GPIO1PIN_P56_INITIN(v)       ( bFM3_GPIO_PCR5_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P6=0, \
                                       bFM3_GPIO_PFR5_P6=0 )

#define GPIO1PIN_P56_INITOUT(v)      ( bFM3_GPIO_PDOR5_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P6=1, \
                                       bFM3_GPIO_PFR5_P6=0 )


/*---- GPIO bit NP56 ----*/

#define GPIO1PIN_NP56_GET            ( !(bFM3_GPIO_PDIR5_P6) )

#define GPIO1PIN_NP56_PUT(v)         ( bFM3_GPIO_PDOR5_P6=!(v) )

#define GPIO1PIN_NP56_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP56_INITIN(v) \
                                                        : GPIO1PIN_NP56_INITOUT(v) )

#define GPIO1PIN_NP56_INITIN(v)      ( bFM3_GPIO_PCR5_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P6=0, \
                                       bFM3_GPIO_PFR5_P6=0 )

#define GPIO1PIN_NP56_INITOUT(v)     ( bFM3_GPIO_PDOR5_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P6=1, \
                                       bFM3_GPIO_PFR5_P6=0 )

/*---- GPIO bit P57 ----*/

#define GPIO1PIN_P57_GET             ( bFM3_GPIO_PDIR5_P7 )

#define GPIO1PIN_P57_PUT(v)          ( bFM3_GPIO_PDOR5_P7=(v) )

#define GPIO1PIN_P57_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P57_INITIN(v) \
                                                        : GPIO1PIN_P57_INITOUT(v) )

#define GPIO1PIN_P57_INITIN(v)       ( bFM3_GPIO_PCR5_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P7=0, \
                                       bFM3_GPIO_PFR5_P7=0 )

#define GPIO1PIN_P57_INITOUT(v)      ( bFM3_GPIO_PDOR5_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P7=1, \
                                       bFM3_GPIO_PFR5_P7=0 )


/*---- GPIO bit NP57 ----*/

#define GPIO1PIN_NP57_GET            ( !(bFM3_GPIO_PDIR5_P7) )

#define GPIO1PIN_NP57_PUT(v)         ( bFM3_GPIO_PDOR5_P7=!(v) )

#define GPIO1PIN_NP57_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP57_INITIN(v) \
                                                        : GPIO1PIN_NP57_INITOUT(v) )

#define GPIO1PIN_NP57_INITIN(v)      ( bFM3_GPIO_PCR5_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P7=0, \
                                       bFM3_GPIO_PFR5_P7=0 )

#define GPIO1PIN_NP57_INITOUT(v)     ( bFM3_GPIO_PDOR5_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P7=1, \
                                       bFM3_GPIO_PFR5_P7=0 )

/*---- GPIO bit P58 ----*/

#define GPIO1PIN_P58_GET             ( bFM3_GPIO_PDIR5_P8 )

#define GPIO1PIN_P58_PUT(v)          ( bFM3_GPIO_PDOR5_P8=(v) )

#define GPIO1PIN_P58_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P58_INITIN(v) \
                                                        : GPIO1PIN_P58_INITOUT(v) )

#define GPIO1PIN_P58_INITIN(v)       ( bFM3_GPIO_PCR5_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P8=0, \
                                       bFM3_GPIO_PFR5_P8=0 )

#define GPIO1PIN_P58_INITOUT(v)      ( bFM3_GPIO_PDOR5_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P8=1, \
                                       bFM3_GPIO_PFR5_P8=0 )


/*---- GPIO bit NP58 ----*/

#define GPIO1PIN_NP58_GET            ( !(bFM3_GPIO_PDIR5_P8) )

#define GPIO1PIN_NP58_PUT(v)         ( bFM3_GPIO_PDOR5_P8=!(v) )

#define GPIO1PIN_NP58_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP58_INITIN(v) \
                                                        : GPIO1PIN_NP58_INITOUT(v) )

#define GPIO1PIN_NP58_INITIN(v)      ( bFM3_GPIO_PCR5_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P8=0, \
                                       bFM3_GPIO_PFR5_P8=0 )

#define GPIO1PIN_NP58_INITOUT(v)     ( bFM3_GPIO_PDOR5_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P8=1, \
                                       bFM3_GPIO_PFR5_P8=0 )

/*---- GPIO bit P59 ----*/

#define GPIO1PIN_P59_GET             ( bFM3_GPIO_PDIR5_P9 )

#define GPIO1PIN_P59_PUT(v)          ( bFM3_GPIO_PDOR5_P9=(v) )

#define GPIO1PIN_P59_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P59_INITIN(v) \
                                                        : GPIO1PIN_P59_INITOUT(v) )

#define GPIO1PIN_P59_INITIN(v)       ( bFM3_GPIO_PCR5_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P9=0, \
                                       bFM3_GPIO_PFR5_P9=0 )

#define GPIO1PIN_P59_INITOUT(v)      ( bFM3_GPIO_PDOR5_P9=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_P9=1, \
                                       bFM3_GPIO_PFR5_P9=0 )


/*---- GPIO bit NP59 ----*/

#define GPIO1PIN_NP59_GET            ( !(bFM3_GPIO_PDIR5_P9) )

#define GPIO1PIN_NP59_PUT(v)         ( bFM3_GPIO_PDOR5_P9=!(v) )

#define GPIO1PIN_NP59_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP59_INITIN(v) \
                                                        : GPIO1PIN_NP59_INITOUT(v) )

#define GPIO1PIN_NP59_INITIN(v)      ( bFM3_GPIO_PCR5_P9=(v).bPullup, \
                                       bFM3_GPIO_DDR5_P9=0, \
                                       bFM3_GPIO_PFR5_P9=0 )

#define GPIO1PIN_NP59_INITOUT(v)     ( bFM3_GPIO_PDOR5_P9=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_P9=1, \
                                       bFM3_GPIO_PFR5_P9=0 )

/*---- GPIO bit P5A ----*/

#define GPIO1PIN_P5A_GET             ( bFM3_GPIO_PDIR5_PA )

#define GPIO1PIN_P5A_PUT(v)          ( bFM3_GPIO_PDOR5_PA=(v) )

#define GPIO1PIN_P5A_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P5A_INITIN(v) \
                                                        : GPIO1PIN_P5A_INITOUT(v) )

#define GPIO1PIN_P5A_INITIN(v)       ( bFM3_GPIO_PCR5_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR5_PA=0, \
                                       bFM3_GPIO_PFR5_PA=0 )

#define GPIO1PIN_P5A_INITOUT(v)      ( bFM3_GPIO_PDOR5_PA=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_PA=1, \
                                       bFM3_GPIO_PFR5_PA=0 )


/*---- GPIO bit NP5A ----*/

#define GPIO1PIN_NP5A_GET            ( !(bFM3_GPIO_PDIR5_PA) )

#define GPIO1PIN_NP5A_PUT(v)         ( bFM3_GPIO_PDOR5_PA=!(v) )

#define GPIO1PIN_NP5A_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP5A_INITIN(v) \
                                                        : GPIO1PIN_NP5A_INITOUT(v) )

#define GPIO1PIN_NP5A_INITIN(v)      ( bFM3_GPIO_PCR5_PA=(v).bPullup, \
                                       bFM3_GPIO_DDR5_PA=0, \
                                       bFM3_GPIO_PFR5_PA=0 )

#define GPIO1PIN_NP5A_INITOUT(v)     ( bFM3_GPIO_PDOR5_PA=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_PA=1, \
                                       bFM3_GPIO_PFR5_PA=0 )

/*---- GPIO bit P5B ----*/

#define GPIO1PIN_P5B_GET             ( bFM3_GPIO_PDIR5_PB )

#define GPIO1PIN_P5B_PUT(v)          ( bFM3_GPIO_PDOR5_PB=(v) )

#define GPIO1PIN_P5B_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P5B_INITIN(v) \
                                                        : GPIO1PIN_P5B_INITOUT(v) )

#define GPIO1PIN_P5B_INITIN(v)       ( bFM3_GPIO_PCR5_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR5_PB=0, \
                                       bFM3_GPIO_PFR5_PB=0 )

#define GPIO1PIN_P5B_INITOUT(v)      ( bFM3_GPIO_PDOR5_PB=(v).bInitVal, \
                                       bFM3_GPIO_DDR5_PB=1, \
                                       bFM3_GPIO_PFR5_PB=0 )


/*---- GPIO bit NP5B ----*/

#define GPIO1PIN_NP5B_GET            ( !(bFM3_GPIO_PDIR5_PB) )

#define GPIO1PIN_NP5B_PUT(v)         ( bFM3_GPIO_PDOR5_PB=!(v) )

#define GPIO1PIN_NP5B_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP5B_INITIN(v) \
                                                        : GPIO1PIN_NP5B_INITOUT(v) )

#define GPIO1PIN_NP5B_INITIN(v)      ( bFM3_GPIO_PCR5_PB=(v).bPullup, \
                                       bFM3_GPIO_DDR5_PB=0, \
                                       bFM3_GPIO_PFR5_PB=0 )

#define GPIO1PIN_NP5B_INITOUT(v)     ( bFM3_GPIO_PDOR5_PB=!((v).bInitVal), \
                                       bFM3_GPIO_DDR5_PB=1, \
                                       bFM3_GPIO_PFR5_PB=0 )

/*---- GPIO bit P60 ----*/

#define GPIO1PIN_P60_GET             ( bFM3_GPIO_PDIR6_P0 )

#define GPIO1PIN_P60_PUT(v)          ( bFM3_GPIO_PDOR6_P0=(v) )

#define GPIO1PIN_P60_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P60_INITIN(v) \
                                                        : GPIO1PIN_P60_INITOUT(v) )

#define GPIO1PIN_P60_INITIN(v)       ( bFM3_GPIO_PCR6_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P0=0, \
                                       bFM3_GPIO_PFR6_P0=0 )

#define GPIO1PIN_P60_INITOUT(v)      ( bFM3_GPIO_PDOR6_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P0=1, \
                                       bFM3_GPIO_PFR6_P0=0 )


/*---- GPIO bit NP60 ----*/

#define GPIO1PIN_NP60_GET            ( !(bFM3_GPIO_PDIR6_P0) )

#define GPIO1PIN_NP60_PUT(v)         ( bFM3_GPIO_PDOR6_P0=!(v) )

#define GPIO1PIN_NP60_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP60_INITIN(v) \
                                                        : GPIO1PIN_NP60_INITOUT(v) )

#define GPIO1PIN_NP60_INITIN(v)      ( bFM3_GPIO_PCR6_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P0=0, \
                                       bFM3_GPIO_PFR6_P0=0 )

#define GPIO1PIN_NP60_INITOUT(v)     ( bFM3_GPIO_PDOR6_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P0=1, \
                                       bFM3_GPIO_PFR6_P0=0 )

/*---- GPIO bit P61 ----*/

#define GPIO1PIN_P61_GET             ( bFM3_GPIO_PDIR6_P1 )

#define GPIO1PIN_P61_PUT(v)          ( bFM3_GPIO_PDOR6_P1=(v) )

#define GPIO1PIN_P61_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P61_INITIN(v) \
                                                        : GPIO1PIN_P61_INITOUT(v) )

#define GPIO1PIN_P61_INITIN(v)       ( bFM3_GPIO_PCR6_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P1=0, \
                                       bFM3_GPIO_PFR6_P1=0 )

#define GPIO1PIN_P61_INITOUT(v)      ( bFM3_GPIO_PDOR6_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P1=1, \
                                       bFM3_GPIO_PFR6_P1=0 )


/*---- GPIO bit NP61 ----*/

#define GPIO1PIN_NP61_GET            ( !(bFM3_GPIO_PDIR6_P1) )

#define GPIO1PIN_NP61_PUT(v)         ( bFM3_GPIO_PDOR6_P1=!(v) )

#define GPIO1PIN_NP61_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP61_INITIN(v) \
                                                        : GPIO1PIN_NP61_INITOUT(v) )

#define GPIO1PIN_NP61_INITIN(v)      ( bFM3_GPIO_PCR6_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P1=0, \
                                       bFM3_GPIO_PFR6_P1=0 )

#define GPIO1PIN_NP61_INITOUT(v)     ( bFM3_GPIO_PDOR6_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P1=1, \
                                       bFM3_GPIO_PFR6_P1=0 )

/*---- GPIO bit P62 ----*/

#define GPIO1PIN_P62_GET             ( bFM3_GPIO_PDIR6_P2 )

#define GPIO1PIN_P62_PUT(v)          ( bFM3_GPIO_PDOR6_P2=(v) )

#define GPIO1PIN_P62_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P62_INITIN(v) \
                                                        : GPIO1PIN_P62_INITOUT(v) )

#define GPIO1PIN_P62_INITIN(v)       ( bFM3_GPIO_PCR6_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P2=0, \
                                       bFM3_GPIO_PFR6_P2=0 )

#define GPIO1PIN_P62_INITOUT(v)      ( bFM3_GPIO_PDOR6_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P2=1, \
                                       bFM3_GPIO_PFR6_P2=0 )


/*---- GPIO bit NP62 ----*/

#define GPIO1PIN_NP62_GET            ( !(bFM3_GPIO_PDIR6_P2) )

#define GPIO1PIN_NP62_PUT(v)         ( bFM3_GPIO_PDOR6_P2=!(v) )

#define GPIO1PIN_NP62_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP62_INITIN(v) \
                                                        : GPIO1PIN_NP62_INITOUT(v) )

#define GPIO1PIN_NP62_INITIN(v)      ( bFM3_GPIO_PCR6_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P2=0, \
                                       bFM3_GPIO_PFR6_P2=0 )

#define GPIO1PIN_NP62_INITOUT(v)     ( bFM3_GPIO_PDOR6_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P2=1, \
                                       bFM3_GPIO_PFR6_P2=0 )

/*---- GPIO bit P63 ----*/

#define GPIO1PIN_P63_GET             ( bFM3_GPIO_PDIR6_P3 )

#define GPIO1PIN_P63_PUT(v)          ( bFM3_GPIO_PDOR6_P3=(v) )

#define GPIO1PIN_P63_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P63_INITIN(v) \
                                                        : GPIO1PIN_P63_INITOUT(v) )

#define GPIO1PIN_P63_INITIN(v)       ( bFM3_GPIO_PCR6_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P3=0, \
                                       bFM3_GPIO_PFR6_P3=0 )

#define GPIO1PIN_P63_INITOUT(v)      ( bFM3_GPIO_PDOR6_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P3=1, \
                                       bFM3_GPIO_PFR6_P3=0 )


/*---- GPIO bit NP63 ----*/

#define GPIO1PIN_NP63_GET            ( !(bFM3_GPIO_PDIR6_P3) )

#define GPIO1PIN_NP63_PUT(v)         ( bFM3_GPIO_PDOR6_P3=!(v) )

#define GPIO1PIN_NP63_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP63_INITIN(v) \
                                                        : GPIO1PIN_NP63_INITOUT(v) )

#define GPIO1PIN_NP63_INITIN(v)      ( bFM3_GPIO_PCR6_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P3=0, \
                                       bFM3_GPIO_PFR6_P3=0 )

#define GPIO1PIN_NP63_INITOUT(v)     ( bFM3_GPIO_PDOR6_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P3=1, \
                                       bFM3_GPIO_PFR6_P3=0 )

/*---- GPIO bit P64 ----*/

#define GPIO1PIN_P64_GET             ( bFM3_GPIO_PDIR6_P4 )

#define GPIO1PIN_P64_PUT(v)          ( bFM3_GPIO_PDOR6_P4=(v) )

#define GPIO1PIN_P64_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P64_INITIN(v) \
                                                        : GPIO1PIN_P64_INITOUT(v) )

#define GPIO1PIN_P64_INITIN(v)       ( bFM3_GPIO_PCR6_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P4=0, \
                                       bFM3_GPIO_PFR6_P4=0 )

#define GPIO1PIN_P64_INITOUT(v)      ( bFM3_GPIO_PDOR6_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P4=1, \
                                       bFM3_GPIO_PFR6_P4=0 )


/*---- GPIO bit NP64 ----*/

#define GPIO1PIN_NP64_GET            ( !(bFM3_GPIO_PDIR6_P4) )

#define GPIO1PIN_NP64_PUT(v)         ( bFM3_GPIO_PDOR6_P4=!(v) )

#define GPIO1PIN_NP64_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP64_INITIN(v) \
                                                        : GPIO1PIN_NP64_INITOUT(v) )

#define GPIO1PIN_NP64_INITIN(v)      ( bFM3_GPIO_PCR6_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P4=0, \
                                       bFM3_GPIO_PFR6_P4=0 )

#define GPIO1PIN_NP64_INITOUT(v)     ( bFM3_GPIO_PDOR6_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P4=1, \
                                       bFM3_GPIO_PFR6_P4=0 )

/*---- GPIO bit P65 ----*/

#define GPIO1PIN_P65_GET             ( bFM3_GPIO_PDIR6_P5 )

#define GPIO1PIN_P65_PUT(v)          ( bFM3_GPIO_PDOR6_P5=(v) )

#define GPIO1PIN_P65_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P65_INITIN(v) \
                                                        : GPIO1PIN_P65_INITOUT(v) )

#define GPIO1PIN_P65_INITIN(v)       ( bFM3_GPIO_PCR6_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P5=0, \
                                       bFM3_GPIO_PFR6_P5=0 )

#define GPIO1PIN_P65_INITOUT(v)      ( bFM3_GPIO_PDOR6_P5=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P5=1, \
                                       bFM3_GPIO_PFR6_P5=0 )


/*---- GPIO bit NP65 ----*/

#define GPIO1PIN_NP65_GET            ( !(bFM3_GPIO_PDIR6_P5) )

#define GPIO1PIN_NP65_PUT(v)         ( bFM3_GPIO_PDOR6_P5=!(v) )

#define GPIO1PIN_NP65_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP65_INITIN(v) \
                                                        : GPIO1PIN_NP65_INITOUT(v) )

#define GPIO1PIN_NP65_INITIN(v)      ( bFM3_GPIO_PCR6_P5=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P5=0, \
                                       bFM3_GPIO_PFR6_P5=0 )

#define GPIO1PIN_NP65_INITOUT(v)     ( bFM3_GPIO_PDOR6_P5=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P5=1, \
                                       bFM3_GPIO_PFR6_P5=0 )

/*---- GPIO bit P66 ----*/

#define GPIO1PIN_P66_GET             ( bFM3_GPIO_PDIR6_P6 )

#define GPIO1PIN_P66_PUT(v)          ( bFM3_GPIO_PDOR6_P6=(v) )

#define GPIO1PIN_P66_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P66_INITIN(v) \
                                                        : GPIO1PIN_P66_INITOUT(v) )

#define GPIO1PIN_P66_INITIN(v)       ( bFM3_GPIO_PCR6_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P6=0, \
                                       bFM3_GPIO_PFR6_P6=0 )

#define GPIO1PIN_P66_INITOUT(v)      ( bFM3_GPIO_PDOR6_P6=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P6=1, \
                                       bFM3_GPIO_PFR6_P6=0 )


/*---- GPIO bit NP66 ----*/

#define GPIO1PIN_NP66_GET            ( !(bFM3_GPIO_PDIR6_P6) )

#define GPIO1PIN_NP66_PUT(v)         ( bFM3_GPIO_PDOR6_P6=!(v) )

#define GPIO1PIN_NP66_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP66_INITIN(v) \
                                                        : GPIO1PIN_NP66_INITOUT(v) )

#define GPIO1PIN_NP66_INITIN(v)      ( bFM3_GPIO_PCR6_P6=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P6=0, \
                                       bFM3_GPIO_PFR6_P6=0 )

#define GPIO1PIN_NP66_INITOUT(v)     ( bFM3_GPIO_PDOR6_P6=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P6=1, \
                                       bFM3_GPIO_PFR6_P6=0 )

/*---- GPIO bit P67 ----*/

#define GPIO1PIN_P67_GET             ( bFM3_GPIO_PDIR6_P7 )

#define GPIO1PIN_P67_PUT(v)          ( bFM3_GPIO_PDOR6_P7=(v) )

#define GPIO1PIN_P67_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P67_INITIN(v) \
                                                        : GPIO1PIN_P67_INITOUT(v) )

#define GPIO1PIN_P67_INITIN(v)       ( bFM3_GPIO_PCR6_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P7=0, \
                                       bFM3_GPIO_PFR6_P7=0 )

#define GPIO1PIN_P67_INITOUT(v)      ( bFM3_GPIO_PDOR6_P7=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P7=1, \
                                       bFM3_GPIO_PFR6_P7=0 )


/*---- GPIO bit NP67 ----*/

#define GPIO1PIN_NP67_GET            ( !(bFM3_GPIO_PDIR6_P7) )

#define GPIO1PIN_NP67_PUT(v)         ( bFM3_GPIO_PDOR6_P7=!(v) )

#define GPIO1PIN_NP67_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP67_INITIN(v) \
                                                        : GPIO1PIN_NP67_INITOUT(v) )

#define GPIO1PIN_NP67_INITIN(v)      ( bFM3_GPIO_PCR6_P7=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P7=0, \
                                       bFM3_GPIO_PFR6_P7=0 )

#define GPIO1PIN_NP67_INITOUT(v)     ( bFM3_GPIO_PDOR6_P7=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P7=1, \
                                       bFM3_GPIO_PFR6_P7=0 )

/*---- GPIO bit P68 ----*/

#define GPIO1PIN_P68_GET             ( bFM3_GPIO_PDIR6_P8 )

#define GPIO1PIN_P68_PUT(v)          ( bFM3_GPIO_PDOR6_P8=(v) )

#define GPIO1PIN_P68_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P68_INITIN(v) \
                                                        : GPIO1PIN_P68_INITOUT(v) )

#define GPIO1PIN_P68_INITIN(v)       ( bFM3_GPIO_PCR6_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P8=0, \
                                       bFM3_GPIO_PFR6_P8=0 )

#define GPIO1PIN_P68_INITOUT(v)      ( bFM3_GPIO_PDOR6_P8=(v).bInitVal, \
                                       bFM3_GPIO_DDR6_P8=1, \
                                       bFM3_GPIO_PFR6_P8=0 )


/*---- GPIO bit NP68 ----*/

#define GPIO1PIN_NP68_GET            ( !(bFM3_GPIO_PDIR6_P8) )

#define GPIO1PIN_NP68_PUT(v)         ( bFM3_GPIO_PDOR6_P8=!(v) )

#define GPIO1PIN_NP68_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP68_INITIN(v) \
                                                        : GPIO1PIN_NP68_INITOUT(v) )

#define GPIO1PIN_NP68_INITIN(v)      ( bFM3_GPIO_PCR6_P8=(v).bPullup, \
                                       bFM3_GPIO_DDR6_P8=0, \
                                       bFM3_GPIO_PFR6_P8=0 )

#define GPIO1PIN_NP68_INITOUT(v)     ( bFM3_GPIO_PDOR6_P8=!((v).bInitVal), \
                                       bFM3_GPIO_DDR6_P8=1, \
                                       bFM3_GPIO_PFR6_P8=0 )

/*---- GPIO bit P70 ----*/

#define GPIO1PIN_P70_GET             ( bFM3_GPIO_PDIR7_P0 )

#define GPIO1PIN_P70_PUT(v)          ( bFM3_GPIO_PDOR7_P0=(v) )

#define GPIO1PIN_P70_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P70_INITIN(v) \
                                                        : GPIO1PIN_P70_INITOUT(v) )

#define GPIO1PIN_P70_INITIN(v)       ( bFM3_GPIO_PCR7_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P0=0, \
                                       bFM3_GPIO_PFR7_P0=0 )

#define GPIO1PIN_P70_INITOUT(v)      ( bFM3_GPIO_PDOR7_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR7_P0=1, \
                                       bFM3_GPIO_PFR7_P0=0 )


/*---- GPIO bit NP70 ----*/

#define GPIO1PIN_NP70_GET            ( !(bFM3_GPIO_PDIR7_P0) )

#define GPIO1PIN_NP70_PUT(v)         ( bFM3_GPIO_PDOR7_P0=!(v) )

#define GPIO1PIN_NP70_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP70_INITIN(v) \
                                                        : GPIO1PIN_NP70_INITOUT(v) )

#define GPIO1PIN_NP70_INITIN(v)      ( bFM3_GPIO_PCR7_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P0=0, \
                                       bFM3_GPIO_PFR7_P0=0 )

#define GPIO1PIN_NP70_INITOUT(v)     ( bFM3_GPIO_PDOR7_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR7_P0=1, \
                                       bFM3_GPIO_PFR7_P0=0 )

/*---- GPIO bit P71 ----*/

#define GPIO1PIN_P71_GET             ( bFM3_GPIO_PDIR7_P1 )

#define GPIO1PIN_P71_PUT(v)          ( bFM3_GPIO_PDOR7_P1=(v) )

#define GPIO1PIN_P71_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P71_INITIN(v) \
                                                        : GPIO1PIN_P71_INITOUT(v) )

#define GPIO1PIN_P71_INITIN(v)       ( bFM3_GPIO_PCR7_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P1=0, \
                                       bFM3_GPIO_PFR7_P1=0 )

#define GPIO1PIN_P71_INITOUT(v)      ( bFM3_GPIO_PDOR7_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR7_P1=1, \
                                       bFM3_GPIO_PFR7_P1=0 )


/*---- GPIO bit NP71 ----*/

#define GPIO1PIN_NP71_GET            ( !(bFM3_GPIO_PDIR7_P1) )

#define GPIO1PIN_NP71_PUT(v)         ( bFM3_GPIO_PDOR7_P1=!(v) )

#define GPIO1PIN_NP71_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP71_INITIN(v) \
                                                        : GPIO1PIN_NP71_INITOUT(v) )

#define GPIO1PIN_NP71_INITIN(v)      ( bFM3_GPIO_PCR7_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P1=0, \
                                       bFM3_GPIO_PFR7_P1=0 )

#define GPIO1PIN_NP71_INITOUT(v)     ( bFM3_GPIO_PDOR7_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR7_P1=1, \
                                       bFM3_GPIO_PFR7_P1=0 )

/*---- GPIO bit P72 ----*/

#define GPIO1PIN_P72_GET             ( bFM3_GPIO_PDIR7_P2 )

#define GPIO1PIN_P72_PUT(v)          ( bFM3_GPIO_PDOR7_P2=(v) )

#define GPIO1PIN_P72_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P72_INITIN(v) \
                                                        : GPIO1PIN_P72_INITOUT(v) )

#define GPIO1PIN_P72_INITIN(v)       ( bFM3_GPIO_PCR7_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P2=0, \
                                       bFM3_GPIO_PFR7_P2=0 )

#define GPIO1PIN_P72_INITOUT(v)      ( bFM3_GPIO_PDOR7_P2=(v).bInitVal, \
                                       bFM3_GPIO_DDR7_P2=1, \
                                       bFM3_GPIO_PFR7_P2=0 )


/*---- GPIO bit NP72 ----*/

#define GPIO1PIN_NP72_GET            ( !(bFM3_GPIO_PDIR7_P2) )

#define GPIO1PIN_NP72_PUT(v)         ( bFM3_GPIO_PDOR7_P2=!(v) )

#define GPIO1PIN_NP72_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP72_INITIN(v) \
                                                        : GPIO1PIN_NP72_INITOUT(v) )

#define GPIO1PIN_NP72_INITIN(v)      ( bFM3_GPIO_PCR7_P2=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P2=0, \
                                       bFM3_GPIO_PFR7_P2=0 )

#define GPIO1PIN_NP72_INITOUT(v)     ( bFM3_GPIO_PDOR7_P2=!((v).bInitVal), \
                                       bFM3_GPIO_DDR7_P2=1, \
                                       bFM3_GPIO_PFR7_P2=0 )

/*---- GPIO bit P73 ----*/

#define GPIO1PIN_P73_GET             ( bFM3_GPIO_PDIR7_P3 )

#define GPIO1PIN_P73_PUT(v)          ( bFM3_GPIO_PDOR7_P3=(v) )

#define GPIO1PIN_P73_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P73_INITIN(v) \
                                                        : GPIO1PIN_P73_INITOUT(v) )

#define GPIO1PIN_P73_INITIN(v)       ( bFM3_GPIO_PCR7_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P3=0, \
                                       bFM3_GPIO_PFR7_P3=0 )

#define GPIO1PIN_P73_INITOUT(v)      ( bFM3_GPIO_PDOR7_P3=(v).bInitVal, \
                                       bFM3_GPIO_DDR7_P3=1, \
                                       bFM3_GPIO_PFR7_P3=0 )


/*---- GPIO bit NP73 ----*/

#define GPIO1PIN_NP73_GET            ( !(bFM3_GPIO_PDIR7_P3) )

#define GPIO1PIN_NP73_PUT(v)         ( bFM3_GPIO_PDOR7_P3=!(v) )

#define GPIO1PIN_NP73_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP73_INITIN(v) \
                                                        : GPIO1PIN_NP73_INITOUT(v) )

#define GPIO1PIN_NP73_INITIN(v)      ( bFM3_GPIO_PCR7_P3=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P3=0, \
                                       bFM3_GPIO_PFR7_P3=0 )

#define GPIO1PIN_NP73_INITOUT(v)     ( bFM3_GPIO_PDOR7_P3=!((v).bInitVal), \
                                       bFM3_GPIO_DDR7_P3=1, \
                                       bFM3_GPIO_PFR7_P3=0 )

/*---- GPIO bit P74 ----*/

#define GPIO1PIN_P74_GET             ( bFM3_GPIO_PDIR7_P4 )

#define GPIO1PIN_P74_PUT(v)          ( bFM3_GPIO_PDOR7_P4=(v) )

#define GPIO1PIN_P74_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P74_INITIN(v) \
                                                        : GPIO1PIN_P74_INITOUT(v) )

#define GPIO1PIN_P74_INITIN(v)       ( bFM3_GPIO_PCR7_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P4=0, \
                                       bFM3_GPIO_PFR7_P4=0 )

#define GPIO1PIN_P74_INITOUT(v)      ( bFM3_GPIO_PDOR7_P4=(v).bInitVal, \
                                       bFM3_GPIO_DDR7_P4=1, \
                                       bFM3_GPIO_PFR7_P4=0 )


/*---- GPIO bit NP74 ----*/

#define GPIO1PIN_NP74_GET            ( !(bFM3_GPIO_PDIR7_P4) )

#define GPIO1PIN_NP74_PUT(v)         ( bFM3_GPIO_PDOR7_P4=!(v) )

#define GPIO1PIN_NP74_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP74_INITIN(v) \
                                                        : GPIO1PIN_NP74_INITOUT(v) )

#define GPIO1PIN_NP74_INITIN(v)      ( bFM3_GPIO_PCR7_P4=(v).bPullup, \
                                       bFM3_GPIO_DDR7_P4=0, \
                                       bFM3_GPIO_PFR7_P4=0 )

#define GPIO1PIN_NP74_INITOUT(v)     ( bFM3_GPIO_PDOR7_P4=!((v).bInitVal), \
                                       bFM3_GPIO_DDR7_P4=1, \
                                       bFM3_GPIO_PFR7_P4=0 )

/*---- GPIO bit P80 ----*/

#define GPIO1PIN_P80_GET             ( bFM3_GPIO_PDIR8_P0 )

#define GPIO1PIN_P80_PUT(v)          ( bFM3_GPIO_PDOR8_P0=(v) )

#define GPIO1PIN_P80_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P80_INITIN(v) \
                                                        : GPIO1PIN_P80_INITOUT(v) )

#define GPIO1PIN_P80_INITIN(v)       ( bFM3_GPIO_PCR8_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR8_P0=0, \
                                       bFM3_GPIO_PFR8_P0=0, \
                                       bFM3_SPSR_USB0C = 0 )

#define GPIO1PIN_P80_INITOUT(v)      ( bFM3_GPIO_PDOR8_P0=(v).bInitVal, \
                                       bFM3_GPIO_DDR8_P0=1, \
                                       bFM3_GPIO_PFR8_P0=0, \
                                       bFM3_SPSR_USB0C = 0 )


/*---- GPIO bit NP80 ----*/

#define GPIO1PIN_NP80_GET            ( !(bFM3_GPIO_PDIR8_P0) )

#define GPIO1PIN_NP80_PUT(v)         ( bFM3_GPIO_PDOR8_P0=!(v) )

#define GPIO1PIN_NP80_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP80_INITIN(v) \
                                                        : GPIO1PIN_NP80_INITOUT(v) )

#define GPIO1PIN_NP80_INITIN(v)      ( bFM3_GPIO_PCR8_P0=(v).bPullup, \
                                       bFM3_GPIO_DDR8_P0=0, \
                                       bFM3_GPIO_PFR8_P0=0, \
                                       bFM3_SPSR_USB0C = 0 )

#define GPIO1PIN_NP80_INITOUT(v)     ( bFM3_GPIO_PDOR8_P0=!((v).bInitVal), \
                                       bFM3_GPIO_DDR8_P0=1, \
                                       bFM3_GPIO_PFR8_P0=0, \
                                       bFM3_SPSR_USB0C = 0 )

/*---- GPIO bit P81 ----*/

#define GPIO1PIN_P81_GET             ( bFM3_GPIO_PDIR8_P1 )

#define GPIO1PIN_P81_PUT(v)          ( bFM3_GPIO_PDOR8_P1=(v) )

#define GPIO1PIN_P81_INIT(v)         ( (0==(v).bOutput) ? GPIO1PIN_P81_INITIN(v) \
                                                        : GPIO1PIN_P81_INITOUT(v) )

#define GPIO1PIN_P81_INITIN(v)       ( bFM3_GPIO_PCR8_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR8_P1=0, \
                                       bFM3_GPIO_PFR8_P1=0, \
                                       bFM3_SPSR_USB0C = 0 )

#define GPIO1PIN_P81_INITOUT(v)      ( bFM3_GPIO_PDOR8_P1=(v).bInitVal, \
                                       bFM3_GPIO_DDR8_P1=1, \
                                       bFM3_GPIO_PFR8_P1=0, \
                                       bFM3_SPSR_USB0C = 0 )


/*---- GPIO bit NP81 ----*/

#define GPIO1PIN_NP81_GET            ( !(bFM3_GPIO_PDIR8_P1) )

#define GPIO1PIN_NP81_PUT(v)         ( bFM3_GPIO_PDOR8_P1=!(v) )

#define GPIO1PIN_NP81_INIT(v)        ( (0==(v).bOutput) ? GPIO1PIN_NP81_INITIN(v) \
                                                        : GPIO1PIN_NP81_INITOUT(v) )

#define GPIO1PIN_NP81_INITIN(v)      ( bFM3_GPIO_PCR8_P1=(v).bPullup, \
                                       bFM3_GPIO_DDR8_P1=0, \
                                       bFM3_GPIO_PFR8_P1=0, \
                                       bFM3_SPSR_USB0C = 0 )

#define GPIO1PIN_NP81_INITOUT(v)     ( bFM3_GPIO_PDOR8_P1=!((v).bInitVal), \
                                       bFM3_GPIO_DDR8_P1=1, \
                                       bFM3_GPIO_PFR8_P1=0, \
                                       bFM3_SPSR_USB0C = 0 )


/************************************************************
   PIN RELOCATION
*************************************************************/


#define PINRELOC_SET_EPFR(epfr,pos,width,value)    \
          (FM3_GPIO->epfr = (FM3_GPIO->epfr & ~(((1<<(width))-1)<<(pos)) | \
          ((value) << (pos))))


/*--- MDATA0 ---*/
#define SetPinFunc_MDATA0(dummy)       ( bFM3_EPFR11_UED00B = 0x01, \
                                         bFM3_GPIO_PFR5_P0 = 1 )

/*--- MDATA1 ---*/
#define SetPinFunc_MDATA1(dummy)       ( bFM3_EPFR11_UED01B = 0x01, \
                                         bFM3_GPIO_PFR5_P1 = 1 )

/*--- MDATA2 ---*/
#define SetPinFunc_MDATA2(dummy)       ( bFM3_EPFR11_UED02B = 0x01, \
                                         bFM3_GPIO_PFR5_P2 = 1 )

/*--- MDATA3 ---*/
#define SetPinFunc_MDATA3(dummy)       ( bFM3_EPFR11_UED03B = 0x01, \
                                         bFM3_GPIO_PFR5_P3 = 1 )

/*--- MDATA4 ---*/
#define SetPinFunc_MDATA4(dummy)       ( bFM3_EPFR11_UED04B = 0x01, \
                                         bFM3_GPIO_PFR5_P4 = 1 )

/*--- MDATA5 ---*/
#define SetPinFunc_MDATA5(dummy)       ( bFM3_EPFR11_UED05B = 0x01, \
                                         bFM3_GPIO_PFR5_P5 = 1 )

/*--- MDATA6 ---*/
#define SetPinFunc_MDATA6(dummy)       ( bFM3_EPFR11_UED06B = 0x01, \
                                         bFM3_GPIO_PFR3_P0 = 1 )

/*--- MDATA7 ---*/
#define SetPinFunc_MDATA7(dummy)       ( bFM3_EPFR11_UED07B = 0x01, \
                                         bFM3_GPIO_PFR3_P1 = 1 )

/*--- MDATA8 ---*/
#define SetPinFunc_MDATA8(dummy)       ( bFM3_EPFR11_UED08B = 0x01, \
                                         bFM3_GPIO_PFR1_P8 = 1, \
                                         bFM3_GPIO_ADE_AN8 = 0 )

/*--- MDATA9 ---*/
#define SetPinFunc_MDATA9(dummy)       ( bFM3_EPFR11_UED09B = 0x01, \
                                         bFM3_GPIO_PFR1_P9 = 1, \
                                         bFM3_GPIO_ADE_AN9 = 0 )

/*--- MDATA10 ---*/
#define SetPinFunc_MDATA10(dummy)      ( bFM3_EPFR11_UED10B = 0x01, \
                                         bFM3_GPIO_PFR1_PA = 1, \
                                         bFM3_GPIO_ADE_ANA = 0 )

/*--- MDATA11 ---*/
#define SetPinFunc_MDATA11(dummy)      ( bFM3_EPFR11_UED11B = 0x01, \
                                         bFM3_GPIO_PFR1_PB = 1, \
                                         bFM3_GPIO_ADE_ANB = 0 )

/*--- MDATA12 ---*/
#define SetPinFunc_MDATA12(dummy)      ( bFM3_EPFR11_UED12B = 0x01, \
                                         bFM3_GPIO_PFR1_PC = 1, \
                                         bFM3_GPIO_ADE_ANC = 0 )

/*--- MDATA13 ---*/
#define SetPinFunc_MDATA13(dummy)      ( bFM3_EPFR11_UED13B = 0x01, \
                                         bFM3_GPIO_PFR1_PD = 1, \
                                         bFM3_GPIO_ADE_AND = 0 )

/*--- MDATA14 ---*/
#define SetPinFunc_MDATA14(dummy)      ( bFM3_EPFR11_UED14B = 0x01, \
                                         bFM3_GPIO_PFR1_PE = 1, \
                                         bFM3_GPIO_ADE_ANE = 0 )

/*--- MDATA15 ---*/
#define SetPinFunc_MDATA15(dummy)      ( bFM3_EPFR11_UED15B = 0x01, \
                                         bFM3_GPIO_PFR1_PF = 1, \
                                         bFM3_GPIO_ADE_ANF = 0 )

/*--- MAD00 ---*/
#define SetPinFunc_MAD00(dummy)        ( bFM3_EPFR10_UEAOOE = 0x01, \
                                         bFM3_GPIO_PFR6_P2 = 1 )

/*--- MAD01 ---*/
#define SetPinFunc_MAD01(dummy)        ( bFM3_EPFR11_UEA01E = 0x01, \
                                         bFM3_GPIO_PFR6_P3 = 1 )

/*--- MAD02 ---*/
#define SetPinFunc_MAD02(dummy)        ( bFM3_EPFR11_UEA02E = 0x01, \
                                         bFM3_GPIO_PFR0_PF = 1 )

/*--- MAD03 ---*/
#define SetPinFunc_MAD03(dummy)        ( bFM3_EPFR11_UEA03E = 0x01, \
                                         bFM3_GPIO_PFR0_PE = 1 )

/*--- MAD04 ---*/
#define SetPinFunc_MAD04(dummy)        ( bFM3_EPFR11_UEA04E = 0x01, \
                                         bFM3_GPIO_PFR0_PD = 1 )

/*--- MAD05 ---*/
#define SetPinFunc_MAD05(dummy)        ( bFM3_EPFR11_UEA05E = 0x01, \
                                         bFM3_GPIO_PFR0_PC = 1 )

/*--- MAD06 ---*/
#define SetPinFunc_MAD06(dummy)        ( bFM3_EPFR11_UEA06E = 0x01, \
                                         bFM3_GPIO_PFR0_PB = 1 )

/*--- MAD07 ---*/
#define SetPinFunc_MAD07(dummy)        ( bFM3_EPFR11_UEA07E = 0x01, \
                                         bFM3_GPIO_PFR0_PA = 1 )

/*--- MAD08 ---*/
#define SetPinFunc_MAD08(dummy)        ( bFM3_EPFR10_UEA08E = 0x01, \
                                         bFM3_GPIO_PFR1_P3 = 1, \
                                         bFM3_GPIO_ADE_AN3 = 0 )

/*--- MAD09 ---*/
#define SetPinFunc_MAD09(dummy)        ( bFM3_EPFR10_UEA09E = 0x00, \
                                         bFM3_GPIO_PFR1_P2 = 1, \
                                         bFM3_GPIO_ADE_AN2 = 0 )

/*--- MAD10 ---*/
#define SetPinFunc_MAD10(dummy)        ( bFM3_EPFR10_UEA10E = 0x01, \
                                         bFM3_GPIO_PFR4_PE = 1 )

/*--- MAD11 ---*/
#define SetPinFunc_MAD11(dummy)        ( bFM3_EPFR10_UEA11E = 0x01, \
                                         bFM3_GPIO_PFR4_PD = 1 )

/*--- MAD12 ---*/
#define SetPinFunc_MAD12(dummy)        ( bFM3_EPFR10_UEA12E = 0x01, \
                                         bFM3_GPIO_PFR4_PC = 1 )

/*--- MAD13 ---*/
#define SetPinFunc_MAD13(dummy)        ( bFM3_EPFR10_UEA13E = 0x01, \
                                         bFM3_GPIO_PFR4_PB = 1 )

/*--- MAD14 ---*/
#define SetPinFunc_MAD14(dummy)        ( bFM3_EPFR10_UEA14E = 0x01, \
                                         bFM3_GPIO_PFR4_PA = 1 )

/*--- MAD15 ---*/
#define SetPinFunc_MAD15(dummy)        ( bFM3_EPFR10_UEA15E = 0x01, \
                                         bFM3_GPIO_PFR4_P9 = 1 )

/*--- MAD16 ---*/
#define SetPinFunc_MAD16(dummy)        ( bFM3_EPFR10_UEA16E = 0x01, \
                                         bFM3_GPIO_PFR4_P8 = 1 )

/*--- MAD17 ---*/
#define SetPinFunc_MAD17(dummy)        ( bFM3_EPFR10_UEA17E = 0x01, \
                                         bFM3_GPIO_PFR4_P5 = 1 )

/*--- MAD18 ---*/
#define SetPinFunc_MAD18(dummy)        ( bFM3_EPFR10_UEA18E = 0x01, \
                                         bFM3_GPIO_PFR4_P4 = 1 )

/*--- MAD19 ---*/
#define SetPinFunc_MAD19(dummy)        ( bFM3_EPFR10_UEA19E = 0x01, \
                                         bFM3_GPIO_PFR4_P3 = 1 )

/*--- MAD20 ---*/
#define SetPinFunc_MAD20(dummy)        ( bFM3_EPFR10_UEA20E = 0x01, \
                                         bFM3_GPIO_PFR4_P2 = 1 )

/*--- MAD21 ---*/
#define SetPinFunc_MAD21(dummy)        ( bFM3_EPFR10_UEA21E = 0x01, \
                                         bFM3_GPIO_PFR4_P1 = 1 )

/*--- MAD22 ---*/
#define SetPinFunc_MAD22(dummy)        ( bFM3_EPFR10_UEA22E = 0x01, \
                                         bFM3_GPIO_PFR4_P0 = 1 )

/*--- MAD23 ---*/
#define SetPinFunc_MAD23(dummy)        ( bFM3_EPFR10_UEA23E = 0x01, \
                                         bFM3_GPIO_PFR3_P5 = 1 )

/*--- MAD24 ---*/
#define SetPinFunc_MAD24(dummy)        ( bFM3_EPFR10_UEA24E = 0x01, \
                                         bFM3_GPIO_PFR3_P4 = 1 )

/*--- MNALE ---*/
#define SetPinFunc_MNALE(dummy)        ( bFM3_EPFR10_UEFLSE = 0x01, \
                                         bFM3_GPIO_PFR5_P7 = 1 )

/*--- MNCLE ---*/
#define SetPinFunc_MNCLE(dummy)        ( bFM3_EPFR10_UEFLSE = 0x01, \
                                         bFM3_GPIO_PFR5_P8 = 1 )

/*--- MNWEX ---*/
#define SetPinFunc_MNWEX(dummy)        ( bFM3_EPFR10_UEFLSE = 0x01, \
                                         bFM3_GPIO_PFR5_P9 = 1 )

/*--- MNREX ---*/
#define SetPinFunc_MNREX(dummy)        ( bFM3_EPFR10_UEFLSE = 0x01, \
                                         bFM3_GPIO_PFR5_PA = 1 )

/*--- MOEX ---*/
#define SetPinFunc_MOEX(dummy)         ( bFM3_EPFR10_UEOEXE = 0x01, \
                                         bFM3_GPIO_PFR1_P6 = 1, \
                                         bFM3_GPIO_ADE_AN6 = 0 )

/*--- MWEX ---*/
#define SetPinFunc_MWEX(dummy)         ( bFM3_EPFR10_UEWEXE = 0x01, \
                                         bFM3_GPIO_PFR1_P7 = 1, \
                                         bFM3_GPIO_ADE_AN7 = 0 )

/*--- MCSX0 ---*/
#define SetPinFunc_MCSX0(dummy)        ( bFM3_EPFR11_UECS0E = 0x01, \
                                         bFM3_GPIO_PFR1_P5 = 1, \
                                         bFM3_GPIO_ADE_AN5 = 0 )

/*--- MCSX1 ---*/
#define SetPinFunc_MCSX1(dummy)        ( bFM3_EPFR10_UECS1E = 0x01, \
                                         bFM3_GPIO_PFR1_P4 = 1, \
                                         bFM3_GPIO_ADE_AN4 = 0 )

/*--- MCSX2 ---*/
#define SetPinFunc_MCSX2(dummy)        ( bFM3_EPFR10_UECS2E = 0x01, \
                                         bFM3_GPIO_PFR3_P7 = 1 )

/*--- MCSX3 ---*/
#define SetPinFunc_MCSX3(dummy)        ( bFM3_EPFR10_UECS3E = 0x01, \
                                         bFM3_GPIO_PFR3_P6 = 1 )

/*--- MCSX4 ---*/
#define SetPinFunc_MCSX4(dummy)        ( bFM3_EPFR10_UECS4E = 0x01, \
                                         bFM3_GPIO_PFR2_P6 = 1 )

/*--- MCSX6 ---*/
#define SetPinFunc_MCSX6(dummy)        ( bFM3_EPFR10_UECS6E = 0x01, \
                                         bFM3_GPIO_PFR2_P8 = 1 )

/*--- MCSX5 ---*/
#define SetPinFunc_MCSX5(dummy)        ( bFM3_EPFR10_UECS5E = 0x01, \
                                         bFM3_GPIO_PFR2_P7 = 1 )

/*--- MCSX7 ---*/
#define SetPinFunc_MCSX7(dummy)        ( bFM3_EPFR10_UECS7E = 0x01, \
                                         bFM3_GPIO_PFR5_P6 = 1 )

/*--- MDQM0 ---*/
#define SetPinFunc_MDQM0(dummy)        ( bFM3_EPFR10_UEDQME = 0x01, \
                                         bFM3_GPIO_PFR3_P2 = 1 )

/*--- MDQM1 ---*/
#define SetPinFunc_MDQM1(dummy)        ( bFM3_EPFR10_UEDQME = 0x01, \
                                         bFM3_GPIO_PFR3_P3 = 1 )

/*--- TRACED0 ---*/
#define SetPinFunc_TRACED0(dummy)      ( bFM3_EPFR00_TRC0E = 0x00, \
                                         bFM3_GPIO_PFR0_P5 = 1 )

/*--- TRACED1 ---*/
#define SetPinFunc_TRACED1(dummy)      ( bFM3_EPFR00_TRC0E = 0x00, \
                                         bFM3_GPIO_PFR0_P6 = 1 )

/*--- TRACED2 ---*/
#define SetPinFunc_TRACED2(dummy)      ( bFM3_EPFR00_TRC1E = 0x01, \
                                         bFM3_GPIO_PFR0_P7 = 1 )

/*--- TRACED3 ---*/
#define SetPinFunc_TRACED3(dummy)      ( bFM3_EPFR00_TRC1E = 0x01, \
                                         bFM3_GPIO_PFR0_P8 = 1 )

/*--- TRACECLK ---*/
#define SetPinFunc_TRACECLK(dummy)     ( bFM3_EPFR00_TRC0E = 0x01, \
                                         bFM3_GPIO_PFR0_P9 = 1 )

/*--- ADTG_0 ---*/
#define SetPinFunc_ADTG_0(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x00 ), \
                                         bFM3_GPIO_PFR0_P7 = 1 )

/*--- ADTG_1 ---*/
#define SetPinFunc_ADTG_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x02 ), \
                                         bFM3_GPIO_PFR5_P5 = 1 )

/*--- ADTG_2 ---*/
#define SetPinFunc_ADTG_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x03 ), \
                                         bFM3_GPIO_PFR3_P9 = 1 )

/*--- ADTG_3 ---*/
#define SetPinFunc_ADTG_3(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x04 ), \
                                         bFM3_GPIO_PFR6_P2 = 1 )

/*--- ADTG_4 ---*/
#define SetPinFunc_ADTG_4(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x05 ), \
                                         bFM3_GPIO_PFR2_P8 = 1 )

/*--- ADTG_5 ---*/
#define SetPinFunc_ADTG_5(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x06 ), \
                                         bFM3_GPIO_PFR1_PF = 1, \
                                         bFM3_GPIO_ADE_ANF = 0 )

/*--- ADTG_6 ---*/
#define SetPinFunc_ADTG_6(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x07 ), \
                                         bFM3_GPIO_PFR3_P3 = 1 )

/*--- ADTG_7 ---*/
#define SetPinFunc_ADTG_7(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x08 ), \
                                         bFM3_GPIO_PFR4_P3 = 1 )

/*--- ADTG_8 ---*/
#define SetPinFunc_ADTG_8(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 20, 4,  0x09 ), \
                                         bFM3_GPIO_PFR6_P6 = 1 )

/*--- X0A ---*/
#define SetPinFunc_X0A(dummy)          ( bFM3_SPSR_SUBXC = 0x01, \
                                         bFM3_GPIO_PFR4_P6 = 1 )

/*--- X1A ---*/
#define SetPinFunc_X1A(dummy)          ( bFM3_SPSR_SUBXC = 0x01, \
                                         bFM3_GPIO_PFR4_P7 = 1 )

/*--- NMIX ---*/
#define SetPinFunc_NMIX(dummy)         ( bFM3_EPFR00_NMIS = 0x01, \
                                         bFM3_GPIO_PFR0_PF = 1 )

/*--- TRSTX ---*/
#define SetPinFunc_TRSTX(dummy)        ( bFM3_EPFR00_JTAGEN1S = 0x01, \
                                         bFM3_GPIO_PFR0_P0 = 1 )

/*--- TCK ---*/
#define SetPinFunc_TCK(dummy)          ( bFM3_EPFR00_JTAGEN0B = 0x01, \
                                         bFM3_GPIO_PFR0_P1 = 1 )

/*--- TDO ---*/
#define SetPinFunc_TDO(dummy)          ( bFM3_EPFR00_JTAGEN0B = 0x01, \
                                         bFM3_GPIO_PFR0_P4 = 1 )

/*--- TMS ---*/
#define SetPinFunc_TMS(dummy)          ( bFM3_EPFR00_JTAGEN0B = 0x01, \
                                         bFM3_GPIO_PFR0_P3 = 1 )

/*--- UDM0 ---*/
#define SetPinFunc_UDM0(dummy)         ( bFM3_SPSR_USB0C = 0x01, \
                                         bFM3_GPIO_PFR8_P0 = 1 )

/*--- UDP0 ---*/
#define SetPinFunc_UDP0(dummy)         ( bFM3_SPSR_USB0C = 0x01, \
                                         bFM3_GPIO_PFR8_P1 = 1 )

/*--- INT00_0 ---*/
#define SetPinFunc_INT00_0(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 0, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P0 = 1 )

/*--- INT00_1 ---*/
#define SetPinFunc_INT00_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 0, 2,  0x02 ), \
                                         bFM3_GPIO_PFR0_P5 = 1 )

/*--- INT00_2 ---*/
#define SetPinFunc_INT00_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 0, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_PA = 1 )

/*--- INT01_0 ---*/
#define SetPinFunc_INT01_0(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 2, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P1 = 1 )

/*--- INT01_1 ---*/
#define SetPinFunc_INT01_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR0_P6 = 1 )

/*--- INT01_2 ---*/
#define SetPinFunc_INT01_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 2, 2,  0x03 ), \
                                         bFM3_GPIO_PFR2_P4 = 1 )

/*--- INT02_0 ---*/
#define SetPinFunc_INT02_0(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 4, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P2 = 1 )

/*--- INT02_1 ---*/
#define SetPinFunc_INT02_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P1 = 1, \
                                         bFM3_GPIO_ADE_AN1 = 0 )

/*--- INT02_2 ---*/
#define SetPinFunc_INT02_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 4, 2,  0x03 ), \
                                         bFM3_GPIO_PFR2_P7 = 1 )

/*--- INT03_0 ---*/
#define SetPinFunc_INT03_0(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 6, 2,  0x00 ), \
                                         bFM3_GPIO_PFR6_P3 = 1 )

/*--- INT03_1 ---*/
#define SetPinFunc_INT03_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 6, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P4 = 1, \
                                         bFM3_GPIO_ADE_AN4 = 0 )

/*--- INT03_2 ---*/
#define SetPinFunc_INT03_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 6, 2,  0x03 ), \
                                         bFM3_GPIO_PFR3_P0 = 1 )

/*--- INT04_0 ---*/
#define SetPinFunc_INT04_0(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 8, 2,  0x00 ), \
                                         bFM3_GPIO_PFR3_P3 = 1 )

/*--- INT04_1 ---*/
#define SetPinFunc_INT04_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 8, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P7 = 1, \
                                         bFM3_GPIO_ADE_AN7 = 0 )

/*--- INT04_2 ---*/
#define SetPinFunc_INT04_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 8, 2,  0x03 ), \
                                         bFM3_GPIO_PFR3_P1 = 1 )

/*--- INT05_0 ---*/
#define SetPinFunc_INT05_0(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 10, 2,  0x00 ), \
                                         bFM3_GPIO_PFR2_P0 = 1 )

/*--- INT05_1 ---*/
#define SetPinFunc_INT05_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PA = 1, \
                                         bFM3_GPIO_ADE_ANA = 0 )

/*--- INT05_2 ---*/
#define SetPinFunc_INT05_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 10, 2,  0x03 ), \
                                         bFM3_GPIO_PFR3_P2 = 1 )

/*--- INT06_1 ---*/
#define SetPinFunc_INT06_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 12, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P1 = 1 )

/*--- INT06_2 ---*/
#define SetPinFunc_INT06_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 12, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_PE = 1 )

/*--- INT07_2 ---*/
#define SetPinFunc_INT07_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 14, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P3 = 1 )

/*--- INT08_1 ---*/
#define SetPinFunc_INT08_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 16, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P5 = 1 )

/*--- INT08_2 ---*/
#define SetPinFunc_INT08_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 16, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P6 = 1 )

/*--- INT09_1 ---*/
#define SetPinFunc_INT09_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P6 = 1 )

/*--- INT09_2 ---*/
#define SetPinFunc_INT09_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 18, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P9 = 1 )

/*--- INT10_1 ---*/
#define SetPinFunc_INT10_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 20, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P7 = 1 )

/*--- INT10_2 ---*/
#define SetPinFunc_INT10_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 20, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P4 = 1 )

/*--- INT11_1 ---*/
#define SetPinFunc_INT11_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 22, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P8 = 1 )

/*--- INT11_2 ---*/
#define SetPinFunc_INT11_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 22, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P6 = 1 )

/*--- INT12_1 ---*/
#define SetPinFunc_INT12_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 24, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P0 = 1 )

/*--- INT12_2 ---*/
#define SetPinFunc_INT12_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 24, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P8 = 1 )

/*--- INT13_1 ---*/
#define SetPinFunc_INT13_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 26, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P1 = 1 )

/*--- INT13_2 ---*/
#define SetPinFunc_INT13_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 26, 2,  0x03 ), \
                                         bFM3_GPIO_PFR7_P1 = 1 )

/*--- INT14_1 ---*/
#define SetPinFunc_INT14_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 28, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P8 = 1 )

/*--- INT14_2 ---*/
#define SetPinFunc_INT14_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 28, 2,  0x03 ), \
                                         bFM3_GPIO_PFR7_P2 = 1 )

/*--- INT15_1 ---*/
#define SetPinFunc_INT15_1(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 30, 2,  0x02 ), \
                                         bFM3_GPIO_PFR6_P0 = 1 )

/*--- INT15_2 ---*/
#define SetPinFunc_INT15_2(dummy)      ( PINRELOC_SET_EPFR( EPFR06, 30, 2,  0x03 ), \
                                         bFM3_GPIO_PFR7_P3 = 1 )

/*--- SIN0_0 ---*/
#define SetPinFunc_SIN0_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 4, 2,  0x00 ), \
                                         bFM3_GPIO_PFR2_P1 = 1 )

/*--- SIN0_1 ---*/
#define SetPinFunc_SIN0_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P4 = 1, \
                                         bFM3_GPIO_ADE_AN4 = 0 )

/*--- SOT0_0 ---*/
#define SetPinFunc_SOT0_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 6, 2,  0x01 ), \
                                         bFM3_GPIO_PFR2_P2 = 1 )

/*--- SOT0_1 ---*/
#define SetPinFunc_SOT0_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 6, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P5 = 1, \
                                         bFM3_GPIO_ADE_AN5 = 0 )

/*--- SCK0_0 ---*/
#define SetPinFunc_SCK0_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 8, 2,  0x01 ), \
                                         bFM3_GPIO_PFR2_P3 = 1 )

/*--- SCK0_1 ---*/
#define SetPinFunc_SCK0_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 8, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P6 = 1, \
                                         bFM3_GPIO_ADE_AN6 = 0 )

/*--- SIN1_0 ---*/
#define SetPinFunc_SIN1_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 10, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P6 = 1 )

/*--- SIN1_1 ---*/
#define SetPinFunc_SIN1_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P1 = 1, \
                                         bFM3_GPIO_ADE_AN1 = 0 )

/*--- SOT1_0 ---*/
#define SetPinFunc_SOT1_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 12, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P7 = 1 )

/*--- SOT1_1 ---*/
#define SetPinFunc_SOT1_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 12, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P2 = 1, \
                                         bFM3_GPIO_ADE_AN2 = 0 )

/*--- SCK1_0 ---*/
#define SetPinFunc_SCK1_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 14, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P8 = 1 )

/*--- SCK1_1 ---*/
#define SetPinFunc_SCK1_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 14, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_P3 = 1, \
                                         bFM3_GPIO_ADE_AN3 = 0 )

/*--- SIN2_0 ---*/
#define SetPinFunc_SIN2_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 16, 2,  0x00 ), \
                                         bFM3_GPIO_PFR7_P2 = 1 )

/*--- SIN2_1 ---*/
#define SetPinFunc_SIN2_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 16, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P4 = 1 )

/*--- SIN2_2 ---*/
#define SetPinFunc_SIN2_2(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 16, 2,  0x03 ), \
                                         bFM3_GPIO_PFR1_P7 = 1, \
                                         bFM3_GPIO_ADE_AN7 = 0 )

/*--- SOT2_0 ---*/
#define SetPinFunc_SOT2_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 18, 2,  0x01 ), \
                                         bFM3_GPIO_PFR7_P3 = 1 )

/*--- SOT2_1 ---*/
#define SetPinFunc_SOT2_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P5 = 1 )

/*--- SOT2_2 ---*/
#define SetPinFunc_SOT2_2(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 18, 2,  0x03 ), \
                                         bFM3_GPIO_PFR1_P8 = 1, \
                                         bFM3_GPIO_ADE_AN8 = 0 )

/*--- SCK2_0 ---*/
#define SetPinFunc_SCK2_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 20, 2,  0x01 ), \
                                         bFM3_GPIO_PFR7_P4 = 1 )

/*--- SCK2_1 ---*/
#define SetPinFunc_SCK2_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 20, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P6 = 1 )

/*--- SCK2_2 ---*/
#define SetPinFunc_SCK2_2(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 20, 2,  0x03 ), \
                                         bFM3_GPIO_PFR1_P9 = 1, \
                                         bFM3_GPIO_ADE_AN9 = 0 )

/*--- SIN3_0 ---*/
#define SetPinFunc_SIN3_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 22, 2,  0x00 ), \
                                         bFM3_GPIO_PFR6_P6 = 1 )

/*--- SIN3_1 ---*/
#define SetPinFunc_SIN3_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 22, 2,  0x02 ), \
                                         bFM3_GPIO_PFR5_P0 = 1 )

/*--- SIN3_2 ---*/
#define SetPinFunc_SIN3_2(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 22, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_P8 = 1 )

/*--- SOT3_0 ---*/
#define SetPinFunc_SOT3_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 24, 2,  0x01 ), \
                                         bFM3_GPIO_PFR6_P7 = 1 )

/*--- SOT3_1 ---*/
#define SetPinFunc_SOT3_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 24, 2,  0x02 ), \
                                         bFM3_GPIO_PFR5_P1 = 1 )

/*--- SOT3_2 ---*/
#define SetPinFunc_SOT3_2(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 24, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_P9 = 1 )

/*--- SCK3_0 ---*/
#define SetPinFunc_SCK3_0(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 26, 2,  0x01 ), \
                                         bFM3_GPIO_PFR6_P8 = 1 )

/*--- SCK3_1 ---*/
#define SetPinFunc_SCK3_1(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 26, 2,  0x02 ), \
                                         bFM3_GPIO_PFR5_P2 = 1 )

/*--- SCK3_2 ---*/
#define SetPinFunc_SCK3_2(dummy)       ( PINRELOC_SET_EPFR( EPFR07, 26, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_PA = 1 )

/*--- SIN4_0 ---*/
#define SetPinFunc_SIN4_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 4, 2,  0x00 ), \
                                         bFM3_GPIO_PFR0_PA = 1 )

/*--- SIN4_1 ---*/
#define SetPinFunc_SIN4_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PA = 1, \
                                         bFM3_GPIO_ADE_ANA = 0 )

/*--- SIN4_2 ---*/
#define SetPinFunc_SIN4_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 4, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P5 = 1 )

/*--- SOT4_0 ---*/
#define SetPinFunc_SOT4_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 6, 2,  0x01 ), \
                                         bFM3_GPIO_PFR0_PB = 1 )

/*--- SOT4_1 ---*/
#define SetPinFunc_SOT4_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 6, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PB = 1, \
                                         bFM3_GPIO_ADE_ANB = 0 )

/*--- SOT4_2 ---*/
#define SetPinFunc_SOT4_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 6, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P6 = 1 )

/*--- SCK4_0 ---*/
#define SetPinFunc_SCK4_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 8, 2,  0x01 ), \
                                         bFM3_GPIO_PFR0_PC = 1 )

/*--- SCK4_1 ---*/
#define SetPinFunc_SCK4_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 8, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PC = 1, \
                                         bFM3_GPIO_ADE_ANC = 0 )

/*--- SCK4_2 ---*/
#define SetPinFunc_SCK4_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 8, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P7 = 1 )

/*--- SIN5_0 ---*/
#define SetPinFunc_SIN5_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 10, 2,  0x00 ), \
                                         bFM3_GPIO_PFR6_P0 = 1 )

/*--- SIN5_1 ---*/
#define SetPinFunc_SIN5_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR6_P3 = 1 )

/*--- SIN5_2 ---*/
#define SetPinFunc_SIN5_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 10, 2,  0x03 ), \
                                         bFM3_GPIO_PFR3_P6 = 1 )

/*--- SOT5_0 ---*/
#define SetPinFunc_SOT5_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 12, 2,  0x01 ), \
                                         bFM3_GPIO_PFR6_P1 = 1 )

/*--- SOT5_1 ---*/
#define SetPinFunc_SOT5_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 12, 2,  0x02 ), \
                                         bFM3_GPIO_PFR6_P4 = 1 )

/*--- SOT5_2 ---*/
#define SetPinFunc_SOT5_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 12, 2,  0x03 ), \
                                         bFM3_GPIO_PFR3_P7 = 1 )

/*--- SCK5_0 ---*/
#define SetPinFunc_SCK5_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 14, 2,  0x01 ), \
                                         bFM3_GPIO_PFR6_P2 = 1 )

/*--- SCK5_1 ---*/
#define SetPinFunc_SCK5_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 14, 2,  0x02 ), \
                                         bFM3_GPIO_PFR6_P5 = 1 )

/*--- SCK5_2 ---*/
#define SetPinFunc_SCK5_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 14, 2,  0x03 ), \
                                         bFM3_GPIO_PFR3_P8 = 1 )

/*--- SIN6_0 ---*/
#define SetPinFunc_SIN6_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 16, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P3 = 1 )

/*--- SIN6_1 ---*/
#define SetPinFunc_SIN6_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 16, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P3 = 1 )

/*--- SOT6_0 ---*/
#define SetPinFunc_SOT6_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 18, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P4 = 1 )

/*--- SOT6_1 ---*/
#define SetPinFunc_SOT6_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P2 = 1 )

/*--- SCK6_0 ---*/
#define SetPinFunc_SCK6_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 20, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P5 = 1 )

/*--- SCK6_1 ---*/
#define SetPinFunc_SCK6_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 20, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P1 = 1 )

/*--- SIN7_0 ---*/
#define SetPinFunc_SIN7_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 22, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P9 = 1 )

/*--- SIN7_1 ---*/
#define SetPinFunc_SIN7_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 22, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_PE = 1 )

/*--- SOT7_0 ---*/
#define SetPinFunc_SOT7_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 24, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_PA = 1 )

/*--- SOT7_1 ---*/
#define SetPinFunc_SOT7_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 24, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_PD = 1 )

/*--- SCK7_0 ---*/
#define SetPinFunc_SCK7_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 26, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_PB = 1 )

/*--- SCK7_1 ---*/
#define SetPinFunc_SCK7_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 26, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_PC = 1 )

/*--- AIN0_0 ---*/
#define SetPinFunc_AIN0_0(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 0, 2,  0x00 ), \
                                         bFM3_GPIO_PFR3_P0 = 1 )

/*--- AIN0_1 ---*/
#define SetPinFunc_AIN0_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 0, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P9 = 1 )

/*--- AIN0_2 ---*/
#define SetPinFunc_AIN0_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 0, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P0 = 1 )

/*--- AIN1_1 ---*/
#define SetPinFunc_AIN1_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 6, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P0 = 1 )

/*--- AIN1_2 ---*/
#define SetPinFunc_AIN1_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 6, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_PC = 1 )

/*--- BIN0_0 ---*/
#define SetPinFunc_BIN0_0(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 2, 2,  0x00 ), \
                                         bFM3_GPIO_PFR3_P1 = 1 )

/*--- BIN0_1 ---*/
#define SetPinFunc_BIN0_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_PA = 1 )

/*--- BIN0_2 ---*/
#define SetPinFunc_BIN0_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 2, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P1 = 1 )

/*--- BIN1_1 ---*/
#define SetPinFunc_BIN1_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 8, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P1 = 1 )

/*--- BIN1_2 ---*/
#define SetPinFunc_BIN1_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 8, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_PD = 1 )

/*--- ZIN0_0 ---*/
#define SetPinFunc_ZIN0_0(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 4, 2,  0x00 ), \
                                         bFM3_GPIO_PFR3_P2 = 1 )

/*--- ZIN0_1 ---*/
#define SetPinFunc_ZIN0_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_PB = 1 )

/*--- ZIN0_2 ---*/
#define SetPinFunc_ZIN0_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 4, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P2 = 1 )

/*--- ZIN1_1 ---*/
#define SetPinFunc_ZIN1_1(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P2 = 1 )

/*--- ZIN1_2 ---*/
#define SetPinFunc_ZIN1_2(dummy)       ( PINRELOC_SET_EPFR( EPFR09, 10, 2,  0x03 ), \
                                         bFM3_GPIO_PFR4_PE = 1 )

/*--- RTO00_0 ---*/
#define SetPinFunc_RTO00_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 0, 2,  0x01 ), \
                                         bFM3_GPIO_PFR3_PA = 1 )

/*--- RTO00_1 ---*/
#define SetPinFunc_RTO00_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 0, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P3 = 1 )

/*--- RTO01_0 ---*/
#define SetPinFunc_RTO01_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 2, 2,  0x01 ), \
                                         bFM3_GPIO_PFR3_PB = 1 )

/*--- RTO01_1 ---*/
#define SetPinFunc_RTO01_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P4 = 1 )

/*--- RTO02_0 ---*/
#define SetPinFunc_RTO02_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 4, 2,  0x01 ), \
                                         bFM3_GPIO_PFR3_PC = 1 )

/*--- RTO02_1 ---*/
#define SetPinFunc_RTO02_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P5 = 1 )

/*--- RTO03_0 ---*/
#define SetPinFunc_RTO03_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 6, 2,  0x01 ), \
                                         bFM3_GPIO_PFR3_PD = 1 )

/*--- RTO03_1 ---*/
#define SetPinFunc_RTO03_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 6, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P6 = 1 )

/*--- RTO04_0 ---*/
#define SetPinFunc_RTO04_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 8, 2,  0x01 ), \
                                         bFM3_GPIO_PFR3_PE = 1 )

/*--- RTO04_1 ---*/
#define SetPinFunc_RTO04_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 8, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P7 = 1 )

/*--- RTO05_0 ---*/
#define SetPinFunc_RTO05_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 10, 2,  0x01 ), \
                                         bFM3_GPIO_PFR3_PF = 1 )

/*--- RTO05_1 ---*/
#define SetPinFunc_RTO05_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P8 = 1 )

/*--- RTO10_0 ---*/
#define SetPinFunc_RTO10_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 0, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P0 = 1 )

/*--- RTO10_1 ---*/
#define SetPinFunc_RTO10_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 0, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P0 = 1 )

/*--- RTO11_0 ---*/
#define SetPinFunc_RTO11_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 2, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P1 = 1 )

/*--- RTO11_1 ---*/
#define SetPinFunc_RTO11_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P1 = 1 )

/*--- RTO12_0 ---*/
#define SetPinFunc_RTO12_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 4, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P2 = 1 )

/*--- RTO12_1 ---*/
#define SetPinFunc_RTO12_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P2 = 1 )

/*--- RTO13_0 ---*/
#define SetPinFunc_RTO13_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 6, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P3 = 1 )

/*--- RTO13_1 ---*/
#define SetPinFunc_RTO13_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 6, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P3 = 1 )

/*--- RTO14_0 ---*/
#define SetPinFunc_RTO14_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 8, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P4 = 1 )

/*--- RTO14_1 ---*/
#define SetPinFunc_RTO14_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 8, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P4 = 1 )

/*--- RTO15_0 ---*/
#define SetPinFunc_RTO15_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 10, 2,  0x01 ), \
                                         bFM3_GPIO_PFR5_P5 = 1 )

/*--- RTO15_1 ---*/
#define SetPinFunc_RTO15_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P5 = 1 )

/*--- RX0_0 ---*/
#define SetPinFunc_RX0_0(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 24, 2,  0x00 ), \
                                         bFM3_GPIO_PFR7_P1 = 1 )

/*--- RX0_1 ---*/
#define SetPinFunc_RX0_1(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 24, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P5 = 1 )

/*--- RX0_2 ---*/
#define SetPinFunc_RX0_2(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 24, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P3 = 1 )

/*--- TX0_0 ---*/
#define SetPinFunc_TX0_0(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 26, 2,  0x01 ), \
                                         bFM3_GPIO_PFR7_P0 = 1 )

/*--- TX0_1 ---*/
#define SetPinFunc_TX0_1(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 26, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P4 = 1 )

/*--- TX0_2 ---*/
#define SetPinFunc_TX0_2(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 26, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P2 = 1 )

/*--- RX1_0 ---*/
#define SetPinFunc_RX1_0(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 28, 2,  0x00 ), \
                                         bFM3_GPIO_PFR2_P4 = 1 )

/*--- RX1_1 ---*/
#define SetPinFunc_RX1_1(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 28, 2,  0x02 ), \
                                         bFM3_GPIO_PFR5_P9 = 1 )

/*--- RX1_2 ---*/
#define SetPinFunc_RX1_2(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 28, 2,  0x03 ), \
                                         bFM3_GPIO_PFR1_P1 = 1, \
                                         bFM3_GPIO_ADE_AN1 = 0 )

/*--- TX1_0 ---*/
#define SetPinFunc_TX1_0(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 30, 2,  0x01 ), \
                                         bFM3_GPIO_PFR2_P5 = 1 )

/*--- TX1_1 ---*/
#define SetPinFunc_TX1_1(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 30, 2,  0x02 ), \
                                         bFM3_GPIO_PFR5_PA = 1 )

/*--- TX1_2 ---*/
#define SetPinFunc_TX1_2(dummy)        ( PINRELOC_SET_EPFR( EPFR09, 30, 2,  0x03 ), \
                                         bFM3_GPIO_PFR1_P2 = 1, \
                                         bFM3_GPIO_ADE_AN2 = 0 )

/*--- TIOA0_0 ---*/
#define SetPinFunc_TIOA0_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 2, 2,  0x01 ), \
                                         bFM3_GPIO_PFR4_P0 = 1 )

/*--- TIOA0_1 ---*/
#define SetPinFunc_TIOA0_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_PA = 1 )

/*--- TIOA0_2 ---*/
#define SetPinFunc_TIOA0_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 2, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P8 = 1 )

/*--- TIOA1_0 ---*/
#define SetPinFunc_TIOA1_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 10, 2,  0x01 ), \
                                         bFM3_GPIO_PFR4_P1 = 1 )

/*--- TIOA1_1 ---*/
#define SetPinFunc_TIOA1_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_PB = 1 )

/*--- TIOA1_2 ---*/
#define SetPinFunc_TIOA1_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 10, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P3 = 1 )

/*--- TIOA2_0 ---*/
#define SetPinFunc_TIOA2_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 18, 2,  0x01 ), \
                                         bFM3_GPIO_PFR4_P2 = 1 )

/*--- TIOA2_1 ---*/
#define SetPinFunc_TIOA2_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_PC = 1 )

/*--- TIOA2_2 ---*/
#define SetPinFunc_TIOA2_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 18, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P0 = 1 )

/*--- TIOA3_0 ---*/
#define SetPinFunc_TIOA3_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 26, 2,  0x01 ), \
                                         bFM3_GPIO_PFR4_P3 = 1 )

/*--- TIOA3_1 ---*/
#define SetPinFunc_TIOA3_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 26, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_PD = 1 )

/*--- TIOA3_2 ---*/
#define SetPinFunc_TIOA3_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 26, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_PD = 1 )

/*--- TIOA4_0 ---*/
#define SetPinFunc_TIOA4_0(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 2, 2,  0x01 ), \
                                         bFM3_GPIO_PFR4_P4 = 1 )

/*--- TIOA4_1 ---*/
#define SetPinFunc_TIOA4_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_PE = 1 )

/*--- TIOA4_2 ---*/
#define SetPinFunc_TIOA4_2(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 2, 2,  0x03 ), \
                                         bFM3_GPIO_PFR7_P0 = 1 )

/*--- TIOA5_0 ---*/
#define SetPinFunc_TIOA5_0(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 10, 2,  0x01 ), \
                                         bFM3_GPIO_PFR4_P5 = 1 )

/*--- TIOA5_1 ---*/
#define SetPinFunc_TIOA5_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 10, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_PF = 1 )

/*--- TIOA5_2 ---*/
#define SetPinFunc_TIOA5_2(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 10, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P5 = 1 )

/*--- TIOA6_1 ---*/
#define SetPinFunc_TIOA6_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR0_PC = 1 )

/*--- TIOA7_0 ---*/
#define SetPinFunc_TIOA7_0(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 26, 2,  0x01 ), \
                                         bFM3_GPIO_PFR6_P4 = 1 )

/*--- TIOA7_1 ---*/
#define SetPinFunc_TIOA7_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 26, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P3 = 1 )

/*--- TIOA7_2 ---*/
#define SetPinFunc_TIOA7_2(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 26, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P7 = 1 )

/*--- TIOB0_0 ---*/
#define SetPinFunc_TIOB0_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 4, 2,  0x00 ), \
                                         bFM3_GPIO_PFR4_P9 = 1 )

/*--- TIOB0_1 ---*/
#define SetPinFunc_TIOB0_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P0 = 1 )

/*--- TIOB0_2 ---*/
#define SetPinFunc_TIOB0_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 4, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P9 = 1 )

/*--- TIOB1_0 ---*/
#define SetPinFunc_TIOB1_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 12, 2,  0x00 ), \
                                         bFM3_GPIO_PFR4_PA = 1 )

/*--- TIOB1_1 ---*/
#define SetPinFunc_TIOB1_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 12, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P1 = 1 )

/*--- TIOB1_2 ---*/
#define SetPinFunc_TIOB1_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 12, 2,  0x03 ), \
                                         bFM3_GPIO_PFR5_P4 = 1 )

/*--- TIOB2_0 ---*/
#define SetPinFunc_TIOB2_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 20, 2,  0x00 ), \
                                         bFM3_GPIO_PFR4_PB = 1 )

/*--- TIOB2_1 ---*/
#define SetPinFunc_TIOB2_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 20, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P2 = 1 )

/*--- TIOB2_2 ---*/
#define SetPinFunc_TIOB2_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 20, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P1 = 1 )

/*--- TIOB3_0 ---*/
#define SetPinFunc_TIOB3_0(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 28, 2,  0x00 ), \
                                         bFM3_GPIO_PFR4_PC = 1 )

/*--- TIOB3_1 ---*/
#define SetPinFunc_TIOB3_1(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 28, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P3 = 1 )

/*--- TIOB3_2 ---*/
#define SetPinFunc_TIOB3_2(dummy)      ( PINRELOC_SET_EPFR( EPFR04, 28, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_PE = 1 )

/*--- TIOB4_0 ---*/
#define SetPinFunc_TIOB4_0(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 4, 2,  0x00 ), \
                                         bFM3_GPIO_PFR4_PD = 1 )

/*--- TIOB4_1 ---*/
#define SetPinFunc_TIOB4_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 4, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P4 = 1 )

/*--- TIOB4_2 ---*/
#define SetPinFunc_TIOB4_2(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 4, 2,  0x03 ), \
                                         bFM3_GPIO_PFR7_P1 = 1 )

/*--- TIOB5_0 ---*/
#define SetPinFunc_TIOB5_0(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 12, 2,  0x00 ), \
                                         bFM3_GPIO_PFR4_PE = 1 )

/*--- TIOB5_1 ---*/
#define SetPinFunc_TIOB5_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 12, 2,  0x02 ), \
                                         bFM3_GPIO_PFR3_P5 = 1 )

/*--- TIOB5_2 ---*/
#define SetPinFunc_TIOB5_2(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 12, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P6 = 1 )

/*--- TIOB6_1 ---*/
#define SetPinFunc_TIOB6_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 20, 2,  0x02 ), \
                                         bFM3_GPIO_PFR0_PB = 1 )

/*--- TIOB7_0 ---*/
#define SetPinFunc_TIOB7_0(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 28, 2,  0x00 ), \
                                         bFM3_GPIO_PFR6_P5 = 1 )

/*--- TIOB7_1 ---*/
#define SetPinFunc_TIOB7_1(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 28, 2,  0x02 ), \
                                         bFM3_GPIO_PFR2_P2 = 1 )

/*--- TIOB7_2 ---*/
#define SetPinFunc_TIOB7_2(dummy)      ( PINRELOC_SET_EPFR( EPFR05, 28, 2,  0x03 ), \
                                         bFM3_GPIO_PFR6_P8 = 1 )

/*--- DTTI0X_0 ---*/
#define SetPinFunc_DTTI0X_0(dummy)     ( PINRELOC_SET_EPFR( EPFR01, 16, 2,  0x00 ), \
                                         bFM3_GPIO_PFR3_P9 = 1 )

/*--- DTTI0X_1 ---*/
#define SetPinFunc_DTTI0X_1(dummy)     ( PINRELOC_SET_EPFR( EPFR01, 16, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PE = 1, \
                                         bFM3_GPIO_ADE_ANE = 0 )

/*--- DTTI1X_0 ---*/
#define SetPinFunc_DTTI1X_0(dummy)     ( PINRELOC_SET_EPFR( EPFR02, 16, 2,  0x00 ), \
                                         bFM3_GPIO_PFR5_P6 = 1 )

/*--- DTTI1X_1 ---*/
#define SetPinFunc_DTTI1X_1(dummy)     ( PINRELOC_SET_EPFR( EPFR02, 16, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_P8 = 1 )

/*--- IC00_0 ---*/
#define SetPinFunc_IC00_0(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 20, 3,  0x00 ), \
                                         bFM3_GPIO_PFR3_P8 = 1 )

/*--- IC00_1 ---*/
#define SetPinFunc_IC00_1(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 20, 3,  0x02 ), \
                                         bFM3_GPIO_PFR1_PA = 1, \
                                         bFM3_GPIO_ADE_ANA = 0 )

/*--- IC01_0 ---*/
#define SetPinFunc_IC01_0(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 23, 3,  0x00 ), \
                                         bFM3_GPIO_PFR3_P7 = 1 )

/*--- IC01_1 ---*/
#define SetPinFunc_IC01_1(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 23, 3,  0x02 ), \
                                         bFM3_GPIO_PFR1_PB = 1, \
                                         bFM3_GPIO_ADE_ANB = 0 )

/*--- IC02_0 ---*/
#define SetPinFunc_IC02_0(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 26, 3,  0x00 ), \
                                         bFM3_GPIO_PFR3_P6 = 1 )

/*--- IC02_1 ---*/
#define SetPinFunc_IC02_1(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 26, 3,  0x02 ), \
                                         bFM3_GPIO_PFR1_PC = 1, \
                                         bFM3_GPIO_ADE_ANC = 0 )

/*--- IC03_0 ---*/
#define SetPinFunc_IC03_0(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 29, 3,  0x00 ), \
                                         bFM3_GPIO_PFR3_P5 = 1 )

/*--- IC03_1 ---*/
#define SetPinFunc_IC03_1(dummy)       ( PINRELOC_SET_EPFR( EPFR01, 29, 3,  0x02 ), \
                                         bFM3_GPIO_PFR1_PD = 1, \
                                         bFM3_GPIO_ADE_AND = 0 )

/*--- IC10_0 ---*/
#define SetPinFunc_IC10_0(dummy)       ( PINRELOC_SET_EPFR( EPFR02, 20, 3,  0x00 ), \
                                         bFM3_GPIO_PFR0_PB = 1 )

/*--- IC10_1 ---*/
#define SetPinFunc_IC10_1(dummy)       ( PINRELOC_SET_EPFR( EPFR02, 20, 3,  0x02 ), \
                                         bFM3_GPIO_PFR4_P9 = 1 )

/*--- IC12_0 ---*/
#define SetPinFunc_IC12_0(dummy)       ( PINRELOC_SET_EPFR( EPFR02, 26, 3,  0x00 ), \
                                         bFM3_GPIO_PFR0_PD = 1 )

/*--- IC12_1 ---*/
#define SetPinFunc_IC12_1(dummy)       ( PINRELOC_SET_EPFR( EPFR02, 26, 3,  0x02 ), \
                                         bFM3_GPIO_PFR4_PB = 1 )

/*--- IC13_0 ---*/
#define SetPinFunc_IC13_0(dummy)       ( PINRELOC_SET_EPFR( EPFR02, 29, 3,  0x00 ), \
                                         bFM3_GPIO_PFR0_PE = 1 )

/*--- IC13_1 ---*/
#define SetPinFunc_IC13_1(dummy)       ( PINRELOC_SET_EPFR( EPFR02, 29, 3,  0x02 ), \
                                         bFM3_GPIO_PFR4_PC = 1 )

/*--- FRCK0_0 ---*/
#define SetPinFunc_FRCK0_0(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 18, 2,  0x00 ), \
                                         bFM3_GPIO_PFR3_P4 = 1 )

/*--- FRCK0_1 ---*/
#define SetPinFunc_FRCK0_1(dummy)      ( PINRELOC_SET_EPFR( EPFR01, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PF = 1, \
                                         bFM3_GPIO_ADE_ANF = 0 )

/*--- FRCK1_0 ---*/
#define SetPinFunc_FRCK1_0(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 18, 2,  0x00 ), \
                                         bFM3_GPIO_PFR0_PA = 1 )

/*--- FRCK1_1 ---*/
#define SetPinFunc_FRCK1_1(dummy)      ( PINRELOC_SET_EPFR( EPFR02, 18, 2,  0x02 ), \
                                         bFM3_GPIO_PFR4_PD = 1 )

/*--- CTS4_0 ---*/
#define SetPinFunc_CTS4_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 2, 2,  0x00 ), \
                                         bFM3_GPIO_PFR0_PE = 1 )

/*--- CTS4_1 ---*/
#define SetPinFunc_CTS4_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 2, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PD = 1, \
                                         bFM3_GPIO_ADE_AND = 0 )

/*--- CTS4_2 ---*/
#define SetPinFunc_CTS4_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 2, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P8 = 1 )

/*--- RTS4_0 ---*/
#define SetPinFunc_RTS4_0(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 0, 2,  0x01 ), \
                                         bFM3_GPIO_PFR0_PD = 1 )

/*--- RTS4_1 ---*/
#define SetPinFunc_RTS4_1(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 0, 2,  0x02 ), \
                                         bFM3_GPIO_PFR1_PE = 1, \
                                         bFM3_GPIO_ADE_ANE = 0 )

/*--- RTS4_2 ---*/
#define SetPinFunc_RTS4_2(dummy)       ( PINRELOC_SET_EPFR( EPFR08, 0, 2,  0x03 ), \
                                         bFM3_GPIO_PFR0_P9 = 1 )


/******************************************************************************/
/* Types                                                                      */
/******************************************************************************/

typedef struct stc_gpio1pin_init
{
    boolean_t bOutput;
    boolean_t bInitVal;
    boolean_t bPullup; 
} stc_gpio1pin_init_t;


//@} // Gpio1PinGroup

#ifdef __cplusplus
}
#endif

#endif /* __GPIO1PIN_H__ */

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
