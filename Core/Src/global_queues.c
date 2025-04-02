/*
 * global_queues.c
 *
 * Created on: Apr 2, 2025
 * Author: 97254
 */

#include "global_queues.h"
#include "cmsis_os.h" // Include the CMSIS-RTOS header
#include "beacon_data.h"

osMessageQueueId_t beacon_queue;


void global_queues_init(void)
{
   beacon_queue = osMessageQueueNew(16, sizeof(BeaconData), NULL); // Adjust size and data type

 }
