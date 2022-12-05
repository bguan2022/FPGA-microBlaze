
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
  sou_addr = 0x44000000;
  offset_addr = 0;
  init_platform();
  print("Starting Microblaze Test program \n\r");
  counter = 0;
  
  while (1){
    readSPIReg(spi_addr,offset_addr,*readData);
    counter++;
    sleep(1000); 			//pause 1 sec
    if (counter > 100) 			//collect 100 samples 
        break;
  }
  
   cleanup_platform();
   return 0;
}; 
