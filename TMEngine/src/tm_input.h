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

#ifdef TM_WIN32

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

#define 	GLFW_KEY_UNKNOWN   -1
#define 	GLFW_KEY_SPACE   32
#define 	GLFW_KEY_APOSTROPHE   39 /* ' */
#define 	GLFW_KEY_COMMA   44 /* , */
#define 	GLFW_KEY_MINUS   45 /* - */
#define 	GLFW_KEY_PERIOD   46 /* . */
#define 	GLFW_KEY_SLASH   47 /* / */
#define 	GLFW_KEY_0   48
#define 	GLFW_KEY_1   49
#define 	GLFW_KEY_2   50
#define 	GLFW_KEY_3   51
#define 	GLFW_KEY_4   52
#define 	GLFW_KEY_5   53
#define 	GLFW_KEY_6   54
#define 	GLFW_KEY_7   55
#define 	GLFW_KEY_8   56
#define 	GLFW_KEY_9   57
#define 	GLFW_KEY_SEMICOLON   59 /* ; */
#define 	GLFW_KEY_EQUAL   61 /* = */
#define 	GLFW_KEY_A   65
#define 	GLFW_KEY_B   66
#define 	GLFW_KEY_C   67
#define 	GLFW_KEY_D   68
#define 	GLFW_KEY_E   69
#define 	GLFW_KEY_F   70
#define 	GLFW_KEY_G   71
#define 	GLFW_KEY_H   72
#define 	GLFW_KEY_I   73
#define 	GLFW_KEY_J   74
#define 	GLFW_KEY_K   75
#define 	GLFW_KEY_L   76
#define 	GLFW_KEY_M   77
#define 	GLFW_KEY_N   78
#define 	GLFW_KEY_O   79
#define 	GLFW_KEY_P   80
#define 	GLFW_KEY_Q   81
#define 	GLFW_KEY_R   82
#define 	GLFW_KEY_S   83
#define 	GLFW_KEY_T   84
#define 	GLFW_KEY_U   85
#define 	GLFW_KEY_V   86
#define 	GLFW_KEY_W   87
#define 	GLFW_KEY_X   88
#define 	GLFW_KEY_Y   89
#define 	GLFW_KEY_Z   90
#define 	GLFW_KEY_LEFT_BRACKET   91 /* [ */
#define 	GLFW_KEY_BACKSLASH   92 /* \ */
#define 	GLFW_KEY_RIGHT_BRACKET   93 /* ] */
#define 	GLFW_KEY_GRAVE_ACCENT   96 /* ` */
#define 	GLFW_KEY_WORLD_1   161 /* non-US #1 */
#define 	GLFW_KEY_WORLD_2   162 /* non-US #2 */
#define 	GLFW_KEY_ESCAPE   256
#define 	GLFW_KEY_ENTER   257
#define 	GLFW_KEY_TAB   258
#define 	GLFW_KEY_BACKSPACE   259
#define 	GLFW_KEY_INSERT   260
#define 	GLFW_KEY_DELETE   261
#define 	GLFW_KEY_RIGHT   262
#define 	GLFW_KEY_LEFT   263
#define 	GLFW_KEY_DOWN   264
#define 	GLFW_KEY_UP   265
#define 	GLFW_KEY_PAGE_UP   266
#define 	GLFW_KEY_PAGE_DOWN   267
#define 	GLFW_KEY_HOME   268
#define 	GLFW_KEY_END   269
#define 	GLFW_KEY_CAPS_LOCK   280
#define 	GLFW_KEY_SCROLL_LOCK   281
#define 	GLFW_KEY_NUM_LOCK   282
#define 	GLFW_KEY_PRINT_SCREEN   283
#define 	GLFW_KEY_PAUSE   284
#define 	GLFW_KEY_F1   290
#define 	GLFW_KEY_F2   291
#define 	GLFW_KEY_F3   292
#define 	GLFW_KEY_F4   293
#define 	GLFW_KEY_F5   294
#define 	GLFW_KEY_F6   295
#define 	GLFW_KEY_F7   296
#define 	GLFW_KEY_F8   297
#define 	GLFW_KEY_F9   298
#define 	GLFW_KEY_F10   299
#define 	GLFW_KEY_F11   300
#define 	GLFW_KEY_F12   301
#define 	GLFW_KEY_F13   302
#define 	GLFW_KEY_F14   303
#define 	GLFW_KEY_F15   304
#define 	GLFW_KEY_F16   305
#define 	GLFW_KEY_F17   306
#define 	GLFW_KEY_F18   307
#define 	GLFW_KEY_F19   308
#define 	GLFW_KEY_F20   309
#define 	GLFW_KEY_F21   310
#define 	GLFW_KEY_F22   311
#define 	GLFW_KEY_F23   312
#define 	GLFW_KEY_F24   313
#define 	GLFW_KEY_F25   314
#define 	GLFW_KEY_KP_0   320
#define 	GLFW_KEY_KP_1   321
#define 	GLFW_KEY_KP_2   322
#define 	GLFW_KEY_KP_3   323
#define 	GLFW_KEY_KP_4   324
#define 	GLFW_KEY_KP_5   325
#define 	GLFW_KEY_KP_6   326
#define 	GLFW_KEY_KP_7   327
#define 	GLFW_KEY_KP_8   328
#define 	GLFW_KEY_KP_9   329
#define 	GLFW_KEY_KP_DECIMAL   330
#define 	GLFW_KEY_KP_DIVIDE   331
#define 	GLFW_KEY_KP_MULTIPLY   332
#define 	GLFW_KEY_KP_SUBTRACT   333
#define 	GLFW_KEY_KP_ADD   334
#define 	GLFW_KEY_KP_ENTER   335
#define 	GLFW_KEY_KP_EQUAL   336
#define 	GLFW_KEY_LEFT_SHIFT   340
#define 	GLFW_KEY_LEFT_CONTROL   341
#define 	GLFW_KEY_LEFT_ALT   342
#define 	GLFW_KEY_LEFT_SUPER   343
#define 	GLFW_KEY_RIGHT_SHIFT   344
#define 	GLFW_KEY_RIGHT_CONTROL   345
#define 	GLFW_KEY_RIGHT_ALT   346
#define 	GLFW_KEY_RIGHT_SUPER   347
#define 	GLFW_KEY_MENU   348
#define 	GLFW_KEY_LAST   GLFW_KEY_MENU

#define TM_KEY_ESCAPE GLFW_KEY_ESCAPE
#define TM_KEY_SPACE  GLFW_KEY_SPACE

#define TM_KEY_W GLFW_KEY_W
#define TM_KEY_S GLFW_KEY_S
#define TM_KEY_A GLFW_KEY_A
#define TM_KEY_D GLFW_KEY_D


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
