/**
 * @file elob.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief ELO-Board specific variables and macros.
 */
#ifndef ELOB_H_
#define ELOB_H_

/**
 * @brief Clock frequency. On the ELO-Board, this is 16 MHz.
 *
 */
#define F_CPU 16000000UL

/**
 * @brief UART interface for serial-over-usb.
 *
 */
#define USB_UART_IF UART1

/**
 * @brief Initializes the most important functions of the ELO-Board, e.g. the DDR registers.
 *
 */
void elob_init();

#endif /* ELOB_H_ */