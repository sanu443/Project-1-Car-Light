#include "fsm/modes/doublelocked_mode.h"
#include "fsm/modes/default_behaviour.h"

void 
doublelocked_on_enter(state* me)
{   
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    actuator* door_lock = me->context.door_lock;
    actuator* window = me->context.window;
    reset_leds(leds);
    reset_timer(timer_);
    rotate_counter_cw(window, 3000, TRUE);
    rotate_cw(door_lock, 1000, TRUE);
}

void 
while_in_doublelocked(state* me)
{   
    actuator* door_lock = me->context.door_lock;
    actuator* window = me->context.window;
    btn_info* door_btn = me->context.door_btn;
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    blink(leds);
    rotate_counter_cw(window, 3000, FALSE);
    rotate_cw(door_lock, 1000, FALSE);
    if(pressed(door_btn))
    {
        reset_timer(timer_);
    }
}

bool_
transition_cond_doublelocked(state* me, OUT_PARAM door_mode* new_mode)
{   
    btn_info* door_btn = me->context.door_btn;
    timer timer_ = me->context.timer_;

    if(door_btn->state == PRESSED && have_passed(timer_, 800))
    {   
        *new_mode = UNLOCKED;
        return TRUE;
    }
    return FALSE;
}

void
doublelocked_on_exit(state* me)
{
    default_on_exit(me);
}

state
create_doublelocked_state(btn_info* door_btn, actuator* door_lock, actuator* window)
{   
    state doublelocked_state;
    doublelocked_state.mode = DOUBLE_LOCKED;
    doublelocked_state.on_enter_state = doublelocked_on_enter;
    doublelocked_state.while_in_state = while_in_doublelocked;
    doublelocked_state.transition_cond = transition_cond_doublelocked;
    doublelocked_state.on_exit_state = doublelocked_on_exit;
    state_context context;
    context.door_btn = door_btn;
    context.door_lock = door_lock;
    context.window = window;
    context.leds = init_leds(DOUBLE_LOCKED);
    reset_leds(&context.leds);
    reset_timer(&context.timer_);
    doublelocked_state.context = context; 
    return doublelocked_state;
}