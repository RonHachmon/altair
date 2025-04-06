/*
 * global_queues.c
 *
 * Created on: Apr 2, 2025
 * Author: 97254
 */

#include "global_queues.h"
#include "sensor_data.h"
#include "tasks/event_task.h"


osMessageQueueId_t g_sensor_queue;

osMessageQueueId_t g_event_queue;

osEventFlagsId_t g_evtID;

void global_queues_init(void)
{
	g_sensor_queue = osMessageQueueNew(16, sizeof(SensorData), NULL);
	g_event_queue =  osMessageQueueNew(16, sizeof(EventData), NULL);

	g_evtID = osEventFlagsNew(NULL);

 }

