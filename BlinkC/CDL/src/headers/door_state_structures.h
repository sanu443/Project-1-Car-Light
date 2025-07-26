#ifndef STATE_STRUCTURES_H
#define STATE_STRUCTURES_H
#include "headers/utils.h"
#include "headers/buttons.h"
#include "headers/actuators.h"
#include "headers/leds.h"
#include "headers/timer.h"

typedef struct
{   
    btn_info* door_btn;
    actuator* door_lock, *window;
    blinking_leds leds;
    timer timer_;
} state_context;

typedef struct state
{   
    door_mode mode;
    state_context context;
    void (*on_enter_state)(state* me);
    void (*while_in_state)(state* me);
    bool_ (*transition_cond)(state* me, OUT_PARAM door_mode* new_mode);
} state;
#endif