#include <stdint.h>
#include <stm32f401xc.h>

typedef enum 
{
	ON = (1 << 12),
	OFF = (1 << 14)
} buttons_t;

void keyboard_gpio_init(void);
void keyboard_handler(buttons_t buttons);
buttons_t keyboard_get_buttons(void);