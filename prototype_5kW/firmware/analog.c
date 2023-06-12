#include "analog.h"

volatile uint8_t adc_seq_cmlpt_flag = 0;
uint16_t adc_raw_data[5] = { 0 };

void ADC_IRQHandler(void)
{
	adc_seq_cmlpt_flag = 1;
    ADC1->SR = ~ADC_SR_EOC; // clear interrupt flag
}

static void adc1_dma2_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN; // Enable clock to DMA2
	
	// Data size - half-word (16-bit) for MEM and PER, memory pointer increment, circular mode
	DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_CIRC;
	DMA2_Stream0->NDTR = 5; // 5 ADC channels -> 5 transfers
	DMA2_Stream0->PAR = (uint32_t)&ADC1->DR; // Source address
	DMA2_Stream0->M0AR = (uint32_t)&adc_raw_data; // Destination address
	
	DMA2_Stream0->CR |= DMA_SxCR_EN; // Enable DMA
}

void adc1_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable clock to GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable clock to ADC1

	// Analog mode on A0-A4 pins
    GPIOA->MODER |= GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2;  
	GPIOA->MODER |= GPIO_MODER_MODER3 | GPIO_MODER_MODER4;
	
	ADC1->CR1 |= ADC_CR1_SCAN; // Enable Scan Mode
    ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_DDS | ADC_CR2_DMA; // Continious conversation mode, DMA Enable

	// Channels 0-4 - 480 sampling cycles
    ADC1->SMPR2 |= ADC_SMPR2_SMP0 | ADC_SMPR2_SMP1 | ADC_SMPR2_SMP2; 
	ADC1->SMPR2 |= ADC_SMPR2_SMP3 | ADC_SMPR2_SMP4;
    
	// 5 channels seq. length, 0 > .. > 4
	ADC1->SQR1 = (5 - 1) << ADC_SQR1_L_Pos;
	ADC1->SQR3 |= (2 << ADC_SQR3_SQ3_Pos) | (1 << ADC_SQR3_SQ2_Pos) | (0 << ADC_SQR3_SQ1_Pos);
	ADC1->SQR3 |= (4 << ADC_SQR3_SQ5_Pos) | (3 << ADC_SQR3_SQ4_Pos);	
	
	adc1_dma2_init();
	
	ADC1->CR1 |= ADC_CR1_EOCIE; // Enable end of conversation interrupt
    NVIC_EnableIRQ(ADC_IRQn); // Enable ADC global interrupt in NVIC
	
    // Enable ADC, Start conversation
	ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}