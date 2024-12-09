/**
 * @file buffer.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source for @ref buffer.h.
 */

#include <elob/util/buffer.h>
#include <elob/util/error.h>

void buffer_put(Buffer_t* buffer, uint8_t data) {
	if(buffer_full(buffer)) {
		throwMessage(ERR_BUFFER_OVERFLOW, "buffer_put: Buffer too long.");
	}
	
	// Add the element to the end of the buffer
	buffer->ptr[buffer->size] = data;
	buffer->size++;
}

bool buffer_empty(Buffer_t* buffer) {
	return buffer->size == 0;
}

bool buffer_full(Buffer_t* buffer) {
	return buffer->size >= buffer->maxSize;
}

uint8_t buffer_get(Buffer_t* buffer) {
	if(buffer_empty(buffer)) {
		throwMessage(ERR_BUFFER_OVERFLOW, "buffer_get: Buffer empty.");
	}
	
	// Get the element from the beginning of the buffer
	buffer->size--;
	uint8_t data = buffer->ptr[0];
	
	// Shift the remaining bytes one to the left
	memmove(buffer->ptr, buffer->ptr + 1, buffer->size);
	
	return data;
}

void buffer_clear(Buffer_t* buffer) {
	buffer->size = 0;
}