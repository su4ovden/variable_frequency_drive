#include "clock.h"

void clock_init(void)
{
	RCC->CR |= RCC_CR_HSEON; // Enable external oscillator
	while(!(RCC->CR & RCC_CR_HSERDY_Msk));

	// Configure FLASH for higher bus frequency
	FLASH->ACR &= ~FLASH_ACR_PRFTEN;
	FLASH->ACR |= FLASH_ACR_PRFTEN; // Enable prefetch
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS; // Change memory access time
	while(!(FLASH->ACR & FLASH_ACR_LATENCY_2WS)); // Wait for changes

	// Configure clock map
	RCC->CFGR &= ~RCC_CFGR_RTCPRE;
	RCC->CFGR |= RCC_CFGR_RTCPRE & (25 << RCC_CFGR_RTCPRE_Pos); // HSE prescaler for RTC (1 MHz)

	RCC->CFGR &= ~RCC_CFGR_HPRE; // AHB no prescaling
	RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB high-speed no prescaling

	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB low-speed with prescaler 2

	// Configure PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; // HSE clock source

	// Reset PLL coefficients registers
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;

	// Set value for PLL coefficients
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM & (25 << RCC_PLLCFGR_PLLM_Pos); // M coifficient
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN & (336 << RCC_PLLCFGR_PLLN_Pos); // N coifficient
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0; // P coifficient division by 4
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ & (8 << RCC_PLLCFGR_PLLQ_Pos);  // Q coifficient

	RCC->CR |= RCC_CR_PLLON; // Enable PLL
	while(!(RCC->CR & RCC_CR_PLLRDY_Msk)); // Wait untill PLL is ready

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // Set PLL as system clock
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)); // Wait until PLL start clocks main bus
}