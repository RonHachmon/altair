/*
 * event_task.c
 *
 *  Created on: Apr 3, 2025
 *      Author: 97254
 */

#include "tasks/event_task.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ff_gen_drv.h"
#include "DateTime.h"
#include "global_queues.h"

static void print_event_status(char *buffer, AltairEvent event);


void event_task(void* context)
{
    FIL fil;
    FRESULT fres;

    fres = f_mkdir("0:/events");
    if (fres != FR_OK && fres != FR_EXIST) {
	    	printf("f_open error to directory (%i)\r\n", fres);
	        while(1);
	}

    fres = f_open(&fil, "0:/events/event", FA_OPEN_APPEND | FA_WRITE);
    if (fres != FR_OK) {
    	printf("f_open error to write (%i)\r\n", fres);
        while(1);
    }

    BYTE writeBuf[128];
    UINT bytesWrote;


    while(1){
    	EventData data;
    	DateTime datetime;
    	uint16_t data_len;
    	osStatus_t status;


		//status = osMessageQueueGet(queue, &data, 0, HAL_MAX_DELAY);
    	status = osMessageQueueGet(g_event_queue, &data, 0, HAL_MAX_DELAY);

    	 if(status == osOK)
    	 {
			parse_timestamp(data.timestamp, &datetime);
			RTC_DateTimeToString((char*) writeBuf, &datetime);

			data_len = strlen((char*)writeBuf);
			print_event_status(writeBuf + data_len, data.event);


			data_len = strlen((char*)writeBuf);
			fres = f_write(&fil, writeBuf, data_len, &bytesWrote);

			if (fres == FR_OK) {
				printf("writing total bytes %i:\r\n",bytesWrote);
				printf("%s\r\n", writeBuf);
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
