/**
 * @file buffer.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Simple FIFO buffer library implementing a 1-byte FIFO buffer.
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef BUFFER_H_
#define BUFFER_H_

/**
 * @brief Enumerator defining the available buffer types.
 */
typedef enum {
	/**
	 * @brief FIFO buffer (first-in first-out). The element that was inserted at first is returned first.
	 */
	BUFFER_TYPE_FIFO
} BufferType_t;

/**
 * @brief Struct type for a buffer.
 * 
 * Always use the functions from @ref buffer.h to modify this struct.
 * 
 */
typedef struct {
	/**
	 * @brief Stores the buffer type as a @ref BufferType_t enumerator.
	 * 
	 */
	BufferType_t type;
	/**
	 * @brief Current number of elements in the buffer.
	 * 
	 */
	size_t size;
	/**
	 * @brief Maximum number of elements in the buffer.
	 * 
	 */
	size_t maxSize;
	/**
	 * @brief Pointer to the buffer's internal array.
	 * 
	 */
	uint8_t* ptr;
} Buffer_t;

/**
 * @brief Inserts a new element at the end of the buffer.
 * 
 * @param buffer A pointer to the @ref Buffer_t struct you want to modify.
 * @param data The data byte you want to append to the buffer.
 */
void buffer_put(Buffer_t* buffer, uint8_t data);

/**
 * @brief Asserts if the buffer is empty.
 * 
 * @param buffer A pointer to the buffer you want to read from.
 * @return Returns `True` if the buffer is empty, `False` if the buffer has elements in it.
 */
bool buffer_empty(Buffer_t* buffer);

/**
 * @brief Asserts if the buffer is full.
 * 
 * @param buffer A pointer to the buffer you want to read from.
 * @return Returns `True` if the buffer is full, `False` if the buffer has available space.
 */
bool buffer_full(Buffer_t* buffer);

/**
 * @brief Gets a single element from the buffer. This also removes the element from the buffer.
 * 
 * @param buffer A pointer to the @ref Buffer_t struct you want to modify.
 * @return The element which was read from the buffer.
 */
uint8_t buffer_get(Buffer_t* buffer);

/**
 * @brief Empties the buffer and removes all elements.
 * 
 * @param buffer A pointer to the @ref Buffer_t struct you want to empty.
 */
void buffer_clear(Buffer_t* buffer);

#endif /* BUFFER_H_ */