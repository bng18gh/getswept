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
void flag(void);
void unflag(void);
void update_mines(void);
void mine(int row, int col);
void loss_screen(int row, int col);
void expose_mines(char character);
void score_points(int row, int col);


int mines_left;
int spaces_exposed; //temporary
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
	write_string(":) Mines Left:--");
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
	LPUART_EscPrint(CURSOR_START); //(start of the board point)
	LPUART_EscPrint(BLACK);
	for (int i = 0; i < ROWS; i++){
		for (int j = 0; j < COLUMNS; j++){
			LPUART_PrintChar('▓'); //odd question mark (MAC), or black box (PC)
		}
		LPUART_EscPrint(DOWN);
		LPUART_EscPrint("[9D");
	}
	LPUART_EscPrint("[1;40H");
	LPUART_Print("CONTROLS");
	LPUART_EscPrint("[2;40H");
	LPUART_Print("Movement: WASD");
	LPUART_EscPrint("[3;40H");
	LPUART_Print("Flag: F   Unflag: U");
	LPUART_EscPrint("[4;40H");
	LPUART_Print("Mine: M");
	set_cursor_position(1,15);
	write_string("10");
	set_cursor_position (2, 8);
	write_string("000000000");
	LPUART_EscPrint(CURSOR_START); //(start of the board point)
	mines_left = 10;
	spaces_exposed = 0;

}

/* -----------------------------------------------------------------------------
 * function : flag( )
 * INs      : none
 * OUTs     : none
 * action   : flag a mine on the board
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240531
 * -------------------------------------------------------------------------- */
void flag(){
	if(mines_left != 0){//only if there's mines you can actually flag
   	LPUART_PrintChar('>');
   	LPUART_EscPrint(LEFT);
   	mines_left--; //decrease mine count
   	update_mines();
   }
}

/* -----------------------------------------------------------------------------
 * function : unflag( )
 * INs      : none
 * OUTs     : none
 * action   : remove a flag
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240531
 * -------------------------------------------------------------------------- */
void unflag(){
	if(mines_left != 10){//only if there's flags to remove
   	LPUART_PrintChar('▓');
   	LPUART_EscPrint(LEFT);
   	mines_left++; //decrease mine count
   	update_mines();
   }
}

/* -----------------------------------------------------------------------------
 * function : update_mines( )
 * INs      : none
 * OUTs     : none
 * action   : update mine count on LCD
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240531
 * -------------------------------------------------------------------------- */
void update_mines(){
	int mines_to_print = mines_left;
	set_cursor_position(1, 15);
	int tenmines = mines_to_print / 10;
	write_char(tenmines + '0');
	mines_to_print = mines_to_print - (10 * tenmines);
	write_char(mines_to_print + '0');
}

/* -----------------------------------------------------------------------------
 * function : mine( )
 * INs      : current row and column position of the cursor
 * OUTs     : none
 * action   : mine on the screen
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240531
 * -------------------------------------------------------------------------- */
void mine(int row, int col){
	if (game[row - 1][col - 1] == '*'){ //dug up a mine
		loss_screen(row, col);
	}
	else{
		score_points(row, col);
	}
}

/* -----------------------------------------------------------------------------
 * function : loss_screen( )
 * INs      : current row and column position of the cursor
 * OUTs     : none
 * action   : screen and LCD update for when you lose the game
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240602
 * -------------------------------------------------------------------------- */
void loss_screen(int row, int col){
	LPUART_EscPrint(CURSOR_START);
	expose_mines('*');
	//change the color for the actual mine you hit
	LPUART_EscPrint(RED);
	LPUART_EscPrint(CURSOR_START);
	//go to the mine location
	for (int i = 0; i < col - 1; i++){
		LPUART_EscPrint(RIGHT);
	}
	for (int i = 0; i < row - 1; i++){
		LPUART_EscPrint(DOWN);
	}
	LPUART_PrintChar(game[row - 1][col - 1]);
	LPUART_EscPrint(LEFT);

	//print the losing terminal and LCD screen
	LPUART_EscPrint("[12;40H");
	LPUART_Print("Too bad! You hit a mine.");
	LPUART_EscPrint("[13;40H");
	LPUART_EscPrint(BLINK);
	LPUART_Print("Press x to restart the game");
	LPUART_EscPrint(NONE);
	set_cursor_position(1,2);
	write_string("(");//sad face on the LCD

}
/* -----------------------------------------------------------------------------
 * function : expose_mines( )
 * INs      : none
 * OUTs     : none
 * action   : show all the mines on the board
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240602
 * -------------------------------------------------------------------------- */
void expose_mines(char character){
	for (int i = 0; i < ROWS; i++){
		for (int j = 0; j < COLUMNS; j++){
			if (game[i][j] =='*'){
				LPUART_PrintChar(character);
			}
			else{
				LPUART_EscPrint(RIGHT);
			}
		}
		LPUART_EscPrint(DOWN);
		LPUART_EscPrint("[9D");
	}
}

/* -----------------------------------------------------------------------------
 * function : score_points( )
 * INs      : current row and column position of the cursor
 * OUTs     : none
 * action   : screen and LCD update for completing a mine
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240602
 * -------------------------------------------------------------------------- */
void score_points(int row, int col){
	//expose how many mines are around
	LPUART_PrintChar(game[row - 1][col - 1]);
	LPUART_EscPrint(LEFT);
	spaces_exposed++;
	//if you've mined all the correct things, win
	if (spaces_exposed == 71){//9x9 grid - 10 mines
		LPUART_EscPrint(CURSOR_START);
		LPUART_EscPrint(GREEN);
		expose_mines('>');
		LPUART_EscPrint("[12;40H");
		LPUART_Print("Yay! You won the game.");
		LPUART_EscPrint("[13;40H");
		LPUART_EscPrint(BLINK);
		LPUART_Print("Press x to make a new game");
		LPUART_EscPrint(NONE);
	}
}

