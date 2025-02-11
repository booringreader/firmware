#### Config
- Cortex-M4 chip, NUCLEOF401RE/STM32F401RET6 board

- install libopencm3 with ```bash git submodule add https://github.com/libopencm3/libopencm3.git```
- install GNU arm toolchain and make with ```bash brew install gcc-arm-embedded make ```
- all the code inside an infinite loop (since there is only one program on the microcontroller and it cannot stop at any point)

#### Hello world (firmware.c)
- configure the clock on stm32 since there is no power button, each piece of hardware functions based on the clock frequency
- direct the processor to turn the GPIO pin connected to LED, ON and OFF
- 'static' means the function belongs to the current file and the headers used in the current file
- 'const' means that the function will not modify the argument that is passed in
- pll(phase locked loop) takes in one clock signal and generates another clock signal (may or may not with some modifications such as phase shifting etc.)
- each port on the stm32 board is made up of a collection of pins, in nucleo stm32f401, the LED is connected to PA5 (port A pin 5).
- all the peripherals and ports are OFF by default on cortex processor boards (clocks need to be turned ON before any device can function)
- ONE_SECOND for a 84MHz clock isn't 84000000 cycles per second, since the "nop" instruction takes one cycle and i++ takes one cycle, i=0 takes one cycle, hence atleast 4 cycles are going on to execute one iteration of the delay_cycle() for loop. Hence a division by 4 is necessary to keep the time to 1 second.