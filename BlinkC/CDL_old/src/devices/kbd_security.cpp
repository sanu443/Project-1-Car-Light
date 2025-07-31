#include "devices/kbd_security.h"
#include "utils/kbd.h"
#include "utils/registers.h"
#include "utils/lcd.h"
#include <string.h>
#include <util/delay.h>
void 
security_check(void)
{   
    static const char cod[] = "1A6";
    char buf[4] = {'\0'};
    u8 i = 0;
    lcd_setCursor(0, 0);
    lcd_print("Cod: ");
    for(;;)
    {   
        while((buf[i] = kbscan()) == NOKEY);
        lcd_print_char(buf[i]);
        i++;
        if (i == 3)
        {   
            i = 0;
            if (strcmp(buf, cod) == 0)
            {   
                lcd_setCursor(0, 0);
                lcd_print("ACCES PERMIS");
                _delay_ms(1000);
                return;
            }
            else
            {   
                lcd_setCursor(0, 0);
                lcd_print("COD INVALID");
                _delay_ms(1000);
                lcd_setCursor(0, 0);
                lcd_print("           ");
                lcd_setCursor(0, 0);
                lcd_print("Cod: ");
            }
        }
    }
}