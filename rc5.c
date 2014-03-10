/***
  RC5 main functions
***/

#include "rc5.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000002)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08006000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08006041)   /* End @ of user Flash area */
#define DATA_32                 ((uint32_t)0x12345678)

// User flash is defined to 32x 16bit fields
// 16 will be used for button registers

uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t NbrOfPage = 0x00;
uint32_t Data = 0x00000000;
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus MemoryProgramStatus = PASSED;

// Default RC5 values
uint16_t remote_data[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void status_blink(uint8_t count) {
  for(;count>0;count--) {
#ifdef STATUS_LED1
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED1, Bit_SET);
#endif  
#ifdef STATUS_LED2
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED2, Bit_SET);
#endif  
#ifdef STATUS_LED3
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED3, Bit_SET);
#endif  
#ifdef STATUS_LED4
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED4, Bit_SET);
#endif  
    Delay(100);
#ifdef STATUS_LED1
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED1, Bit_RESET);
#endif  
#ifdef STATUS_LED2
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED2, Bit_RESET);
#endif  
#ifdef STATUS_LED3
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED3, Bit_RESET);
#endif  
#ifdef STATUS_LED4
    GPIO_WriteBit(STATUS_GPIO, STATUS_LED4, Bit_RESET);
#endif  
    Delay(100);
  }
}

void rc5_mode_2() {
  // TODO:
  //   Wait for button click
  //    -> Send command
}

void rc5_mode_1() {
  TIM1_init();
  GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET);
  Delay(100);
  RC5_cmd(0x00,0x05);
  while(1) {

  }
}

void rc5_mode() {
  //
  
  
  
}

void rc5_serial_mode() {
  USART2_init();
  
  while(1) {              
#ifdef TERMINAL_DEBUG 
    USART2_writeln("Input method:");
    char c = USART2_get();
    USART2_writeln("Input command:");
    char d = USART2_get();
#else
    char c = USART2_get();
    char d = USART2_get();
#endif
  
    if(c == 0xaa || c == 'a') {
      if(d == 0x01 || d == '1') {
        USART2_write("MODE1");
        USART2_newline();
        for(int i=0;i<8;i++){
          USART2_write("Button "); 
          USART2_put(i+48);
          USART2_put(':');
          USART2_newline();
          USART2_write(" - Mode: ");
          USART2_write_num(remote_data[i] >> 13);
          USART2_write(" Address: ");
          USART2_write_num((remote_data[i] >> 8) & 0x1F);
          USART2_write(" Command: "); 
          USART2_write_num(remote_data[i] & 0x00FF);
          USART2_newline();
        }
      
        USART2_write("MODE2");
        USART2_newline();
        USART2_newline();
        for(int i=8;i<16;i++){
          USART2_write("Button "); 
          USART2_put(i+40);
          USART2_put(':');
          USART2_newline();
         // uint8_t temp = 
          USART2_write(" - Mode: ");
          USART2_write_num(remote_data[i] >> 13);
          USART2_write(" Address: ");
          USART2_write_num((remote_data[i] >> 8) & 0x1F);
          USART2_write(" Command: "); 
          USART2_write_num(remote_data[i] & 0x00FF);
          USART2_newline();
        }
      }
      else if(d == 0x02) {
        //Export compact
      }
    }
    else if(c == 0xbb) {     
      if(d == 0x04) {
        post_remote_data();
      }
      else {
        
#ifdef TERMINAL_DEBUG
        USART2_writeln("Mode:");
        char e = USART2_get();
        USART2_writeln("Button:");
        char f = USART2_get();
        USART2_writeln("Value:");
        char g = USART2_get();
#else
        char e = USART2_get();
        char f = USART2_get();
        char g = USART2_get();
#endif
        rc5_config_write((e*8)+f,  d, g);
      }
      
    }
  }
  // TODO:
  //   Wait for request
  //     * Read data
  //        -> Send current configuration
  //            + Button configs
  //            + DIP Configs
  //            + LED Configs
  //            + LiPo status
  //     * Write data
  //        -> Wait for configuration
  //           -> Write new configuration
  
}

void rc5_config_write(uint8_t addr, uint8_t cmd, uint16_t value) {

  uint16_t temp = remote_data[addr];
  
  switch (cmd) {
    case 1: 
      temp = temp & 0x1FFF;
      temp = (value << 13) | temp;
      remote_data[addr] = temp;
      break;
    case 2: 
      temp = temp & 0xE0FF;
      temp = (value << 8) | temp;
      remote_data[addr] = temp;
      break;
    case 3: 
      temp = temp & 0xFF00;
      temp = value | temp;
      remote_data[addr] = temp;
      break;
  }
   USART2_write(" - Mode: ");
          USART2_write_num(remote_data[addr] >> 13);
          USART2_write(" Address: ");
          USART2_write_num((remote_data[addr] >> 8) & 0x1F);
          USART2_write(" Command: "); 
          USART2_write_num(remote_data[addr] & 0x00FF);
          USART2_newline(); 
   USART2_writeln("Updated");
}

void set_remote_data(uint8_t id, uint16_t value) {
  remote_data[id] = value;
}

void get_remote_data(void) {
  Address = FLASH_USER_START_ADDR;
  for(uint8_t i=0;i<32;i+=2) {
    Data = *(__IO uint32_t *)Address;
    remote_data[i] = Data & 0x0000FFFF;
    remote_data[i+1] = Data >> 16; 
    Address = Address + 4;
  }
}

void post_remote_data(void) {
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
  
  NbrOfPage = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++) {
    if (FLASH_ErasePage(FLASH_USER_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter))!= FLASH_COMPLETE) {
      while (1);
    }
  }

  Address = FLASH_USER_START_ADDR;
  for (uint8_t i=0;i<32;i+=2){
    Data = remote_data[i] + (remote_data[i+1] << 16);
    if (FLASH_ProgramWord(Address, Data) == FLASH_COMPLETE) {
      Address = Address + 4;
    }
    else { 
      while (1);
    }
  }
  FLASH_Lock(); 
}

/*----------------------------------------------------------------------------*/
/*                               RC5 Hardware                                 */
/*----------------------------------------------------------------------------*/

// Send RC5 command to address
void RC5_cmd(uint8_t addr, uint8_t cmd) {
  // Array doesn't work correctly for some reason
  // Bit operations will cost time and cause errorss 
  // on the go, so values are pre-calculated
  
  uint8_t addr1 = 0;
  uint8_t addr2 = 0;
  uint8_t addr3 = 0;
  uint8_t addr4 = 0;
  uint8_t addr5 = 0;
  
  uint8_t cmd1 = 0;
  uint8_t cmd2 = 0;
  uint8_t cmd3 = 0;
  uint8_t cmd4 = 0;
  uint8_t cmd5 = 0;
  uint8_t cmd6 = 0;

  
  addr1 = ((addr >> 0) & 0x1);
  addr2 = ((addr >> 1) & 0x1);
  addr3 = ((addr >> 2) & 0x1);
  addr4 = ((addr >> 3) & 0x1);
  addr5 = ((addr >> 4) & 0x1);
  
  cmd1 = ((cmd >> 0) & 0x1);
  cmd2 = ((cmd >> 1) & 0x1);
  cmd3 = ((cmd >> 2) & 0x1);
  cmd4 = ((cmd >> 3) & 0x1);
  cmd5 = ((cmd >> 4) & 0x1);
  cmd6 = ((cmd >> 5) & 0x1);

  // Send start header (3bit)
  RC5_bit(1);
  RC5_bit(1);
  RC5_bit(toggle);
  
  // Send Address (5bit)
  RC5_bit(addr5);//4
  RC5_bit(addr4);//5
  RC5_bit(addr3);//6
  RC5_bit(addr2);//7
  RC5_bit(addr1);//8
  
  // Send Command (6bit)
  RC5_bit(cmd6);//9
  RC5_bit(cmd5);//10
  RC5_bit(cmd4);//11
  RC5_bit(cmd3);//12
  RC5_bit(cmd2);//13
  RC5_bit(cmd1);//14

}

// Send bit via IR
void RC5_bit(uint8_t bit) {
  if(bit > 0) {
    RC5_halfbit(0);
    RC5_halfbit(1);
  }
  else {
    RC5_halfbit(1);
    RC5_halfbit(0);
  } 
}

// Half of one bit
void RC5_halfbit(uint8_t halfbit) {
  uint16_t timer = 0;  
  if(halfbit) { TIM_CtrlPWMOutputs(TIM1, ENABLE); 
     GPIO_WriteBit(STATUS_GPIO, STATUS_LED1, Bit_SET);
  }   
  TIM_Cmd(TIMER_TIM, ENABLE);

  for(uint8_t i = 0; i < 32; i++) {   
    // Wait for 1 cycle (32 peaks)
    while(timer < 600) {
      timer = TIM_GetCounter(TIMER_TIM);
    } 
    while(timer > 600 && timer < TimerPeriod) {
      timer = TIM_GetCounter(TIMER_TIM);
    }
  }
  TIM_Cmd(TIMER_TIM, DISABLE);
  GPIO_WriteBit(STATUS_GPIO, STATUS_LED1, Bit_RESET);
  TIM_CtrlPWMOutputs(TIMER_TIM, DISABLE);
}

// Pause between RC5 command sending
void RC5_pause() {
  for(uint8_t i = 0; i<RC5_PAUSE;i++) {
    RC5_halfbit(0);
  }
}


