
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"

#include "xparameters.h"
#include "xiic.h"
#include "xil_io.h"

#include "cli.h"
#include "i2c.h"


int main(){
  u32 spi_addr,offset_addr,*readData, counter;
  spi_addr = 0x44000000;
  offset_addr = 0;
  init_platform();
  print("Starting Microblaze Test program \n\r");
  counter = 0;
  XSpi * 	Spi;

  DeviceID = ; 
  int stat; 
  
  	/*
	 * Initialize the SPI driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h.
	 */
  stat = XSpi_Initialize(&spi, SPI_DEVICE_ID)		
  check_spi_status(stat); 
  
  stat = SetupInterruptSystem(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
  
	/* Start the SPI driver so that interrupts and the device are enabled. */
	XSpi_Start(&Spi);
  
  while (1){
    readSPIReg(spi_addr,offset_addr,*readData);
    counter++;
    sleep(1000); 			//pause 1 sec
    if (counter > 100) 			//collect 100 samples 
        break;
  };
  
   XSpi_stop(&Spi);
   cleanup_platform();
   return 0;
}; 
