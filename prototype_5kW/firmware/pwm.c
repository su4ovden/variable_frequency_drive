#include "pwm.h"

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

volatile uint16_t adc_value = 0;
volatile uint16_t phase_accumulator = 0;
volatile uint16_t phase_a = 0;
volatile uint16_t phase_b = 0;
volatile uint16_t phase_c = 0;

void TIM1_UP_TIM10_IRQHandler(void)
{
    phase_a = (phase_accumulator >> 6) % 360;
    phase_b = ((phase_accumulator >> 6) + 120) % 360;
    phase_c = ((phase_accumulator >> 6) + 240) % 360;

    TIM1->CCR1 = sinewave_array[phase_a] * (adc_value / 1024.0);
    TIM1->CCR2 = sinewave_array[phase_b] * (adc_value / 1024.0);
    TIM1->CCR3 = sinewave_array[phase_c] * (adc_value / 1024.0);

    phase_accumulator += adc_value;

    TIM1->SR = ~TIM_SR_UIF; // clear interrupt flag
}

void pwm_timer_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // Enable clock to Timer 1

    TIM1->CR1 |= TIM_CR1_ARPE; // ARR preload enable
    TIM1->CR1 |= TIM_CR1_CMS_0; // Center-aligment mode

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // CCR1 PWM mode 1
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE; // CCR1 preload enable
    
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // CCR2 PWM mode 1
    TIM1->CCMR1 |= TIM_CCMR1_OC2PE; // CCR2 preload enable
    
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; // CCR3 PWM mode 1
    TIM1->CCMR2 |= TIM_CCMR2_OC3PE; // CCR3 preload enable

    TIM1->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC1P); // OC1 and OC1N Active high
    TIM1->CCER &= ~(TIM_CCER_CC2NP | TIM_CCER_CC2P); // OC2 and OC2N Active high
    TIM1->CCER &= ~(TIM_CCER_CC3NP | TIM_CCER_CC3P); // OC3 and OC3N Active high

    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E; // OC1, OC2, OC3 enable
    TIM1->CCER |= TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE; // OC1N, OC2N, OC3N enable
    
    TIM1->PSC = (uint16_t)500;
    TIM1->ARR = (uint16_t)359;

    TIM1->EGR |= TIM_EGR_COMG; // Update generation
    
    // Reset compare registers
    TIM1->CCR1 = (uint8_t)0;
    TIM1->CCR2 = (uint8_t)0;
    TIM1->CCR3 = (uint8_t)0;

    TIM1->EGR |= TIM_EGR_UG; // Update generation
    
    TIM1->DIER |= TIM_DIER_UIE; // Enable update interrupt
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void pwm_channels_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable clock on PORTA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock on PORTB

    GPIOB->MODER |= GPIO_MODER_MODE15_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE13_1; // Alternative function
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR13_1; // Fast speed
    GPIOB->AFR[1] |= GPIO_AFRH_AFRH7_0 | GPIO_AFRH_AFRH6_0 | GPIO_AFRH_AFRH5_0; // AF1 TIM1/TIM2

    GPIOA->MODER |= GPIO_MODER_MODE10_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE8_1; // Alternative function
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR9_1 | GPIO_OSPEEDER_OSPEEDR8_1; // Fast speed
    GPIOA->AFR[1] |= GPIO_AFRH_AFRH2_0 | GPIO_AFRH_AFRH1_0 | GPIO_AFRH_AFRH0_0; // AF1 TIM1/TIM2
}

void pwm_enable()
{
    TIM1->BDTR |= TIM_BDTR_MOE; // Main output enable
    TIM1->CR1 |= TIM_CR1_CEN; // Enable timer
}

void pwm_disable()
{
    TIM1->BDTR &= ~TIM_BDTR_MOE; // Main output enable
    TIM1->CR1 &= ~TIM_CR1_CEN; // Disable timer
}