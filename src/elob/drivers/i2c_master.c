/**
 * @file i2c_master.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source file for @ref i2c_master.h
 */
#include <elob/drivers/i2c_master.h>
#include <elob/util/error.h>
#include <elob/util/binary.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * @brief Waits for the TWINT (TWI Interrupt flag) bit in the TWCR register to be set.
 * 
 */
#define _TWINT_AWAIT while(!CHECKBIT(TWCR, TWINT));

/**
 * @brief Resets the TWINT bit in the TWCR register.
 * 
 */
#define _TWINT_RESET SETBIT(TWCR, TWINT);

/**
 * @brief Resets the TWINT bit and waits for it to be set again.
 * 
 */
#define _TWINT_RESET_AWAIT _TWINT_RESET; _TWINT_AWAIT;

void i2c_master_setBitrate(uint32_t bitrate) {
	const uint8_t prescalers[] = { 1, 4, 16, 64 };
		
	float twbrValue = UINT8_MAX + 1;
	uint8_t prescalerID;
		
	// Calculate the appropriate prescaler for the specified bitrate
	//  (prescaler as small as possible, but as big as needed)

	// Iterate until the calculated value fits into TWBR
	for(prescalerID = 0; twbrValue > UINT8_MAX; prescalerID++) {
		if(prescalerID >= sizeof(prescalers) / sizeof(*prescalers)) {
			// The biggest prescaler was already selected and yet, twbrValue did still not fit into 1 byte
			// Therefore, the specified bitrate is too low.
			throwMessage(ERR_BITRATE_TOO_LOW, "i2c_master_setBitrate: Bitrate too low.");
		}
		
		// Calculate the TWBR value for the specified bitrate and currently selected prescaler
		twbrValue = ((float)F_CPU / (float)bitrate - 16.0) / ((float)(pow(4, prescalers[prescalerID])) * 2.0);
	}
	
	// Write the calculated values to the registers
	TWBR = twbrValue;
	TWSR = (TWPS0 | TWPS1) & prescalerID;
}

void i2c_master_enable() {
	// Enable the TWI interface
	SETBIT(TWCR, TWEN);
	SETBIT(TWCR, TWEA);

	// Ensure the interrupt is disabled
	CLEARBIT(TWCR, TWIE);
}

void i2c_master_disable() {
	// Disable the TWI interface
	CLEARBIT(TWCR, TWEN);
}

i2c_masterStatus_t i2c_master_checkStatus() {
	i2c_masterStatus_t status = TWSR & 0xF8;
	if(status == I2C_BUS_ERROR) {
		throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_checkStatus: Bus error reported.");
	} else if(CHECKBIT(TWCR, TWWC)) {
		throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_checkStatus: Write collision detected.");
	}
	
	return status;
}

void i2c_master_sendByte(uint8_t data) {
	// Put the byte into the data register and
	// wait for it to be sent
	TWDR = data;
	_TWINT_RESET_AWAIT;

	i2c_masterStatus_t status = i2c_master_checkStatus();
	if(status != I2C_MASTER_DATA_SENT_ACK) {
		if(status == I2C_MASTER_DATA_SENT_NACK) { // A NACK has been received
			throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_sendByte: Data sent, NACK received.");
		} else { // An unexpected status code was indicated by the hardware TWI interface
			throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_sendByte: Data sent, status invalid.");
		}
	}
}

void i2c_master_startFrame(uint8_t slaveAddress, i2c_communicationDirection_t direction) {
	SETBIT(TWCR, TWSTA); // Generate a start condition
	
	// Wait for the start condition to be generated successfully
	_TWINT_RESET_AWAIT;
	i2c_masterStatus_t status = i2c_master_checkStatus();
	if(status != I2C_MASTER_START_CONDITION_TRANSMITTED && status != I2C_MASTER_REAPEATED_START_CONDITION_TRANSMITTED) {
		// An unexpected status code was indicated by the hardware TWI interface
		throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_startFrame: Failed to generate start condition.");
	}
	
	// Send the address and RW byte
	uint8_t addressRWByte = slaveAddress << 1;
	addressRWByte |= direction;
	TWDR = addressRWByte;
	
	CLEARBIT(TWCR, TWSTA); // Reset the TWSTA bit
	
	// Wait for the byte to be sent successfully
	_TWINT_RESET_AWAIT;
	status = i2c_master_checkStatus();
	if(status != I2C_MASTER_SLAR_ACK && status != I2C_MASTER_SLAW_ACK) {
		// An unexpected status code was indicated by the hardware TWI interface
		throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_startFrame: Failed to send SLA+R/W.");
	}
}

uint8_t i2c_master_readByte(i2c_ackType_t ackType) {
	// Set the specified acknowledgement type for the TWI
	WRITEBIT(TWCR, TWEA, ackType);
	
	// Start receiving the byte by clearing the TWINT bit
	// and wait for the transmission to be finished.
	_TWINT_RESET_AWAIT;
	uint8_t data = TWDR; // Initiate the transmission
	
	i2c_masterStatus_t status = i2c_master_checkStatus();
	if(status != I2C_MASTER_DATA_RECEIVED_ACK && status != I2C_MASTER_DATA_RECEIVED_NACK) {
		// An unexpected status code was indicated by the hardware TWI interface
		throwMessage(ERR_I2C_TRANSMISSION, "i2c_master_readByte: Failed to read message from I2C.");
	}
	
	return data;
}

void i2c_master_endFrame() {
	SETBIT(TWCR, TWSTO); // Generate a stop condition
}