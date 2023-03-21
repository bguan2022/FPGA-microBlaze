//--HsModule.h------------------------------------------------------------------------------------------------------------------------------
// DJ60HS Module Control
// Revision History:		Jul 2008:	Creation, Donald Dansereau
// This is the top level of the module SW - it contains main loop
// See implementation for description of command flow
//------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __HSMODULE_H_
#define __HSMODULE_H_

//------------------------------------------------------------------------------------------------------------------------------------------
#include "HsModuleDefines.h"
#include "CommonDefines.h"
#include "HsClockControl.h"
#include "Drivers/HsXcvrControl.h"
#include "Drivers/tempchips.h"

//------------------------------------------------------------------------------------------------------------------------------------------
struct UserStateStruct
{
	//*** the stuff at the start is NOT reset by a Soft Reset

	//--Clock Configuration control----------------------------------------------------------------------------------------------------------
	unsigned long long DataRate;							// user-provided data rate, set when the user accesses the Commit Clock register. Frequency expressed in increments of 0.1 Hz, stored in 64-bits.
	unsigned int TxPllRatio;
	unsigned int RefClockFrequency_kHz;					// user-provided reference clock frequency, set when user accesses the Commit Clock register. Expressed in kHz, stored in 32 bits.
	unsigned int RefClkPreviousFreq_kHz;
	enum eRefClockMode RefClockMode;
	unsigned int ClkOut1Freq_kHz;
	unsigned int ClkOut2Freq_kHz;
	enum eClkOutMode ClkOut1Mode;
	enum eClkOutMode ClkOut2Mode;
	unsigned int ClkValidateResult;
	unsigned char ClkRegResult;
	unsigned char Clk5338RegResult;
	//*** from this point forward, content is zero'd by Soft Reset

	//--Module Control----------------------------------------------------------------------------------------------------------------------
	unsigned int GlobalStatusMask;

	unsigned int RxCtleKlGain[MODULE_NUM_CHANNELS];
	unsigned int RxCtleKhGain[MODULE_NUM_CHANNELS];
	unsigned int RxCtleOsGain[MODULE_NUM_CHANNELS];
	unsigned int RxCtleGcGain[MODULE_NUM_CHANNELS];
	unsigned int RxCommonMode[MODULE_NUM_CHANNELS];
	unsigned int RxTermMode[MODULE_NUM_CHANNELS];
	unsigned int RxCtleDacSampler[MODULE_NUM_CHANNELS];

	//--Tx control--------------------------------------------------------------------------------------------------------------------------
	struct PreemphasisSettingStruct PreEmphSettings[MODULE_NUM_CHANNELS];

	//--Amplitude control-------------------------------------------------------------------------------------------------------------------
	unsigned int TxAmpRequested_uV[ MODULE_NUM_CHANNELS ];
	unsigned int TxMainCursor[ MODULE_NUM_CHANNELS ];
	unsigned int TxVod[ MODULE_NUM_CHANNELS ];

	//--Diagnostics-------------------------------------------------------------------------------------------------------------------------
	unsigned int HardwareStatusMask;

};

//------------------------------------------------------------------------------------------------------------------------------------------
struct ModuleStateStruct
{
    //--Temperature--------------------------------------------------------------------------------------------------------------------------
	int TemperatureSensorReadings;

	//--Cal Data-----------------------------------------------------------------------------------------------------------------------------
	unsigned long long MaxDataRate;

    //--TEMP DEBUG
    unsigned int configRetryCnt;
    unsigned int configDeviceReadyCnt;
    unsigned int configLockCnt;
    unsigned int configCalibratingCnt;
    bool Si5347Reset;
};

extern struct ModuleStateStruct* gpModuleState;
extern struct UserStateStruct* gpUserState;


//------------------------------------------------------------------------------------------------------------------------------------------
void ModuleRun();
void ModuleInit();

#ifdef PERSONALITY_PROFILING
void LogProfilingData(unsigned int TimeSpent);
#endif

#endif //__HSMODULE_H_
//------------------------------------------------------------------------------------------------------------------------------------------
