#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h> // enables the ability to use interrupts on usart2
#include <libopencm3/stm32/usart.h>

#include "core/uart.h"
#include "core/ring-buffer.h"

#define BAUD_RATE (115200)
#define RING_BUFFER_SIZE (128) // for 10ms

static ring_buffer_t rb = {0U};

static uint8_t data_buffer[RING_BUFFER_SIZE] = {0U};

void usart_isr(void){ // interrupt handler
    const bool overrun_occured = usart_get_flag(USART2, USART_FLAG_ORE) == 1;
    const bool received_data = usart_get_flag(USART2, USART_FLAG_RXNE) == 1;

    if(received_data || overrun_occured){
        if(ring_buffer_write(&rb,(uint8_t)usart_recv(USART2))){
            // error handling
        }
    }
}
void uart_setup(void){
    ring_buffer_setup(&rb, data_buffer, RING_BUFFER_SIZE);
    rcc_periph_clock_enable(RCC_USART2);

    usart_set_mode(USART2, USART_MODE_TX_RX);

    // configuration details in the time cycle diagram photo
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE); 
    usart_set_databits(USART2, 8); // to send bytes at a time
    usart_set_baudrate(USART2, BAUD_RATE); // average baudrate, can be increased or decreased
    usart_set_parity(USART2, 0);
    usart_set_stopbits(USART2, 1);
    
    // interrupts
    usart_enable_rx_interrupt(USART2);
    nvic_enable_irq(NVIC_USART2_IRQ);

    usart_enable(USART2); // enable the USART2 peripheral
}

void uart_write(uint8_t* data, const uint32_t length){
    for(uint32_t i=0; i<length; i++){
        uart_write_byte(data[i]);
    } 
}

void uart_write_byte(uint8_t data){
    usart_send_blocking(USART2, (uint16_t)data);
}
uint32_t uart_read(uint8_t* data, const uint32_t length){ // rn only 1 byte can be read
   if(length == 0) return 0;
   for(uint32_t byte_read=0; byte_read<length; byte_read++){
    if(!ring_buffer_read(&rb, &data[byte_read])){
        return byte_read;
    }
   } 
   return length;
}
uint8_t uart_read_byte(void){
    uint8_t byte = 0;
    (void)uart_read(&byte, 1); //! to ignore a return value of func. typecast it to void
    return byte;
}
bool uart_data_available(void){
    return !ring_buffer_empty(&rb);
}