#ifndef I2C_H
#define I2C_H

#define IIC_DEVICE_ID XPAR_AXI_IIC_0_DEVICE_ID
#define IIC_IIC_ADDR XPAR_IIC_0_BASEADDR
#define IIC_WRITE_LEN_MAX 128

int readI2cReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 readSize, u8 *ReadData);
int writeI2cReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 writeData);
int writeI2cRegMultiple(u8 i2c_addr, u16 regAddr, u8 addrSize, u8* writeData, u8 writeLen);
int readI2cReg32(u8 i2c_addr, u32 regAddr, u32 *ReadData);
int IicMuxConfig(u8 muxIicAddr, u8 configByte, u8* readByte);


#endif
