//AVANATAJE LA REFACTORING:
//NU MAI DUPLIC COD PENTRU LEFT SI RIGHT
//AM LOGICA SEPARATA PENTRU DECLANSAREA UNUI MOD SI PRIORITATI
//AM PRINTUL LA LCD MAI EFICIENT

//DEZAVANTAJE
//OCUPA DE 3 ORI MAI MULT RAM



// DEBUG PORTS:
// 11 - imi intra pe un mod neimplementat in switch
// 9 - right button e pe NOT_PRESSED
// 8 - right button e pe PRESSED
// 7 - au trecut 500ms de la apasarea unui buton
// 10 - am intrat pe lange_change_right
// 6 - imi intra pe normal left
#include <Arduino.h>
#include <LiquidCrystal.h>

#define OUT_PARAM
#define IN_PARAM
#define IN_OUT_PARAM
#define TRUE 1
#define FALSE 0 

#define max2(a,b) (((a) > (b)) ? (a) : (b))
#define max3(a,b,c) max2(max2((a), (b)), (c))

#define NR_BTNS 3
#define NR_BTN_TEST_MODES 2

#define MAX_ANALOG 1023UL
#define NR_LEDS 8

#define LEFT_LED A0
#define RIGHT_LED A1
#define POTENTIOMETER A5    

#define LEFT_BTN 0
#define RIGHT_BTN 1
#define HAZARD_BTN 2

#define LEFT_BTN_PIN PD0
#define RIGHT_BTN_PIN PD1
#define HAZARD_BTN_PIN PD2

#define RS 3    
#define EN 4
#define D4 5
#define D5 12
#define D6 13
#define D7 A4


typedef unsigned long long u64;
typedef unsigned char u8;
typedef u8 bool_;
typedef int i16;

typedef enum
{
    TEST_PRESS = 0,
    TEST_RELEASE
} btn_test;

typedef enum
{   
    NOT_PRESSED = 0,
    PRESSED = 1
} btn_state;

typedef enum
{   
    OFF = 0,    
    LANE_CHANGE_LEFT,
    LANE_CHANGE_RIGHT,
    NORMAL_LEFT,
    NORMAL_RIGHT,
    HAZARD,
} light_mode;

typedef struct 
{
    btn_state pressed;
    u32 last_press;
    u32 last_release;
} btn_info;


//CONTEXT = SUFICIENTE INFORMATII DESPRE STAREA BTNS / MODURI CA FIECARE MOD SA POATA SA RULEZE
//        = MOD DE COMUNICARE A MODURILOR CU MODULUL handle_btn, putand pune un mod pe OFF prin off_cmd
//mecanismul off_cmd e useless, trebuia doar sa fac left_mode, right_mode, hazard_mode available din context
typedef struct 
{
    light_mode last_mode;
    btn_info left_btn, right_btn, hazard_btn;
    light_mode left_mode, right_mode, hazard_mode;
} context;

void 
z(u8 pin, bool_ value = HIGH)
{
    digitalWrite(pin, value);
}

void 
setup(void)
{
    pinMode(LEFT_BTN_PIN, INPUT);
    pinMode(RIGHT_BTN_PIN, INPUT);
    pinMode(HAZARD_BTN_PIN, INPUT);
    pinMode(RIGHT_LED, OUTPUT);
    pinMode(LEFT_LED, OUTPUT);
    pinMode(POTENTIOMETER, INPUT);

    //DEBUG
    for (u8 i = 6; i <= 13; i++)
    {
        pinMode(i, OUTPUT);
    }
    pinMode(A3, OUTPUT);
    pinMode(A2, OUTPUT);
}


void 
print(LiquidCrystal* lcd, light_mode mode,
     btn_state left, btn_state right, btn_state hazard)
{   
     //NU FOLOSESC CLEAR, CASTIG TIMP (dau override si paddez cu spatii)
    static light_mode last_mode = (light_mode)0x10; //STARE INVALIDA IMI INTRA PE last_mode != mode 
    if (last_mode != mode)
    {      
        last_mode = mode;
        static const char* mode_lut[] = 
        {   
            "OFF          ",     
            "LANE_CHANGE_L",     
            "LANE_CHANGE_R",  
            "NORMAL_LEFT  ",  
            "NORMAL_RIGHT ",        
            "HAZARD       ",      
        };
        lcd->setCursor(0, 0);
        lcd->println(mode_lut[mode]);
    }
    static u8 last_btn_state  = 0x10; //STARE INVALIDA IMI INTRA PE last_btn_state != btn_states
    u8 btn_state = (hazard << 2) | (right << 1) | left; 
    if (last_btn_state != btn_state)
    {
        last_btn_state = btn_state;
        ///HAZARD RIGHT LEFT
        static const char* btn_state_lut[] =
        {
            "              ",
            "          LEFT",
            "    RIGHT     ", 
            "    RIGHT LEFT",
            "HAZ           ",
            "HAZ       LEFT",
            "HAZ RIGHT     ",
            "HAZ RIGHT LEFT"
        };
        lcd->setCursor(0,1);
        // APPEND MAI EFICIENT PE AVERAGE DAR LUT E MAI BUN PE WORST CASE
        lcd->print(btn_state_lut[btn_state]);    
    }
}

bool_ 
btn_status(u8 btn, btn_test mode)
{   
    static u32 ts_prev[NR_BTNS][NR_BTN_TEST_MODES] = {0}; //[3][2]: 3BUTTONS, 2 MODES
    static bool_ state_prev[NR_BTNS][NR_BTN_TEST_MODES] = {0};
    bool_ result = 0;
    const u8 DEBOUNCE_DELAY = 20; //CHANGE TO OLD DEBOUNCE IMMEDIATELY   
    if (millis() - ts_prev[btn][mode] >= DEBOUNCE_DELAY)
    {
        bool_ state_now = digitalRead(btn);
        if (mode == TEST_PRESS)
        {
            result = !state_prev[btn][mode] && state_now;
        }
        else //mode == TEST_RELEASE
        {
            result = state_prev[btn][mode] &&!state_now;
        }
        state_prev[btn][mode] = state_now;
        ts_prev[btn][mode] = millis();
    }
    return result;
}

//OFF COMMAND E PROST GANDITA (nu face nimic special) TREBUIA SA FAC LEFT-MODE
//RIGHT-MODE HAZARD-MODE AVAILABLE DIN CONTEXT SI ECHIVALENTUL UNUI OFF COMMAND
//E CA SCRIA DIRECT OFF IN VARIABILA DE MOD RESPECTIVA SI NU MA MAI COMPLICAM

void 
handle_btn_LR(IN_PARAM u8 L_R, IN_OUT_PARAM context* context_)
{   
    // OVERHEAD DAR NU DUPLIC CODUL
    light_mode* mode = &((L_R == LEFT_BTN) ? context_->left_mode : context_->right_mode);
    const u8 btn_pin[2] = {LEFT_BTN_PIN, RIGHT_BTN_PIN}; //LEFT pe PD0, RIGHT pe PD1
    const u8 pin = btn_pin[L_R];
    btn_info* btn = &((L_R == LEFT_BTN) ? context_->left_btn : context_->right_btn);
    btn_state* pressed = &(btn->pressed);
    u32* last_release = &(btn->last_release);
    u32* last_press = &(btn->last_press);
    light_mode LANE_CHANGE = L_R ? LANE_CHANGE_RIGHT: LANE_CHANGE_LEFT;
    light_mode NORMAL = L_R ? NORMAL_RIGHT: NORMAL_LEFT;
    //END OVERHEAD
    //OVERHEAD-UL e si ca memorie pe stiva, nu doar ca timp

    switch(*pressed)
    {
        case NOT_PRESSED:
        {   
            if (btn_status(pin, TEST_PRESS))
            {
                *pressed = PRESSED;
                *last_press = millis();
            }
        }
        break;  
        case PRESSED:
        {
            if (millis() - *last_press >= 500) 
            {   
                *mode = NORMAL;
            }
            if (btn_status(pin, TEST_RELEASE))
            {  
                *pressed = NOT_PRESSED;
                if (millis() - *last_press < 500)
                {   
                   *mode = LANE_CHANGE;
                }
                *last_release = millis();
            }
        }
        break;
    } 

    //avem noile valori pentru mode si btn_pressed
}

void 
handle_btn_left(context* context_)
{
    handle_btn_LR(LEFT_BTN, context_);
}

void 
handle_btn_right(IN_OUT_PARAM context* context_)
{
    handle_btn_LR(RIGHT_BTN, context_);
}

/*TODO: sa updateze din context nu statice last_btn...*/

void 
handle_btn_hazard(IN_OUT_PARAM context* context_)
{      
    btn_info* btn = &(context_->hazard_btn);
    btn_state* hazard_pressed = &(btn->pressed);
    u32* last_release = &(context_->hazard_btn.last_release);
    u32* last_pressed = &(context_->hazard_btn.last_press);
    light_mode*  mode = &(context_->hazard_mode);

    switch (*hazard_pressed) 
    {   
        case NOT_PRESSED:
        {
            if (btn_status(HAZARD_BTN, TEST_PRESS))
            {   
                *hazard_pressed = PRESSED;
                *last_pressed = millis();
            }
        }
        break;

        case PRESSED:
        {
            if (btn_status(HAZARD_BTN, TEST_RELEASE))
            {      
                *hazard_pressed = NOT_PRESSED;
                *last_release = millis();
                if (*last_release - *last_pressed >= 200)
                {      
                    *mode = (*mode == HAZARD) ? OFF : HAZARD;
                }
            }
        }
        break;
    }
}

void
turn_off_mode(light_mode mode, OUT_PARAM context* context_)
{
    switch(mode)
    {
        case LANE_CHANGE_LEFT: 
        case NORMAL_LEFT:
        {
            context_->left_mode = OFF;
        }
        break; // nu aveam break, alt bug la care am stat 2 ore
        case LANE_CHANGE_RIGHT:
        case NORMAL_RIGHT:
        {
            context_->right_mode = OFF;
        }
        break;
        case HAZARD:
        {
            context_->hazard_mode = OFF;
        }
        break;
        default:
        break;
    }
}

light_mode
prioritize(context* context_)
{
    //static light_mode last_left = OFF, last_right = OFF, last_hazard = OFF;
    //static light_mode last_mode = OFF;
    light_mode highest_priority_mode =  
    max3(context_->left_mode, 
        context_->right_mode, 
        context_->hazard_mode);
    
    light_mode mode = highest_priority_mode;
    if (context_->left_mode == LANE_CHANGE_LEFT && 
        context_->right_mode == LANE_CHANGE_RIGHT &&
         context_->hazard_mode != HAZARD)
    {      
        //cazul special ca un LANE_CHANGE il intrerupe pe celalalt
        mode = (context_->last_mode == LANE_CHANGE_LEFT) ? LANE_CHANGE_RIGHT : LANE_CHANGE_LEFT; //schimbam modul 
    }
    return mode;
} 

void    
lane_change(u8 LR, context* context_)
{
    static u8 cycles_[2] = {0}, count_[2] = {0};
    static u32 ts_prev_[2] = {0};
    
    //btn_info* btn = &((LR == LEFT_BTN) ? context_->left_btn : context_->right_btn);
    light_mode* last_LR_mode = &((LR == LEFT_BTN) ? 
    context_->left_mode : context_->right_mode);
    u8* cycles = &cycles_[LR]; 
    u8* count = &count_[LR];
    u32* ts_prev = &ts_prev_[LR];
    light_mode LANE_CHANGE = (LR == LEFT_BTN) ? LANE_CHANGE_LEFT: LANE_CHANGE_RIGHT;
    const u8 LED = (LR == LEFT_BTN) ? LEFT_LED : RIGHT_LED;
    const u8 THE_OTHER_LED = (LR == LEFT_BTN) ? RIGHT_LED : LEFT_LED;

    if (context_->last_mode != LANE_CHANGE)
    {   
        //in cazul in care alt mod a lasat celalalt led pe ON
        //mai important era ca fiecare mod sa curete dupa el la final, nu sa curete la inceput
        digitalWrite(THE_OTHER_LED, LOW);
        *cycles = 0;
        *count = 0;
    }

    if (millis() - *ts_prev >= 100)
    {       
        if (*count == 0)
        {   
            digitalWrite(LED, HIGH);
        }
        else if (*count == 3)
        {
            digitalWrite(LED, LOW);
        }
        if ((*count)++ == 10 - 1) //AICI ERA GRESEALA PENTRU FRECVENTA 0.9HZ
        {
            *count = 0;
            (*cycles)++;
        }
        *ts_prev = millis();
    }
    if (*cycles == 3) 
    {
        *cycles = 0;
        *count = 0;
        *last_LR_mode = OFF;
        //aici trebuia sa curat
        //digitalWrite(LED, LOW);s
    }
}

void 
lane_change_left(context* context_)
{
    lane_change(LEFT_BTN, context_);
}

void
lane_change_right(context* context_)
{
    lane_change(RIGHT_BTN, context_);
}

void 
normal(u8 LR, context* context_)
{
    static u32 ts_prev_[2] = {0};

    btn_info btn = (LR == LEFT_BTN) ? context_->left_btn : context_->right_btn;
    light_mode* last_LR_mode = &((LR == LEFT_BTN) ? 
    context_->left_mode : context_->right_mode);
    u32* ts_prev = &ts_prev_[LR];
    const u8 LED = (LR == LEFT_BTN) ? LEFT_LED : RIGHT_LED;
    const u8 THE_OTHER_LED = (LR == LEFT_BTN) ? RIGHT_LED : LEFT_LED;

    digitalWrite(THE_OTHER_LED, LOW); //TODO: snu e responsabilitatea lui

    u16 adc_result = analogRead(POTENTIOMETER);
    //u16 frequency = 1UL + (10UL - 1UL) * (adc_result * 1UL) / (MAX_ANALOG * 1UL);
    //u16 toggle_period = 1000 / (2 * frequency);
    u32 toggle_period = (500UL * MAX_ANALOG) / (MAX_ANALOG + (10UL - 1UL) * (adc_result * 1UL));
    if (millis() - *ts_prev >= toggle_period)
    {
        digitalWrite(LED, !digitalRead(LED));
        *ts_prev = millis();
    }
    if(millis() - btn.last_release >= 100 && btn.pressed == NOT_PRESSED) 
    {
        *last_LR_mode = OFF;
    }
}

void 
normal_left(context* context_)
{
    normal(LEFT_BTN, context_);
}

void
normal_right(context* context_)
{
    normal(RIGHT_BTN, context_);
}

void 
hazard(context* context_)
{   
    // bool_* off_cmd = &(context_->hazard_off_cmd);
    // u32 last_pressed = context_->hazard_btn.last_press;
    // bool_ pressed = context_->hazard_btn.pressed;
    if (context_->last_mode != HAZARD)
    {
        digitalWrite(LEFT_LED, LOW);
        digitalWrite(RIGHT_LED, LOW);
    }
    static u32 ts_prev = 0;
    if (millis() - ts_prev >= 500)
    {
        digitalWrite(LEFT_LED, !digitalRead(LEFT_LED));
        digitalWrite(RIGHT_LED, !digitalRead(RIGHT_LED));
        ts_prev = millis();
    }
    // PROBLEMA DE DESIGN: LA RESTUL MODURILOR DAU COMANDA DE OFF DIN ELE, LA HAZARD
    //DAU DIN HAZARD_BTN(), DAR NU MERGE ALTFEL
}

void 
off(context* context_)
{   
    if (context_->last_mode != OFF)
    {   
        digitalWrite(LEFT_LED, LOW);
        digitalWrite(RIGHT_LED, LOW);
    }
}

#define NR_MAX_TASKS 100
typedef struct 
{   
    void(*tasks[NR_MAX_TASKS])(void);
    u16 period[NR_MAX_TASKS];
    u32 last_exec_times[NR_MAX_TASKS] = {0};
    u8 crt_task = 0;
} scheduler;
#undef NR_MAX_TASKS

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
        if (millis() - s->last_exec_times[task] >= s->period[task])
        {   
            s->last_exec_times[task] = millis(); 
            (s->tasks[task])();
        }
    }
}

void 
main_task(void);

void 
loop(void)
{   
    static scheduler s;
    static bool_ sched_init = FALSE;
    if (!sched_init)
    {   
        sched_init = TRUE;
        const u8 HOW_OFTEN_RUN_MAIN_LOOP = 10;
        add_task(&s, main_task, HOW_OFTEN_RUN_MAIN_LOOP);
    }
    check_events(&s);
}

void 
lcd_init(LiquidCrystal* lcd)
{
    lcd->begin(16, 2);
    delay(10);
    lcd->clear();
    delay(10);
}

void
context_init(context* context_)
{   
    //TOATE ASTEA NU FAC NIMIC FIINDCA STATIC E INIT AUTOMAT CU 0
    context_->left_mode = OFF;
    context_->right_mode = OFF;
    context_->hazard_mode = OFF;
    context_->last_mode = OFF;
    context_->hazard_btn.pressed = NOT_PRESSED;
    context_->hazard_btn.last_press = 0;
    context_->hazard_btn.last_release = 0;
    context_->left_btn.pressed = NOT_PRESSED;
    context_->left_btn.last_press = 0;
    context_->left_btn.last_release = 0;
    context_->right_btn.pressed = NOT_PRESSED;
    context_->right_btn.last_press = 0;
    context_->right_btn.last_release = 0;
}

void 
wait_for_unlock(void)
{     
    const u8 pin[] = {LEFT_BTN_PIN, RIGHT_BTN_PIN, HAZARD_BTN_PIN};
    bool_ pressed[] = {FALSE, FALSE, FALSE};
    bool_ released[] = {FALSE, FALSE, FALSE};
    bool_ locked = TRUE;
    
    while(locked)
    {
        for (u8 btn = 0; btn < sizeof(pin); btn++)
        {
            if (btn_status(pin[btn], TEST_PRESS))
            {
                pressed[btn] = TRUE;
            }
            if (btn_status(pin[btn], TEST_RELEASE))
            {
                released[btn] = TRUE;
            }
            if(pressed[btn] && released[btn])
            {
                locked = FALSE;
                break;
            }
        }
    }
}

void 
main_task(void)
{   
    static LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
    static context context_;
    static bool_ init= FALSE;
    if (!init) 
    {   
        init = TRUE;
        lcd_init(&lcd);
        context_init(&context_);
        lcd.println("LOCKED");
        wait_for_unlock();
    }
    handle_btn_left(&context_);
    handle_btn_right(&context_);
    handle_btn_hazard(&context_);

    //DsEBUG
    // if(left_mode == LANE_CHANGE_LEFT && right_mode == LANE_CHANGE_RIGHT )
    // {   
    //     if (context_.last_mode == LANE_CHANGE_LEFT)
    //     {
    //         z(8);
    //     }
    //     z(11);
    // }
    //END DEBUG
    
    light_mode  mode = prioritize(&context_);
    //DEBUG
    //MODE
    z(A2, mode & 1);
    z(A3, (mode >> 1) & 1);
    z(9, (mode >> 2) & 1);
    //END DEBUG
    switch(mode)
    {
        case LANE_CHANGE_LEFT: {lane_change_left(&context_);} break;
        case LANE_CHANGE_RIGHT: {lane_change_right(&context_);} break;
        case NORMAL_LEFT: {normal_left(&context_);} break;
        case NORMAL_RIGHT: {normal_right(&context_);} break;
        case HAZARD: {hazard(&context_);} break;
        case OFF: {off(&context_);} break;
        default: break;
    }
    print(&lcd, mode, context_.left_btn.pressed, 
        context_.right_btn.pressed, context_.hazard_btn.pressed);
    //DEBUG
    if (mode == OFF && context_.last_mode != OFF)
    {
        z(10);
    }
    if (mode != LANE_CHANGE_LEFT && mode != LANE_CHANGE_RIGHT && mode != OFF)
    {
        z(8);
    }
    if (mode != context_.last_mode)
    { 
        turn_off_mode(context_.last_mode, &context_);
    }
    //END DEBUG
     context_.last_mode = mode;
    // if (mode != left_mode)
    // {   
    //     turn_off_mode(left_mode, &context_);
    // }
    // if (mode != right_mode)
    // {
    //     turn_off_mode(right_mode, &context_);
    // }
    // if (mode !=  hazard_mode)
    // {
    //     turn_off_mode(hazard_mode, &context_);
    // }