#include "fsm/door_states.h"
#include "fsm/door_state_structures.h"

void doublelocked_on_enter(state* me);
void while_in_doublelocked(state* me);
bool_ transition_cond_doublelocked(state* me, OUT_PARAM door_mode new_mode);
void doublelocked_on_exit(state* me);
state init_doublelocked(void);
state create_doublelocked_state(btn_info* door_btn, actuator* door_lock, actuator* window);