#include <stdlib.h>

#include "tm_window_win32.h"
#include "../tm_window.h"
#include "../tm_input.h"

TMInput gCurrentInput;
TMInput gLastInput;
bool gRunning;

LRESULT WindowProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (msg) {
    case WM_CLOSE: {
        PostQuitMessage(0);
    }break;
    default: {
        result = DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    }
    return result;
}

void win32PollEvents() {
    MSG msg = {};
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
        case WM_QUIT: {
            gRunning = false;
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            bool wasDown = ((msg.lParam & (1 << 30)) != 0);
            bool isDown = ((msg.lParam & (1 << 31)) == 0);
            if (isDown != wasDown) {
                DWORD vkCode = (DWORD)msg.wParam;
                gCurrentInput.keys[vkCode].isPress = isDown;
            }
        }break;
        case WM_MOUSEMOVE: {
            gCurrentInput.mouseX = (int)GET_X_LPARAM(msg.lParam);
            gCurrentInput.mouseY = (int)GET_Y_LPARAM(msg.lParam);
        }break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            gCurrentInput.mouseButtons[0].isPress = ((msg.wParam & MK_LBUTTON) != 0);
            gCurrentInput.mouseButtons[2].isPress = ((msg.wParam & MK_MBUTTON) != 0);
            gCurrentInput.mouseButtons[1].isPress = ((msg.wParam & MK_RBUTTON) != 0);
        }break;
        default: {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }break;
        }
    }
}

 TMWindow* TMWindowCreate(int width, int height, const char* title){
    TMWindow* window = (TMWindow*)malloc(sizeof(TMWindow));
    HINSTANCE hInstance = GetModuleHandleA(0);
    WNDCLASS wndClass{};
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProcA;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(0, IDC_ARROW);
    wndClass.lpszClassName = "TMEngine";
    RegisterClassA(&wndClass);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int clientWidth = width;
    int clientHeight = height;
    RECT windowRect{};
    SetRect(&windowRect,
        (screenWidth / 2) - (clientWidth / 2),
        (screenHeight / 2) - (clientHeight / 2),
        (screenWidth / 2) + (clientWidth / 2),
        (screenHeight / 2) + (clientHeight / 2));

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    AdjustWindowRectEx(&windowRect, style, FALSE, 0);
    HWND hwnd = CreateWindowEx(0, "TMEngine", title, style,
        windowRect.left, windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL, NULL, hInstance, 0);

    window->hwndWindow = hwnd;
    window->width = width;
    window->height = height;

    gRunning = true;

    ShowWindow(window->hwndWindow, SW_SHOW);
    UpdateWindow(window->hwndWindow);

    return window;
}

 void TMWindowDestroy(TMWindow* window) {
    DestroyWindow(window->hwndWindow);
    free(window);
}

 bool TMWindowShouldClose(TMWindow* window) {
    return !gRunning;
}

 void TMWindowFlushEventQueue(TMWindow* window) {
    win32PollEvents();
    if(TMInputKeyboardKeyJustDown(TM_KEY_ESCAPE)){
        gRunning = false;
    }

    for (int i = 0; i < ARRAY_LENGTH(gCurrentInput.keys); ++i) {
        gCurrentInput.keys[i].wasPress = false;
        if (gLastInput.keys[i].isPress) {
            gCurrentInput.keys[i].wasPress = true;
        }
    }
    for (int i = 0; i < ARRAY_LENGTH(gCurrentInput.mouseButtons); ++i) {
        gCurrentInput.mouseButtons[i].wasPress = false;
        if (gLastInput.mouseButtons[i].isPress) {
            gCurrentInput.mouseButtons[i].wasPress = true;
        }
    }
}

 void TMWindowPresent(TMWindow* window) {
    gLastInput = gCurrentInput;
}

TM_EXPORT void TMSleep(float milliseconds) {
    Sleep(milliseconds);
}
