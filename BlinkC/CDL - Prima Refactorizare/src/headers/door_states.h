#ifndef DOOR_STATE
#define DOOR_STATE
#include "headers/utils.h"

typedef enum
{
    UNLOCKED =  0,
    LOCKED,
    DOUBLE_LOCKED
} door_state;

bool_ entered_new_state(void);
void update_state(door_state new_mode);
door_state current_state(void);
#endif
