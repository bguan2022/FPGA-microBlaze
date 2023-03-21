//--HsTimer.c-------------------------------------------------------------------------------------------------------------------------------
#include <xparameters.h>
#include <xstatus.h>
#include <xtmrctr.h>
#include "HsTimer.h"

static XTmrCtr SysTimer;

//------------------------------------------------------------------------------------------------------------------------------------------
void TimerInit()
{
	int Result;

	Result = XTmrCtr_Initialize(&SysTimer, XPAR_AXI_TIMER_0_DEVICE_ID);

	Result = XTmrCtr_SelfTest(&SysTimer, 0);
}

void usleep(unsigned int useconds)
{
	unsigned long long tickSleep = useconds * TIMER_US_TO_TICK;
	XTmrCtr_SetResetValue(&SysTimer, 0, 0);
	XTmrCtr_Start(&SysTimer, 0);
	unsigned int tick = XTmrCtr_GetValue(&SysTimer,0);

	while( tick < tickSleep ){
		tick = XTmrCtr_GetValue(&SysTimer,0);
	}
}

void tickSleep(unsigned int tickSleep)
{
	XTmrCtr_SetResetValue(&SysTimer, 0, 0);
	XTmrCtr_Start(&SysTimer, 0);
	unsigned int tick = XTmrCtr_GetValue(&SysTimer,0);

	while( tick < tickSleep ){
		tick = XTmrCtr_GetValue(&SysTimer,0);
	}
}

void TimerStart()
{
	XTmrCtr_SetResetValue(&SysTimer, 1, 0);
	XTmrCtr_Start(&SysTimer, 1);
}

void TimerStop()
{
	XTmrCtr_Stop(&SysTimer, 1);
}

unsigned int TimerGetValue()
{
	return XTmrCtr_GetValue(&SysTimer,1);
}

unsigned int TimerStopAndGetValue()
{
	unsigned int tick = TimerGetValue();
	TimerStop();
	return tick;
}
