#ifndef DOOR_STATE
#define DOOR_STATE
#include "utils/utils.h"
#include "utils/buttons.h"
#include "devices/actuators.h"

typedef enum
{
    UNLOCKED =  0,
    LOCKED,
    DOUBLE_LOCKED
} door_mode;


bool_ entered_new_state(void);
void update_state(door_mode new_mode);
door_mode last_state(void);
door_mode current_state(void);
void handle_state(void);
void init_states(btn_info* btn, actuator* door_lock, actuator* window);


#endif
