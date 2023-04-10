#ifndef _TM_TIMER_H_
#define _TM_TIMER_H_

#include "tm_defines.h"

struct TMTimer {
    unsigned long long frequency;
};

TM_EXPORT void TMTimerStart(TMTimer *timer);
TM_EXPORT unsigned long long TMTimerGetCurrentTime(TMTimer *timer);

#endif
