/*
 * event_task.h
 *
 *  Created on: Apr 3, 2025
 *      Author: 97254
 */

#ifndef INC_TASKS_EVENT_TASK_H_
#define INC_TASKS_EVENT_TASK_H_

#include <stdint.h>

typedef enum {
    EVENT_OK_TO_ERROR = 0,
    EVENT_ERROR_TO_OK,
    EVENT_WD_RESET,
    EVENT_INIT
} AltairEvent;

typedef struct EventData{

	uint32_t timestamp;
	AltairEvent event;

}EventData;

void event_task(void* context);

#endif /* INC_TASKS_EVENT_TASK_H_ */
