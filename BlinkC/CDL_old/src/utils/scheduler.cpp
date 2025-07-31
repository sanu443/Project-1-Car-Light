#include "utils/scheduler.h"
#include "utils/registers.h"


void 
add_task(scheduler* s, void (*task)(void), u8 period)
{
    s->tasks[s->crt_task] = task;
    s->period[s->crt_task] = period; 
    s->crt_task++;
}

void 
check_events(scheduler* s)
{   
    for (u8 task = 0; task < s->crt_task; task++)
    {
        if (millis_() - s->last_exec_times[task] >= s->period[task])
        {   
            s->last_exec_times[task] = millis_(); 
            (s->tasks[task])();
        }
    }
}