#ifndef _TM_INPUT_H_
#define _TM_INPUT_H_

#include "tm_defines.h"

struct TMButton {
    bool wasPress;
    bool isPress;
};

struct TMInput {
    
    TMButton joyButtons[12];
    float leftStickX;
    float leftStickY;
    float rightStickX;
    float rightStickY;

    TMButton keys[349];
    TMButton mouseButtons[3];
    int mouseX;
    int mouseY;


};

#ifdef TM_WIN32

#define TM_JOYSTICK_BUTTON_UP     0
#define TM_JOYSTICK_BUTTON_DOWN   1
#define TM_JOYSTICK_BUTTON_LEFT   2
#define TM_JOYSTICK_BUTTON_RIGHT  3
#define TM_JOYSTICK_BUTTON_START  4
#define TM_JOYSTICK_BUTTON_BACK   5
#define TM_JOYSTICK_BUTTON_A      6
#define TM_JOYSTICK_BUTTON_B      7
#define TM_JOYSTICK_BUTTON_X      8
#define TM_JOYSTICK_BUTTON_Y      9
#define TM_JOYSTICK_LEFT_TRIGGER  10
#define TM_JOYSTICK_RIGHT_TRIGGER 11

#define TM_KEY_BACKESPACE	 0x08
#define TM_KEY_ESCAPE      0x1B
#define TM_KEY_0  0x30 
#define TM_KEY_1  0x31 
#define TM_KEY_2  0x32 
#define TM_KEY_3  0x33 
#define TM_KEY_4  0x34 
#define TM_KEY_5  0x35 
#define TM_KEY_6  0x36 
#define TM_KEY_7  0x37 
#define TM_KEY_8  0x38 
#define TM_KEY_9  0x39 
#define TM_KEY_A  0x41 
#define TM_KEY_B  0x42 
#define TM_KEY_C  0x43 
#define TM_KEY_D  0x44 
#define TM_KEY_E  0x45 
#define TM_KEY_F  0x46 
#define TM_KEY_G  0x47 
#define TM_KEY_H  0x48 
#define TM_KEY_I  0x49 
#define TM_KEY_J  0x4A 
#define TM_KEY_K  0x4B 
#define TM_KEY_L  0x4C 
#define TM_KEY_M  0x4D 
#define TM_KEY_N  0x4E 
#define TM_KEY_O  0x4F 
#define TM_KEY_P  0x50 
#define TM_KEY_Q  0x51 
#define TM_KEY_R  0x52 
#define TM_KEY_S  0x53 
#define TM_KEY_T  0x54 
#define TM_KEY_U  0x55 
#define TM_KEY_V  0x56 
#define TM_KEY_W  0x57 
#define TM_KEY_X  0x58 
#define TM_KEY_Y  0x59 
#define TM_KEY_Z  0x5A 
#define TM_KEY_NUMPAD0  0x60	    
#define TM_KEY_NUMPAD1  0x61	    
#define TM_KEY_NUMPAD2  0x62	    
#define TM_KEY_NUMPAD3  0x63	    
#define TM_KEY_NUMPAD4  0x64	    
#define TM_KEY_NUMPAD5  0x65	    
#define TM_KEY_NUMPAD6  0x66	    
#define TM_KEY_NUMPAD7  0x67	    
#define TM_KEY_NUMPAD8  0x68	    
#define TM_KEY_NUMPAD9  0x69	    
#define TM_KEY_RETURN   0x0D
#define TM_KEY_SPACE    0x20
#define TM_KEY_TAB      0x09
#define TM_KEY_CONTROL  0x11
#define TM_KEY_SHIFT    0x10
#define TM_KEY_ALT      0x12
#define TM_KEY_CAPS     0x14
#define TM_KEY_LEFT     0x25
#define TM_KEY_UP       0x26
#define TM_KEY_RIGHT    0x27
#define TM_KEY_DOWN     0x28

#endif

#ifdef TM_MACOS
// TODO: ....
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

TM_EXPORT bool TMInputJoystickButtomIsDown(unsigned int button);
TM_EXPORT bool TMInputJoystickButtomJustDown(unsigned int button);
TM_EXPORT bool TMInputJoystickButtomIsUp(unsigned int button);
TM_EXPORT bool TMInputJoystickButtomJustUp(unsigned int button);
TM_EXPORT float TMInputJoystickLeftStickX();
TM_EXPORT float TMInputJoystickLeftStickY();
TM_EXPORT float TMInputJoystickRightStickX();
TM_EXPORT float TMInputJoystickRightStickY();

TM_EXPORT bool TMInputMousButtonIsDown(TMMouseButton button);
TM_EXPORT bool TMInputMousButtonJustDown(TMMouseButton button);
TM_EXPORT bool TMInputMousButtonIsUp(TMMouseButton button);
TM_EXPORT bool TMInputMousButtonJustUp(TMMouseButton button);

TM_EXPORT int TMInputMousePositionX();
TM_EXPORT int TMInputMousePositionY();
TM_EXPORT int TMInputMouseLastPositionX();
TM_EXPORT int TMInputMouseLastPositionY();


#endif
