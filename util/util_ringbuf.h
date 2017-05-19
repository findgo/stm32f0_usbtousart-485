/**
  ******************************************************************************
  * @file   util_queue.h
  * @author  
  * @version 
  * @date    
  * @brief  	块数据队列
  ******************************************************************************
  * @attention 	20151110     v1.1   	jgb		重构		已测试 20151119
  ******************************************************************************
  */

#ifndef __UTIL_RINGBUF_H_
#define __UTIL_RINGBUF_H_

#include "app_cfg.h"

typedef struct  {
	uint8_t *buffer;   		/* 总块指针 */
	uint16_t element_size;     	 		/* 每一块的大小*/
	uint16_t chunks_size;     			/* 块总个数 */
	volatile uint16_t chunks_count;		/* 块计数器*/
	volatile uint16_t chunks_head;      /* 块头 */
	volatile uint16_t chunks_tail;      /* 块尾*/
}ring_buf_t;

void ringbuf_init(ring_buf_t * b,    			/* ring buffer structure */
					uint8_t *buffer,  	/* data block or array of data */
					uint16_t element_size,      /* size of one element in the data block */
					uint16_t chunks_size);		/* number of elements in the data block */
uint16_t ringbuf_count(ring_buf_t const *b);
bool ringbuf_full(ring_buf_t const *b);
bool ringbuf_empty(ring_buf_t const *b);
bool ringbuf_pop(ring_buf_t * b, uint8_t * data_element);
bool ringbuf_put(ring_buf_t * b,uint8_t * data_element);
bool ringbuf_put_front(ring_buf_t * b, uint8_t *data_element);
uint8_t *ringbuf_peek(ring_buf_t const *b);
uint8_t *ringbuf_alloc(ring_buf_t * b);

/************************************************************************************
here show how to use it:

	ring_buf_t ringq;
	uint32_t buf[20];

first init :
	ringbuf_init(&ringq, (uint8_t *)buf, sizeof(uint32_t), 20);

then you can use it
************************************************************************************/


#endif

