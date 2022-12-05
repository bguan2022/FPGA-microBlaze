## Steps:
  1. Add Xilinx IP to bd
  2. Define register map
  3. Generate bitstream 
  4. Export hardware to Vitis
  5. Create workspace and include Xilinx driver (.c .h)
  6. set up UART terminal with specific baud rate 
  7. Initilize the driver (IIC / SPI)
  8. Read / write to a specific address (include offset)

## IIC

7-bit slave addressing 

The XIic_Start() function must be called after this function before the device is ready to send and receive data on the IIC bus.


```unsigned XIic_Send	(	UINTPTR 	BaseAddress, u8 	Address, u8 * 	BufferPtr, unsigned 	ByteCount, u8 	Option )```
returns number of bytes sent. Only support 7-bit addressing, returns zero if bus is busy  

```unsigned XIic_Recv	(	UINTPTR 	BaseAddress, u8 	Address, u8 * 	BufferPtr, unsigned 	ByteCount, u8 	Option )```	
returns number of bytes received 

```XSpi_Initialize(InstancePtr, DeviceId) ``` 
The driver looks up its own configuration structure created by the tool-chain based on an ID provided by the tool-chain.


## SPI
 It is the driver for an SPI master or slave device. It supports 8-bit, 16-bit and 32-bit wide data transfers.


```define XSpi_ReadReg	(	BaseAddress,RegOffset)		   XSpi_In32((BaseAddress) + (RegOffset))```

#Read from the specified Spi device register.

#Parameters
  BaseAddress	contains the base address of the device.
  RegOffset	contains the offset from the 1st register of the device to select the specific register.

#Returns: The value read from the register.

```define XSpi_WriteReg	(	 	BaseAddress,RegOffset,RegisterValue )		   XSpi_Out32((BaseAddress) + (RegOffset), (RegisterValue))```

#Write to the specified Spi device register.

#Parameters
  BaseAddress	contains the base address of the device.
  RegOffset	contains the offset from the 1st register of the device to select the specific register.

#RegisterValue	is the value to be written to the register.
#Returns: None.

**BaseAddress:**	contains the base address of the device.
**RegOffset:**	contains the offset from the 1st register of the device to select the specific register.
**RegisterValue:** the value to be written to the register.



**Reference:**

SPI:
https://xilinx.github.io/embeddedsw.github.io/spi/doc/html/api/index.html


IIC: 
https://xilinx.github.io/embeddedsw.github.io/iic/doc/html/api/index.html

