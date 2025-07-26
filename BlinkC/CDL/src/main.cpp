#include "headers/scheduler.h"
#include "headers/main_task.h"
#include "headers/security.h"
#include "headers/lcd.h"
#include <Arduino.h>

void 
setup() 
{   
    //Serial.begin(9600);
    lcd_init();
    security_check();
}

void 
loop(void)
{   
    static scheduler s;
    static bool_ sched_init = FALSE;
    if (!sched_init)
    {   
        sched_init = TRUE;
        const u8 HOW_OFTEN_RUN_MAIN_LOOP = 20;
        add_task(&s, main_task, HOW_OFTEN_RUN_MAIN_LOOP);
    }
    check_events(&s);
}
