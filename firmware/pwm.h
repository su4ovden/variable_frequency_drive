#include <stm32f4xx.h>

extern void pwm_gpio_init(void);
extern void pwm_timer_init(void);
extern void pwm_enable(void);
extern void pwm_disable(void);

typedef struct
{
	float angle_accumulator;
	float modulation_coeff;
	uint16_t phase_u_angle;
	uint16_t phase_v_angle;
	uint16_t phase_w_angle;
} pwm_control_t;

extern volatile pwm_control_t pwm_control;