#include <stm32f401xc.h>

extern uint16_t adc_raw_data[5];
extern volatile uint8_t adc_seq_cmlpt_flag;

void adc1_init(void);