/**
 * @file usb_terminal.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source file for @ref usb_terminal.h.
 */

#include <elob/util/terminal.h>
#include <elob/util/usb_terminal.h>
#include <elob/util/error.h>
#include <elob/elob.h>

#include <errno.h>

// Internal PUT function for the FILE stream
int _usb_terminal_put(char c, FILE* f) {
	try {
		usb_terminal_printChar(c);
	} catch {
		return 1;
	}
}

// Internal GET function for the FILE stream
int _usb_terminal_get(FILE* f) {
	return usb_terminal_readChar();
}

void usb_terminal_init(
	unsigned long baudrate,
	UART_ParityMode_t parityMode,
	UART_StopBitMode_t stopbitMode,
	UART_ClockPolarityMode_t clockPolarityMode
) {
	// Initialize the UART interface
	uart_init(USB_UART_IF, baudrate, parityMode, stopbitMode, clockPolarityMode);
	
	// Initialize USB-UART FILE stream
	usb_terminal_f = fdevopen(_usb_terminal_put, _usb_terminal_get);
	stdout = usb_terminal_f;
	stdin = usb_terminal_f;
	stderr = usb_terminal_f;

	// Reset the terminal style
	terminal_setStyle(TERMINAL_STYLE_RESET);
}

void usb_terminal_printChar(char c) {
	uart_sendByte(USB_UART_IF, c);
}

void usb_terminal_print(char* s) {
	uart_sendString(USB_UART_IF, s);
}

void usb_terminal_println(char* s) {
	uart_sendString(USB_UART_IF, s);
	uart_sendString(USB_UART_IF, "\r\n");
}

char usb_terminal_readChar() {
	// Read a single char from the UART interface
	char data = uart_readByte(USB_UART_IF);

	// Echo the char to the terminal
	uart_sendByte(USB_UART_IF, data);
	return data;
}

void usb_terminal_readln(char* target, size_t maxLength) {	
	uint8_t currentChar = 0;
	int i = 0;
	while(currentChar != '\n' && currentChar != '\r') {
		// Read the current char
		currentChar = uart_readByte(USB_UART_IF);
		
		if(currentChar == 127) { // A DEL character (backspace) has been received
			if(i <= 0) continue; // Don't allow backspace before the start of the line
			i--;
			uart_sendByte(USB_UART_IF, currentChar);
		} else if(i >= maxLength - 1) {
			// The number of received chars exceeded the max length, throw an error
			throwMessage(ERR_BUFFER_OVERFLOW, "usb_terminal_readln: Maximum buffer length exceeded.");
		} else { // A normal character has been received
			target[i] = currentChar;
			i++;
			uart_sendByte(USB_UART_IF, currentChar);
		}
	}
	// Replace the CR/LF character with a null terminator
	target[i - 1] = 0;

	// Echo a CRLF to the terminal
	uart_sendString(USB_UART_IF, "\r\n");
}

bool usb_terminal_available() {
	return uart_available(USB_UART_IF);
}

unsigned long usb_terminal_ulPrompt(const char* prompt, unsigned long min, unsigned long max) {
	// Convert the min/max value to strings
	char minStr[21];
	char maxStr[21];
	
	sprintf(minStr, "%d", min);
	sprintf(maxStr, "%d", max);
	
	// Print the prompt and range indicator ('[min-max]')
	usb_terminal_print(prompt);
	usb_terminal_print(" [");
	usb_terminal_print(minStr);
	usb_terminal_print("-");
	usb_terminal_print(maxStr);
	usb_terminal_print("]: ");
	
	// Maximum length of an decimal unsigned 64-bit-integer string is 20 characters,
	// so the buffer needs to be 21 bytes long (accounting for a null terminator)
	char inputStr[21];
	usb_terminal_readln(inputStr, 21);
	
	// Try to convert the entered text to a number with strtoul
	char* endptr;
	unsigned long input = strtoul(inputStr, &endptr, 10);
	
	if(*endptr != 0) {  // If endptr doesn't point to the null terminator, the entered number was invalid.
		throwMessage(ERR_INVALID_STR, "usb_terminal_ulPrompt: Invalid unsigned long string.");
	}
	else if(
		input < min || 
		input > max
	) {
		throwMessage(ERR_OUT_OF_RANGE, "usb_terminal_ulPrompt: Out of range.");
	}
	
	return input;
}