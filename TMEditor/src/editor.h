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

struct AABB {
    TMVec2 min;
    TMVec2 max;
};

struct Circle {
    TMVec2 c;
    float r;
};

struct Capsule {
    TMVec2 a;
    TMVec2 b;
    float r;
};

enum CollisionType {
    COLLISION_TYPE_AABB,
    COLLISION_TYPE_OBB,
    COLLISION_TYPE_CIRCLE,
    COLLISION_TYPE_CAPSULE
};

struct Collision {
    CollisionType type;
    union {
        AABB aabb;
        Circle circle;
        Capsule capsule;
    };
    bool solid;
};

enum PrefabsType {
    PREFAB_TYPE_NONE,
    PREFAB_TYPE_PLAYER,
    PREFAB_TYPE_ENEMY
};

struct Entity {
    int id;
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
    TMVec2 position;
    TMVec2 size;
    TMShader *shader;
    TMTexture *texture;
    int zIndex;
    int textureIndex;
    Collision *collision;
    PrefabsType prefabType;
};

struct EditorState {

    TMWindow       *window;
    TMRenderer     *renderer;
    TMBuffer       *vertexBuffer;
    TMShaderBuffer *shaderBuffer;
    TMShader       *colorShader;
    TMShader       *spriteShader;

    TMTexture **textures;
    TMShader  **shaders;
    Entity     *entities;
    char      **texturesAddedNames;
    char      **shadersAddedNames;

    TMUIElement *element;
    Entity      *selectedEntity;

    EditorUI     ui;
    TMVec3       cameraP;
    float        meterToPixel;
    ModifyOption modifyOption;
    PrefabsType  prefabType;
    BrushOption  option;
    LoadOption   loadOption;
    bool         mouseIsHot;

};

struct TMWindow;

void EditorInitialize(EditorState *state, TMWindow *window);
void EditorUpdate(EditorState *state);
void EditorRender(EditorState *state);
void EditorShutdown(EditorState *state);


#endif
