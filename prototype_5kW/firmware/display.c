#include "display.h"
#include "delay.h"

/* Local functions */
static void display_gpio_init(void);
static inline void clock_signal(uint8_t state);
static inline void data_signal(uint8_t state);
static inline void send_start(void);
static inline void send_stop(void);
static inline uint8_t is_ack(void);
static inline void send_byte(uint8_t byte);
static inline void reset_state(void);

/* Global functions */
uint8_t display_send_send_command(uint8_t command);
uint8_t display_send_send_command_data(uint8_t command, uint8_t* data, uint8_t length);
void display_print_value_integer(uint16_t value);
void display_print_value_integer_decimal(uint16_t value, uint8_t dot_position);
void display_set_brightness(uint8_t brightness);
void display_print_string(char* string);
void display_print_string_pos(char* string, uint8_t position);
void display_print_char(char symbol, uint8_t position);
void display_init(void);
void display_test(void);
void display_clear(void);

static const uint8_t symbols_table[] = 
{
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 
	0x6D, 0x7D, 0x07, 0x7F, 0x6F, 
	0x77, 0x7C, 0x39, 0x5E, 0x79, 
	0x71, 0x6F, 0x74, 0x04, 0x1E,
	0x76, 0x38, 0x15, 0x54, 0x5C,
	0x73, 0x67, 0x50, 0x6D, 0x78,
	0x3E, 0x1C, 0x2A, 0x76, 0x6E,
	0x5B, 0x00
}; /* All printable symbols (0-9/A-Z) */

static uint8_t display_data[4] = { 0 }; /* Display data buffer in RAM */

static void display_gpio_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* Enable clock for GPIOB */
	
	GPIOB->ODR |= GPIO_ODR_OD8 | GPIO_ODR_OD9;
	GPIOB->MODER |= GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0; /* General purpose output mode */
	GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9; /* Open-drain */
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0; /* Pulled up */
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
}

static void clock_signal(uint8_t state)
{
	/* 
	Works incorrectly
	CLK_GPIO->BSRR = (1 << (CLK_PORT + 16)) >> (16 * (state & 0x01));
	*/
	
	if(state)
	{
		CLK_GPIO->ODR |= (1 << CLK_PORT);
	}
	else
	{
		CLK_GPIO->ODR &= ~(1 << CLK_PORT);
	}
}

static void data_signal(uint8_t state)
{
	/* 
	Works incorrectly
	DIO_GPIO->BSRR = (1 << (DIO_PORT + 16)) >> (16 * (state & 0x01)); 
	*/
	
	if(state)
	{
		DIO_GPIO->ODR |= (1 << DIO_PORT);
	}
	else
	{
		DIO_GPIO->ODR &= ~(1 << DIO_PORT);
	}
}

static void send_start(void)
{
	data_signal(0);
	delay_us(BIT_PERIOD);
}

static void send_stop(void)
{
	data_signal(0);
	
	clock_signal(0);
	delay_us(BIT_PERIOD);
	
	clock_signal(1);
	delay_us(BIT_PERIOD);
	
	data_signal(1);
}

static uint8_t is_ack(void)
{
	uint8_t is_ack = 0;
	
	clock_signal(0);
	delay_us(BIT_PERIOD);
	
	if(!(GPIOB->IDR & GPIO_IDR_ID7))
	{
		is_ack = 1;
	}
	
	clock_signal(1);
	delay_us(BIT_PERIOD);
	
	return is_ack;
}

static void send_byte(uint8_t byte)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		clock_signal(0);
		delay_us(BIT_PERIOD);
		
		data_signal(byte & (1 << i));
		
		clock_signal(1);
		delay_us(BIT_PERIOD);
	}
	
	data_signal(1); /* Reset to idle state */
}

static void reset_state(void)
{
	data_signal(1);
	clock_signal(1);
}

uint8_t display_send_raw_command(uint8_t command)
{	
	send_start();
	send_byte(command);
	
	if(!is_ack())
	{
		reset_state();
		return 1;
	}
	
	send_stop();
	
	return 0;
}

uint8_t display_send_raw_command_data(uint8_t command, uint8_t* data, uint8_t length)
{
	send_start();
	send_byte(command);
	
	if(!is_ack())
	{
		reset_state();
		return 1;
	}
	
	for(uint8_t i = 0; i < length; i++)
	{
		send_byte(data[i]);
		
		if(!is_ack())
		{
			reset_state();
			return 1;
		}
	}
	
	send_stop();
	
	return 0;
}

void display_init(void)
{
	display_gpio_init();
	display_send_raw_command(0x40);
	delay_us(1000);
	display_send_raw_command(0x8F);
	delay_us(1000);
}

void display_test(void)
{
	uint8_t test_pattern[4] = { 0 };
	
	/* Fill all segments */
	for(uint8_t i = 0; i < 4; i++)
	{
		for(uint8_t j = 0; j < 8; j++)
		{
			test_pattern[i] |= (1 << j);
			display_send_raw_command_data(0xC0, test_pattern, 4);
			delay_us(50000);
		}
	}
	
	display_send_raw_command_data(0xC0, test_pattern, 4);
	
	/* Test for all levels of brightness */
	for(uint8_t i = 7; i != 0; i--)
	{
		display_send_raw_command(0x88 | i);
		delay_us(75000);
	}
	
	for(uint8_t i = 0; i < 8; i++)
	{
		display_send_raw_command(0x88 | i);
		delay_us(75000);
	}
	
	/* Hardware ON/OFF function test */
	for(uint8_t i = 0; i < 4; i++)
	{
		display_send_raw_command(0x87 | ((i & 0x01) << 3));
		delay_us(190000);
	}
}

uint8_t get_symbol(uint8_t index)
{
	if(index > 36)
	{
		return 0;
	}
	
	return symbols_table[index];
}

void display_print_value_integer(uint16_t value)
{
	for(int8_t i = 3; i >= 0; i--)
	{
		if(value > 0)
		{
			display_data[i] = get_symbol(value % 10);
		}
		else
		{
			display_data[i] = 0;
		}
		value /= 10;
	}
}

void display_print_value_integer_decimal(uint16_t value, uint8_t dot_position)
{
	for(int8_t i = 3; i >= 0; i--)
	{
		if(value > 0 || (i >= dot_position))
		{
			display_data[i] = get_symbol(value % 10);
		}
		else
		{
			display_data[i] = 0;
		}
		value /= 10;
	}
	
	if(dot_position > 0 && dot_position <= 4)
	{
		display_data[4 - dot_position] |= 1 << 7;
	}
}

void display_print_char(char symbol, uint8_t position)
{
	if(symbol >= 'a' && symbol <= 'z')
	{
		display_data[position] = get_symbol(symbol - 87);
	}
	else if(symbol >= 'A' && symbol <= 'Z')
	{
		display_data[position] = get_symbol(symbol - 55);
	}
	else if(symbol == ' ')
	{
		display_data[position] = 0;
	}
}

void display_print_string(char* string)
{
	for(int8_t i = 3; i >= 0; i--)
	{
		display_print_char(string[i], i);
	}
}

void display_print_string_pos(char* string, uint8_t position)
{
	if(position > 3)
	{
		return;
	}

	for(int8_t i = (3 - position); i >= 0; i--)
	{
		display_print_char(string[i], i);
	}
}

void display_set_brightness(uint8_t brightness)
{
	display_send_raw_command(0x88 | (brightness & 0x07));
}

void display_update(void)
{
	display_send_raw_command_data(0xC0, display_data, 4);
}

void display_clear(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		display_data[i] = 0;
	}
}
