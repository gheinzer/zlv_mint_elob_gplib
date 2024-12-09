/**
 * @file terminal.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Terminal emulator customization using ANSI escape sequences.
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#define _ANSI_ESCAPE_SEQUENCE 0x1B

/**
 * @brief Enumerator defining the available colors of terminal emulators.
 */
typedef enum {
	TERMINAL_CLR_BLACK = 0,
	TERMINAL_CLR_RED = 1,
	TERMINAL_CLR_GREEN = 2,
	TERMINAL_CLR_YELLOW = 3,
	TERMINAL_CLR_BLUE = 4,
	TERMINAL_CLR_MAGENTA = 5,
	TERMINAL_CLR_CYAN = 6,
	TERMINAL_CLR_WHITE = 7,
	TERMINAL_CLR_DEFAULT = 9
} USB_TerminalColor_t;

/**
 * @brief Enumerator defining the available terminal styles of terminal emulators.
 */
typedef enum {
	TERMINAL_STYLE_RESET = 0,
	TERMINAL_STYLE_BOLD = 1,
	TERMINAL_STYLE_DIM = 2,
	TERMINAL_STYLE_ITALIC = 3,
	TERMINAL_STYLE_UNDERLINE = 4,
	TERMINAL_STYLE_BLINKING = 5,
	TERMINAL_STYLE_REVERSE = 6,
	TERMINAL_STYLE_HIDDEN = 8,
	TERMINAL_STYLE_STRIKETHROUGH = 9
} USB_TerminalStyle_t;

/**
 * @brief Sets the foreground color of the terminal emulator.
 * 
 * @param color The color you want to set the terminal emulator to.
 */
void terminal_setForegroundColor(USB_TerminalColor_t color);
/**
 * @brief Sets the background color of the terminal emulator.
 * 
 * @param color The color you want to set the terminal emulator to.
 */
void terminal_setBackgroundColor(USB_TerminalColor_t color);
/**
 * @brief Sets the foreground and background color of the terminal emulator.
 * 
 * @param foreground The foreground color you want to set the terminal emulator to.
 * @param background The background color you want to set the terminal emulator to.
 */
void terminal_setColors(USB_TerminalColor_t foreground, USB_TerminalColor_t background);
/**
 * @brief Sets the style of the terminal emulator.
 * 
 * @param style The style you want to set the terminal emulator to.
 */
void terminal_setStyle(USB_TerminalStyle_t style);

#endif /* TERMINAL_H_ */