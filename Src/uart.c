/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *EE329 A7 UART Communications
  ******************************************************************************
  * @file           : uart.c
  * @brief          : UART establishment code
  * project         : EE329 A7 Spring 2024
  * authors         : Brandon Ng bng18@calpoly.edu
  * 				       Maks Velikanovs mvelikan@calpoly.edu
  * date            : 240505
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
 * 0.1 240505 bn  created
 *
 *******************************************************************************
 * TODO
 *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "delay.h"
#include "minesweeper.h"
#include "lcd.h"


//Function Prototypes
void uart_init(void);
void LPUART_PrintChar(char letter);
void LPUART_Print(const char* string);
void LPUART_EscPrint(const char *esc_string);


int row_location;
int col_location;

/* -----------------------------------------------------------------------------
 * function : uart_init( )
 * INs      : none
 * OUTs     : none
 * action   : uart register initializer, baud rate set
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240505
 * -------------------------------------------------------------------------- */
void uart_init(void){
	PWR->CR2 |= (PWR_CR2_IOSV);              // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge
	// configure GPIOG pins PG5-8 for:
	// AF mode, push-pull, no pull up or pull down, high speed
	GPIOG->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7
						 | GPIO_MODER_MODE8); //clear bits
	GPIOG->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1
						 | GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1); //set bits AF mode
	GPIOG->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL5_Pos)
						 | (0x000F << GPIO_AFRL_AFSEL6_Pos)
						 | (0x000F << GPIO_AFRL_AFSEL7_Pos));
	GPIOG->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL8_Pos); //clear AF
	GPIOG->AFR[0] |= ((0x0008 << GPIO_AFRL_AFSEL5_Pos)
					    | (0x0008 << GPIO_AFRL_AFSEL6_Pos)
						 | (0x0008 << GPIO_AFRL_AFSEL7_Pos));	//set bits AF 8
	GPIOG->AFR[1] |= (0x0008 << GPIO_AFRH_AFSEL8_Pos);
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
	LPUART1->BRR = 8889; //serial 115200
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
	__enable_irq();                          // enable global interrupts

}

/* -----------------------------------------------------------------------------
 * function : LPUART1_IRQHandler( )
 * INs      : none
 * OUTs     : none
 * action   : UART keyboard interrupt controls
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240505
 * -------------------------------------------------------------------------- */
void LPUART1_IRQHandler(void){
	uint8_t charRecv;
	if (LPUART1->ISR & USART_ISR_RXNE) {
      charRecv = LPUART1->RDR;
      switch ( charRecv ) {
	   	break;
	   	case 'x': //game set and reset
	   		board_init();//create the initial board screen
	   		row_location = 1;
	   		col_location = 1;
		   	break;
	   	//movement commands, limited to the 9x9 box
	   	case 'w': //move up
	   		if(row_location != 1){
		   		LPUART_EscPrint(UP);
	   			row_location--;
	   		}
		   	break;
	   	case 'a': //move left
	   		if (col_location != 1){
	   			LPUART_EscPrint(LEFT);
	   			col_location--;
	   		}
		   	break;
	   	case 's': //move down
	   		if(row_location != 9){
		   		LPUART_EscPrint(DOWN);
	   			row_location++;
	   		}
		   	break;
	   	case 'd': //move right
	   		if (col_location != 9){
	   			LPUART_EscPrint(RIGHT);
	   			col_location++;
	   		}
		   	break;
	   	case 'f': //flag
		      flag(row_location, col_location);
		   	break;
	   	case 'm': //mine
		      mine(row_location, col_location);
		      LPUART_EscPrint(CURSOR_START);
		      for (int i = 0; i < col_location - 1; i++){
		      	LPUART_EscPrint(RIGHT);
		      }
		      for (int i = 0; i < row_location - 1; i++){
		      	LPUART_EscPrint(DOWN);
		      }
		   	break;
	   default:
	      while( !(LPUART1->ISR & USART_ISR_TXE) ); // wait for empty TX buffer
	      LPUART1->TDR = charRecv;  // echo char to terminal

      }  // end switch
   }
}

/* -----------------------------------------------------------------------------
 * function : LPUART_PrintChar( )
 * INs      : single character
 * OUTs     : none
 * action   : send characters via serial
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240505
 * -------------------------------------------------------------------------- */
void LPUART_PrintChar(char letter){
	while(!(LPUART1->ISR & USART_ISR_TXE)); // wait for empty xmit buffer
	LPUART1->TDR = letter;       // send this character
	delay_us(100);
}

/* -----------------------------------------------------------------------------
 * function : LPUART_Print( )
 * INs      : string of characters
 * OUTs     : none
 * action   : send characters via serial
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240505
 * -------------------------------------------------------------------------- */
void LPUART_Print(const char* string){
	uint16_t iStrIdx = 0;
	while ( string[iStrIdx] != '\0' ) {
	   LPUART_PrintChar(string[iStrIdx]);       // send this character
		iStrIdx++;                             // advance index to next char
	}
}

/* -----------------------------------------------------------------------------
 * function : LPUART_EscPrint( )
 * INs      : string of characters
 * OUTs     : none
 * action   : send escape commands via serial
 * authors  : Brandon Ng (bn) - bng18@calpoly.edu
 * version  : 0.1
 * date     : 240505
 * -------------------------------------------------------------------------- */
void LPUART_EscPrint(const char *esc_string){
	LPUART1->TDR = 0x1b; //ESC
	LPUART_Print(esc_string);
	delay_us(100);
}

void Print_String(int row, int column, const char text[]) {
	char escape[3000];
	if ((90 > row) | (row > 0)) {
		if ((90 > column) | (column > 0)) {
			snprintf(escape, sizeof(escape), "\x1B[%d;%dH", row, column);
			LPUART_EscPrint("[H");
			while (!(LPUART1->ISR & USART_ISR_TXE)) { // wait for empty xmit buffer
				;
			}
			strcat(escape, text);
			//sprintf(escape, "\x1B[%d;%dH", row, column);
			LPUART_EscPrint(escape);

		}
	}
}

