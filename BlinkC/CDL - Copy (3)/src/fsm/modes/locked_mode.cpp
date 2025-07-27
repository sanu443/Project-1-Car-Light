#include "fsm/modes/locked_mode.h"
#include "fsm/modes/default_behaviour.h"

void 
locked_on_enter(state* me)
{   
    actuator* door_lock = me->context.door_lock;
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    reset_leds(leds);
    reset_timer(timer_);
    rotate_counter_cw(door_lock, 1000, TRUE);
}

void 
while_in_locked(state* me)
{   
    actuator* door_lock = me->context.door_lock;
    btn_info* door_btn = me->context.door_btn;
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    blink(leds);
    rotate_counter_cw(door_lock, 1000, FALSE);
    if(pressed(door_btn))
    {
        reset_timer(timer_);
    }
}

bool_
transition_cond_locked(state* me, OUT_PARAM door_mode* new_mode)
{   
    btn_info* door_btn = me->context.door_btn;
    timer timer_ = me->context.timer_;

    if(released(door_btn) && have_passed(timer_, 100)
    && !have_passed(timer_, 800))
    {
        *new_mode = DOUBLE_LOCKED;
        return TRUE;
    }
    if(door_btn->state == PRESSED && have_passed(timer_, 800))
    {   
        *new_mode = UNLOCKED;
        return TRUE;
    }
    return FALSE;
}

void
locked_on_exit(state* me)
{
    default_on_exit(me);
}

state
create_locked_state(btn_info* door_btn, actuator* door_lock, actuator* window)
{   
    state locked_state;
    locked_state.mode = LOCKED;
    locked_state.on_enter_state = locked_on_enter;
    locked_state.while_in_state = while_in_locked;
    locked_state.transition_cond = transition_cond_locked;
    locked_state.on_exit_state = locked_on_exit;
    state_context context;
    context.door_btn = door_btn;
    context.door_lock = door_lock;
    context.window = window;
    context.leds = init_leds(LOCKED);
    reset_leds(&context.leds);
    reset_timer(&context.timer_);
    locked_state.context = context; 
    return locked_state;
}