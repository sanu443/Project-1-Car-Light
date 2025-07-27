#include "utils/utils.h"
#include "utils/buttons.h"
#include "devices/lcd_pattern.h"
#include "devices/actuators.h"
#include "fsm/door_states.h"
#include "main_task.h"

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
        init_states(&door_btn, &door_lock, &window);
        update_state(UNLOCKED); // starea initiala
    }
    lcd_print_state(current_state(), door_btn.state);  
    update_btn_state(&door_btn);
    handle_state();
}