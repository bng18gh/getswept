/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *EE329 CP Definitely NOT Minesweeper
  ******************************************************************************
  * @file           : minesweeper.c
  * @brief          : minesweeper game establishment code
  * project         : EE329 A3 Spring 2024
  * authors         : Brandon Ng bng18@calpoly.edu
  * 				       Ryan Rayos rrayos@calpoly.edu
  * date            : 240530
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
 * 0.1 240530 bn  created
 *
 *******************************************************************************
 * TODO
 *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "minesweeper.h"
#include "delay.h"
#include "uart.h"
#include "lcd.h"

//Function Prototypes
void game_start(void);
void board_init(void);


static char game [9][9] = {{'*', '2', '1', '1', ' ', '1', '1', '2', '1'},
						  	  	   {'1', '2', '*', '1', ' ', '1', '*', '2', '*'},
									{' ', '1', '1', '1', ' ', '1', '1', '2', '1'},
									{' ', ' ', ' ', ' ', ' ', ' ', '1', '1', '1'},
									{' ', ' ', ' ', ' ', '1', '1', '2', '*', '1'},
									{' ', ' ', ' ', ' ', '1', '*', '3', '3', '2'},
									{' ', ' ', ' ', ' ', '1', '2', '*', '2', '*'},
									{' ', ' ', '1', '1', '1', '1', '1', '3', '2'},
									{' ', ' ', '1', '*', '1', ' ', ' ', '1', '*'},
								  };

/* -----------------------------------------------------------------------------
 * function : game_start( )
 * INs      : none
 * OUTs     : none
 * action   : game entry screen
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240530
 * -------------------------------------------------------------------------- */
void game_start(){
	LPUART_EscPrint(CLEAR); //clear
	LPUART_EscPrint(CURSOR_START); //cursor to top left
	LPUART_EscPrint("[6;24H");
	LPUART_EscPrint(BLINK); //blinking mode
	LPUART_Print("Welcome to Minesweeper!");
	LPUART_EscPrint("[7;24H");
	LPUART_Print("Press the x key to start");
	LPUART_EscPrint(NONE); //stop blinking
	set_cursor_position(1,1);
	write_string(":) Mines Left:10");
	set_cursor_position(2,1);
	write_string("Score: ---------");
}

/* -----------------------------------------------------------------------------
 * function : board_init( )
 * INs      : none
 * OUTs     : none
 * action   : starts the blank minesweeper screen, adjusts LCD
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240530
 * -------------------------------------------------------------------------- */
void board_init(){
	LPUART_EscPrint(CLEAR);
	LPUART_EscPrint("[9;9H"); //(start of the board point)
	for (int i = 0; i < ROWS; i++){
		for (int j = 0; j < COLUMNS; j++){
			LPUART_PrintChar('▓'); //creates odd question mark
		}
	LPUART_EscPrint(DOWN);
	LPUART_EscPrint("[9D");
	set_cursor_position (2, 8);
	write_string("000000000");
	}
}




