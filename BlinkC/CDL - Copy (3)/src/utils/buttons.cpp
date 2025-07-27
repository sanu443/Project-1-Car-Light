#include "utils/utils.h"
#include "utils/buttons.h"
#include "utils/registers.h"

#define NR_BTNS 1
#define NR_BTN_TEST_MODES 2
 

bool_ 
btn_status(u8 btn, btn_test mode)
{       
    static u32 ts_prev[NR_BTN_TEST_MODES] = {0}; //[3][2]: 3BUTTONS, 2 MODES
    static bool_ state_prev[NR_BTN_TEST_MODES] = {0};
    bool_ result = 0;
    const u8 DEBOUNCE_DELAY = 20; //CHANGE TO OLD DEBOUNCE IMMEDIATELY   
    if (millis_() - ts_prev[mode] >= DEBOUNCE_DELAY)
    {
        bool_ state_now = digitalRead_(btn);
        if (mode == TEST_PRESS)
        {
            result = !state_prev[mode] && state_now;
        }
        else //mode == TEST_RELEASE
        {
            result = state_prev[mode] &&!state_now;
        }
        state_prev[mode] = state_now;
        ts_prev[mode] = millis_();
    }
    return result;
}

bool_ 
test_press(u8 btn)
{
    return btn_status(btn, TEST_PRESS);
}

bool_ 
test_release(u8 btn)
{
    return btn_status(btn, TEST_RELEASE);
}

bool_ 
released(btn_info* btn)
{
    if (btn->released)
    {
        btn->released = FALSE;
        return TRUE;
    }
    return FALSE;
}

bool_ 
pressed(IN_PARAM btn_info* btn)
{
    if (btn->pressed)
    {
        btn->pressed = FALSE;
        return TRUE;
    }
    return FALSE;
}

void 
update_btn_state(OUT_PARAM btn_info* btn)
{
    switch(btn->state)
    {
        case NOT_PRESSED:
        {
            if (test_press(btn->pin))
            {      
                btn->pressed = TRUE;
                btn->state = PRESSED;
                btn->last_press = millis_();
            }
        }
        break;
        case PRESSED:
        {
            if (test_release(btn->pin))
            {   
                btn->released = TRUE;
                btn->state = NOT_PRESSED;
                btn->last_release = millis_();
            }
        }
        break;
    }
}

btn_info 
btn_init(u8 pin)
{
    btn_info btn;
    btn.pin = pin;
    btn.last_press = btn.last_release = 0;
    btn.pressed = btn.released = FALSE;
    btn.state = NOT_PRESSED;
    return btn;
}

