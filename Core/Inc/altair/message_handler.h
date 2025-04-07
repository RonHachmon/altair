/*
 * message_handler.h
 *
 *  Created on: Apr 6, 2025
 *      Author: 97254
 */

#ifndef INC_ALTAIR_MESSAGE_HANDLER_H_
#define INC_ALTAIR_MESSAGE_HANDLER_H_

#include <stdint.h>
#include "utils/send_queue.h"

#define END_MARK 0x55


typedef enum {
	PACKET_TYPE_SEND_CLOCK =  0x00,
	PACKET_TYPE_KEEP_ALIVE =  0x01,
	PACKET_TYPE_GET_CLOCK = 0x02,
	PACKET_TYPE_UPDATE_TEMP = 0x03,
	PACKET_TYPE_UPDATE_HUMIDITY = 0x04,
	PACKET_TYPE_UPDATE_VOLTAGE = 0x05,
	PACKET_TYPE_UPDATE_LIGHT = 0x06,
	PACKET_TYPE_EVENT = 0x07,
	PACKET_TYPE_ACK = 0x08,
	PACKET_TYPE_NACK = 0x09,
} PacketType;



typedef struct MessagePacket{
	uint8_t data_len;
	uint8_t packetType;
	uint8_t checksum;
	uint8_t buffer[128];
	uint8_t end_mark;
}MessagePacket;


void altair_message_handler(uint8_t const * message, uint8_t len);

void send_message(Queue* queue, MessagePacket* message);


#endif /* INC_ALTAIR_MESSAGE_HANDLER_H_ */
