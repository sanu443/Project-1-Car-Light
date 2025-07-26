#ifndef LCD_H
#define LCD_H
#include "buttons.h"
#include "door_states.h"

void lcd_print_state(door_mode mode, btn_state btn);
void lcd_print(const char text[]);
void lcd_print_char(char ch);
void lcd_init();
void lcd_setCursor(u8 col, u8 lin);
#endif