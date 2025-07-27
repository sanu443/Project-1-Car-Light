#include "utils.h"
#include "timer.h"

#ifndef ACTUATOR_H
#define ACTUATOR_H
#define DOOR_LOCK_PLUS A3
#define DOOR_LOCK_MINUS 7

#define WINDOW_PLUS A5
#define WINDOW_MINUS A4

typedef struct
{
    u8 minus_pin, plus_pin;
    timer timer_;
    bool_ stopped;
}  actuator;


actuator actuator_create(u8 minus_pin, u8 plus_pin);
void rotate_cw(actuator* act, u32 time, bool_ start_new_rotation);
void rotate_counter_cw(actuator* act, u32 time, bool_ start_new_rotation);
void stop_actuator(actuator*);
#endif

