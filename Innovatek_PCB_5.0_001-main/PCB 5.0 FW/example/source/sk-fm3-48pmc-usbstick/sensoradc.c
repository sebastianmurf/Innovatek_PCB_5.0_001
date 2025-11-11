/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH                  */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user�s obligation to fully test the software in its      */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FSEU does not warrant that the deliverables do not infringe any      */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FSEU disclaims all */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FSEU�s liability  */
/* is restricted to intention and gross negligence.                     */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.4)                                                               */
/************************************************************************/
/** \file adc.c
 **
 ** for SK-FM3-48PMC-USBSTICK
 **
 ** History:
 **   2012-02-03  1.0  MSc  First version for SK-FM3-48PMC-USBSTICK
 **   2012-07-17  1.1  MSc  base_type.h -> base_types_l3.h
 **   2012-08-31  1.2  MSc  l3 support, rename adc -> sensoradc
 *****************************************************************************/

#include "sensoradc.h"
#include "math.h"

/******************************************************************************/
/* Global variable definitions                                                */
/******************************************************************************/

#if SENSORADC_USE_L3 == 1
    /// Predefinition of ADC struct pointers
    static stc_adcn_t* pstcAdc = NULL ;
    static uint8_t pu8ChannelBuffer[1];
    static uint16_t pu16ChannelBuffer[1];
    /// Configuration of ADC0
    static stc_adc_config_t stcAdcConfig0 ;  
#endif


#if SENSORADC_USE_L3 == 0
/**
 ******************************************************************************
 ** Initializes ADC hardware abstraction layer
 **
 ** \return none
 **
 *****************************************************************************/
void SensorAdc_AdcHardwareInit(void)
{
    FM3_GPIO->ADE   |= 0x0001;
    
    FM3_ADC0->SCIS3 = 0x00; // Scan channel select AN31-AN24
    FM3_ADC0->SCIS2 = 0x00; // Scan channel select AN23-AN16
    FM3_ADC0->SCIS1 = 0x00; // Scan channel select AN15-AN08
    FM3_ADC0->SCIS0 = 0x01; // Scan channel select AN07-AN00 -> AN00

    FM3_ADC0->ADST1 = 0x2F; // Sampling Time 1
    FM3_ADC0->ADST0 = 0x2F; // Sampling Time 0

    FM3_ADC0->ADSS3 = 0x00; // Sampling Time Select AN31-AN24
    FM3_ADC0->ADSS2 = 0x00; // Sampling Time Select AN23-AN16
    FM3_ADC0->ADSS1 = 0x00; // Sampling Time Select AN15-AN08
    FM3_ADC0->ADSS0 = 0x00; // Sampling Time Select AN07-AN00 -> Use Samplin Time 0 for AN0

    FM3_ADC0->ADCT  = 0x00; // Comparison Time = 14 x (ADCT + 2) / HCLK

    FM3_ADC0->ADCEN = 0x01; // Enable ADC    

    while (3 != FM3_ADC0->ADCEN); // wait until ADC operation is enabled

    FM3_ADC0->ADSR  = 0x00; // Stop ADC, Places conversion result on the MSB side. 

    FM3_ADC0->CMPCR = 0x00; // No comparsion
    FM3_ADC0->CMPD  = 0x00; // No comparsion value

    FM3_ADC0->SFNS  = 0x00; // Set Fifo Stage Count Interrupt
    FM3_ADC0->ADCR  = 0x00; // Disable ADC interrupts   
  
    FM3_ADC0->SCCR = 0x11; // FIFO clear, start ADC Single Conversion
}

/**
 ******************************************************************************
 ** Read ADC
 **
 ** \param u8Channel Adc Channel
 **
 ** \return value
 **
 *****************************************************************************/
uint16_t SensorAdc_Read(uint8_t u8Channel)
{
     FM3_ADC0->SCIS0 = (1 << u8Channel);
     bFM3_ADC0_SCCR_SSTR = 1; // Start ADC Single Conversion
     while( 0 != ((FM3_ADC0->SCFDL) & 0x1000)) // Is valid data available?
     {
         
     }
     return ((uint16_t)(FM3_ADC0->SCFD >> 19)); //reads a ADC value
}


#else /* SENSORADC_USE_L3 == 1 */
/**
 ******************************************************************************
 ** Initializes ADC hardware abstraction layer
 **
 ** \return none
 **
 *****************************************************************************/
void SensorAdc_AdcHardwareInit(void)
{
  stcAdcConfig0.u32CannelSelect.AD_CH_0 = 1;  // Enable AN0 for ADC0 
  stcAdcConfig0.enConversionMode = AdcModeScanConversion ;
  stcAdcConfig0.enResolution = Adc12Bit;
  stcAdcConfig0.pu8ChannelBufferPointer[0] = pu8ChannelBuffer;
  stcAdcConfig0.pu16ChannelBufferPointer[0] = pu16ChannelBuffer;
  stcAdcConfig0.u16ChannelBufferCounter= 0;   // Init buffer counter
  stcAdcConfig0.u16ChannelBufferEndCount= 1; // Set buffer end
  stcAdcConfig0.enUseInterruptsDma = AdcPolling;
  
  // Sampling Time: 9 * 4 + 1 = 37 cycles => 25 ns/cycle * 37 cycles = 925 ns
  stcAdcConfig0.u8SamplingTime = 9;
  stcAdcConfig0.enSamplingTimeMultiplier = AdcSamplingTimeMultiplier4; 

  // Comparison cycles: Divider 5 => 25 ns/cycle * 5 = 125 ns =>
  //   125 ns * 14 = 1750 ns overall comparison time (TYPE0 device)
  stcAdcConfig0.u8ComparisonTimeDivider = 5;

  if (Ok != Adc_Init((stc_adcn_t*)&ADC0, &stcAdcConfig0))
  {
      while(1);
  }
}


/**
 ******************************************************************************
 ** Read ADC
 **
 ** \param u8Channel Adc Channel
 **
 ** \return value
 **
 *****************************************************************************/
uint16_t SensorAdc_Read(uint8_t u8Channel)
{
    switch(u8Channel)
    {
    case 0:
      pstcAdc = (stc_adcn_t*)&ADC0;
      Adc_DeInit((stc_adcn_t*)&ADC0);
      stcAdcConfig0.u16ChannelBufferCounter = 0; // Init buffer counter 
      stcAdcConfig0.u16ChannelBufferEndCount = 1; // Set buffer end
      Adc_Init((stc_adcn_t*)&ADC0, &stcAdcConfig0);
      break;
    default:
      return 0;
    }

    Adc_TriggerStart(pstcAdc);
    while(Ok != Adc_ConversionStatus(pstcAdc));
    Adc_StoreData(pstcAdc);
    switch(u8Channel)
    {
    case 0:
      return pu16ChannelBuffer[0];
    default:
      return 0;
    }
}

#endif

/**
 ******************************************************************************
 ** Read ADC in Ohm
 **
 ** \param u8Channel Adc Channel
 **
 ** \return value
 **
 *****************************************************************************/
uint32_t SensorAdc_ReadSensorOhm(uint8_t u8Channel)
{
    uint32_t u32Adc;
    u32Adc = SensorAdc_Read(u8Channel);    
    return CALC_OHM(u32Adc);
}
