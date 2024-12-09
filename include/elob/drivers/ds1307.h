/**
 * @file ds1307.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Driver library for the DS1307 real time clock IC. Communication happens over I2C.
 */
#include <stdint.h>
#include <time.h>

#ifndef DS1307_H_
#define DS1307_H_

/**
 * @brief Enumerator defining the internal registers of the DS1307.
 * The enumeration values correspond to the register addresses.
 * 
 */
typedef enum {
	DS1307_SECOND_REGISTER = 0x00,
	DS1307_MINUTE_REGISTER = 0x01,
	DS1307_HOUR_REGISTER = 0x02,
	DS1307_DOW_REGISTER = 0x03,
	DS1307_DATE_REGISTER = 0x04,
	DS1307_MONTH_REGISTER = 0x05,
	DS1307_YEAR_REGISTER = 0x06,
	DS1307_CTRL_REGISTER = 0x07
} DS1307Register_t;

/**
 * @brief Initializes all resources required for communicating with the DS1307.
 * 
 */
void DS1307_init();

/**
 * @brief Reads a single register from the DS1307.
 * 
 * @param reg The address of the register to read.
 * @return The data read from the register.
 */
uint8_t DS1307_getRegister(DS1307Register_t reg);
/**
 * @brief Sets a single register from the DS1307.
 * 
 * @param reg 
 * @param data 
 */
void DS1307_setRegister(DS1307Register_t reg, uint8_t data);
/**
 * @brief Reads all registers from the DS1307 to the specified array.
 * 
 * @param registerTable An array of 8 bytes to which the register data will be read.
 * The item index corresponds to the register address.
 */
void DS1307_getAllRegisters(uint8_t* registerTable);
/**
 * @brief Sets all registers of the DS1307 to the values of the specified array.
 * 
 * @param registerTable An array of 8 bytes to which the register values will be set.
 * The item index corresponds to the register address.
 */
void DS1307_setAllRegisters(uint8_t* registerTable);

/**
 * @brief Gets a timestamp from the DS1307.
 * 
 * @return A timestamp as specified in the documentation of `time_t`.
 */
time_t DS1307_getDatetime();
/**
 * @brief Sets the time of the DS1307 to the specified timestamp.
 * 
 * @param datetime A timestamp as specified in the documentation of `time_t`.
 */
void DS1307_setTime(time_t datetime);

#endif /* DS1307_H_ */