/*
 * uart_task.c
 *
 *  Created on: Apr 5, 2025
 *      Author: 97254
 */


#include "tasks/uart_task.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "sync_globals.h"

#define ENTER '\r'

static uint8_t uart_buffer[1];

static Queue recieve_queue;

static osSemaphoreId_t commands;



int _write(int file, char* ptr, int len);

static void wait_for_data_if_all_queues_empty(TransmitContext* ctx);

static void transmit_queue_data_beacon(Queue* queue);
static void transmit_queue_event(Queue* queue);


void init_uart()
{
	Queue_create(&recieve_queue);
	commands = osSemaphoreNew(8, 0, NULL);

	HAL_UART_Receive_IT(&huart2, uart_buffer, sizeof(uart_buffer));
}


void transmit_task(void* context)
{
    TransmitContext* ctx = (TransmitContext*) context;

    while (1)
    {
        wait_for_data_if_all_queues_empty(ctx);

        if (Queue_size(&ctx->high_priority) != 0) {
        	transmit_queue_data_beacon(&ctx->high_priority);
        }
        else if (Queue_size(&ctx->medium_priority) != 0) {
        	transmit_queue_event(&ctx->medium_priority);
        }
//        else if (Queue_size(&ctx->low_priority) != 0) {
//            //transmit_queue_data(&ctx->low_priority);
//        }
    }
}

static void wait_for_data_if_all_queues_empty(TransmitContext* ctx)
{
    if (Queue_size(&ctx->high_priority) == 0 &&
        Queue_size(&ctx->medium_priority) == 0 &&
        Queue_size(&ctx->low_priority) == 0) {
        osEventFlagsWait(g_evtID, FLAG_KEEP_ALIVE | FLAG_REPORTS | FLAG_EVENT, osFlagsWaitAny, HAL_MAX_DELAY);
    }
}



static void transmit_queue_event(Queue* queue)
{

	uint8_t len = Queue_getChar(queue);
	uint8_t current_char;
	printf("EVENT \r\n");
	printf("data len: %u\r\n", len);

	current_char = Queue_getChar(queue);
	len--;
	printf("Type : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("checksum : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("Event : %u\r\n", current_char);

	uint32_t timstamp;
	uint8_t* uint32_t_buffer = (uint8_t*) &timstamp;
	*uint32_t_buffer++ = Queue_getChar(queue);
	*uint32_t_buffer++ = Queue_getChar(queue);
	*uint32_t_buffer++ = Queue_getChar(queue);
	*uint32_t_buffer++ = Queue_getChar(queue);
	len -= 4;

	printf("Timestamp : %u\r\n", timstamp);

	current_char = Queue_getChar(queue);
	printf("End char 85? : %u\r\n", current_char);

	printf("-----------------\r\n\r\n");
}

static void transmit_queue_data_beacon(Queue* queue)
{

	uint8_t len = Queue_getChar(queue);
	uint8_t current_char;
	printf("data len: %u\r\n", len);

	current_char = Queue_getChar(queue);
	len--;
	printf("Type : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("checksum : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("Temp : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("Humid : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("Light : %u\r\n", current_char);

	current_char = Queue_getChar(queue);
	len--;
	printf("Mode : %u\r\n", current_char);

	float voltage;
	uint8_t* float_buffer = (uint8_t*) &voltage;
	*float_buffer++ = Queue_getChar(queue);
	*float_buffer++ = Queue_getChar(queue);
	*float_buffer++ = Queue_getChar(queue);
	*float_buffer++ = Queue_getChar(queue);
	len -= 4;

	printf("Voltage : %.2f\r\n", voltage);

	uint32_t timstamp;
	uint8_t* uint32_t_buffer = (uint8_t*) &timstamp;
	*uint32_t_buffer++ = Queue_getChar(queue);
	*uint32_t_buffer++ = Queue_getChar(queue);
	*uint32_t_buffer++ = Queue_getChar(queue);
	*uint32_t_buffer++ = Queue_getChar(queue);
	len -= 4;

	printf("Timestamp : %u\r\n", timstamp);

	current_char = Queue_getChar(queue);
	printf("End char 85?: %u\r\n", current_char);

	printf("-----------------\r\n\r\n");
}




//    do {
//        current_char = Queue_getChar(queue);
//        HAL_UART_Transmit(&huart2, &current_char, 1, HAL_MAX_DELAY);
//    } while (current_char != '\0');


//static void transmit_queue_data(Queue* queue)
//{
//    uint8_t current_char;
//    do {
//        current_char = Queue_getChar(queue);
//        HAL_UART_Transmit(&huart2, &current_char, 1, HAL_MAX_DELAY);
//    } while (current_char != '\0');
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    //echo
    HAL_UART_Transmit(&huart2, (uint8_t*) uart_buffer, 1, HAL_MAX_DELAY);

    Queue_enque(&recieve_queue, uart_buffer , 1);

    if (uart_buffer[0] == ENTER) {
    	osSemaphoreRelease(commands);
    }

    HAL_UART_Receive_IT(&huart2, uart_buffer, sizeof(uart_buffer));
}


void recieve_task(void* context)
{
	MessageHandler message_handler = (MessageHandler) context;

	uint8_t command[256];
	uint8_t command_index;
	uint8_t current_char;

	while(1)
	{

		osSemaphoreAcquire(commands, HAL_MAX_DELAY);

		command_index = 0;
		do {
		    current_char = Queue_getChar(&recieve_queue);
		    command[command_index] = current_char;
		    command_index++;
		} while(current_char != '\0');

		message_handler(command, command_index);
	}
}

int _write(int file, char* ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);

	return len;
}


