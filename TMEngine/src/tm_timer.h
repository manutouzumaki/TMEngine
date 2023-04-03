#ifndef _TM_TIMER_H_
#define _TM_TIMER_H_

#include "tm_defines.h"

struct TMTimer {
    double start;
    double invFrequency;
};

TM_EXPORT void TMTimerStart(TMTimer *timer);
TM_EXPORT double TMTimerGetCurrentTime(TMTimer *timer);
TM_EXPORT double TMTimerGetElapsedTime(TMTimer *timer);

#endif
