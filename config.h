#ifndef __CONFIG_H
#define __CONFIG_H

/*** RC5 HW Config ***/

/* Status LEDs */
#define STATUS_GPIO GPIOC
#define STATUS_LED1 GPIO_Pin_9
#define STATUS_LED2 GPIO_Pin_8
#define STATUS_LED3 GPIO_Pin_9
#define STATUS_LED4 GPIO_Pin_8

/* Button to start config mode */
#define BOOT_CONFIG_GPIO GPIOA
#define BOOT_CONFIG_PIN GPIO_Pin_0

/* Button to start NRF mode */
//#define BOOT_NRF_GPIO GPIOA
//#define BOOT_NRF_PIN GPIO_Pin_1

/* Enable if used wor windows terminal (will add '\r' after newline)*/
#define WINDOWS_TERMINAL

#endif