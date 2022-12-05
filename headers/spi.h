#ifndef SPI_H
#define SPI_H

#define SPI_DEVICE_ID XPAR_AXI_SPI_0_DEVICE_ID
#define SPI_SPI_ADDR XPAR_IIC_SPI_BASEADDR
#define SPI_WRITE_LEN_MAX 128


int readSPIReg(u32 spi_addr, u32 offset, u32 *ReadData);
int writeSPIReg(u32 spi_addr, u32 offset, u32 writeData);

#endif
