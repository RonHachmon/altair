/*
 * uart_task.h
 *
 *  Created on: Apr 5, 2025
 *      Author: 97254
 */

#ifndef INC_TASKS_UART_TASK_H_
#define INC_TASKS_UART_TASK_H_

#include "main.h"
#include "utils/send_queue.h"

typedef void (*MessageHandler)(uint8_t const *message, uint8_t len);

typedef struct TransmitContext {
    Queue high_priority;
    Queue medium_priority;
    Queue low_priority;
} TransmitContext;

void init_uart();
void transmit_task(void* context);

void recieve_task(void* context);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_TASKS_UART_TASK_H_ */
