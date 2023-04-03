#include "../tm_timer.h"

#include <windows.h>

void TMTimerStart(TMTimer *timer) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    timer->invFrequency = 1.0/(double)frequency.QuadPart;

    LARGE_INTEGER start;
    QueryPerformanceCounter(&start);
    timer->start = (double)start.QuadPart;
}

double TMTimerGetCurrentTime(TMTimer *timer) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return (double)current.QuadPart * timer->invFrequency;
}

double TMTimerGetElapsedTime(TMTimer *timer) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return (double)current.QuadPart*timer->invFrequency - timer->start;
}

