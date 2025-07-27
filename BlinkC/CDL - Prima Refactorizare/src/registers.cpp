#include "headers/registers.h"
#include <avr/io.h> //pentru PORT,DDR etc.
#include <avr/interrupt.h>

typedef enum
{
    P_A = 0, P_B, P_C, P_D
} port_;

void 
locate_pin (IN_PARAM u8 id, OUT_PARAM port_* port, OUT_PARAM u8* nr)
{   
    //nr e pe 3 biti si port pe 2 => folosesc un singur u8 
    //era mai elegant sa folosesc un struct cu port pe 2 biti si nr pe 3 biti
    //dar era putin mai greu de initializat
    //prefer sa folosesc lut fiindca nu la ROM am bottleneck ci la RAM si exec time
    // static const u8 port_lut[] = 
    // {
    //     P_D << 3 + 0, P_D << 3 + 1, P_D << 3 + 2, P_D << 3 + 3, 
    //     P_D << 3 + 4, P_D << 3 + 5, P_D << 3 + 6, P_D << 3 + 7, 
        
    // }
    if (id < 8) //PD0-PD7
    {
        *port = P_D;
        *nr = id;
        return;
    }
    id -= 8;
    if (id < 6) //PB0 - PB5
    {
        *port = P_B;
        *nr = id;
        return;
    }
    id -= 6;
    if (id < 6) //PC0 - PC5
    {
        *port = P_C;
        *nr = id;
        return;
    }
    *port = (port_)(-1); //EROARE 
    *nr = 8; //EROARE
}

void 
digitalWrite_(u8 pin, u8 val)
{
    port_ port; u8 nr;
    locate_pin(pin, &port, &nr);
    switch(port) 
    {   
        case P_C: {setbit(PORTC, nr, val);} break;
        case P_D: {setbit(PORTD, nr, val);} break;
        case P_B: {setbit(PORTB, nr, val);} break;
        default: break;
    }
}


u8
digitalRead_(u8 pin)
{
    port_ port; u8 nr;
    locate_pin(pin, &port, &nr);
    switch(port) 
    {
        case P_C: { return getbit(PINC, nr);} break;
        case P_D: { return getbit(PIND, nr);} break;
        case P_B: { return getbit(PINB, nr);} break;
        default: break;
    }
    ////z(9);
    return 2; //EROARE
}

void
pinMode_(u8 pin, u8 mode)
{
    port_ port; u8 nr;
    locate_pin(pin, &port, &nr);
    switch(port) 
    {
        case P_C: {setbit(DDRC, nr, mode);} break;
        case P_D: {setbit(DDRD, nr, mode);} break;
        case P_B: {setbit(DDRB, nr, mode);} break;
        default: break; 
    }
}

void
timer0_init(void)
{   
    // p = prescaler N =OCR  k = de cate ori incrementam in intrerupere 
    // ca sa obtinem 1 ms
    //   1ms = N * p * k * T_clk
    //     => T_clk * 1ms = 16Mhz * 1ms = 16 000 = 
    //     N * p * k = 2^4 * 2^3 * 5^3 = 2^7 * 5^3
    //     alegem p = 2^6 = 64 si N = 2 * 5^3 = 250, k = 1

    TCCR0A = 0b00000010; //OC-urile disconnected si WGM pe CTC
    TCCR0B = 0b00000011; //p = 64, WGM pe CTC, FOC nu mai stiu ce sunt si nu le folosim
    OCR0A = 250 - 1;
}

static volatile u32 cnt = 0;

u32
millis_(void)
{   
    static bool_ init = FALSE;
    if (!init)
    {   
        init = TRUE;
        timer0_init();
        setbit(TIMSK0, OCIE0A, 1);  
        return 0;
    }

    u32 val1 = cnt;
    u32 val2 = cnt;
    return (val1 < val2) ? val1 : val2;
}

ISR(TIMER0_COMPA_vect)
{
    cnt++;
    //DEBUG
    // if(cnt % 1000 == 0)
    // digitalWrite_(6, !digitalRead_(6));
    //END DEBUG
}