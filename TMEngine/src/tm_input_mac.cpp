#include "tm_input.h"
#include <GLFW/glfw3.h>

#define TM_EXPORT __attribute__((visibility("default")))

extern TMInput gCurrentInput;
extern TMInput gLastInput;

TM_EXPORT bool TMInputKeyboardKeyIsDown(unsigned int keyCode) {
    return gCurrentInput.keys[keyCode].isPress;
}

TM_EXPORT bool TMInputKeyboardKeyJustDown(unsigned int keyCode) {
    if(gCurrentInput.keys[keyCode].isPress != gCurrentInput.keys[keyCode].wasPress) {
        return gCurrentInput.keys[keyCode].isPress;
    }
    return false;
}

TM_EXPORT bool TMInputKeyboardKeyIsUp(unsigned int keyCode) {
    return !gCurrentInput.keys[keyCode].isPress;
}

TM_EXPORT bool TMInputKeyboardKeyJustUp(unsigned int keyCode) {
    if(gCurrentInput.keys[keyCode].isPress != gCurrentInput.keys[keyCode].wasPress) {
        return gCurrentInput.keys[keyCode].wasPress;
    }
    return false;
}

TM_EXPORT bool TMInputMousButtonIsDown(TMMouseButton button) {
    return gCurrentInput.mouseButtons[button].isPress;
}

TM_EXPORT bool TMInputMousButtonJustDown(TMMouseButton button) {
    if(gCurrentInput.mouseButtons[button].isPress != gCurrentInput.mouseButtons[button].wasPress) {
        return gCurrentInput.mouseButtons[button].isPress;
    }
    return false;
}

TM_EXPORT bool TMInputMousButtonIsUp(TMMouseButton button) {
    return !gCurrentInput.mouseButtons[button].isPress;
}

TM_EXPORT bool TMInputMousButtonJustUp(TMMouseButton button) {
    if(gCurrentInput.mouseButtons[button].isPress != gCurrentInput.mouseButtons[button].wasPress) {
        return gCurrentInput.mouseButtons[button].wasPress;
    }
    return false;
}

TM_EXPORT int TMInputMousePositionX() {
    return gCurrentInput.mouseX;
}

TM_EXPORT int TMInputMousePositionY() {
    return gCurrentInput.mouseY;
}

TM_EXPORT int TMInputMouseLastPositionX() {
    return gLastInput.mouseX;
}

TM_EXPORT int TMInputMouseLastPositionY() {
    return gLastInput.mouseY;
}