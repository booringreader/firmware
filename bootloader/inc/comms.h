#ifndef INC_COMMS_H
#define INC_COMMS_H

#include "common-defines.h"

#define PACKET_DATA_LENGTH (16)
#define PACKET_LENGTH_BYTES (1)
#define PACKET_CRC_LENGTH (1)
#define PACKET_LENGTH (PACKET_LENGTH_BYTES + PACKET_DATA_LENGTH + PACKET_CRC_LENGTH)

// create the packet structure
typedef struct comms_packet_t{
    uint8_t length;
    uint8_t data[PACKET_DATA_LENGTH]; // 16 bytes (8 bits each)
    uint8_t crc;
}comms_packet_t;

void comms_setup(void); // initial state of the packet state machine
void comms_update(void); // update the packet state machine

boolean comms_packets_available(void);
void comms_write(comms_packet_t* packet);
void comms_read(comms_packet_t* packet);

#endif