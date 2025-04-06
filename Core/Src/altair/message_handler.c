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


void altair_message_handler(uint8_t const * message, uint8_t len)
{
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
