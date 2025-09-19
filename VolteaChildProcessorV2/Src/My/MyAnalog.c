#include "MyAnalog.h"
#include "MyDelay.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_hal_uart.h"

extern UART_HandleTypeDef huart1;

uint64_t TemperatureFilteredDval;
float TemperatureFilteredTemperature;
float TemperatureADC_MeasuredVoltage,TemperatureCalculatedResistance;
	
uint32_t TemperatureArrayForMedian[MAX_NO_OF_SAMPLE_FOR_MEDIAN];
uint32_t TemperatureArrayForAverage[MAX_NO_OF_SAMPLE_FOR_AVERAGE];

void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void ReadTemperature(void)
{	static char median_count=0,average_count=0,error_counter=0;
	char i,j;
	float tmp;
	
	TemperatureArrayForMedian[median_count] = ReadADC(1, 16, 1);
	if(TemperatureArrayForMedian[median_count] == 2147483647)//error with the adc i2c comm
	{	HAL_I2C_Mem_Write(&hi2c1 , MCP_I2C_ADD , 0x06 , I2C_MEMADD_SIZE_8BIT , 0, 0, 100);
		MyDelayMS(100);
		/* Clear NACKF Flag */
		__HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_AF);

		/* Clear STOP Flag */
		__HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_STOPF);

		/* Flush TX register */
		//I2C_Flush_TXDR(&hi2c1);
		hi2c1.Instance->TXDR = 0X00U;
		
		/* Clear Configuration Register 2 */
		I2C_RESET_CR2(&hi2c1);

		error_counter++;
		if(error_counter%5 == 0)
		{	//HAL_UART_DeInit(&huart1);
			//while(1);
		}
	}
	else
		error_counter = 0;
	median_count++;
		
	if(median_count == MAX_NO_OF_SAMPLE_FOR_MEDIAN)
	{	median_count = 0;
		//arrange in assending order
		for(i=0;i<MAX_NO_OF_SAMPLE_FOR_MEDIAN;i++)
		{	for(j=i+1;j<MAX_NO_OF_SAMPLE_FOR_MEDIAN;j++)
			{	if(TemperatureArrayForMedian[i] > TemperatureArrayForMedian[j])
				{	//swap
					tmp = TemperatureArrayForMedian[i];
					TemperatureArrayForMedian[i] = TemperatureArrayForMedian[j];
					TemperatureArrayForMedian[j] = tmp;
				}
			}
		}
		if(average_count == MAX_NO_OF_SAMPLE_FOR_AVERAGE)
		{	for(i=0;i<MAX_NO_OF_SAMPLE_FOR_AVERAGE-1;i++)
				TemperatureArrayForAverage[i] = TemperatureArrayForAverage[i+1];
			TemperatureArrayForAverage[MAX_NO_OF_SAMPLE_FOR_AVERAGE-1] = TemperatureArrayForMedian[MAX_NO_OF_SAMPLE_FOR_MEDIAN/2];//take the median
		}
		else
		{	TemperatureArrayForAverage[average_count] = TemperatureArrayForMedian[MAX_NO_OF_SAMPLE_FOR_MEDIAN/2];//take the median
			average_count++;
		}
		for(i=0,TemperatureFilteredDval = 0;i<average_count;i++)
			TemperatureFilteredDval += TemperatureArrayForAverage[i];
		TemperatureFilteredDval /= average_count;
	}
	//12bit adc in which 1 bit = Vmax/2^(bit-1) = 4.096/(2^12) = 1mV
	//ADC bit = 12bit
	//ADC MaxVoltage = 4.096V = 4096mV
	//ADC StepSize = ADC MaxVoltage/2^ADC bit = 4096/2^12 = 1mV = 1/1000V
	//ADC StepSize = ADC MaxVoltage/2^ADC bit = 4096/2^16 = 4.096/65536V = 0.0000625V
	//ADC StepSize = ADC MaxVoltage/2^ADC bit = 4096/2^18 = 4.096/262144V = 0.000015625V
	
	//TemperatureADC_MeasuredVoltage = (float)TemperatureFilteredDval/1000.0; 
	TemperatureADC_MeasuredVoltage = (float)TemperatureFilteredDval * 0.0000625; 
	TemperatureCalculatedResistance = ((330.0 * TemperatureADC_MeasuredVoltage)/(3.3 - TemperatureADC_MeasuredVoltage));
	TemperatureFilteredTemperature = ((TemperatureCalculatedResistance/100.0)-1)/0.003851;
	//TemperatureFilteredTemperature = ((CalculatedResistance/100.0)-1)/0.004;
}
