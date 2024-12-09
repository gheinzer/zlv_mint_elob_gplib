/**
 * @file uart.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source file for @ref uart.h.
 */
#include <elob/drivers/uart.h>
#include <elob/util/error.h>
#include <elob/util/binary.h>
#include <elob/util/buffer.h>
#include "../../../../config.h"
#include <elob/elob.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * @brief Macro for accessing the UART register with the given id.
 * This requires for `uartInterface` to be declared in the parent scope.
 * 
 */
#define _UART_REGISTER(registerID) *(_uartRegisters[(uartInterface)][(registerID)])

/**
 * @brief Enumerator defining the different registers stored in @ref _uartRegisters.
 * 
 */
typedef enum {
	_DATA_REGISTER = 0,
	_CTRL_REGISTER_A,
	_CTRL_REGISTER_B,
	_CTRL_REGISTER_C,
	_BAUD_RATE_REGISTER_L,
	_BAUD_RATE_REGISTER_H
} _UART_RegisterID_t;

/**
 * @brief Array containing the pointers to the interface-specific UART registers.
 * 
 */
uint8_t* _uartRegisters[][6] = {
	{ &UDR0, &UCSR0A, &UCSR0B, &UCSR0C, &UBRR0L, &UBRR0H },
	{ &UDR1, &UCSR1A, &UCSR1B, &UCSR1C, &UBRR1L, &UBRR1H },
	{ &UDR2, &UCSR2A, &UCSR2B, &UCSR2C, &UBRR2L, &UBRR2H },
	{ &UDR3, &UCSR3A, &UCSR3B, &UCSR3C, &UBRR3L, &UBRR3H },
};

// Allocation of raw arrays used by @ref _uartBuffers
volatile uint8_t _rawUARTBuffers[3][UART_BUFFER_SIZE];

/**
 * @brief List of interface-specific UART receive buffers.
 * 
 */
volatile Buffer_t _uartBuffers[] = {
	{ // FIFO buffer for UART0
		.size = 0,
		.maxSize = UART_BUFFER_SIZE,
		.ptr = _rawUARTBuffers[0],
		.type = BUFFER_TYPE_FIFO
	},
	{ // FIFO buffer for UART1
		.size = 0,
		.maxSize = UART_BUFFER_SIZE,
		.ptr = _rawUARTBuffers[1],
		.type = BUFFER_TYPE_FIFO
	},
	{ // FIFO buffer for UART2
		.size = 0,
		.maxSize = UART_BUFFER_SIZE,
		.ptr = _rawUARTBuffers[2],
		.type = BUFFER_TYPE_FIFO
	},
	{ // FIFO buffer for UART3
		.size = 0,
		.maxSize = UART_BUFFER_SIZE,
		.ptr = _rawUARTBuffers[3],
		.type = BUFFER_TYPE_FIFO
	}
};

void uart_init(
	UART_Interface_t uartInterface, 
	unsigned long baudrate,
	UART_ParityMode_t parityMode,
	UART_StopBitMode_t stopbitMode,
	UART_ClockPolarityMode_t clockPolarityMode
) {
	uart_setBaudrate(uartInterface, baudrate); // Set the baudrate
	
	// Configure interrupts
	SETBIT(_UART_REGISTER(_CTRL_REGISTER_B), RXCIE0);
	CLEARBIT(_UART_REGISTER(_CTRL_REGISTER_B), TXCIE0);
	CLEARBIT(_UART_REGISTER(_CTRL_REGISTER_B), UDRIE0);
	sei();
	
	// Enable the receiver and transmitter
	SETBIT(_UART_REGISTER(_CTRL_REGISTER_B), RXEN0);
	SETBIT(_UART_REGISTER(_CTRL_REGISTER_B), TXEN0);
	
	// Configure the character size
	SETBIT(_UART_REGISTER(_CTRL_REGISTER_C), UCSZ00);
	SETBIT(_UART_REGISTER(_CTRL_REGISTER_C), UCSZ01);
	CLEARBIT(_UART_REGISTER(_CTRL_REGISTER_B), UCSZ02);
	
	// Select the UART mode
	CLEARBIT(_UART_REGISTER(_CTRL_REGISTER_C), UMSEL00);
	CLEARBIT(_UART_REGISTER(_CTRL_REGISTER_C), UMSEL01);
	
	// Configure the stop bit mode
	WRITEBIT(_UART_REGISTER(_CTRL_REGISTER_C), USBS0, stopbitMode);
	
	// Configure the clock polarity
	WRITEBIT(_UART_REGISTER(_CTRL_REGISTER_C), UCPOL0, clockPolarityMode);
	
	// Configure the parity setings
	WRITEBIT(_UART_REGISTER(_CTRL_REGISTER_C), UPM00, CHECKBIT(parityMode, 0));
	WRITEBIT(_UART_REGISTER(_CTRL_REGISTER_C), UPM01, CHECKBIT(parityMode, 1));
}

void uart_setBaudrate(UART_Interface_t uartInterface, unsigned long baudrate) {
	// Calculate the UBRR value for the given baud rate both for double speed and normal mode
	float ubrrValue2X = (float)F_CPU / (8.0 * (float)baudrate) - 1;
	float ubrrValue1X = (float)F_CPU / (16.0 * (float)baudrate) - 1;
	
	if(ubrrValue1X > 2047) {
		throwMessage(ERR_BITRATE_TOO_LOW, "uart_setBaudrate: The specified baudrate is too low.");
	}
	
	// Calculate the differences between the set baudrate
	// and the resulting baudrate
	float diff2X = abs((signed long)((float)F_CPU / (8.0 * (float)(round(ubrrValue2X) + 1)) - baudrate));
	float diff1X = abs((signed long)((float)F_CPU / (16.0 * (float)(round(ubrrValue1X) + 1)) - baudrate));
	
	// Compare the differences and set the control registers accordingly
	bool useDoubleSpeedMode = (diff2X < diff1X) && (ubrrValue2X < 2047);
	uint16_t ubrrValue = round(useDoubleSpeedMode ? ubrrValue2X : ubrrValue1X);
	
	WRITEBIT(_UART_REGISTER(_CTRL_REGISTER_A), U2X0, useDoubleSpeedMode);
	_UART_REGISTER(_BAUD_RATE_REGISTER_L) = ubrrValue & 0xFF;
	_UART_REGISTER(_BAUD_RATE_REGISTER_H) = ubrrValue >> 8;
}

bool uart_available(UART_Interface_t uartInterface) {
	return !buffer_empty(&_uartBuffers[uartInterface]);
}

void uart_sendByte(UART_Interface_t uartInterface, uint8_t data) {
	// Wait for the data register to be empty
	while(!CHECKBIT(_UART_REGISTER(_CTRL_REGISTER_A), UDRE0));
	_UART_REGISTER(_DATA_REGISTER) = data;
}

void uart_sendString(UART_Interface_t uartInterface, uint8_t* s) {
	for(int i = 0; s[i] != 0; i++) {
		// Iterate through the characters of the string and
		// send them one by one
		uart_sendByte(uartInterface, s[i]);
	}
}

uint8_t uart_readByte(UART_Interface_t uartInterface) {
	while(!uart_available(uartInterface));
	uint8_t data = buffer_get(&_uartBuffers[uartInterface]);
	return data;
}

// Interrupt service routines which store the received data
// in the interface-specific receive buffers.
ISR(USART0_RX_vect) {
	uint8_t rxData = UDR0;
	buffer_put(&_uartBuffers[0], rxData);
}

ISR(USART1_RX_vect) {
	uint8_t rxData = UDR1;
	buffer_put(&_uartBuffers[1], rxData);
}

ISR(USART2_RX_vect) {
	uint8_t rxData = UDR2;
	buffer_put(&_uartBuffers[2], rxData);
}

ISR(USART3_RX_vect) {
	uint8_t rxData = UDR3;
	buffer_put(&_uartBuffers[3], rxData);
}