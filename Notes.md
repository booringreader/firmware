#### Config
- Cortex-M4 chip, NUCLEOF401RE/STM32F401RET6 board

- install libopencm3 with ```bash git submodule add https://github.com/libopencm3/libopencm3.git```
- install GNU arm toolchain and make with ```bash brew install gcc-arm-embedded make ```
- all the code inside an infinite loop (since there is only one program on the microcontroller and it cannot stop at any point)

#### blinker (firmware.c)
- configure the clock on stm32 since there is no power button, each piece of hardware functions based on the clock frequency
- direct the processor to turn the GPIO pin connected to LED, ON and OFF
- 'static' means the function belongs to the current file and the headers used in the current file
- 'const' means that the function will not modify the argument that is passed in
- pll(phase locked loop) takes in one clock signal and generates another clock signal (may or may not with some modifications such as phase shifting etc.)
- each port on the stm32 board is made up of a collection of pins, in nucleo stm32f401, the LED is connected to PA5 (port A pin 5).
- all the peripherals and ports are OFF by default on cortex processor boards (clocks need to be turned ON before any device can function)
- ONE_SECOND for a 84MHz clock isn't 84000000 cycles per second, since the "nop" instruction takes one cycle and i++ takes one cycle, i=0 takes one cycle, hence atleast 4 cycles are going on to execute one iteration of the delay_cycle() for loop. Hence a division by 4 is necessary to keep the time to 1 second.

#### libopencm3 working

### Generic
- the datasheet of the device helps to plan out the requirements for the firmware (the peripherals, interfaces etc.)
- memory map of the device helps in writing optimised code for that device. It tells how the memory chips are placed and configured to communicate and divide information
- the 'reference manual' is the guide of interacting with the device and its specific parts

### Interrupts & MemMapped IO
- GPIO_MODER(port) takes a port as argument and replaces it with MMIO32((port) + 0x00); i.e. offset the value of port
- MMIO32 is a macro for casting the address to a pointer, the keyword 'volatile' in the macro instructs the compiler to avoid optimizing the pointer (since optimization might change the associations of the memory address value with the actual hardware)
- in our case, we use GPIOA as port, which itself is a macro for GPIO_PORT_A_BASE
- PERIPH_BASE_AHB is advanced high speed bus, PERIPH_BASE_APB is advanced peripheral bus
- all the macros in GPIO_MODER boil down to memory locations of peripheral registers

- we need to improve the while loop of 'nop' so that more tasks can be accomplished instead of just 'nop' while the loop executes
    - to this end, we will use a peripheral(```systick```), since these do not require additional code to get things done in the background
- systick lets the program know that one time cycle has been completed by using an interrupt
    - the ```interrupt``` when triggered, transfers control to the interrupt handler function (a default or user-defined) in cm3/vector.h library
    - the 'reset' vector in the interrupt vector table runs even before the main() function // when program boots up
    - ```sys_tick_handler``` is an alias for a ```null_handler``` which does nothing and since systick_handler is a weak function, it can be defined somewhere else too
    - we use a 64bit variable to store the count of ticks, but since the microcontroller is 32 bit, addition would be performed twice (32+32). the problem arises when a new interrupt occurs between these two addition operations. hence, mask other interrupts when entering systick_handler interrupt.