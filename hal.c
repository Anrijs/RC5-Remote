#include "hal.h"

static __IO uint32_t TimingDelay;
void status_blink(uint8_t);

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

uint8_t tim_irq_flag = 0;

uint16_t TimerPeriod = 0;
uint16_t IR_Pulse = 0;
 
/*----------------------------------------------------------------------------*/
/*                                    GPIO                                    */
/*----------------------------------------------------------------------------*/

void mx_pinout_config(void) {
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_ClocksTypeDef RCC_Clocks;

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  /*Enable or disable the AHB peripheral clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOC, ENABLE);
  /*Configure GPIO pin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  /*Configure GPIO pin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  
  /* Set up interrupts */
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_1_IRQHandler (void)
{
  EXTI_ClearITPendingBit(EXTI_Line0);
  EXTI_ClearFlag(EXTI_Line0);
  RC5_cmd();
}

/*----------------------------------------------------------------------------*/
/*                                    USART                                   */
/*----------------------------------------------------------------------------*/

void USART2_init() {
  
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

  //Configure USART2 pins:  Rx and Tx ----------------------------
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);

  USART_Cmd(USART2,ENABLE);
}


/**
  * @brief  Sends value directly to USART2 Tx
  * @param  ch: character/value to send to USART
  * @retval None
  */
void USART2_put(uint8_t ch)
{
  // Disable RX while sending to avoid errors
  USART_DirectionModeCmd(USART2,USART_Mode_Rx, DISABLE);
  GPIO_WriteBit(STATUS_GPIO, STATUS_LED4, Bit_SET);
 
  USART_SendData(USART2, (uint8_t) ch);
  // Loop until the end of transmittion
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
  
  // Eanble RX after the end of transmittion
  USART_DirectionModeCmd(USART2,USART_Mode_Rx, ENABLE);
  GPIO_WriteBit(STATUS_GPIO, STATUS_LED4, Bit_RESET);
}

/**
  * @brief  Returns value received from USART2 Rx
  * @param  None
  * @retval value from USART2
  */
uint8_t USART2_get(void){
  // Wait for data
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
  return USART_ReceiveData(USART2);
}

/**
  * @brief  Sends string to USART2 Tx
  * @param  text: string to send to USART2
  * @retval None
  */
void USART2_write(char *text) {
  int i=0;
  while(text[i] || text[i] !='\0') {
    USART2_put(text[i]);
    i++;
  }
}

void USART2_writeln(char *text) {
  USART2_write(text);
  USART2_newline();
}

/**
  * @brief  Sends newline to USART2 Tx
  * @param  None
  * @retval None
  */
void USART2_newline()
{
  USART2_put('\n');
#ifdef WINDOWS_TERMINAL
  USART2_put('\r');
#endif
}

/**
  * @brief  Sends integer as text to USART2 Tx
  * @param  num: 8bit integer to convert
  * @retval None
  */
void USART2_write_num(uint8_t num) {
  uint8_t text[4];
  uint8_t pos = 0;
  while(num > 10) {
    text[pos] = (num%10)+48;
    pos++;
    num = num/10;
  }
  text[pos] = num+48;
  pos++;
  for(;pos>0;pos--) {
    USART2_put(text[pos-1]);
  }
}

/*----------------------------------------------------------------------------*/
/*                                    SPI                                     */
/*----------------------------------------------------------------------------*/

// this function initializes the SPI1 peripheral
void SPI1_init(void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStruct;
  
  /* configure pins used by SPI1
   * PA5 = SCK
   * PA6 = MISO
   * PA7 = MOSI
   */

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // connect SPI1 pins to SPI alternate function
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);
  
  /* Configure the chip select pin PB0 */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
    
  // enable peripheral clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  /* configure SPI1 in Mode 0 
   * CPOL = 0 --> clock is low when idle
   * CPHA = 0 --> data is sampled at the first edge
   */
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB2 frequency / 4
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
  SPI_Init(SPI1, &SPI_InitStruct); 
  
  SPI_Cmd(SPI1, ENABLE); // enable SPI1
}

/* This funtion is used to transmit and receive data 
 * with SPI1
 *      data --> data to be transmitted
 *      returns received value
 */

uint8_t SPI1_send(uint8_t data){
  SPI1->DR = data; // write data to be transmitted to the SPI data register
  while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
  while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
  return SPI1->DR; // return received data from SPI data register
}

uint8_t SPI1_send_only(uint8_t data){
  SPI1->DR = data; // write data to be transmitted to the SPI data register
  while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
  return 1; // return received data from SPI data register
}

void SPI1_enable() {
   GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
}

void SPI1_disable() {
   GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
}

/*----------------------------------------------------------------------------*/
/*                              TIM1 functions                                */
/*----------------------------------------------------------------------------*/

void TIM1_init() {
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Clocks enable */
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2);

  /*************** CONFIGURE TIMER ********************/
  
  /* Compute the value to be set in ARR regiter to generate signal frequency at 36.00 Khz */
  TimerPeriod = (SystemCoreClock / 36000 ) - 1;
  /* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 */
  IR_Pulse = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2, 3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  
  TIM_OCInitStructure.TIM_Pulse = IR_Pulse;
  
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);  
  /* TIM1 counter enable */
}


/**********************************/

void RC5_cmd() {
  // Enable IR for transmittion

  TIM_Cmd(TIM1, DISABLE);
  
  // Send start header 110
  RC5_bit(1);
  RC5_bit(1);
  RC5_bit(0);
  
  // Send Address (5bit)
  RC5_bit(0);//4
  RC5_bit(0);//5
  RC5_bit(0);//6
  RC5_bit(0);//7
  RC5_bit(0);//8
  
  // Sent Command (6bit)
  RC5_bit(0);//9
  RC5_bit(0);//10
  RC5_bit(0);//11
  RC5_bit(0);//12
  RC5_bit(0);//13
  RC5_bit(1);//14
  
 // RC5_bit(0);
 // RC5_bit(0);

  // Disable IR after transmittion

}

void RC5_bit(uint8_t bit) {
  if(bit) {
    // High, then low halfbit
    RC5_halfbit(0);
    RC5_halfbit(1);
  }
  else {
    RC5_halfbit(1);
    RC5_halfbit(0);
  }
  
  //TIM_CtrlPWMOutputs(TIM1, DISABLE);
  
  
}

void RC5_halfbit(uint8_t halfbit) {
  uint16_t timer = 0;  
  if(halfbit) { TIM_CtrlPWMOutputs(TIM1, ENABLE); 
     GPIO_WriteBit(STATUS_GPIO, STATUS_LED1, Bit_SET);
  } // 0    
  TIM_Cmd(TIM1, ENABLE);

  for(uint8_t i = 0; i < 32; i++) {     
    while(timer < 600) {
      timer = TIM_GetCounter(TIM1);
    } // Wait for 1 cycle
    while(timer > 600 && timer < TimerPeriod) {
      timer = TIM_GetCounter(TIM1);
    }
  }
  TIM_Cmd(TIM1, DISABLE);
  GPIO_WriteBit(STATUS_GPIO, STATUS_LED1, Bit_RESET);
  TIM_CtrlPWMOutputs(TIM1, DISABLE);
}

/*----------------------------------------------------------------------------*/
/*                              Default functions                             */
/*----------------------------------------------------------------------------*/

/******  Default functions *******/
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
