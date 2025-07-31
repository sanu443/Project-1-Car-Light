#include <unity.h>
#include "utils/registers.h"
#include "devices/leds.h"
#include "fsm/door_states.h"

static blinking_leds leds;
static timer timer_;

#ifndef LOW
#define LOW 0
#define HIGH 1
#endif

void 
setUp(void) 
{   
    for (u8 pin = 0; pin < 28; pin++)
    {
        digitalWrite_(pin, LOW);
    }
    leds = init_leds(DOUBLE_LOCKED);
    reset_timer(&timer_);
}

void 
tearDown(void) 
{

}

void 
test_double_locked_leds_at_time(u32 time, bool_ expected_led0, bool_ expected_led1)
{
    for (;;)
    {
        blink(&leds);
        if (have_passed(timer_, time)) //daca pun fix 100 e loterie cand merge cand nu merge
        {
            TEST_ASSERT_TRUE((digitalRead_(LED0) == expected_led0) && 
            digitalRead_(LED1) == expected_led1);
            return;
        }
    }
}

void
test_double_locked_leds_interval(u32 time_start, u32 time_end, bool_ expected_led0, bool_ expected_led1)
{
    for (;;)
    {
        blink(&leds);
        if (have_passed(timer_, time_start) && !have_passed(timer_, time_end))
        {
            if (digitalRead_(LED0) != expected_led0 || digitalRead_(LED1) != expected_led1)
            {
                TEST_ASSERT_TRUE(FALSE);
                return;
            }
        }
        if (have_passed(timer_, time_end + 1))
        {
            TEST_ASSERT_TRUE(TRUE);
            return;
        }
    }
}

void 
test_leds_double_locked_after_99ms_leds_on(void) 
{   
    test_double_locked_leds_at_time(99, HIGH, HIGH);
}

void 
test_leds_double_locked_after_101ms_leds_on(void) 
{   
    test_double_locked_leds_at_time(101, HIGH, HIGH);
}

void
test_leds_double_locked_after_500ms_leds_off(void) 
{   
    test_double_locked_leds_at_time(500, LOW, LOW);
}

void
test_leds_double_locked_after_999ms_leds_off(void) 
{   
    test_double_locked_leds_at_time(999, LOW, LOW);
}

void 
test_leds_double_locked_from_1ms_to_299ms_leds_on(void)
{
    test_double_locked_leds_interval(1, 299, HIGH, HIGH);
}

void 
test_leds_double_locked_from_301ms_to_999ms_leds_off(void)
{
    test_double_locked_leds_interval(301, 999, LOW, LOW);
}

void
test_leds_double_locked_from_1001ms_to_1299ms_leds_on(void)
{
    test_double_locked_leds_interval(1001, 1299, HIGH, HIGH);
}

void 
test_leds_double_locked_after_1500ms_leds_off(void)
{
    test_double_locked_leds_at_time(1500, LOW, LOW);
}

void 
test_leds_double_locked_after_2099ms_leds_on(void)
{
    test_double_locked_leds_at_time(2099, HIGH, HIGH);
}

void 
test_leds_double_locked_after_3_cycles_done_leds_off(void)
{
    test_double_locked_leds_interval(3001, 4000, LOW, LOW);
}

int 
main() 
{   
    //fac testele la 299ms de exemplu in loc de 300ms fiindca exista
    //posibilitatea ca have_passed(300) sa se activeze intre 2 instructiuni
    //consecutive
    UNITY_BEGIN();
    RUN_TEST(test_leds_double_locked_after_99ms_leds_on);
    RUN_TEST(test_leds_double_locked_after_101ms_leds_on);
    RUN_TEST(test_leds_double_locked_after_500ms_leds_off);
    RUN_TEST(test_leds_double_locked_from_1ms_to_299ms_leds_on);
    RUN_TEST(test_leds_double_locked_from_301ms_to_999ms_leds_off);
    RUN_TEST(test_leds_double_locked_from_1001ms_to_1299ms_leds_on);
    RUN_TEST(test_leds_double_locked_after_1500ms_leds_off);
    RUN_TEST(test_leds_double_locked_after_2099ms_leds_on);
    RUN_TEST(test_leds_double_locked_after_3_cycles_done_leds_off);
    return UNITY_END();
}