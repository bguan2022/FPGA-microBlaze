//--HsXcvrControl.h-------------------------------------------------------------------------------------------------------------------------
// XcvrControls component driver
// Revision History:		Aug 2008:	Creation, Donald Dansereau
//------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __HSXCVR_CONTROL_H_
#define __HSXCVR_CONTROL_H_

//------------------------------------------------------------------------------------------------------------------------------------------
#include <stdbool.h>
#include "HsXcvrControlRegs.h"
#include "../CommonDefines.h"
#include "../HsModuleDefines.h"

//----Common--------------------------------------------------------------------------------------------------------------------------------
unsigned int XcvrPreCursorRegMap(unsigned char chanIdx);
unsigned int XcvrPostCursorRegMap(unsigned char chanIdx);
unsigned int XcvrDiffRegMap(unsigned char chanIdx);
unsigned int XcvrMainCursorRegMap(unsigned char chanIdx);

//--TX Source Pattern-----------------------------------------------------------------------------------------------------------------------
enum ePatternIndex XcvrGetSourcePattern( unsigned int TxIdx );

typedef enum LpmStates {
	ADAPT    = 0b00,
	FREEZE   = 0b10,
	OVERRIDE = 0b11
} LpmStates;

//--Xcvr Reset Sequences--------------------------------------------------------------------------------------------------------------------
void XcvrFullReset( unsigned int XcvrControlBase );

//----VOD Set/Get macros--------------------------------------------------------------------------------------------------------------------
void XcvrSetVoltageSwingControl(unsigned char ChanIdx, unsigned int NewValVod, unsigned int NewValMainCursor);

//--Preemphasis control---------------------------------------------------------------------------------------------------------------------
void XcvrSetTxMainCursor( unsigned char ChanIdx, unsigned int vodIdx );
void XcvrSetPreemphasis( const struct PreemphasisSettingStruct* pPreemphSettings, unsigned int ChanIdx );
void XcvrSetPreemphasisGrp( const struct PreemphasisSettingStruct* pPreemphSettings, unsigned int NumChannels, unsigned char ChannelMask );
void XcvrGetPreemphasis( unsigned char ChannelIdx, struct PreemphasisSettingStruct* pPreempasisSettingOut );
bool XcvrPreemphasisIsValid( const struct PreemphasisSettingStruct* pPreemphasisSettings );

//----LPM/DFE-------------------------------------------------------------------------------------------------------------------------------
void XcvrSetLpmMode();
void XcvrSetDfeMode();
void XcvrLpmDfeReset();
void XcvrCtleRunAutoAdapt(unsigned char channelMask);
void XcvrCtleSetKlGain(unsigned int klGain, unsigned char chanIdx);
void XcvrCtleSetKhGain(unsigned int khGain, unsigned char chanIdx);
void XcvrCtleSetOsGain(unsigned int osGain, unsigned char chanIdx);
void XcvrEqSetAgcGain(unsigned int gcGain, unsigned char chanIdx);

#endif /*__HSXCVR_CONTROL_H_*/
//------------------------------------------------------------------------------------------------------------------------------------------
