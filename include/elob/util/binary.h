/**
 * @file binary.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Library defining some macros for binary data processing.
 */

#ifndef BINARY_H_
#define BINARY_H_

/**
 * @brief Sets the bit at the position @p bit in the @p variable.
 */
#define SETBIT(variable, bit) ((variable) |= (1 << (bit)))
/**
 * @brief Clears the bit at the position @p bit in the @p variable.
 */
#define CLEARBIT(variable, bit) ((variable) &= ~(1 << (bit)))
/**
 * @brief Toggles the bit at the position @p bit in the @p variable.
 */
#define INVERTBIT(variable, bit) ((variable) ^= (1 << (bit)))
/**
 * @brief Reads the value of the bit at the position @p bit in the @p variable.
 */
#define CHECKBIT(variable, bit) ((variable) & (1 << (bit)))
/**
 * @brief Sets the bit at the position @p bit in the @p variable to the value @p val.
 */
#define WRITEBIT(variable, bit, val) (val ? SETBIT(variable, bit) : CLEARBIT(variable, bit))

/**
 * @brief Converts a 2-digit BCD number to a normal C integer.
 * This requires the lower digit to be located in the lower 4 bits,
 * and the higher digits to be located in the higher 4 bits.
 */
#define FROM_BCD(n) ((n & 0x0F) + (((n & 0xF0) >> 4) * 10))
/**
 * @brief Converts a 2-digit integer to a BCD number.
 * This returns a byte where the lower digits are located in the lower 4 bits,
 * and the higher digits are located in the higher 4 bits.
 */
#define TO_BCD(n) ((n) % 10 | ((int)((n) / 10)) << 4)

#endif /* BINARY_H_ */