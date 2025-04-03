/*
 * altair.c
 *
 *  Created on: Mar 31, 2025
 *      Author: Ron Hachmon
 */

#include "main.h"
#include "usart.h"
#include <stdlib.h>

#include "DateTime.h"
#include "logger.h"

#include "global_queues.h"
#include "tasks/collector_task.h"



int _write(int file, char* ptr, int len);
static uint32_t get_time();


void altair(void* context)
{
	static CollectorSetting settings = {0};
	DateTime datetime;



	//uint32_t timestamp = get_time();
	uint32_t timestamp = 1743517905;

	settings.delay = 2000;

	parse_timestamp(timestamp, &datetime);

	//Initialization
	RTC_SetDateTime(&datetime);
	init_logger();
	global_queues_init();



	const osThreadAttr_t EventTask = {
	  .name = "eventTask",
	  .stack_size = 128 * 4 * 8,
	  .priority = osPriorityLow7,
	};

	const osThreadAttr_t collectDataTask = {
	  .name = "collectDataTask",
	  .stack_size = 128 * 4 * 8,
	  .priority = osPriorityRealtime7,
	};


	const osThreadAttr_t beaconLoggerTask = {
	  .name = "beaconLoggerTask",
	  .stack_size = 128 * 4 * 16,
	  .priority = osPriorityLow7,
	};






	osThreadId_t beacon_logger = osThreadNew(logger_beacon_task, NULL, &beaconLoggerTask);

	if(beacon_logger != NULL){
		printf("created beacon logger \r\n");
	}

	osDelay(1000);

	osThreadId_t collector = osThreadNew(collection_task,(void*) &settings, &collectDataTask);

	if(collector != NULL){
		printf("created collector \r\n");
	}

//	osThreadId_t event = osThreadNew(event_task, (void*) event_queue, &EventTask);
//
//	if(event != NULL){
//		printf("created event task \r\n");
//	}
//
//	EventData data;
//	RTC_ReadDateTime(&datetime);
//	data.timestamp = datetime_to_timestamp(&datetime);
//	data.event = EVENT_INIT;
//
//	xQueueSend(event_queue, &data, 0);





	osStatus_t status =  osThreadTerminate(osThreadGetId());

	if(status != osOK){
	  printf("Fail terminate \r\n");
	  while(1){

	  }

	}

}

//1743517905
static uint32_t get_time()
{
	char time_buffer[10] = {0};
	char input_buffer[1] = {0};
	uint8_t command_index = 0;
	uint32_t timestamp = 0;

	while(1){

		HAL_UART_Receive(&huart2,(uint8_t*) input_buffer, 1, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2,(uint8_t*) input_buffer, 1, HAL_MAX_DELAY);

		if (*input_buffer != '\r') {
			if( *input_buffer < '0' || *input_buffer > '9')
			{
				continue;
			}
			time_buffer[command_index] = *input_buffer;
			++command_index;

		} else {

			time_buffer[command_index] = '\0';
			command_index = 0;

			break;
		}

	}

	timestamp = atoi(time_buffer);
	printf("recived %ld\r\n", timestamp);


	return timestamp;

}


int _write(int file, char* ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);

	return len;
}


