#include <stm32f401xc.h>

typedef enum 
{
	BACK_DISP 	= 0b1,
	FWD_REV 		= 0b10,
	BLANK 			= 0b100,
	PROG 				= 0b1000,
	ARROW_UP 		= 0b10000,
	START 			= 0b100000,
	FUNC_DATA 	= 0b1000000,
	ARROW_DOWN	= 0b10000000,
	STOP 				= 0b100000000
} buttons_t;

void keyboard_gpio_init(void);
uint16_t keyboard_get_buttons(void);