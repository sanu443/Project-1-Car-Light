#include "devices/leds.h"

#define OUTPUT 1
#define HIGH 1
#define LOW 0

void
reset_leds(blinking_leds* leds)
{
    leds->cycles = 0;
    leds->count = 0;
    digitalWrite_(leds->pin0, LOW);
    digitalWrite_(leds->pin1, LOW);
    reset_timer(&(leds->led_timer));
}

blinking_leds 
init_leds(door_mode mode)
{
    u8 nr_blinks[] = {2, 1, 3};
    blinking_leds leds; 
    leds.pin0 = LED0;
    leds.pin1 = LED1;
    pinMode_(leds.pin0, OUTPUT);
    pinMode_(leds.pin1, OUTPUT);
    leds.nr_blinks = nr_blinks[mode];
    reset_leds(&leds);
    return leds;
}

void 
blink(blinking_leds* leds)
{   
    if (leds->cycles >= leds->nr_blinks)
    {   
        return;
    }
    if (have_passed(leds->led_timer, 100)) 
    {   
        reset_timer(&(leds->led_timer));
        if (leds->count == 0)
        {
            digitalWrite_(leds->pin0, HIGH);
            digitalWrite_(leds->pin1, HIGH);
        }
        else if (leds->count  ==  3)
        {
            digitalWrite_(leds->pin0, LOW);
            digitalWrite_(leds->pin1, LOW);
        }
        leds->count++;
        if (leds->count == 10)
        {
            leds->count = 0;
            leds->cycles++;
        }
    }
}
#undef OUTPUT 
#undef HIGH 
#undef LOW  