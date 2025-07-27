#ifndef TIMER
#define TIMER
#include "headers/utils.h"

typedef struct
{
    u32 time_started;
} timer;

void reset(timer* t);
bool_ have_passed(timer t, u32 time_in_ms);
#endif
