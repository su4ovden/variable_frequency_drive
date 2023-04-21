#include "display_i2c.h"

// Warning! The area is under reconstruction!
void i2c1_init(void)
{
	// I2C1 GPIO initialization
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock for GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1; // Alternative function
	GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7; // Open-drain
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0; // Pulled up
	GPIOB->AFR[0] |= GPIO_AFRL_AFRL6_2 | GPIO_AFRL_AFRL7_2; // Alternative function ¹4 I2C1
	
	// Peripherial initialization
	I2C1->CR2 |= 32; // 32 MHz peripherial clock
	I2C1->CCR |= 160; // 100 kHz SCL frequency 160
	
	I2C1->TRISE = 32;
	
	I2C1->CR1 |= I2C_CR1_PE; // Enable I2C1
	
	// Reset I2C1 peripherial
	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~I2C_CR1_SWRST;
}


void display_send_raw_commandd(uint8_t command)
{	
	I2C1->CR1 |= I2C_CR1_START; // Send start to the bus
	while(!(I2C1->SR1 & I2C_SR1_SB)); // Wait until start be sended
	
	(void)I2C1->SR1; // Dummy read of status register 1
	I2C1->DR = command; // Send command in address secion
	
	while(!(I2C1->SR1 & I2C_SR1_ADDR)); // Wait until address flag has been reset
	
	(void)I2C1->SR1; // Dummy read of status register 1
	(void)I2C1->SR2; // Dummy read of status register 2
	
	I2C1->CR1 |= I2C_CR1_STOP; // Send stop to the bus
}

void display_send_raw_command_datad(uint8_t command, uint8_t data)
{
	I2C1->CR1 |= I2C_CR1_START; // Send start to the bus
	while(!(I2C1->SR1 & I2C_SR1_SB)); // Wait until start be sended
	
	(void)I2C1->SR1; // Dummy read of status register 1
	I2C1->DR = command; // Send command in address secion
	
	while(!(I2C1->SR1 & I2C_SR1_ADDR)); // Wait until address flag has been reset
	
	(void)I2C1->SR1; // Dummy read of status register 1
	(void)I2C1->SR2; // Dummy read of status register 2
	
	for(uint8_t i = 0; i < 4; i++)
	{	
		I2C1->DR = data;
	}
	
	I2C1->CR1 |= I2C_CR1_STOP; // Send stop to the bus
}
