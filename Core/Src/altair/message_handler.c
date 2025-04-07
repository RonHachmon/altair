/*
 * message_handler.c
 *
 *  Created on: Apr 6, 2025
 *      Author: 97254
 */

#include <string.h>

#include "altair/message_handler.h"

#include "DateTime.h"
#include "sync_globals.h"

#include "utils/send_queue.h"

#include "tasks/init_altair_task.h"
#include "tasks/collector_task.h"
#include "tasks/event_task.h"
#include "tasks/logger_task.h"
#include "tasks/flash_task.h"
#include "tasks/uart_task.h"


//typedef enum {
//	PACKET_TYPE_SEND_CLOCK =  0x00,
//	PACKET_TYPE_KEEP_ALIVE =  0x01,
//	PACKET_TYPE_GET_CLOCK = 0x02,
//	PACKET_TYPE_UPDATE_TEMP = 0x03,
//	PACKET_TYPE_UPDATE_HUMIDITY = 0x04,
//	PACKET_TYPE_UPDATE_VOLTAGE = 0x05,
//	PACKET_TYPE_UPDATE_LIGHT = 0x06,
//	PACKET_TYPE_EVENT = 0x07,
//} PacketType;


void altair_message_handler(uint8_t const * message, uint8_t len)
{
	if(message == NULL){
		return;
	}

	MessagePacket packet;

	packet.data_len = message[0];
	packet.packetType = message[1];
	packet.checksum = message[2];

	memcpy(packet.buffer, message, packet.data_len);

	if(packet.data_len == PACKET_TYPE_GET_CLOCK){
		float timestamp;
		memcpy(&timestamp, packet.buffer, sizeof(4));

	}



	if(strncmp("time ", message, 5) == 0)
	{
		uint32_t timestamp = atoi(message + 5);
		if(timestamp != 0)
		{
			DateTime datetime;
			printf("time input %d\r\n", timestamp);
			parse_timestamp(timestamp, &datetime);

			RTC_SetDateTime(&datetime);
			printf("time is set\r\n");
			RTC_PrintDateTime(&datetime);

			osEventFlagsSet(g_evtID, FLAG_SET_TIME);
		}

	}
}

void send_message(Queue* queue, MessagePacket* message)
{
	if(queue == NULL || message == NULL){
			return;
		}

		uint8_t writeBuf[128];

		writeBuf[0] = message->data_len;
		writeBuf[1] = message->packetType;
		writeBuf[2] = message->checksum;

		// Copy the message payload to writeBuf starting at index 3
		memcpy(&writeBuf[3], message->buffer, message->data_len);

		// Append END_MARK after the payload
		writeBuf[3 + message->data_len] = END_MARK;

		// Total length is data_len + 4 (3 header bytes + END_MARK)
		Queue_enque(queue, writeBuf, message->data_len + 4);
}
