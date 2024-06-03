/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : minesweeper.h
  * @brief          : Header for minesweeper.c file.
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

#ifndef INC_MINESWEEPER_H_
#define INC_MINESWEEPER_H_


#define TOTAL_MINES (10)
#define ROWS (9)
#define COLUMNS (9)

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


#endif /* INC_MINESWEEPER_H_ */
