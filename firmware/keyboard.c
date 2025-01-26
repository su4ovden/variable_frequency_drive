#include "keyboard.h"
#include "delay.h"

void keyboard_gpio_init(void)
{
	// Enable clock for GPIOB and GPIOC
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
	
	/*
	// Enable pull-down for input keyboard pins
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD4);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD4_1;
	
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD15 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD13);
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD15_1 | GPIO_PUPDR_PUPD14_1 | GPIO_PUPDR_PUPD13_1;
	*/
	
	// Enable pull-up for input keyboard pins
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD4);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD4_0;
	
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD15 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD13);
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD15_0 | GPIO_PUPDR_PUPD14_0 | GPIO_PUPDR_PUPD13_0;
	
	// GPIO ports mode set
	GPIOC->MODER |= GPIO_MODER_MODE15_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE13_0; // Output
	GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE5 | GPIO_MODER_MODE4); // Input
	
	//GPIOC->ODR &= ~0xE000;
	GPIOC->ODR |= 0xE000;
}

uint16_t keyboard_get_buttons(void)
{
	uint16_t buttons = 0;
	
	/*
	for(uint8_t i = 0; i < 3; i++)
	{
		GPIOC->ODR |= GPIO_ODR_OD15 >> i;
		buttons |= ((GPIOB->IDR & 0x70) >> 4) << (i * 3);
		GPIOC->ODR &= ~0xE000;
	}
	*/
	
	for(uint8_t i = 0; i < 3; i++)
	{
		GPIOC->ODR &= ~(GPIO_ODR_OD15 >> i);
		buttons |= (((~GPIOB->IDR) & 0x70) >> 4) << (i * 3);
		GPIOC->ODR |= 0xE000;
	}
	
	return buttons;
}