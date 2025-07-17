// DEBUG PORTS:
// 11 - imi intra pe un mod neimplementat in switch
// 9 - right button e pe NOT_PRESSED
// 8 - right button e pe PRESSED
// 7 - au trecut 500ms de la apasarea unui buton
// 10 - am intrat pe lange_change_right
// 6 - imi intra pe normal left
#include <Arduino.h>
#include <LiquidCrystal.h>


#define MAX_ANALOG 1023
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
}

typedef enum
{
    TEST_PRESS = 0,
    TEST_RELEASE
} btn_test;

typedef enum
{
    PRESSED = 0,
    NOT_PRESSED,
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

unsigned char 
detect_press(unsigned char btn)
{   
    static unsigned long ts_prev[3] = {0};
    static unsigned char state_prev[3] = {0};
    const unsigned char DEBOUNCE_DELAY = 20; //CHANGE IMMEDIATELY   
    unsigned char result_press = 0;
    if (millis() - ts_prev[btn] >= DEBOUNCE_DELAY)
    {
        unsigned char state_now = PIND;
        result_press = ~state_prev[btn] & state_now & 0b111;
        state_prev[btn] = state_now;
        ts_prev[btn] = millis();
    }
    return (result_press >> btn) & 1;
}


unsigned char 
detect_release(unsigned char btn)
{   
    static unsigned long ts_prev[3] = {0};
    static unsigned char state_prev[3] = {0};
    const unsigned char DEBOUNCE_DELAY = 20; //CHANGE IMMEDIATELY   
    unsigned char result_release = 0;
    if (millis() - ts_prev[btn] >= DEBOUNCE_DELAY)
    {
        unsigned char state_now = PIND;
        result_release = state_prev[btn] & ~state_now & 0b111;
        state_prev[btn] = state_now;
        ts_prev[btn] = millis();
    }
    return (result_release >> btn) & 1;
}

// unsigned char  
// buttons_status(btn_test mode)
// {  
//     static unsigned long ts_prev = 0;
//     static unsigned char state_prev = 0;
//     const unsigned char DEBOUNCE_DELAY = 0; //CHANGE IMMEDIATELY   
//     unsigned char result_press = 0;
//     unsigned char result_release = 0;
//     if (millis() - ts_prev >= DEBOUNCE_DELAY)
//     {
//         unsigned char state_now = PIND;
//         result_press = ~state_prev & state_now & 0b111;
//         result_release = state_prev & ~state_now & 0b111;
//         state_prev = state_now;
//         ts_prev = millis();
//     }
//     return (mode == TEST_RELEASE) ?  result_release : result_press;
// }

unsigned char
btn_status(unsigned char btn, btn_test mode) 
{      
    return (mode == TEST_PRESS) ? detect_press(btn) : detect_release(btn);
}

void 
print(LiquidCrystal* lcd, light_mode mode,
     btn_state left, btn_state right, btn_state hazard)
{   

    //static char last_lcd_state[32] = {0};
    const char* mode_lut[] = 
    {
        "NORMAL_LEFT",
        "LANE_CHANGE_L",     
        "NORMAL_RIGHT",
        "LANE_CHANGE_R",
        "HAZARD",
        "OFF"
    };
    // const char all_spaces[] = 
    // {
    //     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    //     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    //     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    //     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    // };
    lcd->setCursor(0,0);
    // lcd->print(all_spaces);
    delay(10);
    lcd->clear();
    delay(10);
    lcd->setCursor(0,0);
    lcd->print(mode_lut[mode]);
    lcd->setCursor(0, 1);
    if (left == PRESSED)
    {
        lcd->print("LEFT ");
    }
    if (right == PRESSED)
    {
        lcd->print("RIGHT ");
    }
    if (hazard == PRESSED)
    {
        lcd->print("HAZ ");
    }
}


void 
main_task(void);

void 
loop(void)
{   
    static unsigned long ts_prev_main_task = 0;
    const unsigned char HOW_OFTEN_RUN_MAIN_LOOP = 10;
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
    static unsigned char init_lcd = 0;
    if (!init_lcd) 
    {   
        init_lcd = 1;
        lcd.begin(16, 2);
    }
    static unsigned long last_press[3] = {0};
    static unsigned long last_release[3] = {0};
    static btn_state left_btn = NOT_PRESSED;
    static btn_state right_btn = NOT_PRESSED;
    static btn_state hazard_btn = NOT_PRESSED;
    static light_mode mode = OFF;
    static light_mode last_mode = OFF;
    static unsigned char cycles_left = 0;
    static unsigned char cycles_right = 0;
    static unsigned char count_left = 0;
    static unsigned char count_right = 0;
    //unsigned char can_verride = TRUE;

    print(&lcd, mode, left_btn, right_btn, hazard_btn);

    if (mode == OFF) 
    {
        digitalWrite(11, HIGH);
    }
    else
    {
        digitalWrite(11, LOW);
    }

    // digitalWrite(7, left_btn == PRESSED);
    // digitalWrite(8, right_btn == PRESSED);
    // digitalWrite(9, hazard_btn == PRESSED);
    

    switch (left_btn)
    {
        case NOT_PRESSED:
        {   
            // digitalWrite(8, HIGH);
            // digitalWrite(9, LOW);
            if (btn_status(LEFT_BTN, TEST_PRESS) && last_mode != HAZARD)
            {
                left_btn = PRESSED;
                last_press[LEFT_LED]= millis();
            }
        }
        break;  
        case PRESSED:
        {
            // digitalWrite(8, LOW);
            // digitalWrite(9, HIGH);
            if (millis() - last_press[LEFT_LED] >= 500 && last_mode != HAZARD) 
            {   
                //digitalWrite(7, HIGH);
                mode = NORMAL_LEFT;
            }
             //digitalWrite(10, LOW);
            if (btn_status(LEFT_BTN, TEST_RELEASE))
            {  
                //digitalWrite(6, HIGH);
                left_btn = NOT_PRESSED;
                if (millis() - last_press[LEFT_LED] < 500)
                {
                    if (last_mode != NORMAL_LEFT && last_mode != NORMAL_RIGHT && last_mode != HAZARD)
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
                last_press[RIGHT_LED]= millis();
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
        static unsigned long ts_prev = 0;
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
            digitalWrite(10, HIGH);
            static unsigned long ts_prev = 0;
            if (millis() - ts_prev >= 100)
            {
                if (cycles_left < 3)
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
            
            static unsigned long ts_prev = 0;
            unsigned int adc_result = analogRead(POTENTIOMETER);
            unsigned int frequency = 1UL + (10UL - 1UL) * (adc_result * 1UL) / (MAX_ANALOG * 1UL);
            unsigned int period = 1000 / 2 / frequency;
            if (millis() - ts_prev >= period)
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
            static unsigned long ts_prev = 0;
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
            static unsigned long ts_prev = 0;
            unsigned int adc_result = analogRead(POTENTIOMETER);
            unsigned int frequency = 1UL + (10UL - 1UL) * (adc_result * 1UL) / (MAX_ANALOG * 1UL);
            unsigned int period = 1000 / 2 / frequency;
            if (millis() - ts_prev >= period)
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
            static unsigned long ts_prev = 0;
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
