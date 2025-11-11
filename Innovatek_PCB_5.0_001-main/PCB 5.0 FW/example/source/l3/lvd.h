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
/** \file lvd.h
 **
 ** Headerfile for LVD functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-27  1.1  MWi  Change for configuration A, B, and C; L3 address
 **                           definition for instance pointer used
 **
 ******************************************************************************/

#ifndef __LVD_H__
#define __LVD_H__

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
 ** \defgroup LvdGroup Low Voltage Detection (LVD)
 **
 ** Provided functions of LVD module:
 ** - Lvd_InitIrq()
 ** - Lvd_DeInitIrq()
 **
 ** \note Because the LVD reset is always enabled, there is no Lvd_Init
 **       or Lvd_DeInit function.
 **
 ** Lvd_InitIrq() initializes the LVD interrupt with the given voltage
 ** threshold and Lvd_DeInitIrq() disables the LVD interrupt. In case of
 ** an interrupt a callback function is called, if its pointer != NULL.
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
 
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
#define LVD0  ((volatile stc_lvd_t *) L3_LVD_BASE)
  
/**
 ******************************************************************************
 ** \brief Redefinition of LVD LVD_CTL low byte bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_lvd_ctll_field
{
        uint8_t RESERVED1  : 1;
  __IO  uint8_t LVDIM      : 1;
#if (L3_DEVICE_TYPE == L3_TYPE6)
  __IO  uint8_t SVHI       : 5;
#else
  __IO  uint8_t SVHI       : 4;
        uint8_t RESERVED2  : 1;
#endif      
  __IO  uint8_t LVDIE      : 1;
} stc_lvd_ctll_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of LVD LVD_CTL high byte bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_lvd_ctlh_field
{
        uint8_t RESERVED1  : 2;
#if (L3_DEVICE_TYPE == L3_TYPE6)
  __IO  uint8_t SVHR       : 5;
#else
  __IO  uint8_t SVHR       : 4;
        uint8_t RESERVED2  : 1;
#endif
  __IO  uint8_t LVDRE      : 1;
} stc_lvd_ctlh_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of LVD LVD_STR bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_lvd_str_field
{
        uint8_t RESERVED1  : 7;
  __IO  uint8_t LVDIR      : 1;
} stc_lvd_str_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of LVD LVD_CLR bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_lvd_clr_field
{
        uint8_t RESERVED1  : 7;
  __IO  uint8_t LVDCL      : 1;
} stc_lvd_clr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of LVD LVD_STR2 bit field structures for
 **        unification of all configurations
 ******************************************************************************/
typedef struct stc_lvd_str2_field
{
        uint8_t RESERVED1  : 6;
  __IO  uint8_t LVDRRDY    : 1;        
  __IO  uint8_t LVDIRDY    : 1;
} stc_lvd_str2_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of LVD register structure for unification of
 **        all configurations
 ******************************************************************************/
typedef struct stc_lvd {
  union {
    uint8_t                   LVD_CTLL;
    stc_lvd_ctll_field_t      u8LVD_CTLL;
  };
  union {
    uint8_t                   LVD_CTLH;
    stc_lvd_ctlh_field_t      u8LVD_CTLH;
  };
  uint16_t                    Reserved0;
  union {
    uint8_t                   LVD_STR;
    stc_lvd_str_field_t       u8LVD_STR;
  };
  uint16_t                    Reserved1;
  uint8_t                     Reserved2;
  union {
    uint8_t                   LVD_CLR;
    stc_lvd_clr_field_t       u8LVD_CLR;
  };
  uint16_t                    Reserved3;
  uint8_t                     Reserved4;
  __IO uint32_t               LVD_RLR;
  union {
    uint8_t                   LVD_STR2;
    stc_lvd_str2_field_t      u8LVD_STR2;
  };  
} stc_lvd_t;

/**
 ******************************************************************************
 ** \brief Low voltage detection interrupt voltage
 **
 ** LVD Configurations:
 ** - (A): TYPE0, 1, 2, 4, 5
 ** - (B): TYPE3, 7
 ** - (C): TYPE6
 **
 ** \note The enumerated values do not correspond with the bit patterns of
 **       SVHI for upward compatibility reasons.
 ******************************************************************************/  
typedef enum en_lvd_irq_voltage
{
  // Configuration C
  LvdIrqVoltage170 =  0 ,  ///< interrupt voltage in the vivinity of 1.70 Volts for LVD configuration (C)
  LvdIrqVoltage175 =  1 ,  ///< interrupt voltage in the vivinity of 1.75 Volts for LVD configuration (C)
  LvdIrqVoltage180 =  2 ,  ///< interrupt voltage in the vivinity of 1.80 Volts for LVD configuration (C)
  LvdIrqVoltage185 =  3 ,  ///< interrupt voltage in the vivinity of 1.85 Volts for LVD configuration (C)
  LvdIrqVoltage190 =  4 ,  ///< interrupt voltage in the vivinity of 1.90 Volts for LVD configuration (C)
  LvdIrqVoltage195 =  5 ,  ///< interrupt voltage in the vivinity of 1.95 Volts for LVD configuration (C)
  LvdIrqVoltage200 =  6 ,  ///< interrupt voltage in the vivinity of 2.00 Volts for LVD configuration (C)
  LvdIrqVoltage205 =  7 ,  ///< interrupt voltage in the vivinity of 2.05 Volts for LVD configuration (C)
  LvdIrqVoltage250 =  8 ,  ///< interrupt voltage in the vivinity of 2.50 Volts for LVD configuration (C)
  LvdIrqVoltage260 =  9 ,  ///< interrupt voltage in the vivinity of 2.60 Volts for LVD configuration (C)
  LvdIrqVoltage270 = 10 ,  ///< interrupt voltage in the vivinity of 2.70 Volts for LVD configuration (C)
  LvdIrqVoltage280 = 11 ,  ///< interrupt voltage in the vivinity of 2.80 Volts for LVD configuration (C)
  LvdIrqVoltage290 = 12 ,  ///< interrupt voltage in the vivinity of 2.90 Volts for LVD configuration (C)
  LvdIrqVoltage300 = 13 ,  ///< interrupt voltage in the vivinity of 3.00 Volts for LVD configuration (C)
  LvdIrqVoltage310 = 14 ,  ///< interrupt voltage in the vivinity of 3.10 Volts for LVD configuration (C)
  LvdIrqVoltage320 = 15 ,  ///< interrupt voltage in the vivinity of 3.20 Volts for LVD configuration (C)

  // Configuration B
  LvdIrqVoltage20 = 100 ,  ///< interrupt voltage in the vivinity of 2.0 Volts for LVD configuration (B)
  LvdIrqVoltage21 = 101 ,  ///< interrupt voltage in the vivinity of 2.1 Volts for LVD configuration (B)
  LvdIrqVoltage22 = 102 ,  ///< interrupt voltage in the vivinity of 2.2 Volts for LVD configuration (B)
  LvdIrqVoltage23 = 103 ,  ///< interrupt voltage in the vivinity of 2.3 Volts for LVD configuration (B)
  LvdIrqVoltage24 = 104 ,  ///< interrupt voltage in the vivinity of 2.4 Volts for LVD configuration (B)
  LvdIrqVoltage25 = 105 ,  ///< interrupt voltage in the vivinity of 2.5 Volts for LVD configuration (B)
  LvdIrqVoltage26 = 106 ,  ///< interrupt voltage in the vivinity of 2.6 Volts for LVD configuration (B)
  
  // Configuration A + B
  LvdIrqVoltage28 = 200 ,  ///< interrupt voltage in the vivinity of 2.8 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage30 = 201 ,  ///< interrupt voltage in the vivinity of 3.0 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage32 = 202 ,  ///< interrupt voltage in the vivinity of 3.2 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage36 = 203 ,  ///< interrupt voltage in the vivinity of 3.6 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage37 = 204 ,  ///< interrupt voltage in the vivinity of 3.7 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage40 = 205 ,  ///< interrupt voltage in the vivinity of 4.0 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage41 = 206 ,  ///< interrupt voltage in the vivinity of 4.1 Volts for LVD configuration (A)+(B)
  LvdIrqVoltage42 = 207    ///< interrupt voltage in the vivinity of 4.2 Volts for LVD configuration (A)+(B)
}
en_lvd_irq_voltage_t ;

/**
 ******************************************************************************
 ** \brief Low voltage detection reset voltage
 **
 ** LVD Configurations:
 ** - (A): TYPE0, 1, 2, 4, 5
 ** - (B): TYPE3, 7
 ** - (C): TYPE6
 **
 ** \note The enumerated values do not correspond with the bit patterns of
 **       SVHR for upward compatibility reasons.
 ******************************************************************************/  
typedef enum en_lvd_res_voltage
{
  // Configuration C
  LvdResVoltage150 =  0 , ///< reset voltage in the vivinity of 1.50 Volts for LVD configuration (C)
  LvdResVoltage155 =  1 , ///< reset voltage in the vivinity of 1.55 Volts for LVD configuration (C)
  LvdResVoltage160 =  2 , ///< reset voltage in the vivinity of 1.60 Volts for LVD configuration (C)
  LvdResVoltage165 =  3 , ///< reset voltage in the vivinity of 1.65 Volts for LVD configuration (C)
  LvdResVoltage170 =  4 , ///< reset voltage in the vivinity of 1.70 Volts for LVD configuration (C)
  LvdResVoltage175 =  5 , ///< reset voltage in the vivinity of 1.75 Volts for LVD configuration (C)
  LvdResVoltage180 =  6 , ///< reset voltage in the vivinity of 1.80 Volts for LVD configuration (C)
  LvdResVoltage185 =  7 , ///< reset voltage in the vivinity of 1.85 Volts for LVD configuration (C)
  LvdResVoltage190 =  8 , ///< reset voltage in the vivinity of 1.90 Volts for LVD configuration (C)
  LvdResVoltage195 =  9 , ///< reset voltage in the vivinity of 1.95 Volts for LVD configuration (C)
  LvdResVoltage200 = 10 , ///< reset voltage in the vivinity of 2.00 Volts for LVD configuration (C)
  LvdResVoltage205 = 11 , ///< reset voltage in the vivinity of 2.05 Volts for LVD configuration (C)
  LvdResVoltage250 = 12 , ///< reset voltage in the vivinity of 2.50 Volts for LVD configuration (C)
  LvdResVoltage260 = 13 , ///< reset voltage in the vivinity of 2.60 Volts for LVD configuration (C)
  LvdResVoltage270 = 14 , ///< reset voltage in the vivinity of 2.70 Volts for LVD configuration (C)
  LvdResVoltage280 = 15 , ///< reset voltage in the vivinity of 2.80 Volts for LVD configuration (C)
  LvdResVoltage290 = 16 , ///< reset voltage in the vivinity of 2.90 Volts for LVD configuration (C)
  LvdResVoltage300 = 17 , ///< reset voltage in the vivinity of 3.00 Volts for LVD configuration (C)
  LvdResVoltage310 = 18 , ///< reset voltage in the vivinity of 3.10 Volts for LVD configuration (C)
  LvdResVoltage320 = 19 , ///< reset voltage in the vivinity of 3.20 Volts for LVD configuration (C)
    
  // Configuration B
  LvdResVoltage153 = 100 , ///< reset voltage in the vivinity of 1.53 Volts for LVD configuration (B) 
  LvdResVoltage193 = 101   ///< reset voltage in the vivinity of 1.93 Volts for LVD configuration (B)
}
en_lvd_res_voltage_t ;

/**
 ******************************************************************************
 ** \brief Low Voltage Detection internal data
 ******************************************************************************/ 
typedef struct stc_lvd_intern_data
{
  func_ptr_t    pfnCallback ;       ///< LVD interrupt callback function
} stc_lvd_intern_data_t ;

/**
 ******************************************************************************
 ** \brief Clock Supervisor configuration
 ** 
 ** The Clock Supervisor configuration settings
 ******************************************************************************/
typedef struct stc_lvd_config
{
  en_lvd_irq_voltage_t  enIrqVoltage ;    ///< LVD interrupt voltage, see #en_lvd_irq_voltage_t for details.
#if (L3_DEVICE_TYPE == L3_TYPE3) || \
    (L3_DEVICE_TYPE == L3_TYPE6) || \
    (L3_DEVICE_TYPE == L3_TYPE7)
  en_lvd_res_voltage_t  enResVoltage ;    ///< LVD reset voltage, see #en_lvd_res_voltage_t for details.
  boolean_t             bEnableLvdReset ; ///< TRUE: Enable LVD reset, FALSE: Disable LVD reset
#endif
  func_ptr_t            pfnCallback ;     ///< LVD interrupt callback function
} stc_lvd_config_t ;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern stc_lvd_intern_data_t stcLvdInternData ; 

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

void LvdIrqHandler(void) ;

en_result_t Lvd_InitIrq(stc_lvd_config_t* pstcConfig) ;

en_result_t Lvd_DeInitIrq(void) ;

//@} // LvdGroup

#ifdef __cplusplus
}
#endif

#endif /* __LVD_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
