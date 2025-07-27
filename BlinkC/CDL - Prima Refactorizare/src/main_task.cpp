#include "headers/utils.h"
#include "headers/buttons.h"
#include "headers/lcd.h"
#include "headers/registers.h"
#include "headers/actuators.h"
#include "headers/door_states.h"
#include "headers/timer.h"
#include "headers/leds.h"
#include "headers/security.h"
#include "headers/main_task.h"


void 
unlocked_loop(btn_info* door_btn, actuator* door_lock, actuator* window,
 door_state mode_in_last_iter)
{   
    static timer timer_;
    static blinking_leds leds;
    static door_state last_mode = UNLOCKED;
    static bool_ start_new_rotation = FALSE;
    static bool_ init = 0;
    if (!init)
    {
        init = TRUE;
        leds = init_leds(UNLOCKED);
        reset(&timer_);
    }
    if (entered_new_state())
    {    
        //Serial.println("entered_in_unlocked");
        last_mode = mode_in_last_iter;
        start_new_rotation = TRUE;
        reset_leds(&leds);
        reset(&timer_);
       // Serial.println("Stare noua");
    }
    else
    {
        start_new_rotation = FALSE;
    }
    blink(&leds);
    u32 time = (last_mode == LOCKED) ? 1000 : 2000; 
    rotate_counter_cw(door_lock, time, start_new_rotation);
    if (door_btn->state == PRESSED)
    {
        rotate_cw(window, 3000, start_new_rotation);
    }
    else
    {
        stop_actuator(window);
    }
    if(pressed(door_btn))
    {
        // Serial.println("Pressed btn. UNLOCKED");
        reset(&timer_);
    }
    if(released(door_btn) && have_passed(timer_, 800))
    {   
        update_state(LOCKED);
        // Serial.println("Schimb starea");
    }
}

void 
locked_loop(btn_info* door_btn, actuator* door_lock)
{
    static timer timer_;
    static blinking_leds leds;
    static bool_ init = 0;
    static bool_ start_new_rotation = FALSE;
    if (!init)
    {
        init = TRUE;
        leds = init_leds(LOCKED);
        reset(&timer_);
    }
    if (entered_new_state())
    {       
       reset_leds(&leds);
       reset(&timer_);
       start_new_rotation = TRUE;
    }
    else
    {
        start_new_rotation = FALSE;
    }
    rotate_cw(door_lock, 1000, start_new_rotation);
    blink(&leds);
    if(pressed(door_btn))
    {   
        reset(&timer_);
    }
    if (released(door_btn))
    {   
        Serial.println("Am dat release la buton in modul locked.");
        if (have_passed(timer_, 100) && !have_passed(timer_, 800))
        {
            update_state(DOUBLE_LOCKED);
        }
    }
    if(door_btn->state == PRESSED && have_passed(timer_, 800))
    {   
        Serial.println("Trec in unlocked.");
        update_state(UNLOCKED);
    }
}

void 
double_locked_loop(btn_info* door_btn, 
    actuator* door_lock, actuator* window)
{
    static timer timer_;
    static blinking_leds leds;
    static bool_ init = FALSE;
    static bool_ start_new_rotation = FALSE;
    if (!init)
    {
        init = TRUE;
        leds = init_leds(DOUBLE_LOCKED);
        start_new_rotation = TRUE;
        reset(&timer_);
    }
    if (entered_new_state())
    {      
       reset_leds(&leds);
       reset(&timer_);
       start_new_rotation = TRUE;
    }
    else
    {
        start_new_rotation = FALSE;
    }
    blink(&leds);
    rotate_counter_cw(window, 3000, start_new_rotation);
    rotate_cw(door_lock, 1000, start_new_rotation);
    if(pressed(door_btn))
    {
        // Serial.println("Pressed btn.DOUBLE_LOCKED");
        reset(&timer_);
    }
    if(door_btn->state == PRESSED && have_passed(timer_, 800))
    {
        update_state(UNLOCKED);
        Serial.println("Trec in unlocked.");
    }
}   

void 
main_task(void)
{     
    static btn_info door_btn;
    static actuator door_lock, window;
    static bool_ init = FALSE;
    if (!init)
    {
        init = TRUE;
        door_btn = btn_init(DOOR_BTN);
        door_lock = actuator_create(DOOR_LOCK_MINUS, DOOR_LOCK_PLUS);
        window = actuator_create(WINDOW_MINUS, WINDOW_PLUS);
    }
    door_state mode = current_state();
    print_state(mode, door_btn.state);  
    update_btn_state(&door_btn);
    switch(mode)
    {   
        case UNLOCKED: { unlocked_loop(&door_btn, &door_lock, &window, mode);} break;
        case LOCKED: { locked_loop(&door_btn, &door_lock);} break;
        case DOUBLE_LOCKED: { double_locked_loop(&door_btn, &door_lock, &window);} break;
    }
}
