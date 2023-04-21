#include "delay.h"

void SysTick_Init(void);
void delay_us(uint32_t delay);
void delay_ms(uint32_t delay);

static uint32_t CLK_PERIOD = 0;

void SysTick_Init(void)
{
	CLK_PERIOD = SystemCoreClock / (1000000 - 1);
	
	SysTick->LOAD &= ~SysTick_LOAD_RELOAD_Msk;
	SysTick->LOAD = SysTick_VAL_CURRENT_Msk;
	SysTick->VAL &= ~SysTick_LOAD_RELOAD_Msk;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void delay_us(uint32_t us_value)
{
	uint32_t start_systick_value = SysTick->VAL;
	while (SysTick->VAL > (start_systick_value - (us_value * CLK_PERIOD)));
	SysTick->VAL = 0;
}

void delay_ms(uint32_t ms_value)
{
	while(ms_value)
	{
		ms_value--;
		delay_us(1000);
	}
}

/*
void delay_ms(uint16_t ms_value)
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL &= ~SysTick_VAL_CURRENT_Msk;
	SysTick->VAL = SystemCoreClock / (1000 - 1);

	systick_count = ms_value;

	while(systick_count);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
*/