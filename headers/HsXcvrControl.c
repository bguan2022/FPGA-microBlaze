//--HsXcvrControl.c-------------------------------------------------------------------------------------------------------------------------
//#include <unistd.h>
#include <string.h>
#include "HsXcvrControlRegs.h"
#include "HsXcvrControl.h"
#include "../HsModuleDefines.h"
#include "../HsModule.h"
#include "pios_interface.h"
#include "HsTimer.h"
#include "math.h"

//------------------------------------------------------------------------------------------------------------------------------------------
unsigned int XcvrPreCursorRegMap(unsigned char chanIdx)
{
	unsigned int regAddr;
	switch (chanIdx)
	{
		case 0:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG0;
			break;
		case 1:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG1;
			break;
		case 2:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG2;
			break;
		case 3:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG3;
			break;
		case 4:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG4;
			break;
		case 5:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG5;
			break;
		case 6:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG6;
			break;
		case 7:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG7;
			break;
		default:
			regAddr = XCVR_CTRL_TX_PRE_CURSOR_REG0;
			break;
	}

	return regAddr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
unsigned int XcvrPostCursorRegMap(unsigned char chanIdx)
{
	unsigned int regAddr;
	switch (chanIdx)
	{
		case 0:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG0;
			break;
		case 1:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG1;
			break;
		case 2:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG2;
			break;
		case 3:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG3;
			break;
		case 4:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG4;
			break;
		case 5:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG5;
			break;
		case 6:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG6;
			break;
		case 7:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG7;
			break;
		default:
			regAddr = XCVR_CTRL_TX_POST_CURSOR_REG0;
			break;
	}

	return regAddr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
unsigned int XcvrDMonitorRegMap(unsigned char chanIdx)
{
	unsigned int regAddr;
	switch (chanIdx)
	{
		case 0:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG0;
			break;
		case 1:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG1;
			break;
		case 2:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG2;
			break;
		case 3:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG3;
			break;
		case 4:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG4;
			break;
		case 5:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG5;
			break;
		case 6:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG6;
			break;
		case 7:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG7;
			break;
		default:
			regAddr = XCVR_CTRL_DMONITOR_OUT_REG0;
			break;
	}
	return regAddr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
unsigned int XcvrDiffRegMap(unsigned char chanIdx)
{
	unsigned int regAddr;
	switch (chanIdx)
	{
		case 0:
			regAddr = XCVR_CTRL_TX_DIFF_REG0;
			break;
		case 1:
			regAddr = XCVR_CTRL_TX_DIFF_REG1;
			break;
		case 2:
			regAddr = XCVR_CTRL_TX_DIFF_REG2;
			break;
		case 3:
			regAddr = XCVR_CTRL_TX_DIFF_REG3;
			break;
		case 4:
			regAddr = XCVR_CTRL_TX_DIFF_REG4;
			break;
		case 5:
			regAddr = XCVR_CTRL_TX_DIFF_REG5;
			break;
		case 6:
			regAddr = XCVR_CTRL_TX_DIFF_REG6;
			break;
		case 7:
			regAddr = XCVR_CTRL_TX_DIFF_REG7;
			break;
		case 8:
			regAddr = XCVR_CTRL_TX_DIFF_REG8;
			break;
		case 9:
			regAddr = XCVR_CTRL_TX_DIFF_REG9;
			break;
		default:
			regAddr = XCVR_CTRL_TX_DIFF_REG0;
			break;
	}

	return regAddr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void XcvrControlRMW(unsigned int xcrControlBase, unsigned short controlAddr, unsigned int newVal, unsigned int mask )
{
	u32 oldVal = XcvrControlGetReg( (xcrControlBase), (controlAddr) );
	XcvrControlSetReg( (xcrControlBase), (controlAddr), ( ( (newVal) & (mask) ) | ( oldVal & (~mask) ) ) );
	oldVal = XcvrControlGetReg( (xcrControlBase), (controlAddr) );
}

//--Xcvr Reset Sequences--------------------------------------------------------------------------------------------------------------------
void XcvrFullReset(unsigned int XcvrControlBase)
{
	// Does tx/rx full resets and QPLL0/QPLL1 resets
	u8 resetDone;
	XcvrControlRMW(XcvrControlBase, XCVR_CTRL_RESET_STATUS_CTRL_REG, XCVR_CTRL_RESET_START, XCVR_CTRL_RESET_START);

	do
	{
		resetDone = XcvrControlGetReg(XcvrControlBase, XCVR_CTRL_RESET_STATUS_CTRL_REG) & XCVR_CTRL_RESET_DONE;
	} while (!resetDone);

	usleep(5000);
}

//----VOD Set/Get macros--------------------------------------------------------------------------------------------------------------------
void XcvrSetVoltageSwingControl(unsigned char ChanIdx, unsigned int NewValVod, unsigned int NewValMainCursor)
{
	XcvrControlRMW( XPAR_AXI_CONTROL_REGS_BASEADDR,
				XcvrDiffRegMap(ChanIdx),
				( NewValMainCursor << XCVR_CTRL_TX_MAIN_CURSOR_CTRL_SHIFT) | ( NewValVod ),
				XCVR_CTRL_TX_DRIVER_CTRL_MASK | XCVR_CTRL_TX_MAIN_CURSOR_CTRL_MASK);
}

//----Pre-emphasis Functions----------------------------------------------------------------------------------------------------------------
void XcvrSetPreemphasis( const struct PreemphasisSettingStruct* pPreemphSettings, unsigned int ChanIdx )
{
	XcvrControlRMW( XPAR_AXI_CONTROL_REGS_BASEADDR,
				XcvrPreCursorRegMap(ChanIdx),
				( pPreemphSettings->PreemphPreTap ),
				XCVR_CTRL_TX_DRIVER_CTRL_MASK );
	XcvrControlRMW( XPAR_AXI_CONTROL_REGS_BASEADDR,
				XcvrPostCursorRegMap(ChanIdx),
				( pPreemphSettings->PreemphPostTap1 ),
				XCVR_CTRL_TX_DRIVER_CTRL_MASK );
}

//------------------------------------------------------------------------------------------------------------------------------------------
void XcvrSetPreemphasisGrp( const struct PreemphasisSettingStruct* pPreemphSettings, unsigned int NumChannels, unsigned char ChannelMask )
{
	unsigned int iChan;
	unsigned int iChanMask = 1;
	for( iChan = 0; iChan<NumChannels; ++iChan, iChanMask <<= 1 )
		if( ChannelMask & iChanMask )
			XcvrSetPreemphasis( pPreemphSettings, iChan );
}

//------------------------------------------------------------------------------------------------------------------------------------------
void XcvrGetPreemphasis(unsigned char ChannelIdx, struct PreemphasisSettingStruct* pPreempasisSettingOut )
{
	unsigned int preCursorValue = XcvrControlGetReg( XPAR_AXI_CONTROL_REGS_BASEADDR, XcvrPreCursorRegMap(ChannelIdx));
	unsigned int postCursorValue = XcvrControlGetReg( XPAR_AXI_CONTROL_REGS_BASEADDR, XcvrPostCursorRegMap(ChannelIdx));
	pPreempasisSettingOut->PreemphPreTap = (preCursorValue & XCVR_CTRL_TX_DRIVER_CTRL_MASK);
	pPreempasisSettingOut->PreemphPostTap1 = (postCursorValue & XCVR_CTRL_TX_DRIVER_CTRL_MASK);
}

//----LPM/DFE-------------------------------------------------------------------------------------------------------------------------------
void XcvrSetCtleState(LpmStates state, unsigned int reg, unsigned char channelMask)
{
	unsigned char iChan;
	unsigned int result = XcvrControlGetReg(XPAR_AXI_CONTROL_REGS_BASEADDR, reg);

	for (iChan = 0; iChan < MODULE_NUM_CHANNELS; ++iChan)
	{
		int mask = 0b11 << (iChan * 2);
		if (channelMask & (1 << iChan)) {
			result =  (result & ~mask) | (state << (iChan * 2));
		}
	}
	XcvrControlSetReg(XPAR_AXI_CONTROL_REGS_BASEADDR, reg, result);
}

unsigned int XcvrGetLpmState(unsigned int reg)
{
	return (unsigned int) XcvrControlGetReg(XPAR_AXI_CONTROL_REGS_BASEADDR, reg);
}

void XcvrSetLpmMode()
{
	// 0 DFE, 1 LPM
	XcvrControlRMW(XPAR_AXI_CONTROL_REGS_BASEADDR, XCVR_CTRL_RESET_STATUS_CTRL_REG, XCVR_CTRL_RXLPMEN_MASK, XCVR_CTRL_RXLPMEN_MASK);
}

void XcvrSetDfeMode()
{
	// 0 DFE, 1 LPM
	XcvrControlRMW(XPAR_AXI_CONTROL_REGS_BASEADDR, XCVR_CTRL_RESET_STATUS_CTRL_REG, XCVR_CTRL_RXLPMEN_MASK, 0);
}

void XcvrLpmDfeReset()
{
	XcvrControlRMW(XPAR_AXI_CONTROL_REGS_BASEADDR, XCVR_CTRL_RESET_STATUS_CTRL_REG, XCVR_CTRL_LPMDFERESET_START, XCVR_CTRL_LPMDFERESET_START);
}


void XcvrCtleRunAutoAdapt(unsigned char channelMask)
{
	XcvrSetCtleState(ADAPT, XCVR_CTRL_LPM_KH_STATE_REG, channelMask);
	XcvrSetCtleState(ADAPT, XCVR_CTRL_LPM_KL_STATE_REG, channelMask);
	XcvrSetCtleState(ADAPT, XCVR_CTRL_LPM_OS_STATE_REG, channelMask);
	XcvrSetCtleState(OVERRIDE, XCVR_CTRL_LPM_GC_STATE_REG, channelMask);

	XcvrSetLpmMode(channelMask); //Set to lpm
	XcvrLpmDfeReset(channelMask);
}
