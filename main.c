#include "main.h"

int main(void)
{ 
  uint8_t mode = 1;  
  mx_pinout_config();
  
#ifdef BOOT_NRF_PIN
  if (GPIO_ReadInputDataBit(BOOT_NRF_GPIO, BOOT_NRF_PIN)) {
    mode = 2;   
  }
#endif
#ifdef BOOT_CONFIG_PIN
  if (GPIO_ReadInputDataBit(BOOT_CONFIG_GPIO, BOOT_CONFIG_PIN)) {
    mode = 3;
    get_remote_data();
  }
#endif

  switch (mode) {
    case 1: status_blink(1); rc5_mode_1(); break;
    case 2: status_blink(2); rc5_mode_2(); break;
    case 3: status_blink(3); rc5_serial_mode(); break;
    default: status_blink(1); rc5_mode_1(); break;
  }
  
  
}