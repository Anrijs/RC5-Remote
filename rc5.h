#ifndef __RC5_H
#define __RC5_H

#include "main.h"

void status_blink(uint8_t count);

// Software
void rc5_mode();
void rc5_mode_1();
void rc5_mode_2();
void rc5_serial_mode();
void rc5_config_mode();
void rc5_config_write(uint8_t addr, uint8_t cmd, uint16_t value);

void get_remote_data();
void post_remote_data();

// Hardware commands
void RC5_cmd(uint8_t addr, uint8_t cmd);
void RC5_bit(uint8_t bit);
void RC5_halfbit(uint8_t bit);
void RC5_pause();


#endif