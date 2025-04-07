/*
 * keep_alive_task.c
 *
 *  Created on: Apr 7, 2025
 *      Author: 97254
 */
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ff_gen_drv.h"

#include "tasks/keep_alive_task.h"
#include "utils/send_queue.h"
#include "altair/message_handler.h"

#include "sensor_data.h"
#include "sync_globals.h"
#include "DateTime.h"

static void create_keep_alive_packet(MessagePacket* packet);

void keep_alive_task(void* context)
{
	printf("start keep alive\r\n");
	Queue* transmit_queue = (Queue*) context;




	while(UNINTILIZED_MODE == g_latest_sensor_data.mode){
		osDelay(100);
	}

	MessagePacket message_packet;


	printf("running keep alive\r\n");
	while(1){

		create_keep_alive_packet(&message_packet);

		//enque to logger
		osMessageQueuePut(g_sensor_queue, &g_latest_sensor_data, 0, 0);
		//Queue_enque(transmit_queue, (uint8_t*) writeBuf , data_len + 1);

		//send to uart
		send_message(transmit_queue, &message_packet);

		osEventFlagsSet(g_evtID, FLAG_KEEP_ALIVE);

		osDelay(10000);
	}
}


//		message_packet.buffer[0] = g_latest_sensor_data.temp;
//		message_packet.buffer[1] = g_latest_sensor_data.humid;
//		message_packet.buffer[2] = g_latest_sensor_data.light;
//		message_packet.buffer[3] = g_latest_sensor_data.mode;
//
//		uint8_t* voltage = (uint8_t*)&g_latest_sensor_data.volage;
//		message_packet.buffer[4] = voltage[0];
//		message_packet.buffer[5] = voltage[1];
//		message_packet.buffer[6] = voltage[2];
//		message_packet.buffer[7] = voltage[3];
//
//		uint8_t* timstamp = (uint8_t*)&g_latest_sensor_data.timestamp;
//		message_packet.buffer[8]   =  timstamp[0];
//		message_packet.buffer[9]   =  timstamp[1];
//		message_packet.buffer[10]  =  timstamp[2];
//		message_packet.buffer[11]  =  timstamp[3];


static void create_keep_alive_packet(MessagePacket* packet)
{
    if (packet == NULL) {
        return; // Ensure the pointer is valid
    }

    packet->packetType = PACKET_TYPE_KEEP_ALIVE;
    packet->data_len = 12;
    packet->checksum = 8; // Or calculate it if needed
    packet->end_mark = END_MARK;

    packet->buffer[0] = g_latest_sensor_data.temp;
    packet->buffer[1] = g_latest_sensor_data.humid;
    packet->buffer[2] = g_latest_sensor_data.light;
    packet->buffer[3] = g_latest_sensor_data.mode;

    uint8_t* voltage = (uint8_t*)&g_latest_sensor_data.volage;
    memcpy(&packet->buffer[4], voltage, sizeof(float));

    uint8_t* timestamp = (uint8_t*)&g_latest_sensor_data.timestamp;
    memcpy(&packet->buffer[8], timestamp, sizeof(uint32_t));
}



// uint16_t data_len;
//BYTE writeBuf[128];
//DateTime datetime;
//		parse_timestamp(g_latest_sensor_data.timestamp, &datetime);
//		RTC_DateTimeToString((char*) writeBuf, &datetime);
//		data_len = strlen((char*)writeBuf);
//
//		sprintf(writeBuf + data_len,
//		        "\r\nTemperature: %d\r\n"
//		        "Humidity: %d\r\n"
//		        "Light: %d\r\n"
//		        "Voltage: %.2f\r\n"
//		        "Mode: Safe\r\n"
//		        "---------------\r\n",
//		        g_latest_sensor_data.temp,
//		        g_latest_sensor_data.humid,
//		        g_latest_sensor_data.light,
//		        g_latest_sensor_data.volage);
//
//		data_len = strlen((char*)writeBuf);

//#define END_MARK 0x55
//
//typedef enum {
//	PACKET_TYPE_SEND_CLOCK =  0x00,
//	PACKET_TYPE_KEEP_ALIVE =  0x01,
//	PACKET_TYPE_GET_CLOCK = 0x02,
//	PACKET_TYPE_UPDATE_TEMP = 0x03,
//	PACKET_TYPE_UPDATE_HUMIDITY = 0x04,
//	PACKET_TYPE_UPDATE_VOLTAGE = 0x05,
//	PACKET_TYPE_UPDATE_LIGHT = 0x06,
//} PacketType;
//
//
//
//typedef struct MessagePacket{
//	uint8_t data_len;
//	uint8_t packetType;
//	uint8_t checksum;
//	uint8_t buffer[128];
//	uint8_t end_mark;
//}MessagePacket;
