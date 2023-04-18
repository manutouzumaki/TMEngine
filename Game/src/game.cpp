#include "game.h"

#include <utils/tm_darray.h>
#include <tm_debug_renderer.h>
#include "message.h"
#include "systems/physics_sys.h"
#include "systems/collision_sys.h"
#include "systems/graphics_sys.h"

#include <math.h>
#include <stdio.h>
#include <float.h>

struct ShaderMatrix {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
};

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
        ShaderMatrix mats{};
        mats.proj = state->proj;
        mats.view = state->view;
        mats.world = TMMat4Identity();
        TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);
    }

}

void GameInitialize(GameState *state, TMWindow *window) {
    state->renderer = TMRendererCreate(window);

    state->shader = TMRendererShaderCreate(state->renderer,
                                           "../../assets/shaders/batchVert.hlsl",
                                           "../../assets/shaders/batchFrag.hlsl");

    state->texture = TMRendererTextureCreate(state->renderer,
                                             "../../assets/images/characters_packed.png");


    state->batchRenderer = TMRendererRenderBatchCreate(state->renderer, state->shader, state->texture, 100);
    TMDebugRendererInitialize(state->renderer, 100);

    int width = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);
    TMVec3 pos = {(-width*0.5f)/MetersToPixel, (-height*0.5f)/MetersToPixel, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    state->view = TMMat4LookAt(pos, pos + tar, up);
    state->proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.0f, 100.0f);

    ShaderMatrix mats{};
    mats.proj = state->proj;
    mats.view = state->view;
    mats.world = TMMat4Identity();
    state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &mats, sizeof(ShaderMatrix), 0);

    TMRendererDepthTestDisable(state->renderer);
    TMRendererFaceCulling(state->renderer, false, 0);


    MessageSystemInitialize();

    PhysicSystemInitialize();
    CollisionSystemInitialize();
    GraphicsSystemInitialize();


    EntitySystemInitialize(100);
    
    AABB aabb{};




    // create the cealing
    Entity *ceal = EntityCreate();
    EntityAddGraphicsComponent(ceal, {0, 1.9}, {4, 1}, {0, 0.2, 0.4, 1});
    aabb.min = {0 - 2, 1.9 - 0.5};
    aabb.max = {0 + 2, 1.9 + 0.5};
    EntityAddCollisionComponent(ceal, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, ceal, Entity *);
    
    // create the cealing
    Entity *ceal1 = EntityCreate();
    EntityAddGraphicsComponent(ceal1, {-4, 1.9}, {4, 1}, {0.5, 0.2, 0.4, 1});
    aabb.min = {-4 - 2, 1.9 - 0.5};
    aabb.max = {-4 + 2, 1.9 + 0.5};
    EntityAddCollisionComponent(ceal1, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, ceal1, Entity *);

    Entity *player2 = EntityCreate();
    EntityAddGraphicsComponent(player2, {1.3, -0.2}, {0.8, 1}, {1, 0.8, 0.2, 1});
    //EntityAddPhysicsComponent(player2, {0.9, 1.0}, {0, 0}, {0, 0}, 0.0001f);
    aabb.min = {1.3 - 0.4, -0.2 - 0.5};
    aabb.max = {1.3 + 0.4, -0.2 + 0.5};
    EntityAddCollisionComponent(player2, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player2, Entity *);

    Entity *player3 = EntityCreate();
    EntityAddGraphicsComponent(player3, {-2.3, 0.6}, {0.8, 1}, {1, 0.2, 0.5, 1});
    aabb.min = {-2.3 - 0.4, 0.6 - 0.5};
    aabb.max = {-2.3 + 0.4, 0.6 + 0.5};
    EntityAddCollisionComponent(player3, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player3, Entity *);

    Entity *player4 = EntityCreate();
    EntityAddGraphicsComponent(player4, {5.0, 0.0}, {0.8, 1}, {1, 0.2, 0.5, 1});
    aabb.min = {5.0 - 0.4, 0.0 - 0.5};
    aabb.max = {5.0 + 0.4, 0.0 + 0.5};
    EntityAddCollisionComponent(player4, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player4, Entity *);

    Entity *player5 = EntityCreate();
    EntityAddGraphicsComponent(player5, {5.0 + 0.8*1.8, 0.0}, {0.8, 1}, {1, 0.2, 0.5, 1});
    aabb.min = {5.0 + 0.8*1.8 - 0.4, 0.0 - 0.5};
    aabb.max = {5.0 + 0.8*1.8 + 0.4, 0.0 + 0.5};
    EntityAddCollisionComponent(player5, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player5, Entity *);

    // create the floor
    Entity *floor = EntityCreate();
    EntityAddGraphicsComponent(floor, {0, -1.9}, {8, 1}, {0, 0.2, 0.4, 1});
    aabb.min = {0 - 4, -1.9 - 0.5};
    aabb.max = {0 + 4, -1.9 + 0.5};
    EntityAddCollisionComponent(floor, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, floor, Entity *);


    // create the player
    Entity *player = EntityCreate();
    state->player = player;
    EntityAddInputComponent(player);
    EntityAddGraphicsComponent(player, {-5, 0}, {0.8, 0.8}, {1, 0, 0, 1});
    EntityAddPhysicsComponent(player, {-5, 0}, {0, 0}, {0, 0}, 0.01f);

    //Circle circle;
    //circle.c = {-5, 0};
    //circle.r = 0.4;
    //EntityAddCollisionComponent(player, COLLISION_TYPE_CIRCLE, circle);
    //aabb.min = {-5 - 0.4, 0 - 0.4};
    //aabb.max = {-5 + 0.4, 0 + 0.4};
    //EntityAddCollisionComponent(player, COLLISION_TYPE_AABB, aabb);
    Capsule capsule;
    capsule.r = 0.4;
    capsule.a = {-5.0, 0.4};
    capsule.b = {-5.0, -0.4};
    EntityAddCollisionComponent(player, COLLISION_TYPE_CAPSULE, capsule);


    TMDarrayPush(state->entities, player, Entity *);

}

void GameUpdate(GameState *state, float dt) {
    InputSystemUpdate(state->entities); 
}

void GameFixUpdate(GameState *state, float dt) {
    PhysicSystemUpdate(state->entities, dt);
}

void GamePostUpdate(GameState *state, float t) {
    PhysicSystemPostUpdate(state->entities, t);
    CollisionSystemUpdate(state->entities);
    UpdateCameraToFollowTarget(state, state->player);
}

void GameRender(GameState *state) {
    TMRendererClear(state->renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);
    TMRendererBindShader(state->renderer, state->shader);
    GraphicsSystemDraw(state->batchRenderer, state->entities);


    // TODO: closest point in circle test ...
    TMDebugRendererDrawCircle(5, 0, 1, 0xFFFF33FF, 20);
    Circle circle;
    circle.c = {5, 0};
    circle.r = 1;
    TMVec2 closest;
    ClosestPtPointCircle(state->player->graphics->position, 
                         circle, closest);
    TMDebugRendererDrawCircle(closest.x, closest.y, 0.05f, 0xFF0aa234, 10);

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
    GraphicsSystemShutdown();
    CollisionSystemShutdown();
    PhysicSystemShutdown();
    MessageSystemShoutdown();
    TMDebugRendererShutdown();
    TMRendererRenderBatchDestroy(state->renderer, state->batchRenderer);
    TMRendererTextureDestroy(state->renderer, state->texture);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);

}

