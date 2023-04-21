#include <stm32f4xx.h>
#include <system_stm32f4xx.h>

#include "clock.h"
#include "keyboard.h"
#include "delay.h"
#include "display.h"

void clock_init(void);

uint16_t old_keyboard_buttons = 0;
uint16_t keyboard_buttons = 0;

int main(void)
{
	/* System initialization */
	SystemInit();
	
	/* Main clock initialization */
	clock_init();

	/* Update core clock after clock initialization */
	SystemCoreClockUpdate();
	SysTick_Init();
	
	/* Debugging LED initialization */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER |= GPIO_MODER_MODE13_0;
	
	keyboard_gpio_init();
	display_init();
	delay_us(100);
	
	display_print_string("lodc");
	display_update();
	display_clear();
	
	delay_ms(1000);
	
	uint16_t value = 0;
	
	while(1)
	{
		old_keyboard_buttons = keyboard_buttons;
		keyboard_buttons = keyboard_get_buttons();
		
		if(keyboard_buttons > old_keyboard_buttons)
		{
			keyboard_handler(keyboard_buttons);
		}
		
		display_print_value_integer_decimal(value, 2);
		display_print_char('f', 0);
		display_update();
		
		value += 5;
		if(value > 999)
		{
			value = 0;
		}
		
		delay_ms(100);
	}
}