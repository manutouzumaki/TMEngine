#include "tm_input.h"

extern TMInput gCurrentInput;
extern TMInput gLastInput;


// TODO: add support for the joystics ....

bool TMInputKeyboardKeyIsDown(unsigned int keyCode) {
    return gCurrentInput.keys[keyCode].isPress;
}

bool TMInputKeyboardKeyJustDown(unsigned int keyCode) {
    if(gCurrentInput.keys[keyCode].isPress != gCurrentInput.keys[keyCode].wasPress) {
        return gCurrentInput.keys[keyCode].isPress;
    }
    return false;
}

bool TMInputKeyboardKeyIsUp(unsigned int keyCode) {
    return !gCurrentInput.keys[keyCode].isPress;
}

bool TMInputKeyboardKeyJustUp(unsigned int keyCode) {
    if(gCurrentInput.keys[keyCode].isPress != gCurrentInput.keys[keyCode].wasPress) {
        return gCurrentInput.keys[keyCode].wasPress;
    }
    return false;
}


bool TMInputJoystickButtomIsDown(unsigned int button) {
    return gCurrentInput.joyButtons[button].isPress;
}

bool TMInputJoystickButtomJustDown(unsigned int button) {
    if(gCurrentInput.joyButtons[button].isPress != gCurrentInput.joyButtons[button].wasPress) {
        return gCurrentInput.joyButtons[button].isPress;
    }
    return false;
}

bool TMInputJoystickButtomIsUp(unsigned int button) {
    return !gCurrentInput.joyButtons[button].isPress;
}

bool TMInputJoystickButtomJustUp(unsigned int button) {
    if(gCurrentInput.joyButtons[button].isPress != gCurrentInput.joyButtons[button].wasPress) {
        return gCurrentInput.joyButtons[button].wasPress;
    }
    return false;
}

float TMInputJoystickLeftStickX() {
    return gCurrentInput.leftStickX;
}

float TMInputJoystickLeftStickY() {
    return gCurrentInput.leftStickY;
}

float TMInputJoystickRightStickX() {
    return gCurrentInput.rightStickX;
}

float TMInputJoystickRightStickY() {
    return gCurrentInput.rightStickY;
}








bool TMInputMousButtonIsDown(TMMouseButton button) {
    return gCurrentInput.mouseButtons[button].isPress;
}

bool TMInputMousButtonJustDown(TMMouseButton button) {
    if(gCurrentInput.mouseButtons[button].isPress != gCurrentInput.mouseButtons[button].wasPress) {
        return gCurrentInput.mouseButtons[button].isPress;
    }
    return false;
}

bool TMInputMousButtonIsUp(TMMouseButton button) {
    return !gCurrentInput.mouseButtons[button].isPress;
}

bool TMInputMousButtonJustUp(TMMouseButton button) {
    if(gCurrentInput.mouseButtons[button].isPress != gCurrentInput.mouseButtons[button].wasPress) {
        return gCurrentInput.mouseButtons[button].wasPress;
    }
    return false;
}

int TMInputMousePositionX() {
    return gCurrentInput.mouseX;
}

int TMInputMousePositionY() {
    return gCurrentInput.mouseY;
}

int TMInputMouseLastPositionX() {
    return gLastInput.mouseX;
}

int TMInputMouseLastPositionY() {
    return gLastInput.mouseY;
}
