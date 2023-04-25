#include <stm32f401xc.h>

typedef enum 
{
	ON = 1,
	OFF = 2,
	START = 4,
	CLEAR = 8
} buttons_t;

void keyboard_gpio_init(void);
void keyboard_handler(buttons_t buttons);
uint16_t keyboard_get_buttons(void);