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
#include "sync_globals.h"

#include "utils/send_queue.h"

#include "tasks/init_altair_task.h"
#include "tasks/collector_task.h"
#include "tasks/event_task.h"
#include "tasks/logger_task.h"
#include "tasks/flash_task.h"
#include "tasks/uart_task.h"
#include "tasks/keep_alive_task.h"

#include "altair/message_handler.h"

//#include "FreeRTOS.h"
//#include "task.h"


static TransmitContext transmit_context;

// Setup
static void init_system(void);
static void setup_transmit_context(void);

// Task creation
static void create_recieve_task(void);
static void wait_for_time_sync(void);
static void create_core_tasks(void);
static void create_event_task(void);
static void create_logger_task(void);
static void create_collector_task(void);
static void create_transmit_task(void);
static void create_keep_alive_task(void);

// Finalization
static void self_destruct(void);



void init_altair(void* context)
{
	printf("!!!!!!!!!!!!\r\n");

	init_system();

	create_recieve_task();

	wait_for_time_sync();

	create_core_tasks();

	self_destruct();
}

static void init_system() {
    printf("Setting up System...\r\n");

    init_uart();
    init_logger();
    global_queues_init();
    reset_flash();

    setup_transmit_context() ;
}



static void setup_transmit_context() {
    Queue_create(&transmit_context.high_priority);
    Queue_create(&transmit_context.medium_priority);
    Queue_create(&transmit_context.low_priority);
}

static void create_recieve_task() {
    const osThreadAttr_t attrs = {
        .name = "recieveTask",
        .stack_size = 128 * 4 * 8,
        .priority = osPriorityNormal,
    };

    osThreadId_t id = osThreadNew(recieve_task, altair_message_handler, &attrs);
    if (id == NULL) {
        printf("FAILED to create recieve task\r\n");
    }
}

static void wait_for_time_sync() {

//	while(1)
//	{
//
//	    if(FLAG_SET_TIME & osEventFlagsGet(g_evtID)){
//	    	break;
//	    }
//
//	}

    printf("Please provide the time...\r\n");
    osEventFlagsWait(g_evtID, FLAG_SET_TIME, osFlagsWaitAny, 3000);

    printf("Time received. Continuing...\r\n");
}

static void create_core_tasks() {
    create_event_task();
    create_logger_task();
    create_collector_task();
    create_transmit_task();
    create_keep_alive_task();

}

static void create_event_task() {
    const osThreadAttr_t attrs = {
        .name = "eventTask",
        .stack_size = 128 * 4 * 8,
        .priority = osPriorityLow7,
    };

    EventData data;
    DateTime datetime;
    RTC_ReadDateTime(&datetime);
    data.timestamp = datetime_to_timestamp(&datetime);
    data.event = EVENT_INIT;

    osMessageQueuePut(g_event_queue, &data, 0, 0);

    if (osThreadNew(event_task, &transmit_context.medium_priority, &attrs) == NULL) {
        printf("FAILED to create event task\r\n");
    }
}

static void create_logger_task() {
    const osThreadAttr_t attrs = {
        .name = "beaconLoggerTask",
        .stack_size = 128 * 4 * 16,
        .priority = osPriorityLow7,
    };

    if (osThreadNew(logger_beacon_task, NULL, &attrs) == NULL) {
        printf("FAILED to create logger task\r\n");
    }
}

static void create_collector_task() {
    const osThreadAttr_t attrs = {
        .name = "collectDataTask",
        .stack_size = 128 * 4 * 8,
        .priority = osPriorityHigh7,
    };

    static CollectorSetting settings = {0};
    read_settings(&settings);

    if (osThreadNew(collection_task, (void*) &settings, &attrs) == NULL) {
        printf("FAILED to create collector task\r\n");
    }
}

static void create_transmit_task() {
    const osThreadAttr_t attrs = {
        .name = "transmitTask",
        .stack_size = 128 * 4 * 16,
        .priority = osPriorityNormal,
    };

    if (osThreadNew(transmit_task, &transmit_context, &attrs) == NULL) {
        printf("FAILED to create transmit task\r\n");
    }
}
static void create_keep_alive_task(){
    const osThreadAttr_t attrs = {
        .name = "KeepAliveTask",
        .stack_size = 128 * 4 * 16,
        .priority = osPriorityRealtime7,
    };

    if (osThreadNew(keep_alive_task, &transmit_context.high_priority , &attrs) == NULL) {
        printf("FAILED to create keep alive task\r\n");
    }

}

static void self_destruct() {
    printf("Initialization complete. Self-terminating init task.\r\n");

    osStatus_t status = osThreadTerminate(osThreadGetId());
    if (status != osOK) {
        printf("FAILED to terminate init task\r\n");
        while (1); // trap
    }
}






