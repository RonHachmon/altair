/*
 * global_queues.c
 *
 * Created on: Apr 2, 2025
 * Author: 97254
 */

#include "global_queues.h"
#include "sensor_data.h"

osMessageQueueId_t g_sensor_queue;


void global_queues_init(void)
{
	g_sensor_queue = osMessageQueueNew(16, sizeof(SensorData), NULL);

 }

