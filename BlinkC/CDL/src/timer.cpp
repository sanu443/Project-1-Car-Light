#include "headers/timer.h"
#include "headers/registers.h"

void 
reset_timer(timer* t)
{
    t->time_started = millis_();
}

bool_ 
have_passed(timer t, u32 time_in_ms)
{
    return (millis_() - t.time_started >= time_in_ms);
}