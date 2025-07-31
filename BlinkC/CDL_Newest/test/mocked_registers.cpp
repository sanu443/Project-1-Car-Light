#include "utils/registers.h"
#include <chrono>
#define NR_PINS 28

using namespace std;

static u8 pins[NR_PINS] = {0};

void 
digitalWrite_(u8 pin, u8 val)
{   
    pins[pin] = val;
}

u8 
digitalRead_(u8 pin)
{
    return pins[pin];
}

void 
pinMode_(u8 pin, u8 mode)
{
    return; //toti pinii sunt de iesire nu ma mai complic
}

//LUAT DE PE GEEKSFORGEEKS
u32 
millis_(void)
{
    // Get the current time from the system clock
    auto now = chrono::system_clock::now();

    // Convert the current time to time since epoch
    auto duration = now.time_since_epoch();

    // Convert duration to milliseconds
    auto milliseconds
        = chrono::duration_cast<chrono::milliseconds>(
            duration)
            .count();
    return milliseconds;
}