#include "xparameters.h"
#include "xspi.h"
#include "xil_printf.h"
#include "spi.h"

#define SPI_DEV_ADDR_PLUS_BYTE 	2
#define SPI_DEV_ADDR_LEN 		1
#define WR_LEN_MAX 				128
#define WR_DATA_LEN 			(WR_LEN_MAX + 2)
#define RD_LEN_MAX				128

int readSPIReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 readSize, u8 *readData) {
	u8 outBuffer[2];
	u8 inBuffer[RD_LEN_MAX];
	if (addrSize == 2){
		outBuffer[0] = (regAddr >>8) & 0xFF;	// Send high register address byte first
		outBuffer[1] = regAddr & 0xFF;
	}
	else{
		outBuffer[0] = regAddr & 0xFF;
	}

	inBuffer[0] = 0;
	inBuffer[1] = 0;
	int i;
	int sendCount, readCount;

	if (readSize > RD_LEN_MAX) {
		printf("spi read size must be less than d bytes!\r\n", RD_LEN_MAX);
		return 1;
	}

	if (addrSize < 1 || addrSize > 2) {
		printf("readSPIReg: invalid addrSize %d", addrSize);
		return 1;
	}

	sendCount=XSpi_ReadReg(SPI_ADDR, spi_addr, outBuffer, addrSize, XSPI_STOP);
    if(sendCount == 0) {
    	return 1; // failed
    }
    readCount=XSpi_ReadReg(SPI_ADDR,spi_addr, inBuffer, readSize, XSPI_STOP);

    if (readCount != readSize) {
    	printf("spi readCount = %d, requested %d bytes\r\n", readCount, readSize);
    }

    for(i=0;i<readCount;i++) {
    	readData[i] = inBuffer[i];
    }
    return 0;
}

int readSPIReg32(u8 i2c_addr, u32 regAddr, u32 *readData) {
	u8 outBuffer[4];
	u8 inBuffer[16];
	outBuffer[0] = regAddr & 0xFF;		// Send low register address byte first
	outBuffer[1] = (regAddr >>8) & 0xFF;
	outBuffer[2] = (regAddr >>16) & 0xFF;
	outBuffer[3] = (regAddr >>24) & 0xFF;
	inBuffer[0] = 0;
	inBuffer[1] = 0;
	inBuffer[2] = 0;
	inBuffer[3] = 0;
	int sendCount, readCount;

	sendCount=XSpi_WriteReg(SPI_ADDR, spi_addr, outBuffer, 4, XIIC_STOP);
    if(sendCount == 0) {
    	return 1; // failed
    }
    readCount=XSpi_ReadReg(IIC_IIC_ADDR, i2c_addr, inBuffer, 4, XIIC_STOP);
    printf("Data read of %02x%02x%02x%02x is %02x %02x %02x %02x \r\n", outBuffer[3], outBuffer[2], outBuffer[1], outBuffer[0], inBuffer[0], inBuffer[1], inBuffer[2], inBuffer[3]);

    if (readCount != 4) {
    	printf("SPI readCount = %d, requested 4 bytes\r\n", readCount);
    }

	*readData = inBuffer[0];
	*readData |= inBuffer[1] <<8;
	*readData |= inBuffer[2] <<16;
	*readData |= inBuffer[3] <<24;
  return 0;
}

//SPI write: 1 or 2 byte register address + 1 byte data
int writeI2cReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 writeData) {
	u8 WriteData[3];
	int sendcount;

	if (addrSize == 1) {
		WriteData[0] = regAddr & 0xFF;
		WriteData[1] = writeData;
	} else if (addrSize == 2) {
		WriteData[0] = (regAddr >> 8) & 0xFF;		// Send high register address byte first
		WriteData[1] = regAddr & 0xFF;
		WriteData[2] = writeData;
	} else {
		printf("writeSPIReg: invalid addrSize %d", addrSize);
		return 1;
	}

	sendcount = XSpi_WriteReg(SPI_ADDR, spi_addr, WriteData, addrSize + SPI_DEV_ADDR_PLUS_BYTE, XSPI_STOP);
	if(sendcount != (addrSize + SPI_DEV_ADDR_PLUS_BYTE)) {
		print(" Did not send enough data\r\n");
		return 1;
	}
	return 0;
}



