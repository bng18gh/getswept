/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 *EE329 CP Definitely NOT Minesweeper
 ******************************************************************************
 * @file           : main.c
 * @brief          : control hub to run minesweeper
 * project         : EE329 CP Spring 2024
 * authors         : Brandon Ng bng18@calpoly.edu
 * 						Ryan Rayos rrayos@calpoly.edu
 * date            : 240607
 * compiler        : STM32CubeIDE v.1.15.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L496ZG
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *******************************************************************************
 * BREADBOARD WIRING (pinout NUCLEO-L4A6ZG = L496ZG)
 * LCD: Pin1 - GND; Pin2 - 3V3; Pin4 - PA4; Pin5 - PA5; Pin6 - PA6;
 * 	  Pin11 - PA0; Pin12 - PA1; Pin13 - PA2; Pin14 - PA3;
 * 	  Pin15 - 560Ω; Pin16 - GND
 * Resistors: 560Ω - LCD Pin15 and 3V3
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240529 bn created
 * 0.2 240530 bn added game setup, LCD, board print
 * 0.2 240531 bn added flag and unflag
 * 0.3 240602 bn added winning, losing, scoring, timer
 * 0.4 240603 bn switched timer and score, added error prevention
 * 0.5 240603 rr added randomizer
 *******************************************************************************
 * TODO
 * make the navigation array
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "minesweeper.h"
#include "lcd.h"
#include "delay.h"
#include "uart.h"


/* Function Prototypes -------------------------------------------------------*/
void SystemClock_Config(void);
void GPIO_Init(void);


/* Main Execution ------------------------------------------------------------*/
int main(void) {
	HAL_Init();
	SystemClock_Config();
	uart_init();
	LCD_init();
	timer_init();

	game_start();

	while (1) {
	}

}




void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
