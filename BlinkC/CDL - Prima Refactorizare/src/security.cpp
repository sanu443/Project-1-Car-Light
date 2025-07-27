#include "headers/security.h"
#include "headers/kbd.h"
#include "headers/registers.h"
void 
security_check(void)
{   
    static LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
    static bool_ init = FALSE;
    if (!init)
    {
        init = TRUE;
        lcd_init(&lcd);
    }
    static const char cod[] = "1A6";
    char buf[4] = {NOKEY, NOKEY, NOKEY, '\0'};
    u8 i = 0;
    lcd.setCursor(0, 0);
    lcd.print("Cod: ");
    for(;;)
    {   
        while((buf[i] = kbscan()) == NOKEY);
        lcd.print(buf[i]);
        i++;
        if (i == 3)
        {   
            i = 0;
            if (strcmp(buf, cod) == 0)
            {   
                lcd.setCursor(0, 0);
                lcd.print("ACCES PERMIS");
                delay(1000);
                lcd.clear();
                return;
            }
            else
            {   
                lcd.clear();
                lcd.print("COD INVALID");
                delay(1000);
                lcd.clear();
                lcd.print("Cod: ");
            }
        }
    }
}