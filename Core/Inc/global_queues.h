/*
 * global_queues.h
 *
 *  Created on: Apr 2, 2025
 *      Author: 97254
 */

#ifndef INC_GLOBAL_QUEUES_H_
#define INC_GLOBAL_QUEUES_H_

#include "cmsis_os2.h"

extern osMessageQueueId_t g_sensor_queue;

extern osMessageQueueId_t g_event_queue;

void global_queues_init(void);

#endif /* INC_GLOBAL_QUEUES_H_ */
