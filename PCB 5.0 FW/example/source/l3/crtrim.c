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
/** \file crtrim.c
 **
 ** A detailed description is available at 
 ** @link CrtrimGroup High-Speed CR Trimming module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "crtrim.h"

#if L3_PERIPHERAL_ENABLE_CRTRIM_FLASH == L3_ON
  #include "flash.h"
#endif

#if (defined(L3_PERIPHERAL_CRTRIM_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup CrtrimGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Set CR Trimming Data from Flash
 **
 ** This function copies the data of the Flash CR trimming contents to the
 ** HS-CR clock trimming register.
 **
 ** \retval Ok                      HS-CR Trimming data updated in HS-CR Clock
 ** \retval ErrorInvalidParameter   Flash HS-CR Trimming data empty
 ******************************************************************************/
en_result_t Crtrim_SetTrimmingFromFlash(void)
{
  if( CR_TRIMM_MAX_VALUE == (FM3_FLASH_IF->CRTRMM & CR_TRIMM_MAX_VALUE) )
  {  
    // Flash CR Trim sector: All relevant bits == 1 => No valid data
    return ErrorInvalidParameter ;  
  }
   
  FM3_CRTRIM->MCR_RLR  = 0x1ACCE554;            // Unlock (MCR_FTRM)
  FM3_CRTRIM->MCR_FTRM = FM3_FLASH_IF->CRTRMM;  // Set MCR_FTRM 
  FM3_CRTRIM->MCR_RLR  = 0x00000000;            // Lock (MCR_FTRM)
  
  return Ok ;
} // Crtrim_SetTrimmingFromFlash

/**
 ******************************************************************************
 ** \brief Set CR Trimming Data immediate
 **
 ** This function copies the function's argument to the HS-CR clock trimming
 ** register.
 **
 ** \param [in]  u16TrimmData       HS-CR Trimming data
 **
 ** \retval Ok                      HS-CR Trimming data updated in HS-CR Clock
 ** \retval ErrorInvalidParameter   Trimming data invalid (>= CR_TRIMM_MAX_VALUE)
 **
 ******************************************************************************/
en_result_t Crtrim_SetTrimming(uint16_t u16TrimmData)
{
  if(u16TrimmData >= CR_TRIMM_MAX_VALUE)
  {
    return ErrorInvalidParameter ;  
  }
   
  FM3_CRTRIM->MCR_RLR  = 0x1ACCE554;            // Unlock (MCR_FTRM)
  FM3_CRTRIM->MCR_FTRM = u16TrimmData;          // Set MCR_FTRM 
  FM3_CRTRIM->MCR_RLR  = 0x00000000;            // Lock (MCR_FTRM)
  
  return Ok ;
} // Crtrim_SetTrimming

#if L3_PERIPHERAL_ENABLE_CRTRIM_FLASH == L3_ON
/**
 ******************************************************************************
 ** \brief Set CR Trimming Data in Flash memory. Erase CR Trimming sector
 **        previously.
 **
 ** \param [in]  u16TrimmData         HS-CR Trimming data
 **
 ** \retval Ok                        HS-CR Trimming data updated in HS-CR
 **                                   Sector
 ** \retval ErrorInvalidParameter     Trimming data invalid (>= CR_TRIMM_MAX_VALUE)
 ** \retval ErrorTimeout              Flash programming time out error
 ** \retval ErrorOperationInProgress  Flash not ready to program
 ** \retval ErrorInvalidParameter     Flash data cannot be programmed, because
 **                                     one or more bits cannot be programmed 
 **                                     from '0' to '1'.
 **
 ******************************************************************************/
en_result_t Crtrim_UpdateFlash(uint16_t u16TrimmData)
{
  en_result_t enErrorCode;
  
  if(u16TrimmData >= CR_TRIMM_MAX_VALUE)
  {
    return ErrorInvalidParameter ;  
  }
  
  enErrorCode = Flash_Erase(CRTRIM_SECTOR_ADDRESS);
  if (enErrorCode != Ok)
  {
    return enErrorCode;
  }
      
  enErrorCode = Flash_Write16(CRTRIM_SECTOR_ADDRESS, u16TrimmData);
  if (enErrorCode != Ok)
  {
    return enErrorCode;
  }    
  
#if (L3_ECC_FLASH == L3_ON)
  // Fill upper 16 Bit word for ECC Flash to calculate ECC
  enErrorCode = Flash_Write16(CRTRIM_SECTOR_ADDRESS + 2, 0xFFFF);
  if (enErrorCode != Ok)
  {
    return enErrorCode;
  } 
#endif
      
  return Ok;
} // Crtrim_UpdateFlash
#endif // L3_PERIPHERAL_ENABLE_CRTRIM_FLASH == L3_ON

/**
 ******************************************************************************
 ** \brief Set High Speed RC Clock Divider
 **
 ** \param [in]  enHsRcClkDiv      Clock divider
 ** 
 ** \retval Ok                     Clock sourse set
 ** \retval ErrorInvalidParameter  illegal setting for enHsRcClockDiv
 **
 ******************************************************************************/
en_result_t Crtrim_SetHsRcClkDiv(en_clk_hs_rc_div_t enHsRcClkDiv)
{
  switch(enHsRcClkDiv)
  {
    case HsClkDiv4:
      FM3_CRTRIM->MCR_PSR = 0x00;
      break;
    case HsClkDiv8:
      FM3_CRTRIM->MCR_PSR = 0x01;
      break;   
    case HsClkDiv16:
      FM3_CRTRIM->MCR_PSR = 0x02;
      break;  
    case HsClkDiv32:
      FM3_CRTRIM->MCR_PSR = 0x03;
      break;  
      
#if (L3_DEVICE_TYPE == L3_TYPE3)
    case HsClkDiv64:
      FM3_CRTRIM->MCR_PSR = 0x04;
      break;
    case HsClkDiv128:
      FM3_CRTRIM->MCR_PSR = 0x05;
      break;   
    case HsClkDiv256:
      FM3_CRTRIM->MCR_PSR = 0x06;
      break;  
    case HsClkDiv512:
      FM3_CRTRIM->MCR_PSR = 0x07;
      break;  
#endif
      
    default:
      return ErrorInvalidParameter ;  
  }
  
  return Ok;
} // Crtrim_SetHsRcClkDiv

//@} // CrtrimGroup

#endif // #if (defined(L3_PERIPHERAL_CRTRIM_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/


