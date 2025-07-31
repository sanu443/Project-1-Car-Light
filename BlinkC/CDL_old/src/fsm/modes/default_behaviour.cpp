#include "fsm/modes/default_behaviour.h"
#include "fsm/door_states.h"
#include "devices/actuators.h"
#include "devices/leds.h"

void 
default_on_exit(state* me)
{
    actuator* window = me->context.window;
    actuator* door_lock = me->context.door_lock;
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    stop_actuator(window);
    stop_actuator(door_lock);
    reset_leds(leds);
    reset_timer(timer_);
}
