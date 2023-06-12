#include <stm32f4xx.h>
#include <system_stm32f4xx.h>

#include "clock.h"
#include "keyboard.h"
#include "delay.h"
#include "display.h"
#include "pwm.h"
#include "periphery.h"
#include "analog.h"

uint16_t old_keyboard_buttons = 0;
uint16_t keyboard_buttons = 0;
uint16_t value = 0;
uint16_t frequency = 0;

uint8_t state = 0;
int main(void)
{
	// System initialization
	SystemInit();
	
	// Main clock initialization
	clock_init();

	// Update core clock after clock initialization
	SystemCoreClockUpdate();
	SysTick_Init();
	
	delay_ms(1000);
	
	adc1_init();
	relay_gpio_init();
	fan_gpio_init();
	keyboard_gpio_init();
	display_init();
	pwm_channels_init();
	pwm_timer_init();
	pwm_enable();
	
	display_print_string("lodc");
	display_update();
	
	delay_ms(2000);
	relay_set_state(1);
	//fan_set_state(1);
	
	display_set_brightness(3);
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
		
		if(value > 999)
		{
			value = 0;
		}
		
		frequency = (50000 * (adc_raw_data[4] / 4095.0f)) / 100;
		display_print_value_integer_decimal(frequency, 2);
		display_update();
		delay_ms(33);
	}
}