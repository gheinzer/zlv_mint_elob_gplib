/**
 * @file ds1307.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source file for @ref ds1307.h.
 */
#include <avr/io.h>
#include <time.h>
#include <elob/drivers/ds1307.h>
#include <elob/drivers/i2c_master.h>
#include <elob/util/binary.h>
#include <elob/util/error.h>

/**
 * @brief I2C address of the DS13007.
 * 
 */
#define _DS1307_I2C_ADDR 0b1101000

void DS1307_init() {
	// Initialize the required I2C interface
	i2c_master_setBitrate(10000); // The DS1307 requires 10 kHz as it's bitrate
	i2c_master_enable();
}

uint8_t DS1307_getRegister(DS1307Register_t reg) {
	i2c_master_startFrame(_DS1307_I2C_ADDR, I2C_WRITE); // Send a start condition and SLA+W
	i2c_master_sendByte(reg); // Set the internal memory pointer of the DS1307
	i2c_master_startFrame(_DS1307_I2C_ADDR, I2C_READ); // Send a repeated start condition and SLA+R
	uint8_t registerValue = i2c_master_readByte(I2C_NACK); // Read one byte from the DS1307
	i2c_master_endFrame(); // Send a stop condition
	
	return registerValue;
}

void DS1307_getAllRegisters(uint8_t* registerTable) {	
	i2c_master_startFrame(_DS1307_I2C_ADDR, I2C_WRITE); // Send a start condition and SLA+W
	i2c_master_sendByte(0); // Reset the internal memory pointer of the DS1307
	i2c_master_startFrame(_DS1307_I2C_ADDR, I2C_READ); // Send a repeated start condition and SLA+R
	
	// Read all 8 registers from the DS1307
	for(int registerAddress = 0; registerAddress < 8; registerAddress++) {
		registerTable[registerAddress] = i2c_master_readByte(
			registerAddress < 7 ? I2C_ACK : I2C_NACK // Send a NACK for the last read
		); // Read one byte from the DS1307
	}
	
	i2c_master_endFrame(); // Send a stop condition
}

void DS1307_setRegister(DS1307Register_t reg, uint8_t data) {
	i2c_master_startFrame(_DS1307_I2C_ADDR, I2C_WRITE); // Send a start condition and SLA+W
	i2c_master_sendByte(reg); // Set the internal memory pointer of the DS1307
	i2c_master_sendByte(data); // Set the register value of the DS1307
	i2c_master_endFrame(); // Send a stop condition
}

void DS1307_setAllRegisters(uint8_t* registerTable) {	
	i2c_master_startFrame(_DS1307_I2C_ADDR, I2C_WRITE); // Send a start condition and SLA+W
	i2c_master_sendByte(0); // Reset the internal memory pointer of the DS1307
	
	// Set all 8 registers of the DS1307
	for(int i = 0; i < 8; i++) {
		i2c_master_sendByte(registerTable[i]); // Read one byte from the DS1307
	}
	
	i2c_master_endFrame(); // Send a stop condition
}

time_t DS1307_getDatetime() {	
	// Read the values of all 8 registers.
	uint8_t registers[8];
	DS1307_getAllRegisters(registers);
	
	// Construct a 'struct tm' with the time from the DS1307
	struct tm currentTime;
	currentTime.tm_sec = FROM_BCD(registers[DS1307_SECOND_REGISTER] & 0x7F); // Parse the second register value
	currentTime.tm_min = FROM_BCD(registers[DS1307_MINUTE_REGISTER] & 0x7F); // Parse the minute register value
	// Parse the hour register value
	if(CHECKBIT(registers[DS1307_HOUR_REGISTER], 6)) { // Check if 12h is activated
		// The RTC is in 12h mode
		currentTime.tm_hour = FROM_BCD(registers[DS1307_HOUR_REGISTER] & 0x1F);
		
		if(CHECKBIT(registers[DS1307_HOUR_REGISTER], 5)) { // Check if it's PM
			// It's PM, add 12 hours
			currentTime.tm_hour += 12;
		}
	} else {
		// The RTC is in 24h mode
		currentTime.tm_hour = FROM_BCD(registers[DS1307_HOUR_REGISTER] & 0x3F);	
	}

	// Parse the date register value
	currentTime.tm_mday = FROM_BCD(registers[DS1307_DATE_REGISTER] & 0x3F);
	
	// Parse the month register value
	// The month range of 'struct tm' is from 0 - 11 instead of 1 - 12
	currentTime.tm_mon = FROM_BCD(registers[DS1307_MONTH_REGISTER] & 0x1F) - 1;
	
	// Parse the year register value
	// The year range of 'struct tm' begins at 1900, the one of the RTC at 2000
	currentTime.tm_year = FROM_BCD(registers[DS1307_YEAR_REGISTER] & 0xFF) + 100;

	// These are not needed, but have to be set to 0 for mktime to work right.
	currentTime.tm_isdst = 0;
	currentTime.tm_yday = 0;
	
	// Convert the 'struct tm' to timestamp as specified in the documentation of `time_t`.
	time_t time = mktime(&currentTime);
	return time;
}

void DS1307_setDatetime(time_t time) {
	// Get a 'struct tm' from the given timestamp
	struct tm datetime;
	gmtime_r(&time, &datetime);
	
	// Convert all the values to BCD for the DS1307
	uint8_t registers[8];
	registers[DS1307_SECOND_REGISTER] = TO_BCD(datetime.tm_sec) & 0x7F;
	
	registers[DS1307_MINUTE_REGISTER] = TO_BCD(datetime.tm_min) & 0x7F;
	
	// This also ensures the RTC is in 24-hour mode
	registers[DS1307_HOUR_REGISTER] = TO_BCD(datetime.tm_hour) & 0x3F;
	
	registers[DS1307_DATE_REGISTER] =  TO_BCD(datetime.tm_mday) & 0x3F;
	
	// The week day range of 'struct tm' begins at 0, hence the +1
	registers[DS1307_DOW_REGISTER] =  TO_BCD(datetime.tm_wday + 1) & 0x3F;
	
	// The month range of 'struct tm' begins at 0, hence the +1
	registers[DS1307_MONTH_REGISTER] =  TO_BCD(datetime.tm_mon + 1) & 0x3F;
	
	// The year range of 'struct tm' begins with the year 1900, hence the -100
	registers[DS1307_YEAR_REGISTER] =  TO_BCD(datetime.tm_year - 100);
	
	// Don't change the control register
	registers[DS1307_CTRL_REGISTER] = DS1307_getRegister(DS1307_CTRL_REGISTER);
	
	// Write the values to the RTC
	DS1307_setAllRegisters(registers);
}