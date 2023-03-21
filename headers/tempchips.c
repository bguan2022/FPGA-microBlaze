/***************************************************************************
 * File name   : tempchips.c
 * Description : functions for communicating with temperature chips
 *
 * Revision History:   June 20, 2008: Creation, Mohamed Sebbar
 *
 ***************************************************************************/

//#include <io.h>
//#include <unistd.h>
#include <stdbool.h>
#include <xil_types.h>
#include <xiic.h>
#include "../system.h"
#include "tempchips.h"
#include "../HsModule.h"
#include "HsInterrupt.h"
#include "HsTimer.h"

XIic IicTemp451;

volatile struct {
	int  EventStatus;
	int  RemainingRecvBytes;
	int  RemainingSendBytes;
	int EventStatusUpdated;
	int RecvBytesUpdated;
	int SendBytesUpdated;
} HandlerTempInfo;

static void SendHandler(void *CallbackRef, int ByteCount)
{
	HandlerTempInfo.RemainingSendBytes = ByteCount;
	HandlerTempInfo.SendBytesUpdated = TRUE;
}

static void RecvHandler(void *CallbackRef, int ByteCount)
{
	HandlerTempInfo.RemainingRecvBytes = ByteCount;
	HandlerTempInfo.RecvBytesUpdated = TRUE;
}

static void StatusHandler(void *CallbackRef, int Status)
{
	HandlerTempInfo.EventStatus |= Status;
	HandlerTempInfo.EventStatusUpdated = TRUE;
}

static void ResetHandler()
{
	HandlerTempInfo.EventStatusUpdated = FALSE;
	HandlerTempInfo.RecvBytesUpdated = FALSE;
	HandlerTempInfo.SendBytesUpdated = FALSE;
}

static void IicInit()
{
	int Result;
	Result = XIic_Initialize(&IicTemp451, XPAR_TEMP_AXI_IIC_0_DEVICE_ID);

	Result = XIic_SelfTest(&IicTemp451);

	XIic_SetSendHandler(&IicTemp451, (void *)&HandlerTempInfo, SendHandler);
	XIic_SetRecvHandler(&IicTemp451, (void *)&HandlerTempInfo, RecvHandler);
	XIic_SetStatusHandler(&IicTemp451, (void *)&HandlerTempInfo, StatusHandler);

	// Connect to Interrupt Controller
	InterruptConnect(XPAR_INTC_0_IIC_1_VEC_ID, XIic_InterruptHandler, &IicTemp451);
	InterruptEnable(XPAR_INTC_0_IIC_1_VEC_ID);

	Result = XIic_Start(&IicTemp451);

	Result = XIic_SetAddress(&IicTemp451, XII_ADDR_TO_SEND_TYPE, TMP451);

	ResetHandler();
}

static bool IicRead(unsigned char regAddr, unsigned char* pRegVal)
{
	int Result;
	unsigned int count = 0;
	u8 txData[1] = {(u8)regAddr};
	while (XIic_IsIicBusy(&IicTemp451) != 0)
	{
		usleep(1000);
		count++;
		if(count > 10)	//timeout
			return false;
	}
	Result = XIic_MasterSend(&IicTemp451, txData, 1);
	while (1)
	{
		if (HandlerTempInfo.EventStatusUpdated)
		{
			ResetHandler();
			return false;
		}
		if(HandlerTempInfo.SendBytesUpdated && (HandlerTempInfo.RemainingSendBytes == 0))
		{
			ResetHandler();
			break;
		}
	}

	while (XIic_IsIicBusy(&IicTemp451) != 0)
	{
		usleep(1000);
		count++;
		if(count > 10)	//timeout
			return false;
	}
	Result = XIic_MasterRecv(&IicTemp451, pRegVal, 1);
	while (1)
	{
		if (HandlerTempInfo.EventStatusUpdated)
		{
			ResetHandler();
			return false;
		}
		if(HandlerTempInfo.RecvBytesUpdated && (HandlerTempInfo.RemainingRecvBytes == 0))
		{
			ResetHandler();
			break;
		}
	}
	return (Result == XST_SUCCESS);
}

void Tmp451Init()
{
	IicInit();
}


//returns remote temp in high precision. used for temp loop control
void ReadAllTemps()
{
	//read local kintex
	u8 tempHigh;
	IicRead(TMP451_LOCAL_TEMP, &tempHigh);
	gpModuleState->TemperatureSensorReadings = tempHigh;		//store local temp
}

//------------------------------------------------------------------------------------------------------------------------------------------
