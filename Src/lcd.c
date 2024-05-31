/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *EE329 A3 Parallel LCD
  ******************************************************************************
  * @file           : lcd.c
  * @brief          : LCD establishment code
  * project         : EE329 A3 Spring 2024
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
 * BREADBOARD WIRING (pinout NUCLEO-L4A6ZG = L496ZG)
 * A0:D4
 * A1:D5
 * A2:D6
 * A3:D7
 * A4:RS
 * A5:RW
 * A6:EN
 * 3v3:VDD
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
#include "lcd.h"
#include "delay.h"

//Function Prototypes
void LCD_init(void);
void enable(void);
void LCD_command(uint8_t command);
void write_char(char letter);
void write_string (char *string);
void set_cursor_position(uint8_t row, uint8_t col);
int time_sec (int min1, int min2, int sec1, int sec2);

//create an array to loop through in output
uint16_t LCD_Pins[] = {D4, D5, D6, D7};

/* -----------------------------------------------------------------------------
 * function : LCD_Init( )
 * INs      : none
 * OUTs     : none
 * action   : LCD register initializer
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void LCD_init(void){
   //enable the LCD Clock
   // configure GPIOA pins PA0-6 for:
   // output mode, push-pull, no pull up or pull down, high speed
   delay_us(100000);
   RCC->AHB2ENR      |=  (RCC_AHB2ENR_GPIOAEN);
   LCD_PORT->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
	    				  	  	  GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
   	   	   	   	  GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
   	   	   	   	  GPIO_MODER_MODE6);
   LCD_PORT->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 |
   							  GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
								  GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
								  GPIO_MODER_MODE6_0);
   LCD_PORT->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 |
	    				  	  	  GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
								  GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |
								  GPIO_OTYPER_OT6);
   LCD_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
	    				  	  	  GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
								  GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |
								  GPIO_PUPDR_PUPD6);
   LCD_PORT->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
	                       (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
	                       (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
	                       (3 << GPIO_OSPEEDR_OSPEED3_Pos) |
								  (3 << GPIO_OSPEEDR_OSPEED4_Pos) |
								  (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
								  (3 << GPIO_OSPEEDR_OSPEED6_Pos));

   // preset PA0-6 to 0
   LCD_PORT->BRR = (D4|D5|D6|D7|RS|RW|EN);

   //LCD wakeup and setup commands
   delay_us(100000);
   LCD_command(0x30);
   delay_us(100);
   LCD_command(0x30);
   delay_us(100);
   LCD_command(0x30);
   delay_us(100);
   LCD_command(0x20);
   delay_us(100);
   LCD_command(0x02);
   delay_us(100);
}

/* -----------------------------------------------------------------------------
 * function : enable( )
 * INs      : none
 * OUTs     : none
 * action   : send a pulse from STM32 to LCD
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void enable(void){
   LCD_PORT-> BSRR = (EN); //set enable
   delay_us(50);
   LCD_PORT-> BRR = (EN); //drop enable
   delay_us(50);
   LCD_PORT-> ODR &= ~(D4|D5|D6|D7); //clear the registers holding the data
}

/* -----------------------------------------------------------------------------
 * function : LCD_command( )
 * INs      : none
 * OUTs     : none
 * action   : send command address to LCD (good for modal set)
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void LCD_command(uint8_t command){
   uint8_t high_command = command >> 4; //make both nibbles accessible
   LCD_PORT->BRR = (RS|RW); //for commands, both RS and RW are 0
   for (int i = 0; i < 4; i++){
	  if ((high_command >> i) & 0x01){ //output the first nibble to the correct pins
		 LCD_PORT->BSRR = LCD_Pins[i];
	  }
	  else{
	     LCD_PORT->BRR = LCD_Pins[i];
	  }
   }
   delay_us (1000); //stall before enabling
   enable();

   for (int i = 0; i < 4; i++){
	  if ((command >> i) & 0x01){ //output the second nibble to the correct pins
		 LCD_PORT->BSRR = LCD_Pins[i];
	  }
	  else{
	     LCD_PORT->BRR = LCD_Pins[i];
	  }
   }
   delay_us (1000); //stall before enabling
   enable();

}

/* -----------------------------------------------------------------------------
 * function : write_char( )
 * INs      : char letter
 * OUTs     : none
 * action   : given a character, write it to the LCD
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void write_char(char letter){
   uint8_t low_char = letter;
   uint8_t high_char = low_char >> 4; //make nibbles of both parts of the letter
   LCD_PORT-> BSRR = (RS);
   LCD_PORT-> BRR = (RW); //writing, so set RS to 1 and RW to 0

   for (int i = 0; i < 4; i++){
	  if ((high_char >> i) & 0x01){ //output the first nibble to the correct pins
		 LCD_PORT->BSRR = LCD_Pins[i];
	  }
	  else{
	     LCD_PORT->BRR = LCD_Pins[i];
	  }
   }
   delay_us (1000); //stall before enabling
   enable();

   for (int i = 0; i < 4; i++){
	  if ((low_char >> i) & 0x01){ //output the second nibble to the correct pins
		 LCD_PORT->BSRR = LCD_Pins[i];
	  }
	  else{
	     LCD_PORT->BRR = LCD_Pins[i];
	  }
   }
   delay_us (1000); //stall before enabling
   enable();
   LCD_PORT-> BRR = (RS);  //reset RS

}

/* -----------------------------------------------------------------------------
 * function : LCD_Init( )
 * INs      : char array
 * OUTs     : none
 * action   : given a list of characters, print all of them to LCD
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void write_string(char *string){
   for (int i = 0; string[i] != '\0'; i++){
	  write_char(string[i]);
	  delay_us(100);
   }
}

/* -----------------------------------------------------------------------------
 * function : set_cursor_position( )
 * INs      : row position and column position
 * OUTs     : none
 * action   : set the cursor at the location indicated on input
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
void set_cursor_position(uint8_t row, uint8_t col){
   //for readability, we'll go row 1,2 and column 1-16
   //set in DDRAM, so DB7 = 1, then set 00 for row 1 and 40 for row 2
   uint8_t address;
   if (row == 1){
      address = 0x80 + col -1;
   }
   else if (row == 2){
	  address = 0xC0 + col -1;
   }
   else {
	  address = 0x80; //default to row 1 column 1
   }
   LCD_command(address);
}

/* -----------------------------------------------------------------------------
 * function : time_sec( )
 * INs      : four numbers, implicitly as min1,min2:sec1,sec2
 * OUTs     : integer expressing the full time in seconds
 * action   : given four numbers, create a time MM:SS and
 * 			  calculate how many seconds that is
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 2404021
 * -------------------------------------------------------------------------- */
int time_sec (int min1, int min2, int sec1, int sec2){
	int seconds_1 = min1 * 600;
	int seconds_2 = min2 * 60;
	int seconds_3 = sec1 * 10;
	int total_time = seconds_1 + seconds_2 + seconds_3 + sec2;
	return total_time;
}


