//--HsInterrupt.c---------------------------------------------------------------------------------------------------------------------------

void InterruptInit();
void InterruptConnect(u8 IntrSrcId, XInterruptHandler Handler, void *CallBackRef);
void InterruptEnable(u8 IntrSrcId);
void InterruptDisableAll();

//-----------------------------------------------------------------------------------------------------------------------------------------
void MicroBlazeInterruptInit(void* CallBackFunc);
void MicroBlazeInterruptEnable();
void MicroBlazeInterruptDisable();
void MicroBlazeForceSwInterrupt();
