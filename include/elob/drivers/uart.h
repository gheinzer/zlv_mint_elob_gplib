/**
 * @file uart.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief UART driver library.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../../../config.h"

#ifndef UART_H_
#define UART_H_

// Check if UART_BUFFER_SIZE is defined
#ifndef UART_BUFFER_SIZE
#error Please define UART_BUFFER_SIZE in your configuration.
#endif

/**
 * @brief Enumeration defining the parity modes of the UART interface.
 * 
 */
typedef enum {
	/**
	 * @brief No parity mode.
	 * 
	 */
	UART_PARITY_DISABLED = 0,
	/**
	 * @brief Even parity mode.
	 * 
	 */
	UART_PARITY_EVEN = 2,
	/**
	 * @brief Odd parity mode.
	 * 
	 */
	UART_PARITY_ODD = 3,
} UART_ParityMode_t;

/**
 * @brief Enumerator defining the stop bit modes of the UART interface.
 * 
 */
typedef enum {
	/**
	 * @brief 1-stop-bit mode.
	 * 
	 */
	UART_STOP_1BIT = 0,
	/**
	 * @brief 2-stop-bit-mode.
	 * 
	 */
	UART_STOP_2BIT = 1,
} UART_StopBitMode_t;

/**
 * @brief Enumerator defining the clock polarity modes of the UART interface.
 * 
 */
typedef enum {
	/**
	 * @brief Sample on falling edge (i.e. set-up on rising edge) mode.
	 * 
	 */
	UART_CLOCK_POLARITY_SAMPLE_ON_FALLING = 0,
	/**
	 * @brief Sample on rising edge (i.e. set-up on falling edge) mode.
	 * 
	 */
	UART_CLOCK_POLARITY_SAMPLE_ON_RISING = 1
} UART_ClockPolarityMode_t;

/**
 * @brief Enumerator defining the available UART interfaces.
 * 
 */
typedef enum {
	/**
	 * @brief UART 0 interface.
	 * 
	 */
	UART0 = 0,
	/**
	 * @brief UART 1 interface.
	 * 
	 */
	UART1 = 1,
	/**
	 * @brief UART 2 interface.
	 * 
	 */
	UART2 = 2,
	/**
	 * @brief UART 3 interface.
	 * 
	 */
	UART3 = 3
} UART_Interface_t;

/**
 * @brief Initializes the given UART interface with the given parameters.
 * 
 * @param uartInterface The @ref UART_Interface_t to initialize.
 * @param baudrate The baud rate to configure.
 * @param parityMode The parity mode to configure.
 * @param stopbitMode The stop bit mode to configure.
 * @param clockPolarityMode The clock polarity mode to configure.
 */
void uart_init(
	UART_Interface_t uartInterface,
	unsigned long baudrate,
	UART_ParityMode_t parityMode,
	UART_StopBitMode_t stopbitMode,
	UART_ClockPolarityMode_t clockPolarityMode
);
/**
 * @brief Configures the baud rate of the given UART interface.
 * 
 * @param uartInterface The @ref UART_Interface_t to set the baud rate of.
 * @param baudrate The baud rate to configure.
 */
void uart_setBaudrate(UART_Interface_t uartInterface, unsigned long baudrate);

/**
 * @brief Checks if there is any data available on the given UART interface.
 * 
 * @param uartInterface The UART interface to check.
 * @return `true` There is data in the UART data buffer.
 * @return `false` There is no data available in the UART data buffer.
 */
bool uart_available(UART_Interface_t uartInterface);

/**
 * @brief Sends a single byte to the given UART interface.
 * 
 * @param uartInterface The UART interface to send the data to.
 * @param data The data byte to send to the interface.
 */
void uart_sendByte(UART_Interface_t uartInterface, uint8_t data);
/**
 * @brief Reads a single byte from the given UART interface. If no data
 * is available from the buffer, this blocks until something is received.
 * 
 * @param uartInterface The UART interface to read the data from.
 * @return The data byte received from the interface.
 */
uint8_t uart_readByte(UART_Interface_t uartInterface);

/**
 * @brief Sends a string of characters to the specified UART interface.
 * 
 * @param uartInterface The UART interface to send the string to.
 * @param s The string to send to the interface.
 */
void uart_sendString(UART_Interface_t uartInterface, uint8_t* s);

#endif /* UART_H_ */