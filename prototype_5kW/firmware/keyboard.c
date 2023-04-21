#include "keyboard.h"

void keyboard_gpio_init(void)
{
	// Enable clock for GPIOA and GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// Enable pull-down for input keyboard pins
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD10_1 | GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD1_1 | GPIO_PUPDR_PUPD0_1;
	
	// GPIO ports mode set
	GPIOA->MODER |= GPIO_MODER_MODE7_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE4_0; // Output
	GPIOB->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE2 | GPIO_MODER_MODE1 | GPIO_MODER_MODE0); // Input
}

uint16_t keyboard_get_buttons(void)
{
	uint16_t buttons = 0;
	
	for(uint8_t i = 0; i < 4; i++)
	{
		GPIOA->ODR &= ~0xF0;
		GPIOA->ODR |= GPIO_ODR_OD4 << i;
		buttons |= (((GPIOB->IDR & 0x400) >> 7) | (GPIOB->IDR & 0x07)) << (i * 4);
	}
	
	return buttons;
}

void keyboard_handler(buttons_t buttons)
{
	/*
	for(uint8_t i = 0; i < 16; i++)
	{
		if(keyboard_buttons & (1 << i))
		{
			switch(i)
			{
				case 0: 
				GPIOC->ODR ^= GPIO_ODR_OD13;
				//GPIOC->ODR |= GPIO_ODR_OD13;
				break;
				
				case 1: 
				GPIOC->ODR ^= GPIO_ODR_OD13;
				//GPIOC->ODR &= ~GPIO_ODR_OD13;
				break;
			}
		}
	}
	*/
	
	switch(buttons)
	{
		case ON:
			GPIOC->ODR |= GPIO_ODR_OD13;
			break;
		case OFF:
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			break;
		default:
			break;
	}
}