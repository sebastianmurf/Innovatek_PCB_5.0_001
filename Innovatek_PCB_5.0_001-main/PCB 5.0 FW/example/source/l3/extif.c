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
/** \file extif.c
 **
 ** A detailed description is available at 
 ** @link ExtifGroup EXT-I/F Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-06-22  1.1  MWi  Device NAND Flash support check
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "extif.h"

#if (defined(L3_PERIPHERAL_EXTIF_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup ExtifGroup
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
 ** \brief Setup (init) an EXT-I/F area
 **
 ** \param u8Area       Extbus I/F area (chip select) number
 ** \param pstcConfig   Pointer to area configuration
 **
 ** \retval Ok                      Setup successful
 ** \retval ErrorInvalidParameter   pstcConfig == NULL, Area number wrong,
 **                                 other invalid setting.
 ******************************************************************************/
en_result_t Extif_InitArea( uint8_t                  u8Area,
                            stc_extif_area_config_t* pstcConfig
                           )
{
  // Local register predefinitions
  stc_exbus_mode_field_t  unMODE  = { 0 };
  stc_exbus_tim_field_t   unTIM   = { 0 };
  stc_exbus_area_field_t  unAREA  = { 0 };
  
#if (L3_DEVICE_TYPE != L3_TYPE0)  
  stc_exbus_atim_field_t  unATIM  = { 0 };
  stc_exbus_dclkr_field_t unDCLKR = { 0 };
#endif
  
  // Check for NULL Pointers
  if ((pstcConfig == NULL) ||
      (u8Area >= EXTIF_MAX_AREAS))
  {
    return ErrorInvalidParameter ;
  }
  
  // Configure mode
  switch(pstcConfig->enWidth)
  {
    case Extif8Bit :
      unMODE.WDTH = 0;
      break;
    case Extif16Bit :
      unMODE.WDTH = 1;
      break;
    default :
      return ErrorInvalidParameter ;
  }
  
  unMODE.RBMON    = (pstcConfig->bReadByteMask == TRUE)           ? 1 : 0 ;
  unMODE.WEOFF    = (pstcConfig->bWriteEnableOff == TRUE)         ? 1 : 0 ;
#if (L3_NAND_SUPPORT == L3_ON)
  unMODE.NAND     = (pstcConfig->bNandFlash == TRUE)              ? 1 : 0 ;
#endif
  unMODE.PAGE     = (pstcConfig->bPageAccess == TRUE)             ? 1 : 0 ;
#if (L3_DEVICE_TYPE != L3_TYPE0)
  unMODE.RDY      = (pstcConfig->bRdyOn == TRUE)                  ? 1 : 0 ;
  unMODE.SHRTDOUT = (pstcConfig->bStopDataOutAtFirstIdle == TRUE) ? 1 : 0 ;
  unMODE.MPXMODE  = (pstcConfig->bMultplexMode == TRUE)           ? 1 : 0 ;
  unMODE.ALEINV   = (pstcConfig->bAleInvert == TRUE)              ? 1 : 0 ;
  unMODE.MPXDOFF  = (pstcConfig->bAddrOnDataLinesOff == TRUE)     ? 1 : 0 ;
  unMODE.MPXCSOF  = (pstcConfig->bMpxcsOff == TRUE)               ? 1 : 0 ;
  unMODE.MOEXEUP  = (pstcConfig->bMoexWidthAsFradc == TRUE)       ? 1 : 0 ;
#endif
  
  // Configurate cycle timings
  //
  // Write Idle Cycle: 1 cycle ... 16 cycles
  if ((pstcConfig->enWriteIdleCycle == Extif0Cycle) ||
      (pstcConfig->enWriteIdleCycle > 16))
  {
    return ErrorInvalidParameter ;
  }
  else
  {  
    unTIM.WIDLC = (0x0F & (uint8_t)(pstcConfig->enWriteIdleCycle) - 1);
  }
  
  // Write Enable Cycle: 1 cycle ... 15 cycles + setting disabled
  if ((pstcConfig->enWriteEnableCycle == Extif0Cycle) ||
      (pstcConfig->enWriteEnableCycle == Extif16Cycle))
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    if (pstcConfig->enWriteEnableCycle == ExtifDisabled)
    {
      unTIM.WWEC  = 0xF;
    }
    else
    {
      unTIM.WWEC  = (0x0F & (uint8_t)(pstcConfig->enWriteEnableCycle) - 1);
    }
  }
  
  // Write Address Setup Cycle: 1 cycle ... 15 cycles + setting disabled
  if ((pstcConfig->enWriteAddressSetupCycle == Extif0Cycle) ||
      (pstcConfig->enWriteAddressSetupCycle == Extif16Cycle))
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    if (pstcConfig->enWriteAddressSetupCycle == ExtifDisabled)
    {
      unTIM.WADC  = 0xF;
    }
    else
    {
      unTIM.WADC  = (0x0F & (uint8_t)(pstcConfig->enWriteAddressSetupCycle) - 1);
    }
  }
  
  // Write Access Cycle: 3 cycles ... 16 cycles + setting disabled
  if (pstcConfig->enWriteAccessCycle < Extif3Cycle) 
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    if (pstcConfig->enWriteAccessCycle == ExtifDisabled)
    {
      unTIM.WACC  = 0;
    }
    else
    {
      unTIM.WACC  = (0x0F & (uint8_t)(pstcConfig->enWriteAccessCycle) - 1);
    }
  }
  
  // Read Idle Cycle: 1 cycle ... 16 cycles
  if ((pstcConfig->enReadIdleCycle == Extif0Cycle) ||
      (pstcConfig->enReadIdleCycle > 16))
  {
    return ErrorInvalidParameter ;
  }
  else
  {  
    unTIM.RIDLC = (0x0F & (uint8_t)(pstcConfig->enReadIdleCycle) - 1);
  }
  
  // First Read Address Cycle: 0 cycles ... 15 cycles
  if (pstcConfig->enFirstReadAddressCycle > Extif15Cycle)
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    unTIM.FRADC  = (0x0F & (uint8_t)(pstcConfig->enFirstReadAddressCycle));
  }

  // Read Address Setup Cycle: 0 cycles ... 15 cycles
  if (pstcConfig->enReadAddressSetupCycle > Extif15Cycle)
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    unTIM.RADC  = (0x0F & (uint8_t)(pstcConfig->enReadAddressSetupCycle));
  }
  
  // Read Access Cycle: 1 cycle ... 16 cycles
  if ((pstcConfig->enReadAccessCycle == Extif0Cycle) ||
      (pstcConfig->enReadAccessCycle > 16))
  {
    return ErrorInvalidParameter ;
  }
  else
  {  
    unTIM.RACC = (0x0F & (uint8_t)(pstcConfig->enReadAccessCycle) - 1);
  }

#if (L3_DEVICE_TYPE != L3_TYPE0)
  // Address Latch Enable Cycle: 1 cycle ... 16 cycles
  if ((pstcConfig->enReadAccessCycle == Extif0Cycle) ||
      (pstcConfig->enReadAccessCycle > 16))
  {
    return ErrorInvalidParameter ;
  }
  else
  {  
    unATIM.ALEW = (0x0F & (uint8_t)(pstcConfig->enAdressLatchEnableCycle) - 1);
  }

  // Address Latch Enable Setup Cycle: 0 cycles ... 15 cycles
  if (pstcConfig->enAddressLatchEnableSetupCycle > Extif15Cycle)
  {
    return ErrorInvalidParameter ;
  }
  else
  { 
    unATIM.ALES  = (0x0F & (uint8_t)(pstcConfig->enAddressLatchEnableSetupCycle));
  }

  // Address Latch Cycle: 1 cycle ... 16 cycles
  if ((pstcConfig->enAddressLatchCycle == Extif0Cycle) ||
      (pstcConfig->enAddressLatchCycle > 16))
  {
    return ErrorInvalidParameter ;
  }
  else
  {  
    unATIM.ALC = (0x0F & (uint8_t)(pstcConfig->enAddressLatchCycle) - 1);
  }
#endif
  
  // Set Area and Mask
  switch(pstcConfig->enAreaMask)
  {
    case Extif1MB:
      unAREA.MASK = 0x00;
      break;
    case Extif2MB:
      unAREA.MASK = 0x01;
      break;    
    case Extif4MB:
      unAREA.MASK = 0x03;
      break;
    case Extif8MB:
      unAREA.MASK = 0x07;
      break;
    case Extif16MB:
      unAREA.MASK = 0x0F;
      break;
    case Extif32MB:
      unAREA.MASK = 0x1F;
      break;    
    case Extif64MB:
      unAREA.MASK = 0x3F;
      break;
    case Extif128MB:
      unAREA.MASK = 0x7F;
      break; 
    default:
      return ErrorInvalidParameter ;
  }
  
  if (pstcConfig->u8AreaAddress >= 0x80)
  {
    return ErrorInvalidParameter ;
  }

  unAREA.ADDR = pstcConfig->u8AreaAddress;
  
#if (L3_DEVICE_TYPE != L3_TYPE0)
  // MCLK and division
  unDCLKR.MCLKON  = (pstcConfig->bMclkDivisionEnable == TRUE) ? 1 : 0 ;
  
  if (pstcConfig->u8MclkDivision > 15)
  {
    return ErrorInvalidParameter ;
  }
  
  unDCLKR.MDIV = pstcConfig->u8MclkDivision;
#endif
  
  // now setup hardware  
  EXTIF0.u8MODE[u8Area] = unMODE  ;
  EXTIF0.u8TIM[u8Area]  = unTIM   ;
  EXTIF0.u8AREA[u8Area] = unAREA  ;
  
#if (L3_DEVICE_TYPE != L3_TYPE0)
  EXTIF0.u8ATIM[u8Area] = unATIM  ;
  EXTIF0.u8DCLKR        = unDCLKR ;
#endif
  
  return Ok;
} // Extif_InitArea

/**
 ******************************************************************************
 ** \brief Clear (de-init) an EXT-I/F area
 **
 ** \param u8Area       Extbus I/F area (chip select) number
 **
 ** \retval Ok                      De-Init successful
 ** \retval ErrorInvalidParameter   Area number wrong.
 ******************************************************************************/
en_result_t Extif_DeInitArea( uint8_t u8Area )
{
  // Check for correct area number
  if (u8Area >= EXTIF_MAX_AREAS)
  {
    return ErrorInvalidParameter ;
  }  
  
  // now clear hardware  
  EXTIF0.MODE[u8Area] = 0 ;
  EXTIF0.TIM[u8Area]  = 0 ;
  EXTIF0.AREA[u8Area] = 0 ;
  
#if (L3_DEVICE_TYPE != L3_TYPE0)
  EXTIF0.ATIM[u8Area] = 0 ;
  EXTIF0.DCLKR        = 0 ;
#endif  
  
  return Ok;
} // Extif_DeInitArea

//@} // ExtifGroup

#endif // #if (defined(L3_PERIPHERAL_EXTIF_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/


