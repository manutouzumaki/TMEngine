#include <tm_defines.h>
#include <tm_window.h>
#include "editor.h"

int main() {

    TMWindow *window = TMWindowCreate(1280, 720, "TMEditor");

    EditorState editorState{};
    EditorInitialize(&editorState, window);

    while(!TMWindowShouldClose(window)) {
        TMWindowFlushEventQueue(window);

        EditorUpdate(&editorState);
        EditorRender(&editorState);

        TMWindowPresent(window);
    }

    EditorShutdown(&editorState);
    TMWindowDestroy(window);

    return 0;
}
