#include "headers/utils.h"
#include "headers/buttons.h"
#include "headers/lcd.h"
#include "headers/actuators.h"
#include "headers/door_states.h"
#include "headers/main_task.h"

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
    }
    lcd_print_state(current_state(), door_btn.state);  
    update_btn_state(&door_btn);
    handle_state();
}
