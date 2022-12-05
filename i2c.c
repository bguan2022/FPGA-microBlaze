#include "xparameters.h"
#include "xiic.h"
#include "xil_printf.h"


#include "i2c.h"

#define I2C_DEV_ADDR_PLUS_BYTE 	  2
#define I2C_DEV_ADDR_LEN 		      1
#define WR_LEN_MAX 				        128
#define WR_DATA_LEN 			        (WR_LEN_MAX + 2)
#define RD_LEN_MAX				        128



int readI2cReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 readSize, u8 *readData) {
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
		printf("i2c read size must be less than d bytes!\r\n", RD_LEN_MAX);
		return 1;
	}

	if (addrSize < 1 || addrSize > 2) {
		printf("readI2cReg: invalid addrSize %d", addrSize);
		return 1;
	}

    //printf("readI2cReg: i2c_addr: %02x outBuffer = %02x %02x addrSize: %d\r\n",i2c_addr, outBuffer[0], outBuffer[1], addrSize);
	sendCount=XIic_Send(IIC_IIC_ADDR, i2c_addr, outBuffer, addrSize, XIIC_STOP);
    if(sendCount == 0) {
    	return 1; // failed
    }
//    printf("Data sent: %i\r\n", sendCount);
    readCount=XIic_Recv(IIC_IIC_ADDR, i2c_addr, inBuffer, readSize, XIIC_STOP);
//    printf("Data read is 0x%x\r\n", inBuffer[0]);

    if (readCount != readSize) {
    	printf("i2c readCount = %d, requested %d bytes\r\n", readCount, readSize);
    }

    for(i=0;i<readCount;i++) {
    	readData[i] = inBuffer[i];
    }
    return 0;
}

int readI2cReg32(u8 i2c_addr, u32 regAddr, u32 *readData) {
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

    //printf("readI2cReg: i2c_addr: %02x outBuffer = %02x %02x addrSize: %d\r\n",i2c_addr, outBuffer[0], outBuffer[1], addrSize);
	sendCount=XIic_Send(IIC_IIC_ADDR, i2c_addr, outBuffer, 4, XIIC_STOP);
    if(sendCount == 0) {
    	return 1; // failed
    }
//    printf("Data sent: %i\r\n", sendCount);
    readCount=XIic_Recv(IIC_IIC_ADDR, i2c_addr, inBuffer, 4, XIIC_STOP);
    printf("Data read of %02x%02x%02x%02x is %02x %02x %02x %02x \r\n", outBuffer[3], outBuffer[2], outBuffer[1], outBuffer[0], inBuffer[0], inBuffer[1], inBuffer[2], inBuffer[3]);

    if (readCount != 4) {
    	printf("i2c readCount = %d, requested 4 bytes\r\n", readCount);
    }

	*readData = inBuffer[0];
	*readData |= inBuffer[1] <<8;
	*readData |= inBuffer[2] <<16;
	*readData |= inBuffer[3] <<24;

    return 0;
}

// I2C write: 1 or 2 byte register address + 1 byte data
int writeI2cReg(u8 i2c_addr, u16 regAddr, u8 addrSize, u8 writeData) {
	u8 WriteData[3];
	int sendcount;

	if (addrSize == 1) {
		WriteData[0] = regAddr & 0xFF;
		WriteData[1] = writeData;
		// printf("writeI2cReg: Device %02x Addr: 0x%02x Data: %02x\r\n", i2c_addr, WriteData[0], WriteData[1]);
	} else if (addrSize == 2) {
		WriteData[0] = (regAddr >> 8) & 0xFF;		// Send high register address byte first
		WriteData[1] = regAddr & 0xFF;
		WriteData[2] = writeData;
		// printf("writeI2cReg: Device %02x Addr: 0x%04x Data: %02x\r\n", i2c_addr, regAddr, WriteData[2]);
	} else {
		printf("writeI2cReg: invalid addrSize %d", addrSize);
		return 1;
	}

	sendcount = XIic_Send(IIC_IIC_ADDR, i2c_addr, WriteData, addrSize + I2C_DEV_ADDR_PLUS_BYTE, XIIC_STOP);
	if(sendcount != (addrSize + I2C_DEV_ADDR_PLUS_BYTE)) {
		print(" Did not send enough data\r\n");
		return 1;
	}
	return 0;
}

// I2C write: 1 or 2 byte register address + 1 byte data
int writeI2cRegMultiple(u8 i2c_addr, u16 regAddr, u8 addrSize, u8* writeData, u8 writeLen) {
	u8 WriteData[WR_DATA_LEN];
	int sendcount;

	if (writeLen > WR_LEN_MAX){
		printf(" Write length too many bytes, max %d \r\n", WR_LEN_MAX);
		return 1;
	}

	if (addrSize == 1) {
		WriteData[0] = regAddr & 0xFF;
		//WriteData[1] = writeData;
		for (u8 i = 0; i < writeLen; i++){
			WriteData[1+i] = writeData[i];
		}
		// printf("writeI2cReg: Device %02x Addr: 0x%02x Data: %02x\r\n", i2c_addr, WriteData[0], WriteData[1]);
	} else if (addrSize == 2) {
		WriteData[0] = (regAddr >> 8) & 0xFF;		// Send high register address byte first
		WriteData[1] = regAddr & 0xFF;
		//WriteData[2] = writeData;
		for (u8 i = 0; i < writeLen; i++){
			WriteData[2+i] = writeData[i];
		}
		// printf("writeI2cReg: Device %02x Addr: 0x%04x Data: %02x\r\n", i2c_addr, regAddr, WriteData[2]);
	} else {
		printf("writeI2cReg: invalid addrSize %d", addrSize);
		return 1;
	}

	sendcount = XIic_Send(IIC_IIC_ADDR, i2c_addr, WriteData, (addrSize + writeLen), XIIC_STOP);
	if(sendcount != (addrSize + writeLen)) {
		print(" Did not send enough data\r\n");
		return 1;
	}
	return 0;
}


int IicMuxConfig(u8 muxIicAddr, u8 configByte, u8* readByte)
{
#define CONFIG_SIZE 1
	int sendcount;
	sendcount = XIic_Send(IIC_IIC_ADDR, muxIicAddr, &configByte, CONFIG_SIZE, XIIC_STOP);
	if(sendcount != (CONFIG_SIZE)) {
		print(" Did not send enough data\r\n");
		return 1;
	}

	sendcount = XIic_Recv(IIC_IIC_ADDR, muxIicAddr, readByte, CONFIG_SIZE, XIIC_STOP);
	if(sendcount != (CONFIG_SIZE)) {
		print(" Did not receive enough data\r\n");
		return 1;
	}

	if (*readByte != configByte){
		print(" Data readback did not match data written\r\n");
		return 1;
	}
	return 0;
}
