#include <stdint.h>
#include <stm32f401xc.h>

/* Warning! The area is under reconstruction! */
void i2c1_init(void);
void display_send_raw_command(uint8_t command);
void display_send_raw_command_data(uint8_t command, uint8_t data);
/* Warning! The area is under reconstruction! */