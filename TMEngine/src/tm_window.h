#ifndef _TM_WINDOW_H_
#define _TM_WINDOW_H_

#include "tm_defines.h"

struct TMWindow;

#define TM_COLOR_BUFFER_BIT   (1 << 0)
#define TM_DEPTH_BUFFER_BIT   (1 << 1)
#define TM_STENCIL_BUFFER_BIT (1 << 2)

TM_EXPORT TMWindow *TMWindowCreate(int width, int height, const char *title);
TM_EXPORT void TMWindowDestroy(TMWindow *window);
TM_EXPORT bool TMWindowShouldClose(TMWindow *window);
TM_EXPORT void TMWindowFlushEventQueue(TMWindow *window);
TM_EXPORT void TMWindowPresent(TMWindow *window);

#endif
