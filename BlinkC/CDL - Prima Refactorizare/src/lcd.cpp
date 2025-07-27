#include "headers/lcd.h"
#include "headers/registers.h"

void 
lcd_init(LiquidCrystal* lcd)
{   
    pinMode_(RS, OUTPUT);
    pinMode_(EN, OUTPUT);
    pinMode_(D4, OUTPUT);
    pinMode_(D5, OUTPUT);
    pinMode_(D6, OUTPUT);
    pinMode_(D7, OUTPUT);
    lcd->begin(16, 2);
    delay(10);
    lcd->clear();
    delay(10);
}

void 
print_state(door_state mode, btn_state door_btn)
{   
    static LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
    static bool_ init = FALSE;
    if (!init)
    {
        init = TRUE;
        lcd_init(&lcd);
    }
     //NU FOLOSESC CLEAR, CASTIG TIMP (dau override si paddez cu spatii)
    static door_state last_mode = (door_state)0x10; //STARE INVALIDA IMI INTRA PE last_mode != mode 
    if (last_mode != mode)
    {      
        last_mode = mode;
        static const char* mode_lut[] = 
        {   
            "UNLOCKED     ",     
            "LOCKED       ",     
            "DOUBLE_LOCKED",        
        };
        lcd.setCursor(0, 0);
        lcd.print(mode_lut[mode]);
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
        lcd.setCursor(0,1);
        // APPEND MAI EFICIENT PE AVERAGE DAR LUT E MAI BUN PE WORST CASE
        lcd.print(btn_state_lut[btn_state]);    
    }
}