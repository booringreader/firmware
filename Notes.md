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

### PWM and Timers - use system clock to generate a modulated pulse, using `timer` peripheral hardware and the concept of PWM
- the autoReloadValue(ARR) register dictates the ticking frequency, compareValue(CCR) register compares the value of the coutner with the autoReloadValue, compareValue register can be modified to control the duty cycle
- PWM is used for motor control (controlling the speed of the motor etc.) or control the frequency of the LED, or control the intentsity of the MOSFET
- "alternate function mapping table" in the datasheet shows that the PA5 (port A, pin5) can also be mapped with the TIM2 timer, hence we use TIM2 for setup (timer.c)
- TIM2 is a peripheral pin, whose output is mapped to the pin5 of port A on stm32. we are using this peripheral to output the value of pwm, which will be used for intensity control or other objectives on hardware
- timer_set_mode() takes parameters: output pin, compareVal (clock division), alignment of pulse, direction (UP or DOWN) of counter progression
- we want 1000 cycles per second(compareValue) (10KHz) and 1000 divisions (autoReloadValue)

### Bootloader
- bootloader and the firmware blocks both have independent code, data and interrupt vector tables.
- bootloader jobs:
    - load the address of the firmware / main application's reset_vector (address which acts as the starting point of execution) and jump there (begin execution)
    - prevent linking when bootloader size exceeds allowed limit (prevent cyber attacks)
    - pad the bootloader to max size (32K in this case), this makes it easier to put the bootloader as the first block of the main applicaiton
    - each entry in IVT is 4bytes
- main application (firmware + bootloader) jobs:
    - attach bootloader object file to the firmware (using .S file)
    - when the firmware execution begins from 0x8000, it will start encountering interrupts from systick_enable() function, the workflow would look like this : encounter interrupt while executing in code block -> jump in the IVT to resolve interrupt -> jump back in the code block to resume execution -> encounter interrupt -> repeat. Instead of jumping in the IVT of the firmware, the PC would look for the interrupt in the IVT of the bootloader. To resolve this, arm chips have a VTOR, vector table offset register.

### Assembly
- `asm-dump` file may show `UNDEFINED` in .vector-table if the hardware isn't connected, since we would be trying to access registers (ARR,CCR) which aren't available yet.
- constant data associated with a function is placed in the vicinity of that function translation (maybe for efficiency)
- using vector-table structure to simplify syntax in bootloader.c, cast the address of the main application into the vector table, then acesss the reset() vector to initiate execution of main application
- the c/c++ compiler inserts padding in structure based on some rules, but each entity in the vector-table is uint32_t (4bytes), hence no padding

### UART Driver - hardware based transfer protocol
- even with different make files and directories, bootlaoder and main application can have shared code through UART
- PA2 and PA3 are connected with USART2 for TX and RX, hence we use USART2 instead of USART1 to ease the connection process.
- UART (universal asynchronous receive and transmit) : data transfer happens at an agreed upon baud rate.
    - USART (universal synchronous or asynchronous receive and transmit) : data transfer can be synced with an internal clock
    - usart uses the rs232 protocol which has a lot of rules on how a signal is transmitted and received. usart_set_flow_control() regulates this protocol
- usart_send_blocking() sends data in blocking mode, no code is executed while the data is being sent. (non-blocking mode can also be used)
- interrupts: usart_isr()
    - normal interrupt byte received
    - byte received and overload happened, more data overlaods the peripheral than it can handle, might lead to buffer overflow
    - read flags / registers from peripheral to decide which of the above has happened
    - store the excess data temporarily in a buffer variable
- Driver Interface:
    - `void uart_setup(void)` : clock, interrupts etc.
    - `void uart_write(uint8_t* data, const uint32_t length)` : pointer to data, longer length, since multiple bytes can be transmitted
    - `void uart_write_byte(uint8_t data)` : instead of bits, send one byte
    - `uint32_t uart_read(uint8_t* data, const uint32_t length)` : pointer to data and length to be read; returns the number of bytes actually read
    - `uint8_t uart_read_byte(void)` : read one byte of data
    - `bool uart_data_available(void)` : check before reading data
- firmware.c:
    - configure ports, pins and setup function
    - main() : check if data is received and send ack
    - uart data is usually characters, assuming we received a ascii character we add '1' to it and return the new data
    future work: (use ring buffer)
        - if we send data faster than the firmware can process it, the data will be overwritten (configured in uart.c). hence we need more buffer storage
        - making `data_available = false` can cause issues if more data comes in immediately after current one is read or an interrupt occurs

### INTERRUPT SAFE RING BUFFER
- system.c and system.h are modified to introduce a waiting period (delay) to simulate the condition of an ISR getting triggered and buffer being overwritten before it can be read
- Ring Buffer (circular buffer): similar to round robin, data wraps around at the end
- we take local copies of the read and write index, to avoid collisions in case multiple users access the buffer
- if the read and write index become equal then all the data following the index is lost, since it gets overwritten before it can be read. 
    - If such a case were to arise, we would need more storage
    - we can move both pointers together, but that would be complex
- ring-buffer size calculation 
    - baud rate / 1000 : bytes coming in per second
    - (bytes/second)*10 rounded off to nearest power of 2 : size of ring buffer

### PACKET TRANSFER PROTOCOL
- packet size is 18 bytes, with 1 leading byte reserved for the length of the data portion, followed by the data portion, last byte is the CRC-8 (8 bit) byte for error checking
- CRC is calculated for all 17 preceding bytes, even if the data portion has only padding
- total data portion size allowed is 16 bytes, whose length can be recorded using only 4 bits, but the 'length' portion is 8 bits, that's 4 bits remaining idle. These 4 bits can be used to fulfill some other purpose, say transferring packet between the system and another uart device, or implementing better error checking or collision handling on multi-bus system.
- DC balancing is a concept which dictates that the longer a signal remains in one state, the more energy it takes to switch the state. if the 16 bytes of data portion is not used to the maximum, there would be padding and the signal through that padding would remain in the same state for a long period of time.
- DC balancing is not a problem in slower protocls like uart because there is hardly any ac coupling or passing through transformers to get damaged.
- predefined fixed packets for 
    - ACK: acknowledgement
    - NACK: not acknowledged
    - ReTx: Retransmission request
- `packet state machine` receives bytes from the data stream and converts them into packets
    - 3 states of receiving packtet information (one for length, one for data bytes, one for CRC)
    - CRC is checked through operations, then either of the 3 paths is followed
        - retransmission requested (ReTx): retransmit the last packet (need to keep a record of the last sent in a buffer), without storing anything received so far
        - if packet type is not ACK, send an ackowledgement and store the packet in a buffer
        - request retransmit in case of bad CRC
    - 0x19: retransmit packet
    - 0x15: acknowledge