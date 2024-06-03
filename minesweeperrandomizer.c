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
#include <stdio.h>
#include <stdlib.h>
//#include "timer.h"

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
void update_score(int row, int col, int winning);
void final_score(void);

//int COLUMNS = 9;
//int ROWS = 9;
int mines_left;
int spaces_exposed; //temporary
uint32_t score;
uint32_t starting_time;
uint32_t ending_time;
char game[9][9];
//static char game [9][9] = {{'*', '2', '1', '1', ' ', '1', '1', '2', '1'},
//						  	  	   {'1', '2', '*', '1', ' ', '1', '*', '2', '*'},
//									{' ', '1', '1', '1', ' ', '1', '1', '2', '1'},
//									{' ', ' ', ' ', ' ', ' ', ' ', '1', '1', '1'},
//									{' ', ' ', ' ', ' ', '1', '1', '2', '*', '1'},
//									{' ', ' ', ' ', ' ', '1', '*', '3', '3', '2'},
//									{' ', ' ', ' ', ' ', '1', '2', '*', '2', '*'},
//									{' ', ' ', '1', '1', '1', '1', '1', '3', '2'},
//									{' ', ' ', '1', '*', '1', ' ', ' ', '1', '*'},
//								  };


/* -----------------------------------------------------------------------------
 * function : initialize_grid()
 * INs      : none
 * OUTs     : none
 * action   : fill grid with "0"s
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240530
 * -------------------------------------------------------------------------- */
void initialize_grid(char grid[ROWS][COLUMNS]){
	for (int i=0; i<ROWS; ++i){
		for (int j = 0; j < COLUMNS; ++j){
			grid[i][j] = '0';
		}
	}
}
/* -----------------------------------------------------------------------------
 * function : place_mines()
 * INs      : game grid
 * OUTs     : none
 * action   : game entry screen
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240530
 * -------------------------------------------------------------------------- */
void place_mines (char grid[ROWS][COLUMNS], int nummines){
	int minesplaced = 0;
	while (minesplaced < nummines){
		int row = rand() % ROWS;
		int columns = rand() % COLUMNS;

		if (grid[row][columns] != 'M'){
			grid[row][columns] ='M';
			minesplaced++;
		}
	}
}
/* -----------------------------------------------------------------------------
 * function : count_miens( )
 * INs      : game grid
 * OUTs     : none
 * action   : game entry screen
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240530
 * -------------------------------------------------------------------------- */
void count_mines(char grid[ROWS][COLUMNS]){
	int directions[8][2] = {
			{-1, -1}, {-1, 0}, {-1, 1},
			{ 0, -1},          { 0, 1},
			{ 1, -1}, { 1, 0}, { 1, 1}};

	for (int row = 0; row<ROWS; ++row){
		for (int col = 0; col<COLUMNS; ++col){
			if (grid[row][col] == 'M'){
					continue;
			}
			int minestotal = 0;

			for (int i = 0; i <8; ++i){
				int newRow = row + directions[i][0];
				int newCol = col + directions[i][1];

				if (newRow >= 0 && newRow < ROWS &&
					 newCol >= 0 && newCol < COLUMNS){
					 if (grid[newRow][newCol] == 'M') {
					 minestotal++;
				}
				}
			}
			grid[row][col] = minestotal + '0';
		}
	}

}
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
	LCD_Set_Cursor_Position(1,1);
	Str_Write(":) Mines Left:--");
	LCD_Set_Cursor_Position(2,1);
	Str_Write("Score: ---------");

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
	LCD_Set_Cursor_Position(1,2);
	Str_Write(")");
	LCD_Set_Cursor_Position(1,15);
	Str_Write("10");
	LCD_Set_Cursor_Position (2, 8);
	Str_Write("000000000");
	LPUART_EscPrint(CURSOR_START); //(start of the board point)
	mines_left = 10;
	spaces_exposed = 0;
	score = 0;
	starting_time = TIM2->CNT; //record time
	initialize_grid(game);
	place_mines(game, 10);
	count_mines(game);
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
	LCD_Set_Cursor_Position(1, 15);
	int tenmines = mines_to_print / 10;
	Write(tenmines + '0');
	mines_to_print = mines_to_print - (10 * tenmines);
	Write(mines_to_print + '0');
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
	if (game[row - 1][col - 1] == 'M'){ //dug up a mine
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
	expose_mines('M');
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
	LCD_Set_Cursor_Position(1,2);
	Str_Write("(");//sad face on the LCD

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
			if (game[i][j] =='M'){
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
	update_score(row, col, 0);
	//if you've mined all the correct things, win
	if (spaces_exposed == 71){//9x9 grid - 10 mines
		ending_time = TIM2->CNT; //record time
		LPUART_EscPrint(CURSOR_START);
		LPUART_EscPrint(GREEN);
		expose_mines('>');
		LPUART_EscPrint("[12;40H");
		LPUART_Print("Yay! You won the game.");
		LPUART_EscPrint("[13;40H");
		LPUART_EscPrint(BLINK);
		LPUART_Print("Press x to make a new game");
		LPUART_EscPrint(NONE);
		final_score();
	}
}

/* -----------------------------------------------------------------------------
 * function : update_score( )
 * INs      : current row and column position of the cursor, winning marker
 * OUTs     : none
 * action   : score update on LCD
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240602
 * -------------------------------------------------------------------------- */
void update_score(int row, int col, int winning){
	if (winning == 0){ //we don't want the tile affected to work if winning
		int score_marker;
		if (game[row - 1][col - 1] == ' '){
			score_marker = 0;
		}
		else{
			score_marker = game[row - 1][col - 1] - '0';
		}
		score += ((score_marker*100) + 100); //values mining riskier spaces
	}
	LCD_Set_Cursor_Position(2,9);
	uint32_t print_score = score;
	int tenmil = print_score/10000000;
	Write(tenmil + '0');
	print_score -= (tenmil*10000000);
	int mil = print_score/1000000;
	Write(mil + '0');
	print_score -= (mil*1000000);
	int hunthou = print_score/100000;
	Write(hunthou + '0');
	print_score -= (hunthou*100000);
	int tenthou = print_score/10000;
	Write(tenthou + '0');
	print_score -= (tenthou*10000);
	int thou = print_score/1000;
	Write(thou + '0');
	print_score -= (thou*1000);
	int hundreds = print_score/100;
	Write(hundreds + '0');
	print_score -= (hundreds*100);
	int tens = print_score/10;
	Write(tens + '0');
	print_score -= (tens*10);
	Write(print_score + '0');

}

/* -----------------------------------------------------------------------------
 * function : final_score( )
 * INs      : current row and column position of the cursor
 * OUTs     : none
 * action   : score update, only when game is won
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240602
 * -------------------------------------------------------------------------- */
void final_score(){
	score += 500000; //bonus score for winning
	uint32_t time_score = (10000000 - (ending_time - starting_time))%10000000;
	score += time_score;
	update_score(0, 0, 1);

}
