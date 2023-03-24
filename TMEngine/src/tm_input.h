#ifndef _TM_INPUT_H_
#define _TM_INPUT_H_

#include "tm_defines.h"

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

#ifdef TM_WIN32
#define TM_KEY_ESCAPE VK_ESCAPE
#define TM_KEY_SPACE  VK_SPACE
#endif

#ifdef TM_MACOS
#define TM_KEY_ESCAPE GLFW_KEY_ESCAPE
#define TM_KEY_SPACE  GLFW_KEY_SPACE
#endif

enum TMMouseButton {
    TM_MOUSE_BUTTON_LEFT,
    TM_MOUSE_BUTTON_RIGHT,
    TM_MOUSE_BUTTON_MIDDLE
};

TM_EXPORT bool TMInputKeyboardKeyIsDown(unsigned int keyCode);
TM_EXPORT bool TMInputKeyboardKeyJustDown(unsigned int keyCode);
TM_EXPORT bool TMInputKeyboardKeyIsUp(unsigned int keyCode);
TM_EXPORT bool TMInputKeyboardKeyJustUp(unsigned int keyCode);

TM_EXPORT bool TMInputMousButtonIsDown(TMMouseButton button);
TM_EXPORT bool TMInputMousButtonJustDown(TMMouseButton button);
TM_EXPORT bool TMInputMousButtonIsUp(TMMouseButton button);
TM_EXPORT bool TMInputMousButtonJustUp(TMMouseButton button);

TM_EXPORT int TMInputMousePositionX();
TM_EXPORT int TMInputMousePositionY();
TM_EXPORT int TMInputMouseLastPositionX();
TM_EXPORT int TMInputMouseLastPositionY();

#endif