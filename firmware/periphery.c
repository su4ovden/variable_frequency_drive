#include "periphery.h"

void relay_gpio_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // enable clock to GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE10_0; // B10 general purpose output mode
}

void relay_set_state(uint8_t state)
{
	GPIOB->ODR = (GPIOB->ODR & ~GPIO_ODR_OD10) | (state << GPIO_ODR_OD10_Pos);
}

void fan_gpio_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // enable clock to GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE1_0; // B1 general purpose output mode
}

void fan_set_state(uint8_t state)
{
	GPIOB->ODR = (GPIOB->ODR & ~GPIO_ODR_OD1) | (state << GPIO_ODR_OD1_Pos);
}