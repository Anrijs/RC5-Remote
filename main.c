#include "main.h"

int main(void)
{ 
  uint8_t mode = 1;
  /*** Mode description ***
  * Mode 0: RC5 mode (default)
            Load if nothing pressed on startup
            Allows user to:
              + Use RC5 remote.
  * Mode 1: NRF Mode + Basic IR
            Load by pressing #BOOT_NRF_GPIO #BOOT_NRF_PIN at startup
            Allows user to:
              + Send data over NRF (transmit data on click)
              + Use NRF as kill switch (transmit data, while btn high) 
  * Mode 2: Serial mode
            Load by pressing #BOOT_CONFIG_GPIO #BOOT_CONFIG_PIN at startup
            Allows user to: 
              + Configure RC5 commands for buttons
              + Configure DIP switch behavior
            PC Interface:
              + Show LiPo voltage
              + Show button configuration
              + IR Mode & NRF Mode tabs
  */
  
  mx_pinout_config();
  
#ifdef BOOT_NRF_PIN
  if (GPIO_ReadInputDataBit(BOOT_NRF_GPIO, BOOT_NRF_PIN)) {
    mode = 2;
    
  }
#endif
#ifdef BOOT_CONFIG_PIN
  if (GPIO_ReadInputDataBit(BOOT_CONFIG_GPIO, BOOT_CONFIG_PIN)) {
    mode = 3;
    // FLash!   aa
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