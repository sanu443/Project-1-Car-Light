#ifndef LEDS_H
#define LEDS_H
#include "utils.h"
#include "timer.h"
#include "door_states.h"
#include "registers.h"

#define LED0 14
#define LED1 15

typedef struct 
{
    u8 cycles, count, nr_blinks;
    timer led_timer;
    u8 pin0, pin1;
} blinking_leds;

void blink(blinking_leds* leds);
blinking_leds init_leds(door_mode mode);
void reset_leds(blinking_leds* leds);
#endif