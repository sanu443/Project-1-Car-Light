// DEBUG PORTS:
// 11 - imi intra pe un mod neimplementat in switch
// 9 - right button e pe NOT_PRESSED
// 8 - right button e pe PRESSED
// 7 - au trecut 500ms de la apasarea unui buton
// 10 - am intrat pe lange_change_right
// 6 - imi intra pe normal left
#include <Arduino.h>
#include <LiquidCrystal.h>


#define MAX_ANALOG 1023UL
#define NR_LEDS 8
#define TRUE 1
#define FALSE 0 

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

void setup(void)
{
    pinMode(LEFT_BTN_PIN, INPUT);
    pinMode(RIGHT_BTN_PIN, INPUT);
    pinMode(HAZARD_BTN_PIN, INPUT);
    pinMode(RIGHT_LED, OUTPUT);
    pinMode(LEFT_LED, OUTPUT);
    pinMode(POTENTIOMETER, INPUT);

    //DEBUG
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(A2, OUTPUT);
}

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
    NORMAL_LEFT = 0,
    LANE_CHANGE_LEFT,
    NORMAL_RIGHT,
    LANE_CHANGE_RIGHT,
    HAZARD,
    OFF,    
} light_mode;

u8 
detect_press(u8 btn)
{   
    static u32 ts_prev[3] = {0};
    static u8 state_prev[3] = {0};
    const u8 DEBOUNCE_DELAY = 20; //CHANGE TO OLD DEBOUNCE IMMEDIATELY   
    u8 result_press = 0;
    if (millis() - ts_prev[btn] >= DEBOUNCE_DELAY)
    {
        u8 state_now = PIND;
        result_press = ~state_prev[btn] & state_now & 0b111;
        state_prev[btn] = state_now;
        ts_prev[btn] = millis();
    }
    return (result_press >> btn) & 1;
}


u8  
detect_release(u8 btn)
{   
    static u32 ts_prev[3] = {0};
    static u8 state_prev[3] = {0};
    const u8 DEBOUNCE_DELAY = 20; //CHANGE TO OLD DEBOUNCE IMMEDIATELY   
    u8 result_release = 0;
    if (millis() - ts_prev[btn] >= DEBOUNCE_DELAY)
    {
        u8 state_now = PIND;
        result_release = state_prev[btn] & ~state_now & 0b111;
        state_prev[btn] = state_now;
        ts_prev[btn] = millis();
    }
    return (result_release >> btn) & 1;
}

// u8  
// buttons_status(btn_test mode)
// {  
//     static u32 ts_prev = 0;
//     static u8 state_prev = 0;
//     const u8 DEBOUNCE_DELAY = 0; //CHANGE IMMEDIATELY   
//     u8 result_press = 0;
//     u8 result_release = 0;
//     if (millis() - ts_prev >= DEBOUNCE_DELAY)
//     {
//         u8 state_now = PIND;
//         result_press = ~state_prev & state_now & 0b111;
//         result_release = state_prev & ~state_now & 0b111;
//         state_prev = state_now;
//         ts_prev = millis();
//     }
//     return (mode == TEST_RELEASE) ?  result_release : result_press;
// }

bool_
btn_status(u8 btn, btn_test mode) 
{      
    return (mode == TEST_PRESS) ? detect_press(btn) : detect_release(btn);
}

void 
my_strcpy (const char* src, char* dest)
{   
    strcpy(dest, src);
    return;
    while (*src != '\0')
    {
        *dest++ = *src++;
    }
    *dest = '\0';
}

u16
my_strlen (const char* str)
{   
    return strlen(str);
    const char* base = str;
    while (*str++ != '\0');
    return str - base;
}

void
my_strcat(const char* suffix, char* prefix)
{   
    strcat(prefix, suffix);
    return;
    my_strcpy(suffix, prefix + strlen(prefix));
}

i16
my_strcmp(const char* left, const char* right)
{   
    return strcmp(left, right);
    while (*left != '\0' && *right != 0)
    {
        if (*left > *right)
        {
            return 1;
        }
        if (*left < *right)
        {
            return -1;
        }
        left++;
        right++;
    }
    if (*left == '\0' && *right == '\0')
    {
        return 0;
    }
    if (*left == '\0')
    {
        return -1;
    }
    return 1;
}
   
void 
print(LiquidCrystal* lcd, light_mode mode,
     btn_state left, btn_state right, btn_state hazard)
{   

    //static char last_lcd_state[32] = {0};
    static char last_buffer[32] = {'\0'};
    const char* mode_lut[] = 
    {
        "NORMAL_LEFT",
        "LANE_CHANGE_L",     
        "NORMAL_RIGHT",
        "LANE_CHANGE_R",
        "HAZARD",
        "OFF"
    };
    char buffer[33];
    for (u8 i = 0; i < 32; i++)
    {
        buffer[i] = ' ';
    }
    buffer[32] = '\0';
    const char* mode_text = mode_lut[mode];
    my_strcpy(mode_text, buffer);
    buffer[my_strlen(mode_text)] = ' ';
    char btn_text[20] = {'\0'};         
    // lcd->print(all_spaces);
    if (left == PRESSED)
    {
        my_strcat("LEFT ", btn_text);
    }
    if (right == PRESSED)
    {
         my_strcat("RIGHT ", btn_text);
    }
    if (hazard == PRESSED)
    {
         my_strcat("HAZ ", btn_text);
    }
    my_strcpy(btn_text, buffer + 16);
    buffer[16 + my_strlen(btn_text)] = ' ';
    if (my_strcmp(buffer, last_buffer) != 0)
    {
        lcd->setCursor(0,0);
        lcd->print(buffer);
        my_strcpy(buffer, last_buffer);
    }
}


void 
main_task(void);

void 
loop(void)
{   
    static u32 ts_prev_main_task = 0;
    const u8 HOW_OFTEN_RUN_MAIN_LOOP = 10;
    if (millis() - ts_prev_main_task >= HOW_OFTEN_RUN_MAIN_LOOP)
    {
        main_task();
        ts_prev_main_task = millis();
    }
}

    void 
    main_task(void)
{   
    static LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
    static u8 init_lcd = 0;
    if (!init_lcd) 
    {   
        init_lcd = 1;
        lcd.begin(16, 2);
        delay(10);
    }
    static u32 last_press[3] = {0};
    static u32 last_release[3] = {0};
    static btn_state left_btn = NOT_PRESSED;
    static btn_state right_btn = NOT_PRESSED;
    static btn_state hazard_btn = NOT_PRESSED;
    static light_mode mode = OFF;
    static light_mode last_mode = OFF;
    static u8 cycles_left = 0;
    static u8 cycles_right = 0;
    static u8 count_left = 0;
    static u8 count_right = 0;
    //bool_ can_verride = TRUE;

    print(&lcd, mode, left_btn, right_btn, hazard_btn);


    //DEBUG
    //BUTTONS
    digitalWrite(6, left_btn == PRESSED);
    digitalWrite(7, right_btn == PRESSED);
    digitalWrite(8, hazard_btn == PRESSED);
    //MODE
    digitalWrite(A2, mode % 2);
    digitalWrite(A3, (mode / 2) % 2);
    digitalWrite(9, (mode / 4) % 2);
    //END DEBUG
    
    switch(left_btn)
    {
        case NOT_PRESSED:
        {   
            //digitalWrite(8, HIGH);
            //digitalWrite(9, LOW);
            if (btn_status(LEFT_BTN, TEST_PRESS))
            {       
                left_btn = PRESSED;
                if(last_mode != HAZARD)
                {
                    last_press[LEFT_LED]= millis();
                }
            }
            
        }
        break;  
        case PRESSED:
        {
            //digitalWrite(8, LOW);
            //digitalWrite(9, HIGH);
            if (millis() - last_press[LEFT_LED] >= 500) 
            {   
                //digitalWrite(7, HIGH);
                if (last_mode != NORMAL_RIGHT && last_mode != HAZARD)
                {
                    mode = NORMAL_LEFT;
                } 
            }
             //digitalWrite(10, LOW);
            if (btn_status(LEFT_BTN, TEST_RELEASE))
            {  
                //digitalWrite(6, HIGH);
                left_btn = NOT_PRESSED;
                if (millis() - last_press[LEFT_LED] < 500)
                {   
                    if (last_mode != NORMAL_LEFT && last_mode != NORMAL_RIGHT && last_mode !=HAZARD)
                    {
                        cycles_left = 0;
                        count_left = 0;
                        mode = LANE_CHANGE_LEFT;
                    }
                }
                last_release[LEFT_LED] = millis();
            }
        }
         break;
    }

   switch(right_btn)
    {
        case NOT_PRESSED:
        {   
            //digitalWrite(8, HIGH);
            //digitalWrite(9, LOW);
            if (btn_status(RIGHT_BTN, TEST_PRESS))
            {
                right_btn = PRESSED;
                if (last_mode != HAZARD)
                {
                    last_press[RIGHT_LED]= millis();
                }
            }
        }
        break;  
        case PRESSED:
        {
            //digitalWrite(8, LOW);
            //digitalWrite(9, HIGH);
            if (millis() - last_press[RIGHT_LED] >= 500) 
            {   
                //digitalWrite(7, HIGH);
                if (last_mode != NORMAL_LEFT && last_mode != HAZARD)
                {
                    mode = NORMAL_RIGHT;
                } 
            }
             //digitalWrite(10, LOW);
            if (btn_status(RIGHT_BTN, TEST_RELEASE))
            {  
                //digitalWrite(6, HIGH);
                right_btn = NOT_PRESSED;
                if (millis() - last_press[RIGHT_LED] < 500)
                {   
                    if (last_mode != NORMAL_LEFT && last_mode != NORMAL_RIGHT && last_mode !=HAZARD)
                    {
                        cycles_right = 0;
                        count_right = 0;
                        mode = LANE_CHANGE_RIGHT;
                    }
                }
                last_release[RIGHT_LED] = millis();
            }
        }
         break;
    }

    switch (hazard_btn) 
    {   
        static u32 ts_prev = 0;
        case NOT_PRESSED:
        {
            if (btn_status(HAZARD_BTN, TEST_PRESS))
            {   
                hazard_btn = PRESSED;
                ts_prev = millis();
            }
        }
        break;

        case PRESSED:
        {
            if (btn_status(HAZARD_BTN, TEST_RELEASE))
            {   
                hazard_btn = NOT_PRESSED;
                if (last_mode != HAZARD)
                {
                    if (millis() - ts_prev >= 200)
                    {
                        mode = HAZARD;
                    }
                }
                else
                {
                    if (millis() - ts_prev >= 200)
                    {
                        mode = OFF;
                        //can_verride = FALSE;
                    }
                }
            }
        }
        break;
    }

    switch (mode)
    {
        case LANE_CHANGE_LEFT:
        {   
            //digitalWrite(10, HIGH);
            static u32 ts_prev = 0;
            if (millis() - ts_prev >= 100)
            {
                if (count_left > 2)
                {
                    digitalWrite(LEFT_LED, LOW);
                }
                else
                {
                    digitalWrite(LEFT_LED, HIGH);
                }
                if (count_left++ == 10)
                {
                    count_left = 0;
                    cycles_left++;
                }
                ts_prev = millis();
            }
            if (cycles_left == 3 && last_mode != HAZARD) 
            {
                cycles_left = 0;
                mode = OFF;
            }
        }
        break;
        case NORMAL_LEFT:
        {          
            
            static u32 ts_prev = 0;
            u16 adc_result = analogRead(POTENTIOMETER);
            //deduction of the toggle period:
            // frequency = 1UL + (10UL - 1UL) * (adc_result * 1UL)) / (MAX_ANALOG * 1UL);
            // toggle_period = 1000 / (2 * frequency);
            u32 toggle_period = (500UL * MAX_ANALOG) / (MAX_ANALOG + (10UL - 1UL) * (adc_result * 1UL));
            if (millis() - ts_prev >= toggle_period)
            {
                digitalWrite(LEFT_LED, !digitalRead(LEFT_LED));
                ts_prev = millis();
            }
            if(millis() - last_release[LEFT_LED] >= 100 && left_btn == NOT_PRESSED && last_mode != HAZARD) 
            {
                mode = OFF;
                //can_verride = FALSE;
            }
        } 
        break;
        case LANE_CHANGE_RIGHT:
        {   
            digitalWrite(10, HIGH);
            static u32 ts_prev = 0;
            if (millis() - ts_prev >= 100)
            {
                if (cycles_right < 3)
                {
                    if (count_right > 2)
                    {
                        digitalWrite(RIGHT_LED, LOW);
                    }
                    else
                    {
                        digitalWrite(RIGHT_LED, HIGH);
                    }
                    if (count_right++ == 10)
                    {
                        count_right = 0;
                        cycles_right++;
                    }
                }
                ts_prev = millis();
            }
            if (cycles_right == 3 && last_mode != HAZARD) 
            {
                cycles_right = 0;
                count_right = 0;
                mode = OFF;
            }
        }
        break;
        case NORMAL_RIGHT:
        {      
            static u32 ts_prev = 0;
            u16 adc_result = analogRead(POTENTIOMETER);
            //u16 frequency = 1UL + (10UL - 1UL) * (adc_result * 1UL) / (MAX_ANALOG * 1UL);
            //u16 toggle_period = 1000 / (2 * frequency);
            u32 toggle_period = (500UL * MAX_ANALOG) / (MAX_ANALOG + (10UL - 1UL) * (adc_result * 1UL));
            if (millis() - ts_prev >= toggle_period)
            {
                digitalWrite(RIGHT_LED, !digitalRead(RIGHT_LED));
                ts_prev = millis();
            }
            if(millis() - last_release[RIGHT_LED] >= 100 && right_btn == NOT_PRESSED && last_mode != HAZARD) 
            {
                mode = OFF;
            }
        } 
        break;

        case HAZARD:
        {   
            if (last_mode != HAZARD)
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
        }
        break;

        case OFF:
        {   
            digitalWrite(LEFT_LED, LOW);
            digitalWrite(RIGHT_LED, LOW);
        }
        break;

        default:
            break;
    }
    if (mode != LANE_CHANGE_LEFT)
    {
        cycles_left = 0;
        count_left = 0;
    }
    if (mode != LANE_CHANGE_RIGHT)
    {
        cycles_right = 0;
        count_right = 0;
    }
    if (mode == LANE_CHANGE_RIGHT || mode == NORMAL_RIGHT)
    {
        digitalWrite(LEFT_LED, LOW);
    }
    if (mode == LANE_CHANGE_LEFT || mode == NORMAL_LEFT)
    {
        digitalWrite(RIGHT_LED, LOW);
    }
    last_mode = mode;
}