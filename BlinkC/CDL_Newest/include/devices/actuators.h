#include "utils/utils.h"
#include "utils/timer.h"

#ifndef ACTUATOR_H
#define ACTUATOR_H
#define DOOR_LOCK_PLUS 17
#define DOOR_LOCK_MINUS 7

#define WINDOW_PLUS 19
#define WINDOW_MINUS 18

typedef struct 
{
    u32 exec_time;
    bool_ cw_0_ccw_1;
    bool_ active;
}  actuator_command;

typedef struct
{
    u8 minus_pin, plus_pin;
    timer timer_;
    actuator_command cmd;
}  actuator;


actuator actuator_create(u8 minus_pin, u8 plus_pin);
void rotate_cw(actuator* act, u32 time);
void monitor_actuator(actuator* act);
void rotate_counter_cw(actuator* act, u32 time);
void stop_actuator(actuator* act);
#endif

