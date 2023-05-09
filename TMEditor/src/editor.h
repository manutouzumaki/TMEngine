#ifndef _EDITOR_H_
#define _EDITOR_H_

// TODO: move this to cpp files ...
#include <tm_renderer.h>
#include <tm_debug_renderer.h>
#include <utils/tm_hashmap.h>
#include <utils/tm_math.h>
#include <utils/tm_darray.h>
#include <utils/tm_json.h>
#include <tm_input.h>
#include <stdio.h>
#include <math.h>
#include "editor_ui.h"

struct Entity {
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
    TMVec2 position;
    TMVec2 size;
    TMShader *shader;
    TMTexture *texture;
};

struct EditorState {

    TMRenderer     *renderer;
    TMBuffer       *vertexBuffer;
    TMShaderBuffer *shaderBuffer;
    TMShader       *colorShader;
    TMShader       *spriteShader;


    TMUIElement *element;
    Entity      *entities;
    Entity      *selectedEntity;


    EditorUI ui;
    TMVec3       cameraP;
    float        meterToPixel;
    ModifyOption modifyOption;
    BrushOption  option;
    bool         mouseIsHot;

};

struct TMWindow;

void EditorInitialize(EditorState *state, TMWindow *window);
void EditorUpdate(EditorState *state);
void EditorRender(EditorState *state);
void EditorShutdown(EditorState *state);


#endif
