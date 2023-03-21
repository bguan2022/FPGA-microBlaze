//--HSModule.c------------------------------------------------------------------------------------------------------------------------------
#include <string.h>
#include "system.h"
#include <xparameters.h>
#include <malloc.h>
#include "HsModule.h"
#include "Drivers/HsCalFlash.h"
#include "Drivers/pios_interface.h"
#include "HsClockControl.h"
#include "Drivers/tempchips.h"
#include "Drivers/HsTimer.h"
#include "Drivers/HsInterrupt.h"

#ifdef DFT_DEBUG
#include <stdio.h>
#endif

//------------------------------------------------------------------------------------------------------------------------------------------
// DJ60HS Command Flow
// Reference: Command Interface Design Doc
// (svn://kramer/projects/DJ60HS/engineering documents/Embedded HLD/CommandInterface/CommandInterfaceDesign.pdf)
//
// The main loop is responsible for housekeeping and polling tasks, like periodically going out and checking on temperature sensors.
// It maintains a "register memory", which always reflects the current state of the module.
// The main loop gets interrupted by the SPI DMA controller as transfers complete.
// Because of this, non-atomic modifications to the register memory must be guarded against interruption, like this:
//		DisableInterrupt();
//		 ChangeSomeRegisterValues();
//		 ChangeMoreRegisterValues();
//		EnableInterrupt();
//
// Guarded sections must be kept to an absolute minimum, and must be very, very short.  As a general rule, only the main loop is allowed
// to use guarded sections, as only the main loop will be modifying the register memory.
//
// Transactions follow one of a few different models, which have the main loop interacting with the SPI DMA ISR:
// 1) Immediate register transfer: 2 transfers
//		[!busy]
//		- Main loop interrupted when first transfer (instruction) completes
//			- ISR: busy
//			- ISR queues up the second transfer, including register read values
//			- ISR: !busy
//		- Main loop interrupted when second transfer completes
//			- ISR: busy
//			- ISR queues up to receive the next instruction
//			- ISR: !busy
//		- Main loop continues
// 2) Immediate data transfer: 3 transfers
//		- !busy
//		- Main loop interrupted when first transfer (instruction) completes
//			- ISR: busy
//			- ISR queues up the second transfer (data)
//			- ISR: !busy
//		- Main loop interrupted when second transfer completes
//			- ISR: busy
//			- ISR computes and queues up the third transfer (CRC)
//			- ISR: !busy
//		- Main loop interrupted when third transfer completes
//			- ISR: busy
//			- ISR queues up to receive the next instruction
//			- ISR: !busy
//		- Main loop continues
// 3) Extended, busy, register operation: 2 transfers and a wait
//		- !busy
//		- Main loop interrupted when first transfer (instruction) completes
//			- ISR: busy
//			- ISR queues up the second transfer (Ack / reg value)
//			- ISR: !busy
//		- Main loop interrupted when second transfer completes
//			- ISR: busy
//			- ISR performs the busy operation (busy wait - SPI reporting busy, ignoring incoming data)
//			- ISR queues up to receive the next instruction
//			- ISR: !busy
//		- Main loop continues
// 3) Extended, busy, data operation: 3 transfers and a wait
//		- busy
//		- Main loop interrupted when first transfer (instruction) completes
//			- ISR: busy
//			- ISR queues up the second transfer (data)
//			- ISR: !busy
//		- Main loop interrupted when second transfer completes
//			- ISR: busy
//			- ISR computes and queues up the third transfer (CRC)
//			- ISR: !busy
//		- Main loop interrupted when third transfer completes
//			- ISR: busy
//			- ISR performs the busy operation (busy wait - SPI reporting busy, ignoring incoming data)
//			- ISR queues up to receive the next instruction
//			- ISR: !busy
//		- Main loop continues
//
// In the documentation, an extended operation with feedback is shown as being distinct from one without.  In terms of implementation,
// the feedback is already handled by accepting a new command, which will be a read result command.
//
// Although it's not in the document, it's possible to extend this processor for background (non-busy extended) operations.  For example:
// 4) Extended, background, register operation: 2 transfers and a main-loop process
//		- busy
//		- Main loop interrupted when first transfer (instruction) completes
//			- ISR: busy
//			- ISR queues up the second transfer (Ack / reg value)
//			- ISR: !busy
//		- Main loop interrupted when second transfer completes
//			- ISR: busy
//			- ISR requests the extended operation (no wait)
//			- ISR queues up to receive the next instruction
//			- ISR: !busy
//		- Main loop continues
//		- Main loop eventually comes to its check for enqueued operations, and performs the requested extended operation
//		- The extended operation completes, and the main loop continues.
//
// The advantage of this last model, and its data (as opposed to register) brother, is that the module can keep processing commands even
// while the main loop is busy with the extended operation.  It's not clear when we'll start using this, but the option is there.
// Care must be taken that a repeated request, or one that interferes with the extended operation, be dealt with appropriately.  For example,
// if a very lengthy single-shot BERT is implemented as an extended operation, a repeated request to perform a BERT on the same HW must
// either interrupt the first, or return an error.
//------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------------------
struct ModuleStateStruct* gpModuleState = 0;
struct UserStateStruct* gpUserState = 0;

//------------------------------------------------------------------------------------------------------------------------------------------
void ModuleRun()
{
	//--The main loop-----------------------------------------------------------------------------------------------------------------------
	while( 1 )
	{
		ReadAllTemps();
		ClockSetSi5338Si5347LockStatus();
		usleep( 1000000 );
	}
	//--...end main loop--------------------------------------------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ModuleInit()
{

	//--one-time memory allocation------------------------------------------------------------------------------------------------------
	gpModuleState = (struct ModuleStateStruct*) malloc( sizeof(struct ModuleStateStruct) );
	gpUserState = (struct UserStateStruct*) malloc( sizeof(struct UserStateStruct) );
	memset( gpModuleState, 0, sizeof( struct ModuleStateStruct ) );

	InterruptInit();

	//--MoPio--Check Tx Ready---------------------------------------------------------------------------------------------------------------
	pios_init();

	TimerInit();

	//--Temp Chip Init----------------------------------------------------------------------------------------------------------------------
	Tmp451Init();

	//--Reset the global user state-------------------------------------------------------------------------------------------------------------
	memset(gpUserState, 0, sizeof(struct UserStateStruct));

	gpModuleState->MaxDataRate = CLOCK_MAX_DATA_RATE;

	//--Clock init--------------------------------------------------------------------------------------------------------------------------
	ClockControlInit();

	ReadAllTemps();
	ClockSetSi5338Si5347LockStatus();

}
