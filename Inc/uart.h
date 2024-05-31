/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : uart.h
  * @brief          : Header for uart.c file.
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

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

void uart_init(void);
void LPUART_PrintChar(char letter);
void LPUART_Print(const char* string);
void LPUART_EscPrint(const char *esc_string);

#define CLEAR ("[2J")
#define CURSOR_START ("[H")
#define BLINK ("[5m")
#define NONE ("[0m")
#define UP ("[1A")
#define DOWN ("[1B")
#define LEFT ("[1D")
#define RIGHT ("[1C")

#endif /* INC_UART_H_ */
