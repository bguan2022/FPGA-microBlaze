/*
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design (typically set to 115200)
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

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

#define LED_MATRIX_REG_BASE 0x44A00000
#define LED_MATRIX_NUM_REGS 5
#define I2C_ADDRESS_SIZE 1

funDef cli_fun [] = {
	{"?", helpq},
	{"help", help},
	{"seti2caddr", seti2caddr},
	{"readi2c", readi2c},
	{"readi2c32", readi2c32},
	{"writei2c", writei2c},
	{"writeiicmux", writeiicmux},
	{"readledregs", readledregs},
	{"writeledreg", writeledreg},
	{"status", status},
	{"mainloop", mloop},
	{"stoploop", sloop},
	{"SiInit", SiInit}
};

int main()
{
    init_platform();
    print("Starting Microblaze Test program \n\r");
    unsigned int backgroundLoop = 1;

    char buf[100];
    char delimit[] = " ";

    char *arg0, *arg1, *arg2;

	int stat;
	//u8 i2c_addr = 0x28;
	u8 i2c_addr = 0x39;
	u8 	addr8, size;
	u16 addr;
	u32 addr32, data32;
	u8 data8[IIC_WRITE_LEN_MAX], regData[16];
	int i;

	setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    // Initialization Sequences
    //initSi();
	print("User Input Command: ");
	int dataReadSize = 0;
	unsigned int elements = sizeof(cli_fun) / sizeof(funDef);
    while(1) {

    	dataReadSize = nonBlock_getInput(buf, 100);
		if(dataReadSize > 0) { // Handle CLI
			print("\n\r");

			arg0 = strtok(buf, delimit);
			funEnum funcSelected = None;
			for(int i = 0; i < elements; i++) {
				if(!strcmp(arg0, cli_fun[i].name)) {
					funcSelected = cli_fun[i].func;
					break;
				}
			}

			switch(funcSelected) {

        case helpq: // list all commands
        case help: // list all commands
            printf("Microblaze test program. Valid commands:\r\n");
            for(int i = 0; i < elements; i++) {
                printf("\t%s\r\n",cli_fun[i].name);
            }
            printmenu();
          break;

        case seti2caddr: // set i2c register address - format r address
          arg1 = strtok(NULL, delimit);
          i2c_addr = strtoul(arg1, NULL, 16);
            printf("Setting i2c device addr = 0x%x\r\n", i2c_addr);
          break;

        case readi2c: // read i2c register - format r address
            arg1 = strtok(NULL, delimit);
            addr8 = strtoul(arg1, NULL, 16);
            arg2 = strtok(NULL, delimit);
            size = strtoul(arg2, NULL, 10);
            stat = readI2cReg(i2c_addr, addr8, I2C_ADDRESS_SIZE, size, regData);
            if(stat == 0) {
              printf("i2c %02x Reg 0x%04x = ", i2c_addr, addr8);
              for (i=0;i<size;i++) {
                printf(" %02x",regData[i]);
              }
              printf("\r\n");
            } else {
              print("Read Register Failed\r\n");
            }
            break;

        case readi2c32: // read i2c register - format r address
            arg1 = strtok(NULL, delimit);
            addr32 = strtoul(arg1, NULL, 16);
            u32 regData32[2];

            stat = readI2cReg32(i2c_addr, addr32, regData32);
            if(stat == 0) {
              printf("i2c %02x Reg 0x%08x = %08x\r\n", i2c_addr, addr32, regData32[0]);
            } else {
              printf("readi2c32 of address: 0x%02x Failed\r\n", i2c_addr);
            }
            break;

        case writei2c: // write i2c register - format 'w address data', 16 bit address, 8 bit data
            arg1 = strtok(NULL, delimit);
            addr8 = strtoul(arg1, NULL, 16);
            u8 dataIndex;
            for(dataIndex = 0; dataIndex < IIC_WRITE_LEN_MAX; dataIndex++){
              arg2 = strtok(NULL, delimit);
              if (arg2 == NULL){
                break;
              }
              data8[dataIndex] = strtoul(arg2, NULL, 16);
            }
            stat = writeI2cRegMultiple(i2c_addr, addr8, I2C_ADDRESS_SIZE, data8, dataIndex);
            if(stat == 0) {
              printf("Wrote i2c %02x Register 0x%x(Reg%i) with", i2c_addr, addr8, addr8);
              for (u8 i = 0; i < dataIndex; i++){
                printf(" 0x%x",data8[i]);
              }
              printf("\r\n");
            } else {
              print("Write Register Failed\r\n");
            }
            break;

        case writeiicmux: //write the control register of the iic mux to configure which channels are active
            arg1 = strtok(NULL, delimit);
            data8[0] = strtoul(arg1, NULL, 16);
            u8 readData8;

            stat = IicMuxConfig(i2c_addr, data8[0], &readData8);
            if(stat == 0) {
              printf("Wrote i2c mux control register with 0x%x\r\n", data8[0]);
              printf("Read i2c mux control register with 0x%x\r\n", readData8);
            } else {
              print("Write Mux Control Register Failed\r\n");
            }
            break;
          case readledregs: // read all axi registers (offset from the base reg)
            addr = strtoul(arg1, NULL, 16);
            printf("LED Matrix Registers:\r\n");
            for (i=0;i<LED_MATRIX_NUM_REGS;i++) {
              printf("\t%02X: %08X\r\n",(i*4), Xil_In32(LED_MATRIX_REG_BASE + (i*4)));
            }
            break;

        case writeledreg: // write register - format 'w address data',  (offset from the base reg)
            arg1 = strtok(NULL, delimit);
            arg2 = strtok(NULL, delimit);
            addr = strtoul(arg1, NULL, 16);
            data32 = strtoul(arg2, NULL, 16);
            Xil_Out32((LED_MATRIX_REG_BASE + addr),data32);
            printf("Wrote Register 0x%x with 0x%x\r\n", addr, data32);
            printf("Arg1 0x%s\r\n Arg2 0x%s\r\n", arg1, arg2);
            break;

        case status: // status
          arg1 = strtok(NULL, delimit);
          break;

        case mloop:
          print("Starting background loop\r\n");
          backgroundLoop = 1;
          break;

        case sloop:
          print("Stopping background loop\r\n");
          backgroundLoop = 0;
          break;
        // Si Specific Functions

        case SiInit:
          initSi();
          break;
        default: //
          printf("Command <%s> not recognized, please re-enter your command\n\r", arg0);
        }
        print("Input Command: ");
      }

      // Background Loop
      if(backgroundLoop){

      }
   }

      cleanup_platform();
      return 0;

}


int printmenu(){
          printf("LED Matrix registers:\r\n");
    			printf("LED Matrix AXI REGISTER DEFINITIONS\r\n");
    			printf("\t reg[0] = VERSION (read only)\r\n");
    			printf("\t reg[2] = control_reg\r\n");
    			printf("\t    cfg_led_matrix_mode = control_reg[3:0]\r\n");
    			printf("\t reg[2] = cfg_col_timer [31:0]\r\n");
    			printf("\t reg[3] = manual_row_col_reg\r\n");
    			printf("\t    cfg_manual_col_val = manual_row_col_reg[15:0]\r\n");
    			printf("\t    cfg_manual_row_val = manual_row_col_reg[31:16]\r\n");
    			printf("\treg[4] = vsync_interval [31:0]  (read only)\r\n");

    			printf("cfg_led_matrix_mode:\r\n");
    			printf("      00: LEDs off\r\n");
    			printf("      01: Step through all 256 LEDs with timing interval of cfg_col_timer t_clks per LED.\r\n");
    			printf("      02: Manually set row and col from manual_row_col_reg.\r\n");
    			printf("      03: Increment LED rows once per vsync; LED columns increment from cfg_manual_col_val\r\n");
    			printf("      04: Increment LED rows once per vsync; LED columns increment vsync_interval / 16\r\n");
          return 0;
}
