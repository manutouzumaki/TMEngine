#ifndef _TM_INPUT_H_
#define _TM_INPUT_H_

struct TMButton {
    bool wasPress;
    bool isPress;
};

struct TMInput {
    TMButton keys[349];
    TMButton mouseButtons[3];
    int mouseX;
    int mouseY;
};

enum TMMouseButton {
    TM_MOUSE_BUTTON_LEFT,
    TM_MOUSE_BUTTON_RIGHT,
    TM_MOUSE_BUTTON_MIDDLE
};

bool TMInputKeyboardKeyIsDown(unsigned int keyCode);
bool TMInputKeyboardKeyJustDown(unsigned int keyCode);
bool TMInputKeyboardKeyIsUp(unsigned int keyCode);
bool TMInputKeyboardKeyJustUp(unsigned int keyCode);

bool TMInputMousButtonIsDown(TMMouseButton button);
bool TMInputMousButtonJustDown(TMMouseButton button);
bool TMInputMousButtonIsUp(TMMouseButton button);
bool TMInputMousButtonJustUp(TMMouseButton button);

int TMInputMousePositionX();
int TMInputMousePositionY();
int TMInputMouseLastPositionX();
int TMInputMouseLastPositionY();

#endif