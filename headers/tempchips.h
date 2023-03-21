/***************************************************************************
 * File name   : tempchips.h
 * Description : header file for communicating with temperature chips  
 *
 * Revision History:   June 20, 2008: Creation, Mohamed Sebbar
 * 
 ***************************************************************************/

#ifndef TEMPCHIPS_
#define TEMPCHIPS_

#include <xil_types.h>

//#define DEBUG_TEMPCHIP    // enable this define to include the debugging printfs

/*** I2C-Slaves addresses ***/
#define TMP451  0x4C

#define TEMPCHIP_NUM_SENSORS		2


/*** TMP451 Registers ***/
#define TMP451_LOCAL_TEMP    		0x00   	//Read local(internal)temperature

#define TMP451_TIMEOUT				55000	// 55ms timeout.

void Tmp451Init();
void ReadAllTemps();
#endif /*TEMPCHIPS_*/
