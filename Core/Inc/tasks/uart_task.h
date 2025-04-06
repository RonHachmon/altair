/*
 * uart_task.h
 *
 *  Created on: Apr 5, 2025
 *      Author: 97254
 */

#ifndef INC_TASKS_UART_TASK_H_
#define INC_TASKS_UART_TASK_H_

#include "main.h"

typedef enum {
	PACKET_TYPE_SEND_CLOCK = 0x00,
	PACKET_TYPE_KEEP_ALIVE = 0x00,
	PACKET_TYPE_GET_CLOCK = 0x10,
	PACKET_TYPE_UPDATE_TEMP = 0x11,
	PACKET_TYPE_UPDATE_HUMIDITY = 0x12,
	PACKET_TYPE_UPDATE_VOLTAGE = 0x13,
	PACKET_TYPE_UPDATE_LIGHT = 0x14,
} PacketType;

typedef struct AltairPacket{
	uint16_t sequence_id;
	PacketType packetType;
	uint8_t data_len;
	uint8_t checksum;
	uint8_t buffer[128];
}AltairPacket;

void send_packet(void* context);

void uart_task(void* context);

#endif /* INC_TASKS_UART_TASK_H_ */
