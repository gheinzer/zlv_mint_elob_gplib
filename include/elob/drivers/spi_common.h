/**
 * @file spi_common.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Common declarations for the SPI libraries.
 */

#ifndef SPI_COMMON_H_
#define SPI_COMMON_H_

/**
 * @brief Defines the two main modes of the SPI communication.
 * 
 */
typedef enum {
    /**
     * @brief SPI master mode.
     */
    SPI_MASTER = 1,
    /**
     * @brief SPI slave mode.
     */
    SPI_SLAVE = 0
} SPI_Mode_t;

/**
 * @brief Enumerator containing the SPI data order options.
 * 
 */
typedef enum {
    /**
     * @brief MSB is sent first.
     */
    SPI_DATAORDER_MSB_FIRST = 0,
    /**
     * @brief LSB is sent first.
     */
    SPI_DATAORDER_LSB_FIRST = 1
} SPI_DataOrder_t;

/**
 * @brief Enumerator containing the SPI clock polarity options.
 * 
 */
typedef enum {
    /**
     * @brief The rising edge is the leading edge.
     */
    SPI_CPOL_RISING_LEADING = 0,
    /**
     * @brief The falling edge is the leading edge.
     */
    SPI_CPOL_FALLING_LEADING = 1
} SPI_ClockPolarity_t;

/**
 * @brief Enumerator containing the SPI clock phase options.
 * 
 */
typedef enum {
    /**
     * @brief The setup is done on the trailing edge, the sample is done on the leading edge.
     */
    SPI_CPHA_LEADING_SAMPLE = 0,
    /**
     * @brief The setup is done on the leading edge, the sample is done on the trailing edge.
     */
    SPI_CPHA_LEADING_SETUP = 1
} SPI_ClockPhase_t;

/**
 * @brief Enumerator containing the SPI clock phase options.
 * 
 */
typedef enum {
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{2}@f$
     */
    SPI_FOSC_2 = 0b100,
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{4}@f$
     */
    SPI_FOSC_4 = 0b000,
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{8}@f$
     */
    SPI_FOSC_8 = 0b101,
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{16}@f$
     */
    SPI_FOSC_16 = 0b001,
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{32}@f$
     */
    SPI_FOSC_32 = 0b110,
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{64}@f$
     */
    SPI_FOSC_64 = 0b010,
    /**
     * @brief @f$f_{SCL} = \frac{f_{OSC}}{128}@f$
     */
    SPI_FOSC_128 = 0b011
} SPI_ClockPrescaler_t;

/**
 * @brief Initializes the SPI interface in master mode.
 * 
 * @param spiMode The SPI mode (master/slave) to use for the initialization.
 * @param clockPrescaler Clock prescaler to use for the SPI communication.
 * @param dataOrder Data order to use for the SPI communication.
 * @param clockPolarity Clock polarity which is used for the SPI communication.
 * @param clockPhase CLock phase which is used for the SPI communication.
 */
void spi_init(
    SPI_Mode_t spiMode
    SPI_ClockPrescaler_t clockPrescaler,
    SPI_DataOrder_t dataOrder,
    SPI_ClockPolarity_t clockPolarity,
    SPI_ClockPhase_t clockPhase
);

#endif /* SPI_COMMON_H_ */