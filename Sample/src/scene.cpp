#include "scene.h"

#include "game.h"
#include "entity.h"
#include "systems/animation_sys.h"
#include "systems/graphics_sys.h"

#include <utils/tm_json.h>
#include <utils/tm_darray.h>

#include <assert.h>
#include <memory.h>
#include <stdio.h>

extern TMTexture *gPlayerTexture;
extern TMTexture *gShotEnemyTexture;
extern TMTexture *gMoveEnemyTexture;

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

void EntityAddGraphicCmpFromJson(Entity *entity, TMJsonObject *jsonObject, GameState *state, int prefabType) {
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

    int type = StringToInt(jsonType->values[0].value, jsonType->values[0].size);
    TMShader *shader = (type == 0) ? state->colorShader : state->spriteShader;

    TMTexture *texture = NULL;
    if(prefabType == 0) {
        int textureIndex = StringToInt(jsonTextureIndex->values[0].value, jsonTextureIndex->values[0].size); 
        if(textureIndex >= 0) {
            texture = state->levelTextures[textureIndex];
        }
    }
    else if(prefabType == 1) {
        texture = gPlayerTexture;
    }
    else if(prefabType == 2) {
        texture = gShotEnemyTexture;
    }
    else if(prefabType == 3) {
        texture = gMoveEnemyTexture;
    }

    EntityAddGraphicsComponent(entity, position, size, color,
                               absUvs, relUvs, zIndex, shader, texture);
}

void EntityAddPhysicsCmpFromJson(Entity *entity, TMJsonObject *jsonObject) {

    TMJsonObject *jsonPosition     = TMJsonFindChildByName(jsonObject, "Position"); 
    TMJsonObject *jsonVelocity     = TMJsonFindChildByName(jsonObject, "Velocity");
    TMJsonObject *jsonAcceleration = TMJsonFindChildByName(jsonObject, "Acceleration");
    TMJsonObject *jsonDamping      = TMJsonFindChildByName(jsonObject, "Damping");

    TMJsonValue jsonValue = jsonPosition->childs[0].values[0];
    float xPos = StringToFloat(jsonValue.value, jsonValue.size);
    jsonValue = jsonPosition->childs[1].values[0];
    float yPos = StringToFloat(jsonValue.value, jsonValue.size);
    TMVec2 position = {xPos, yPos};

    jsonValue = jsonVelocity->childs[0].values[0];
    float xVel = StringToFloat(jsonValue.value, jsonValue.size);
    jsonValue = jsonVelocity->childs[1].values[0];
    float yVel = StringToFloat(jsonValue.value, jsonValue.size);
    TMVec2 velocity = {xVel, yVel};

    jsonValue = jsonAcceleration->childs[0].values[0];
    float xAcc = StringToFloat(jsonValue.value, jsonValue.size);
    jsonValue = jsonAcceleration->childs[1].values[0];
    float yAcc = StringToFloat(jsonValue.value, jsonValue.size);
    TMVec2 acceleration = {xAcc, yAcc};

    float damping = StringToFloat(jsonDamping->values[0].value, jsonDamping->values[0].size);

    EntityAddPhysicsComponent(entity, position, velocity, acceleration, damping);
}

void EntityAddCollisionCmpFromJson(Entity *entity, TMJsonObject *jsonObject) {
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

        EntityAddCollisionComponent(entity, COLLISION_TYPE_CAPSULE, capsule);
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

        EntityAddCollisionComponent(entity, COLLISION_TYPE_AABB, aabb);
    }
    else if(jsonCircle) {
        TMJsonObject *jsonC = TMJsonFindChildByName(jsonCircle, "C");
        TMJsonObject *jsonR = TMJsonFindChildByName(jsonCircle, "Radio");

        float x = StringToFloat(jsonC->values[0].value, jsonC->values[0].size);
        float y = StringToFloat(jsonC->values[1].value, jsonC->values[1].size);
        TMVec2 c  = {x, y};

        float radio = StringToFloat(jsonR->values[0].value, jsonR->values[0].size);

        Circle circle;
        circle.r = radio;
        circle.c = c;

        EntityAddCollisionComponent(entity, COLLISION_TYPE_CIRCLE, circle);
        
    }
}

void EntityAddAnimationCmpFromJson(Entity *entity, TMJsonObject *jsonObject) {

    EntityAddAnimationComponet(entity);
    
    TMJsonObject *jsonCount = TMJsonFindChildByName(jsonObject, "AnimationStatesCount");
    int count = StringToInt(jsonCount->values[0].value, jsonCount->values[0].size);

    TMJsonObject *jsonAnimIndex = TMJsonFindChildByName(jsonObject, "AnimationIndex");
    int index = StringToInt(jsonAnimIndex->values[0].value, jsonAnimIndex->values[0].size);
    
    entity->animation->index = index;

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

        AnimationSystemAddState(entity, animationState);

    }

}

void EntityAddInputCmpFromJson(Entity *entity, TMJsonObject *jsonObject, GameState *state) {

    state->player = entity;
    EntityAddInputComponent(entity);

}

void EntityAddEnemyMovementCmpFromJson(Entity *entity, TMJsonObject *jsonObject) {

    EntityAddEnemyMovementComponent(entity, entity->collision, entity->physics);

}

void EntityAddEnemyShotCmpFromJson(Entity *entity, TMJsonObject *jsonObject, GameState *state) {

    EntityAddEnemyShotComponent(&state->entities, entity, entity->graphics, state->colorShader);

}


void LoadSceneFromFile(GameState *state, const char *filepath) {

    TMJson *jsonFile = TMJsonOpen(filepath);

    TMJsonObject *jsonRoot = &jsonFile->root.childs[0];
    
    TMJsonObject *jsonLevelTextures = TMJsonFindChildByName(jsonRoot, "LevelTextures");
    TMJsonObject *jsonLevelAmbient = TMJsonFindChildByName(jsonRoot, "LevelAmbient");
    TMJsonObject *jsonLevelLights = TMJsonFindChildByName(jsonRoot, "LevelLights");
    TMJsonObject *jsonScene = TMJsonFindChildByName(jsonRoot, "Scene");

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
        
        TMDarrayPush(state->levelTextures, texture, TMTexture *);
    }

    TMVec3 ambient  = {};
    ambient.x = StringToFloat(jsonLevelAmbient->values[0].value, jsonLevelAmbient->values[0].size);
    ambient.y = StringToFloat(jsonLevelAmbient->values[1].value, jsonLevelAmbient->values[1].size);
    ambient.z = StringToFloat(jsonLevelAmbient->values[2].value, jsonLevelAmbient->values[2].size);

    GraphicsSystemSetAmbientLight(state->renderer, ambient);

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

        GraphicsSystemAddLight(state->renderer, position, attributes, color, range);
    }
    
    for(int i = 0; i < jsonScene->childsCount; ++i) {

        TMJsonObject *jsonEntity = jsonScene->childs + i;

        TMJsonObject *jsonPrefabType    = TMJsonFindChildByName(jsonEntity, "PrefabType");
        TMJsonObject *jsonGraphic       = TMJsonFindChildByName(jsonEntity, "Graphics");
        TMJsonObject *jsonPhysics       = TMJsonFindChildByName(jsonEntity, "Physics");
        TMJsonObject *jsonCollision     = TMJsonFindChildByName(jsonEntity, "Collision");
        TMJsonObject *jsonAnimation     = TMJsonFindChildByName(jsonEntity, "Animation");
        TMJsonObject *jsonInput         = TMJsonFindChildByName(jsonEntity, "Input");
        TMJsonObject *jsonEnemyMovement = TMJsonFindChildByName(jsonEntity, "EnemyMovement");
        TMJsonObject *jsonEnemyShot     = TMJsonFindChildByName(jsonEntity, "EnemyShot");
 
        int prefabType = StringToInt(jsonPrefabType->values[0].value, jsonPrefabType->values[0].size);

        Entity *entity = EntityCreate();

        if(jsonGraphic) EntityAddGraphicCmpFromJson(entity, jsonGraphic, state, prefabType);
        if(jsonPhysics) EntityAddPhysicsCmpFromJson(entity, jsonPhysics);
        if(jsonCollision) EntityAddCollisionCmpFromJson(entity, jsonCollision);
        if(jsonAnimation) EntityAddAnimationCmpFromJson(entity, jsonAnimation);
        if(jsonInput) EntityAddInputCmpFromJson(entity, jsonInput, state);
        if(jsonEnemyMovement) EntityAddEnemyMovementCmpFromJson(entity, jsonEnemyMovement);
        if(jsonEnemyShot) EntityAddEnemyShotCmpFromJson(entity, jsonEnemyMovement, state);

        TMDarrayPush(state->entities, entity, Entity *);

    }

    TMJsonClose(jsonFile);

}
