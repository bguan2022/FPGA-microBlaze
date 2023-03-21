/***************************************************************************
 * File name   : HsCalFlash.c
 * Description : functions for communicating with the calibration flash
 *
 *
 ***************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"	/* EDK generated parameters */
#include "xintc.h"		/* Interrupt controller device driver */
#include "xspi.h"		/* SPI device driver */
#include "xil_exception.h"
#include "../HsModule.h"
#include "spi_flash_controller.h"
#include "HsCalFlash.h"

#define CAL_SPI_DEVICE_ID	XPAR_SPI_0_DEVICE_ID

#define CAL_SECTION_ID_SIZE		 4
#define CAL_SECTION_LENGTH_SIZE	 4

/*
 * The instances to support the device drivers are global such that they
 * are initialized to zero each time the program runs. They could be local
 * but should at least be static so they are zeroed.
 */
static XSpi CalFlashSpi;

int InitCalFlash()
{
	return SpiFlashInit(CAL_SPI_DEVICE_ID, &CalFlashSpi);
}

int CalFlashRead(unsigned int StartAddress, unsigned char* DestBuffer, unsigned int Length)
{
	return SpiFlashRead(&CalFlashSpi, StartAddress, DestBuffer, Length);
}

int CalFlashWrite(unsigned int StartAddress, unsigned char* SrcBuffer, unsigned int Length)
{
	return SpiFlashWrite(&CalFlashSpi, StartAddress, SrcBuffer, Length);
}
