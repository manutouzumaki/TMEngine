#include "scene.h"
#include "editor.h"

#include <utils/tm_json.h>
#include <tm_ui.h>

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

static float StringToFloat(const char *c, size_t size) {
    assert(size < 32);
    static char buffer[32];
    memcpy((void *)buffer, (void *)c, size);
    buffer[size] = '\0';
    return (float)atof(buffer);
}

static unsigned int StringToInt(const char *c, size_t size) {
    assert(size < 32);
    static char buffer[32];
    memcpy((void *)buffer, (void *)c, size);
    buffer[size] = '\0';
    return (int)atoi(buffer);
}

static unsigned int StringToUnsignedInt(const char *c, size_t size) {
    assert(size < 32);
    static char buffer[32];
    memcpy((void *)buffer, (void *)c, size);
    buffer[size] = '\0';
    return (unsigned int)atoi(buffer);
}

static const char *StringToNullTerString(const char *c, size_t size) {
    char *nullTerString = (char *)malloc(size + 1);
    memcpy((void *)nullTerString, (void *)c, size);
    nullTerString[size] = '\0';
    return (const char *)nullTerString;
}

static int StringLength(char *string) {
    int counter = 0;
    char *letter = string; 
    while(*letter++ != L'\0') counter++;
    return counter;
}


static void ClearTextures(EditorState *state) {

    if(state->textures) {
        for(int i = 0; i < TMDarraySize(state->textures); ++i) {
            TMTexture *texture = state->textures[i];
            TMRendererTextureDestroy(state->renderer, texture);
        }

        TMDarrayDestroy(state->textures);
    }

    if(state->texturesAddedNames) TMDarrayDestroy(state->texturesAddedNames);
    state->textures = NULL;
    state->texturesAddedNames = NULL;

    TMUIElementFreechilds(state->ui.texturesChilds[0]);
    TMUIElementFreechilds(state->ui.texturesChilds[1]);
    TMUIElementFreechilds(state->ui.texturesChilds[2]);


}

static void LoadTextures(EditorState *state, TMJsonObject *jsonLevelTextures) {

    for(int i = 0; i < jsonLevelTextures->valuesCount; ++i) {
        TMJsonValue *jsonValue = jsonLevelTextures->values + i;

        char filepath[10000] = "../../assets/images/";
        int headerSize = StringLength(filepath);
        int nameSize = jsonValue->size;
        assert(headerSize + nameSize < 10000);

        memcpy(filepath + headerSize, jsonValue->value, nameSize);
        filepath[headerSize + nameSize] = '\0';

        printf("path created: %s\n", filepath);

        TMTexture *texture = TMRendererTextureCreate(state->renderer, filepath);
        
        TMDarrayPush(state->textures, texture, TMTexture *);

        int index = -1;
        if(state->ui.texturesNames) {
            for(int i = 0; i < TMDarraySize(state->ui.texturesNames); ++i) {
                if(strncmp(jsonValue->value, state->ui.texturesNames[i], jsonValue->size) == 0) {
                    index = i;
                }
            }
            if(index >= 0) {
                TMDarrayPush(state->texturesAddedNames, state->ui.texturesNames[index], char *);
                if(TMDarraySize(state->textures) <= 3*8) {
                    int textureIndex = TMDarraySize(state->texturesAddedNames) - 1;
                    int slot = (TMDarraySize(state->textures) - 1) / 8;
                    TMVec4 uvs = {0, 0, 1, 1};
                    TMUIElementAddChildImageButton(state->ui.texturesChilds[slot], TM_UI_ORIENTATION_HORIZONTAL,
                                                   texture, uvs, uvs, ElementSelected, state, textureIndex);

                }

            }
            else {
                printf("Error textures not found!!\n");
                return;
            }
        }
    }

}

void LoadLights(EditorState *state, TMJsonObject *jsonLevelAmbient, TMJsonObject *jsonLevelLights) {

    TMVec3 ambient  = {};
    ambient.x = StringToFloat(jsonLevelAmbient->values[0].value, jsonLevelAmbient->values[0].size);
    ambient.y = StringToFloat(jsonLevelAmbient->values[1].value, jsonLevelAmbient->values[1].size);
    ambient.z = StringToFloat(jsonLevelAmbient->values[2].value, jsonLevelAmbient->values[2].size);

    SetAmbientLight(state, ambient);

    for(int i = 0; i < jsonLevelLights->childsCount; ++i) {

        TMJsonObject *jsonLight = jsonLevelLights->childs + i;
        TMJsonObject *jsonPosition = TMJsonFindChildByName(jsonLight, "Position");
        TMJsonObject *jsonColor    = TMJsonFindChildByName(jsonLight, "Color");
        TMJsonObject *jsonAttrib   = TMJsonFindChildByName(jsonLight, "Attributes");
        TMJsonObject *jsonRange    = TMJsonFindChildByName(jsonLight, "Range");

        TMVec2 position  = {};
        position.x = StringToFloat(jsonPosition->values[0].value, jsonPosition->values[0].size);
        position.y = StringToFloat(jsonPosition->values[1].value, jsonPosition->values[1].size);

        TMVec3 color  = {};
        color.x = StringToFloat(jsonColor->values[0].value, jsonColor->values[0].size);
        color.y = StringToFloat(jsonColor->values[1].value, jsonColor->values[1].size);
        color.z = StringToFloat(jsonColor->values[2].value, jsonColor->values[2].size);

        TMVec3  attributes = {};
        attributes.x = StringToFloat(jsonAttrib->values[0].value, jsonAttrib->values[0].size);
        attributes.y = StringToFloat(jsonAttrib->values[1].value, jsonAttrib->values[1].size);
        attributes.z = StringToFloat(jsonAttrib->values[2].value, jsonAttrib->values[2].size);

        float range = StringToFloat(jsonRange->values[0].value, jsonRange->values[0].size);

        AddLight(state, position, attributes, color, range);
    }

}

extern TMTexture   *gPlayerTexture;

static void EntityAddGraphic(Entity *entity, TMJsonObject *jsonObject, EditorState *state) {

    TMJsonObject *jsonType     = TMJsonFindChildByName(jsonObject, "Type");
    TMJsonObject *jsonPosition = TMJsonFindChildByName(jsonObject, "Position"); 
    TMJsonObject *jsonSize     = TMJsonFindChildByName(jsonObject, "Size");
    TMJsonObject *jsonColor     = TMJsonFindChildByName(jsonObject, "Color");
    TMJsonObject *jsonAbsUVs      = TMJsonFindChildByName(jsonObject, "AbsUvs");
    TMJsonObject *jsonRelUVs    = TMJsonFindChildByName(jsonObject, "RelUvs");
    TMJsonObject *jsonZIndex    = TMJsonFindChildByName(jsonObject, "ZIndex");
    TMJsonObject *jsonTextureIndex    = TMJsonFindChildByName(jsonObject, "TextureIndex");

    TMJsonValue jsonValue = jsonPosition->childs[0].values[0];
    float xPos = StringToFloat(jsonValue.value, jsonValue.size);
    jsonValue = jsonPosition->childs[1].values[0];
    float yPos = StringToFloat(jsonValue.value, jsonValue.size);
    TMVec2 position = {xPos, yPos};

    jsonValue = jsonSize->childs[0].values[0];
    float xSiz = StringToFloat(jsonValue.value, jsonValue.size);
    jsonValue = jsonSize->childs[1].values[0];
    float ySiz = StringToFloat(jsonValue.value, jsonValue.size);
    TMVec2 size = {xSiz, ySiz};

    int zIndex = StringToInt(jsonZIndex->values[0].value, jsonZIndex->values[0].size); 
    int textureIndex = StringToInt(jsonTextureIndex->values[0].value, jsonTextureIndex->values[0].size); 

    int type = StringToInt(jsonType->values[0].value, jsonType->values[0].size);

    float r = StringToFloat(jsonColor->childs[0].values[0].value, jsonColor->childs[0].values[0].size);
    float g = StringToFloat(jsonColor->childs[1].values[0].value, jsonColor->childs[1].values[0].size);
    float b = StringToFloat(jsonColor->childs[2].values[0].value, jsonColor->childs[2].values[0].size);
    float a = StringToFloat(jsonColor->childs[3].values[0].value, jsonColor->childs[3].values[0].size);
    TMVec4 color = {r, g, b, a};

    float absX = StringToFloat(jsonAbsUVs->values[0].value, jsonAbsUVs->values[0].size);
    float absY = StringToFloat(jsonAbsUVs->values[1].value, jsonAbsUVs->values[1].size);
    float absZ = StringToFloat(jsonAbsUVs->values[2].value, jsonAbsUVs->values[2].size);
    float absW = StringToFloat(jsonAbsUVs->values[3].value, jsonAbsUVs->values[3].size);
    TMVec4 absUvs = {absX, absY, absZ, absW};

    float relX = StringToFloat(jsonRelUVs->values[0].value, jsonRelUVs->values[0].size);
    float relY = StringToFloat(jsonRelUVs->values[1].value, jsonRelUVs->values[1].size);
    float relZ = StringToFloat(jsonRelUVs->values[2].value, jsonRelUVs->values[2].size);
    float relW = StringToFloat(jsonRelUVs->values[3].value, jsonRelUVs->values[3].size);
    TMVec4 relUvs = {relX, relY, relZ, relW};

    TMShader *shader = type ? state->spriteShader : state->colorShader;

    TMTexture *texture = NULL;
    if(entity->prefabType == PREFAB_TYPE_NONE) {
        texture = state->textures[textureIndex];
    }else if(entity->prefabType == PREFAB_TYPE_PLAYER) {
        texture = gPlayerTexture;
    }

    entity->color = color;
    entity->absUVs = absUvs;
    entity->relUVs = relUvs;
    entity->position = position;
    entity->size = size;
    entity->texture = texture;
    entity->zIndex = zIndex;
    entity->textureIndex = textureIndex;
    entity->shader = shader;
}

static void EntityAddCollision(Entity *entity, TMJsonObject *jsonObject) {

    entity->collision = (Collision *)malloc(sizeof(Collision));

    TMJsonObject *jsonSolid   = TMJsonFindChildByName(jsonObject, "Solid"); 
    TMJsonObject *jsonCapsule = TMJsonFindChildByName(jsonObject, "Capsule");
    TMJsonObject *jsonAABB    = TMJsonFindChildByName(jsonObject, "AABB");
    TMJsonObject *jsonCircle  = TMJsonFindChildByName(jsonObject, "Circle");
    TMJsonObject *jsonOBB     = TMJsonFindChildByName(jsonObject, "OBB");

    bool solid = (bool)StringToInt(jsonSolid->values[0].value, jsonSolid->values[0].size);
    
    if(jsonCapsule) {
        TMJsonObject *jsonA = TMJsonFindChildByName(jsonCapsule, "A");
        TMJsonObject *jsonB = TMJsonFindChildByName(jsonCapsule, "B");
        TMJsonObject *jsonR = TMJsonFindChildByName(jsonCapsule, "Radio");
    

        float x = StringToFloat(jsonA->values[0].value, jsonA->values[0].size);
        float y = StringToFloat(jsonA->values[1].value, jsonA->values[1].size);
        TMVec2 a  = {x, y};

        x = StringToFloat(jsonB->values[0].value, jsonB->values[0].size);
        y = StringToFloat(jsonB->values[1].value, jsonB->values[1].size);
        TMVec2 b  = {x, y};
            
        float radio = StringToFloat(jsonR->values[0].value, jsonR->values[0].size);

        Capsule capsule;
        capsule.r = radio;
        capsule.a = a;
        capsule.b = b;

        entity->collision->type = COLLISION_TYPE_CAPSULE;
        entity->collision->capsule = capsule;
        entity->collision->solid = solid;

    }
    else if(jsonAABB) {
        TMJsonObject *jsonMin = TMJsonFindChildByName(jsonAABB, "Min");
        TMJsonObject *jsonMax = TMJsonFindChildByName(jsonAABB, "Max");

        float x = StringToFloat(jsonMin->values[0].value, jsonMin->values[0].size);
        float y = StringToFloat(jsonMin->values[1].value, jsonMin->values[1].size);
        TMVec2 min  = {x, y};

        x = StringToFloat(jsonMax->values[0].value, jsonMax->values[0].size);
        y = StringToFloat(jsonMax->values[1].value, jsonMax->values[1].size);
        TMVec2 max  = {x, y};

        AABB aabb;
        aabb.min = min;
        aabb.max = max;

        entity->collision->type = COLLISION_TYPE_AABB;
        entity->collision->aabb = aabb;
        entity->collision->solid = solid;

    }
}

static void EntityAddAnimation(Entity *entity, TMJsonObject *jsonObject) {

    entity->animation = (Animation *)malloc(sizeof(Animation));
    
    TMJsonObject *jsonCount = TMJsonFindChildByName(jsonObject, "AnimationStatesCount");
    int count = StringToInt(jsonCount->values[0].value, jsonCount->values[0].size);

    TMJsonObject *jsonAnimIndex = TMJsonFindChildByName(jsonObject, "AnimationIndex");
    int index = StringToInt(jsonAnimIndex->values[0].value, jsonAnimIndex->values[0].size);
    
    entity->animation->index = index;
    entity->animation->statesCount = count;

    int stateIndex = 0;
    for(int i = 1; i <= count; ++i) {

        TMJsonObject *jsonAnimationState = jsonObject->childs + (i+1);

        TMJsonObject *jsonFrameCount = TMJsonFindChildByName(jsonAnimationState, "FrameCount");
        TMJsonObject *jsonFrames     = TMJsonFindChildByName(jsonAnimationState, "Frames");
        TMJsonObject *jsonSpeed      = TMJsonFindChildByName(jsonAnimationState, "Speed");

        int frameCount = StringToInt(jsonFrameCount->values[0].value, jsonFrameCount->values[0].size);
        float speed = StringToFloat(jsonSpeed->values[0].value, jsonSpeed->values[0].size);

        AnimationState animationState;
        animationState.frameCount = frameCount;
        animationState.speed = speed;

        for(int j = 0; j < frameCount; ++j) {
            int frame = StringToInt(jsonFrames->values[j].value, jsonFrames->values[j].size);
            animationState.frames[j] = frame;
        }

        entity->animation->states[stateIndex++] = animationState;

    }

}

void LoadSceneFromFile(EditorState *state, char *filepath) {

    TMJson *jsonFile = TMJsonOpen(filepath);

    TMJsonObject *jsonRoot = &jsonFile->root.childs[0];
    
    TMJsonObject *jsonLevelTextures = TMJsonFindChildByName(jsonRoot, "LevelTextures");
    TMJsonObject *jsonLevelAmbient = TMJsonFindChildByName(jsonRoot, "LevelAmbient");
    TMJsonObject *jsonLevelLights = TMJsonFindChildByName(jsonRoot, "LevelLights");
    TMJsonObject *jsonScene = TMJsonFindChildByName(jsonRoot, "Scene");

    ClearTextures(state);
    LoadTextures(state, jsonLevelTextures);

    ClearLights(state);
    LoadLights(state, jsonLevelAmbient, jsonLevelLights);


    state->selectedEntity = NULL;
    if(state->entities) {
        for(int i = 0; i < TMDarraySize(state->entities); ++i) {
            Entity *entity = state->entities + i;
            if(entity->collision) free(entity->collision);
            if(entity->animation) free(entity->animation);
            entity->collision = NULL;
            entity->animation = NULL;
        }
        TMDarrayDestroy(state->entities);
        state->entities = NULL;
    }


    for(int i = 0; i < jsonScene->childsCount; ++i) {

        TMJsonObject *jsonEntity = jsonScene->childs + i;

        TMJsonObject *jsonPrefabType = TMJsonFindChildByName(jsonEntity, "PrefabType");
        TMJsonObject *jsonGraphic    = TMJsonFindChildByName(jsonEntity, "Graphics");
        TMJsonObject *jsonCollision  = TMJsonFindChildByName(jsonEntity, "Collision");
        TMJsonObject *jsonAnimation  = TMJsonFindChildByName(jsonEntity, "Animation");

        Entity entity = {};
        entity.id = i;
        entity.prefabType = (PrefabsType)StringToInt(jsonPrefabType->values[0].value, jsonPrefabType->values[0].size); 

        if(jsonGraphic) EntityAddGraphic(&entity, jsonGraphic, state);
        if(jsonCollision) EntityAddCollision(&entity, jsonCollision);
        if(jsonAnimation) EntityAddAnimation(&entity, jsonAnimation);

        TMDarrayPush(state->entities, entity, Entity);

    }

    TMJsonClose(jsonFile);


}

void SaveScene(TMUIElement *element) {
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
