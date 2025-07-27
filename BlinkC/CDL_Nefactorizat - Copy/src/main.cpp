#include <Arduino.h>
#include <LiquidCrystal.h>

#define OUT_PARAM
#define IN_PARAM
#define IN_OUT_PARAM
#define TRUE 1
#define FALSE 0 

#define max2(a,b) (((a) > (b)) ? (a) : (b))
#define max3(a,b,c) max2(max2((a), (b)), (c))

#define getbit(num, pos) (((num) >> (pos)) & 1)
#define clrbit(num, pos) (num) &= ~(1 << (pos))
#define setbit1(num, pos) (num) |= (1 << (pos))
#define setbit(num, pos, new_bit) (new_bit) ? setbit1(num, pos) : clrbit(num, pos)

#define DOOR_LOCK_PLUS A3
#define DOOR_LOCK_MINUS 7

#define WINDOW_PLUS A5
#define WINDOW_MINUS A4


#define RS 8
#define EN 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13   

#define LED0 A0
#define LED1 A1

#define NR_BTNS 1
#define NR_BTN_TEST_MODES 2
#define DOOR_BTN A2


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
    UNLOCKED =  0,
    LOCKED,
    DOUBLE_LOCKED
} door_state;

void 
z(u8 pin, bool_ value = HIGH)
{
    digitalWrite(pin, value);
}

void 
setup() 
{   
    Serial.begin(9600);
    pinMode(RS, OUTPUT);
    pinMode(EN, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A0, OUTPUT);
    pinMode(DOOR_LOCK_PLUS, OUTPUT);
    pinMode(DOOR_LOCK_MINUS, OUTPUT);
    pinMode(WINDOW_PLUS, OUTPUT);
    pinMode(WINDOW_MINUS, OUTPUT);
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

bool_ 
test_press(u8 btn)
{
    return btn_status(btn, TEST_PRESS);
}

bool_ 
test_release(u8 btn)
{
    return btn_status(btn, TEST_RELEASE);
}

void 
print(LiquidCrystal* lcd, door_state mode,
     btn_state door_btn)
{   
     //NU FOLOSESC CLEAR, CASTIG TIMP (dau override si paddez cu spatii)
    static door_state last_mode = (door_state)0x10; //STARE INVALIDA IMI INTRA PE last_mode != mode 
    if (last_mode != mode)
    {      
        last_mode = mode;
        static const char* mode_lut[] = 
        {   
            "UNLOCKED     ",     
            "LOCKED       ",     
            "DOUBLE_LOCKED",        
        };
        lcd->setCursor(0, 0);
        lcd->println(mode_lut[mode]);
    }
    static u8 last_btn_state  = 0x10; //STARE INVALIDA IMI INTRA PE last_btn_state != btn_states
    u8 btn_state = door_btn; 
    if (last_btn_state != btn_state)
    {
        last_btn_state = btn_state;
        ///HAZARD RIGHT LEFT
        static const char* btn_state_lut[] =
        {
            "        ",
            "DOOR_BTN"
        };
        lcd->setCursor(0,1);
        // APPEND MAI EFICIENT PE AVERAGE DAR LUT E MAI BUN PE WORST CASE
        lcd->print(btn_state_lut[btn_state]);    
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
        const u8 HOW_OFTEN_RUN_MAIN_LOOP = 20;
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

void blink_led(door_state mode)
{   
    static door_state last_mode = UNLOCKED;
    static u8 count = 0, cycles = 0, nr_cycles = 0;
    static u8 nr_blinks[] = {2, 1, 3};
    if (last_mode != mode)
    {   
        cycles = 0;
        count = 0;
        nr_cycles = nr_blinks[mode];
        last_mode = mode;
    }
    //digitalWrite(10, HIGH);
    static u32 ts_prev = 0;
    if (cycles < nr_cycles)
    {   
        if (millis() - ts_prev >= 100)
        {   
            if (count == 0)
            {
                digitalWrite(LED0, HIGH);
                digitalWrite(LED1, HIGH);
            }
            if (count  ==  3)
            {
                digitalWrite(LED0, LOW);
                digitalWrite(LED1, LOW);
            }
            
            if (count++ == 10 - 1)
            {
                count = 0;
                cycles++;
            }
            ts_prev = millis();
        }
    }
}

void
door_cw_1s(door_state last_mode, door_state mode)
{      
    static u32 ts_begin = 0;
    if (last_mode != mode)
    {   
        ts_begin = millis();
        digitalWrite(DOOR_LOCK_PLUS, HIGH);
        digitalWrite(DOOR_LOCK_MINUS, LOW);   
    }
    if (millis() - ts_begin > 1000)
    {
        digitalWrite(DOOR_LOCK_PLUS, LOW);
        digitalWrite(DOOR_LOCK_MINUS, LOW);   
    }
}

void
window_ccw_3s(door_state last_mode)
{
    static u32 ts_begin = 0;
    if (last_mode != DOUBLE_LOCKED)
    {   
        ts_begin = millis();
        digitalWrite(WINDOW_PLUS, LOW);
        digitalWrite(WINDOW_MINUS, HIGH);   
    }
    if (millis() - ts_begin > 3000)
    {
        digitalWrite(WINDOW_PLUS, LOW);
        digitalWrite(WINDOW_MINUS, LOW);   
    }
}

void
door_in_unlocked(door_state last_mode)
{   
    static u32 ts_begin = 0;
    static u16 time = 1000;
    if (last_mode != UNLOCKED)
    {   
        ts_begin = millis();
        digitalWrite(DOOR_LOCK_PLUS, LOW);
        digitalWrite(DOOR_LOCK_MINUS, HIGH);   
        time = (last_mode == DOUBLE_LOCKED) ? 2000 : 1000;
    }
    if (millis() - ts_begin > time)
    {
        digitalWrite(DOOR_LOCK_PLUS, LOW);
        digitalWrite(DOOR_LOCK_MINUS, LOW);   
    }
}

void 
window_in_unlocked(door_state last_mode, btn_state door_btn)
{
    static u32 ts_begin = 0;
    // static bool_ pressed = FALSE;
    // static bool_ ok = FALSE;
    static btn_state last_door_btn = NOT_PRESSED;
    if (last_mode != UNLOCKED)
    {
        last_door_btn = NOT_PRESSED;
    }
    if (door_btn == PRESSED && last_door_btn != PRESSED)
    {   
        ts_begin = millis();
        digitalWrite(WINDOW_PLUS, HIGH);
        digitalWrite(WINDOW_MINUS, LOW);   
    }
    if ((door_btn == NOT_PRESSED && last_door_btn == PRESSED))
    {
        // Serial.println("Imi intra aici");
    }
    if (millis() - ts_begin > 3000 || (door_btn == NOT_PRESSED && last_door_btn == PRESSED))
    {   
        // Serial.println("Sunt aici");
        digitalWrite(WINDOW_PLUS, LOW);
        digitalWrite(WINDOW_MINUS, LOW);   
    }
    last_door_btn = door_btn;
}

void 
cleanup()
{
    digitalWrite(WINDOW_PLUS, LOW);
    digitalWrite(WINDOW_MINUS, LOW);
    digitalWrite(DOOR_LOCK_MINUS, LOW);
    digitalWrite(DOOR_LOCK_PLUS, LOW);   
}

#define NOKEY 0x7F
//!FUNCTIE INSPIRATA DE LA DL LEMENI!
char 
kbscan(void)
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
   for(DDRD = (1 << 3) | (1 << 7); DDRD != (1 << 7); DDRD = (DDRD << 1) | (1 << 7) , lina++) 
   {
      _delay_us(100); // DACA SCHIMBI DDR PREA REPEDE NU MAI MERGE
                      //CRED, FUNCTIA E INSPIRATA DE DL LEMENI :)
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
debounced_kbscan(void)
{
    static u32 ts_prev = 0;
    static char ch_prev = NOKEY;
    static const u16 DEBOUNCE_DELAY = 30;
    char result = NOKEY;
    if (millis() - ts_prev >= DEBOUNCE_DELAY)
    {      
        char ch_now = kbscan();
        if (ch_prev == NOKEY && ch_now != NOKEY)
        {
            result = ch_now;
        }
        ts_prev = millis();
        ch_prev = ch_now;
    }
    return result;
}

void 
unlock(LiquidCrystal* lcd)
{   
    static const char cod[] = "1A6";
    char buf[4];
    buf[3] = '\0';
    u8 i = 0;
    lcd->setCursor(0, 0);
    lcd->print("Cod: ");
    for(;;)
    {   
        while((buf[i] = debounced_kbscan()) == NOKEY);
        lcd->print(buf[i]);
        i++;
        if (i == 3)
        {   
            i = 0;
            if (strcmp(buf, cod) == 0)
            {   
                lcd->setCursor(0, 0);
                lcd->print("ACCES PERMIS");
                delay(1000);
                lcd->clear();
                return;
            }
            else
            {   
                lcd->clear();
                lcd->print("COD INVALID");
                delay(1000);
                lcd->clear();
                lcd->print("Cod: ");
            }
        }
    }

}

void 
main_task(void)
{    

    static door_state state = UNLOCKED;
    static door_state last_state = UNLOCKED;
    static btn_state door_btn = NOT_PRESSED;
    static LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
    static bool_ init = FALSE;
    if (!init)
    {   
        init = TRUE;
        lcd_init(&lcd);
        Serial.println("O singura data.");
        //unlock(&lcd);
    }
    print(&lcd, state, door_btn);
    static bool_ ok = FALSE;
    if (last_state != state)
    {
        ok = TRUE;
        cleanup();
    }
    if (state != UNLOCKED && state != last_state)
    {
        // Serial.println(state);
    }
    switch(state)
    {   
        case UNLOCKED:
        {          
            blink_led(UNLOCKED);
            static u32 ts_prev = 0;
            door_in_unlocked(last_state);
            window_in_unlocked(last_state, door_btn);     
            if (last_state != state)
            {
                ts_prev = millis();
            }
            switch(door_btn)
            {
                case NOT_PRESSED:
                {
                    if (test_press(DOOR_BTN))
                    {   
                        door_btn = PRESSED;
                        ts_prev = millis();
                    }
                }
                break;

                case PRESSED:
                {
                    if (test_release(DOOR_BTN))
                    {   
                        door_btn = NOT_PRESSED;
                        if (millis() - ts_prev >= 800)
                        {   
                            last_state = state;
                            state = LOCKED;
                        }
                    }
                }
                break;
            }
        }
        break;
        case LOCKED:
        {  
            static u32 last_press = 0;
            blink_led(LOCKED);
            door_cw_1s(last_state, LOCKED);
            if (last_state != state)
            {
                last_press = millis();
            }
            switch(door_btn)
            {
                case NOT_PRESSED:
                {
                    if (test_press(DOOR_BTN))
                    {   
                        door_btn = PRESSED;
                        last_press = millis();
                    }
                }
                break;

                case PRESSED:
                {   
                    u32 time_pressed = millis() - last_press;
                    if (test_release(DOOR_BTN))
                    {   
                        door_btn = NOT_PRESSED;
                        if (time_pressed >= 100 && time_pressed <= 800)
                        {   
                            last_state = state;
                            state = DOUBLE_LOCKED;
                        }
                    }
                    if (time_pressed >= 800)
                    {   
                        last_state = state;
                        state = UNLOCKED;
                    }
                }
                break;
            }
        }
        break;

        case DOUBLE_LOCKED:
        {  
            static u32 last_press = 0;
            if (last_state != state)
            {
                last_press = millis();
            }
            blink_led(DOUBLE_LOCKED);
            door_cw_1s(last_state, DOUBLE_LOCKED);
            window_ccw_3s(last_state);
            switch(door_btn)
            {   
                case NOT_PRESSED:
                {
                    if (test_press(DOOR_BTN))
                    {   
                        door_btn = PRESSED;
                        last_press = millis();
                    }
                }
                break;

                case PRESSED:
                {   
                    u32 time_pressed = millis() - last_press;
                    if (time_pressed >= 800)
                    {
                        last_state = state;
                        state = UNLOCKED;
                    }
                    if (test_release(DOOR_BTN))
                    {
                        door_btn = NOT_PRESSED;
                    }   
                }
                break;
            }
        }
    }
    if (ok)
    {   
        ok  = FALSE;
        last_state = state;
    }

}
