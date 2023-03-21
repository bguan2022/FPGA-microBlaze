//--HsMain.c--------------------------------------------------------------------------------------------------------------------------------
// Entry point for DJ60HS
// Revision History:		Jul 2008:	Creation, Donald Dansereau
//------------------------------------------------------------------------------------------------------------------------------------------
//#include <unistd.h>
#include <xparameters.h>
#include <stdio.h>
#include "platform.h"
#include "HsModule.h"
#include "Drivers/HsXcvrControlRegs.h"

//------------------------------------------------------------------------------------------------------------------------------------------
int main ()
{
	cleanup_platform();
    init_platform();

	//--------------------------------------------------------------------------------------------------------------------------------------
	ModuleInit();

	//--------------------------------------------------------------------------------------------------------------------------------------
	ModuleRun();

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------
