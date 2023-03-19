# Micro-blaze Driver Set-up
## Steps:
  1. Add Xilinx IP (AXI_IIC / AXI_QSPI) to bd
  2. Define register map 
  3. Generate bitstream 
  4. Export hardware to Vitis
  5. Create workspace and include Xilinx driver (.c .h)
  6. set up UART terminal with specific baud rate 
  7. Initilize the driver (IIC / SPI)
  8. Read / write to a specific address (include offset)

## Xilinx IP
- SPI mode: standalone / dual / quad 
- For xip and standard modes, ext_spi_clk may be limited to 60 MHz.

SPI: https://docs.xilinx.com/r/en-US/pg153-axi-quad-spi/Dual/Quad-SPI-Mode

![image](https://user-images.githubusercontent.com/42010432/205732836-8a5c5b40-5160-4124-ae43-36b2feeb923d.png)

## IIC Driver
7-bit slave addressing 

The XIic_Start() function must be called after this function before the device is ready to send and receive data on the IIC bus.


```unsigned XIic_Send	(	UINTPTR 	BaseAddress, u8 	Address, u8 * 	BufferPtr, unsigned 	ByteCount, u8 	Option )```

returns number of bytes sent. Only support 7-bit addressing, returns zero if bus is busy  

```unsigned XIic_Recv	(	UINTPTR 	BaseAddress, u8 	Address, u8 * 	BufferPtr, unsigned 	ByteCount, u8 	Option )```	

returns number of bytes received 

```XSpi_Initialize(InstancePtr, DeviceId) ``` 

The driver looks up its own configuration structure created by the tool-chain based on an ID provided by the tool-chain.


## SPI Driver 
 It is the driver for an SPI master or slave device. It supports 8-bit, 16-bit and 32-bit wide data transfers.


```define XSpi_ReadReg	(	BaseAddress,RegOffset)		   XSpi_In32((BaseAddress) + (RegOffset))```

#Read from the specified Spi device register.

```define XSpi_WriteReg	(	 	BaseAddress,RegOffset,RegisterValue )		XSpi_Out32((BaseAddress) + (RegOffset), (RegisterValue))```

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

#Simulation 
https://docs.xilinx.com/v/u/en-US/xapp1180
https://medium.com/@sapphire.sharma1996/behavioral-rtl-simulation-with-microblaze-131671e86f04

1.  By saving and building the SDK project, it creates a .elf file which we will be using for our simulation.
2.  Add your generated .elf file from your SDK project into Vivado
3.  Associate your .elf file by right-clicking on your .elf file under Simulation sources hierarchy
4.  Make sure Properties of your .elf file associate with the ublaze
5.  Run RTL sim


