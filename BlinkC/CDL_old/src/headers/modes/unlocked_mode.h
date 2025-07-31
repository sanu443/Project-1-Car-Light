#include "headers/door_states.h"
#include "headers/door_state_structures.h"


void unlocked_on_enter(state* me);
void while_in_unlocked(state* me);
bool_ transition_cond_unlocked(state* me, OUT_PARAM door_mode* new_mode);
state init_unlocked(void);
state create_unlocked_state(btn_info* door_btn, actuator* door_lock, actuator* window);
