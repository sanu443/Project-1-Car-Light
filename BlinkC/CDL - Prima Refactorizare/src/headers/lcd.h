#ifndef LCD_H
#define LCD_H
#include <LiquidCrystal.h>
#include "buttons.h"
#include "door_states.h"
#define RS 8
#define EN 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13  

void print_state(door_state mode, btn_state btn);
void lcd_init(LiquidCrystal* lcd);
#endif