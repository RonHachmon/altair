/*
 * uart_task.c
 *
 *  Created on: Apr 5, 2025
 *      Author: 97254
 */

#include "tasks/uart_task.h"
#include "usart.h"

static uint8_t command_buffer[256];
static uint8_t uart_buffer[1];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void uart_task(void* context)
{
	HAL_UART_Receive_IT(&huart2, uart_buffer, sizeof(uart_buffer));
}


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    static int write_index = 0;
//    uartSend((char*) buffer_uart,1);
//
//
//    if (buffer_uart[0] == BACKSPACE) {
//        if (write_index > 0) {
//            write_index--;
//        }
//    }
//    else if (buffer_uart[0] == ENTER) {
//        user_command[write_index] = '\0';
//        exec_command((char*) user_command);
//        write_index = 0;
//    }
//    else {
//        user_command[write_index] = buffer_uart[0];
//        write_index++;
//    }
//
//
//    HAL_UART_Receive_IT(&huart2, uart_buffer, sizeof(uart_buffer));
//}
