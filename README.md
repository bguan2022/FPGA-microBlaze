**Steps:**
  1. Add Xilinx IP to bd
  2. Define register map
  3. Generate bitstream 
  4. Export hardware to Vitis
  5. Create workspace and include Xilinx driver (.c .h)
  6. Initilize the driver (IIC / SPI)
  7. Read / write to a specific address (include offset)


**Reference: **
SPI:
https://xilinx.github.io/embeddedsw.github.io/spi/doc/html/api/index.html


IIC: 
https://xilinx.github.io/embeddedsw.github.io/iic/doc/html/api/index.html


**BaseAddress:**	contains the base address of the device.
**RegOffset:**	contains the offset from the 1st register of the device to select the specific register.
**RegisterValue:** the value to be written to the register.



