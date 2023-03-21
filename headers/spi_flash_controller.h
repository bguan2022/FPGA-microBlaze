//--spi_flash_controller.h---------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

#include "xspi.h"		/* SPI device driver */

int SpiFlashInit(unsigned int SpiDeviceBase, XSpi *SpiPtr);
int SpiFlashRead(XSpi *SpiPtr, unsigned int StartAddress, unsigned char* DestBuffer, unsigned int Length);
int SpiFlashWrite(XSpi *SpiPtr, unsigned int StartAddress, unsigned char* SrcBuffer, unsigned int Length);
int SpiFlashWriteEnable(XSpi *SpiPtr);
int SpiFlashWriteBytes(XSpi *SpiPtr, u8* SrcBuffer, u32 Addr, u32 ByteCount);
int SpiFlashReadBytes(XSpi *SpiPtr, u8* DestBuffer, u32 Addr, u32 ByteCount);
int SpiFlashSubsectorErase(XSpi *SpiPtr, u32 Addr);
int SpiFlashGetStatus(XSpi *SpiPtr);
int SpiFlashWaitForFlashNotBusy(XSpi *SpiPtr);


