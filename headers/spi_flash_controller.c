/******************************************************************************
*
* Copyright (C) 2008 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/******************************************************************************
*
* @file xspi_stm_flash_example.c
*
* This file contains a design example using the SPI driver (XSpi) and hardware
* device with an STM serial Flash device (M25P series) in the interrupt mode.
* This example erases a Sector, writes to a Page within the Sector, reads back
* from that Page and compares the data.
*
* This example  has been tested with an M25P16 device. The bytes per page
* (STM_PAGE_SIZE) in M25P16 is 256.
*
* This example assumes that the underlying processor is MicroBlaze.
*
* @note
*
* None.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sd   02/26/08 First release
* 3.00a ktn  10/28/09 Converted all register accesses to 32 bit access.
*		      Updated to use the HAL APIs/macros. Replaced call to
*		      XSpi_Initialize API with XSpi_LookupConfig and
*		      XSpi_CfgInitialize.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"	/* EDK generated parameters */
#include "xil_exception.h"
#include "../HsModule.h"
#include "spi_flash_controller.h"


/************************** Constant Definitions *****************************/

/*
 * The following constant defines the slave select signal that is used to
 * to select the Flash device on the SPI bus, this signal is typically
 * connected to the chip select of the device.
 */
#define SPI_SELECT 0x01

/*
 * Definitions of the commands shown in this example.
 */
#define COMMAND_RANDOM_READ			0x03 /* Random read command */
#define COMMAND_PAGEPROGRAM_WRITE	0x02 /* Page Program command */
#define	COMMAND_WRITE_ENABLE		0x06 /* Write Enable command */
#define COMMAND_SUBSECTOR_ERASE		0x20 /* Sector Erase command */
#define COMMAND_SECTOR_ERASE		0xD8 /* Sector Erase command */
#define COMMAND_BULK_ERASE			0xC7 /* Bulk Erase command */
#define COMMAND_STATUSREG_READ		0x05 /* Status read command */


/**
 * This definitions specify the EXTRA bytes in each of the command
 * transactions. This count includes Command byte, address bytes and any
 * don't care bytes needed.
 */
#define READ_WRITE_EXTRA_BYTES	4 /* Read/Write extra bytes */
#define	WRITE_ENABLE_BYTES		1 /* Write Enable bytes */
#define SUBSECTOR_ERASE_BYTES	4 /* Sector erase extra bytes */
#define SECTOR_ERASE_BYTES		4 /* Sector erase extra bytes */
#define BULK_ERASE_BYTES		1 /* Bulk erase extra bytes */
#define STATUS_READ_BYTES		2 /* Status read bytes count */
#define STATUS_WRITE_BYTES		2 /* Status write bytes count */

/*
 * Flash not busy mask in the status register of the flash device.
 */
#define FLASH_SR_IS_READY_MASK	0x01 /* Ready mask */

/*
 * Number of bytes per page in the flash device.
 */
#define FLASH_MAX_ADDRESS	0xFFFFFF // 16 Mbytes
#define PAGE_SIZE_LOG		8 //256 Bytes
#define PAGE_SIZE			(1 << PAGE_SIZE_LOG)
#define PAGE_ADDR_MASK		(PAGE_SIZE - 1)
#define SUBSECTOR_SIZE_LOG	12 //4 KBytes
#define SUBSECTOR_SIZE		(1 << SUBSECTOR_SIZE_LOG)
#define SUBSECTOR_ADDR_MASK	(SUBSECTOR_SIZE - 1)
/*
 * Address of the page to perform Erase, Write and Read operations.
 */
#define FLASH_TEST_ADDRESS	0x00

/*
 * Byte offset value written to Flash. This needs to redefined for writing
 * different patterns of data to the Flash device.
 */
#define FLASH_TEST_BYTE	0x20

/*
 * Byte Positions.
 */
#define BYTE1				0 /* Byte 1 position */
#define BYTE2				1 /* Byte 2 position */
#define BYTE3				2 /* Byte 3 position */
#define BYTE4				3 /* Byte 4 position */
#define BYTE5				4 /* Byte 5 position */

#define DUMMYBYTE			0xFF /* Dummy byte */

/************************** Function Definitions ******************************/

/*****************************************************************************/
/**
*
* Main function to execute the Flash example.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/

#define CAL_DATA_START_ADDR	0
#define LIC_DATA_BASE		0xFFF000	//licence file offset at end of flash. 16MB - 4KBytes


#define CAL_HEADER_ID		0x1B0312FC
#define JIT_HEADER_ID		0x3D408ABB
#define RX_THRESH_HEADER_ID	0xB341D09E
#define RX_PHASE_HEADER_ID	0x43573437
#define LIC_HEADER_ID		0x8375BC73

#define CAL_SECTION_ID_SIZE		 4
#define CAL_SECTION_LENGTH_SIZE	 4

#define CAL_SERIAL_NUM_OFFSET	 0
#define CAL_HARDWARE_REV_OFFSET	16
#define CAL_MAN_DATE_OFFSET		32
#define CAL_CAL_DATE_OFFSET		48
#define CAL_DATA_RATE_OFFSET	64

int SpiFlashInit(unsigned int SpiDeviceBase, XSpi *SpiPtr)
{
	int Status;
	XSpi_Config *ConfigPtr;	/* Pointer to Configuration data */

	/*
	 * Initialize the SPI driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h.
	 */
	ConfigPtr = XSpi_LookupConfig(SpiDeviceBase);
	if (ConfigPtr == NULL) {
		return XST_DEVICE_NOT_FOUND;
	}

	Status = XSpi_CfgInitialize(SpiPtr, ConfigPtr,
				  ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the SPI device as a master and in manual slave select mode such
	 * that the slave select signal does not toggle for every byte of a
	 * transfer, this must be done before the slave select is set.
	 */
	Status = XSpi_SetOptions(SpiPtr, XSP_MASTER_OPTION |
						XSP_MANUAL_SSELECT_OPTION);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Select the STM flash device on the SPI bus, so that it can be
	 * read and written using the SPI bus.
	 */
	Status = XSpi_SetSlaveSelect(SpiPtr, SPI_SELECT);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the SPI driver so that interrupts and the device are enabled.
	 */
	XSpi_Start(SpiPtr);

	/*
	 * Disable Global interrupt to use polled mode operation
	 */
	XSpi_IntrGlobalDisable(SpiPtr);

	/*
	 * Wait till the Flash is not Busy.
	 */
	Status = SpiFlashWaitForFlashNotBusy(SpiPtr);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int SpiFlashRead(XSpi *SpiPtr, unsigned int StartAddress, unsigned char* DestBuffer, unsigned int Length)
{
	int Status;
	unsigned char* pBuffer;
	unsigned int SubsectorReadLength;

	unsigned int CurrAddress = StartAddress;
	unsigned int RemainingBytes = Length;
	unsigned int CurrSubsector = CurrAddress >> PAGE_SIZE_LOG;
	unsigned int CurrSubsectorAddress = CurrSubsector << PAGE_SIZE_LOG;
	while (RemainingBytes > 0) {
		pBuffer = DestBuffer + (CurrAddress-StartAddress);
		SubsectorReadLength = (CurrSubsectorAddress + PAGE_SIZE) - CurrAddress;
		if (SubsectorReadLength >= RemainingBytes) {
			SubsectorReadLength = RemainingBytes;
		}
		RemainingBytes -= SubsectorReadLength;

		Status = SpiFlashReadBytes(SpiPtr, (u8*)pBuffer, CurrAddress, SubsectorReadLength);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		CurrAddress = CurrSubsectorAddress + PAGE_SIZE;
		CurrSubsector++;
		CurrSubsectorAddress = CurrAddress;
	}

	return XST_SUCCESS;
}

int SpiFlashWrite(XSpi *SpiPtr, unsigned int StartAddress, unsigned char* SrcBuffer, unsigned int Length)
{
	int Status;
	unsigned int EndAddress = StartAddress + Length; // Address of byte after last
	unsigned int LastSubsector = EndAddress >> SUBSECTOR_SIZE_LOG;

	unsigned char SubsectorBuffer[SUBSECTOR_SIZE];
	unsigned char* pBuffer;

	unsigned int CurrAddress = StartAddress;
	unsigned int RemainingBytes = Length;
	unsigned int CurrSubsector = CurrAddress >> SUBSECTOR_SIZE_LOG;
	unsigned int CurrSubsectorAddress = CurrSubsector << SUBSECTOR_SIZE_LOG;
	while (CurrSubsector <= LastSubsector) {
		unsigned int SrcBufferStart = (CurrAddress-StartAddress);
		if ((CurrAddress == CurrSubsectorAddress) && (RemainingBytes >= SUBSECTOR_SIZE)) {
			pBuffer = SrcBuffer + SrcBufferStart;
			RemainingBytes -= SUBSECTOR_SIZE;
		}
		else {
			Status = SpiFlashRead(SpiPtr, CurrSubsectorAddress, SubsectorBuffer, SUBSECTOR_SIZE);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			unsigned int SrcBufferEnd = (CurrSubsectorAddress + SUBSECTOR_SIZE) - StartAddress;
			if (SrcBufferEnd > Length)
				SrcBufferEnd = Length;
			unsigned int Index;
			for (Index = SrcBufferStart; Index < SrcBufferEnd; Index++) {
				SubsectorBuffer[(Index + StartAddress) & SUBSECTOR_ADDR_MASK] = SrcBuffer[Index];
			}
			pBuffer = (unsigned char*) SubsectorBuffer;
			RemainingBytes -= (SrcBufferEnd-SrcBufferStart);
		}

		/*
		 * Perform the Write Enable operation.
		 */
		Status = SpiFlashWriteEnable(SpiPtr);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait till the Flash is not Busy.
		 */
		Status = SpiFlashWaitForFlashNotBusy(SpiPtr);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Perform the Sector Erase operation.
		 */
		Status = SpiFlashSubsectorErase(SpiPtr, CurrSubsectorAddress);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait till the Flash is not Busy.
		 */
		Status = SpiFlashWaitForFlashNotBusy(SpiPtr);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		unsigned int iChunk;
		for (iChunk = 0; iChunk < SUBSECTOR_SIZE; iChunk += PAGE_SIZE, pBuffer += PAGE_SIZE)
		{
			/*
			 * Perform the Write Enable operation.
			 */
			Status = SpiFlashWriteEnable(SpiPtr);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			/*
			 * Wait till the Flash is not Busy.
			 */
			Status = SpiFlashWaitForFlashNotBusy(SpiPtr);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			/*
			 * Write the data to the Page.
			 * Perform the Write operation.
			 */
			Status = SpiFlashWriteBytes(SpiPtr, pBuffer, CurrSubsectorAddress+iChunk, PAGE_SIZE);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			/*
			 * Wait till the Flash is not Busy.
			 */
			Status = SpiFlashWaitForFlashNotBusy(SpiPtr);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
		}

		CurrAddress = CurrSubsectorAddress + SUBSECTOR_SIZE;
		CurrSubsector++;
		CurrSubsectorAddress = CurrAddress;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function enables writes to the STM Serial Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashWriteEnable(XSpi *SpiPtr)
{
	int Status;

	/*
	 * Buffers used during read and write transactions.
	 */
	u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

	/*
	 * Prepare the WriteBuffer.
	 */
	WriteBuffer[BYTE1] = COMMAND_WRITE_ENABLE;

	/*
	 * Initiate the Transfer.
	 */
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				WRITE_ENABLE_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function writes the data to the specified locations in the STM Serial
* Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the address in the Buffer, where to write the data.
* @param	ByteCount is the number of bytes to be written.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashWriteBytes(XSpi *SpiPtr, u8* SrcBuffer, u32 Addr, u32 ByteCount)
{
	u32 Index;
	int Status;

	/*
	 * Buffers used during read and write transactions.
	 */
	u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

	/*
	 * Prepare the WriteBuffer.
	 */
	WriteBuffer[BYTE1] = COMMAND_PAGEPROGRAM_WRITE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 16);
	WriteBuffer[BYTE3] = (u8) (Addr >> 8);
	WriteBuffer[BYTE4] = (u8) Addr;
	for(Index = 0; Index < ByteCount; Index++) {
		WriteBuffer[Index+READ_WRITE_EXTRA_BYTES] = SrcBuffer[Index];
	}

	/*
	 * Initiate the Transfer.
	 */
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				(ByteCount + READ_WRITE_EXTRA_BYTES));
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function reads the data from the STM Serial Flash Memory
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the starting address in the Flash Memory from which the
*		data is to be read.
* @param	ByteCount is the number of bytes to be read.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashReadBytes(XSpi *SpiPtr, u8* DestBuffer, u32 Addr, u32 ByteCount)
{
	u32 Index;
	int Status;

	/*
	 * Buffers used during read and write transactions.
	 */
	u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];
	u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

	/*
	 * Prepare the WriteBuffer.
	 */
	WriteBuffer[BYTE1] = COMMAND_RANDOM_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 16);
	WriteBuffer[BYTE3] = (u8) (Addr >> 8);
	WriteBuffer[BYTE4] = (u8) Addr;

	/*
	 * Initiate the Transfer.
	 */
	Status = XSpi_Transfer( SpiPtr, WriteBuffer, ReadBuffer,
				(ByteCount + READ_WRITE_EXTRA_BYTES));
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	for(Index = 0; Index < ByteCount; Index++) {
		DestBuffer[Index] = ReadBuffer[Index+READ_WRITE_EXTRA_BYTES];
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function erases the contents of the specified Subector in the Serial
* Flash device.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the address within a sector of the Buffer, which is to
*		be erased.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The erased bytes will be read back as 0xFF.
*
******************************************************************************/
int SpiFlashSubsectorErase(XSpi *SpiPtr, u32 Addr)
{
	int Status;

	/*
	 * Buffers used during read and write transactions.
	 */
	u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

	/*
	 * Prepare the WriteBuffer.
	 */
	WriteBuffer[BYTE1] = COMMAND_SUBSECTOR_ERASE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 16);
	WriteBuffer[BYTE3] = (u8) (Addr >> 8);
	WriteBuffer[BYTE4] = (u8) (Addr);

	/*
	 * Initiate the Transfer.
	 */
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
					SUBSECTOR_ERASE_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function waits till the serial Flash is ready to accept next command.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		This function reads the status register of the Buffer and waits
*.		till the WIP bit of the status register becomes 0.
*
******************************************************************************/
int SpiFlashWaitForFlashNotBusy(XSpi *SpiPtr)
{
	int Status;
	u8 StatusReg;

	/*
	 * Buffers used during read and write transactions.
	 */
	u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];
	u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

	while(1) {
		/*
		 * Prepare the Write Buffer.
		 */
		WriteBuffer[BYTE1] = COMMAND_STATUSREG_READ;

		/*
		 * Initiate the Transfer.
		 */
		Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
							STATUS_READ_BYTES);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Check if the flash is ready to accept the next command.
		 * If so break.
		 */
		StatusReg = ReadBuffer[1];
		if((StatusReg & FLASH_SR_IS_READY_MASK) == 0) {
			break;
		}
	}

	return XST_SUCCESS;
}
