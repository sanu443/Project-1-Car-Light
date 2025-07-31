#ifndef STATE_STRUCTURES_H
#define STATE_STRUCTURES_H
#include "utils/utils.h"
#include "utils/buttons.h"
#include "devices/actuators.h"
#include "devices/leds.h"
#include "utils/timer.h"

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
    void (*on_exit_state)(state* me);
} state;
#endif