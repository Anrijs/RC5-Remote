#ifndef __HAL_H
#define __HAL_H

/***
 Hardware init
***/

#include "main.h"

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);
void mx_pinout_config(void);

void TIM1_init();
void TIM1_config();

void RC5_cmd();
void RC5_bit(uint8_t bit);
void RC5_halfbit(uint8_t bit);

void SPI1_init();     // Initialize SPI1
void SPI1_enable();   // Enable chip on SPI1                   
void SPI1_disable();  // Disable chip on SPI1

uint8_t SPI1_send(uint8_t data); // Send data to SPI1
uint8_t SPI1_send_only(uint8_t data); 

void USART2_init();
uint8_t USART2_get();
void USART2_put(uint8_t ch);
void USART2_write(char *text);
void USART2_write_num(uint8_t num);
void USART2_newline();
void USART2_writeln();

#endif