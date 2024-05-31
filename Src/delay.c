/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *EE329 A3 Parallel LCD
  ******************************************************************************
  * @file           : delay.c
  * @brief          : establish microsecond delay timer
  * project         : EE321 A3 Spring 2024
  * authors         : Brandon Ng bng18@calpoly.edu
  * 				       James Pan jpan14@calpoly.edu
  * date            : 240421
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
 * REVISION HISTORY
 * 0.1 240421 bn  created
 *
 *******************************************************************************
 * TODO
 *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "delay.h"

/* -----------------------------------------------------------------------------
 * function : SysTick_Init( )
 * INs      : none
 * OUTs     : none
 * action   : establish external clock
 * authors  : Brandon Ng (bn) bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void SysTick_Init(void) {
   SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     	// enable SysTick Timer
                     SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
   SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	// disable interrupt
}

// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : results in longer delays than specified:
//	   e.g. @4MHz, delay_us(1) = 10=15 us delay.
/* -----------------------------------------------------------------------------
 * function : delay_us( )
 * INs      : time in microseconds
 * OUTs     : none
 * action   : delay in microseconds the time specified
 * authors  : Brandon Ng (bn) bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void delay_us(const uint32_t time_us) {
   // set the counts for the specified delay
   SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
   SysTick->VAL = 0;                                  	 // clear timer count
   SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);    	 // clear count flag
   while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}

