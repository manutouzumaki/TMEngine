#include "../tm_timer.h"

#include <windows.h>
#include <stdio.h>

static bool firstTime;

void TMTimerStart(TMTimer *timer) {
    if(!firstTime) {
        printf("timeBeginPeriod\n");
        timeBeginPeriod(1);
        firstTime = true;
    }
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    timer->frequency = frequency.QuadPart;
}

unsigned long long TMTimerGetCurrentTime(TMTimer *timer) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return current.QuadPart;
}

