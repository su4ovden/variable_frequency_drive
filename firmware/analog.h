#include <stm32f401xc.h>

#define ADC_RAW_TO_VOLTAGE_COEFF (3.3f / 4096.0f)
#define ADC_UDC_DEVIDER_COEFF 129.0f
#define ADC_UDC_NOMINAL 310.0f

typedef enum
{
	DC_VOLTAGE = 0,
	PHASE_U_CURRENT,
	PHASE_V_CURRENT,
	TEMPERATURE,
	USER_ANALOG
} adc_channel_t;

extern volatile uint8_t adc_conv_cmlpt_flag;

extern void adc1_init(void);
extern float adc_get_udc(void);
extern uint16_t adc_get_raw(adc_channel_t channel);