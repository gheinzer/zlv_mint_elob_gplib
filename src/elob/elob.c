/**
 * @file elob.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source file for @ref elob.h
 */
#include <elob/elob.h>
#include <elob/util/binary.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void elob_init()
{
	DDRA = 0xFF; // LEDs
	DDRC = 0x00; // Switches
	DDRJ = 0x00; // Buttons

	// RGB-LED
	SETBIT(DDRB, 5);
	SETBIT(DDRB, 6);
	SETBIT(DDRB, 7);

	// Enable interrupts globally
	sei();
}