#include <stm32f401xc.h>

void relay_gpio_init(void);
void relay_set_state(uint8_t state);
void fan_gpio_init(void);
void fan_set_state(uint8_t state);
