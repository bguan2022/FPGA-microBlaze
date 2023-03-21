/*
 * HsCalFlash.h
 */

int InitCalFlash();
int CalFlashRead(unsigned int StartAddress, unsigned char* DestBuffer, unsigned int Length);
int CalFlashWrite(unsigned int StartAddress, unsigned char* SrcBuffer, unsigned int Length);
