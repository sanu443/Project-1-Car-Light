#ifndef LCD_H
#define LCD_H
#include "utils/utils.h"

void lcd_print(const char text[]);
void lcd_print_char(char ch);
void lcd_init();
void lcd_setCursor(u8 col, u8 lin);
#endif