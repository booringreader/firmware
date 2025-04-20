#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "../inc/core/system.h"
#include "../inc/core/timer.h"

#define LED_PORT            GPIOA
#define LED_PIN             GPIO5

static void gpio_config(void){
    rcc_periph_clock_enable(RCC_GPIOA); //! turn ON the clock for GPIO port A
    gpio_mode_setup(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN); //? 4th argument can be used to define multiple ports using a bitwise OR (|), 3rd is to configure the state when neither pull-up or pull-down is enabled (intermediate state)
    gpio_set_af(LED_PORT, GPIO_AF1, LED_PIN); //? 2nd argument is the alternate function (detailed in af table in datasheet)
}

int main(void){
    system_setup();
    gpio_config();
    timer_setup();

    uint64_t start_time = system_get_ticks();
    float duty_cycle = 0.0f;
    
    timer_pwm_set_duty_cycle(duty_cycle);

    while(1){
        if(system_get_ticks()-start_time >= 10){ // 10 ms
            duty_cycle += 1.0f;
            if(duty_cycle > 100.0f) duty_cycle = 0.0f;

            timer_pwm_set_duty_cycle(duty_cycle);
            start_time = system_get_ticks();
        }
        // not the time to blink LED, continue with other work
    }
    //! NO RETURN    
    return 0;
}