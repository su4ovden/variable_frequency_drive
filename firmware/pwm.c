#include "pwm.h"
#include "analog.h"
#include "control.h"

#define PWM_MODULATION_FREQ_NOMINAL 50.00f
#define PWM_MODULATION_COEF_MIN 0.2f
#define PWM_MODULATION_COEF_MAX 1.1f

static const float sinewave_array[360] = 
{ 
	0.000, 0.030, 0.060, 0.091, 0.121, 0.151,
	0.180, 0.210, 0.239, 0.268, 0.297, 0.325, 0.353, 0.381, 0.408, 0.435,
	0.462, 0.487, 0.513, 0.538, 0.562, 0.586, 0.609, 0.631, 0.653, 0.674,
	0.695, 0.715, 0.734, 0.752, 0.770, 0.787, 0.804, 0.819, 0.834, 0.849,
	0.862, 0.875, 0.887, 0.899, 0.909, 0.919, 0.929, 0.938, 0.946, 0.953,
	0.960, 0.966, 0.972, 0.977, 0.981, 0.985, 0.989, 0.992, 0.994, 0.996,
	0.998, 0.999, 1.000, 1.000, 1.000, 1.000, 1.000, 0.999, 0.998, 0.997,
	0.996, 0.994, 0.993, 0.991, 0.989, 0.987, 0.986, 0.984, 0.982, 0.980,
	0.978, 0.976, 0.974, 0.973, 0.971, 0.969, 0.968, 0.967, 0.966, 0.965,
	0.964, 0.964, 0.963, 0.963, 0.963, 0.963, 0.963, 0.964, 0.964, 0.965,
	0.966, 0.967, 0.968, 0.969, 0.971, 0.973, 0.974, 0.976, 0.978, 0.980,
	0.982, 0.984, 0.986, 0.987, 0.989, 0.991, 0.993, 0.994, 0.996, 0.997,
	0.998, 0.999, 1.000, 1.000, 1.000, 1.000, 1.000, 0.999, 0.998, 0.996,
	0.994, 0.992, 0.989, 0.985, 0.981, 0.977, 0.972, 0.966, 0.960, 0.953,
	0.946, 0.938, 0.929, 0.919, 0.909, 0.899, 0.887, 0.875, 0.862, 0.849,
	0.834, 0.819, 0.804, 0.787, 0.770, 0.752, 0.734, 0.715, 0.695, 0.674,
	0.653, 0.631, 0.609, 0.586, 0.562, 0.538, 0.513, 0.487, 0.462, 0.435,
	0.408, 0.381, 0.353, 0.325, 0.297, 0.268, 0.239, 0.210, 0.180, 0.151,
	0.121, 0.091, 0.060, 0.030, 0.000, -0.030, -0.060, -0.091, -0.121,
	-0.151, -0.180, -0.210, -0.239, -0.268, -0.297, -0.325, -0.353, -0.381,
	-0.408, -0.435, -0.462, -0.487, -0.513, -0.538, -0.562, -0.586, -0.609,
	-0.631, -0.653, -0.674, -0.695, -0.715, -0.734, -0.752, -0.770, -0.787,
	-0.804, -0.819, -0.834, -0.849, -0.862, -0.875, -0.887, -0.899, -0.909,
	-0.919, -0.929, -0.938, -0.946, -0.953, -0.960, -0.966, -0.972, -0.977,
	-0.981, -0.985, -0.989, -0.992, -0.994, -0.996, -0.998, -0.999, -1.000,
	-1.000, -1.000, -1.000, -1.000, -0.999, -0.998, -0.997, -0.996, -0.994,
	-0.993, -0.991, -0.989, -0.987, -0.986, -0.984, -0.982, -0.980, -0.978,
	-0.976, -0.974, -0.973, -0.971, -0.969, -0.968, -0.967, -0.966, -0.965,
	-0.964, -0.964, -0.963, -0.963, -0.963, -0.963, -0.963, -0.964, -0.964,
	-0.965, -0.966, -0.967, -0.968, -0.969, -0.971, -0.973, -0.974, -0.976,
	-0.978, -0.980, -0.982, -0.984, -0.986, -0.987, -0.989, -0.991, -0.993,
	-0.994, -0.996, -0.997, -0.998, -0.999, -1.000, -1.000, -1.000, -1.000,
	-1.000, -0.999, -0.998, -0.996, -0.994, -0.992, -0.989, -0.985, -0.981,
	-0.977, -0.972, -0.966, -0.960, -0.953, -0.946, -0.938, -0.929, -0.919,
	-0.909, -0.899, -0.887, -0.875, -0.862, -0.849, -0.834, -0.819, -0.804,
	-0.787, -0.770, -0.752, -0.734, -0.715, -0.695, -0.674, -0.653, -0.631,
	-0.609, -0.586, -0.562, -0.538, -0.513, -0.487, -0.462, -0.435, -0.408,
	-0.381, -0.353, -0.325, -0.297, -0.268, -0.239, -0.210, -0.180, -0.151,
	-0.121, -0.091, -0.060, -0.030 	
};

volatile pwm_control_t pwm_control = { 0 };
static float freq_correction_coeff = 0.0f;

// Voltage vector sinthesis
void TIM1_UP_TIM10_IRQHandler(void)
{
	uint16_t field_angle = 0;
	
	field_angle = (uint16_t)pwm_control.angle_accumulator;
	
  pwm_control.phase_u_angle = field_angle % 360;
	
	if(vfd.direction == FORWARD)
	{
		pwm_control.phase_v_angle = (field_angle + 120) % 360;
		pwm_control.phase_w_angle = (field_angle + 240) % 360;
	}
	else
	{
		pwm_control.phase_v_angle = (field_angle + 240) % 360;
		pwm_control.phase_w_angle = (field_angle + 120) % 360;
	}
  
	
	pwm_control.modulation_coeff = ((vfd.freq_curr / (PWM_MODULATION_FREQ_NOMINAL / (1.00f - PWM_MODULATION_COEF_MIN))) + PWM_MODULATION_COEF_MIN);
	pwm_control.modulation_coeff =  pwm_control.modulation_coeff * (ADC_UDC_NOMINAL / adc_get_udc());
	
	if(pwm_control.modulation_coeff > PWM_MODULATION_COEF_MAX)
	{
		pwm_control.modulation_coeff = PWM_MODULATION_COEF_MAX;
	}
	
  TIM1->CCR1 = 360 + (sinewave_array[pwm_control.phase_u_angle] * 360.0f) * pwm_control.modulation_coeff;
  TIM1->CCR2 = 360 + (sinewave_array[pwm_control.phase_v_angle] * 360.0f) * pwm_control.modulation_coeff;
  TIM1->CCR3 = 360 + (sinewave_array[pwm_control.phase_w_angle] * 360.0f) * pwm_control.modulation_coeff;
	
  pwm_control.angle_accumulator += vfd.freq_curr * freq_correction_coeff;
	if(pwm_control.angle_accumulator > 360.00f)
	{
		pwm_control.angle_accumulator -= 360.00f;
	}

  TIM1->SR = ~TIM_SR_UIF; // clear interrupt flag
}

void pwm_timer_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // enable clock to Timer 1

  TIM1->CR1 |= TIM_CR1_ARPE; // ARR preload enable
  TIM1->CR1 |= TIM_CR1_CMS_0; // center-aligment mode, interrupt flag is set both when the counter is counting up or down
	
	// set idle state to 1
	TIM1->CR2 |= TIM_CR2_OIS1 | TIM_CR2_OIS1N | TIM_CR2_OIS2 | TIM_CR2_OIS2N | TIM_CR2_OIS2 | TIM_CR2_OIS3N; 
	
	// set PWM mode 1 and enable CCRx registers preload
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
  TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
  TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;

	// set output compare channels polarity
	TIM1->CCER |= TIM_CCER_CC1P | TIM_CCER_CC2P | TIM_CCER_CC3P; // OC1, OC2, OC3 active low
	TIM1->CCER |= TIM_CCER_CC1NP | TIM_CCER_CC2NP | TIM_CCER_CC3NP; // OC1N, OC2N, OC3N active low
	
	// enable output compare channels
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E; // OC1, OC2, OC3 enable
  TIM1->CCER |= TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE; // OC1N, OC2N, OC3N enable
    
	TIM1->BDTR |= 185; // dead-time 3.0 uS
	
	TIM1->PSC = 22; // 35 - 1620 Hz > 22 - 2540 Hz
  TIM1->ARR = 719;

  TIM1->EGR |= TIM_EGR_COMG; // update generation
    
  // reset compare registers
  TIM1->CCR1 = 0;
  TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;
	TIM1->EGR |= TIM_EGR_UG; // update generation
	
	freq_correction_coeff = 360.0f / (SystemCoreClock / ((TIM1->PSC + 1) * (TIM1->ARR + 1)));
    
  TIM1->DIER |= TIM_DIER_UIE; // enable update interrupt
	
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0); // set highest priority to pwm generation interrupt
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void pwm_gpio_init(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // enable clock on PORTA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // enable clock on PORTB

  GPIOB->MODER 	|= GPIO_MODER_MODE15_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE13_1 | GPIO_MODER_MODE12_1; // alternative function
  GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR13_1; // fast speed
  GPIOB->AFR[1] |= GPIO_AFRH_AFRH7_0 | GPIO_AFRH_AFRH6_0 | GPIO_AFRH_AFRH5_0 | GPIO_AFRH_AFRH4_0; // AF1 TIM1/TIM2

  GPIOA->MODER |= GPIO_MODER_MODE10_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE8_1; // alternative function
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR9_1 | GPIO_OSPEEDER_OSPEEDR8_1; // fast speed
  GPIOA->AFR[1] |= GPIO_AFRH_AFRH2_0 | GPIO_AFRH_AFRH1_0 | GPIO_AFRH_AFRH0_0; // AF1 TIM1/TIM2
}

void pwm_enable(void)
{
  TIM1->BDTR |= TIM_BDTR_MOE; // main output enable
  TIM1->CR1 |= TIM_CR1_CEN; // enable timer
}

void pwm_disable(void)
{
  TIM1->BDTR &= ~TIM_BDTR_MOE; // main output disable
  TIM1->CR1 &= ~TIM_CR1_CEN; // disable timer
}