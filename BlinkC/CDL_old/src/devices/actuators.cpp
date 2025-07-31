#include "utils/utils.h"
#include "utils/registers.h"
#include "devices/actuators.h"

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
    reset_timer(&(a.timer_));
    pinMode_(minus_pin, OUTPUT);
    pinMode_(plus_pin, OUTPUT);
    return a;
}

void 
rotate(actuator* act, bool_ cw_0_ccw_1, u32 time)
{   
    reset_timer(&(act->timer_));
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
    actuator_command cmd;
    cmd.exec_time = time;
    cmd.cw_0_ccw_1 = cw_0_ccw_1;
    cmd.active = TRUE;
    act->cmd = cmd;
}

void 
rotate_cw(actuator* act, u32 time)
{   
    rotate(act, CW, time);    
}

void 
rotate_counter_cw(actuator* act, u32 time)
{   
    rotate(act, CCW, time);
}

void 
monitor_actuator(actuator* act)
{   
    if (act->cmd.active && have_passed(act->timer_, act->cmd.exec_time))
    {   
        stop_actuator(act);
    }
}

void 
stop_actuator(actuator* act)
{
    digitalWrite_(act->minus_pin, LOW); 
    digitalWrite_(act->plus_pin, LOW);
    act->cmd.active = FALSE;
}

#undef CW
#undef CCW
#undef HIGH 
#undef LOW 
#undef INPUT 
#undef OUTPUT 