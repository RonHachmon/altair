/*
 * event_task.c
 *
 *  Created on: Apr 3, 2025
 *      Author: 97254
 */



#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ff_gen_drv.h"
#include "DateTime.h"
#include "sync_globals.h"

#include "tasks/event_task.h"
#include "utils/send_queue.h"
#include "altair/message_handler.h"

static void print_event_status(char *buffer, AltairEvent event);
static void create_event_packet(MessagePacket* packet, EventData* event_data);

void event_task(void* context)
{
	Queue* transmit_queue = (Queue*) context;
    FIL fil;
    FRESULT fres;

    fres = f_mkdir("0:/events");
    if (fres != FR_OK && fres != FR_EXIST) {
	    	printf("f_open error to directory (%i)\r\n", fres);
	        while(1);
	}

    //fres = f_open(&fil, "0:/events/event", FA_OPEN_APPEND | FA_WRITE);

    fres = f_open(&fil, "0:/events/event", FA_CREATE_ALWAYS | FA_WRITE);
    if (fres != FR_OK) {
    	printf("f_open error to write (%i)\r\n", fres);
        while(1);
    }

    BYTE writeBuf[128];
    UINT bytesWrote;

    MessagePacket message_packet;


    while(1){
    	EventData data;
    	DateTime datetime;
    	uint16_t data_len;
    	osStatus_t status;


    	status = osMessageQueueGet(g_event_queue, &data, 0, HAL_MAX_DELAY);

    	 if(status == osOK)
    	 {
//			parse_timestamp(data.timestamp, &datetime);
//			RTC_DateTimeToString((char*) writeBuf, &datetime);
//
//			data_len = strlen((char*)writeBuf);
//			print_event_status(writeBuf + data_len, data.event);
//
//
//			data_len = strlen((char*)writeBuf);
//			fres = f_write(&fil, &data, sizeof(EventData), &bytesWrote);


    		fres = f_write(&fil, &data, sizeof(EventData), &bytesWrote);
			//printf("size  EventData %d, size Wrote %d\r\n", sizeof(EventData), bytesWrote);

			if (fres == FR_OK) {
				create_event_packet(&message_packet, &data);

				//Queue_enque(transmit_queue, (uint8_t*) writeBuf , data_len + 1);
				send_message(transmit_queue, &message_packet);

				osEventFlagsSet(g_evtID, FLAG_EVENT);
				printf("wrote event to queue \r\n");


			} else {
				printf("f_write error (%i)\r\n", fres);
			}
			f_sync(&fil);
    	 }
    	 else{
    		 printf("failed to get data,Status: %d\r\n", status);
    	 }
    }
}


static void create_event_packet(MessagePacket* packet, EventData* event_data)
{

    packet->packetType = PACKET_TYPE_EVENT;
    packet->data_len = 5;
    packet->checksum = 8;
    packet->end_mark = END_MARK;

    packet->buffer[0] = event_data->event;

    uint8_t* timestamp = (uint8_t*)&event_data->timestamp;
    memcpy(&packet->buffer[1], timestamp, sizeof(uint32_t));
}


static void print_event_status(char *buffer, AltairEvent event) {
    if (buffer == NULL) {
        printf("Invalid buffer!\n");
        return;
    }

    switch (event) {
        case EVENT_OK_TO_ERROR:
            sprintf(buffer, "\r\nStatus: OK to ERROR\r\n---------------\r\n");
            break;
        case EVENT_ERROR_TO_OK:
            sprintf(buffer, "\r\nStatus: ERROR to OK\r\n---------------\r\n");
            break;
        case EVENT_WD_RESET:
            sprintf(buffer, "\r\nStatus: Watchdog Reset\r\n---------------\r\n");
            break;
        case EVENT_INIT:
            sprintf(buffer, "\r\nStatus: System Initialized\r\n---------------\r\n");
            break;
        default:
            sprintf(buffer, "\r\nStatus: Unknown Event\r\n---------------\r\n");
            break;
    }
}
