#include "core/System.h"

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/rcc.h>

static volatile uint64_t ticks = 0; // why a 64bit variable for a 32bit microcontroller; 32bit will roll over to 0 after 50 days hence 64bit
void sys_tick_handler(void){
    //! implement an interrupt mask
    ticks++; // increment the ticks variable every time the systick interrupt is called
}

static void rcc_setup(void){
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]); 
}

static void systick_setup(void){ 
    systick_set_frequency(SYSTICK_FREQUENCY, CPU_FREQUENCY); //? set the frequency of the systick timer to 10000Hz
    systick_counter_enable();
    systick_interrupt_enable(); //? enable the systick interrupt
}


uint64_t system_get_ticks(void){
    return ticks;
}
void system_setup(void){
    rcc_setup();
    systick_setup();
}