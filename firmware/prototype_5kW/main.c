#include <stm32f4xx.h>
#include <system_stm32f4xx.h>

volatile uint64_t systick_count = 0;

volatile uint16_t phase_accumulator = 0;
volatile uint16_t phase_a = 0;
volatile uint16_t phase_b = 0;
volatile uint16_t phase_c = 0;
volatile uint16_t adc_value = 0;

uint8_t button_state = 0;
uint8_t led_state = 0;
uint8_t input_button = 0;

const uint16_t sinewave_array[360] = {
    180, 185, 190, 196, 201, 207, 212, 217, 223,
	228, 233, 239, 244, 249, 255, 260, 265, 270, 276, 281, 286, 291, 296,
	301, 306, 311, 316, 321, 325, 330, 335, 336, 338, 339, 341, 342, 343,
	345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 354, 355, 356, 356,
	357, 357, 358, 358, 358, 359, 359, 359, 359, 359, 359, 359, 359, 359,
	358, 358, 358, 357, 357, 356, 356, 355, 354, 354, 353, 352, 351, 350,
	349, 348, 347, 346, 345, 343, 342, 341, 339, 338, 336, 335, 336, 338,
	339, 341, 342, 343, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354,
	354, 355, 356, 356, 357, 357, 358, 358, 358, 359, 359, 359, 359, 359,
	359, 359, 359, 359, 358, 358, 358, 357, 357, 356, 356, 355, 354, 354,
	353, 352, 351, 350, 349, 348, 347, 346, 345, 343, 342, 341, 339, 338,
	336, 335, 330, 325, 321, 316, 311, 306, 301, 296, 291, 286, 281, 276,
	270, 265, 260, 255, 249, 244, 239, 233, 228, 223, 217, 212, 207, 201,
	196, 190, 185, 180, 174, 169, 163, 158, 152, 147, 142, 136, 131, 126,
	120, 115, 110, 104, 99, 94, 89, 83, 78, 73, 68, 63, 58, 53, 48, 43, 38,
	34, 29, 24, 23, 21, 20, 18, 17, 16, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3,
	3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 24,
	23, 21, 20, 18, 17, 16, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 4, 3, 3,
	2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 24, 29, 34, 38,
	43, 48, 53, 58, 63, 68, 73, 78, 83, 89, 94, 99, 104, 110, 115, 120, 126,
	131, 136, 142, 147, 152, 158, 163, 169, 174
};

void SysTick_Init(void)
{
    SysTick->LOAD &= ~SysTick_LOAD_RELOAD_Msk;
    SysTick->LOAD = SystemCoreClock / (1000 - 1);
    SysTick->VAL &= ~SysTick_VAL_CURRENT_Msk;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(void)
{
    if(systick_count > 0) systick_count--;
}

void delay_ms(uint16_t ms_value)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL &= ~SysTick_VAL_CURRENT_Msk;
    SysTick->VAL = SystemCoreClock / (1000 - 1);

    systick_count = ms_value;

    while(systick_count);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void clock_init(void)
{
    RCC->CR |= RCC_CR_HSEON; /* Enable external oscillator */
    while(!(RCC->CR & RCC_CR_HSERDY_Msk));

    /* Configure FLASH */
    FLASH->ACR &= ~FLASH_ACR_PRFTEN;
    FLASH->ACR |= FLASH_ACR_PRFTEN; /* Enable prefetch */

    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2WS; /* Change memory access time */
    while(!(FLASH->ACR & FLASH_ACR_LATENCY_2WS)); /* Wait for changes */

    /* Configure clock map */
    RCC->CFGR &= ~RCC_CFGR_RTCPRE;
    RCC->CFGR |= RCC_CFGR_RTCPRE & ((uint8_t)25 << RCC_CFGR_RTCPRE_Pos); /* HSE prescaler for RTC (1 MHz) */

    RCC->CFGR &= ~RCC_CFGR_HPRE; /* AHB no prescaling */
    RCC->CFGR &= ~RCC_CFGR_PPRE2; /* APB high-speed no prescaling*/

    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; /* APB low-speed with prescaler 2*/

    /* Configure PLL */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; /* HSE clock source */

    /* Reset PLL coefficients registers */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ & ((uint8_t)8 << RCC_PLLCFGR_PLLQ_Pos);  /* Q coifficient */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM & ((uint8_t)25 << RCC_PLLCFGR_PLLM_Pos); /* M coifficient */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN & ((uint16_t)328 << RCC_PLLCFGR_PLLN_Pos); /* N coifficient */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0; /* P coifficient division by 4 */

    RCC->CR |= RCC_CR_PLLON; /* Enable PLL */
    while(RCC->CR & RCC_CR_PLLRDY_Msk); /* Wait PLL */

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL; /* PLL used as system clock */
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)); /* Wait until PLL start clocks main bus */
}

void adc1_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; /* Enable clock to GPIOA */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* Enable clock to ADC 1 */

    GPIOA->MODER |= GPIO_MODER_MODE0_1 | GPIO_MODER_MODE0_0; /* Analog mode on A0 pin */
    ADC1->CR2 |= ADC_CR2_CONT; /* Continious conversation mode */

    ADC1->SMPR2 |= ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_0; /* Channel 0 144 sampling cycles*/
    
    ADC1->CR1 |= ADC_CR1_EOCIE; /* Enable end of conversation interrupt */
    NVIC_EnableIRQ(ADC_IRQn); /* Enable ADC global interrupt in NVIC */
    
    ADC1->CR2 |= ADC_CR2_ADON; /* Enable ADC */
    ADC1->CR2 |= ADC_CR2_SWSTART; /* Start conversation */
}

void timer1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; /* Enable clock to Timer 1 */

    TIM1->CR1 |= TIM_CR1_ARPE; /* ARR preload enable */
    TIM1->CR1 |= TIM_CR1_CMS_0; /* Center-aligment mode */

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /* CCR1 PWM mode 1 */
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE; /* CCR1 preload enable */
    
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; /* CCR2 PWM mode 1 */
    TIM1->CCMR1 |= TIM_CCMR1_OC2PE; /* CCR2 preload enable */
    
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; /* CCR3 PWM mode 1 */
    TIM1->CCMR2 |= TIM_CCMR2_OC3PE; /* CCR3 preload enable */

    TIM1->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC1P); /* OC1 and OC1N Active high */
    TIM1->CCER &= ~(TIM_CCER_CC2NP | TIM_CCER_CC2P); /* OC2 and OC2N Active high */
    TIM1->CCER &= ~(TIM_CCER_CC3NP | TIM_CCER_CC3P); /* OC3 and OC3N Active high */

    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E; /* OC1, OC2, OC3 enable */
    TIM1->CCER |= TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE; /* OC1N, OC2N, OC3N enable */
    
    TIM1->PSC = (uint16_t)500;
    TIM1->ARR = (uint16_t)359;

    TIM1->EGR |= TIM_EGR_COMG; /* Update generation */
    
    /* Reset compare registers */
    TIM1->CCR1 = (uint8_t)0;
    TIM1->CCR2 = (uint8_t)0;
    TIM1->CCR3 = (uint8_t)0;

    TIM1->EGR |= TIM_EGR_UG; /* Update generation */
    
    TIM1->DIER |= TIM_DIER_UIE; /* Enable update interrupt */
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void timer1_enable()
{
    TIM1->BDTR |= TIM_BDTR_MOE; /* Main output enable */
    TIM1->CR1 |= TIM_CR1_CEN; /* Enable timer */
}

void timer1_disable()
{
    TIM1->BDTR &= ~TIM_BDTR_MOE; /* Main output enable */
    TIM1->CR1 &= ~TIM_CR1_CEN; /* Disable timer */
}

void pwm_channels_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; /* Enable clock on PORTA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* Enable clock on PORTB */

    GPIOB->MODER |= GPIO_MODER_MODE15_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE13_1; /* Alternative function */
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR13_1; /* Fast speed */
    GPIOB->AFR[1] |= GPIO_AFRH_AFRH7_0 | GPIO_AFRH_AFRH6_0 | GPIO_AFRH_AFRH5_0; /* AF1 TIM1/TIM2 */

    GPIOA->MODER |= GPIO_MODER_MODE10_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE8_1; /* Alternative function */
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR9_1 | GPIO_OSPEEDER_OSPEEDR8_1; /* Fast speed */
    GPIOA->AFR[1] |= GPIO_AFRH_AFRH2_0 | GPIO_AFRH_AFRH1_0 | GPIO_AFRH_AFRH0_0; /* AF1 TIM1/TIM2 */
}

int main(void)
{
    /* System initialization */
    SystemInit();
    
    /* Clock initialization */
    clock_init();

    /* Update core clock after clock initialization */
    SystemCoreClockUpdate();
    SysTick_Init();

    /* Configure timer and channels */
    pwm_channels_init();
    timer1_init();
    adc1_init();

    /* GPIO */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; /* Enable clock on PORTC */
    GPIOC->MODER |= GPIO_MODER_MODE13_0; /* Output mode on PINC13 */
    GPIOC->BSRR = GPIO_BSRR_BR_13;
    
    timer1_enable();

    while(1)
    {
        if(input_button && !button_state)
        {
            button_state = 1;
            if(led_state)
            {
                GPIOC->BSRR = GPIO_BSRR_BR_13;
                led_state = 0;
            }
            else
            {
                GPIOC->BSRR = GPIO_BSRR_BS_13;
                timer1_disable();
                led_state = 1;
            }
        }
        else if(!input_button && button_state)
        {
            button_state = 0;
        }
    }
}

void TIM1_UP_TIM10_IRQHandler(void)
{
    phase_a = (phase_accumulator >> 6) % 360;
    phase_b = ((phase_accumulator >> 6) + 120) % 360;
    phase_c = ((phase_accumulator >> 6) + 240) % 360;

    TIM1->CCR1 = sinewave_array[phase_a] * (adc_value / 1024.0);
    TIM1->CCR2 = sinewave_array[phase_b] * (adc_value / 1024.0);
    TIM1->CCR3 = sinewave_array[phase_c] * (adc_value / 1024.0);

    phase_accumulator += adc_value;

    TIM1->SR = ~TIM_SR_UIF; /* clear interrupt flag */
}

void ADC_IRQHandler(void)
{
    adc_value = ADC1->DR >> 2;

    ADC1->SR = ~ADC_SR_EOC; /* clear interrupt flag */
}