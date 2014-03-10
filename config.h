#ifndef __CONFIG_H
#define __CONFIG_H

/*** RC5 HW Config ***/
// Pause between transmittions
// (t = RC5_pause * 445us)
// Optimal: 100-120, MAX 255
#define RC5_PAUSE 100

/* Timers */

#define TIMER_TIM TIM1
#define TIMER_RCC_GPIO RCC_AHBPeriph_GPIOA
#define TIMER_RCC_TIM RCC_APB2Periph_TIM1
#define TIMER_LED_PIN GPIO_Pin_10
#define TIMER_GPIO GPIOA
#define TIMER_PIN_SOURCE GPIO_PinSource10

#define TIM_OC_CH TIM_OC1Init

/* Status LEDs */
#define STATUS_GPIO GPIOB
#define STATUS_LED1 GPIO_Pin_3
#define STATUS_LED2 GPIO_Pin_4
#define STATUS_LED3 GPIO_Pin_5
#define STATUS_LED4 GPIO_Pin_6

/*Buttons */
#define BUTTON_GPIO GPIOB
#define BUTTON_START GPIO_Pin_14
#define BUTTON_STOP GPIO_Pin_0
#define BUTTON_1 GPIO_Pin_12
#define BUTTON_2 GPIO_Pin_10
#define BUTTON_3 GPIO_Pin_2
#define BUTTON_4 GPIO_Pin_13
#define BUTTON_5 GPIO_Pin_11
#define BUTTON_6 GPIO_Pin_1

/* Button to start config mode */
#define BOOT_CONFIG_GPIO GPIOA
#define BOOT_CONFIG_PIN GPIO_Pin_0

/* Interrupt lines */
#define EXTI0_GPIO EXTI_PortSourceGPIOB

/* Button to start NRF mode */
//#define BOOT_NRF_GPIO GPIOA
//#define BOOT_NRF_PIN GPIO_Pin_1

/* Enable if used for windows terminal (will add '\r' after newline)*/
#define WINDOWS_TERMINAL

/* Enable for debuging in terminal */
#define TERMINAL_DEBUG

#endif