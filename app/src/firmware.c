#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>

#define LED_PORT            GPIOA
#define LED_PIN             GPIO5
#define CPU_FREQUENCY       (84000000) //! four cycles for each count increment, hence div by 4
#define SYSTICK_FREQUENCY   (1000) //! 1ms

volatile uint64_t ticks = 0; // why a 64bit variable for a 32bit microcontroller; 32bit will roll over to 0 after 50 days hence 64bit
static void systick_handler(void){
    //! implement an interrupt mask
    ticks++; // increment the ticks variable every time the systick interrupt is called
}
uint64_t get_ticks(void){
    return ticks; // return the current value of ticks
}

static void rcc_config(){
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]); 
}
static void gpio_config(){
    rcc_periph_clock_enable(RCC_GPIOA); //! turn ON the clock for GPIO port A
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN); //? 4th argument can be used to define multiple ports using a bitwise OR (|), 3rd is to configure the state when neither pull-up or pull-down is enabled (intermediate state)
}

static void systick_setup(){ //? 10000 is the number of clock cycles to wait for, CPU_FREQUENCY is the frequency of the system clock
    systick_set_frequency(1000, CPU_FREQUENCY); //? set the frequency of the systick timer to 10000Hz
    systick_counter_enable();
    systick_interrupt_enable(); //? enable the systick interrupt
}

static void delay_cycles(uint32_t cycles){ //? clock is running at 84MHZ i.e 84million cycles per second, so for a 1 second delay, the number of cycles would have to be 84million. Hence 32 bit input is an appropriate size
    for(uint32_t i=0; i<cycles; i++){
        __asm__("nop"); //! so the compiler doesn't discard the loop on the grounds of nothing happening inside it
    }
}

int main(){
    rcc_config();
    gpio_config();

    while(1){
        gpio_toggle(LED_PORT, LED_PIN); //! will toggle the led too fast (50% bright)
        delay_cycles(CPU_FREQUENCY/4); // will take in no. of clock cycles and delay the toggle by that amount
    }

    //! NO RETURN    
    return 0;
}