#ifndef _TM_WINDOW_H_
#define _TM_WINDOW_H_

struct TMWindow;

#define TM_COLOR_BUFFER_BIT   (1 << 0)
#define TM_DEPTH_BUFFER_BIT   (1 << 1)
#define TM_STENCIL_BUFFER_BIT (1 << 2)

TMWindow *TMWindowCreate(int width, int height, const char *title);
void TMWindowDestroy(TMWindow *window);
bool TMWindowShouldClose(TMWindow *window);
void TMWindowFlushEventQueue(TMWindow *window);
void TMWindowPresent(TMWindow *window);

#endif
