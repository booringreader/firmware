#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "../inc/core/system.h"

#define LED_PORT            GPIOA
#define LED_PIN             GPIO5

static void gpio_config(void){
    rcc_periph_clock_enable(RCC_GPIOA); //! turn ON the clock for GPIO port A
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN); //? 4th argument can be used to define multiple ports using a bitwise OR (|), 3rd is to configure the state when neither pull-up or pull-down is enabled (intermediate state)
}

int main(void){
    system_setup();
    gpio_config();

    uint64_t start_time = system_get_ticks();
    while(1){
        if(system_get_ticks()-start_time >= 1000){ // time to blink LED
            gpio_toggle(LED_PORT, LED_PIN); //! will toggle the led too fast (50% bright)
            start_time = system_get_ticks();
        }
        // not the time to blink LED, continue with other work
    }
    //! NO RETURN    
    return 0;
}