#include <stm32f401xc.h>

#define BIT_PERIOD 30 /* In microseconds */

#define CLK_GPIO GPIOB
#define CLK_PORT 6

#define DIO_GPIO GPIOB
#define DIO_PORT 7

uint8_t display_send_command(uint8_t command);
uint8_t display_send_command_data(uint8_t command, uint8_t* data, uint8_t length);
void display_print_value_integer(uint16_t value);
void display_print_value_integer_decimal(uint16_t value, uint8_t dot_position);
void display_set_brightness(uint8_t brightness);
void display_print_string(char* string);
void display_print_string_pos(char* string, uint8_t position);
void display_print_char(char symbol, uint8_t position);
void display_init(void);
void display_test(void);
void display_update(void);
void display_clear(void);