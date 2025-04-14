#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>

#define LED_PORT            GPIOA
#define LED_PIN             GPIO5
#define CPU_FREQUENCY       (84000000) //! four cycles for each count increment, hence div by 4
#define SYSTICK_FREQUENCY   (1000) 

volatile uint64_t ticks = 0; // why a 64bit variable for a 32bit microcontroller; 32bit will roll over to 0 after 50 days hence 64bit
void sys_tick_handler(void){
    //! implement an interrupt mask
    ticks++; // increment the ticks variable every time the systick interrupt is called
}
static uint64_t get_ticks(void){
    return ticks;
}

static void rcc_config(void){
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]); 
}
static void gpio_config(void){
    rcc_periph_clock_enable(RCC_GPIOA); //! turn ON the clock for GPIO port A
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN); //? 4th argument can be used to define multiple ports using a bitwise OR (|), 3rd is to configure the state when neither pull-up or pull-down is enabled (intermediate state)
}

static void systick_setup(void){ 
    systick_set_frequency(SYSTICK_FREQUENCY, CPU_FREQUENCY); //? set the frequency of the systick timer to 10000Hz
    systick_counter_enable();
    systick_interrupt_enable(); //? enable the systick interrupt
}

int main(void){
    rcc_config();
    gpio_config();

    systick_setup();
    uint64_t start_time = get_ticks();

    while(1){
        if(get_ticks()-start_time >= 1000){ // time to blink LED
            gpio_toggle(LED_PORT, LED_PIN); //! will toggle the led too fast (50% bright)
            start_time = get_ticks();
        }
        // not the time to blink LED, continue with other work
    }
    //! NO RETURN    
    return 0;
}