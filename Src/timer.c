/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *EE329 CP Definitely NOT Minesweeper
  ******************************************************************************
  * @file           : timer.c
  * @brief          : TIM2 timer operator
  * project         : EE321 CP Spring 2024
  * authors         : Brandon Ng bng18@calpoly.edu
  * 				       Ryan Rayos rrayos@calpoly.edu
  * date            : 240602
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
 *
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240602 bn created
 *******************************************************************************
 * TODO
 *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "main.h"

void timer_init(void);

/* -----------------------------------------------------------------------------
 * function : timer_init( )
 * INs      : none
 * OUTs     : none
 * action   : set up the TIM2 timer
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240602
 * -------------------------------------------------------------------------- */
void timer_init(){
	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);         // enable clock for TIM2
	TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
	TIM2->ARR = 0xFFFFFFFF - 1;                     // ARR = T = counts @4MHz
	TIM2->CCR1 = 400 - 1;                           // ticks for duty cycle
	TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
	NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
	__enable_irq();                                 // global IRQ enable
	TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
		TIM2->CCR1 += 400;
	   TIM2->SR &= ~(TIM_SR_CC1IF);     // manage the flag
	}

}
