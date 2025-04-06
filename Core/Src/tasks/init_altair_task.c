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
#include "global_queues.h"

#include "utils/send_queue.h"

#include "tasks/init_altair_task.h"
#include "tasks/collector_task.h"
#include "tasks/event_task.h"
#include "tasks/logger_task.h"
#include "tasks/flash_task.h"
#include "tasks/uart_task.h"

#include "altair/message_handler.h"

//#include "FreeRTOS.h"
//#include "task.h"


static TransmitContext transmit_context;




void init_altair(void* context)
{

	DateTime datetime;



	//uint32_t timestamp = get_time();
	uint32_t timestamp = 1743517905;



	//parse_timestamp(timestamp, &datetime);

	//Initialization
	//RTC_SetDateTime(&datetime);
	init_uart();
	init_logger();
	global_queues_init();
	reset_flash();


	Queue_create(&transmit_context.high_priority);
	Queue_create(&transmit_context.medium_priority);
	Queue_create(&transmit_context.low_priority);






	const osThreadAttr_t recieveTask = {
	  .name = "recieveTask",
	  .stack_size = 128 * 4 * 8,
	  .priority = osPriorityNormal,
	};

	osThreadId_t recieve = osThreadNew(recieve_task, altair_message_handler, &recieveTask);

	if(recieve == NULL){
		printf("create recieve FAIL \r\n");
	}

	printf("please provide the time\r\n");

	osEventFlagsWait(g_evtID, FLAG_SET_TIME, osFlagsWaitAny,HAL_MAX_DELAY);
	printf("init time recieved\r\n");


// ------ Task attributes ---------

	const osThreadAttr_t transmitTask = {
	  .name = "transmitTask",
	  .stack_size = 128 * 4 * 16,
	  .priority = osPriorityNormal,
	};

	const osThreadAttr_t eventTask = {
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

	printf("created attributes\r\n");
// ------ End Task attributes ---------


// ------ Create Tasks ---------

	EventData data;
	RTC_ReadDateTime(&datetime);
	data.timestamp = datetime_to_timestamp(&datetime);
	data.event = EVENT_INIT;

	osMessageQueuePut(g_event_queue, &data, 0,0);

	osThreadId_t event = osThreadNew(event_task, &transmit_context.medium_priority, &eventTask);

	if(event == NULL){
		printf("created event FAIL \r\n");
	}
	printf("created e\r\n");



	osThreadId_t beacon_logger = osThreadNew(logger_beacon_task, &transmit_context.high_priority, &beaconLoggerTask);

	if(beacon_logger == NULL){
		printf("logger task FAIL \r\n");
	}

	osDelay(1000);

	static CollectorSetting settings = {0};
	read_settings(&settings);

	osThreadId_t collector = osThreadNew(collection_task,(void*) &settings, &collectDataTask);

	if(collector == NULL){
		printf("collector task FAIL \r\n");
	}

	osThreadId_t transmit = osThreadNew(transmit_task, &transmit_context, &transmitTask);

	if(transmit == NULL){
		printf("transmit task FAIL \r\n");
	}

	printf("created tasks\r\n");


// ------ End Create Tasks ---------


// ------ Self-Destruct ---------
	printf("init task destroyed\r\n");
	osStatus_t status =  osThreadTerminate(osThreadGetId());

	if(status != osOK){
	  printf("Fail terminate \r\n");
	  while(1){

	  }

	}


}






