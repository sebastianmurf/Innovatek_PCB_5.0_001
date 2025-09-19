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
/** \file mft.h
 **
 ** Headerfile for MFT functions
 **  
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-04-20  1.1  MWi  IGBT mode for Type7 devices added; L3 address
 **                           definition for instance pointer used
 **   - 2012-06-27  1.2  MWi  Description updated
 **
 ******************************************************************************/

#ifndef __MFT_H__
#define __MFT_H__

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
 ** \defgroup MftGroup Multi Function Timer Functions (MFT)
 **
 ** A Multi Function Timer instance consists of the follwoing components:
 **
 ** - 3 FRT  (3 Free-Run Timer)*
 ** - 6 OCU  (6 Output Compare Unit Channels)*
 ** - 3 WFG  (3 Waveform Generator Units)
 ** - 3 PPG  (3 dedicated PPG Channels)
 ** - 4 ICU  (4 Input Campture Channles)
 ** - 3 ACMP (3 ADC Compare Trigger Units)
 ** - 1 NZCL (1 Noise Cancel/Emergency stop Unit)
 ** - 1 IGBT (1 IGBT module for Tpye7 devices only)
 **
 ** The components marked by * above are needed for the minimum configuration
 ** #stc_mft_frt_ocu_config_t to initialize an MFT instance. Thus the FRT
 ** and OCU have to be configured to generate an output signal on the RTxy
 ** pins. If the WFG's configuration is set to NULL, it is automatically
 ** initialized to 'through mode'. In this case the PPGs can only generate
 ** interrupts. If the PPG shall modulate the OCU signals, the WFG has to
 ** to be configured for PPG. For only using PPGs the OCU configuration has
 ** to be set to disabled OCU channels.
  
 ** Therefore in contrast to other resource drivers the MFT has more than one
 ** configuration to save configuration memory of not used components of an MFT
 ** instance.
 **
 ** The configurations for the MFT components are:
 **
 ** - #stc_mft_frt_ocu_config_t (FRT and OCU)
 ** - #stc_mft_wfg_config_t (WFG)
 ** - #stc_mft_ppg_config_t (PPG)
 ** - #stc_mft_icu_config_t (ICU)
 ** - #stc_mft_adcmp_config_t (ADCMP)
 ** - #stc_mft_nzcl_config_t (NZCL)
 ** - #stc_mft_ppg_igbt_config_t (IGBT for Type7 devices only)
 **
 ** Provided functions of MFT module:
 **  
 ** - Mft_Init()
 ** - Mft_FrtStart()
 ** - Mft_FrtStop()
 ** - Mft_FrtRead()
 ** - Mft_OcuStartStop()
 ** - Mft_IcuStartStop()
 ** - Mft_IcuRead()
 ** - Mft_PpgSwTrigger()
 ** - Mft_PpgTimerStart()
 ** - Mft_PpgTimerStop()
 ** - Mft_PpgSetValue8()
 ** - Mft_PpgSetValue16()
 ** - Mft_PpgSetValue8plus8()
 ** - Mft_DeInit()
 **
 ** Mft_Init() initializes an MFT instance. The minimum configuration, which is
 ** needed is defined by #stc_mft_frt_ocu_config_t. In this case, the MFT's
 ** corresponding OCUs are routed directly to the RTni pins, where 'n' is the
 ** number of the MFT instance and 'i' the output pin from 0 to 5. The WFG is
 ** set automatically to 'through mode' by Mft_Init(). Mft_FrtStart() starts
 ** the FRT operation together with the OCU's configuration. Note, that because
 ** of different start control bit address locations, the (maximum) three FRTs
 ** are not started synchronously! This driver starts FRT0 first, then FRT1 and
 ** at last FRT2. Same appears to Mft_FrtStop(), which stops the FRTs given in
 ** the function arguments. FRT0 ist stopped first and FRT2 at last.
 ** Mft_FrtRead() reads out the recent FRT counter value.
 ** Each FRT and OCU interrupt has an own callback function. The pointer to
 ** these (user) functions can be defined in the #stc_mft_frt_ocu_config_t
 ** configuration. If an interrupt is enabled in the configuration, the driver's
 ** ISRs are taking care of clearing the interrupt cause and calling the
 ** callback functions, if their pointers are unequal to NULL.
 **
 ** The OCU channels can be enabled by Mft_Init() or later enabled or disabled
 ** individually by Mft_OcuStartStop() with their configuration
 ** #stc_mft_frt_ocu_config_t.
 **
 ** The ICU channels are not enabled by Mft_Init(). For individually en- and
 ** disabling ICU channels by its configuration #stc_mft_icu_config_t the
 ** function Mft_IcuStartStop() is used.
 ** Mft_IcuRead() reads out the captured FRT value of an selected ICU channel
 ** and can be called in the ICU's channels corresponding callback function. 
 **
 ** This driver only configures and handles the PPG channels 0, 2, and 4 for
 ** MFT0 and channels 8, 10, 12 for MFT1, because only these channels are
 ** connected to the WFG (outputs). Defining configurations for all PPGs
 ** (for internal counting purposes only) would oversize the configuration
 ** itself and the RAM consumption.
 ** Mft_PpgSwTrigger() triggers selected PPG channels, if configured to
 ** en_mft_ppg_trigger_t#MftPpgTriggerRegister. Mft_PpgTimerStart() starts
 ** PPG channels with the up-counter delay synchronously, if configured
 ** to en_mft_ppg_trigger_t#MftPpgTimingGenerator. Mft_PpgTimerStop() stops
 ** selected PPGs.
 ** With Mft_PpgSetValue8(), Mft_PpgSetValue16(), and Mft_PpgSetValue8plus8()
 ** new pulse values can be set corresponding to the PPG mode defined in
 ** #en_mft_ppg_mode_t.
 ** Note that for generating PPG-only signals bRt[n]HighLevel of
 ** #stc_mft_frt_ocu_config_t has to be set to TRUE.
 **
 ** Mft_DeInit() deinitializes an MFT instance. Note, that also here NULL
 ** pointers to not defined configurations cause no touch of the corresponding
 ** MFT parts.
 **
 ** On Tpye7 devices the following IGBT functions are also available:
 ** - Mft_PpgIgbt_Init()
 ** - Mft_PpgIgbt_Enable()
 ** - Mft_PpgIgbt_Disable()
 ** - Mft_PpgIgbt_DeInit()
 **
 ** \note 
 **       - To save configuration memory 16 + 16 bit mode is only available if
 **         the definition L3_PPG_1616_MODE_AVAILABLE in l3_user.h is set to
 **         L3_ON.
 **
 ** \attention This driver only supports Interrupt A ISR handling for MFT!
 ******************************************************************************/
//@{
  
/******************************************************************************
 * Global definitions
 ******************************************************************************/
  
#define MFT0       (*((volatile stc_mftn_t *)     L3_MFT0_BASE))
#define MFT1       (*((volatile stc_mftn_t *)     L3_MFT1_BASE))
#define MFT2       (*((volatile stc_mftn_t *)     L3_MFT2_BASE))
#define PPG        (*((volatile stc_ppg_base_t *) L3_MFT_PPG_BASE))
#define PPG_MFT0   (*((volatile stc_ppgn_t *)     (L3_MFT_PPG_BASE + 0x200)))
#define PPG_MFT1   (*((volatile stc_ppgn_t *)     (L3_MFT_PPG_BASE + 0x280)))
#define PPG_MFT2   (*((volatile stc_ppgn_t *)     (L3_MFT_PPG_BASE + 0x300)))
  
#if (L3_DEVICE_TYPE == L3_TYPE7)
  #define PPG_IGBT   (*((volatile stc_igbtn_t *)  (L3_MFT_PPG_BASE + 0x380)))
#endif

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief Redefinition of TCSA register bitfields
 ******************************************************************************/
typedef struct stc_mft_frt_tcsa
{
  __IO  uint16_t CLK      :4;
  __IO  uint16_t SCLR     :1;
  __IO  uint16_t MODE     :1;
  __IO  uint16_t STOP     :1;
  __IO  uint16_t BFE      :1;
  __IO  uint16_t ICRE     :1;
  __IO  uint16_t ICLR     :1;
        uint16_t RESERVED :3;
  __IO  uint16_t IRQZE    :1;
  __IO  uint16_t IRQZF    :1;
  __IO  uint16_t ECKE     :1;
} stc_mft_frt_tcsa_t;

/**
 ******************************************************************************
 ** \brief Redefinition of OCFS register bitfields
 ******************************************************************************/
typedef struct stc_mft_ocu_ocfs
{
  __IO  uint8_t FSO0  :4;
  __IO  uint8_t FSO1  :4;
} stc_mft_ocu_ocfs_t;

/**
 ******************************************************************************
 ** \brief Redefinition of WFSA register bitfields
 ******************************************************************************/
typedef struct stc_mft_wfg_wfsa
{
  __IO  uint16_t DCK      :3;
  __IO  uint16_t TMD      :3;  
  __IO  uint16_t GTEN     :2; 
  __IO  uint16_t PSEL     :2; 
  __IO  uint16_t PGEN     :2; 
  __IO  uint16_t DMOD     :1;
        uint16_t RESERVED :3;
} stc_mft_wfg_wfsa_t;

/**
 ******************************************************************************
 ** \brief Redefinition of NZCL register bitfields
 ******************************************************************************/
typedef struct stc_mft_wfg_nzcl
{
  __IO  uint16_t DTIE     :1;
  __IO  uint16_t MWS      :3;
  __IO  uint16_t SDTI     :1;
        uint16_t RESERVED :11;
} stc_mft_wfg_nzcl_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ICFS register bitfields
 ******************************************************************************/
typedef struct stc_mft_icu_icfs
{
  __IO  uint8_t FSI0 :4;
  __IO  uint8_t FSI1 :4;
} stc_mft_icu_icfs_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ACSA register bitfields
 ******************************************************************************/
typedef struct stc_mft_adcmp_acsa
{
  __IO  uint16_t CE0       :2;
  __IO  uint16_t CE1       :2; 
  __IO  uint16_t CE2       :2;
        uint16_t RESERVED0 :2;
  __IO  uint16_t SEL0      :2;
  __IO  uint16_t SEL1      :2;
  __IO  uint16_t SEL2      :2;
        uint16_t RESERVED1 :2;
} stc_mft_adcmp_acsa_t;

/**
 ******************************************************************************
 ** \brief Redefinition of ATSA register bitfields
 ******************************************************************************/
typedef struct stc_mft_adcmp_atsa
{
  __IO  uint16_t AD0S      :2;
  __IO  uint16_t AD1S      :2;
  __IO  uint16_t AD2S      :2;
        uint16_t RESERVED0 :2;  
  __IO  uint16_t AD0P      :2;
  __IO  uint16_t AD1P      :2;
  __IO  uint16_t AD2P      :2;
        uint16_t RESERVED1 :2;        
} stc_mft_adcmp_atsa_t;

#if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Redefinition of IGBTC register bitfields
 ******************************************************************************/
typedef struct stc_ppg_igbtc
{
  __IO  uint8_t IGBTMD      :1;
  __IO  uint8_t IGTRGLV     :1;
  __IO  uint8_t IGOSEL0     :1;
  __IO  uint8_t IGOSEL1     :1; 
  __IO  uint8_t IGNFW       :3;
  __IO  uint8_t IGATIH      :1;
} stc_ppg_igbtc_t;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Redefinition of MFT register structures for unification of all 
 **        components
 ******************************************************************************/
typedef struct stc_mftn
{
  __IO uint16_t OCCP0;
        uint8_t RESERVED0[2];
  __IO uint16_t OCCP1;
        uint8_t RESERVED1[2];
  __IO uint16_t OCCP2;
        uint8_t RESERVED2[2];
  __IO uint16_t OCCP3;
        uint8_t RESERVED3[2];
  __IO uint16_t OCCP4;
        uint8_t RESERVED4[2];
  __IO uint16_t OCCP5;
        uint8_t RESERVED5[2];
  union {
    __IO  uint8_t OCSA10;
    stc_mft_ocu_ocsa10_field_t u8OCSA10;
  };
  union {
    __IO  uint8_t OCSB10;
    stc_mft_ocu_ocsb10_field_t u8OCSB10;
  };
        uint8_t RESERVED6[2];
  union {
    __IO  uint8_t OCSA32;
    stc_mft_ocu_ocsa32_field_t u8OCSA32;
  };
  union {
    __IO  uint8_t OCSB32;
    stc_mft_ocu_ocsb32_field_t u8OCSB32;
  };
        uint8_t RESERVED7[2];
  union {
    __IO  uint8_t OCSA54;
    stc_mft_ocu_ocsa54_field_t u8OCSA54;
  };
  union {
    __IO  uint8_t OCSB54;
    stc_mft_ocu_ocsb54_field_t u8OCSB54;
  };
        uint8_t RESERVED8[3];
  union {
    __IO  uint8_t OCSC;
    stc_mft_ocu_ocsc_field_t u8OCSC;
  };
        uint8_t RESERVED9[2];
  __IO uint16_t TCCP0;
        uint8_t RESERVED10[2];
  __IO uint16_t TCDT0;
        uint8_t RESERVED11[2];       
  union {
    __IO uint16_t TCSA0;
    stc_mft_frt_tcsa_t u16TCSA0;
  };
        uint8_t RESERVED12[2];
  union {
    __IO uint16_t TCSB0;
    stc_mft_frt_tcsb0_field_t u16TCSB0;
  };
        uint8_t RESERVED13[2];
  __IO uint16_t TCCP1;
        uint8_t RESERVED14[2];
  __IO uint16_t TCDT1;
        uint8_t RESERVED15[2];
  union {
    __IO uint16_t TCSA1;
    stc_mft_frt_tcsa_t u16TCSA1;
  };
        uint8_t RESERVED16[2];
  union {
    __IO uint16_t TCSB1;
    stc_mft_frt_tcsb1_field_t u16TCSB1;
  };
        uint8_t RESERVED17[2];
  __IO uint16_t TCCP2;
        uint8_t RESERVED18[2];
  __IO uint16_t TCDT2;
        uint8_t RESERVED19[2];
  union {
    __IO uint16_t TCSA2;
    stc_mft_frt_tcsa_t u16TCSA2;
  };
        uint8_t RESERVED20[2];
  union {
    __IO uint16_t TCSB2;
    stc_mft_frt_tcsb2_field_t u16TCSB2;
  };        
        uint8_t RESERVED21[2]; 
  union {
    __IO  uint8_t OCFS10;
    stc_mft_ocu_ocfs_t u8OCFS10;
  };
  union {
    __IO  uint8_t OCFS32;
    stc_mft_ocu_ocfs_t u8OCFS32;
  };
        uint8_t RESERVED22[2];
  union {
    __IO  uint8_t OCFS54;
    stc_mft_ocu_ocfs_t u8OCFS54;
  };
        uint8_t RESERVED23[2];
  union {
    __IO  uint8_t ICFS10;
    stc_mft_icu_icfs_t u8ICFS10;
  };
  union {
    __IO  uint8_t ICFS32;
    stc_mft_icu_icfs_t u8ICFS32;
  };
        uint8_t RESERVED24[6];
  __IO uint16_t ICCP0;
        uint8_t RESERVED25[2];
  __IO uint16_t ICCP1;
        uint8_t RESERVED26[2];
  __IO uint16_t ICCP2;
        uint8_t RESERVED27[2];
  __IO uint16_t ICCP3;
        uint8_t RESERVED28[2];
  union {
    __IO  uint8_t ICSA10;
    stc_mft_icu_icsa10_field_t u8ICSA10;
  };
  union {
    __IO  uint8_t ICSB10;
    stc_mft_icu_icsb10_field_t u8ICSB10;
  };
        uint8_t RESERVED29[2];
  union {
    __IO  uint8_t ICSA32;
    stc_mft_icu_icsa32_field_t u8ICSA32;
  };
  union {
    __IO  uint8_t ICSB32;
    stc_mft_icu_icsb32_field_t u8ICSB32;
  };  
        uint8_t RESERVED30[2];
  __IO uint16_t WFTM10;
        uint8_t RESERVED31[2];
  __IO uint16_t WFTM32;
        uint8_t RESERVED32[2];
  __IO uint16_t WFTM54;
        uint8_t RESERVED33[2];
  union {
    __IO uint16_t WFSA10;
    stc_mft_wfg_wfsa_t u16WFSA10;
  };
        uint8_t RESERVED34[2];
  union {
    __IO uint16_t WFSA32;
    stc_mft_wfg_wfsa_t u16WFSA32;
  };
        uint8_t RESERVED35[2];
  union {
    __IO uint16_t WFSA54;
    stc_mft_wfg_wfsa_t u16WFSA54;
  };
        uint8_t RESERVED36[2];
  union {
    __IO uint16_t WFIR;
    stc_mft_wfg_wfir_field_t u16WFIR;
  };
        uint8_t RESERVED37[2];
  union {
    __IO uint16_t NZCL;
    stc_mft_wfg_nzcl_t u16NZCL;
  };
        uint8_t RESERVED38[2];
  __IO uint16_t ACCP0;
        uint8_t RESERVED39[2];
  __IO uint16_t ACCPDN0;
        uint8_t RESERVED40[2];
  __IO uint16_t ACCP1;
        uint8_t RESERVED41[2];
  __IO uint16_t ACCPDN1;
        uint8_t RESERVED42[2];
  __IO uint16_t ACCP2;
        uint8_t RESERVED43[2];
  __IO uint16_t ACCPDN2;
        uint8_t RESERVED44[2];
  union {
    __IO  uint8_t ACSB;
    stc_mft_adcmp_acsb_field_t u8ACSB;
  };
        uint8_t RESERVED45[3];
  union {
    __IO uint16_t ACSA;
    stc_mft_adcmp_acsa_t u16ACSA;
  };
        uint8_t RESERVED46[2];
  union {
    __IO uint16_t ATSA;
    stc_mft_adcmp_atsa_t u16ATSA;
  };  
} stc_mftn_t;

/**
 ******************************************************************************
 ** \brief Redefinition of PPG base register structures for unification of all 
 **        components
 ******************************************************************************/
typedef struct stc_ppg_base
{
        uint8_t RESERVED0;
  union {
    __IO  uint8_t TTCR0;
    stc_mft_ppg_ttcr0_field_t u8TTCR0;
  };
        uint8_t RESERVED1[7];
  __IO  uint8_t COMP0;
        uint8_t RESERVED2[2];
  __IO  uint8_t COMP2;
        uint8_t RESERVED3[4];
  __IO  uint8_t COMP4;
        uint8_t RESERVED4[2];
  __IO  uint8_t COMP6;
        uint8_t RESERVED5[12];
  union {
    __IO  uint8_t TTCR1;
    stc_mft_ppg_ttcr0_field_t u8TTCR1;
  };
        uint8_t RESERVED6[7];
  __IO  uint8_t COMP1;
        uint8_t RESERVED7[2];
  __IO  uint8_t COMP3;
        uint8_t RESERVED8[4];
  __IO  uint8_t COMP5;
        uint8_t RESERVED9[2];
  __IO  uint8_t COMP7;
        uint8_t RESERVED10[203];
  union {
    __IO uint16_t TRG0;
    stc_mft_ppg_trg_field_t u16TRG;
  };
        uint8_t RESERVED11[2];
  union {
    __IO uint16_t REVC0;
    stc_mft_ppg_revc_field_t u16REVC0;
  };
} stc_ppg_base_t;

/**
 ******************************************************************************
 ** \brief Redefinition of PPG registers structures for unification of all 
 **        components
 ******************************************************************************/
typedef struct stc_ppgn
{
    union {
    __IO  uint8_t PPGC1;
    stc_mft_ppg_ppgc1_field_t u8PPGC1;
  };
  union {
    __IO  uint8_t PPGC0;
    stc_mft_ppg_ppgc0_field_t u8PPGC0;
  };
        uint8_t RESERVED0[2];
  union {
    __IO  uint8_t PPGC3;
    stc_mft_ppg_ppgc3_field_t u8PPGC3;
  };
  union {
    __IO  uint8_t PPGC2;
    stc_mft_ppg_ppgc2_field_t u8PPGC2;
  };
        uint8_t RESERVED1[2];
  union {
    __IO uint16_t PRL0;
    struct {
      __IO  uint8_t PRLL0;
      __IO  uint8_t PRLH0;
    };
  };
        uint8_t RESERVED2[2];
  union {
    __IO uint16_t PRL1;
    struct {
      __IO  uint8_t PRLL1;
      __IO  uint8_t PRLH1;
    };
  };
        uint8_t RESERVED3[2];
  union {
    __IO uint16_t PRL2;
    struct {
      __IO  uint8_t PRLL2;
      __IO  uint8_t PRLH2;
    };
  };
        uint8_t RESERVED4[2];
  union {
    __IO uint16_t PRL3;
    struct {
      __IO  uint8_t PRLL3;
      __IO  uint8_t PRLH3;
    };
  };
        uint8_t RESERVED5[2];
  union {
    __IO  uint8_t GATEC0;
    stc_mft_ppg_gatec0_field_t u8GATEC0;
  };
  
        uint8_t RESERVED6[39];
  union {
    __IO  uint8_t PPGC5;
    stc_mft_ppg_ppgc5_field_t u8PPGC5;
  };
  union {
    __IO  uint8_t PPGC4;
    stc_mft_ppg_ppgc4_field_t u8PPGC4;
  };
        uint8_t RESERVED7[2];
  union {
    __IO  uint8_t PPGC7;
    stc_mft_ppg_ppgc7_field_t u8PPGC7;
  };
  union {
    __IO  uint8_t PPGC6;
    stc_mft_ppg_ppgc6_field_t u8PPGC6;
  };
        uint8_t RESERVED8[2];
  union {
    __IO uint16_t PRL4;
    struct {
      __IO  uint8_t PRLL4;
      __IO  uint8_t PRLH4;
    };
  };
        uint8_t RESERVED9[2];
  union {
    __IO uint16_t PRL5;
    struct {
      __IO  uint8_t PRLL5;
      __IO  uint8_t PRLH5;
    };
  };
        uint8_t RESERVED10[2];
  union {
    __IO uint16_t PRL6;
    struct {
      __IO  uint8_t PRLL6;
      __IO  uint8_t PRLH6;
    };
  };
        uint8_t RESERVED11[2];
  union {
    __IO uint16_t PRL7;
    struct {
      __IO  uint8_t PRLL7;
      __IO  uint8_t PRLH7;
    };
  };
        uint8_t RESERVED12[2];
  union {
    __IO  uint8_t GATEC4;
    stc_mft_ppg_gatec4_field_t u8GATEC4;
  };
} stc_ppgn_t;


#if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Redefinition of PPG IGBTC register structure for unification of all 
 **        components
 ******************************************************************************/
typedef struct stc_igbtn_t
{
  union {
    __IO  uint8_t   IGBTC;
    stc_ppg_igbtc_t u8IGBTC;
  };
} stc_igbtn_t;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mft Clock Setting
 ******************************************************************************/
typedef enum en_mft_clock
{
  MftPclk1   = 0,    ///< FRT count clock cycle: PCLK
  MftPclk2   = 1,    ///< FRT count clock cycle: PCLK * 2
  MftPclk4   = 2,    ///< FRT count clock cycle: PCLK * 4
  MftPclk8   = 3,    ///< FRT count clock cycle: PCLK * 8
  MftPclk16  = 4,    ///< FRT count clock cycle: PCLK * 16
  MftPclk32  = 5,    ///< FRT count clock cycle: PCLK * 32
  MftPclk64  = 6,    ///< FRT count clock cycle: PCLK * 64
  MftPclk128 = 7,    ///< FRT count clock cycle: PCLK * 128
  MftPclk256 = 8     ///< FRT count clock cycle: PCLK * 256
} en_mft_clock_t;

/**
 ******************************************************************************
 ** \brief Mft count mode
 ******************************************************************************/
typedef enum en_mft_mode
{
  MftUpCount     = 0,    ///< FRT up-count mode
  MftUpDownCount = 1     ///< FRT up-/down-count mode
} en_mft_mode_t;

/**
 ******************************************************************************
 ** \brief Mft Ocu connection setting
 ******************************************************************************/
typedef enum en_mft_ocu_connect
{
  MftFrt0Ocu    = 0,    ///< Connects FRT0 to OCUn
  MftFrt1Ocu    = 1,    ///< Connects FRT1 to OCUn
  MftFrt2Ocu    = 2,    ///< Connects FRT2 to OCUn
  MftOcuFrtMft1 = 3,    ///< Connects FRT of external MFT1
  MftOcuFrtMft2 = 4     ///< Connects FRT of external MFT2
} en_mft_ocu_connect_t;

/**
 ******************************************************************************
 ** \brief Mft Wfg clock setting
 ******************************************************************************/
typedef enum en_mft_wfg_clock
{
  MftWfgPclk1   = 0,    ///< WFG count clock cycle: PCLK
  MftWfgPclk2   = 1,    ///< WFG count clock cycle: PCLK * 2
  MftWfgPclk4   = 2,    ///< WFG count clock cycle: PCLK * 4
  MftWfgPclk8   = 3,    ///< WFG count clock cycle: PCLK * 8
  MftWfgPclk16  = 4,    ///< WFG count clock cycle: PCLK * 16
  MftWfgPclk32  = 5,    ///< WFG count clock cycle: PCLK * 32
  MftWfgPclk64  = 6     ///< WFG count clock cycle: PCLK * 64
} en_mft_wfg_clock_t;

/**
 ******************************************************************************
 ** \brief Mft Wfg operation mode
 ******************************************************************************/
typedef enum en_mft_wfg_mode
{
  MftWfgThroughMode      = 0,    ///< WFG operation mode: Through Mode
  MftWfgRtPpgMode        = 1,    ///< WFG operation mode: RT-PPG Mode
  MftWfgTimerPpgMode     = 2,    ///< WFG operation mode: Timer-PPG Mode
  MftWfgRtDeadTimerMode  = 3,    ///< WFG operation mode: RT-Dead Timer Mode
  MftWfgPpgDeadTimerMode = 4     ///< WFG operation mode: PPG-Deat Timer Mode
} en_mft_wfg_mode_t;

/**
 ******************************************************************************
 ** \brief Mft Wfg Gate Signal setting
 ******************************************************************************/
typedef enum en_mft_wfg_gate
{
  MftWfgLowLevel   = 0,  ///< Outputs Low-level signals
  MftWfgRt0Flag0   = 1,  ///< Outputs RT(0) in PT-PPG, PPG-DT mode; Timer active Flag0 in Timer-PPG mode
  MftWfgRt1Flag1   = 2,  ///< Outputs RT(1) in PT-PPG, PPG-DT mode; Timer active Flag1 in Timer-PPG mode
  MftWfgRt10Flag10 = 3   ///< Or'ed signal from MftWfgRt0Flag0 and MftWfgRt1Flag1 mode
} en_mft_wfg_gate_t;

/**
 ******************************************************************************
 ** \brief Mft Wfg Ppg selection
 ******************************************************************************/
typedef enum en_mft_wfg_psel
{
  MftWfgGatePpg0 = 0,    ///< GATE PPG0 enable
  MftWfgGatePpg2 = 1,    ///< GATE PPG2 enable
  MftWfgGatePpg4 = 2     ///< GATE PPG4 enable
} en_mft_wfg_psel_t;

/**
 ******************************************************************************
 ** \brief Mft Wfg RT/PPG output selection
 ******************************************************************************/
typedef enum en_mft_wfg_pgen
{
  MftWfgRt1Rt0 = 0,    ///< RTO1 = RT1,  RTO0 = RT0
  MftWfgRt1Ppg = 1,    ///< RTO1 = RT1,  RTO0 = PPGn
  MftWfgPpgRt0 = 2,    ///< RTO1 = PPGm, RTO0 = RT0
  MftWfgPpgPpg = 3     ///< RTO1 = PPGm, RTO0 = PPGn
} en_mft_wfg_pgen_t;

/**
 ******************************************************************************
 ** \brief Mft Noise Canceler Timing
 ******************************************************************************/
typedef enum en_mft_nzcl_time
{
  MftNzclNoNoiseCancel     = 0, ///< Direct DTTIX pin connection w/o noise canceling
  MftNzclNoiseCancel4Pclk  = 1, ///< Noise canceling with 4 PCLK cycles
  MftNzclNoiseCancel8Pclk  = 2, ///< Noise canceling with 8 PCLK cycles
  MftNzclNoiseCancel16Pclk = 3, ///< Noise canceling with 16 PCLK cycles
  MftNzclNoiseCancel32Pclk = 4  ///< Noise canceling with 32 PCLK cycles
} en_mft_nzcl_time_t;

/**
 ******************************************************************************
 ** \brief Mft Icu Connection
 ******************************************************************************/
typedef enum en_mft_icu_connect
{
  MftIcuFrt0     = 0, ///< Cenncets FRT0 to ICU0/1
  MftIcuFrt1     = 1, ///< Cenncets FRT1 to ICU0/1
  MftIcuFrt2     = 2, ///< Cenncets FRT2 to ICU0/1
  MftIcuFrtMft1  = 3, ///< Cenncets FRT0 to external MFT(1)
  MftIcuFrtMft2  = 4  ///< Cenncets FRT0 to external MFT(2)
} en_mft_icu_connect_t;

/**
 ******************************************************************************
 ** \brief Mft Icu Edge selection
 ******************************************************************************/
typedef enum en_mft_icu_edge
{
  MftIcuDisable     = 0, ///< Disables ICU
  MftIcuRisingEdge  = 1, ///< Sets ICU rising edge detection
  MftIcuFallingEdge = 2, ///< Sets ICU falling edge detection
  MftIcuBothEdges   = 3  ///< Sets ICU rising and falling edge detection
} en_mft_icu_edge_t;

/**
 ******************************************************************************
 ** \brief Mft Adc Compare setting
 ******************************************************************************/
typedef enum en_mft_adc_cmp
{
  MftAdcCmpDisable  = 0, ///< Disables ADCMPn
  MftAdcCmpFrt0     = 1, ///< Enables ADCMPn for FRT0
  MftAdcCmpFrt1     = 2, ///< Enables ADCMPn for FRT1
  MftAdcCmpFrt2     = 3  ///< Enables ADCMPn for FRT2
} en_mft_adc_cmp_t;

/**
 ******************************************************************************
 ** \brief Mft Adc Compare count setting
 ******************************************************************************/
typedef enum en_mft_adc_cmp_count
{
  MftAdcCmpUpPeakDown  = 0, ///< ADC start, if FRT is in Up-count/Peak/Down-Count state and ACCP match.
  MftAdcCmpUp          = 1, ///< ADC start, if FRT is in Up-count state and ACCP match.
  MftAdcCmpPeakDown    = 2, ///< ADC start, if FRT is in Peak/Down-count state and ACCP match.
  MftAdcCmpUpPeakDown2 = 3  ///< ADC start, if FRT is in Up-count state and ACCP match or Peak/Down and ACCPDN
} en_mft_adc_cmp_count_t;


/**
 ******************************************************************************
 ** \brief Mft Adc Compare start trigger signal
 ******************************************************************************/
typedef enum en_mft_adc_cmp_start
{
  MftAdcCmpAdcmp  = 0, ///< ADC start on ADCMP signal
  MftAdcCmpFrt02  = 1  ///< ADC start on ored FRT0/2
} en_mft_adc_cmp_start_t;

/**
 ******************************************************************************
 ** \brief Mft Adc Compare Priority start trigger signal
 ******************************************************************************/
typedef enum en_mft_adc_cmp_start_prio
{
  MftAdcCmpAdcmpPrio  = 0, ///< ADC priority scan start on ADCMP signal
  MftAdcCmpFrt02Prio  = 1  ///< ADC priority scan start on ored FRT0/2
} en_mft_adc_cmp_start_prio_t;

/**
 ******************************************************************************
 ** \brief Mft PPG prescaler clock settings
 ******************************************************************************/
typedef enum en_mft_ppg_clock
{
  MftPpgPclk2  = 0,  ///< Up count clock: PCLK / 2
  MftPpgPclk8  = 1,  ///< Up count clock: PCLK / 8
  MftPpgPclk32 = 2,  ///< Up count clock: PCLK / 32
  MftPpgPclk64 = 3   ///< Up count clock: PCLK / 64
} en_mft_ppg_clock_t;

/**
 ******************************************************************************
 ** \brief Mft PPG mode settings
 ******************************************************************************/
typedef enum en_mft_ppg_mode
{
  MftPpg8Bit        = 0,  ///< 8 Bit mode
  MftPpg8plus8Bit   = 1,  ///< 8 + 8 Bit mode
  MftPpg16Bit       = 2,  ///< 16 Bit mode
  MftPpg16plus16Bit = 3   ///< 16 + 16 Bit mode
} en_mft_ppg_mode_t;

/**
 ******************************************************************************
 ** \brief Mft PPG trigger mode
 ******************************************************************************/
typedef enum en_mft_ppg_trigger
{
  MftPpgTimingGenerator = 0, ///< Trigger by Timing Generator Circuit (Start delay)
  MftPpgGateSignal      = 1, ///< Trigger by GATE signals of MFT
  MftPpgTriggerRegister = 2  ///< Trigger by TRG register writing (SW trigger)
} en_mft_ppg_trigger_t;

#if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mft PPG IGBT noise filter mode
 ******************************************************************************/
typedef enum en_mft_ppg_igbt_noisefilter
{
  MftPpgIgbtNoFilter      = 0, ///< No noisef ilter operation
  MftPpgIgbt4PclkFilter   = 1, ///< Noise filter with 4 PCLK cycles width
  MftPpgIgbt8PclkFilter   = 2, ///< Noise filter with 8 PCLK cycles width
  MftPpgIgbt16PclkFilter  = 3, ///< Noise filter with 16 PCLK cycles width
  MftPpgIgbt32PclkFilter  = 4, ///< Noise filter with 32 PCLK cycles width
} en_mft_ppg_igbt_noisefilter_t;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mft Free-Running-Timer (Frt) and Output-Compare-Unit (OCU)
 ** configuration.
 **
 ** Contains all parameters for the FRT and OCU configuration of an MFT
 ** instance.  
 ******************************************************************************/
typedef struct stc_mft_frt_ocu_config
{
  // FRT settings
  en_mft_clock_t       enClock0;                   ///< See #en_mft_clock_t for details
  en_mft_clock_t       enClock1;                   ///< See #en_mft_clock_t for details
  en_mft_clock_t       enClock2;                   ///< See #en_mft_clock_t for details
  en_mft_mode_t        enMode0;                    ///< See #en_mft_mode_t for details
  en_mft_mode_t        enMode1;                    ///< See #en_mft_mode_t for details
  en_mft_mode_t        enMode2;                    ///< See #en_mft_mode_t for details
  boolean_t            bTccp0BufferEnable;         ///< TRUE: Enables TCCP0 buffering
  boolean_t            bTccp1BufferEnable;         ///< TRUE: Enables TCCP1 buffering
  boolean_t            bTccp2BufferEnable;         ///< TRUE: Enables TCCP2 buffering
  boolean_t            bTcsa0IclrInterruptEnable;  ///< TRUE: Enables interrupt generation, if '1' is set to TCSA0.ICLR
  boolean_t            bTcsa1IclrInterruptEnable;  ///< TRUE: Enables interrupt generation, if '1' is set to TCSA1.ICLR
  boolean_t            bTcsa2IclrInterruptEnable;  ///< TRUE: Enables interrupt generation, if '1' is set to TCSA2.ICLR
  boolean_t            bTcsa0IrqzfInterruptEnable; ///< TRUE: Enables interrupt generation, if '1' is set to TCSA0.IRQZF
  boolean_t            bTcsa1IrqzfInterruptEnable; ///< TRUE: Enables interrupt generation, if '1' is set to TCSA1.IRQZF
  boolean_t            bTcsa2IrqzfInterruptEnable; ///< TRUE: Enables interrupt generation, if '1' is set to TCSA2.IRQZF
  boolean_t            bExternalClock0Enable;      ///< TRUE: Enables external clock (FRCK0 input)
  boolean_t            bExternalClock1Enable;      ///< TRUE: Enables external clock (FRCK1 input)
  boolean_t            bExternalClock2Enable;      ///< TRUE: Enables external clock (FRCK2 input)
  uint16_t             u16Frt0Cycle;               ///< FRT Cycle setting
  uint16_t             u16Frt1Cycle;               ///< FRT Cycle setting
  uint16_t             u16Frt2Cycle;               ///< FRT Cycle setting
  en_mft_ocu_connect_t enFrtOcu0Connection;        ///< Connects FRT0/1/2 to OCU0
  en_mft_ocu_connect_t enFrtOcu1Connection;        ///< Connects FRT0/1/2 to OCU1   
  en_mft_ocu_connect_t enFrtOcu2Connection;        ///< Connects FRT0/1/2 to OCU2
  en_mft_ocu_connect_t enFrtOcu3Connection;        ///< Connects FRT0/1/2 to OCU3
  en_mft_ocu_connect_t enFrtOcu4Connection;        ///< Connects FRT0/1/2 to OCU4
  en_mft_ocu_connect_t enFrtOcu5Connection;        ///< Connects FRT0/1/2 to OCU5
  // OCU settings
  boolean_t   bOcu0Enable;                    ///< TRUE: Enables operation of OCU0
  boolean_t   bOcu1Enable;                    ///< TRUE: Enables operation of OCU1
  boolean_t   bOcu2Enable;                    ///< TRUE: Enables operation of OCU2
  boolean_t   bOcu3Enable;                    ///< TRUE: Enables operation of OCU3
  boolean_t   bOcu4Enable;                    ///< TRUE: Enables operation of OCU4
  boolean_t   bOcu5Enable;                    ///< TRUE: Enables operation of OCU5
  boolean_t   bOccp0BufferEnable;             ///< TRUE: Enables buffering of OCCP0
  boolean_t   bOccp1BufferEnable;             ///< TRUE: Enables buffering of OCCP1
  boolean_t   bOccp2BufferEnable;             ///< TRUE: Enables buffering of OCCP2
  boolean_t   bOccp3BufferEnable;             ///< TRUE: Enables buffering of OCCP3
  boolean_t   bOccp4BufferEnable;             ///< TRUE: Enables buffering of OCCP4
  boolean_t   bOccp5BufferEnable;             ///< TRUE: Enables buffering of OCCP5
  boolean_t   bOcsaIop0InterruptEnable;       ///< TRUE: Enables interrupt generation, if '1' is set to OCSA.IOP0
  boolean_t   bOcsaIop1InterruptEnable;       ///< TRUE: Enables interrupt generation, if '1' is set to OCSA.IOP1
  boolean_t   bOcsaIop2InterruptEnable;       ///< TRUE: Enables interrupt generation, if '1' is set to OCSA.IOP2
  boolean_t   bOcsaIop3InterruptEnable;       ///< TRUE: Enables interrupt generation, if '1' is set to OCSA.IOP3
  boolean_t   bOcsaIop4InterruptEnable;       ///< TRUE: Enables interrupt generation, if '1' is set to OCSA.IOP4
  boolean_t   bOcsaIop5InterruptEnable;       ///< TRUE: Enables interrupt generation, if '1' is set to OCSA.IOP5
  boolean_t   bRt0HighLevel;                  ///< TRUE: Sets RT0 to high level (only if OCSA.CST0 == 0)
  boolean_t   bRt1HighLevel;                  ///< TRUE: Sets RT1 to high level (only if OCSA.CST1 == 0)
  boolean_t   bRt2HighLevel;                  ///< TRUE: Sets RT2 to high level (only if OCSA.CST2 == 0)
  boolean_t   bRt3HighLevel;                  ///< TRUE: Sets RT3 to high level (only if OCSA.CST3 == 0)
  boolean_t   bRt4HighLevel;                  ///< TRUE: Sets RT4 to high level (only if OCSA.CST4 == 0)
  boolean_t   bRt5HighLevel;                  ///< TRUE: Sets RT5 to high level (only if OCSA.CST5 == 0)
  boolean_t   bCmod01;                        ///< TRUE: Sets common mode of RT0 and RT1
  boolean_t   bCmod23;                        ///< TRUE: Sets common mode of RT2 and RT3
  boolean_t   bCmod45;                        ///< TRUE: Sets common mode of RT4 and RT5
  boolean_t   bOccp0BufTransferPeakDetection; ///< TRUE: Performs buffer transfer of OCCP0 upon peak value detection,
                                              ///< FALSE: Performs buffer transfer of OCCP0 upon zero value detection
  boolean_t   bOccp1BufTransferPeakDetection; ///< TRUE: Performs buffer transfer of OCCP1 upon peak value detection,
                                              ///< FALSE: Performs buffer transfer of OCCP1 upon zero value detection
  boolean_t   bOccp2BufTransferPeakDetection; ///< TRUE: Performs buffer transfer of OCCP2 upon peak value detection,
                                              ///< FALSE: Performs buffer transfer of OCCP2 upon zero value detection
  boolean_t   bOccp3BufTransferPeakDetection; ///< TRUE: Performs buffer transfer of OCCP3 upon peak value detection,
                                              ///< FALSE: Performs buffer transfer of OCCP3 upon zero value detection
  boolean_t   bOccp4BufTransferPeakDetection; ///< TRUE: Performs buffer transfer of OCCP4 upon peak value detection,
                                              ///< FALSE: Performs buffer transfer of OCCP4 upon zero value detection
  boolean_t   bOccp5BufTransferPeakDetection; ///< TRUE: Performs buffer transfer of OCCP5 upon peak value detection,
                                              ///< FALSE: Performs buffer transfer of OCCP5 upon zero value detection  
  boolean_t   bMod0;                          ///< TRUE: Determines operation mode of OCU0 in combination with #bCmod01
  boolean_t   bMod1;                          ///< TRUE: Determines operation mode of OCU1 in combination with #bCmod01  
  boolean_t   bMod2;                          ///< TRUE: Determines operation mode of OCU2 in combination with #bCmod23
  boolean_t   bMod3;                          ///< TRUE: Determines operation mode of OCU3 in combination with #bCmod23 
  boolean_t   bMod4;                          ///< TRUE: Determines operation mode of OCU4 in combination with #bCmod45
  boolean_t   bMod5;                          ///< TRUE: Determines operation mode of OCU5 in combination with #bCmod45 
  uint16_t    u16Ocu0CompareValue;            ///< Sets the OCU0 compare value
  uint16_t    u16Ocu1CompareValue;            ///< Sets the OCU1 compare value
  uint16_t    u16Ocu2CompareValue;            ///< Sets the OCU2 compare value
  uint16_t    u16Ocu3CompareValue;            ///< Sets the OCU3 compare value
  uint16_t    u16Ocu4CompareValue;            ///< Sets the OCU4 compare value
  uint16_t    u16Ocu5CompareValue;            ///< Sets the OCU5 compare value
  func_ptr_t  pfnFrt0PeakCallback;            ///< Callback function pointer of FRT0 peak detection interrupt 
  func_ptr_t  pfnFrt1PeakCallback;            ///< Callback function pointer of FRT1 peak detection interrupt 
  func_ptr_t  pfnFrt2PeakCallback;            ///< Callback function pointer of FRT2 peak detection interrupt 
  func_ptr_t  pfnFrt0ZeroCallback;            ///< Callback function pointer of FRT0 zero detection interrupt 
  func_ptr_t  pfnFrt1ZeroCallback;            ///< Callback function pointer of FRT1 zero detection interrupt 
  func_ptr_t  pfnFrt2ZeroCallback;            ///< Callback function pointer of FRT2 zero detection interrupt 
  func_ptr_t  pfnOcu0Callback;                ///< Callback function pointer of OCU0 interrupt 
  func_ptr_t  pfnOcu1Callback;                ///< Callback function pointer of OCU1 interrupt 
  func_ptr_t  pfnOcu2Callback;                ///< Callback function pointer of OCU2 interrupt 
  func_ptr_t  pfnOcu3Callback;                ///< Callback function pointer of OCU3 interrupt 
  func_ptr_t  pfnOcu4Callback;                ///< Callback function pointer of OCU4 interrupt 
  func_ptr_t  pfnOcu5Callback;                ///< Callback function pointer of OCU5 interrupt 
} stc_mft_frt_ocu_config_t;

/**
 ******************************************************************************
 ** \brief Mft Wave-Form-Generator (WFG) configuration.
 **
 ** Contains all parameters for the OCU configuration of an MFT instance.  
 ******************************************************************************/
typedef struct stc_mft_wfg_config
{
  en_mft_wfg_clock_t   enWfgClock01;              ///< See #en_mft_wfg_clock_t for details
  en_mft_wfg_clock_t   enWfgClock23;              ///< See #en_mft_wfg_clock_t for details
  en_mft_wfg_clock_t   enWfgClock45;              ///< See #en_mft_wfg_clock_t for details
  en_mft_wfg_mode_t    enWfgMode01;               ///< See #en_mft_wfg_mode_t for details
  en_mft_wfg_mode_t    enWfgMode23;               ///< See #en_mft_wfg_mode_t for details
  en_mft_wfg_mode_t    enWfgMode45;               ///< See #en_mft_wfg_mode_t for details
  en_mft_wfg_gate_t    enWfgGate01;               ///< See #en_mft_wfg_gate_t for details
  en_mft_wfg_gate_t    enWfgGate23;               ///< See #en_mft_wfg_gate_t for details
  en_mft_wfg_gate_t    enWfgGate45;               ///< See #en_mft_wfg_gate_t for details
  en_mft_wfg_psel_t    enWfgPpgSel01;             ///< See #en_mft_wfg_psel_t for details
  en_mft_wfg_psel_t    enWfgPpgSel23;             ///< See #en_mft_wfg_psel_t for details
  en_mft_wfg_psel_t    enWfgPpgSel45;             ///< See #en_mft_wfg_psel_t for details  
  en_mft_wfg_pgen_t    enWfgPpgEnable01;          ///< See #en_mft_wfg_pgen_t for details
  en_mft_wfg_pgen_t    enWfgPpgEnable23;          ///< See #en_mft_wfg_pgen_t for details
  en_mft_wfg_pgen_t    enWfgPpgEnable45;          ///< See #en_mft_wfg_pgen_t for details  
  boolean_t            bNonOverlapHighPolarity0;  ///< TRUE: Sets ouput of the non-overlap to active high polarity FTR0
  boolean_t            bNonOverlapHighPolarity1;  ///< TRUE: Sets ouput of the non-overlap to active high polarity FTR1
  boolean_t            bNonOverlapHighPolarity2;  ///< TRUE: Sets ouput of the non-overlap to active high polarity FTR2
  uint16_t             u16Wfg01TimerValue;        ///< Sets WFG 0/1 timer value
  uint16_t             u16Wfg23TimerValue;        ///< Sets WFG 2/3 timer value
  uint16_t             u16Wfg45TimerValue;        ///< Sets WFG 4/5 timer value
  boolean_t            bWfgInterruptEnable;       ///< TRUE: Enable WFG interrupts (handled only by NVIC_EnableIRQ())
  func_ptr_t           pfnWfg01Callback;          ///< Callback function pointer of WFG 0/1 interrupt
  func_ptr_t           pfnWfg23Callback;          ///< Callback function pointer of WFG 2/3 interrupt
  func_ptr_t           pfnWfg45Callback;          ///< Callback function pointer of WFG 4/5 interrupt
} stc_mft_wfg_config_t;

/**
 ******************************************************************************
 ** \brief Mft Noise-Cancler (NZCL) configuration.
 **
 ** Contains all parameters for the NZCL configuration of an MFT instance.  
 ******************************************************************************/
typedef struct stc_mft_nzcl_config
{
  boolean_t            bDtifInterruptEnable; ///< TRUE: Enables DTIF interrupt from DTTIX pin
  en_mft_nzcl_time_t   enNoiceCancelTime;    ///< See #en_mft_nzcl_time_t for details
  func_ptr_t           pfnNzclCallback;      ///< Callback function for NZCL
} stc_mft_nzcl_config_t;

/**
 ******************************************************************************
 ** \brief Mft Input-Capture-unit (ICU) configuration.
 **
 ** Contains all parameters for the ICU configuration of an MFT instance.  
 ******************************************************************************/
typedef struct stc_mft_icu_config
{
  en_mft_icu_connect_t enIcu0Connect;             ///< See #en_mft_icu_connect_t for details
  en_mft_icu_connect_t enIcu1Connect;             ///< See #en_mft_icu_connect_t for details  
  en_mft_icu_connect_t enIcu2Connect;             ///< See #en_mft_icu_connect_t for details
  en_mft_icu_connect_t enIcu3Connect;             ///< See #en_mft_icu_connect_t for details 
  en_mft_icu_edge_t    enIcu0Edge;                ///< See #en_mft_icu_edge_t for details
  en_mft_icu_edge_t    enIcu1Edge;                ///< See #en_mft_icu_edge_t for details
  en_mft_icu_edge_t    enIcu2Edge;                ///< See #en_mft_icu_edge_t for details
  en_mft_icu_edge_t    enIcu3Edge;                ///< See #en_mft_icu_edge_t for details
  boolean_t            bIcu0InterruptEnable;      ///< TRUE: Enables interrupt for ICU0
  boolean_t            bIcu1InterruptEnable;      ///< TRUE: Enables interrupt for ICU1
  boolean_t            bIcu2InterruptEnable;      ///< TRUE: Enables interrupt for ICU2
  boolean_t            bIcu3InterruptEnable;      ///< TRUE: Enables interrupt for ICU3 
  func_ptr_t           pfnIcu0Callback;           ///< Callback function pointer of ICU0 interrupt 
  func_ptr_t           pfnIcu1Callback;           ///< Callback function pointer of ICU1 interrupt 
  func_ptr_t           pfnIcu2Callback;           ///< Callback function pointer of ICU2 interrupt 
  func_ptr_t           pfnIcu3Callback;           ///< Callback function pointer of ICU3 interrupt 
} stc_mft_icu_config_t;

/**
 ******************************************************************************
 ** \brief Mft Input-Capture-unit (ICU) configuration.
 **
 ** Contains all parameters for the ICU configuration of an MFT instance.  
 ******************************************************************************/
typedef struct stc_mft_adcmp_config
{
  en_mft_adc_cmp_t     enAdcmp0Frt;                 ///< See #en_mft_adc_cmp_t for details
  en_mft_adc_cmp_t     enAdcmp1Frt;                 ///< See #en_mft_adc_cmp_t for details
  en_mft_adc_cmp_t     enAdcmp2Frt;                 ///< See #en_mft_adc_cmp_t for details
  boolean_t            bFrt0Adc0StartZeroDetection; ///< TRUE: Generate ADC0 start signal at Zero value detection by FRT0
  boolean_t            bFrt0Adc1StartZeroDetection; ///< TRUE: Generate ADC1 start signal at Zero value detection by FRT0
  boolean_t            bFrt0Adc2StartZeroDetection; ///< TRUE: Generate ADC2 start signal at Zero value detection by FRT0 
  boolean_t            bFrt1Adc0StartZeroDetection; ///< TRUE: Generate ADC0 start signal at Zero value detection by FRT1
  boolean_t            bFrt1Adc1StartZeroDetection; ///< TRUE: Generate ADC1 start signal at Zero value detection by FRT1
  boolean_t            bFrt1Adc2StartZeroDetection; ///< TRUE: Generate ADC2 start signal at Zero value detection by FRT1 
  boolean_t            bFrt2Adc0StartZeroDetection; ///< TRUE: Generate ADC0 start signal at Zero value detection by FRT2
  boolean_t            bFrt2Adc1StartZeroDetection; ///< TRUE: Generate ADC1 start signal at Zero value detection by FRT2
  boolean_t            bFrt2Adc2StartZeroDetection; ///< TRUE: Generate ADC2 start signal at Zero value detection by FRT2 
  en_mft_adc_cmp_count_t enAdcmp0Count;             ///< See #en_mft_adc_cmp_count_t for details
  en_mft_adc_cmp_count_t enAdcmp1Count;             ///< See #en_mft_adc_cmp_count_t for details
  en_mft_adc_cmp_count_t enAdcmp2Count;             ///< See #en_mft_adc_cmp_count_t for details
  boolean_t            bAdcmp0BufferDisable;        ///< TRUE: Disable buffering of ACCP0 and ACCPDN0
  boolean_t            bAdcmp1BufferDisable;        ///< TRUE: Disable buffering of ACCP1 and ACCPDN1
  boolean_t            bAdcmp2BufferDisable;        ///< TRUE: Disable buffering of ACCP2 and ACCPDN2 
  boolean_t            bAdcmp0PeakValueEnable;      ///< TRUE: Perform buffer transfer of ACCP0 and ACCPDN0 at Peak detection,
                                                    ///< FALSE: Perform buffer transfer of ACCP0 and ACCPDN0 at Zero detection
  boolean_t            bAdcmp1PeakValueEnable;      ///< TRUE: Perform buffer transfer of ACCP1 and ACCPDN1 at Peak detection,
                                                    ///< FALSE: Perform buffer transfer of ACCP1 and ACCPDN1 at Zero detection
  boolean_t            bAdcmp2PeakValueEnable;      ///< TRUE: Perform buffer transfer of ACCP2 and ACCPDN2 at Peak detection,
                                                    ///< FALSE: Perform buffer transfer of ACCP2 and ACCPDN2 at Zero detection
  uint16_t             u16Adcmp0Accp;               ///< ADCMP compare value 0
  uint16_t             u16Adcmp1Accp;               ///< ADCMP compare value 1
  uint16_t             u16Adcmp2Accp;               ///< ADCMP compare value 2  
  uint16_t             u16Adcmp0Accpdn;             ///< ADCMP compare value 0 for down counting
  uint16_t             u16Adcmp1Accpdn;             ///< ADCMP compare value 1 for down counting
  uint16_t             u16Adcmp2Accpdn;             ///< ADCMP compare value 2 for down counting  
  en_mft_adc_cmp_start_t enAdcmp0Start;             ///< ADCMP0 start signal
  en_mft_adc_cmp_start_t enAdcmp1Start;             ///< ADCMP1 start signal
  en_mft_adc_cmp_start_t enAdcmp2Start;             ///< ADCMP2 start signal  
  en_mft_adc_cmp_start_prio_t enAdcmp0StartPrio;    ///< ADCMP0 priority conversion start signal
  en_mft_adc_cmp_start_prio_t enAdcmp1StartPrio;    ///< ADCMP1 priority conversion start signal
  en_mft_adc_cmp_start_prio_t enAdcmp2StartPrio;    ///< ADCMP2 priority conversion start signal  
} stc_mft_adcmp_config_t;

/**
 ******************************************************************************
 ** \brief Mft Programmable Pulese Generator (PPG) configuration.
 **
 ** Contains all parameters for the PPG configuration of an MFT instance.  
 ******************************************************************************/
typedef struct stc_mft_ppg_config
{
  en_mft_ppg_mode_t    enPpg0Mode;           ///< PPG0 mode. See #en_mft_ppg_mode_t for details
  en_mft_ppg_mode_t    enPpg2Mode;           ///< PPG2 mode. See #en_mft_ppg_mode_t for details
  en_mft_ppg_mode_t    enPpg4Mode;           ///< PPG4 mode. See #en_mft_ppg_mode_t for details  
  en_mft_ppg_clock_t   enPpg0Clock;          ///< Prescaler of PPG0. See #en_mft_ppg_clock_t for details
  en_mft_ppg_clock_t   enPpg2Clock;          ///< Prescaler of PPG2. See #en_mft_ppg_clock_t for details
  en_mft_ppg_clock_t   enPpg4Clock;          ///< Prescaler of PPG4. See #en_mft_ppg_clock_t for details
  boolean_t            bPpg0InvertPolarity;  ///< TRUE: Invert polarity to high, if PPG0 is stopped
  boolean_t            bPpg2InvertPolarity;  ///< TRUE: Invert polarity to high, if PPG1 is stopped
  boolean_t            bPpg4InvertPolarity;  ///< TRUE: Invert polarity to high, if PPG2 is stopped
  boolean_t            bPpg0InterruptEnable; ///< TRUE: Enables PPG0 interrupt
  boolean_t            bPpg2InterruptEnable; ///< TRUE: Enables PPG2 interrupt
  boolean_t            bPpg4InterruptEnable; ///< TRUE: Enables PPG4 interrupt
  boolean_t            bPpg0InterruptMode;   ///< TRUE: PUF->1 on PPLH0 underfolw, FALSE: PUF->1 on PPLH0 or PPLL0 underflow
  boolean_t            bPpg2InterruptMode;   ///< TRUE: PUF->1 on PPLH2 underfolw, FALSE: PUF->1 on PPLH2 or PPLL2 underflow
  boolean_t            bPpg4InterruptMode;   ///< TRUE: PUF->1 on PPLH4 underfolw, FALSE: PUF->1 on PPLH4 or PPLL4 underflow
  union 
  {
    uint16_t           u16Ppg0Low;           ///< PPG0 low count (16 Bit mode)
    struct
    {
      uint8_t          u8Ppg0Low;            ///< PPG0 low count  (8 Bit and 8 + 8 bit mode)
      uint8_t          u8Ppg0High;           ///< PPG0 high count (8 Bit and 8 + 8 bit mode)
    };
  };
  union 
  {
    uint16_t           u16Ppg0High;          ///< PPG0 high count (16 Bit mode)
    struct
    {
      uint8_t          u8Ppg0PrescalerLow;   ///< PPG0 low count  (8 + 8 bit mode)
      uint8_t          u8Ppg0PrescalerHigh;  ///< PPG0 high count (8 + 8 bit mode)
    };
  };  
  union 
  {
    uint16_t           u16Ppg2Low;           ///< PPG2 low count (16 Bit mode)
    struct
    {
      uint8_t          u8Ppg2Low;            ///< PPG2 low count  (8 Bit and 8 + 8 bit mode)
      uint8_t          u8Ppg2High;           ///< PPG2 high count (8 Bit and 8 + 8 bit mode)
    };
  };
  union 
  {
    uint16_t           u16Ppg2High;          ///< PPG2 high count (16 Bit mode)
    struct
    {
      uint8_t          u8Ppg2PrescalerLow;   ///< PPG2 low count  (8 + 8 bit mode)
      uint8_t          u8Ppg2PrescalerHigh;  ///< PPG2 high count (8 + 8 bit mode)
    };
  };  
  union 
  {
    uint16_t           u16Ppg4Low;           ///< PPG4 low count (16 Bit mode)
    struct
    {
      uint8_t          u8Ppg4Low;            ///< PPG4 low count  (8 Bit and 8 + 8 bit mode)
      uint8_t          u8Ppg4High;           ///< PPG4 high count (8 Bit and 8 + 8 bit mode)
    };
  };
  union 
  {
    uint16_t           u16Ppg4High;          ///< PPG4 high count (16 Bit mode)
    struct
    {
      uint8_t          u8Ppg4PrescalerLow;   ///< PPG4 low count  (8 + 8 bit mode)
      uint8_t          u8Ppg4PrescalerHigh;  ///< PPG4 high count (8 + 8 bit mode)
    };
  };  
#if L3_PPG_1616_MODE_AVAILABLE == L3_ON
  uint16_t             u16Ppg6Low;           ///< PPG6 Prescaler in 16 + 16 bit mode
  uint16_t             u16Ppg6High;          ///< PPG7 Prescaler in 16 + 16 bit mode
#endif
  en_mft_ppg_trigger_t enPpg0Trigger;        ///< PPG0 trigger type: See #en_mft_ppg_trigger_t for details
  en_mft_ppg_trigger_t enPpg2Trigger;        ///< PPG2 trigger type: See #en_mft_ppg_trigger_t for details
  en_mft_ppg_trigger_t enPpg4Trigger;        ///< PPG4 trigger type: See #en_mft_ppg_trigger_t for details
  boolean_t            bPpg0StartLowLevel;   ///< TRUE: Start PPG0 with Low level GATE, FALSE: Start with high level GATE
  boolean_t            bPpg2StartLowLevel;   ///< TRUE: Start PPG2 with Low level GATE, FALSE: Start with high level GATE
  boolean_t            bPpg4StartLowLevel;   ///< TRUE: Start PPG4 with Low level GATE, FALSE: Start with high level GATE
  en_mft_ppg_clock_t   enPpgCompareClock;    ///< Prescaler of PPG0/2/4 compare. See #en_mft_ppg_clock_t for details
  uint8_t              u8Ppg0Compare;        ///< PPG0 compare value
  uint8_t              u8Ppg2Compare;        ///< PPG2 compare value
  uint8_t              u8Ppg4Compare;        ///< PPG4 compare value
  func_ptr_t           pfnPpg0Callback;      ///< Callback function pointer of PPG0 interrupt 
  func_ptr_t           pfnPpg2Callback;      ///< Callback function pointer of PPG2 interrupt 
  func_ptr_t           pfnPpg4Callback;      ///< Callback function pointer of PPG4 interrupt 
} stc_mft_ppg_config_t;

#if (L3_DEVICE_TYPE == L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mft Programmable Pulese Generator - IGBT configuration.
 **
 ** Contains all parameters for the PPG IGBT configuration.  
 ******************************************************************************/
typedef struct stc_mft_ppg_igbt_config
{
  boolean_t                      bStopProhibMode;   ///< TRUE: Stop prohibition mode in output active, FALSE: Normal mode
  en_mft_ppg_igbt_noisefilter_t  enNoiseFilter;     ///< Sets noise filer width, see #en_mft_ppg_igbt_noisefilter_t for details
  boolean_t                      bInvertedOutput0;  ///< TRUE: Output0 level inverted, FALSE: Output0 level normal
  boolean_t                      bInvertedOutput1;  ///< TRUE: Output1 level inverted, FALSE: Output1 level normal
  boolean_t                      bInvertedInput;    ///< TRUE: Trigger input level inverted, FLASE: Trigger input level normal
} stc_mft_ppg_igbt_config_t;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE7)

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled MFT instance
typedef enum en_mft_instance_index
{
  #if (L3_PERIPHERAL_ENABLE_MFT0 == L3_ON)
    MftInstanceIndexMft0,
  #endif
  #if (L3_PERIPHERAL_ENABLE_MFT1 == L3_ON)
    MftInstanceIndexMft1,
  #endif
  #if (L3_PERIPHERAL_ENABLE_MFT2 == L3_ON)
    MftInstanceIndexMft2,
  #endif
    MftInstanceIndexMax
} en_mft_instance_index_t;

/// MFT instance internal data, storing internal information for each enabled MFT instance.
typedef struct stc_mft_intern_data
{
  func_ptr_t  pfnFrt0PeakCallback;  ///< Callback function pointer of FRT0 peak detection interrupt 
  func_ptr_t  pfnFrt1PeakCallback;  ///< Callback function pointer of FRT1 peak detection interrupt 
  func_ptr_t  pfnFrt2PeakCallback;  ///< Callback function pointer of FRT2 peak detection interrupt 
  func_ptr_t  pfnFrt0ZeroCallback;  ///< Callback function pointer of FRT0 zero detection interrupt 
  func_ptr_t  pfnFrt1ZeroCallback;  ///< Callback function pointer of FRT1 zero detection interrupt 
  func_ptr_t  pfnFrt2ZeroCallback;  ///< Callback function pointer of FRT2 zero detection interrupt 
  func_ptr_t  pfnWfg01Callback;     ///< Callback function pointer of WFG 0/1 interrupt
  func_ptr_t  pfnWfg23Callback;     ///< Callback function pointer of WFG 2/3 interrupt
  func_ptr_t  pfnWfg45Callback;     ///< Callback function pointer of WFG 4/5 interrupt
  func_ptr_t  pfnOcu0Callback;      ///< Callback function pointer of OCU0 interrupt 
  func_ptr_t  pfnOcu1Callback;      ///< Callback function pointer of OCU1 interrupt 
  func_ptr_t  pfnOcu2Callback;      ///< Callback function pointer of OCU2 interrupt 
  func_ptr_t  pfnOcu3Callback;      ///< Callback function pointer of OCU3 interrupt 
  func_ptr_t  pfnOcu4Callback;      ///< Callback function pointer of OCU4 interrupt 
  func_ptr_t  pfnOcu5Callback;      ///< Callback function pointer of OCU5 interrupt 
  func_ptr_t  pfnIcu0Callback;      ///< Callback function pointer of ICU0 interrupt 
  func_ptr_t  pfnIcu1Callback;      ///< Callback function pointer of ICU1 interrupt 
  func_ptr_t  pfnIcu2Callback;      ///< Callback function pointer of ICU2 interrupt 
  func_ptr_t  pfnIcu3Callback;      ///< Callback function pointer of ICU3 interrupt 
  func_ptr_t  pfnPpg0Callback;      ///< Callback function pointer of PPG0 interrupt 
  func_ptr_t  pfnPpg2Callback;      ///< Callback function pointer of PPG2 interrupt 
  func_ptr_t  pfnPpg4Callback;      ///< Callback function pointer of PPG4 interrupt 
  func_ptr_t  pfnNzclCallback;      ///< NZCL Callback Function Pointer
  int8_t      i8MftInstanceNumber;  ///< MFT instance number
} stc_mft_intern_data_t;

/// MFT instance data type
typedef struct stc_mft_instance_data
{
  volatile stc_mftn_t*  pstcInstance;  ///< pointer to registers of an instance
  stc_mft_intern_data_t stcInternData; ///< module internal data of instance
} stc_mft_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled MFS instances and their internal data
extern stc_mft_instance_data_t m_astcMftInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
extern void MftWfgIrqHandler( stc_mftn_t*            pstcMft,
                              stc_mft_intern_data_t* pstcMftInternData
                            ) ;

extern void MftFrtIrqHandler( stc_mftn_t*            pstcMft,
                              stc_mft_intern_data_t* pstcMftInternData
                            ) ;

extern void MftIcuIrqHandler( stc_mftn_t*            pstcMft,
                              stc_mft_intern_data_t* pstcMftInternData
                            ) ;

extern void MftOcuIrqHandler( stc_mftn_t*            pstcMft,
                              stc_mft_intern_data_t* pstcMftInternData
                            ) ;

extern void MftPpgIrqHandler( stc_ppgn_t*            pstcPpg,
                              stc_mft_intern_data_t* pstcMftInternData
                            ) ;

extern en_result_t Mft_Init( stc_mftn_t*                pstcMft, 
                             stc_mft_frt_ocu_config_t*  pstcFrtOcuConfig,
                             stc_mft_wfg_config_t*      pstcWfgConfig,
                             stc_mft_nzcl_config_t*     pstcNzclConfig,
                             stc_mft_icu_config_t*      pstcIcuConfig,
                             stc_mft_adcmp_config_t*    pstcAdcmpConfig,
                             stc_mft_ppg_config_t*      pstcPpgConfig
                           ) ;

extern en_result_t Mft_FrtStart( stc_mftn_t*  pstcMft,
                                 boolean_t    bStartFrt0,
                                 boolean_t    bStartFrt1,
                                 boolean_t    bStartFrt2
                               ) ;

extern en_result_t Mft_FrtStop( stc_mftn_t*  pstcMft,
                                boolean_t    bStopFrt0,
                                boolean_t    bStopFrt1,
                                boolean_t    bStopFrt2
                              ) ;

extern uint16_t Mft_FrtRead( stc_mftn_t*  pstcMft,
                             uint8_t      u8FrtChannel
                           ) ;

extern en_result_t Mft_OcuStartStop( stc_mftn_t*                pstcMft, 
                                     stc_mft_frt_ocu_config_t*  pstcFrtOcuConfig
                                   ) ;

extern en_result_t Mft_IcuStartStop( stc_mftn_t*            pstcMft, 
                                     stc_mft_icu_config_t*  pstcIcuConfig
                                   ) ;

extern uint16_t Mft_IcuRead( stc_mftn_t*  pstcMft,
                             uint8_t      u8IcuChannel
                           ) ;

extern en_result_t Mft_PpgSwTrigger( stc_mftn_t*  pstcMft,
                                     boolean_t    bPpg0,
                                     boolean_t    bPpg2,
                                     boolean_t    bPpg4
                                   ) ;

extern en_result_t Mft_PpgSwStop( stc_mftn_t*  pstcMft,
                                  boolean_t    bPpg0,
                                  boolean_t    bPpg2,
                                  boolean_t    bPpg4
                                ) ;

extern en_result_t Mft_PpgTimerStart( stc_mftn_t*  pstcMft ) ;

extern en_result_t Mft_PpgTimerStop( stc_mftn_t*  pstcMft,
                                     boolean_t    bPpg0,
                                     boolean_t    bPpg2,
                                     boolean_t    bPpg4
                                   ) ;

extern en_result_t Mft_PpgSetValue8( stc_mftn_t*  pstcMft,
                                     uint8_t      u8PpgNumber,
                                     uint8_t      u8LowValue,
                                     uint8_t      u8HighValue
                                   ) ;

extern en_result_t Mft_PpgSetValue16( stc_mftn_t*  pstcMft,
                                      uint8_t      u8PpgNumber,
                                      uint16_t     u16LowValue,
                                      uint16_t     u16HighValue
                                    ) ;

extern en_result_t Mft_PpgSetValue8plus8( stc_mftn_t*  pstcMft,
                                          uint8_t      u8PpgNumber,
                                          uint8_t      u8PscLowValue,
                                          uint8_t      u8PscHighValue,
                                          uint8_t      u8LowValue,
                                          uint8_t      u8HighValue
                                        ) ;

extern en_result_t Mft_DeInit( stc_mftn_t*                pstcMft, 
                               stc_mft_frt_ocu_config_t*  pstcFrtOcuConfig,
                               stc_mft_wfg_config_t*      pstcWfgConfig,
                               stc_mft_nzcl_config_t*     pstcNzclConfig,
                               stc_mft_icu_config_t*      pstcIcuConfig,
                               stc_mft_adcmp_config_t*    pstcAdcmpConfig,
                               stc_mft_ppg_config_t*      pstcPpgConfig
                             ) ;

#if (L3_DEVICE_TYPE == L3_TYPE7)

extern en_result_t Mft_PpgIgbt_Init( stc_mft_ppg_igbt_config_t* pstcConfig ) ;

extern en_result_t Mft_PpgIgbt_Enable(void) ;

extern en_result_t Mft_PpgIgbt_Disable(void) ;

extern en_result_t Mft_PpgIgbt_DeInit(void) ;

#endif // #if (L3_DEVICE_TYPE == L3_TYPE7)

//@} // MftGroup

#ifdef __cplusplus
}
#endif

#endif /* __MFT_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/  
