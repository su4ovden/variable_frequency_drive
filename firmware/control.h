#include <stm32f4xx.h>

typedef enum
{
	FORWARD = 0,
	REVERSE
} rotation_direction_t;

typedef enum
{
	STOPPED = 0,
	STARTING,
	RUNNING,
	STOPPING
} operation_state_t;

typedef struct
{
	float starting;
	float stopping;
	float change;
} acceleration_t;

typedef struct 
{
	float freq_set;
	float freq_curr;
	rotation_direction_t direction;
	operation_state_t state;
	acceleration_t accacceleration;
	uint16_t dc_component_duration;
} vfd_control_t;

extern void control_acceleration_timer_init(void);
extern void control_acceleration_timer_enable(void);
extern void control_acceleration_timer_disable(void);
extern void control_vfd_init(void);

extern vfd_control_t vfd;