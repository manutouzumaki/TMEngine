#ifndef _EDITOR_UI_H_
#define _EDITOR_UI_H_

#include <tm_ui.h>

enum ModifyOption {
    MODIFY_NONE,
    MODIFY_TRANSLATE,
    MODIFY_SCALE,
    MODIFY_INC_ABS_U,
    MODIFY_INC_ABS_V,
    MODIFY_OFF_ABS_U,
    MODIFY_OFF_ABS_V,
    MODIFY_INC_REL_U,
    MODIFY_INC_REL_V,
    MODIFY_OFF_REL_U,
    MODIFY_OFF_REL_V,
};

enum BrushOption {
    OPTION_TEXTURE,
    OPTION_COLOR,
    OPTION_PREFABS,
    OPTION_CLEAR
};

enum LoadOption {
    LOAD_OPTION_NONE,
    LOAD_OPTION_TEXTURE,
    LOAD_OPTION_SHADER
};

struct EditorUI {

    TMUIElement *options;
    TMUIElement *textures;
    TMUIElement *texturesChilds[3];
    TMUIElement *colors;
    TMUIElement *shaders;
    TMUIElement *prefabs;
    TMUIElement *modify;
    TMUIElement *save;
    TMUIElement *loadTexture;
    TMUIElement *loadShader;

    char **texturesNames;
    char **shadersNames;
};

struct EditorState;

void EditorUIInitialize(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel);
void EditorUIUpdate(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel);
void EditorUIDraw(EditorState *state, EditorUI *ui, TMRenderer *renderer);
void EditorUIShutdown(EditorUI *ui);

#endif
