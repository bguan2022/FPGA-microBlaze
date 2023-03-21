/***************************************************************************
 * File name   : i2c_master.c
 * Description : This file contains functions for using the i2c master core 
 *
 * Revision History:   June 17, 2008:  Creation, Mohamed Sebbar
 * 
 ***************************************************************************/ 

#include <xil_types.h>
//#include <unistd.h>
#include <xio.h>
#include "i2c_master.h"
#include "HsTimer.h"

//------------------------------------------------------------------------------------------------------------------------------------------
void I2CMasterSetReg( u32 I2CMasterBase, u32 RegIdx, u32 NewVal )
{
	XIo_Out8( (I2CMasterBase) + (RegIdx), (NewVal) );
}
unsigned char I2CMasterGetReg( u32 I2CMasterBase, u32 RegIdx )
{
	return XIo_In32( (I2CMasterBase) + (RegIdx) );
}

/*
 * Function name: i2c_master_init
 * Parameters:   u32 i2c_master_base  : i2c-master core base address
 * Return value: none
 * Description:  Initializes the I2C Master Core.
 *               Must be called at the begining of Main.
 */
void i2c_master_init(u32 i2c_master_base)
{
	//reset i2c core
	I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CTR, I2C_CORE_BIT_RESET);
	usleep(100);

	//remove from reset
	I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CTR, 0);

    //clear control register. Place core in reset and disable I2C-Master Core
	I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CTR, 0x00);

    //set clock prescaler to set SCL clock to 90KHz
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_PRERLO, 0xdd);
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_PRERHI, 0x00);

    //enable I2C-Master Core
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CTR, I2C_CORE_BIT_COREEN_MSK);

    usleep(100000); // todo: determine whether this is really necessary / how to get around it
}


/*
 * Function name: i2c_master_write_register
 * Parameters:   u32 i2c_master_base  : i2c-master core base address
 *               u8 i2c_slave_address : i2c-slave device address
 *               u8 I2C_slave_register: i2c-slave device register address
 *               u8 value             : byte to write to the register in i2c-slave device
 * Return value: 0x00 : Operation successful
 *               0x10: ACK not received after sending the i2c-slave device address
 *               0x11: ACK not received after sending the i2c-slave device register address
 *               0x12: ACK not received after sending data byte 
 *               0xFF: I2C bus busy
 * Description:  Writes a byte to a register in the i2c-slave device.
 *               I2C Sequence:
 *                 1) generate start signal, write slave address + write bit
 *                 2) receive acknowledge from slave
 *                 3) write slave register address
 *                 4) receive acknowledge from slave
 *                 5) write data byte
 *                 6) receive acknowledge from slave
 *                 7) generate stop signal, indicating end of transfer
 */
u8 i2c_master_write_register(u32 i2c_master_base, u8 i2c_slave_address, u8 I2C_slave_register, u8 value)
{
    u8 status = 0;

    //guarantee bus stays free for 4.7us between accesses (between a stop and start condition)
    usleep(5);

    //check if the i2c bus is free
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_BUSY_MSK) == I2C_CORE_BIT_BUSY_MSK)
        return 0xFF;

    //write i2c-slave device address + write bit to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, i2c_slave_address << 1);
    //set STA bit and WR bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_STA_MSK | I2C_CORE_BIT_WR_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x10;

    //write i2c-slave device register address to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, I2C_slave_register);
    //set WR bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_WR_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x11;

    //write data byte to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, value);
    //set WR bit and STO bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_WR_MSK | I2C_CORE_BIT_STO_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x12;

    return status;
}


/*
 * Function name: i2c_master_read_register
 * Parameters:   u32 i2c_master_base  : i2c-master core base address
 *               u8 i2c_slave_address : i2c-slave device address
 *               u8 I2C_slave_register: i2c-slave device register address
 *               u8* value            : where to save the value readen from the i2c-slave device register
 * Return value: 0x00: Operation successful
 *               0x13: ACK not received after sending i2c-slave address
 *               0x14: ACK not received after sending i2c-slave register address
 *               0x15: ACK not received after sending data byte
 * Description:  Reads Register value in the I2C slave device.
 *               I2C Sequence:
 *                 1) generate start signal, write slave address + write bit
 *                 2) receive acknowledge from slave
 *                 3) write slave register address
 *                 4) receive acknowledge from slave
 *                 5) generate stop signal
 *                 6) generate start signal
 *                 7) write slave address + read bit
 *                 8) receive acknowledge from slave
 *                 9) read byte from slave
 *                 10) write no acknowledge (NACK) to slave, indicating end of transfer
 *                 11) generate stop signal
 */
u8 i2c_master_read_register(u32 i2c_master_base, u8 i2c_slave_address, u8 I2C_slave_register, u8* value)
{
    u8 status = 0;
    
    //guarantee bus stays free for 4.7us between accesses (between a stop and start condition)
    usleep(5);

    //check if the i2c bus is free
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_BUSY_MSK) == I2C_CORE_BIT_BUSY_MSK)
        return 0xFF;

    //write i2c-slave device address + write bit to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, i2c_slave_address << 1);
    //set STA bit and WR bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_STA_MSK | I2C_CORE_BIT_WR_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x13;

    //write i2c-slave device register address to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, I2C_slave_register);
    //set WR bit and STO bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_WR_MSK | I2C_CORE_BIT_STO_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x14;

    if(status == 0)
    {
        //guarantee bus stays free for 4.7us between accesses (between a stop and start condition)
        usleep(5);

        //check if the i2c bus is free
        if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_BUSY_MSK) == I2C_CORE_BIT_BUSY_MSK)
            return 0xFF;

        //write i2c-slave device address + read bit to transmit register
        I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, (i2c_slave_address << 1) | 0x01);
        //set STA bit and WR bit
        I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_STA_MSK | I2C_CORE_BIT_WR_MSK);

        //wait for TIP flag to negate
        while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

        //read RxACK bit from Status Register, should be ‘0’
        if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
            status += 0x15;

        //set RD, ACK and STO bits
        I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_RD_MSK | I2C_CORE_BIT_ACK_MSK | I2C_CORE_BIT_STO_MSK);

        //wait for TIP flag to negate
        while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

        *value = I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_RXR);
    }

    return status;
}


/*
 * Function name: i2c_master_write
 * Parameters:   u32 i2c_master_base  : i2c-master core base address
 *               u8 i2c_slave_address : i2c-slave device address
 *               u8 value             : byte to write to the i2c-slave device
 * Return value: 0x00 : Operation successful
 *               0x16: ACK not received after sending the i2c-slave device address
 *               0x17: ACK not received after sending data byte 
 * Description:  Writes a byte to a register in the i2c-slave device.
 *               I2C Sequence:
 *                 1) generate start signal, write slave address + write bit
 *                 2) receive acknowledge from slave
 *                 3) write data byte
 *                 4) receive acknowledge from slave
 *                 5) generate stop signal, indicating end of transfer
 */
u8 i2c_master_write(u32 i2c_master_base, u8 i2c_slave_address, u8 value)
{
    u8 status = 0;
    
    //check if the i2c bus is free
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_BUSY_MSK) == I2C_CORE_BIT_BUSY_MSK)
        return 0xFF;

    //guarantee bus stays free for 4.7us between accesses (between a stop and start condition)
    usleep(5);

    //write i2c-slave device address + write bit to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, i2c_slave_address << 1);
    //set STA bit and WR bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_STA_MSK | I2C_CORE_BIT_WR_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x16;

    //write data byte to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, value);
    //set WR bit and STO bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_WR_MSK | I2C_CORE_BIT_STO_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x17;

    return status;
}

u8 i2c_master_read(u32 i2c_master_base,	u8 i2c_slave_address, u8* value)
{

    u8 status = 0;

    //guarantee bus stays free for 4.7us between accesses (between a stop and start condition)
    usleep(5);

    //check if the i2c bus is free
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_BUSY_MSK) == I2C_CORE_BIT_BUSY_MSK)
        return 0xFF;

    //write i2c-slave device address + read bit to transmit register
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_TXR, (i2c_slave_address << 1) | 0x01);
    //set STA bit and WR bit
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_STA_MSK | I2C_CORE_BIT_WR_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    //read RxACK bit from Status Register, should be ‘0’
    if((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_RXACK_MSK) == I2C_CORE_BIT_RXACK_MSK)
        status += 0x15;

    //set RD, ACK and STO bits
    I2CMasterSetReg(i2c_master_base, I2C_CORE_REG_CR, I2C_CORE_BIT_RD_MSK | I2C_CORE_BIT_ACK_MSK | I2C_CORE_BIT_STO_MSK);

    //wait for TIP flag to negate
    while ((I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_SR) & I2C_CORE_BIT_TIP_MSK) == I2C_CORE_BIT_TIP_MSK);

    *value = I2CMasterGetReg(i2c_master_base, I2C_CORE_REG_RXR);

    return status;
}
