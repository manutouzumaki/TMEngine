#include "game.h"
#include "scene.h"

#include <utils/tm_darray.h>
#include <tm_debug_renderer.h>
#include "message.h"
#include "systems/physics_sys.h"
#include "systems/collision_sys.h"
#include "systems/graphics_sys.h"
#include "systems/animation_sys.h"
#include "systems/enemy_sys.h"
#include "systems/aabb_sys.h"
#include "systems/player_sys.h"

#include <math.h>
#include <stdio.h>
#include <float.h>
#include <assert.h>
#include <memory.h>

static float MetersToPixel = 100;

static float MinF32(float a, float b) {
    if(a < b) return a;
    return b;
}

static float MaxF32(float a, float b) {
    if(a > b) return a;
    return b;
}

void UpdateCameraToFollowTarget(GameState *state, Entity *target) {

    if(target->graphics) {
        int width = TMRendererGetWidth(state->renderer);
        int height = TMRendererGetHeight(state->renderer);
        GraphicsComponent *graphics = target->graphics;

        float cameraUnitsX = (float)width/MetersToPixel;
        float cameraUnitsY = (float)height/MetersToPixel;

        // TODO: save this in the level file, this should be set per level        
        float levelStartX = state->cameraMin.x;
        float levelStartY = state->cameraMin.y;
        float levelEndX   = state->cameraMax.x;
        float levelEndY   = state->cameraMax.y;

        float x = MinF32(MaxF32(graphics->position.x - (width*0.5f)/MetersToPixel, levelStartX), levelEndX - cameraUnitsX);
        float y = MinF32(MaxF32(graphics->position.y - (height*0.5f)/MetersToPixel, levelStartY), levelEndY - cameraUnitsY);

        TMVec3 pos = {x, y, 0};
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

    state->particleSystem = ParticleSystemCreate(state->renderer);

}

static void RestartLevel(GameState *state) {
    // TODO: check if we are freeing all the memory ...

    AnimationSystemShutdown(state->entities);
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        EntityDestroy(entity);
    }
    EntitySystemShutdown();
    TMDarrayDestroy(state->entities);
    
    state->entities = NULL;
    GraphicsSystemRemoveLights(state->renderer);

    if(state->levelTextures) {
        for(int i = 0; i < TMDarraySize(state->levelTextures); ++i) {
            TMTexture *texture = state->levelTextures[i];
            TMRendererTextureDestroy(state->renderer, texture);
        }
        TMDarrayDestroy(state->levelTextures);
        state->levelTextures = NULL;
    }

    EntitySystemInitialize(100);
    LoadSceneFromFile(state, "../../assets/json/testScene.json");

}

void GameUpdate(GameState *state, float dt) {

    if(!PlayerAlive(state->player)) {
        RestartLevel(state);
    }

    ParticleSystemUpdate(&state->particleSystem, dt);

    MessageFireFirstHit(MESSAGE_TYPE_PHYSICS_CLEAR_FORCES, (void *)state->entities, {});
    
    InputSystemUpdate(state->entities, dt); 
    AnimationSystemUpdate(state->entities, dt);
    EnemySystemUpdate(state->entities, dt);
    PhysicSystemUpdate(state->entities, dt);
    AABBSystemUpdate(state->player, state->entities);
    PlayerComponentUpdate(state->player, dt);
}

void GameFixUpdate(GameState *state, float dt) {
    PhysicSystemFixUpdate(state->entities, dt);
    if((TMVec2LenSq(state->player->physics->velocity) > 0.1f) && state->player->physics->grounded) {
        TMVec2 position = state->player->physics->position;
        position.y -= state->player->graphics->size.y*0.44f;
        ParticleSystemAddParticles(&state->particleSystem, position);
    }
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

    TMRendererClear(state->renderer, 0.1, 0.1, 0.1, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);


    GraphicsSystemDraw(state->renderer, state->entities);


    ParticleSystemDraw(&state->particleSystem);
    
    TMDebugRenderDraw();

    TMRendererPresent(state->renderer, 1);
}

void GameShutdown(GameState *state) {
    // TODO: this should be handle by the entity system not the game directly
    
    ParticleSystemDestroy(state->renderer, &state->particleSystem);

    GraphicsSystemShutdown(state->renderer);
    CollisionSystemShutdown();
    PhysicSystemShutdown();
    AnimationSystemShutdown(state->entities);
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        EntityDestroy(entity);
    }
    EntitySystemShutdown();
    TMDarrayDestroy(state->entities);
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
