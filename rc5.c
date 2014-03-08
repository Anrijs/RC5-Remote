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
  while(1) {}
}

void rc5_mode() {
  //
}

void rc5_serial_mode() {
  USART2_init();
  
  while(1) {
  //USART2_put('a');
    // 170:: ¬x `                
#ifdef TERMINAL_DEBUG 
    USART2_writeln("Input method:");
    char c = USART2_get();
    USART2_writeln("Input command:");
    char d = USART2_get();
#else
    char c = USART2_get();
    char d = USART2_get();
#endif
  
    if(c == 0xaa) {
      if(d == 0x01) {
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

        USART2_put(e);
        USART2_put(f);
        USART2_put(g);
        rc5_config_write((e*8)+f,  d, g);
      }
      
      /*
        0xbb 0x01 #<rmd> #<btn> #<bmd>
        0xbb 0x02 #<rmd> #<btn> #<cmd>
        0xbb 0x03 #<rmd> #<btn> #<addr>
      */
      // Write command(d) remode_mode(e) button(f) button_mode(g)
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
  /*
    0xbb 0x01 #<rmd> #<btn> #<bmd>
    0xbb 0x03 #<rmd> #<btn> #<cmd>
    0xbb 0x02 #<rmd> #<btn> #<addr>
  */
  
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


