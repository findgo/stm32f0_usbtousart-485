




#include "util_ringbuf.h"

/**
  * @brief	获得有多少个可用块 在队列环中中
  * @param	None
  * @note	    
  * @retval None
  */
uint16_t ringbuf_count(ring_buf_t const *b)
{
	if (b) {
		return b->chunks_count;
	}

	return 0;
}
/**
  * @brief	Returns the empty/full status of the ring buffer
  * @param	None
  * @note		
  * @retval 	true if the ring buffer is full, false if it is not.
  */
bool ringbuf_full(ring_buf_t const *b)
{
	return (b ? (b->chunks_count == b->chunks_size) : true);
}
/**
  * @brief	Returns the empty/full status of the ring buffer
  * @param	None
  * @note		
  * @retval 	true if the ring buffer is empty, false if it is not.
  */
bool ringbuf_empty(ring_buf_t const *b)
{
	return (b ? (b->chunks_count == 0) : true);
}

/**
  * @brief	Copy the data fromthe list, and removes it
  * @param	None
  * @note		
  * @retval 	true if data was copied, false if list is empty
  */
bool ringbuf_pop(ring_buf_t * b, uint8_t * data_element)
{
	bool status = false;        /* return value */
	volatile uint8_t *ring_data = NULL; /* used to help point ring data */
	unsigned i; /* loop counter */

	if (!ringbuf_empty(b)) {
		ring_data = b->buffer;
		ring_data += b->chunks_head * b->element_size;
		if (data_element) {
			for (i = 0; i < b->element_size; i++) {
				data_element[i] = ring_data[i];
			}
		}
		b->chunks_count--;
		if(++b->chunks_head >= b->chunks_size){
			b->chunks_head = 0;
		}
		status = true;
	}

	return status;
}
/**
  * @brief	Adds an element of data to the ring buffer
  * @param	None
  * @note		
  * @retval 	true on succesful add, false if not added
  */
bool ringbuf_put(ring_buf_t * b,uint8_t * data_element)
{
    bool status = false;
    volatile uint8_t *ring_data = NULL; /* used to help point ring data */
    uint16_t i; /* loop counter */

	if (b && data_element) {
		if (!ringbuf_full(b)) {
			
			ring_data = b->buffer;
			ring_data += b->chunks_tail * b->element_size;
			for (i = 0; i < b->element_size; i++) {
				ring_data[i] = data_element[i];
			}
			b->chunks_count++;
			if(++b->chunks_tail >= b->chunks_size){
				b->chunks_tail = 0;
			}
			status = true;
		}
	}

    return status;
}

/**
  * @brief	Adds an element of data to the front of the ring buffer
  * @param	None
  * @note		
  * @retval 	true on succesful add, false if not added
  */
bool ringbuf_put_front(ring_buf_t * b, uint8_t *data_element)
{
	bool status = false;        /* return value */
 	uint8_t *ring_data = NULL; /* used to help point ring data */
	uint16_t i = 0;     /* loop counter */

	if (b && data_element) {
		/* limit the amount of elements that we accept */
		if (!ringbuf_full(b)) {
			if(b->chunks_head == 0){
				b->chunks_head = b->chunks_size - 1;
			}else{
				b->chunks_head--;
			}
			ring_data = b->buffer;
			ring_data += (b->chunks_head * b->element_size);

			for (i = 0; i < b->element_size; i++) {
				ring_data[i] = data_element[i];
			}
			b->chunks_count++;
			status = true;
		}
	}

    return status;
}
/**
  * @brief	Looks at the data from the head of the list without removing it
  * @param	None
  * @note		
  * @retval 	the data element point
  */
uint8_t *ringbuf_peek(ring_buf_t const *b)
{
	uint8_t *data_element = NULL;      /* return value */

	if (!ringbuf_empty(b)) {
        data_element = b->buffer;
        data_element += b->chunks_head * b->element_size;
    }

	return data_element;
}
/**
  * @brief	Reserves and gets the next data portion of the buffer.
  * @param	None
  * @note		
  * @retval 	pointer to the data, or NULL if the list is full
  */
uint8_t *ringbuf_alloc(ring_buf_t * b)
{
	uint8_t *ring_data = NULL; /* used to help point ring data */

	if (b) {
		if (!ringbuf_full(b)) {
			ring_data = b->buffer;
			ring_data += b->chunks_tail * b->element_size;
			 b->chunks_count++;
			if(++b->chunks_tail >= b->chunks_size){
				b->chunks_tail = 0;
			}
		}
	}

	return ring_data;
}
/**
  * @brief  初始化ringbuf
  * @param	None
  * @note	   chunks_size must be a power of two
  * @retval None
  */
void ringbuf_init(ring_buf_t * b,    			/* ring buffer structure */
					uint8_t *buffer,  			/* data block or array of data */
					uint16_t element_size,      /* size of one element in the data block */
					uint16_t chunks_size)		/* number of elements in the data block */
{      
	if(b){
		b->buffer = buffer;
		b->element_size = element_size;
		b->chunks_size  = chunks_size;
		b->chunks_head  = 0;
		b->chunks_tail  = 0;
		b->chunks_count = 0;
	}
}

