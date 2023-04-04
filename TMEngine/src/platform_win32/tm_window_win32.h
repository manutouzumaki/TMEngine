#ifndef _TM_WINDOW_WIN32_H_
#define _TM_WINDOW_WIN32_H_

#include <windows.h>
#include <windowsx.h>

struct TMWindow {
    HWND hwndWindow;
    int width;
    int height;
};

#endif
