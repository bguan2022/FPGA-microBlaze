#include "xparameters.h"
#include "xspi.h"
#include "xil_printf.h"
#include "spi.h"

#define SPI_DEV_ADDR_PLUS_BYTE 	2
#define SPI_DEV_ADDR_LEN 		1
#define WR_LEN_MAX 				128
#define WR_DATA_LEN 			(WR_LEN_MAX + 2)
#define RD_LEN_MAX				128


int readSPIReg(u32 SPI_ADDR, u32 offset, u32 *readData) {
	*readData=XSpi_ReadReg(SPI_ADDR, spi_addr);
	printf("Read %d SPI reg at %d, value is", writeData, SPI_ADDR+offset, *readData);
	return inBuffer;
}

int writeSPIReg(u32 SPI_ADDR, u32 offset, u32 writeData) {
	XSpi_WriteReg(SPI_ADDR, offset, writeData, addrSize);
	printf("Writing %d to SPI reg at %d", writeData, SPI_ADDR+offset);
	return 0;
}



