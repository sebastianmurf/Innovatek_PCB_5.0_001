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
/** \file crtrim.h
 **
 ** Headerfile for CRTRIM functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-10  1.1  MWi  Flash programming usage comments updated
 **   - 2012-06-26  1.2  MWi  Device type 6 and 7 added
 **
 ******************************************************************************/

#ifndef __CRTRIM_H__
#define __CRTRIM_H__

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
 ** \defgroup CrtrimGroup High-Speed CR Trimming (CRTRIM)
 **
 ** Provided functions of CRTRIM module:
 ** - Crtrim_SetTrimmingFromFlash()
 ** - Crtrim_SetTrimming()
 ** - Crtrim_SetHsRcClkDiv()
 ** - Crtrim_UpdateFlash()
 **
 ** The trimming function Crtrim_SetTrimmingFromFlash() copies the Flash HS-CR
 ** trim data to the trimming register of the HS-CR Clock. If all bits of this
 ** data are equal to 1 (erased CR Trimming Sector)
 ** result_t#ErrorInvalidParameter returned and the trimming register remains
 ** unchanged.
 **
 ** Crtrim_SetTrimming() sets the function's argument to the HS-CR trimming
 ** register.
 **
 ** \note If #L3_PERIPHERAL_ENABLE_CRTRIM_FLASH is set to #L3_ON in l3_user.h
 ** also the function Crtrim_UpdateFlash() is compiled. #L3_NO_FLASH_RAMCODE 
 ** must be set to #L3_OFF and #L3_PERIPHERAL_ENABLE_FLASH to #L3_ON in this
 ** case!
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
  
/// CR-TRIMM sector address
#define CRTRIM_SECTOR_ADDRESS 0x00101004
  
/// Device dependent maximum CR-TRIMM value
#if   (L3_DEVICE_TYPE == L3_TYPE0) || \
      (L3_DEVICE_TYPE == L3_TYPE3) || \
      (L3_DEVICE_TYPE == L3_TYPE7)
  #define CR_TRIMM_MAX_VALUE 0x03FF
#elif (L3_DEVICE_TYPE == L3_TYPE1) || \
      (L3_DEVICE_TYPE == L3_TYPE2) || \
      (L3_DEVICE_TYPE == L3_TYPE4) || \
      (L3_DEVICE_TYPE == L3_TYPE5) 
  #define CR_TRIMM_MAX_VALUE 0x00FF
#elif (L3_DEVICE_TYPE == L3_TYPE6)
  #define CR_TRIMM_MAX_VALUE 0x03FF
#else
  #error Device Type not available!  
#endif
        
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
 
/**
 ******************************************************************************
 ** \brief High-Speed RC-Clock Frequency Division
 ** 
 ** Enumeration for the HS-RC Clock Divider
 ******************************************************************************/
typedef enum en_clk_hs_rc_div
{
  HsClkDiv8   = 0, ///< Clock division ratio 1/8 (initial value)
  HsClkDiv4   = 1, ///< Clock division ratio 1/4
  HsClkDiv16  = 2, ///< Clock division ratio 1/16
  HsClkDiv32  = 3, ///< Clock division ratio 1/32
  HsClkDiv64  = 4, ///< Clock division ratio 1/64  (only supported for TYPE3 devices!)
  HsClkDiv128 = 5, ///< Clock division ratio 1/128 (only supported for TYPE3 devices!)
  HsClkDiv256 = 6, ///< Clock division ratio 1/256 (only supported for TYPE3 devices!)
  HsClkDiv512 = 7, ///< Clock division ratio 1/512 (only supported for TYPE3 devices!) 
}
en_clk_hs_rc_div_t;
        
/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern en_result_t Crtrim_SetTrimmingFromFlash(void) ;

extern en_result_t Crtrim_SetTrimming(uint16_t u16TrimmData) ;

#if L3_PERIPHERAL_ENABLE_CRTRIM_FLASH == L3_ON
  extern en_result_t Crtrim_UpdateFlash(uint16_t u16TrimmData) ;
#endif
  
extern en_result_t Crtrim_SetHsRcClkDiv(en_clk_hs_rc_div_t enHsRcClockDiv) ;

//@} // CrtrimGroup

#ifdef __cplusplus
}
#endif

#endif /* __CRTRIM_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/


