#include "control.h"
#include "pwm.h"

void control_acceleration_timer_init(void);
void control_acceleration_timer_enable(void);
void control_acceleration_timer_disable(void);
void control_vfd_init(void);

extern uint8_t need_reverse;

vfd_control_t vfd = { 0 };

// acceleration control timer
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
	float current_frequency = vfd.freq_curr;
	
	switch(vfd.state)
	{
		case STARTING:
			if(current_frequency == 0.0f)
			{
				pwm_enable();
			}
			
			current_frequency += vfd.accacceleration.starting;
		
			if(current_frequency >= vfd.freq_set)
			{
				current_frequency = vfd.freq_set;
				vfd.state = RUNNING;
			}
			break;
		case STOPPING:
			current_frequency -= vfd.accacceleration.stopping;
			
			if(current_frequency <= 0.0f)
			{
				current_frequency = 0.0f;
				
				if(vfd.dc_component_duration == 0)
				{
					vfd.dc_component_duration = 0;
					pwm_disable();
					//control_acceleration_timer_disable();
					vfd.state = STOPPED;
				}
				
				vfd.dc_component_duration--;
			}
			break;
		case RUNNING:
			if(current_frequency < vfd.freq_set)
			{
				current_frequency += vfd.accacceleration.change;
				if(current_frequency > vfd.freq_set)
				{
					current_frequency = vfd.freq_set;
				}
			} 
			else if(current_frequency > vfd.freq_set)
			{
				current_frequency -= vfd.accacceleration.change;
				if(current_frequency < vfd.freq_set)
				{
					current_frequency = vfd.freq_set;
				}
			}
			
			if(need_reverse)
			{
				vfd.state = STOPPING;
			}
			break;
		case STOPPED:
			if(need_reverse)
			{
				if(vfd.direction == FORWARD)
				{
					vfd.direction = REVERSE;
				}
				else
				{
					vfd.direction = FORWARD;
				}
				
				vfd.state = STARTING;
				need_reverse = 0;
			}
			break;
		default:
			break;
	}
	
	vfd.freq_curr = current_frequency;
	TIM11->SR &= ~TIM_SR_UIF; // clear interrupt flag
}

void control_acceleration_timer_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM11EN; // enable clock to Timer 11
	
	TIM11->CR1 |= TIM_CR1_ARPE; 	// enable auto-reload preload
	
	TIM11->PSC = 83;
	TIM11->ARR = 999; 						// 1 kHz -> 1 ms interval
	TIM11->EGR |= TIM_EGR_UG;			// update generation
	
	TIM11->DIER |= TIM_DIER_UIE; 	// enable update interrupt
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 1);
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	
	TIM11->CR1 |= TIM_CR1_CEN;
}

void control_acceleration_timer_enable(void)
{
	TIM11->CNT = 0;
	TIM11->CR1 |= TIM_CR1_CEN;
}

void control_acceleration_timer_disable(void)
{
	TIM11->CR1 &= ~TIM_CR1_CEN;
	TIM11->CNT = 0;
}

void control_vfd_init(void)
{
	vfd.state = STOPPED;
	vfd.direction = FORWARD;
	vfd.freq_curr = 0.0f;
	vfd.freq_set = 0.0f;
	vfd.accacceleration.change = 0.05f;
	vfd.accacceleration.starting = 0.2f;
	vfd.accacceleration.stopping = 0.15f;
	vfd.dc_component_duration = 100; // in milliseconds
}