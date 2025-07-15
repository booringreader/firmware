#include "comms.h"
#include "core/uart.h"

typedef enum comms_state_t{
    CommsState_Length,
    CommsState_Data,
    CommsState_CRC
} comms_state_t;

static comms_state_t state = CommsState_Length;
static uint8_t data_byte_count = 0; // count 16 bytes
static comms_packet_t temporary_packet = {.length = 0, .data = {0}, .crc = 0};

void comms_setup(void){

}
void comms_update(void){
    while(uart_data_available()){ // work whilte the data is available
        switch(state){
            case CommsState_Length: {
                temporary_packet.length = uart_read_byte();
                state = CommsState_Data;
            } break;
            case CommsState_Data: {
                temporary_packet.data[data_byte_count++] = uart_read_byte();
                if(data_byte_count >= PACKET_DATA_LENGTH){
                    data_byte_count = 0;
                    state = CommsState_CRC;
                }
            } break;
            case CommsState_CRC: {
                temporary_packet.crc = uart_read_byte();
                // check the crc of the data+length portions
                uint8_t computed_crc = crc8((uint8_t*)&temporary_packet,PACKET_LENGTH - PACKET_CRC_LENGTH);
                // be careful while typecasting structures, if the definition contains different data types, the compiler will add padding
                // we can typecase here because the packet is just a series of bytes, but if there was padding, the bytes won't be contiguous
                if(temporary_packet.crc != computed_crc){

                }
            } break;
            default: {
                state = CommsState_Length; // reset the state machine
            }
        }
    }
}

boolean comms_packets_available(void);
void comms_write(comms_packet_t* packet);
void comms_read(comms_packet_t* packet);
