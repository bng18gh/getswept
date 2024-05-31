/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : lcd.h
  * @brief          : Header for lcd.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l4xx_hal.h"

#define LCD_PORT (GPIOA)
#define D4 (GPIO_PIN_0)
#define D5 (GPIO_PIN_1)
#define D6 (GPIO_PIN_2)
#define D7 (GPIO_PIN_3)
#define RS (GPIO_PIN_4)
#define RW (GPIO_PIN_5)
#define EN (GPIO_PIN_6)


void LCD_init(void);
void system_enable(void);
void LCD_command(uint8_t command);
void write_char(char letter);
void set_cursor_position(uint8_t row, uint8_t col);
void write_string (char *string);
int time_sec (int min1, int min2, int sec1, int sec2);

#endif /* INC_LCD_H_ */
