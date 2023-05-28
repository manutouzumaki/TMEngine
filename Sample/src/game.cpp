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

#include <math.h>
#include <stdio.h>
#include <float.h>
#include <assert.h>
#include <memory.h>

static float MetersToPixel = 100;


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


    LoadSceneFromFile(state, "../../assets/json/level1.json");

    // create the player
    Entity *player = EntityCreate();
    state->enemy = player;

    EntityAddGraphicsComponent(player, {10, 3.5}, {1, 1}, {1, 0, 0, 1},
                               {}, {}, 3, state->colorShader, NULL);


    EntityAddEnemyShotComponent(&state->entities, player, player->graphics, state->colorShader);


    TMDarrayPush(state->entities, player, Entity *);

}

void GameUpdate(GameState *state, float dt) {

    MessageFireFirstHit(MESSAGE_TYPE_PHYSICS_CLEAR_FORCES, (void *)state->entities, {});
    InputSystemUpdate(state->entities, dt); 
    AnimationSystemUpdate(state->entities, dt);
    EnemySystemUpdate(state->entities, dt);
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

    TMRendererClear(state->renderer, 0.1, 0.1, 0.1, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);


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
