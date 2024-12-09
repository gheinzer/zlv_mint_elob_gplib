/**
 * @file usb_terminal.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Library for usage of the USB terminal functionality.
 */

#include <elob/drivers/uart.h>
#include <stdio.h>

#ifndef USB_TERMINAL_H_
#define USB_TERMINAL_H_

/**
 * @brief Initializes the UART interface used for the USB-over-Serial connection.
 * 
 * @param baudrate Baudrate used for the connection.
 * @param parityMode Parity mode for the connection.
 * @param stopbitMode Stop-Bit mode for the connection.
 * @param clockPolarityMode Clock polarity mode for the connection.
 */
void usb_terminal_init(
	unsigned long baudrate,
	UART_ParityMode_t parityMode,
	UART_StopBitMode_t stopbitMode,
	UART_ClockPolarityMode_t clockPolarityMode
);

/**
 * @brief USB terminal `FILE` object. This allows for `printf`-like functions to be used.
 * 
 * If this is the first `FILE` object which is initialized, it is also assigned to `stdin`, `stdout` and `stderr`.
 * 
 */
FILE* usb_terminal_stream;

/**
 * @brief Prints a single char to the terminal.
 * 
 * @param c The char to print to the terminal.
 */
void usb_terminal_printChar(char c);
/**
 * @brief Prints a string to the terminal.
 * 
 * @param s The string to print to the terminal.
 */
void usb_terminal_print(char* s);
/**
 * @brief Prints a string to the terminal with a trailing newline.
 * 
 * @param s The string to print to terminal.
 */
void usb_terminal_println(char* s);

/**
 * @brief Reads a single char from the terminal.
 * 
 * @return The char which was just read from the terminal.
 */
char usb_terminal_readChar();
/**
 * @brief Reads a stream of characters from the terminal, limited by a newline or the maximum length of the target buffer.
 * 
 * @param target Target buffer to write the received characters to.
 * @param maxLength The maximum length of the buffer.
 */
void usb_terminal_readln(char* target, size_t maxLength);

/**
 * @brief Displays a prompt allowing the user to enter an unsigned integer with the
 * given range limits.
 * 
 * @param prompt The prompt you want to display to the user. A range note is added automatically.
 * @param min Inclusive minimal range limit.
 * @param max Inclusive maximal range limit.
 * @return unsigned long 
 */
unsigned long usb_terminal_ulPrompt(const char* prompt, unsigned long min, unsigned long max);

/**
 * @brief Asserts if anything has been received from the terminal.
 * 
 * @return Returns `true` if something has been received from the terminal in the background, otherwise `false`.
 */
bool usb_terminal_available();

#endif /* USB_TERMINAL_H_ */