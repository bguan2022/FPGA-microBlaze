/***************************************************************************
 * File name   : pios_interface.c
 * Description : This file contains functions for doing operations on pio bits. 
 *
 * Revision History:   July 02, 2008:  Creation, Mohamed Sebbar
 * 
 ***************************************************************************/
#include "pios_interface.h"

#ifdef DEBUG_PIOS
#include <stdio.h>
#define PioPrintf(...) printf(__VA_ARGS__)
#else
#define PioPrintf(...)    // does nothing if DEBUG_PIOS not defined
#endif

XGpio Gpio0;

//global variables for PIO_0
u32 pio0_out_data = 0;
u32 pio1_out_data = 0;
u32 pio0_irq_msk = 0;


/*
 * Function name: pio_0_init
 * Parameters:   none
 * Return value: none
 * Description:  Initializes the pio_0.
 *               Must be called at the begining of Main.
 */
void pios_init()
{
	int Result;

	// GPIO 0
	Result = XGpio_Initialize(&Gpio0, XPAR_GPIO_0_DEVICE_ID);
	if(Result != XST_SUCCESS) {
		return;
	}
	XGpio_SelfTest(&Gpio0);

	XGpio_SetDataDirection(&Gpio0, GPIO_CHAN_OUT, GPIO_DIR_OUT);
	XGpio_SetDataDirection(&Gpio0, GPIO_CHAN_IN, GPIO_DIR_IN);

	pio0_out_data = 0;
	pios_set_value(&Gpio0, &pio0_out_data, pio0_out_data);
}

/*
* Function name: pios_set_value
* Parameters:   32 pio_base : PIO core base address
*               32* out_prev : variable containing previous PIO output value
*               32 new_value  : bit number
* Return value: none
* Description:  Sets a PIO output, all 32 bits in Parallel
*/
void pios_set_value(XGpio* pGpio, u32* out_prev, u32 new_value)
{
	*out_prev = new_value;
	XGpio_DiscreteWrite(pGpio, GPIO_CHAN_OUT, *out_prev);
}


/*
* Function name: pios_read_value
* Parameters:   32 pio_base : PIO core base address
*               32* out_prev : variable containing previous PIO output value
*               32 new_value  : bit number
* Return value: none
* Description:  Read a PIO input, all 32 bits in Parallel
*/
u32 pios_read_value(XGpio* pGpio)
{
    u32 value;

    value = XGpio_DiscreteRead(pGpio, GPIO_CHAN_IN);

    return value;
}

/*
 * Function name: pios_set_bit
 * Parameters:   32 pio_base : PIO core base address
 *               32* out_prev : variable containing previous PIO output value
 *               32 bit_msk  : bit number
 * Return value: none
 * Description:  Sets a PIO output bit.
 * Exemple for setting the output "OUT_SL1_RSTN":
 *               pios_set_bit(OUT_SL1_RSTN_BASE, &OUT_SL1_RSTN_PREV, OUT_SL1_RSTN_MSK);
 *               OUT_STAUS_OK_BASE, OUT_STAUS_OK_PREV and OUT_STAUS_OK_MSK are defined in "pios_interface.h"
 */
void pios_set_bit(XGpio* pGpio, u32* out_prev, u32 bit_msk)
{
    *out_prev |= bit_msk;
    XGpio_DiscreteWrite(pGpio, GPIO_CHAN_OUT, *out_prev);
}


/*
 * Function name: pios_clear_bit
 * Parameters:   32 pio_base : PIO core base address
 *               32* out_prev : variable containing previous PIO output value
 *               32 bit_msk  : bit number
 * Return value: none
 * Description:  Clears a PIO output bit.
 * Exemple for clearing the output "STAUS_OK":
 *               pios_clear_bit(OUT_STAUS_OK_BASE, &OUT_STAUS_OK_PREV, OUT_STAUS_OK_MSK);
 *               OUT_STAUS_OK_BASE, OUT_STAUS_OK_PREV and OUT_STAUS_OK_MSK are defined in "pios_interface.h"
 */
void pios_clear_bit(XGpio* pGpio, u32* out_prev, u32 bit_msk)
{
    *out_prev &= ~bit_msk;
    XGpio_DiscreteWrite(pGpio, GPIO_CHAN_OUT, *out_prev);
}


/*
 * Function name: pios_read_bit
 * Parameters:   32 pio_base : PIO core base address
 *               32 bit_msk  : bit number
 * Return value: 8 : input bit value
 * Description:  Reads and returns the value of a PIO input bit.
 * Exemple for reading the input "TRIG_IN1":
 *               8 trig_in1_value = pios_read_bit(IN_TRIG_IN1_BASE, IN_TRIG_IN1_MSK);
 *               IN_TRIG_IN1_BASE and IN_TRIG_IN1_MSK are defined in "pios_interface.h"
 */
u8 pios_read_bit(XGpio* pGpio, u32 bit_msk)
{
    u32 value;
    
    value = XGpio_DiscreteRead(pGpio, GPIO_CHAN_IN);
    
    return ((value & bit_msk) == bit_msk);
}


/*
 * Function name: pios_irq
 * Parameters:   32 pio_base : PIO core base address
 *               32 bit_msk  : input bit to which activate IRQ
 *               32* irq_msk : irq mask
 * Return value: none
 * Description:  Activates IRQ for PIO input bit.
 * Exemple for setting the output "STAUS_OK":
 *               pios_set_bit(IN_TRIG_IN1_BASE, IN_TRIG_IN1_MSK, &IN_TRIG_IN1_IRQ);
 *               IN_TRIG_IN1_BASE, IN_TRIG_IN1_MSK and IN_TRIG_IN1_IRQ are defined in "pios_interface.h"
 */
void pios_set_irq(XGpio* pGpio, u32 bit_msk, u32* irq_msk)
{
    *irq_msk |= bit_msk;
    //XGpio_DiscreteWrite(&Gpio, GPIO_CHAN_OUT, *out_prev);
    //XIo_Out32(pio_base, *irq_msk);	//IOWR_ALTERA_AVALON_PIO_IRQ_MASK(pio_base, *irq_msk);
}


/*
 * Function name: PIO_0_ISR
 * Parameters:   void* context : pointer that can be used to pass context-specific information to
 *                               the ISR
 *               32 id    : hardware interrupt number for the device
 * Return value: none
 * Description:  ISR routine for PIO_0 IRQ.
 */
/*static void PIO_0_ISR(void* context, 32 id)
{
    volatile 32 interrupt_src;

    //get interrupt source
    interrupt_src = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE);
    
    //reset the edge capture register
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0);

    switch (interrupt_src)
    {
        case IN_TEMP1_OTB_MSK:
            //PioPrintf("TEMP1_OTB interrupt ...\n");
            break;
        case IN_TEMP2_THERMB_MSK:
            //PioPrintf("TEMP2_THERMB interrupt ...\n");
            break;
        case IN_TEMP2_ALERTB_MSK:
            //PioPrintf("TEMP2_ALERTB interrupt ...\n");
            break;
        case IN_PG_1V2A_MSK:
            //PioPrintf("PG_1V2A interrupt ...\n");
            break;
         case IN_PG_2V5_MSK:
            //PioPrintf("PG_2V5 interrupt ...\n");
            break; 
        default:
            ;//PioPrintf("Unkown Source of Interrupt ...\n");
    }
}
*/
