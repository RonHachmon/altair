/*
 * collector_task.h
 *
 *  Created on: Apr 2, 2025
 *      Author: 97254
 */

#ifndef INC_COLLECTOR_TASK_H_
#define INC_COLLECTOR_TASK_H_

typedef struct CollectorSetting{
	uint16_t delay;
	uint16_t safe_temp;
	uint16_t safe_humidity;
	uint16_t safe_battery;
}CollectorSetting;

void collection_task(void* context);

#endif /* INC_COLLECTOR_TASK_H_ */
