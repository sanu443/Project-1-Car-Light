#include "utils.h"
#include "timer.h"
#include "door_states.h"
#include "registers.h"

#define A0 14
#define A1 15

#define LED0 A0
#define LED1 A1

typedef struct 
{
    u8 cycles, count, nr_blinks;
    timer led_timer;
    u8 pin0, pin1;
} blinking_leds;

void blink(blinking_leds* leds);
blinking_leds init_leds(door_state mode);
void reset_leds(blinking_leds* leds);