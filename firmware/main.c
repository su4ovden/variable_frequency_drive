#include <stm32f4xx.h>
#include <system_stm32f4xx.h>

#include "main.h"
#include "clock.h"
#include "keyboard.h"
#include "delay.h"
#include "display.h"
#include "pwm.h"
#include "periphery.h"
#include "analog.h"
#include "control.h"

#define ADC_AVERAGE_CYCLES 200

void vfd_init(void);
void keyboard_handler(buttons_t buttons);

uint16_t old_keyboard_buttons = 0;
uint16_t keyboard_buttons = 0;
uint16_t adc_average_cycle = 0;
uint32_t adc_average = 0;
uint8_t need_reverse = 0;

int main(void)
{
	// system initialization
	SystemInit();
	
	// main clock initialization
	clock_init();

	// update core clock after clock initialization
	SystemCoreClockUpdate();
	SysTick_Init();
	
	control_vfd_init();
	adc1_init();
	relay_gpio_init();
	fan_gpio_init();
	keyboard_gpio_init();
	display_init();
	pwm_gpio_init();
	pwm_timer_init();
	control_acceleration_timer_init();
	control_acceleration_timer_enable();
	
	for(uint8_t i = 0; i < 6; i++)
	{
		if((i & 0x01) == 0)
		{
			display_print_string("E0.00");
		}
		else
		{
			display_clear();
		}
		
		display_update();
		delay_ms(500);
	}
	
	relay_set_state(1);
	fan_set_state(1);
	
	display_print_value_integer_decimal(0, 2);
	display_print_char('f', 0);
	display_update();
	
	while(1)
	{
		keyboard_buttons = keyboard_get_buttons();
		
		if(keyboard_buttons != old_keyboard_buttons) // single press
		{
			keyboard_handler(keyboard_buttons);
			old_keyboard_buttons = keyboard_buttons;
		}
		
		if(adc_conv_cmlpt_flag)
		{
			adc_average += (adc_get_raw(USER_ANALOG) & (uint16_t)~0x03) >> 1;
			//adc_average += adc_get_raw(DC_VOLTAGE) & (uint16_t)~0x3;
			adc_average_cycle++;
			
			if(adc_average_cycle == ADC_AVERAGE_CYCLES)
			{
				vfd.freq_set = 0.5f * (uint16_t)(199 * ((adc_average / ADC_AVERAGE_CYCLES) / 2045.0f)) + 0.5f;
				display_print_value_integer_decimal(vfd.freq_curr * 10, 2);
				display_print_char('f', 0);
				//display_print_value_integer(((adc_average / ADC_AVERAGE_CYCLES) * ADC_RAW_TO_VOLTAGE_COEFF) * ADC_UDC_DEVIDER_COEFF);
				display_update();
				
				adc_average = 0;
				adc_average_cycle = 0;
			}
			
			adc_conv_cmlpt_flag = 0;
		}
		
		// <---- place display_update() here, no, bad idea (15.09.2024)
	}
}

void keyboard_handler(buttons_t buttons)
{
	switch(buttons)
	{
		case START:
			if(vfd.state != RUNNING)
			{
				//control_acceleration_timer_enable();
				vfd.state = STARTING;
			}
			break;
		case STOP:
			if(vfd.state != STOPPED)
			{
				vfd.state = STOPPING;
			}
			break;
		case FWD_REV:
			need_reverse = 1;
			break;
		case 0b1000:
			break;
		default:
			break;
	}
}