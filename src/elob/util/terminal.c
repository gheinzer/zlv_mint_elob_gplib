#include <elob/util/usb_terminal.h>
#include <elob/util/terminal.h>
#include <elob/util/error.h>
#include <elob/elob.h>

void terminal_setForegroundColor(USB_TerminalColor_t color) {
	printf("%c", _ANSI_ESCAPE_SEQUENCE);
	printf("[3");
	printf("%c", color + '0');
	printf("m");
}

void terminal_setBackgroundColor(USB_TerminalColor_t color) {
	printf("%c", _ANSI_ESCAPE_SEQUENCE);
	printf("[4");
	printf("%c", color + '0');
	printf("m");
}

void terminal_setColors(USB_TerminalColor_t foreground, USB_TerminalColor_t background) {
	terminal_setForegroundColor(foreground);
	terminal_setBackgroundColor(background);
}

void terminal_setStyle(USB_TerminalStyle_t style) {
	printf("%c", _ANSI_ESCAPE_SEQUENCE);
	printf("[");
	printf("%c", style + '0');
	printf("m");
}
