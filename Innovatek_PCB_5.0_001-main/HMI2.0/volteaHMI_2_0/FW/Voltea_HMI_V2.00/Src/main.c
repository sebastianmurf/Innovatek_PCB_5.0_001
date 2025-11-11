/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Lcd8Bit16x2.h"
#include "SerialCommunication.h"
#include "MyDelay.h"
#include "MyConstants.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

WWDG_HandleTypeDef hwwdg;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define MY_FALSE 1
#define MY_TRUE 0
unsigned char KeyUpFlag = MY_FALSE, KeyDownFlag = MY_FALSE, KeyEscFlag = MY_FALSE, KeyEnterFlag = MY_FALSE;
char CopyBuff[100];
unsigned int uiTemp;
unsigned int MainLoopCounter = 0;
unsigned char DisplayRefreshCounter = 1;//added for version 2.1
unsigned char ucTemp, Row=0, Col=0;
char PacketCRC, MyCRC;

#define KEY_DEBOUNCE_COUNT 50
char KeyUpDebounceCounter = 0;
char KeyDownDebounceCounter = 0;
char KeyEscDebounceCounter = 0;
char KeyEnterDebounceCounter = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_WWDG_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_WWDG_Init();
  /* USER CODE BEGIN 2 */
	LCD_Initialization();
	HAL_UART_Receive_IT(&huart1,&SerialRecvByte,1);
	CommFlag.CommandReceive = MY_FALSE;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  strcpy(LcdLine1,"Voltea HMI V2.00");
	strcpy(LcdLine2,"Waiting For Data");
	OutTextXY(0,1,LcdLine1);
	OutTextXY(0,2,LcdLine2);
	while (1)
  {		MainLoopCounter++;
			if(MainLoopCounter > 10000)
			{	MainLoopCounter = 0;
				HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port,STATUS_LED_Pin);
			}
			HAL_UART_Receive_IT(&huart1,&SerialRecvByte,1);//without this line, serial recv gets disabled, don't know why.
			
			#ifdef WATCH_DOG
					HAL_WWDG_Refresh(&hwwdg);
			#endif
			
			if(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET){
				MyDelayMS(1000);// 2 sec delay
				if(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET){
					LCD_Initialization();
				}
			}
			else
			{
				if(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin) == GPIO_PIN_RESET){
					KeyUpDebounceCounter++;
					if(KeyUpDebounceCounter > KEY_DEBOUNCE_COUNT){
						if(KeyUpFlag == MY_FALSE){
							KeyUpFlag = MY_TRUE;
							SendString("{");
							SendCRC = 0;
							SendString("01=00\r\n--------------------\r\nUP\r\n--------------------\r\nxxxx\r\n*");
							sprintf(CopyBuff,"%d}",SendCRC);
							SendString(CopyBuff);
							//OutTextXY(0,1,"U ");//test code
						}
					}
				}
				else
				{	KeyUpFlag = MY_FALSE;
					KeyUpDebounceCounter = 0;
				}

				if(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET){
					KeyDownDebounceCounter++;
					if(KeyDownDebounceCounter > KEY_DEBOUNCE_COUNT){
						if(KeyDownFlag == MY_FALSE){
							KeyDownFlag = MY_TRUE;
							SendString("{");
							SendCRC = 0;
							SendString("01=00\r\n--------------------\r\nDOWN\r\n--------------------\r\nxxxx\r\n*");
							sprintf(CopyBuff,"%d}",SendCRC);
							SendString(CopyBuff);
							//OutTextXY(0,1,"D ");//test code
						}
					}
				}
				else
				{	KeyDownFlag = MY_FALSE;
					KeyDownDebounceCounter = 0;
				}
			}
			
			if(HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port, KEY_ESC_Pin) == GPIO_PIN_RESET){
				KeyEscDebounceCounter++;
				if(KeyEscDebounceCounter > KEY_DEBOUNCE_COUNT){
					if(KeyEscFlag == MY_FALSE){
						KeyEscFlag = MY_TRUE;
						SendString("{");
						SendCRC = 0;
						SendString("01=00\r\n--------------------\r\nESC\r\n--------------------\r\nxxxx\r\n*");
						sprintf(CopyBuff,"%d}",SendCRC);
						SendString(CopyBuff);
						//OutTextXY(0,1,"O ");//test code
						//while(1);//test code
					}
				}
			}
			else
			{	KeyEscFlag = MY_FALSE;
				KeyEscDebounceCounter = 0;
			}

			if(HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port, KEY_ENTER_Pin) == GPIO_PIN_RESET){
				KeyEnterDebounceCounter++;
				if(KeyEnterDebounceCounter > KEY_DEBOUNCE_COUNT){
					if(KeyEnterFlag == MY_FALSE){
						KeyEnterFlag = MY_TRUE;
						SendString("{");
						SendCRC = 0;
						SendString("01=00\r\n--------------------\r\nENTER\r\n--------------------\r\nxxxx\r\n*");
						sprintf(CopyBuff,"%d}",SendCRC);
						SendString(CopyBuff);
						//OutTextXY(0,1,"I ");//test code
					}
				}
			}
			else
			{	KeyEnterFlag = MY_FALSE;
				KeyEnterDebounceCounter = 0;
			}
			if(CommFlag.CommandReceive == MY_TRUE)
			{	uiTemp = (SerialRxBuff[3]-48)*10+SerialRxBuff[4]-48;
				if(uiTemp == 0)//Broadcast message
				{
					//Checking CRC
					for(uiTemp =0,MyCRC =0; SerialRxBuff[uiTemp]!='*';uiTemp++)
					{
						MyCRC += SerialRxBuff[uiTemp];	
						HAL_Delay(1);
					}
					MyCRC += '*';
					for(uiTemp = uiTemp+1, PacketCRC =0; SerialRxBuff[uiTemp]!='\0';uiTemp++)
					{
						PacketCRC *= 10;
						PacketCRC += SerialRxBuff[uiTemp] -48;
					}
					if(MyCRC == PacketCRC)
					{
						//Valid Packet
						for(ucTemp = 0;ucTemp<16;ucTemp++)
						{
							LcdLine1[ucTemp] = SerialRxBuff[25+ucTemp];
							LcdLine2[ucTemp] = SerialRxBuff[43+ucTemp];
						}
						OutTextXY(0,1,LcdLine1);
						OutTextXY(0,2,LcdLine2);
						Row = SerialRxBuff[79]-48;
						Col = (SerialRxBuff[80]-48)*10+SerialRxBuff[81]-48;
						GotoXY(Col,Row);
						if(SerialRxBuff[82]=='A')	//Active
						{  LcdCommand(0x0f);  //Display cursor
						}
						else
						{	LcdCommand(0x0c); // Don't display cursor	  
						}
						CommFlag.CommandReceive = MY_FALSE;
  				}
					/*else
					{	OutTextXY(0,1,"      CRC       ");
						OutTextXY(0,2,"    MISMATCH    ");
					}*/
				}
				CommFlag.CommandReceive = MY_FALSE;
			}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_WWDG_Init(void)
{

  /* USER CODE BEGIN WWDG_Init 0 */

  /* USER CODE END WWDG_Init 0 */

  /* USER CODE BEGIN WWDG_Init 1 */

  /* USER CODE END WWDG_Init 1 */
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
  hwwdg.Init.Window = 127;
  hwwdg.Init.Counter = 127;
  hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, STATUS_LED_Pin|LCD_RS_Pin|LCD_RW_Pin|LCD_EN_Pin
                          |DATA_D7_Pin|DATA_D6_Pin|DATA_D5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DATA_D4_Pin|DATA_D3_Pin|DATA_D2_Pin|DATA_D1_Pin
                          |DATA_D0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : KEY_ENTER_Pin KEY_DOWN_Pin KEY_UP_Pin KEY_ESC_Pin */
  GPIO_InitStruct.Pin = KEY_ENTER_Pin|KEY_DOWN_Pin|KEY_UP_Pin|KEY_ESC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : STATUS_LED_Pin LCD_RS_Pin LCD_RW_Pin LCD_EN_Pin
                           DATA_D7_Pin DATA_D6_Pin DATA_D5_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_Pin|LCD_RS_Pin|LCD_RW_Pin|LCD_EN_Pin
                          |DATA_D7_Pin|DATA_D6_Pin|DATA_D5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DATA_D4_Pin DATA_D3_Pin DATA_D2_Pin DATA_D1_Pin
                           DATA_D0_Pin */
  GPIO_InitStruct.Pin = DATA_D4_Pin|DATA_D3_Pin|DATA_D2_Pin|DATA_D1_Pin
                          |DATA_D0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
