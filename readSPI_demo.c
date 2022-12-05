
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
  u32 spi_addr,offset_addr,*readDat;
  sou_addr = 0x44000000;
  offset_addr = 0;
  
  while (1){
    readSPIReg(spi_addr,offset_addr,*readData);
    sleep(1000); //pause 1 sec
  }
}; 
