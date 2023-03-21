//--HsClockControl.h---------------------------------------------------------------------------------------------------------------------------
// Clock Control
// Revision History:		Aug 2008:	Creation, Donald Dansereau
//------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __HSCLOCK_CONTROL_H_
#define __HSCLOCK_CONTROL_H_

//------------------------------------------------------------------------------------------------------------------------------------------
#include "HsModule.h"
#include "HsModuleDefines.h"
#include "Drivers/HsXcvrControl.h"

//--Defines---------------------------------------------------------------------------------------------------------------------------------
#define CLOCK_REF_CLOCK_SOURCE_DEFAULT			( REF_CLOCK_SOURCE_SYSTEM )
#define CLOCK_REF_CLOCK_MODE_DEFAULT			( CLOCK_MODE_INTERNAL )
#define CLOCK_REF_CLOCK_SYNC_MODE_DEFAULT		( CLOCK_MODE_SYNC )
#define CLOCK_REF_CLOCK_FREQ_DEFAULT_kHz        ( 250000 )
#define CLOCK_OUT_FREQ_DEFAULT_kHz				( 100000 )

#define CLOCK_DATA_RATE_DEFAULT					( 240000000000LL )		// 0.1 Hz increments

#define CLOCK_5338_DEAD_ZONE_UPPPER				(    550000000LL )
#define CLOCK_5338_DEAD_ZONE_LOWER				(    473000000LL )

#define CLOCK_MIN_DATA_RATE                     ( 160000000001LL  )
#define CLOCK_MAX_DATA_RATE                     ( 240000000000LL )

#define REF_FREQ_MIN_RATE						(  25000 )
#define REF_FREQ_MAX_RATE						( 500000 )

#define CLOCK_OUT_MIN_RATE						(  10000 )
#define CLOCK_OUT_MAX_RATE						( 250000 )


//--Clock config Settings-------------------------------------------------------------------------------------------------------------------
struct Si5338PllSettings
{
	unsigned long long DataRateLowerBound;	// Denotes lower bound, EXCLUSIVE (thus -1 in the first entry) (see below)
};

struct Si5347RegStruct
{
	unsigned short regAddr;
	unsigned short value;
};

// structure for cache entry
struct ClockCacheStruct
{
	unsigned long long DataRate;			// requested value
	unsigned int refClkMode;				//Internal:1, External:0
	unsigned int refClkFreq;
	struct Si5347RegStruct Si5347Regs[800];	//max number of registers
};

struct ClockTestConfigStruct
{
	unsigned long long DataRate;			// requested value
	unsigned int refClkMode;				//Internal:1, External:0
	unsigned int refClkFreq;
};

//--Init------------------------------------------------------------------------------------------------------------------------------------
void ClockControlInit();
bool ClockControlApplySettings();

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateRefClockSyncMode( enum eRefClockSyncMode RefClockSyncMode );
bool ClockValidateRefClockMode( enum eRefClockMode RefClockMode );
void ClockSetRefClockMode( enum eRefClockMode RefClockMode );
bool ClockValidateDataRate( unsigned long long DataRate ) ;
bool ClockValidateRefClockRate( unsigned int RefFreq );
bool ClockConfigureRefClock(struct ClockCacheStruct *ClockCache, struct Si5347RegStruct *Si5347RegsCache);
bool ClockValidateOutClockMode( enum eClkOutMode OutClockMode);
bool ClockValidateOutClockFreq( unsigned int ClkOutFreq );
void ClockSetSi5338Si5347LockStatus();

bool ClockValidateTestConfig(unsigned long long DataRate, unsigned int refClkMode, unsigned int refClkFreq);
bool SearchClockCache(unsigned long long DataRate, unsigned int refClkMode, unsigned int refClkFreq, struct ClockCacheStruct **ClockCache);
bool ClockCacheCheck(unsigned long long DataRate, unsigned int refClkMode, unsigned int refClkFreq);
bool ClockCacheReplace(unsigned int profileLen);

//--RX Reference Clock Sources--------------------------------------------------------------------------------------------------------------
void ClockResetRxRefClockLogic();

//--TX Reference Clock Sources--------------------------------------------------------------------------------------------------------------
void ClockResetTxRefClockLogic();

void ClockXcvrFreqMeas();
void ClockReadCacheEntry();


#endif //__HSCLOCK_CONTROL_H_
//------------------------------------------------------------------------------------------------------------------------------------------
