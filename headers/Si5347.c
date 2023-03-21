/*
 * Si5347.c
 *
 *  Created on: Oct 5, 2020
 *      Author: David Edkins
 */

#include <xil_types.h>
#include <unistd.h>
#include <xparameters.h>
#include <xiic.h>
#include "Si5347.h"
#include "HsInterrupt.h"
#include "../HsModule.h"
#include "pios_interface.h"

XIic IicSi5347;

volatile struct {
	int  EventStatus;
	int  RemainingRecvBytes;
	int  RemainingSendBytes;
	int EventStatusUpdated;
	int RecvBytesUpdated;
	int SendBytesUpdated;
} HandlerInfo;


static void SendHandler(void *CallbackRef, int ByteCount)
{
	HandlerInfo.RemainingSendBytes = ByteCount;
	HandlerInfo.SendBytesUpdated = TRUE;
}

static void RecvHandler(void *CallbackRef, int ByteCount)
{
	HandlerInfo.RemainingRecvBytes = ByteCount;
	HandlerInfo.RecvBytesUpdated = TRUE;
}

static void StatusHandler(void *CallbackRef, int Status)
{
	HandlerInfo.EventStatus |= Status;
	HandlerInfo.EventStatusUpdated = TRUE;
}

static void ResetHandler()
{
	HandlerInfo.EventStatusUpdated = FALSE;
	HandlerInfo.RecvBytesUpdated = FALSE;
	HandlerInfo.SendBytesUpdated = FALSE;
}

static void IicInit()
{
	int Result;
	Result = XIic_Initialize(&IicSi5347, XPAR_SI5347_AXI_IIC_0_DEVICE_ID);

	Result = XIic_SelfTest(&IicSi5347);

	XIic_SetSendHandler(&IicSi5347, (void *)&HandlerInfo, SendHandler);
	XIic_SetRecvHandler(&IicSi5347, (void *)&HandlerInfo, RecvHandler);
	XIic_SetStatusHandler(&IicSi5347, (void *)&HandlerInfo, StatusHandler);

	// Connect to Interrupt Controller
	InterruptConnect(XPAR_INTC_0_IIC_0_VEC_ID, XIic_InterruptHandler, &IicSi5347);
	InterruptEnable(XPAR_INTC_0_IIC_0_VEC_ID);

	Result = XIic_Start(&IicSi5347);

	Result = XIic_SetAddress(&IicSi5347, XII_ADDR_TO_SEND_TYPE, SI5347_SLAVE_ADDR);

	ResetHandler();
}

static bool IicWrite(unsigned char regAddr, unsigned char regVal)
{
	int Result;
	unsigned int count = 0;
	u8 txData[2] = {(u8)regAddr, (u8)regVal};
	while (XIic_IsIicBusy(&IicSi5347) != 0)
	{
		usleep(1000);
		count++;
		if(count > 10)	//timeout
			return false;
	}
	Result = XIic_MasterSend(&IicSi5347, txData, 2);
	while (1)
	{
		if (HandlerInfo.EventStatusUpdated)
		{
			ResetHandler();
			return false;
		}
		if(HandlerInfo.SendBytesUpdated && (HandlerInfo.RemainingSendBytes == 0))
		{
			ResetHandler();
			break;
		}
	}
	return (Result == XST_SUCCESS);
}

static bool IicRead(unsigned char regAddr, unsigned char* pRegVal)
{
	int Result;
	unsigned int count = 0;
	u8 txData[1] = {(u8)regAddr};
	while (XIic_IsIicBusy(&IicSi5347) != 0)
	{
		usleep(1000);
		count++;
		if(count > 10)	//timeout
			return false;
	}
	Result = XIic_MasterSend(&IicSi5347, txData, 1);
	while (1)
	{
		if (HandlerInfo.EventStatusUpdated)
		{
			ResetHandler();
			return false;
		}
		if(HandlerInfo.SendBytesUpdated && (HandlerInfo.RemainingSendBytes == 0))
		{
			ResetHandler();
			break;
		}
	}

	while (XIic_IsIicBusy(&IicSi5347) != 0)
	{
		usleep(1000);
		count++;
		if(count > 10)	//timeout
			return false;
	}
	Result = XIic_MasterRecv(&IicSi5347, pRegVal, 1);
	while (1)
	{
		if (HandlerInfo.EventStatusUpdated)
		{
			ResetHandler();
			return false;
		}
		if(HandlerInfo.RecvBytesUpdated && (HandlerInfo.RemainingRecvBytes == 0))
		{
			ResetHandler();
			break;
		}
	}
	return (Result == XST_SUCCESS);
}

static bool Si5347PageSelect(unsigned int regAddr)
{
	unsigned char txData;
	static unsigned char currentPage = 0x0F;

	if(regAddr < 0x100)
		txData = 0x00;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x00);	//select page 0
	else if(regAddr < 0x200)
		txData = 0x01;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x01);	//select page 1
	else if(regAddr < 0x300)
		txData = 0x02;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x02);	//select page 2
	else if(regAddr < 0x400)
		txData = 0x03;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x03);	//select page 3
	else if(regAddr < 0x500)
		txData = 0x04;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x04);	//select page 4
	else if(regAddr < 0x600)
		txData = 0x05;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x05);	//select page 5
	else if(regAddr < 0x700)
		txData = 0x06;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x06);	//select page 6
	else if(regAddr < 0x800)
		txData = 0x07;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x07);	//select page 7
	else if(regAddr < 0x900)
		txData = 0x08;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x08);	//select page 8
	else if(regAddr < 0xA00)
		txData = 0x09;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x09);	//select page 9
	else if(regAddr < 0xB00)
		txData = 0x0A;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x0A);	//select page A
	else if(regAddr < 0xC00)
		txData = 0x0B;	//i2c_master_write_register(MasterControlBase, SI5347, PAGE_SEL_REG, 0x0B);
	else if(regAddr < 0xD00)
		txData = 0x0C;
	else
		txData = 0x0D;

	if(currentPage != txData)
	{
		if (!IicWrite(SI5347_PAGE_SEL_REG, txData))
			return false;
		currentPage = txData;
	}
	return true;
}

static bool Si5347WriteReg(unsigned int regAddr, unsigned char regVal)
{
	//set page based on register addr
	Si5347PageSelect(regAddr);

	//page set. look at LSB only
	regAddr &= 0xFF;
	bool success = IicWrite(regAddr, regVal);
	usleep(100);

	return success;
}

static bool Si5347ReadReg(unsigned int regAddr, unsigned char* pRegVal)
{
	//set page based on register addr
	Si5347PageSelect(regAddr);


	regAddr &= 0xFF;
	bool success = IicRead(regAddr, pRegVal);
	return success;
}

void Si5347Init()
{
	IicInit();
}

bool Si5347Configure(unsigned int numRegs, struct ClockCacheStruct *ClockCache, struct Si5347RegStruct *Si5347RegsCache)
{
	int entryIdx;
	unsigned int regAddr;
	unsigned char regValue;
	unsigned int prevRegAddr;
	unsigned char prevRegValue;
	unsigned char calibrating = 1;
	bool statusOk = 1;
	u8 deviceReady = 0;
	int retry = 0;

	while(deviceReady != SI5347_DEVICE_READY_MSK)
	{
		IicRead(DEVICE_READY_REG, &deviceReady);

		retry++;
		if(retry > 100) //timeout
			return false;
	}

	//step through configuration array
	for(entryIdx = 0; entryIdx < numRegs; entryIdx++)
	{
		regAddr = ClockCache->Si5347Regs[entryIdx].regAddr;
		regValue = ClockCache->Si5347Regs[entryIdx].value;
		prevRegAddr = Si5347RegsCache[entryIdx].regAddr;
		prevRegValue = Si5347RegsCache[entryIdx].value;

		// Reprogram the register if it is in the first 3 or last 5 (calibraiton pre/post amble)
		// If the address is 0xFFFF (invalid address), which is set on init so all regs are programmed
		// Or if the value has changed from the previous config to the current
		if ((entryIdx < SI5347_NUM_PREAMBLE_REGS) | (entryIdx >= numRegs-SI5347_NUM_POSTAMBLE_REGS) | (prevRegAddr == 0xFFFF) | (regValue != prevRegValue) | gpModuleState->Si5347Reset)
		{
			//mask parameter is set to 0xFF because full configuration sets all necessary bits
			statusOk = Si5347WriteReg(regAddr, regValue);
			if(!statusOk)
				return false;
		}

		if((entryIdx == 2) && gpModuleState->Si5347Reset)	//need to wait 300ms for device to stabilize
			usleep(300000);
	}

	//wait for 'device calibrating' to go low
	retry = 0;
	while(calibrating)
	{
		statusOk = Si5347ReadReg(SI5347_STATUS_REG, &calibrating);
		if(!statusOk)
			return false;
		calibrating = calibrating & SI5347_STATUS_SYSINCAL_MSK;

		retry++;
		if(retry > 100) //timeout
			return false;
	}

	//wait for lock status
	retry = 0;
	while(!Si5347LockStatus(false))
	{
		retry++;
		if(retry > 100000) //timeout
			return false;
	}

	//set device reset to false. Configuration completed without issue
	gpModuleState->Si5347Reset = false;

	return true;
}

bool Si5347LockStatus()
{
	unsigned char regStatus, regLOS, regLOL;

	//check internal status bits. should all be low after programming
	Si5347ReadReg(SI5347_STATUS_REG, &regStatus);
	regStatus = regStatus & SI5347_STATUS_ERR_MSK;
	if(regStatus != 0)
	{
		return false;
	}

	/*
	Si5347ReadReg(SI5347_LOS_REG, &regLOS);
	if(regLOS != 0)
	{
		if(flagError)
			gpModuleState->HardwareStatusReg |= ERROR_SYS_CLK_LOS_MASK;
		return false;
	}

	Si5347ReadReg(SI5347_LOL_REG, &regLOL);
	regLOL = regLOL & SI5347_LOL_PLL_MSK;
	if(regLOL != 0)
	{
		return false;
	}
	*/

	return true;
}
