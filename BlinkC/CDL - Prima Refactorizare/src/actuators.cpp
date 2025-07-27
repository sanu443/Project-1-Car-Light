#include "headers/utils.h"
#include "headers/registers.h"
#include "headers/actuators.h"

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1

#define CW 0
#define CCW 1


actuator 
actuator_create(u8 minus_pin, u8 plus_pin)
{
    actuator a;
    a.minus_pin = minus_pin;
    a.plus_pin = plus_pin;
    a.stopped = TRUE;
    reset(&(a.timer_));
    pinMode_(minus_pin, OUTPUT);
    pinMode_(plus_pin, OUTPUT);
    return a;
}

void 
rotate(actuator* act, bool_ cw_0_ccw_1, u32 time,
     bool_ start_new_rotation)
{   
    if (start_new_rotation)
    {   
        reset(&(act->timer_));
        act->stopped = FALSE;
        if (cw_0_ccw_1)
        {
            digitalWrite_(act->plus_pin, !cw_0_ccw_1); // obligatoriu incepem cu low ca sa nu scriem high pe ambii pini
            digitalWrite_(act->minus_pin, cw_0_ccw_1); 
        }
        else
        {   
            digitalWrite_(act->minus_pin, cw_0_ccw_1);
            digitalWrite_(act->plus_pin, !cw_0_ccw_1); 
        }
    }
    if (have_passed(act->timer_, time) || act->stopped)
    {   
        stop_actuator(act);
    }
}

void 
rotate_cw(actuator* act, u32 time, bool_ start_new_rotation)
{   
    rotate(act, CW, time, start_new_rotation);    
}

void 
rotate_counter_cw(actuator* act, u32 time, bool_ start_new_rotation)
{   
    rotate(act, CCW, time, start_new_rotation);
}

void 
stop_actuator(actuator* act)
{
    digitalWrite_(act->minus_pin, LOW); 
    digitalWrite_(act->plus_pin, LOW);
    act->stopped = TRUE;
}

#undef CW
#undef CCW
#undef HIGH 
#undef LOW 
#undef INPUT 
#undef OUTPUT 