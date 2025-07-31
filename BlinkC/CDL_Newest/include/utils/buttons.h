#ifndef BUTTONS_H
#define BUTTONS_H
#include "utils.h"

#define DOOR_BTN 16

typedef enum
{
    TEST_PRESS = 0,
    TEST_RELEASE
} btn_test;

typedef enum
{   
    NOT_PRESSED = 0,
    PRESSED = 1
} btn_state;


typedef struct 
{   
    u8 pin;
    btn_state state;
    bool_ pressed;
    bool_ released;
    u32 last_press;
    u32 last_release;
} btn_info;

bool_ released(btn_info* btn);
bool_ pressed(btn_info* btn);
void update_btn_state(btn_info* btn);
btn_info btn_init(u8 pin);
#endif