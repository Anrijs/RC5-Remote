#ifndef __HAL_H
#define __HAL_H

#include "main.h"

extern uint16_t TimerPeriod;
extern uint8_t toggle;

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);
void mx_pinout_config(void);

void exti_init();

void TIM1_init();
void RC5_toggle_bit();

#ifdef NRF
void SPI1_init();     // Initialize SPI1
void SPI1_enable();   // Enable chip on SPI1                   
void SPI1_disable();  // Disable chip on SPI1

uint8_t SPI1_send(uint8_t data); // Send data to SPI1
uint8_t SPI1_send_only(uint8_t data); 
#endif

void USART2_init();
void USART2_put(uint8_t ch);
void USART2_write(char *text);
void USART2_write_num(uint8_t num);
void USART2_newline();
void USART2_writeln();
uint8_t USART2_get();

#endif