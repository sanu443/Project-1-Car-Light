#include "utils/utils.h"

#define NR_MAX_TASKS 10
typedef struct 
{   
    void(*tasks[NR_MAX_TASKS])(void);
    u32 period[NR_MAX_TASKS];
    u32 last_exec_times[NR_MAX_TASKS] = {0};
    u8 crt_task = 0;
} scheduler;
#undef NR_MAX_TASKS

void add_task(scheduler* s, void (*task)(void), u8 period);
void check_events(scheduler* s);
