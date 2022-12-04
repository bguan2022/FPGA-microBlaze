
define XSpi_ReadReg	(	BaseAddress,RegOffset)		   XSpi_In32((BaseAddress) + (RegOffset))
#Read from the specified Spi device register.

#Parameters
  BaseAddress	contains the base address of the device.
  RegOffset	contains the offset from the 1st register of the device to select the specific register.

#Returns: The value read from the register.

define XSpi_WriteReg	(	 	BaseAddress,RegOffset,RegisterValue )		   XSpi_Out32((BaseAddress) + (RegOffset), (RegisterValue))
#Write to the specified Spi device register.

#Parameters
  BaseAddress	contains the base address of the device.
  RegOffset	contains the offset from the 1st register of the device to select the specific register.

#RegisterValue	is the value to be written to the register.
#Returns: None.

