#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED_PORT GPIOA
#define LED_PIN GPIO5
#define ONE_SECOND (84000000/4) //! four cycles for each count increment, hence div by 4

static void rcc_config(){
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]); 
}
static void gpio_config(){
    rcc_periph_clock_enable(RCC_GPIOA); //! turn ON the clock for GPIO port A
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN); //? 4th argument can be used to define multiple ports using a bitwise OR (|), 3rd is to configure the state when neither pull-up or pull-down is enabled (intermediate state)
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
        delay_cycles(ONE_SECOND); // will take in no. of clock cycles and delay the toggle by that amount
    }

    //! NO RETURN    
    return 0;
}