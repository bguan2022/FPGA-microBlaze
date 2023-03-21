//--HsTimer.h-------------------------------------------------------------------------------------------------------------------------------
void TimerInit();
#ifndef __HSTIMER_H_
#define __HSTIMER_H_

#define TIMER_US_TO_TICK	(100)

void TimerInit();
void tickSleep(unsigned int tickSleep);
void usleep(unsigned int useconds);

void TimerStart();
void TimerStop();
unsigned int TimerGetValue();
unsigned int TimerStopAndGetValue();

#endif //__HSTIMER_H_
