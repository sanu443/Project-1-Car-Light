#include "fsm/modes/unlocked_mode.h"
#include "fsm/modes/default_behaviour.h"

void 
unlocked_on_enter(state* me)
{   
    actuator* door_lock = me->context.door_lock;
    actuator* window = me->context.window;
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    reset_leds(leds);
    reset_timer(timer_);
    u32 time = (last_state() == LOCKED) ? 1000 : 2000; 
    rotate_counter_cw(door_lock, time, TRUE);
    rotate_cw(window, 3000, TRUE);
}

void 
while_in_unlocked(state* me)
{   
    actuator* door_lock = me->context.door_lock;
    actuator* window = me->context.window;
    btn_info* door_btn = me->context.door_btn;
    blinking_leds* leds = &(me->context.leds);
    timer* timer_ = &(me->context.timer_);
    blink(leds);
    u32 time = (last_state() == LOCKED) ? 1000 : 2000; 
    rotate_counter_cw(door_lock, time, FALSE);
    if (door_btn->state == PRESSED)
    {
        rotate_cw(window, 3000, FALSE);
    }
    else
    {
        stop_actuator(window);
    }
    if(pressed(door_btn))
    {
        reset_timer(timer_);
    }
}

bool_
transition_cond_unlocked(state* me, OUT_PARAM door_mode* new_mode)
{   
    btn_info* door_btn = me->context.door_btn;
    timer timer_ = me->context.timer_;

    if(released(door_btn) && have_passed(timer_, 800))
    {   
        *new_mode = LOCKED;
        return TRUE;
    }
    return FALSE;
}

void
unlocked_on_exit(state* me)
{
    default_on_exit(me);
}

state
create_unlocked_state(btn_info* door_btn, actuator* door_lock, actuator* window)
{   
    state unlocked_state;
    unlocked_state.mode = UNLOCKED;
    unlocked_state.on_enter_state = unlocked_on_enter;
    unlocked_state.while_in_state = while_in_unlocked;
    unlocked_state.transition_cond = transition_cond_unlocked;
    unlocked_state.on_exit_state = unlocked_on_exit;
    state_context context;
    context.door_btn = door_btn;
    context.door_lock = door_lock;
    context.window = window;
    context.leds = init_leds(UNLOCKED);
    reset_leds(&context.leds);
    reset_timer(&context.timer_);
    unlocked_state.context = context; 
    return unlocked_state;
}