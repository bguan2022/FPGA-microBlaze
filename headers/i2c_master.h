/***************************************************************************
 * File name   : i2c_master.h
 * Description : header file for using the i2c master core  
 *
 * Revision History:   June 17, 2008: Creation, Mohamed Sebbar
 * 
 ***************************************************************************/
 
#ifndef I2C_MASTER_
#define I2C_MASTER_

#include <xil_types.h>

/*** I2C-Master Core Registers ***/
#define I2C_CORE_REG_PRERLO 0x00   //Clock Prescale register lo-byte
#define I2C_CORE_REG_PRERHI 0x01   //Clock Prescale register hi-byte
#define I2C_CORE_REG_CTR    0x02   //Control register
#define I2C_CORE_REG_TXR    0x03   //Transmit register
#define I2C_CORE_REG_RXR    0x03   //Receive register
#define I2C_CORE_REG_CR     0x04   //Command register
#define I2C_CORE_REG_SR     0x04   //Status register

/*** I2C-Master Core Control register Bit Masks ***/
#define I2C_CORE_BIT_COREEN_MSK (1<<7) //I2C core enable bit
#define I2C_CORE_BIT_IEN_MSK    (1<<6) //IEN, I2C core interrupt enable bit
#define I2C_CORE_BIT_RESET		(1<<5) //Master reset. Active low.

/*** I2C-Master Core Command register Bit Masks ***/
#define I2C_CORE_BIT_STA_MSK    (1<<7) //STA, generate (repeated) start condition
#define I2C_CORE_BIT_STO_MSK    (1<<6) //STO, generate stop condition
#define I2C_CORE_BIT_RD_MSK     (1<<5) //RD, read from slave
#define I2C_CORE_BIT_WR_MSK     (1<<4) //WR, write to slave
#define I2C_CORE_BIT_ACK_MSK    (1<<3) //ACK, when a receiver, sent ACK (ACK = ‘0’) or NACK (ACK = ‘1’)
#define I2C_CORE_BIT_IACK_MSK   (1<<0) //IACK, Interrupt acknowledge

/*** I2C-Master Core Status register Bit Masks ***/
#define I2C_CORE_BIT_RXACK_MSK  (1<<7) //RxACK, Received acknowledge from slave
#define I2C_CORE_BIT_BUSY_MSK   (1<<6) //Busy, I2C bus busy
#define I2C_CORE_BIT_AL_MSK     (1<<5) //AL, Arbitration lost
#define I2C_CORE_BIT_TIP_MSK    (1<<1) //TIP, Transfer in progress
#define I2C_CORE_BIT_IF_MSK     (1<<0) //IF, Interrupt Flag

void i2c_master_init(u32 i2c_master_base);
u8 i2c_master_write_register(u32 i2c_master_base,
                                 u8 i2c_slave_address, u8 i2c_slave_register,
                                 u8 value);
u8 i2c_master_read_register(u32 i2c_master_base,
                                u8 i2c_slave_address, u8 i2c_slave_register,
                                u8* value);
u8 i2c_master_write(u32 i2c_master_base,
                        u8 i2c_slave_address,
                        u8 value);

u8 i2c_master_read(u32 i2c_master_base,
						u8 i2c_slave_address,
						u8* value);
#endif /*I2C_MASTER_*/
