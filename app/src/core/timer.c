#include "core/timer.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h> // first thing to do is enable the clock for the peripheral

// frequency = system frequence / ((prescaler - 1) * (arr - 1)) ; to avoid dividing by 0
#define PRESCALER (84)
#define ARR_VALUE (1000)

void timer_setup(void){
    rcc_periph_clock_enable(RCC_TIM2);

    // timer config
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // set the timer mode to internal clock
    // pwm mode setup
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1); // timer output compare mode (compare compare register value to output)

    timer_enable_counter(TIM2);
    timer_enable_oc_output(TIM2, TIM_OC1); // enable the output compare channel (post setup)

    // setup frequency and resolution
    timer_set_prescaler(TIM2, PRESCALER - 1);
    timer_set_period(TIM2, ARR_VALUE - 1);
}

void timer_pwm_set_duty_cycle(float duty_cycle){
    // duty cycle = (CCR / ARR_VALUE) * 100 % ; we need the CCR value
    const float raw_value = (float)ARR_VALUE * (duty_cycle / 100.0f);
    timer_set_oc_value(TIM2, TIM_OC1, (uint32_t)raw_value); 
}