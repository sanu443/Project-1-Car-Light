#include "headers/door_states.h"

static bool_ entered_new_mode = FALSE;
static door_state mode = UNLOCKED;

bool_ 
entered_new_state(void)
{   
    if (entered_new_mode)
    {
        entered_new_mode =  FALSE;
        return TRUE;
    }
    return FALSE;
}

void 
update_state(door_state new_mode)
{   
    if (new_mode != mode)
    {
        entered_new_mode = TRUE;  
        mode = new_mode;
    }
}

door_state 
current_state(void)
{       
    return mode;
}