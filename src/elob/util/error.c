/**
 * @file ds1307.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source file for @ref error.h.
 */
#include <elob/util/error.h>
#include <elob/util/binary.h>
#include <elob/util/terminal.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

jmp_buf _error_h_globalJmpBuffer;

void error_init() {
	// Set the default jmp_buf to the global jmp_buf
	_error_h_currentJmpBuf = &_error_h_globalJmpBuffer;

	// Initialize the global jmp_buf
	int setjmpVal = setjmp(_error_h_globalJmpBuffer);
	if(setjmpVal) {
		_error_h_uncaughtErrorHandler();
	}
}

void _error_h_throw(unsigned int errorCode, const char* errorName, const char* errorMessage, int line, const char* file) {
	// Store the error name and message in the global variables
	_error_h_currentErrorName = errorName;
	_error_h_currentErrorMessage = errorMessage; // Null pointer as no error message was thrown
	_error_h_currentErrorLine = line;
	_error_h_currentErrorFile = file;
	
	// Jump to the error handler
	longjmp(_error_h_currentJmpBuf, errorCode);
}

void _error_h_uncaughtErrorHandler() {
	// Ensure no interrupts are received while being
	// in the error state
	cli();
	
	// Set the RGB LED to be red
	SETBIT(PORTB, 5);
	CLEARBIT(PORTB, 6);
	CLEARBIT(PORTB, 7);
	
	// Print the error information to stderr
	terminal_setStyle(TERMINAL_STYLE_RESET);
	fprintf(stderr, "\r\n");
	terminal_setColors(TERMINAL_CLR_BLACK, TERMINAL_CLR_RED);
	fprintf(stderr, " %s ", _error_h_currentErrorName);
	terminal_setStyle(TERMINAL_STYLE_RESET);
	terminal_setStyle(TERMINAL_STYLE_BOLD);
	fprintf(stderr, " %s\r\n", _error_h_currentErrorMessage);
	terminal_setStyle(TERMINAL_STYLE_RESET);
	
	// Inform the user that the error was not caught
	terminal_setStyle(TERMINAL_STYLE_DIM);
	fprintf(stderr, "Error thrown in file '%s' on line %d\r\n", _error_h_currentErrorFile, _error_h_currentErrorLine);
	fprintf(stderr, "The above error was not caught.\r\n");
	fprintf(stderr, "Reset the board to continue operation.\r\n");
	
	// Enter an infinite loop
	while(1);
}