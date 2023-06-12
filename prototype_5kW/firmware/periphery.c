#include "periphery.h"

void relay_gpio_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock to GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE10_0; // B10 General purpose output mode
}

void relay_set_state(uint8_t state)
{
	GPIOB->ODR = (GPIOB->ODR & ~GPIO_ODR_OD10) | (state << GPIO_ODR_OD10_Pos);
}

void fan_gpio_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock to GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE1_0; // B1 General purpose output mode
}

void fan_set_state(uint8_t state)
{
	GPIOB->ODR = (GPIOB->ODR & ~GPIO_ODR_OD1) | (state << GPIO_ODR_OD1_Pos);
}