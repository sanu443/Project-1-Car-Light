#include "headers/kbd.h"
#include "headers/registers.h"
#include <avr/io.h>


//FUNCTIE INSPIRATA DE DL LEMENI
char 
kbscan_no_debounce(void)
{   
    PORTD = 0;   
    u8 temp;
    //u8 cols;
    u8 lina = 0; //linia activa
    u8 cola = 0; //coloana activa
    //unsigned char cod_intern;
    //char cod_extern
    char tabela_trans[] =
    {
        "123A456B789C*0#D"
    };
    for(DDRD = (1 << 3) | (1 << 7); DDRD != (1 << 7); DDRD = (DDRD << 1) | (1 << 7), lina++) 
    {
        temp = ~PIND & 0b111; 
        //char debug[40];
        //sprintf(debug, "%03d", temp);
        //clrLCD();
        //putsLCD(debug);
        if (temp) 
        {
            cola = 0;
            for (; temp != 1; temp >>= 1, cola++)
            ;
            return tabela_trans[3 * lina + cola];
        } 
    }
    return NOKEY;
}

char
kbscan(void)
{
    static u32 ts_prev = 0;
    static char ch_prev = NOKEY;
    static const i16 DEBOUNCE_DELAY = 30;
    char result = NOKEY;
    if (millis_() - ts_prev >= DEBOUNCE_DELAY)
    {      
        char ch_now = kbscan_no_debounce();
        if (ch_prev == NOKEY && ch_now != NOKEY)
        {
            result = ch_now;
        }
        ts_prev = millis_();
        ch_prev = ch_now;
    }
    return result;
}
