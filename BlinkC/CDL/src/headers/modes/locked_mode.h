#include "headers/door_states.h"
#include "headers/door_state_structures.h"


void locked_on_enter(state* me);
void while_in_locked(state* me);
bool_ transition_cond_locked(state* me, OUT_PARAM door_mode* new_mode);
state init_locked(void);
state create_locked_state(btn_info* door_btn, actuator* door_lock, actuator* window);
