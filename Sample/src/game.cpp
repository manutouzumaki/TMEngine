#include "game.h"

#include <utils/tm_darray.h>
#include <tm_debug_renderer.h>
#include <utils/tm_json.h>
#include "message.h"
#include "systems/physics_sys.h"
#include "systems/collision_sys.h"
#include "systems/graphics_sys.h"
#include "systems/animation_sys.h"

#include <math.h>
#include <stdio.h>
#include <float.h>
#include <assert.h>
#include <memory.h>

struct ShaderMatrix {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
};

static float MetersToPixel = 100;


bool EntitiesSerialize(Entity **entities, float *uvs, int uvsCount) {

    TMJsonObject jsonRoot = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonRoot, "Root");

    TMJsonObject jsonPlayerUvs = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonPlayerUvs, "PlayerUvs");
    for(int i = 0; i < uvsCount*4; i++) {
        TMJsonObjectSetValue(&jsonPlayerUvs, uvs[i]);
    }

    TMJsonObject jsonScene = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonScene, "Scene");


    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        TMJsonObject jsonEntity = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonEntity, "Entity");

        if(entity->graphics) {
            TMJsonObject jsonGraphic = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonGraphic, "Graphics");

            // graphics type
            TMJsonObject jsonType = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonType, "Type");
            TMJsonObjectSetValue(&jsonType, (float)entity->graphics->type); 

            // position
            TMJsonObject jsonPosition = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonPosition, "Position");
            TMJsonObject xPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&xPos, "X");
            TMJsonObjectSetValue(&xPos, entity->graphics->position.x);
            TMJsonObject yPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&yPos, "Y");
            TMJsonObjectSetValue(&yPos, entity->graphics->position.y);
            TMJsonObjectAddChild(&jsonPosition, &xPos);
            TMJsonObjectAddChild(&jsonPosition, &yPos);

            // size
            TMJsonObject jsonSize = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonSize, "Size");
            TMJsonObject xSiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&xSiz, "X");
            TMJsonObjectSetValue(&xSiz, entity->graphics->size.x);
            TMJsonObject ySiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&ySiz, "Y");
            TMJsonObjectSetValue(&ySiz, entity->graphics->size.y);
            TMJsonObjectAddChild(&jsonSize, &xSiz);
            TMJsonObjectAddChild(&jsonSize, &ySiz);

            TMJsonObjectAddChild(&jsonGraphic, &jsonType);
            TMJsonObjectAddChild(&jsonGraphic, &jsonPosition);
            TMJsonObjectAddChild(&jsonGraphic, &jsonSize);

            switch(entity->graphics->type) {
            
                case GRAPHICS_TYPE_SOLID_COLOR: {
                    // color
                    TMJsonObject jsonColor = TMJsonObjectCreate();
                    TMJsonObjectSetName(&jsonColor, "Color");
                    TMJsonObject r = TMJsonObjectCreate();
                    TMJsonObjectSetName(&r, "R");
                    TMJsonObjectSetValue(&r, entity->graphics->color.x);
                    TMJsonObject g = TMJsonObjectCreate();
                    TMJsonObjectSetName(&g, "G");
                    TMJsonObjectSetValue(&g, entity->graphics->color.y);
                    TMJsonObject b = TMJsonObjectCreate();
                    TMJsonObjectSetName(&b, "B");
                    TMJsonObjectSetValue(&b, entity->graphics->color.z);
                    TMJsonObject a = TMJsonObjectCreate();
                    TMJsonObjectSetName(&a, "A");
                    TMJsonObjectSetValue(&a, entity->graphics->color.w);
                    TMJsonObjectAddChild(&jsonColor, &r);
                    TMJsonObjectAddChild(&jsonColor, &g);
                    TMJsonObjectAddChild(&jsonColor, &b);
                    TMJsonObjectAddChild(&jsonColor, &a);
                    
                    TMJsonObjectAddChild(&jsonGraphic, &jsonColor);


                } break;
                case GRAPHICS_TYPE_SPRITE: {
                    // absUVs
                    TMJsonObject jsonUVs = TMJsonObjectCreate();
                    TMJsonObjectSetName(&jsonUVs, "Uvs");
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->relUVs[0]);
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->relUVs[1]);
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->relUVs[2]);
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->relUVs[3]);

                    // Index
                    TMJsonObject jsonIndex = TMJsonObjectCreate();
                    TMJsonObjectSetName(&jsonIndex, "Index");
                    TMJsonObjectSetValue(&jsonIndex, (float)entity->graphics->index);
                    
                    TMJsonObjectAddChild(&jsonGraphic, &jsonUVs);
                    TMJsonObjectAddChild(&jsonGraphic, &jsonIndex);

                } break;
                case GRAPHICS_TYPE_SUBSPRITE: {
                    TMJsonObject jsonUVs = TMJsonObjectCreate();
                    TMJsonObjectSetName(&jsonUVs, "Uvs");
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->absUVs.v[0]);
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->absUVs.v[1]);
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->absUVs.v[2]);
                    TMJsonObjectSetValue(&jsonUVs, entity->graphics->absUVs.v[3]);

                    // Index
                    TMJsonObject jsonIndex = TMJsonObjectCreate();
                    TMJsonObjectSetName(&jsonIndex, "Index");
                    TMJsonObjectSetValue(&jsonIndex, (float)entity->graphics->index);
                    
                    TMJsonObjectAddChild(&jsonGraphic, &jsonUVs);
                    TMJsonObjectAddChild(&jsonGraphic, &jsonIndex);

                } break;

            }

            TMJsonObjectAddChild(&jsonEntity, &jsonGraphic);

        }

        if(entity->physics) {
            TMJsonObject jsonPhysics = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonPhysics, "Physics");

            // position
            TMJsonObject jsonPosition = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonPosition, "Position");
            TMJsonObject xPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&xPos, "X");
            TMJsonObjectSetValue(&xPos, entity->physics->position.x);
            TMJsonObject yPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&yPos, "Y");
            TMJsonObjectSetValue(&yPos, entity->physics->position.y);
            TMJsonObjectAddChild(&jsonPosition, &xPos);
            TMJsonObjectAddChild(&jsonPosition, &yPos);

            TMJsonObject jsonVelocity = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonVelocity, "Velocity");
            TMJsonObject xVel = TMJsonObjectCreate();
            TMJsonObjectSetName(&xVel, "X");
            TMJsonObjectSetValue(&xVel, entity->physics->velocity.x);
            TMJsonObject yVel = TMJsonObjectCreate();
            TMJsonObjectSetName(&yVel, "Y");
            TMJsonObjectSetValue(&yVel, entity->physics->position.y);
            TMJsonObjectAddChild(&jsonVelocity, &xVel);
            TMJsonObjectAddChild(&jsonVelocity, &yVel);

            TMJsonObject jsonAccel = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonAccel, "Acceleration");
            TMJsonObject xAcc = TMJsonObjectCreate();
            TMJsonObjectSetName(&xAcc, "X");
            TMJsonObjectSetValue(&xAcc, entity->physics->acceleration.x);
            TMJsonObject yAcc = TMJsonObjectCreate();
            TMJsonObjectSetName(&yAcc, "Y");
            TMJsonObjectSetValue(&yAcc, entity->physics->acceleration.y);
            TMJsonObjectAddChild(&jsonAccel, &xAcc);
            TMJsonObjectAddChild(&jsonAccel, &yAcc);

            TMJsonObject jsonDamping = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonDamping, "Damping");
            TMJsonObjectSetValue(&jsonDamping, entity->physics->damping);

            TMJsonObjectAddChild(&jsonPhysics, &jsonPosition);
            TMJsonObjectAddChild(&jsonPhysics, &jsonVelocity);
            TMJsonObjectAddChild(&jsonPhysics, &jsonAccel);
            TMJsonObjectAddChild(&jsonPhysics, &jsonDamping);

            TMJsonObjectAddChild(&jsonEntity, &jsonPhysics);

        }

        if(entity->input) {
            TMJsonObject jsonInput = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonInput, "Input");
            TMJsonObjectSetValue(&jsonInput, 1.0f);
            TMJsonObjectAddChild(&jsonEntity, &jsonInput);
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

        if(entity->animation) {

            TMJsonObject jsonAnimation = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonAnimation, "Animation");

            TMJsonObject statesCount = TMJsonObjectCreate();
            TMJsonObjectSetName(&statesCount, "AnimationStatesCount");
            TMJsonObjectSetValue(&statesCount, (float)entity->animation->statesCount);

            TMJsonObjectAddChild(&jsonAnimation, &statesCount);

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

        TMJsonObjectAddChild(&jsonScene, &jsonEntity);

    }
    
    TMJsonObjectAddChild(&jsonRoot, &jsonScene);
    TMJsonObjectAddChild(&jsonRoot, &jsonPlayerUvs);

    int bytesCount = 0;
    TMJsonObjectStringify(&jsonRoot, NULL, &bytesCount);
    
    char *buffer = (char *)malloc(bytesCount + 1);
    int bytesWriten = 0;
    TMJsonObjectStringify(&jsonRoot, buffer, &bytesWriten);
    printf("%s", buffer);
    
    TMFileWriteText("../../assets/json/scene.json", buffer, bytesWriten);
    free(buffer);
    
    TMJsonObjectFree(&jsonRoot);

    return false;
}

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

void EntityAddGraphicCmpFromJson(Entity *entity, TMJsonObject *jsonObject, GameState *state) {
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

    printf("textureIndex: %d\n", textureIndex);

    GraphicsComponentType type = (GraphicsComponentType)StringToInt(jsonType->values[0].value,
                                                                    jsonType->values[0].size);

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

    TMShader *shader = (type == GRAPHICS_TYPE_SOLID_COLOR) ? state->colorShader : state->spriteShader;

    TMTexture *texture = NULL;
    if(textureIndex >= 0) {
        texture = state->levelTextures[textureIndex];
    }

    EntityAddGraphicsComponent(entity, type, position, size, color,
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
        // TODO: ...
    }
    else if(jsonOBB) {
        // TODO: ...
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

void LoadSceneFromFile(GameState *state, const char *filepath) {

    TMJson *jsonFile = TMJsonOpen(filepath);

    TMJsonObject *jsonRoot = &jsonFile->root.childs[0];
    
    TMJsonObject *jsonLevelTextures = TMJsonFindChildByName(jsonRoot, "LevelTextures");
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
    
    int texturesCount = TMDarraySize(state->levelTextures);
    
    for(int i = 0; i < jsonScene->childsCount; ++i) {

        TMJsonObject *jsonEntity = jsonScene->childs + i;

        TMJsonObject *jsonGraphic   = TMJsonFindChildByName(jsonEntity, "Graphics");
        TMJsonObject *jsonPhysics   = TMJsonFindChildByName(jsonEntity, "Physics");
        TMJsonObject *jsonCollision = TMJsonFindChildByName(jsonEntity, "Collision");
        TMJsonObject *jsonAnimation = TMJsonFindChildByName(jsonEntity, "Animation");
        TMJsonObject *jsonInput     = TMJsonFindChildByName(jsonEntity, "Input");

        Entity *entity = EntityCreate();

        if(jsonGraphic) EntityAddGraphicCmpFromJson(entity, jsonGraphic, state);
        if(jsonPhysics) EntityAddPhysicsCmpFromJson(entity, jsonPhysics);
        if(jsonCollision) EntityAddCollisionCmpFromJson(entity, jsonCollision);
        if(jsonAnimation) EntityAddAnimationCmpFromJson(entity, jsonAnimation);
        if(jsonInput) EntityAddInputCmpFromJson(entity, jsonInput, state);

        TMDarrayPush(state->entities, entity, Entity *);

    }

    TMJsonClose(jsonFile);

}



void UpdateCameraToFollowTarget(GameState *state, Entity *target) {

    if(target->graphics) {
        int width = TMRendererGetWidth(state->renderer);
        int height = TMRendererGetHeight(state->renderer);
        GraphicsComponent *graphics = target->graphics;
        TMVec3 pos = {graphics->position.x - (width*0.5f)/MetersToPixel,
                      graphics->position.y - (height*0.5f)/MetersToPixel,
                      0};
        TMVec3 tar = {0, 0, 1};
        TMVec3 up  = {0, 1, 0};
        state->view = TMMat4LookAt(pos, pos + tar, up);
        GraphicsSystemSetViewMatrix(state->renderer, state->view);
    }

}

void GameInitialize(GameState *state, TMWindow *window) {

    state->renderer = TMRendererCreate(window);

    state->spriteShader = TMRendererShaderCreate(state->renderer,
                                  "../../assets/shaders/defaultVert.hlsl",
                                  "../../assets/shaders/spriteFrag.hlsl");
    state->colorShader  =  TMRendererShaderCreate(state->renderer,
                                  "../../assets/shaders/defaultVert.hlsl",
                                  "../../assets/shaders/colorFrag.hlsl");

    TMDebugRendererInitialize(state->renderer, 100);

    MessageSystemInitialize();

    PhysicSystemInitialize();
    CollisionSystemInitialize();
    GraphicsSystemInitialize(state->renderer, state->colorShader);
    AnimationSystemInitialize();

    int width = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);
    TMVec3 pos = {(-width*0.5f)/MetersToPixel, (-height*0.5f)/MetersToPixel, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    state->view = TMMat4LookAt(pos, pos + tar, up);
    state->proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.1f, 100.0f);

    GraphicsSystemSetViewMatrix(state->renderer, state->view);
    GraphicsSystemSetProjMatrix(state->renderer, state->proj);
    GraphicsSystemSetWorldMatrix(state->renderer, TMMat4Identity());

    EntitySystemInitialize(100);


    LoadSceneFromFile(state, "../../assets/json/testScene.json");

}

void GameUpdate(GameState *state, float dt) {

    MessageFireFirstHit(MESSAGE_TYPE_PHYSICS_CLEAR_FORCES, (void *)state->entities, {});
    InputSystemUpdate(state->entities, dt); 
    AnimationSystemUpdate(state->entities, dt);
    PhysicSystemUpdate(state->entities, dt);
}

void GameFixUpdate(GameState *state, float dt) {
    PhysicSystemFixUpdate(state->entities, dt);
}

void GamePostUpdate(GameState *state, float t) {
    PhysicSystemPostUpdate(state->entities, t);
    CollisionSystemUpdate(state->entities);
    UpdateCameraToFollowTarget(state, state->player);
}

void GameRender(GameState *state) {

    int clientWidth, clientHeight = 0;
    if(TMRendererUpdateRenderArea(state->renderer, &clientWidth, &clientHeight)) {
        state->proj = TMMat4Ortho(0, clientWidth/MetersToPixel, 0, clientHeight/MetersToPixel, 0.1f, 100.0f);
        GraphicsSystemSetProjMatrix(state->renderer, state->proj);
        printf("Update Render Area\n");
    }

    TMRendererClear(state->renderer, 0.6, 0.6, 0.9, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);


    GraphicsSystemDraw(state->renderer, state->entities);
    
    TMDebugRenderDraw();

    TMRendererPresent(state->renderer, 1);
}

void GameShutdown(GameState *state) {
    // TODO: this should be handle by the entity system not the game directly
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        EntityDestroy(entity);
    }
    EntitySystemShutdown();
    GraphicsSystemShutdown(state->renderer);
    CollisionSystemShutdown();
    PhysicSystemShutdown();
    AnimationSystemShutdown(state->entities);
    MessageSystemShoutdown();
    TMDebugRendererShutdown();
    if(state->levelTextures) {
        for(int i = 0; i < TMDarraySize(state->levelTextures); ++i) {
            TMTexture *texture = state->levelTextures[i];
            TMRendererTextureDestroy(state->renderer, texture);
        }
        TMDarrayDestroy(state->levelTextures);
    }
    TMRendererShaderDestroy(state->renderer, state->colorShader);
    TMRendererShaderDestroy(state->renderer, state->spriteShader);
    TMRendererDestroy(state->renderer);

}

