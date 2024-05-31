/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : delay.h
  * @brief          : Header for delay.c file.
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
#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "main.h"

void SysTick_Init (void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
