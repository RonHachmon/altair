/*
 * collector_task.h
 *
 *  Created on: Apr 2, 2025
 *      Author: 97254
 */

#ifndef INC_COLLECTOR_TASK_H_
#define INC_COLLECTOR_TASK_H_

#include "main.h"

typedef struct CollectorSetting{
	uint16_t delay;
	uint16_t min_temp;
	uint16_t max_temp;
	uint16_t min_humidity;
	uint8_t min_light;
	float safe_voltage;

}CollectorSetting;

void collection_task(void* context);

#endif /* INC_COLLECTOR_TASK_H_ */
