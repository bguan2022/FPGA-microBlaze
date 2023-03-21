//--HsInterrupt.c---------------------------------------------------------------------------------------------------------------------------
#include <xparameters.h>
#include <xstatus.h>
#include <xintc.h>
#include "HsInterrupt.h"

static XIntc InterruptController;

void InterruptInit()
{
	int Status;
	Status = XIntc_Initialize(&InterruptController, XPAR_AXI_INTC_0_DEVICE_ID);

	Status = XIntc_SelfTest(&InterruptController);

	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				 (Xil_ExceptionHandler) XIntc_InterruptHandler,
				 &InterruptController);

	Xil_ExceptionEnable();
}

void InterruptConnect(u8 IntrSrcId, XInterruptHandler Handler, void *CallBackRef)
{
	int Status;
	Status = XIntc_Connect(&InterruptController, IntrSrcId,
							Handler, CallBackRef);
}

void InterruptEnable(u8 IntrSrcId)
{
	XIntc_Enable(&InterruptController, IntrSrcId);
}

void InterruptDisable(u8 IntrSrcId)
{
	XIntc_Disable(&InterruptController, IntrSrcId);
}

