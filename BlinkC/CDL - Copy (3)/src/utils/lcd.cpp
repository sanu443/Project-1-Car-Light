#include "utils/lcd.h"
#include "utils/registers.h"
#include <LiquidCrystal.h>


#define RS 8
#define EN 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13  

static LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void 
lcd_init()
{   
    pinMode_(RS, OUTPUT);
    pinMode_(EN, OUTPUT);
    pinMode_(D4, OUTPUT);
    pinMode_(D5, OUTPUT);
    pinMode_(D6, OUTPUT);
    pinMode_(D7, OUTPUT);
    lcd.begin(16, 2);
    delay(10);
    lcd.clear();
    delay(10);
}

void 
lcd_print(const char text[])
{   
    lcd.print(text);
}

void 
lcd_print_char(char ch)
{
    lcd.print(ch);
}

void 
lcd_setCursor(u8 col, u8 lin)
{
    lcd.setCursor(col, lin);
}
