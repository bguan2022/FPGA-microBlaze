/***************************************************************************
* File name   : pios_interface.h
* Description : header file for using the PIOs  
*
* Revision History:	July 04, 2008: Creation, Mohamed Sebbar
*					Sept, 2008: Added RxEnableExtSyncInputMux 
* 					Sept 24, 2008: Added RXSE1-8, FCSE1-2, AUX_CLK_IN1_SE, CHAINR1-2_SE PIO addresses
* 					Sept 25, 2008: Added ATP's SYNC_OUT_BIT1-4 PIO addresses
***************************************************************************/

#ifndef PIOS_INTERFACE_H_
#define PIOS_INTERFACE_H_

#include <xil_types.h>
#include <xparameters.h>
#include <xgpio.h>
#include <xstatus.h>

#define GPIO_DIR_OUT				0
#define GPIO_DIR_IN					0xFFFFFFFF

#define GPIO_CHAN_OUT				1
#define GPIO_CHAN_IN				2

extern XGpio Gpio0;

extern u32 pio0_out_data;
extern u32 pio0_irq_msk;


/*** Outputs ***/
/**** PIO_0 ****/

#define OUT_SI5347_RESET_BASE					&Gpio0
#define OUT_SI5347_RESET_MSK					0x00000001
#define OUT_SI5347_RESET_PREV					pio0_out_data

#define OUT_CLOCK_CHIP_LOCK_BASE				&Gpio0
#define OUT_CLOCK_CHIP_LOCK_MSK					0x00000002
#define OUT_CLOCK_CHIP_LOCK_PREV				pio0_out_data


/**** PIO_0 IN ****/

#define IN_SI5347_LOL_BASE						&Gpio0
#define IN_SI5347_LOL_MSK						0x00000001
#define IN_SI5347_LOL_IRQ						pio0_irq_msk

#define IN_SI5347_LOS_BASE						&Gpio0
#define IN_SI5347_LOS_MSK						0x00000002
#define IN_SI5347_LOS_IRQ						pio0_irq_msk


void pios_init();
void pios_set_value(XGpio* pGpio, u32* out_prev, u32 new_value);
u32 pios_read_value(XGpio* pGpio);
void pios_set_bit(XGpio* pGpio, u32* out_prev, u32 bit_msk);
void pios_clear_bit(XGpio* pGpio, u32* out_prev, u32 bit_msk);
u8 pios_read_bit(XGpio* pGpio, u32 bit_msk);
void pios_set_irq(XGpio* pGpio, u32 bit_msk, u32* irq_msk);
//static void PIO_0_ISR(void* context, u32 id);

#endif /*PIOS_INTERFACE_H_*/
