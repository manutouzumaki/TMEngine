#include "../tm_timer.h"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

static bool firstTime;

void TMTimerStart(TMTimer *timer)
{
    if(!firstTime) {
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);

        firstTime = true;
        timer->frequency = 1.0f / (float)info.numer / (float)info.denom;
    } 
}

unsigned long long TMTimerGetCurrentTime(TMTimer* timer) {
    return mach_absolute_time();
}
