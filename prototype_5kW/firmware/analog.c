#include "analog.h"

extern volatile uint16_t adc_value;

void ADC_IRQHandler(void)
{
    adc_value = ADC1->DR >> 2;

    ADC1->SR = ~ADC_SR_EOC; /* clear interrupt flag */
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