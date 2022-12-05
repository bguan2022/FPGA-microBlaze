#ifndef SPI_H
#define SPI_H

#define SPI_DEVICE_ID XPAR_AXI_IIC_0_DEVICE_ID
#define SPI_IIC_ADDR XPAR_IIC_0_BASEADDR
#define SPI_WRITE_LEN_MAX 128


int readSPIReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 readSize, u8 *ReadData);
int writeSPIReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 writeData);
int writeSPIRegMultiple(u8 i2c_addr, u16 regAddr, u8 addrSize, u8* writeData, u8 writeLen);
int readSPIReg32(u8 i2c_addr, u32 regAddr, u32 *ReadData);

#endif
