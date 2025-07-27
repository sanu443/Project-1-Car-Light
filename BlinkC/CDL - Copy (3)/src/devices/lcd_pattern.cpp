#include "utils/lcd.h"
#include "devices/lcd_pattern.h"

void lcd_print_state(door_mode mode, btn_state door_btn)
{  
     //NU FOLOSESC CLEAR, CASTIG TIMP (dau override si paddez cu spatii)
    static door_mode last_mode = (door_mode)0x10; //STARE INVALIDA IMI INTRA PE last_mode != mode 
    if (last_mode != mode)
    {      
        last_mode = mode;
        static const char* mode_lut[] = 
        {   
            "UNLOCKED     ",     
            "LOCKED       ",     
            "DOUBLE_LOCKED",        
        };
        lcd_setCursor(0, 0);
        lcd_print(mode_lut[mode]);
    }
    static u8 last_btn_state  = 0x10; //STARE INVALIDA IMI INTRA PE last_btn_state != btn_states
    u8 btn_state = door_btn; 
    if (last_btn_state != btn_state)
    {
        last_btn_state = btn_state;
        ///HAZARD RIGHT LEFT
        static const char* btn_state_lut[] =
        {
            "        ",
            "DOOR_BTN"
        };
        lcd_setCursor(0,1);
        // APPEND MAI EFICIENT PE AVERAGE DAR LUT E MAI BUN PE WORST CASE
        lcd_print(btn_state_lut[btn_state]);    
    }
}
