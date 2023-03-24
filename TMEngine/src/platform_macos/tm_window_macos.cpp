#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "../tm_window.h"
#include "../tm_input.h"

struct TMWindow {
    GLFWwindow *glfwWindow;
    int width;
    int height;
};

TMInput gCurrentInput;
TMInput gLastInput;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    gCurrentInput.keys[key].isPress = action;
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    gCurrentInput.mouseX = xpos;
    gCurrentInput.mouseY = ypos;
}
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {}
    if(button < 3) {
        gCurrentInput.mouseButtons[button].isPress = action;
    }
}

 TMWindow *TMWindowCreate(int width, int height, const char *title) {
    TMWindow *window = (TMWindow *)malloc(sizeof(TMWindow));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window->width = width;
    window->height = height;
    window->glfwWindow = glfwCreateWindow(window->width, window->height, title, NULL, NULL);
    if(window->glfwWindow == NULL) {
        printf("Failed to create TMWindow\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window->glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }

    glfwSetFramebufferSizeCallback(window->glfwWindow, framebuffer_size_callback);
    glfwSetKeyCallback(window->glfwWindow, key_callback);
    glfwSetCursorPosCallback(window->glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(window->glfwWindow, mouse_button_callback);

    return window;
}

 void TMWindowDestroy(TMWindow *window) {
    glfwTerminate();
}

 bool TMWindowShouldClose(TMWindow *window) {
    return glfwWindowShouldClose(window->glfwWindow);
}

 void TMWindowPresent(TMWindow *window) {
    glfwSwapBuffers(window->glfwWindow);
    gLastInput = gCurrentInput;
}

 void TMWindowFlushEventQueue(TMWindow *window) {
    glfwPollEvents();    
    if(TMInputKeyboardKeyJustDown(TM_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window->glfwWindow, true); 
    }

    for(int i = 0; i < ARRAY_LENGTH(gCurrentInput.keys); ++i) {
        gCurrentInput.keys[i].wasPress = false;
        if(gLastInput.keys[i].isPress) {
            gCurrentInput.keys[i].wasPress = true;
        }
    }
    for(int i = 0; i < ARRAY_LENGTH(gCurrentInput.mouseButtons); ++i) {
        gCurrentInput.mouseButtons[i].wasPress = false;
        if(gLastInput.mouseButtons[i].isPress) {
            gCurrentInput.mouseButtons[i].wasPress = true;
        }
    }
}
