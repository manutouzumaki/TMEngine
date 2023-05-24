#include <stdlib.h>

#include "tm_window_win32.h"
#include "../tm_window.h"
#include "../tm_input.h"
#include "../utils/tm_darray.h"

#include <xinput.h>
#include <stdlib.h>
#include <stdio.h>

TMInput gCurrentInput;
TMInput gLastInput;
bool gRunning;
TMWindow *gWindow;

static WORD XInputButtons[] = {
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y
};

LRESULT WindowProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (msg) {
    case WM_CLOSE: {
        PostQuitMessage(0);
    }break;

    case WM_SIZE: {
        if( wParam == SIZE_MINIMIZED )
        {
        }
        else
        {
            int clientWidth  = LOWORD(lParam);
            int clientHeight = HIWORD(lParam);
            gWindow->width = clientWidth;
            gWindow->height = clientHeight;
            gWindow->updateRenderArea = true;
        }
    } break;

    default: {
        result = DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    }
    return result;
}
 
static float Win32ProcessXInputStick(SHORT value, int deadZoneValue)
{
    float result = 0;
    if(value < -deadZoneValue)
    {
        result = (float)(value + deadZoneValue) / (32768.0f - deadZoneValue);
    }
    else if(value > deadZoneValue)
    {
        result = (float)(value - deadZoneValue) / (32767.0f - deadZoneValue);
    }
    return result;
}

static void Win32PollEvents(TMWindow *window) {
    MSG msg = {};
    window->updateRenderArea = false;
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

    XINPUT_STATE state = {};
    if(XInputGetState(0, &state) == ERROR_SUCCESS)
    {
        XINPUT_GAMEPAD *pad = &state.Gamepad;
        for(int i = 0; i < ARRAY_LENGTH(gCurrentInput.joyButtons) - 2; ++i)
        {
            gCurrentInput.joyButtons[i].isPress = pad->wButtons & XInputButtons[i];
        }
        gCurrentInput.joyButtons[10].isPress = (pad->bLeftTrigger > 0);
        gCurrentInput.joyButtons[11].isPress = (pad->bRightTrigger > 0);
        gCurrentInput.leftStickX =  Win32ProcessXInputStick(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        gCurrentInput.leftStickY =  Win32ProcessXInputStick(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        gCurrentInput.rightStickX = Win32ProcessXInputStick(pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        gCurrentInput.rightStickY = Win32ProcessXInputStick(pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
    else
    {
        for(int i = 0; i < ARRAY_LENGTH(gCurrentInput.joyButtons); ++i)
        {
            gCurrentInput.joyButtons[i].isPress = false;
        }
        gCurrentInput.leftStickX = 0.0f; 
        gCurrentInput.leftStickY = 0.0f;
        gCurrentInput.rightStickX = 0.0f;
        gCurrentInput.rightStickY = 0.0f;
    }

}

 TMWindow* TMWindowCreate(int width, int height, const char* title){
    TMWindow* window = (TMWindow*)malloc(sizeof(TMWindow));
    gWindow = window;
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
    window->updateRenderArea = false;

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
    Win32PollEvents(window);
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
    for(int i = 0; i < ARRAY_LENGTH(gCurrentInput.joyButtons); ++i) {
            gCurrentInput.joyButtons[i].wasPress = false; 
        if(gLastInput.joyButtons[i].isPress) {
            gCurrentInput.joyButtons[i].wasPress = true;
        }
    }

}

void TMWindowPresent(TMWindow* window) {
    gLastInput = gCurrentInput;
}

void TMSleep(float milliseconds) {
    Sleep(milliseconds);
}

void TMMouseSetCapture(TMWindow *window) {
    SetCapture(window->hwndWindow);
}

void TMMouseReleaseCapture() {
    ReleaseCapture();
}

static int StringLength(char *string) {
    int counter = 0;
    char *letter = string; 
    while(*letter++ != L'\0') counter++;
    return counter;
}

#include <strsafe.h>

void TMGetFileNamesInDirectory(char *directoryPath, char ***files) {
    
    char path[MAX_PATH];

    char *postPath = "//*";
    int postPathSize = 3;

    int dirPathSize = StringLength(directoryPath);
    int i = 0;

    memcpy(path, directoryPath, dirPathSize);
    memcpy(path + dirPathSize, postPath, postPathSize);

    path[dirPathSize + postPathSize] = '\0';
    
    printf("path: %s\n", path);

    WIN32_FIND_DATAA findData;

    HANDLE hFind = FindFirstFile(path, &findData);
    if(hFind == INVALID_HANDLE_VALUE) {
        printf("ERROR: directory: %s NOT FOUND\n", path);
        return;
    }

    do {
        if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

            int fileNameSize = StringLength(findData.cFileName);
            char *fileName = (char *)malloc(fileNameSize + 1);
            memcpy(fileName, findData.cFileName, fileNameSize);
            fileName[fileNameSize] = '\0';
            TMDarrayPush(*files, fileName, char *);
            i++;
        } 
    }
    while(FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);

}
