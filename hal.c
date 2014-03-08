#include "hal.h"

static __IO uint32_t TimingDelay;
void status_blink(uint8_t);

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
  
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
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
