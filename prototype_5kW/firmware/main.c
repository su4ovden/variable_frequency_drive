#include <stm32f4xx.h>
#include <system_stm32f4xx.h>

#include "clock.h"
#include "keyboard.h"
#include "delay.h"
#include "display.h"
#include "pwm.h"

void clock_init(void);

uint16_t old_keyboard_buttons = 0;
uint16_t keyboard_buttons = 0;
uint16_t value = 0;

int main(void)
{
	// System initialization
	SystemInit();
	
	// Main clock initialization
	clock_init();

	// Update core clock after clock initialization
	SystemCoreClockUpdate();
	SysTick_Init();
	
	keyboard_gpio_init();
	display_init();
	pwm_channels_init();
	pwm_timer_init();
	pwm_enable();
	
	display_print_string("lodc");
	display_update();
	display_clear();
	
	delay_ms(1000);
	
	display_set_brightness(1);
	display_print_value_integer_decimal(value, 2);
	display_print_char('f', 0);
	display_update();
	
	while(1)
	{
		keyboard_buttons = keyboard_get_buttons();
		
		if(keyboard_buttons != old_keyboard_buttons)
		{
			keyboard_handler(keyboard_buttons);
			
			display_print_value_integer_decimal(value, 2);
			display_print_char('f', 0);
			display_update();
			
			old_keyboard_buttons = keyboard_buttons;
		}
		
		if(value > 998)
		{
			value = 0;
		}
		
		//delay_us(1000);
	}
}