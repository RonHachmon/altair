/*
 * send_queue.h
 *
 *  Created on: Apr 6, 2025
 *      Author: 97254
 */

#ifndef INC_UTILS_SEND_QUEUE_H_
#define INC_UTILS_SEND_QUEUE_H_

#include <stdint.h>

#define QUEUE_SIZE 256

typedef struct Queue {
	uint8_t m_que[QUEUE_SIZE];
	uint16_t m_head;
	uint16_t m_tail;
	uint16_t m_nItems;
} Queue;

Queue* Queue_create();

uint8_t Queue_enque(Queue *queue, uint8_t *data, uint16_t size);

uint8_t Queue_getChar(Queue *queue);

uint16_t Queue_size(Queue *queue);



#endif /* INC_UTILS_SEND_QUEUE_H_ */
