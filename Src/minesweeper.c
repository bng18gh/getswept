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

//Function Prototypes
void initialize_grid(char grid[ROWS][COLUMNS]);
void place_mines (char grid[ROWS][COLUMNS], int nummines);
void count_mines(char grid[ROWS][COLUMNS]);
void game_start(void);
void board_init(void);
void flag(int row, int col);
void update_mines(void);
void mine(int row, int col);
void loss_screen(int row, int col);
void expose_mines(char character);
void score_points(int row, int col);
void update_score(int row, int col, int winning);
void final_score(void);
void timer_init(void);


int mines_left;
int spaces_exposed;
int timer_on;
int seconds;
uint32_t score;
uint32_t starting_time;
uint32_t ending_time;
static int status [9][9];
static char game [9][9];


/* -----------------------------------------------------------------------------
 * function : initialize_grid()
 * INs      : none
 * OUTs     : none
 * action   : fill grid with "0"s
 * authors  : Ryan Rayos (rr) - rrayos@calpoly.edu
 * version  : 0.1
 * date     : 240603
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
 * INs      : game grid, number of mines to place
 * OUTs     : none
 * action   : game entry screen
 * authors  : Ryan Rayos (rr) - rrayos@calpoly.edu
 * version  : 0.1
 * date     : 240603
 * -------------------------------------------------------------------------- */
void place_mines (char grid[ROWS][COLUMNS], int nummines){
	int minesplaced = 0;
	while (minesplaced < nummines){
		int row = rand() % ROWS;
		int columns = rand() % COLUMNS;

		if (grid[row][columns] != '*'){
			grid[row][columns] ='*';
			minesplaced++;
		}
	}
}

/* -----------------------------------------------------------------------------
 * function : count_mines( )
 * INs      : game grid
 * OUTs     : none
 * action   : game entry screen
 * authors  : Ryan Rayos (rr) - rrayos@calpoly.edu
 * version  : 0.1
 * date     : 240603
 * -------------------------------------------------------------------------- */
void count_mines(char grid[ROWS][COLUMNS]){
	int directions[8][2] = {
			{-1, -1}, {-1, 0}, {-1, 1},
			{ 0, -1},          { 0, 1},
			{ 1, -1}, { 1, 0}, { 1, 1}};

	for (int row = 0; row<ROWS; ++row){
		for (int col = 0; col<COLUMNS; ++col){
			if (grid[row][col] == '*'){
					continue;
			}
			int minestotal = 0;

			for (int i = 0; i <8; ++i){
				int newRow = row + directions[i][0];
				int newCol = col + directions[i][1];

				if (newRow >= 0 && newRow < ROWS &&
					 newCol >= 0 && newCol < COLUMNS){
					 if (grid[newRow][newCol] == '*') {
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
	set_cursor_position(1,1);
	write_string(":) Mines Left:--");
	set_cursor_position(2,1);
	write_string("Time: ---       ");
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
	timer_on = 0;
	seconds = 0;
	LPUART_EscPrint(CLEAR);
	LPUART_EscPrint(CURSOR_START); //(start of the board point)
	LPUART_EscPrint(BLACK);
	//fill the status bar and terminal
	for (int i = 0; i < ROWS; i++){
		for (int j = 0; j < COLUMNS; j++){
			LPUART_PrintChar('▓'); //odd question mark (MAC), or black box (PC)
			status[i][j] = 0;
		}
		LPUART_EscPrint(DOWN);
		LPUART_EscPrint("[9D");
	}
	LPUART_EscPrint("[1;40H");
	LPUART_Print("CONTROLS");
	LPUART_EscPrint("[2;40H");
	LPUART_Print("Movement: WASD");
	LPUART_EscPrint("[3;40H");
	LPUART_Print("Flag: F");
	LPUART_EscPrint("[4;40H");
	LPUART_Print("Mine: M");
	LPUART_EscPrint("[20;1H");
	LPUART_Print("Score: ");
	set_cursor_position(1,2);
	write_string(")");
	set_cursor_position(1,15);
	write_string("10");
	LPUART_EscPrint ("[12;1H");
	write_string("Score: 00000000");
	LPUART_EscPrint(CURSOR_START); //(start of the board point)
	mines_left = 10;
	spaces_exposed = 0;
	score = 0;
	timer_on = 1;
	starting_time = TIM2->CNT; //record time
	initialize_grid(game);
	place_mines(game, 10);
	count_mines(game);

}

/* -----------------------------------------------------------------------------
 * function : flag( )
 * INs      : current row and column position of the cursor
 * OUTs     : none
 * action   : flag a mine on the board
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240531
 * -------------------------------------------------------------------------- */
void flag(int row, int col){
	if(status[row - 1][col - 1] == 0){//unmarked, can be flagged
		if(mines_left != 0){//only if there's mines you can actually flag
	   	LPUART_PrintChar('>');
	   	LPUART_EscPrint(LEFT);
	   	mines_left--; //decrease mine count
	   	status[row - 1][col - 1] = 2; //change status to flagged
	   	update_mines();
		}
   }
	else if (status[row - 1][col - 1] == 2){//flagged, so unflag the space
		if(mines_left != 10){//only if there's flags to remove
	   	LPUART_PrintChar('▓');
	   	LPUART_EscPrint(LEFT);
	   	mines_left++; //increase mine count
	   	status[row - 1][col - 1] = 0; //change status to unflagged
	   	update_mines();
	   }
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
	timer_on = 0;
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
	if (status [row - 1][col - 1] == 0){//blank space, can be mined
		//expose how many mines are around
		LPUART_PrintChar(game[row - 1][col - 1]);
		LPUART_EscPrint(LEFT);
		spaces_exposed++;
		status [row - 1][col - 1] = 1; //1 is mined
		update_score(row, col, 0);
		//if you've mined all the correct things, win
		if (spaces_exposed == 71){//9x9 grid - 10 mines
			timer_on = 0;
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
		if (game[row - 1][col - 1] == '0'){
			score_marker = 0;
		}
		else{
			score_marker = game[row - 1][col - 1] - '0';
		}
		score += ((score_marker*100) + 100); //values mining riskier spaces
	}
	LPUART_EscPrint("[20;8H");
	uint32_t print_score = score;
	int tenmil = print_score/10000000;
	LPUART_PrintChar(tenmil + '0');
	print_score -= (tenmil*10000000);
	int mil = print_score/1000000;
	LPUART_PrintChar(mil + '0');
	print_score -= (mil*1000000);
	int hunthou = print_score/100000;
	LPUART_PrintChar(hunthou + '0');
	print_score -= (hunthou*100000);
	int tenthou = print_score/10000;
	LPUART_PrintChar(tenthou + '0');
	print_score -= (tenthou*10000);
	int thou = print_score/1000;
	LPUART_PrintChar(thou + '0');
	print_score -= (thou*1000);
	int hundreds = print_score/100;
	LPUART_PrintChar(hundreds + '0');
	print_score -= (hundreds*100);
	int tens = print_score/10;
	LPUART_PrintChar(tens + '0');
	print_score -= (tens*10);
	LPUART_PrintChar(print_score + '0');

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
	TIM2->CCR1 = 4000000 - 1;                           // ticks for duty cycle
	TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
	NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
	__enable_irq();               // global IRQ enable
	TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
		TIM2->CCR1 += 4000000;				//every second
		if(timer_on == 1){
			seconds++;
			set_cursor_position(2, 7);
			int time_seconds = seconds;
			int hundreds = time_seconds/100;
			write_char(hundreds + '0');
			time_seconds -= (hundreds * 100);
			int tens = time_seconds/10;
			write_char(tens + '0');
			time_seconds -= (tens * 10);
			write_char(time_seconds + '0');
			write_string("       ");
		}
		TIM2->SR &= ~(TIM_SR_CC1IF);     // manage the flag
	}

}
