/**
 * @file i2c_master.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Driver for I2C (TWI) in master mode.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <elob/elob.h>

#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

/**
 * @brief Enumeration defining the I2C status codes supplied by the hardware I2C interface.
 * 
 */
typedef enum {
	// Status codes for both modes
	I2C_MASTER_START_CONDITION_TRANSMITTED = 0x08,
	I2C_MASTER_REAPEATED_START_CONDITION_TRANSMITTED = 0x10,
	
	// Status codes for Master Transmitter Mode
	I2C_MASTER_SLAW_ACK = 0x18,
	I2C_MASTER_SLAW_NACK = 0x20,
	I2C_MASTER_DATA_SENT_ACK = 0x28,
	I2C_MASTER_DATA_SENT_NACK = 0x30,
	I2C_MASTER_ARBITRATION_LOST = 0x38,
	
	// Status codes for Master Receiver Mode
	I2C_MASTER_SLAR_ACK = 0x40,
	I2C_MASTER_SLAR_NACK = 0x48,
	I2C_MASTER_DATA_RECEIVED_ACK = 0x50,
	I2C_MASTER_DATA_RECEIVED_NACK = 0x58,
	
	// Generic status codes
	I2C_MASTER_NO_STATUS = 0xF8,
	I2C_BUS_ERROR = 0x00
} i2c_masterStatus_t;

/**
 * @brief Enumeration defining the two I2C communication directions.
 * 
 */
typedef enum {
	// This corresponds to the I2C R/W bit
	I2C_WRITE = 0,
	I2C_READ = 1
} i2c_communicationDirection_t;

/**
 * @brief Enumeration the acknowlegement types available for the I2C interface.
 * 
 */
typedef enum {
	I2C_NACK = 0,
	I2C_ACK = 1,
} i2c_ackType_t;

/**
 * @brief Sets the I2C interface bitrate.
 * 
 * @param bitrate The bitrate to set the interface to.
 * 
 * @exception ERR_BITRATE_TOO_LOW Raised when the passed bitrate is not achievable with the available prescalers.
 */
void i2c_master_setBitrate(uint32_t bitrate);

/**
 * @brief Enables the I2C master interface.
 * 
 */
void i2c_master_enable();

/**
 * @brief Disables the I2C master interface.
 * 
 */
void i2c_master_disable();

/**
 * @brief Checks the current status code indicated by the hardware TWI interface.
 * If there is a transmission or bus error, this throws an error.
 * 
 * @return The status code which is currently indicated by the hardware I2C interface.
 * 
 * @exception ERR_I2C_TRANSMISSION The last I2C transmission failed.
 */
i2c_masterStatus_t i2c_master_checkStatus();

/**
 * @brief Starts an I2C frame with the given slave address and communication direction.
 * 
 * Under the hood, this sends a start condition and then sends the slave address and a
 * read/write bit.
 * 
 * @param slaveAddress The slave address you want to start the frame with.
 * @param direction The direction the frame is going to go (transmit/receive).
 * 
 * @exception ERR_I2C_TRANSMISSION A I2C transmission error occurred.
 */
void i2c_master_startFrame(uint8_t slaveAddress, i2c_communicationDirection_t direction);

/**
 * @brief Sends a single byte to the I2C interface. This requires a frame to be started
 * before calling this function.
 * 
 * @param data The data byte you want to send to the interface.
 * 
 * @exception ERR_I2C_TRANSMISSION A I2C transmission error occurred.
 */
void i2c_master_sendByte(uint8_t data);

/**
 * @brief Reads a single byte to the I2C interface. This requires a frame to be started
 * before calling this function.
 * 
 * @param ackType The type of acknowledgement you want to send to the I2C slave.
 * @return The data byte received from the interface.
 */
uint8_t i2c_master_readByte(i2c_ackType_t ackType);

/**
 * @brief Ends a I2C frame. This sends a stop condition.
 * 
 */
void i2c_master_endFrame();

#endif /* I2C_MASTER_H_ */