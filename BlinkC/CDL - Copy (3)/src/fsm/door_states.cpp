#include "fsm/door_states.h"
#include "fsm/door_state_structures.h"
#include "fsm/modes/unlocked_mode.h"
#include "fsm/modes/locked_mode.h"
#include "fsm/modes/doublelocked_mode.h"

static state unlckd_state;
static state lckd_state;
static state dblckd_state;

static state* state_ = &unlckd_state;
static state* last_state_ = &unlckd_state;
static bool_ entered_new_mode = FALSE;

void 
init_states(btn_info* btn, actuator* door_lock, actuator* window)
{ 
    unlckd_state = create_unlocked_state(btn, door_lock, window);
    lckd_state = create_locked_state(btn, door_lock, window);
    dblckd_state = create_doublelocked_state(btn, door_lock, window);
}

// void 
// init_locked_state(btn_state* btn, actuator* door_lock, actuator* window)
// {
//     lckd_state = create_locked_state(btn, door_lock, window);
// }

// void 
// init_doublelocked_state(btn_state* btn, actuator* door_lock, actuator* window)
// {
//     dblckd_state = create_doublelocked_state(btn, door_lock, window);
// }

bool_ 
entered_new_state(void)
{   
    if (entered_new_mode)
    {
        entered_new_mode =  FALSE; //consumam flagul
        return TRUE;
    }
    return FALSE;
}

static state* 
state_from_mode(door_mode mode)
{
    switch(mode)
    {
        case UNLOCKED: {return &unlckd_state;} break;
        case LOCKED: {return &lckd_state;} break;
        case DOUBLE_LOCKED: {return &dblckd_state;} break;
    } 
    return (state*)0;
}

void 
update_state(door_mode new_mode)
{   
    entered_new_mode = TRUE;
    last_state_ = state_;
    state_ = state_from_mode(new_mode);
}

door_mode 
current_state(void)
{       
    return state_->mode;
}

door_mode
last_state(void)
{
    return last_state_->mode;
}

void 
handle_state(void)
{   
    if (entered_new_state())
    {
        state_->on_enter_state(state_);
    }
    state_->while_in_state(state_);
    door_mode new_mode;
    if (state_->transition_cond(state_, &new_mode))
    {   
        state_->on_exit_state(state_);
        update_state(new_mode);
    }
}