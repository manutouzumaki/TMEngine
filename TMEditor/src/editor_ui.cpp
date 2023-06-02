#include "editor_ui.h"
#include "editor.h"
#include "utils.h"
#include "scene.h"

#include <stdio.h>
#include <memory.h>
#include <assert.h>

static void OptionSelected(TMUIElement *element) {
    printf("Option selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->option = (BrushOption)element->index;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_NONE;
    state->selectedEntity = NULL;
    state->lightSelected = -1;
}

static void ClearSelected(TMUIElement *element) {
    printf("Clear selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->prefabType = PREFAB_TYPE_NONE;
    state->element = NULL;
    state->lightSelected = -1;
    state->option = OPTION_CLEAR;
}

void ElementSelected(TMUIElement *element) {
    printf("Element selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->selectedEntity = NULL;
    state->lightSelected = -1;
}

static void PlayerPrefabSelected(TMUIElement *element) {
    printf("Prefab player selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_PLAYER;
    state->selectedEntity = NULL;

}

static void WinPrefabSelected(TMUIElement *element) {
    printf("Prefab player selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_WIN;
    state->selectedEntity = NULL;
}

static void GameOverPrefabSelected(TMUIElement *element) {
    printf("Prefab player selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_GAME_OVER;
    state->selectedEntity = NULL;
}

static void ShotEnemyPrefabSelected(TMUIElement *element) {
    printf("Prefab shot enemy selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_SHOT_ENEMY;
    state->selectedEntity = NULL;
}

static void MoveEnemyPrefabSelected(TMUIElement *element) {
    printf("Prefab move enemy selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_MOVE_ENEMY;
    state->selectedEntity = NULL;
}

static void TranslateEntity(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_TRANSLATE; }
static void ScaleEntity(TMUIElement *element)     { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_SCALE;     }
static void IncrementAbsU(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_ABS_U; }
static void IncrementAbsV(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_ABS_V; }
static void OffsetAbsU(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_ABS_U; }
static void OffsetAbsV(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_ABS_V; }
static void IncrementRelU(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_REL_U; }
static void IncrementRelV(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_REL_V; }
static void OffsetRelU(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_REL_U; }
static void OffsetRelV(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_REL_V; }


static void TranslateLight(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_TRANSLATE_LIGHT; }

static void Quadratic(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_QUADRA_LIGHT; }
static void Linear(TMUIElement *element)    { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_LINEAR_LIGHT; }
static void Constant(TMUIElement *element)    { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_CONSTA_LIGHT; }
static void Range(TMUIElement *element)    { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_RANGE_LIGHT; }

static void Red(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_R_LIGHT; }
static void Green(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_G_LIGHT; }
static void Blue(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_B_LIGHT; }

static void IncrementZ(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        entity->zIndex++;
        printf("zIndex: %d\n", entity->zIndex);
    }

}

static void DecrementZ(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        entity->zIndex--;
        entity->zIndex = MaxI32(entity->zIndex, 1);
        printf("zIndex: %d\n", entity->zIndex);
    }

}

static void AddCollision(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        if(!entity->collision) {

            entity->collision = (Collision *)malloc(sizeof(Collision));
            Collision *collision = entity->collision;

            collision->type = COLLISION_TYPE_AABB;
            collision->solid = true;
            AABB aabb;
            aabb.min = entity->position - entity->size*0.5f;
            aabb.max = entity->position + entity->size*0.5f;
            collision->aabb = aabb;
        }

    }

}

static void RemCollision(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        if(entity->collision) free(entity->collision);
        entity->collision = NULL;
    }

}

static void SolidCollision(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;
    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        if(entity->collision) {
            entity->collision->solid = !entity->collision->solid;
        }
    }

}

static void EnemyShotDir(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;
    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        if(entity->enemyShot) {
            entity->enemyShot->facingLeft = !entity->enemyShot->facingLeft;
        }
    }

}

static void EnemyShotRange(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_SHOT_RANGE;

}

static void EnemyShotSpeed(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_SHOT_SPEED;

}


static void LoadTexture(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    if(state->loadOption != LOAD_OPTION_TEXTURE) {
        state->loadOption = LOAD_OPTION_TEXTURE;
    }
    else {
        state->loadOption = LOAD_OPTION_NONE;
    }

}

static void LoadShader(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    if(state->loadOption != LOAD_OPTION_SHADER) {
        state->loadOption = LOAD_OPTION_SHADER;
    }
    else {
        state->loadOption = LOAD_OPTION_NONE;
    }
}

static void SelectTexture(TMUIElement *element) {
    
    EditorState *state = (EditorState *)element->userData;
    printf("Texture Selected: %s\n", state->ui.texturesNames[element->index]);

    char filepath[10000] = "../../assets/images/";
    int headerSize = StringLength(filepath);
    int nameSize = StringLength(state->ui.texturesNames[element->index]);
    assert(headerSize + nameSize < 10000);

    memcpy(filepath + headerSize, state->ui.texturesNames[element->index], nameSize);

    filepath[headerSize + nameSize] = '\0';

    printf("path created: %s\n", filepath);

    TMTexture *texture = TMRendererTextureCreate(state->renderer, filepath);
    
    TMDarrayPush(state->textures, texture, TMTexture *);
    TMDarrayPush(state->texturesAddedNames, state->ui.texturesNames[element->index], char *);

    int textureIndex = TMDarraySize(state->texturesAddedNames) - 1;


    int index = (TMDarraySize(state->textures) - 1) / 8;
    if(TMDarraySize(state->textures) <= 3*8) {

        TMVec4 uvs = {0, 0, 1, 1};
        TMUIElementAddChildImageButton(state->ui.texturesChilds[index], TM_UI_ORIENTATION_HORIZONTAL,
                                       texture, uvs, uvs, ElementSelected, state, textureIndex);

    }
    state->loadOption = LOAD_OPTION_NONE;

}

static void SelectShader(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;
    state->loadOption = LOAD_OPTION_NONE;
}


static void LoadScene(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;
    if(state->loadOption != LOAD_OPTION_SCENE) {
        state->loadOption = LOAD_OPTION_SCENE;
    }
    else {
        state->loadOption = LOAD_OPTION_NONE;
    }
}

static void SelectScene(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;
    
    state->currentSceneName = state->ui.scenesNames[element->index];

    char filepath[10000] = "../../assets/json/";
    int headerSize = StringLength(filepath);
    int nameSize = StringLength(state->currentSceneName);
    assert(headerSize + nameSize < 10000);
    memcpy(filepath + headerSize, state->currentSceneName, nameSize);
    filepath[headerSize + nameSize] = '\0';


    LoadSceneFromFile(state, filepath);
    
    state->loadOption = LOAD_OPTION_NONE;


}

static void RecalculateEntitiesIds(Entity *entities) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities + i;
        entity->id = i;
    }

}

// TODO: fix this
static void DeleteEntity(EditorState *state, Entity *entity) {

    if(state->entities) {
        if(TMDarraySize(state->entities) > 0) {
            int index = -1;
            for(int i = 0; i < TMDarraySize(state->entities); ++i) {
                Entity *other = state->entities + i;
                if(other->id == entity->id) {
                    index = i;
                }
            }
            if(index >= 0) {
                Entity *entityToDelete = &state->entities[index];
                if(entityToDelete->collision) free(entityToDelete->collision);
                if(entityToDelete->animation) free(entityToDelete->animation);
                if(entityToDelete->enemyShot) free(entityToDelete->enemyShot);
                state->entities[index] = state->entities[TMDarraySize(state->entities) - 1];
                TMDarrayModifySize(state->entities, TMDarraySize(state->entities) - 1);
                printf("entities count: %d\n", TMDarraySize(state->entities));
                if(TMDarraySize(state->entities) > 0) {
                    RecalculateEntitiesIds(state->entities);
                }
                else {
                    state->entities = NULL;
                }
            }
        }
    }
    
}

static void DeleteLight(EditorState *state, int light) {

    LightsConstBuffer *lightsConstBuffer = &state->lightsConstBuffer;

    if(lightsConstBuffer->count > 0) {
        lightsConstBuffer->lights[light] = lightsConstBuffer->lights[lightsConstBuffer->count - 1];
        lightsConstBuffer->count--;
        TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
    }

}

static void DeleteSelectedEntity(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;

    // Delete entity
    if(state->selectedEntity) {
        DeleteEntity(state, state->selectedEntity);
        state->selectedEntity = NULL;
    }

    // Delete light
    if(state->lightSelected >= 0) {
        DeleteLight(state, state->lightSelected);
        state->lightSelected = -1;
    }

}

static void IncrementAmbienRed(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    TMVec3 *ambient = &state->lightsConstBuffer.ambient;
    ambient->x += 0.01f;
    ambient->x = MinF32(ambient->x, 1.0f);
    printf("ambient red: %f green: %f blue: %f\n", ambient->x, ambient->y, ambient->z);
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

}


static void DecrementAmbienRed(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    TMVec3 *ambient = &state->lightsConstBuffer.ambient;
    ambient->x -= 0.01f;
    ambient->x = MaxF32(ambient->x, 0.0f);
    printf("ambient red: %f green: %f blue: %f\n", ambient->x, ambient->y, ambient->z);
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

}


static void IncrementAmbienGreen(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    TMVec3 *ambient = &state->lightsConstBuffer.ambient;
    ambient->y += 0.01f;
    ambient->y = MinF32(ambient->y, 1.0f);
    printf("ambient red: %f green: %f blue: %f\n", ambient->x, ambient->y, ambient->z);
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

}

static void DecrementAmbienGreen(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    TMVec3 *ambient = &state->lightsConstBuffer.ambient;
    ambient->y -= 0.01f;
    ambient->y = MaxF32(ambient->y, 0.0f);
    printf("ambient red: %f green: %f blue: %f\n", ambient->x, ambient->y, ambient->z);
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

}

static void IncrementAmbienBlue(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    TMVec3 *ambient = &state->lightsConstBuffer.ambient;
    ambient->z += 0.01f;
    ambient->z = MinF32(ambient->z, 1.0f);
    printf("ambient red: %f green: %f blue: %f\n", ambient->x, ambient->y, ambient->z);
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

}

static void DecrementAmbienBlue(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    TMVec3 *ambient = &state->lightsConstBuffer.ambient;
    ambient->z -= 0.01f;
    ambient->z = MaxF32(ambient->z, 0.0f);
    printf("ambient red: %f green: %f blue: %f\n", ambient->x, ambient->y, ambient->z);
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

}

void EditorUIInitialize(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel) {

    LoadFileNamesFromDirectory("../../assets/json", &ui->scenesNames);
    LoadFileNamesFromDirectory("../../assets/images", &ui->texturesNames);
    LoadFileNamesFromDirectory("../../assets/shaders", &ui->shadersNames);

    ui->options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {6, 0.4}, {0.1, 0.1, 0.1, 1});
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Textures    ", {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Colors      ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Prefabs     ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Lights      ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Clear Brush ",   {1, 1, 1, 1}, ClearSelected, state);

    ui->textures = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.4f, 0.1f, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    ui->texturesChilds[0] = TMUIElementGetChild(ui->textures, 2);
    ui->texturesChilds[1] = TMUIElementGetChild(ui->textures, 1);
    ui->texturesChilds[2] = TMUIElementGetChild(ui->textures, 0);

    TMUIElement *child = NULL;
    ui->colors = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    child = TMUIElementGetChild(ui->colors, 0);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.1f + 0.1f*(float)i, 0, 0, 1}, ElementSelected, state);
    }
    child = TMUIElementGetChild(ui->colors, 1);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.1f + 0.1f*(float)i, 0, 1}, ElementSelected, state);
    }
    child = TMUIElementGetChild(ui->colors, 2);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0, 0.1f + 0.1f*(float)i, 1}, ElementSelected, state);
    }

    ui->prefabs = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildLabel(ui->prefabs, TM_UI_ORIENTATION_VERTICAL,  " Move Enemy Prefab ", {1, 1, 1, 1}, MoveEnemyPrefabSelected, state);
    TMUIElementAddChildLabel(ui->prefabs, TM_UI_ORIENTATION_VERTICAL,  " Shot Enemy Prefab ", {1, 1, 1, 1}, ShotEnemyPrefabSelected, state);
    TMUIElementAddChildLabel(ui->prefabs, TM_UI_ORIENTATION_VERTICAL,  " Player Prefab     ", {1, 1, 1, 1}, PlayerPrefabSelected,    state);
    TMUIElementAddChildLabel(ui->prefabs, TM_UI_ORIENTATION_VERTICAL,  " Win Prefab        ", {1, 1, 1, 1}, WinPrefabSelected,       state);
    TMUIElementAddChildLabel(ui->prefabs, TM_UI_ORIENTATION_VERTICAL,  " GameOver Prefab   ", {1, 1, 1, 1}, GameOverPrefabSelected,  state);

    ui->modify = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {8.0, 0}, {4.8, 2}, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildLabel(ui->modify, TM_UI_ORIENTATION_VERTICAL,   " Scale ", {1, 1, 1, 1}, ScaleEntity, state);
    TMUIElementAddChildLabel(ui->modify, TM_UI_ORIENTATION_VERTICAL,   " Translate ", {1, 1, 1, 1}, TranslateEntity, state);
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    child = TMUIElementGetChild(ui->modify, 2);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc absU ", {1, 1, 1, 1}, IncrementAbsU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc absV ", {1, 1, 1, 1}, IncrementAbsV, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off absU ", {1, 1, 1, 1}, OffsetAbsU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off absV ", {1, 1, 1, 1}, OffsetAbsV, state);
    child = TMUIElementGetChild(ui->modify, 3);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc relU ", {1, 1, 1, 1}, IncrementRelU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc relV ", {1, 1, 1, 1}, IncrementRelV, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off relU ", {1, 1, 1, 1}, OffsetRelU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off relV ", {1, 1, 1, 1}, OffsetRelV, state);
    child = TMUIElementGetChild(ui->modify, 4);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc z ", {1, 1, 1, 1}, IncrementZ, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " dec z ", {1, 1, 1, 1}, DecrementZ, state);
    child = TMUIElementGetChild(ui->modify, 5);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " add Collider ", {1, 1, 1, 1}, AddCollision, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " rem Collider ", {1, 1, 1, 1}, RemCollision, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Solid ",        {1, 1, 1, 1}, SolidCollision, state);

    ui->shotEnemyModify = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {8.0, 2}, {4.8, 1}, {0.1f, 0.2f, 0.1f, 1});
    TMUIElementAddChildLabel(ui->shotEnemyModify, TM_UI_ORIENTATION_VERTICAL,   " facing ", {1, 1, 1, 1}, EnemyShotDir, state);
    TMUIElementAddChildLabel(ui->shotEnemyModify, TM_UI_ORIENTATION_VERTICAL,   " range ", {1, 1, 1, 1}, EnemyShotRange, state);
    TMUIElementAddChildLabel(ui->shotEnemyModify, TM_UI_ORIENTATION_VERTICAL,   " speed ", {1, 1, 1, 1}, EnemyShotSpeed, state);
    
    ui->save = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0.0f, height/meterToPixel - 0.25f}, {18.0, 0.25}, {0.1f, 0.1f, 0.1f, 1.0f});
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Save Scene    ", {1, 1, 1, 1}, SaveScene, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Scene    ", {1, 1, 1, 1}, LoadScene, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Texture  ", {1, 1, 1, 1}, LoadTexture, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Shader   ", {1, 1, 1, 1}, LoadShader, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Delete Entity ", {1, 1, 1, 1}, DeleteSelectedEntity, state);
    TMUIElementAddChildButton(ui->save, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});

    child = TMUIElementGetChild(ui->save, 5);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " +R ", {1, 1, 1, 1}, IncrementAmbienRed, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " -R ", {1, 1, 1, 1}, DecrementAmbienRed, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " +G ", {1, 1, 1, 1}, IncrementAmbienGreen, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " -G ", {1, 1, 1, 1}, DecrementAmbienGreen, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " +B ", {1, 1, 1, 1}, IncrementAmbienBlue, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " -B ", {1, 1, 1, 1}, DecrementAmbienBlue, state);


    ui->loadScene = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {3.0f, height/meterToPixel - 0.25f - 3.5f}, {2.5, 3.5}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->scenesNames) {
        for(int i = 0; i < TMDarraySize(ui->scenesNames); ++i) {
            TMUIElementAddChildLabel(ui->loadScene, TM_UI_ORIENTATION_VERTICAL, ui->scenesNames[i], {1, 1, 1, 1}, SelectScene, state);
        }
    }

    ui->loadTexture = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {6.0f, height/meterToPixel - 0.25f - 4.0f}, {2.5, 4}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->texturesNames) {
        for(int i = 0; i < TMDarraySize(ui->texturesNames); ++i) {
            TMUIElementAddChildLabel(ui->loadTexture, TM_UI_ORIENTATION_VERTICAL, ui->texturesNames[i], {1, 1, 1, 1}, SelectTexture, state);
        }
    }

    ui->loadShader = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {9.0f, height/meterToPixel - 0.25f - 4.5f}, {2.5, 4.5}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->shadersNames) {
        for(int i = 0; i < TMDarraySize(ui->shadersNames); ++i) {
            TMUIElementAddChildLabel(ui->loadShader, TM_UI_ORIENTATION_VERTICAL, ui->shadersNames[i], {1, 1, 1, 1}, SelectShader, state);
        }
    }



    ui->lights = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.1f, 0.1f, 1});

    ui->clear = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.1f, 0.1f, 1});

    ui->lightModify = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {8.0, 0}, {4.8, 2}, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildLabel(ui->lightModify, TM_UI_ORIENTATION_VERTICAL,   " Translate ", {1, 1, 1, 1}, TranslateLight, state);
    TMUIElementAddChildButton(ui->lightModify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->lightModify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});

    child = TMUIElementGetChild(ui->lightModify, 1);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " constant  ", {1, 1, 1, 1}, Constant, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " linear    ",    {1, 1, 1, 1}, Linear, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " quadratic ", {1, 1, 1, 1}, Quadratic, state);

    child = TMUIElementGetChild(ui->lightModify, 2);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Red   ",      {1, 1, 1, 1}, Red, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Green ",    {1, 1, 1, 1}, Green, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Blue  ",     {1, 1, 1, 1}, Blue, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Range ",     {1, 1, 1, 1}, Range, state);
}

void EditorUIUpdate(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel) {

    TMVec3 pos = state->cameraP;
    TMUIElementProcessInput(ui->save, pos.x, pos.y, width, height, meterToPixel);
    TMUIElementProcessInput(ui->options, pos.x, pos.y, width, height, meterToPixel);
    if(state->option == OPTION_TEXTURE) {
        TMUIElementProcessInput(ui->textures, pos.x, pos.y, width, height, meterToPixel);
    }
    else if (state->option == OPTION_COLOR) {
        TMUIElementProcessInput(ui->colors, pos.x, pos.y, width, height, meterToPixel);
    }
    else if (state->option == OPTION_PREFABS) {
        TMUIElementProcessInput(ui->prefabs, pos.x, pos.y, width, height, meterToPixel);
    }
    else if(state->option == OPTION_LIGHT) {
        TMUIElementProcessInput(ui->lights, pos.x, pos.y, width, height, meterToPixel);
    }
    else if(state->option == OPTION_CLEAR) {
        TMUIElementProcessInput(ui->clear, pos.x, pos.y, width, height, meterToPixel);
    }

    if(!state->element && state->selectedEntity) {
        TMUIElementProcessInput(ui->modify, pos.x, pos.y, width, height, meterToPixel);
        if(state->selectedEntity->prefabType == PREFAB_TYPE_SHOT_ENEMY) {
            TMUIElementProcessInput(ui->shotEnemyModify, pos.x, pos.y, width, height, meterToPixel);
        }
    }

    if(!state->element && state->lightSelected >= 0) {
        TMUIElementProcessInput(ui->lightModify, pos.x, pos.y, width, height, meterToPixel);
    }

    if(state->loadOption == LOAD_OPTION_TEXTURE) {
        TMUIElementProcessInput(ui->loadTexture, pos.x, pos.y, width, height, meterToPixel);
    }

    if(state->loadOption == LOAD_OPTION_SHADER) {
        TMUIElementProcessInput(ui->loadShader, pos.x, pos.y, width, height, meterToPixel);
    }
    if(state->loadOption == LOAD_OPTION_SCENE) {
        TMUIElementProcessInput(ui->loadScene, pos.x, pos.y, width, height, meterToPixel);
    }

    state->mouseIsHot = false;
    TMUIMouseIsHot(ui->options,  &state->mouseIsHot);
    TMUIMouseIsHot(ui->textures, &state->mouseIsHot);
    TMUIMouseIsHot(ui->colors,   &state->mouseIsHot);
    TMUIMouseIsHot(ui->prefabs,   &state->mouseIsHot);
    TMUIMouseIsHot(ui->lights,   &state->mouseIsHot);
    TMUIMouseIsHot(ui->clear,   &state->mouseIsHot);
    TMUIMouseIsHot(ui->save,     &state->mouseIsHot);
    if(!state->element && state->selectedEntity) {
        TMUIMouseIsHot(ui->modify, &state->mouseIsHot);
        if(state->selectedEntity->prefabType == PREFAB_TYPE_SHOT_ENEMY) {
            TMUIMouseIsHot(ui->shotEnemyModify, &state->mouseIsHot);
        }
    }
    if(!state->element && state->lightSelected >= 0) {
        TMUIMouseIsHot(ui->lightModify, &state->mouseIsHot);
    }
    if(state->loadOption == LOAD_OPTION_TEXTURE) {
        TMUIMouseIsHot(ui->loadTexture, &state->mouseIsHot);
    }
    if(state->loadOption == LOAD_OPTION_SHADER) {
        TMUIMouseIsHot(ui->loadShader, &state->mouseIsHot);
    }
    if(state->loadOption == LOAD_OPTION_SCENE) {
        TMUIMouseIsHot(ui->loadScene, &state->mouseIsHot);
    }

}

void EditorUIDraw(EditorState *state, EditorUI *ui, TMRenderer *renderer) {

    TMUIElementDraw(renderer, ui->options, 0.0f);
    TMUIElementDraw(renderer, ui->save, 0.0f);

    if(state->option == OPTION_TEXTURE) {
        TMUIElementDraw(renderer, ui->textures, 0.0f);
    }
    else if (state->option == OPTION_COLOR) {
        TMUIElementDraw(renderer, ui->colors, 0.0f);
    }
    else if(state->option == OPTION_PREFABS) {
        TMUIElementDraw(renderer, ui->prefabs, 0.0f);
    }
    else if(state->option == OPTION_LIGHT) {
        TMUIElementDraw(renderer, ui->lights, 0.0f);
    }
    else if(state->option == OPTION_CLEAR) {
        TMUIElementDraw(renderer, ui->lights, 0.0f);
    }

    if(state->loadOption == LOAD_OPTION_TEXTURE) {
        TMUIElementDraw(renderer, ui->loadTexture, 0.0f);
    }
    if(state->loadOption == LOAD_OPTION_SHADER) {
        TMUIElementDraw(renderer, ui->loadShader, 0.0f);
    }
    if(state->loadOption == LOAD_OPTION_SCENE) {
        TMUIElementDraw(renderer, ui->loadScene, 0.0f);
    }

    if(!state->element && state->selectedEntity) {
        TMUIElementDraw(renderer, ui->modify, 0.0f);
        if(state->selectedEntity->prefabType == PREFAB_TYPE_SHOT_ENEMY) {
            TMUIElementDraw(renderer, ui->shotEnemyModify, 0.0f);
        }
    }
    if(!state->element && state->lightSelected >= 0) {
        TMUIElementDraw(renderer, ui->lightModify, 0.0f);
    }
}

void EditorUIShutdown(EditorUI *ui) {

    TMUIElementDestroy(ui->save);
    TMUIElementDestroy(ui->modify);
    TMUIElementDestroy(ui->shotEnemyModify);
    TMUIElementDestroy(ui->clear);
    TMUIElementDestroy(ui->lights);
    TMUIElementDestroy(ui->prefabs);
    TMUIElementDestroy(ui->colors);
    TMUIElementDestroy(ui->textures);
    TMUIElementDestroy(ui->options);
    TMUIElementDestroy(ui->loadTexture);
    TMUIElementDestroy(ui->loadShader);
    TMUIElementDestroy(ui->loadScene);
    FreeFileNames(&ui->shadersNames);
    FreeFileNames(&ui->texturesNames);
    FreeFileNames(&ui->scenesNames);

}
