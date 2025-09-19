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
/** \file extif.h
 **
 ** Headerfile for EXT-I/F functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  MWi  L3 address definition for instance pointer used
 **   - 2012-06-22  1.2  MWi  Device NAND Flash support check
 **
 ******************************************************************************/

#ifndef __EXTIF_H__
#define __EXTIF_H__

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
 ** \defgroup ExtifGroup External Bus Interface (EXT-I/F)
 **
 ** Provided functions of EXTIF module:
 ** 
 ** - Extif_InitArea()
 ** - Extif_DeInitArea()
 **
 ** An chip select area is configured with all its parameter in a structure
 ** of the type #stc_extif_area_config_t. This area is then initialized by
 ** calling Extif_InitArea() together with the chip select number (area number)
 ** and the pointer to the configuration. Extif_DeInitArea() clears all
 ** register of an chip select area.
 **
 ** \note This dirver does not check the availability of a chip select
 **       area of a certain MCU. The user has to take care of using the
 **       chip select areas correctly.
 ******************************************************************************/
//@{

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define EXTIF0          (*((stc_extifn_t *) L3_EXBUS_BASE))

#define EXTIF_MAX_AREAS 8   ///< <b> Adjust gaps and big gap to DCLKR in #stc_extifn_t when changing number! </b>
  
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
  
/**
 ******************************************************************************
 ** \brief Redefinition of MODEn bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_exbus_mode_field
{
  __IO uint32_t WDTH       : 2;
  __IO uint32_t RBMON      : 1;
  __IO uint32_t WEOFF      : 1;
  __IO uint32_t NAND       : 1;
  __IO uint32_t PAGE       : 1;
  __IO uint32_t RDY        : 1;
  __IO uint32_t SHRTDOUT   : 1;
  __IO uint32_t MPXMODE    : 1;
  __IO uint32_t ALEINV     : 1;
       uint32_t RESERVED1  : 1;
  __IO uint32_t MPXDOFF    : 1;  
  __IO uint32_t MPXCSOF    : 1;  
  __IO uint32_t MOEXEUP    : 1;  
} stc_exbus_mode_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of TIMn bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_exbus_tim_field
{
  __IO uint32_t RACC       : 4;
  __IO uint32_t RADC       : 4;
  __IO uint32_t FRADC      : 4;
  __IO uint32_t RIDLC      : 4;
  __IO uint32_t WACC       : 4;
  __IO uint32_t WADC       : 4;
  __IO uint32_t WWEC       : 4;
  __IO uint32_t WIDLC      : 4;
} stc_exbus_tim_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of AREAn bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_exbus_area_field
{
  __IO uint32_t ADDR       : 8;
       uint32_t RESERVED1  : 8;
  __IO uint32_t MASK       : 7;
} stc_exbus_area_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ATIMn bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_exbus_atim_field
{
  __IO uint32_t ALC        : 4;
  __IO uint32_t ALES       : 4;
  __IO uint32_t ALEW       : 4;
} stc_exbus_atim_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ATIMn bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_exbus_dclkr_field
{
  __IO uint32_t MDIV       : 4;
  __IO uint32_t MCLKON     : 1;
} stc_exbus_dclkr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of External Bus Interface register structure
 ******************************************************************************/
typedef struct stc_extifn
{
  union {
    uint32_t                MODE[EXTIF_MAX_AREAS];
    stc_exbus_mode_field_t  u8MODE[EXTIF_MAX_AREAS];
  };  
  union {
    uint32_t                TIM[EXTIF_MAX_AREAS];
    stc_exbus_tim_field_t   u8TIM[EXTIF_MAX_AREAS];
  };   
  union {
    uint32_t                AREA[EXTIF_MAX_AREAS];
    stc_exbus_area_field_t  u8AREA[EXTIF_MAX_AREAS];
  };   
  union {
    uint32_t                ATIM[EXTIF_MAX_AREAS];
    stc_exbus_atim_field_t  u8ATIM[EXTIF_MAX_AREAS];
  };
  uint8_t RESERVED1[0x280];
  union {
    uint32_t                DCLKR;
    stc_exbus_dclkr_field_t u8DCLKR;
  };
} stc_extifn_t;

/**
 ******************************************************************************
 ** \brief Extif data width
 ** 
 ** To select the data bus width
 ******************************************************************************/
typedef enum en_extif_width
{
  Extif8Bit   = 0,  ///< 8 Bit mode
  Extif16Bit  = 1   ///< 16 Bit  mode
} en_extif_width_t;

/**
 ******************************************************************************
 ** \brief Extif cycle defintion
 ** 
 ** To select the bus mode of the interface.
 **
 ** \note Do not change enumeration number. The numbers are taken for
 **       calculating the corresponding bitfield!
 ******************************************************************************/
typedef enum en_extif_cycle
{ 
  Extif0Cycle   = 0,  ///<  0 cycles
  Extif1Cycle   = 1,  ///<  1 cycle
  Extif2Cycle   = 2,  ///<  2 cycles
  Extif3Cycle   = 3,  ///<  3 cycles
  Extif4Cycle   = 4,  ///<  4 cycles
  Extif5Cycle   = 5,  ///<  5 cycles
  Extif6Cycle   = 6,  ///<  6 cycles
  Extif7Cycle   = 7,  ///<  7 cycles
  Extif8Cycle   = 8,  ///<  8 cycles
  Extif9Cycle   = 9,  ///<  9 cycles
  Extif10Cycle  = 10, ///< 10 cycles
  Extif11Cycle  = 11, ///< 11 cycles
  Extif12Cycle  = 12, ///< 12 cycles
  Extif13Cycle  = 13, ///< 13 cycles
  Extif14Cycle  = 14, ///< 14 cycles
  Extif15Cycle  = 15, ///< 15 cycles
  Extif16Cycle  = 16, ///< 16 cycles
  ExtifDisabled = 17  ///< Setting disabled
} en_extif_cycle_t;

/**
 ******************************************************************************
 ** \brief Extif mask setup (area size)
 ** 
 ** To select the Mask Setup value for the address area per chip select.
 ******************************************************************************/
typedef enum en_extif_mask
{
  Extif1MB   = 0,  ///< Area 1 MByte 
  Extif2MB   = 1,  ///< Area 2 MByte 
  Extif4MB   = 2,  ///< Area 4 MByte 
  Extif8MB   = 3,  ///< Area 8 MByte 
  Extif16MB  = 4,  ///< Area 16 MByte 
  Extif32MB  = 5,  ///< Area 32 MByte 
  Extif64MB  = 6,  ///< Area 64 MByte 
  Extif128MB = 7   ///< Area 128 MByte 
} en_extif_mask_t;

/**
 *****************************************************************************
 ** \brief Extif area configuration
 ** 
 ** The WDG configuration is done on a per area (Chip select) basis
 *****************************************************************************/
typedef struct stc_extif_area_config
{
  en_extif_width_t  enWidth;                   ///< 8, 16 bit data bus width. See description of #en_extif_width_t
  boolean_t         bMultplexMode;             ///< TRUE: Multiplex mode
  boolean_t         bReadByteMask;             ///< TRUE: Read Byte Mask enable
  boolean_t         bWriteEnableOff;           ///< TRUE: Write enable disabled
#if (L3_NAND_SUPPORT == L3_ON)
  boolean_t         bNandFlash;                ///< TRUE: NAND Flash bus enable, FLASE: NOR Flash/SRAM bus enable
#endif // #if (L3_NAND_SUPPORT == L3_ON)
  boolean_t         bPageAccess;               ///< TRUE: NOR Flash memory page access mode enabled
  boolean_t         bRdyOn;                    ///< TRUE: RDY mode enabled (Device Type 1)
  boolean_t         bStopDataOutAtFirstIdle;   ///< TRUE: Stop to write data output at first idle cycle, FALSE: Extends to write data output to the last idle cycle (Device Type 1)
  boolean_t         bAleInvert;                ///< TRUE: Invert ALE signal (negative polarity) (Device Type 1)
  boolean_t         bAddrOnDataLinesOff;       ///< TRUE: Do not output address to data lines (Hi-Z during ALC cycle period) (Device Type 1)
  boolean_t         bMpxcsOff;                 ///< TRUE: Do not assert MCSX in ALC cycle period (Device Type 1)
  boolean_t         bMoexWidthAsFradc;         ///< TRUE: MOEX width is set with FRADC, FALSE: MOEX width is set with RACC-RADC (Device Type 1)
  en_extif_cycle_t  enWriteIdleCycle;          ///< Write Idle Cycle timing
  en_extif_cycle_t  enWriteEnableCycle;        ///< Write Enable Cycle timing
  en_extif_cycle_t  enWriteAddressSetupCycle;  ///< Write Address Setup Cycle timing
  en_extif_cycle_t  enWriteAccessCycle;        ///< Write Access Cycle timing
  en_extif_cycle_t  enReadIdleCycle;           ///< Read Idle Cycle timing
  en_extif_cycle_t  enFirstReadAddressCycle;   ///< First Read Address Cycle timing
  en_extif_cycle_t  enReadAddressSetupCycle;   ///< Read Address Setup Cycle timing
  en_extif_cycle_t  enReadAccessCycle;         ///< Read Access Cycle timing
  en_extif_cycle_t  enAdressLatchEnableCycle;  ///< Address Latch Enable Cycle timing (Device Type 1)
  en_extif_cycle_t  enAddressLatchEnableSetupCycle; /// < Address Latch Enable Setup Cycle timing (Device Type 1)
  en_extif_cycle_t  enAddressLatchCycle;       ///< Addres Latch Cycle (Device Type 1)
  boolean_t         bMclkDivisionEnable;       ///< TRUE: Enables MCLK division (Device Type 1)
  uint8_t           u8MclkDivision;            ///< Division ration for MCLK (0 => 1 div, 1 => 2 div, ..., 15 => 16 div) (Device Type 1)
  en_extif_mask_t   enAreaMask;                ///< See description of #en_extif_mask_t
  uint8_t           u8AreaAddress;             ///< Address bits [27:20]
} stc_extif_area_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern en_result_t Extif_InitArea( uint8_t                  u8Area,
                                   stc_extif_area_config_t* pstcConfig
                                 ) ;

extern en_result_t Extif_DeInitArea( uint8_t u8Area ) ;

//@} // ExtifGroup

#ifdef __cplusplus
}
#endif

#endif /* __EXTIF_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
