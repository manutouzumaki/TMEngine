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

    MODIFY_TRANSLATE_LIGHT,
    MODIFY_QUADRA_LIGHT,
    MODIFY_LINEAR_LIGHT,
    MODIFY_CONSTA_LIGHT,
    MODIFY_RANGE_LIGHT,

    MODIFY_R_LIGHT,
    MODIFY_G_LIGHT,
    MODIFY_B_LIGHT,

    MODIFY_SHOT_RANGE,
    MODIFY_SHOT_SPEED
};

enum BrushOption {
    OPTION_TEXTURE,
    OPTION_COLOR,
    OPTION_PREFABS,
    OPTION_LIGHT,
    OPTION_CLEAR
};

enum LoadOption {
    LOAD_OPTION_NONE,
    LOAD_OPTION_TEXTURE,
    LOAD_OPTION_SHADER,
    LOAD_OPTION_SCENE
};

struct EditorUI {

    TMUIElement *options;
    TMUIElement *textures;
    TMUIElement *texturesChilds[3];
    TMUIElement *colors;
    TMUIElement *shaders;
    TMUIElement *prefabs;
    TMUIElement *lights;
    TMUIElement *clear;
    TMUIElement *modify;
    TMUIElement *save;
    TMUIElement *loadTexture;
    TMUIElement *loadShader;
    TMUIElement *loadScene;

    TMUIElement *lightModify;
    TMUIElement *shotEnemyModify;

    char **texturesNames;
    char **shadersNames;
    char **scenesNames;
};

struct EditorState;

void EditorUIInitialize(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel);
void EditorUIUpdate(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel);
void EditorUIDraw(EditorState *state, EditorUI *ui, TMRenderer *renderer);
void EditorUIShutdown(EditorUI *ui);


void ElementSelected(TMUIElement *element);

#endif
