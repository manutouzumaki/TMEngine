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

static void EnemyPrefabSelected(TMUIElement *element) {
    printf("Prefab enemy selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_ENEMY;
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

static void SaveScene(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;
    printf("Scene Saved\n");

    TMJsonObject jsonRoot = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonRoot, "Root");

    TMJsonObject jsonLevelTextures = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonLevelTextures, "LevelTextures");

    for(int i = 0; i < TMDarraySize(state->texturesAddedNames); ++i) {
        const char *textureName =  (const char *)state->texturesAddedNames[i];
        TMJsonObjectSetValue(&jsonLevelTextures, textureName);
    
    }
    TMJsonObjectAddChild(&jsonRoot, &jsonLevelTextures);

    TMJsonObject jsonLevelAmbient = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonLevelAmbient, "LevelAmbient");
    TMJsonObjectSetValue(&jsonLevelAmbient, state->lightsConstBuffer.ambient.x);
    TMJsonObjectSetValue(&jsonLevelAmbient, state->lightsConstBuffer.ambient.y);
    TMJsonObjectSetValue(&jsonLevelAmbient, state->lightsConstBuffer.ambient.z);
    TMJsonObjectAddChild(&jsonRoot, &jsonLevelAmbient);

    TMJsonObject jsonLevelLights = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonLevelLights, "LevelLights");
    for(int i = 0; i < state->lightsConstBuffer.count; ++i) {

        PointLight *light = state->lightsConstBuffer.lights + i;

        TMJsonObject jsonPosition = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonPosition, "Position");
        TMJsonObjectSetValue(&jsonPosition, light->position.x);
        TMJsonObjectSetValue(&jsonPosition, light->position.y);

        TMJsonObject jsonColor = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonColor, "Color");
        TMJsonObjectSetValue(&jsonColor, light->color.x);
        TMJsonObjectSetValue(&jsonColor, light->color.y);
        TMJsonObjectSetValue(&jsonColor, light->color.z);

        TMJsonObject jsonAttrib = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonAttrib, "Attributes");
        TMJsonObjectSetValue(&jsonAttrib, light->attributes.x);
        TMJsonObjectSetValue(&jsonAttrib, light->attributes.y);
        TMJsonObjectSetValue(&jsonAttrib, light->attributes.z);

        TMJsonObject jsonRange = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonRange, "Range");
        TMJsonObjectSetValue(&jsonRange, light->range);

        TMJsonObject jsonLight = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonLight, "Light");
        TMJsonObjectAddChild(&jsonLight, &jsonPosition);
        TMJsonObjectAddChild(&jsonLight, &jsonColor);
        TMJsonObjectAddChild(&jsonLight, &jsonAttrib);
        TMJsonObjectAddChild(&jsonLight, &jsonRange); 

        TMJsonObjectAddChild(&jsonLevelLights, &jsonLight); 
    }
    TMJsonObjectAddChild(&jsonRoot, &jsonLevelLights);


    TMJsonObject jsonScene = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonScene, "Scene");

    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities + i;

        TMJsonObject jsonEntity = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonEntity, "Entity");

        TMJsonObject jsonType = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonType, "PrefabType");
        TMJsonObjectSetValue(&jsonType, (float)entity->prefabType);
        TMJsonObjectAddChild(&jsonEntity, &jsonType);
        
        // Save Graphic Component
        {
            TMJsonObject jsonGraphic = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonGraphic, "Graphics");

            // graphics type
            TMJsonObject jsonType = TMJsonObjectCreate();

            TMJsonObjectSetName(&jsonType, "Type");
            if(entity->shader == state->colorShader) {
                TMJsonObjectSetValue(&jsonType, 0.0f);
            }
            else if(entity->shader == state->spriteShader) {
                TMJsonObjectSetValue(&jsonType, 1.0f);
            }

            // position
            TMJsonObject jsonPosition = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonPosition, "Position");
            TMJsonObject xPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&xPos, "X");
            TMJsonObjectSetValue(&xPos, entity->position.x);
            TMJsonObject yPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&yPos, "Y");
            TMJsonObjectSetValue(&yPos, entity->position.y);
            TMJsonObjectAddChild(&jsonPosition, &xPos);
            TMJsonObjectAddChild(&jsonPosition, &yPos);

            // size
            TMJsonObject jsonSize = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonSize, "Size");
            TMJsonObject xSiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&xSiz, "X");
            TMJsonObjectSetValue(&xSiz, entity->size.x);
            TMJsonObject ySiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&ySiz, "Y");
            TMJsonObjectSetValue(&ySiz, entity->size.y);
            TMJsonObjectAddChild(&jsonSize, &xSiz);
            TMJsonObjectAddChild(&jsonSize, &ySiz);

            // color
            TMJsonObject jsonColor = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonColor, "Color");
            TMJsonObject r = TMJsonObjectCreate();
            TMJsonObjectSetName(&r, "R");
            TMJsonObjectSetValue(&r, entity->color.x);
            TMJsonObject g = TMJsonObjectCreate();
            TMJsonObjectSetName(&g, "G");
            TMJsonObjectSetValue(&g, entity->color.y);
            TMJsonObject b = TMJsonObjectCreate();
            TMJsonObjectSetName(&b, "B");
            TMJsonObjectSetValue(&b, entity->color.z);
            TMJsonObject a = TMJsonObjectCreate();
            TMJsonObjectSetName(&a, "A");
            TMJsonObjectSetValue(&a, entity->color.w);
            TMJsonObjectAddChild(&jsonColor, &r);
            TMJsonObjectAddChild(&jsonColor, &g);
            TMJsonObjectAddChild(&jsonColor, &b);
            TMJsonObjectAddChild(&jsonColor, &a);
            
            // Texture
            TMJsonObject jsonAbsUVs = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonAbsUVs, "AbsUvs");
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[0]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[1]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[2]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[3]);
            TMJsonObject jsonRelUVs = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonRelUVs, "RelUvs");
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[0]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[1]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[2]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[3]);

            TMJsonObject jsonZIndex = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonZIndex, "ZIndex");
            TMJsonObjectSetValue(&jsonZIndex, entity->zIndex);

            TMJsonObject jsonTextureIndex = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonTextureIndex, "TextureIndex");
            TMJsonObjectSetValue(&jsonTextureIndex, entity->textureIndex);

            TMJsonObjectAddChild(&jsonGraphic, &jsonType);
            TMJsonObjectAddChild(&jsonGraphic, &jsonPosition);
            TMJsonObjectAddChild(&jsonGraphic, &jsonSize);
            TMJsonObjectAddChild(&jsonGraphic, &jsonColor);
            TMJsonObjectAddChild(&jsonGraphic, &jsonAbsUVs);
            TMJsonObjectAddChild(&jsonGraphic, &jsonRelUVs);
            TMJsonObjectAddChild(&jsonGraphic, &jsonZIndex);
            TMJsonObjectAddChild(&jsonGraphic, &jsonTextureIndex);
        
            TMJsonObjectAddChild(&jsonEntity, &jsonGraphic);

        }
        if(entity->prefabType != PREFAB_TYPE_NONE) {
        
            // add Physics Component
            {
                TMJsonObject jsonPhysics = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonPhysics, "Physics");

                // position
                TMJsonObject jsonPosition = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonPosition, "Position");
                TMJsonObject xPos = TMJsonObjectCreate();
                TMJsonObjectSetName(&xPos, "X");
                TMJsonObjectSetValue(&xPos, entity->position.x);
                TMJsonObject yPos = TMJsonObjectCreate();
                TMJsonObjectSetName(&yPos, "Y");
                TMJsonObjectSetValue(&yPos, entity->position.y);
                TMJsonObjectAddChild(&jsonPosition, &xPos);
                TMJsonObjectAddChild(&jsonPosition, &yPos);

                TMJsonObject jsonVelocity = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonVelocity, "Velocity");
                TMJsonObject xVel = TMJsonObjectCreate();
                TMJsonObjectSetName(&xVel, "X");
                TMJsonObjectSetValue(&xVel, 0.0f);
                TMJsonObject yVel = TMJsonObjectCreate();
                TMJsonObjectSetName(&yVel, "Y");
                TMJsonObjectSetValue(&yVel, 0.0f);
                TMJsonObjectAddChild(&jsonVelocity, &xVel);
                TMJsonObjectAddChild(&jsonVelocity, &yVel);

                TMJsonObject jsonAccel = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonAccel, "Acceleration");
                TMJsonObject xAcc = TMJsonObjectCreate();
                TMJsonObjectSetName(&xAcc, "X");
                TMJsonObjectSetValue(&xAcc, 0.0f);
                TMJsonObject yAcc = TMJsonObjectCreate();
                TMJsonObjectSetName(&yAcc, "Y");
                TMJsonObjectSetValue(&yAcc, 0.0f);
                TMJsonObjectAddChild(&jsonAccel, &xAcc);
                TMJsonObjectAddChild(&jsonAccel, &yAcc);

                TMJsonObject jsonDamping = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonDamping, "Damping");
                TMJsonObjectSetValue(&jsonDamping, 0.01f);

                TMJsonObjectAddChild(&jsonPhysics, &jsonPosition);
                TMJsonObjectAddChild(&jsonPhysics, &jsonVelocity);
                TMJsonObjectAddChild(&jsonPhysics, &jsonAccel);
                TMJsonObjectAddChild(&jsonPhysics, &jsonDamping);

                TMJsonObjectAddChild(&jsonEntity, &jsonPhysics);
            }

            if(entity->prefabType == PREFAB_TYPE_PLAYER) {
                TMJsonObject jsonInput = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonInput, "Input");
                TMJsonObjectSetValue(&jsonInput, 1.0f);
                TMJsonObjectAddChild(&jsonEntity, &jsonInput);
            }
        }

        if(entity->animation) {

            TMJsonObject jsonAnimation = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonAnimation, "Animation");

            TMJsonObject statesCount = TMJsonObjectCreate();
            TMJsonObjectSetName(&statesCount, "AnimationStatesCount");
            TMJsonObjectSetValue(&statesCount, (float)entity->animation->statesCount);

            TMJsonObjectAddChild(&jsonAnimation, &statesCount);

            TMJsonObject animationIndex = TMJsonObjectCreate();
            TMJsonObjectSetName(&animationIndex, "AnimationIndex");
            TMJsonObjectSetValue(&animationIndex, (float)entity->animation->index);

            TMJsonObjectAddChild(&jsonAnimation, &animationIndex);

            for(int i = 0; i < entity->animation->statesCount; ++i) {
                TMJsonObject jsonAnimState = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonAnimState, "AnimationState");

                TMJsonObject frameCount = TMJsonObjectCreate();
                TMJsonObjectSetName(&frameCount, "FrameCount");
                TMJsonObjectSetValue(&frameCount, (float)entity->animation->states[i].frameCount);

                TMJsonObject frames = TMJsonObjectCreate();
                TMJsonObjectSetName(&frames, "Frames");
                for(int j = 0; j < entity->animation->states[i].frameCount; ++j) {
                    TMJsonObjectSetValue(&frames, (float)entity->animation->states[i].frames[j]);
                }

                TMJsonObject speed = TMJsonObjectCreate();
                TMJsonObjectSetName(&speed, "Speed");
                TMJsonObjectSetValue(&speed, (float)entity->animation->states[i].speed);

                TMJsonObjectAddChild(&jsonAnimState, &frameCount);
                TMJsonObjectAddChild(&jsonAnimState, &frames);
                TMJsonObjectAddChild(&jsonAnimState, &speed);

                TMJsonObjectAddChild(&jsonAnimation, &jsonAnimState);
            } 

            TMJsonObjectAddChild(&jsonEntity, &jsonAnimation);
        }

        if(entity->collision) {
            TMJsonObject jsonCollision = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonCollision, "Collision");

            TMJsonObject solid = TMJsonObjectCreate();
            TMJsonObjectSetName(&solid, "Solid");
            TMJsonObjectSetValue(&solid, (float)((int)entity->collision->solid));

            TMJsonObjectAddChild(&jsonCollision, &solid);

            switch(entity->collision->type) {
            
                case COLLISION_TYPE_AABB: {
                    TMJsonObject aabb = TMJsonObjectCreate();
                    TMJsonObjectSetName(&aabb, "AABB");

                    TMJsonObject min = TMJsonObjectCreate();
                    TMJsonObjectSetName(&min, "Min");
                    TMJsonObjectSetValue(&min, entity->collision->aabb.min.x);
                    TMJsonObjectSetValue(&min, entity->collision->aabb.min.y);

                    TMJsonObject max = TMJsonObjectCreate();
                    TMJsonObjectSetName(&max, "Max");
                    TMJsonObjectSetValue(&max, entity->collision->aabb.max.x);
                    TMJsonObjectSetValue(&max, entity->collision->aabb.max.y);

                    TMJsonObjectAddChild(&aabb, &min);
                    TMJsonObjectAddChild(&aabb, &max);

                    TMJsonObjectAddChild(&jsonCollision, &aabb);
                    
                } break;
                case COLLISION_TYPE_CIRCLE: {
                    // TODO: ...

                } break;
                case COLLISION_TYPE_CAPSULE: {
                    TMJsonObject capsule = TMJsonObjectCreate();
                    TMJsonObjectSetName(&capsule, "Capsule");

                    TMJsonObject a = TMJsonObjectCreate();
                    TMJsonObjectSetName(&a, "A");
                    TMJsonObjectSetValue(&a, entity->collision->capsule.a.x);
                    TMJsonObjectSetValue(&a, entity->collision->capsule.a.y);

                    TMJsonObject b = TMJsonObjectCreate();
                    TMJsonObjectSetName(&b, "B");
                    TMJsonObjectSetValue(&b, entity->collision->capsule.b.x);
                    TMJsonObjectSetValue(&b, entity->collision->capsule.b.y);

                    TMJsonObject r = TMJsonObjectCreate();
                    TMJsonObjectSetName(&r, "Radio");
                    TMJsonObjectSetValue(&r, entity->collision->capsule.r);

                    TMJsonObjectAddChild(&capsule, &a);
                    TMJsonObjectAddChild(&capsule, &b);
                    TMJsonObjectAddChild(&capsule, &r);

                    TMJsonObjectAddChild(&jsonCollision, &capsule);

                } break;

            }
 
            TMJsonObjectAddChild(&jsonEntity, &jsonCollision);

        }

        TMJsonObjectAddChild(&jsonScene, &jsonEntity);

    }

    TMJsonObjectAddChild(&jsonRoot, &jsonScene);

    int bytesCount = 0;
    TMJsonObjectStringify(&jsonRoot, NULL, &bytesCount);
    
    char *buffer = (char *)malloc(bytesCount + 1);
    int bytesWriten = 0;
    TMJsonObjectStringify(&jsonRoot, buffer, &bytesWriten);
    printf("%s", buffer);


    if(state->currentSceneName) {

        char filepath[10000] = "../../assets/json/";
        int headerSize = StringLength(filepath);
        int nameSize = StringLength(state->currentSceneName);
        assert(headerSize + nameSize < 10000);
        memcpy(filepath + headerSize, state->currentSceneName, nameSize);
        filepath[headerSize + nameSize] = '\0';
        TMFileWriteText(filepath, buffer, bytesWriten);

    }
    else {
        TMFileWriteText("../../assets/json/testScene.json", buffer, bytesWriten);
    }
    
    free(buffer);

    TMJsonObjectFree(&jsonRoot);
}

void EditorUIInitialize(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel) {

    LoadFileNamesFromDirectory("../../assets/json", &ui->scenesNames);
    LoadFileNamesFromDirectory("../../assets/images", &ui->texturesNames);
    LoadFileNamesFromDirectory("../../assets/shaders", &ui->shadersNames);

    ui->options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {6, 0.4}, {0.1, 0.1, 0.1, 1});
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Textures ", {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Colors ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Prefabs ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Lights ",   {1, 1, 1, 1}, OptionSelected, state);
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

    ui->prefabs = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    child = TMUIElementGetChild(ui->prefabs, 0);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.5f, 0.5f, 1}, EnemyPrefabSelected, state);
    child = TMUIElementGetChild(ui->prefabs, 1);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.5f, 0.5f, 0, 1}, PlayerPrefabSelected, state);

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
    
    ui->save = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0.0f, height/meterToPixel - 0.25f}, {8.0, 0.25}, {0.1f, 0.1f, 0.1f, 1.0f});
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Save Scene ", {1, 1, 1, 1}, SaveScene, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Scene ", {1, 1, 1, 1}, LoadScene, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Texture ", {1, 1, 1, 1}, LoadTexture, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Shader ", {1, 1, 1, 1}, LoadShader, state);

    ui->loadScene = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {2.0f, height/meterToPixel - 0.25f - 3.5f}, {2.5, 3.5}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->scenesNames) {
        for(int i = 0; i < TMDarraySize(ui->scenesNames); ++i) {
            TMUIElementAddChildLabel(ui->loadScene, TM_UI_ORIENTATION_VERTICAL, ui->scenesNames[i], {1, 1, 1, 1}, SelectScene, state);
        }
    }

    ui->loadTexture = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {4.0f, height/meterToPixel - 0.25f - 4.0f}, {2.5, 4}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->texturesNames) {
        for(int i = 0; i < TMDarraySize(ui->texturesNames); ++i) {
            TMUIElementAddChildLabel(ui->loadTexture, TM_UI_ORIENTATION_VERTICAL, ui->texturesNames[i], {1, 1, 1, 1}, SelectTexture, state);
        }
    }

    ui->loadShader = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {6.0f, height/meterToPixel - 0.25f - 4.5f}, {2.5, 4.5}, {0.02f, 0.02f, 0.02f, 1.0f});
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
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " constant ", {1, 1, 1, 1}, Constant, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " linear ",    {1, 1, 1, 1}, Linear, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " quadratic ", {1, 1, 1, 1}, Quadratic, state);

    child = TMUIElementGetChild(ui->lightModify, 2);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Red ",      {1, 1, 1, 1}, Red, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Green ",    {1, 1, 1, 1}, Green, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Blue ",     {1, 1, 1, 1}, Blue, state);
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
    }
    if(!state->element && state->lightSelected >= 0) {
        TMUIElementDraw(renderer, ui->lightModify, 0.0f);
    }
}

void EditorUIShutdown(EditorUI *ui) {

    TMUIElementDestroy(ui->save);
    TMUIElementDestroy(ui->modify);
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
