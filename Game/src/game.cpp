#include "game.h"

#include <utils/tm_darray.h>
#include <tm_input.h>
#include <tm_debug_renderer.h>

#include <math.h>
#include <stdio.h>
#include <float.h>

struct ShaderMatrix {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
};

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
    TMVec3 pos = {0, 0, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    state->view = TMMat4LookAt(pos, pos + tar, up);
    state->proj = TMMat4Ortho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, 0.0f, 100.0f);

    ShaderMatrix mats{};
    mats.proj = state->proj;
    mats.view = state->view;
    mats.world = TMMat4Identity();
    state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &mats, sizeof(ShaderMatrix), 0);

    TMRendererDepthTestDisable(state->renderer);


    EntitySystemInitialize(100);
    
    AABB aabb{};

    // create the floor
    Entity *floor = EntityCreate();
    EntityAddGraphicsComponent(floor, {0, -199}, {800, 100}, {0, 0.2, 0.4, 1});
    aabb.min = {0 - 400, -199 - 50};
    aabb.max = {0 + 400, -199 + 50};
    EntityAddCollisionComponent(floor, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, floor, Entity *);

    // create the cealing
    Entity *ceal = EntityCreate();
    EntityAddGraphicsComponent(ceal, {0, 199}, {400, 100}, {0, 0.2, 0.4, 1});
    aabb.min = {0 - 200, 199 - 50};
    aabb.max = {0 + 200, 199 + 50};
    EntityAddCollisionComponent(ceal, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, ceal, Entity *);

    // create the cealing
    Entity *ceal1 = EntityCreate();
    EntityAddGraphicsComponent(ceal1, {-400, 199}, {400, 100}, {0.5, 0.2, 0.4, 1});
    aabb.min = {-400 - 200, 199 - 50};
    aabb.max = {-400 + 200, 199 + 50};
    EntityAddCollisionComponent(ceal1, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, ceal1, Entity *);

    // create the player
    Entity *player = EntityCreate();
    state->player = player;
    EntityAddGraphicsComponent(player, {0, 0}, {80, 120}, {1, 0, 0, 1});
    EntityAddPhysicsComponent(player, {0, 0}, {0, 0}, {0, 0}, 0.01f);
    EntityAddInputComponent(player);
    aabb.min = {0 - 40, 0 - 60};
    aabb.max = {0 + 40, 0 + 60};
    EntityAddCollisionComponent(player, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player, Entity *);

    Entity *player2 = EntityCreate();
    EntityAddGraphicsComponent(player2, {90, 30}, {80, 100}, {1, 0.2, 0.5, 1});
    EntityAddPhysicsComponent(player2, {90, 30}, {0, 0}, {0, 0}, 0.0001f);
    aabb.min = {90 - 40, 30 - 50};
    aabb.max = {90 + 40, 30 + 50};
    EntityAddCollisionComponent(player2, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player2, Entity *);


}

void GameUpdate(GameState *state, float dt) {
    
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];

        if(entity->input && entity->physics) {
            PhysicsComponent *physics = entity->physics;
            TMVec2 acceleration = {};
            if(TMInputKeyboardKeyIsDown('D')) {
                acceleration.x = 1.0f;
            }
            if(TMInputKeyboardKeyIsDown('A')) {
                acceleration.x = -1.0f;
            }
            if(TMInputKeyboardKeyIsDown('W')) {
                acceleration.y = 1.0f;
            }
            if(TMInputKeyboardKeyIsDown('S')) {
                acceleration.y = -1.0f;
            }
            if(TMVec2LenSq(acceleration) > 0) {
              physics->acceleration = TMVec2Normalized(acceleration) * 2000.0f;
            }
            else {
                physics->acceleration = acceleration;
            }
        
        }
    }
}

void GameFixUpdate(GameState *state, float dt) {
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        PhysicSystemUpdate(state, entity, dt);
    }
}

void GamePostUpdate(GameState *state, float t) {
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        PhysicSystemPostUpdate(entity, t);

        if(entity->collision, entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            AABB *aabb = &entity->collision->aabb;
            float width = aabb->max.x - aabb->min.x;
            float height = aabb->max.y - aabb->min.y;
            float x = aabb->min.x + width*0.5f;
            float y = aabb->min.y + height*0.5f;
            aabb->min = {graphics->position.x - width*0.5f, graphics->position.y - height*0.5f};
            aabb->max = {graphics->position.x + width*0.5f, graphics->position.y + height*0.5f};
        }
    }

    GraphicsComponent *graphics = state->player->graphics;
    TMVec3 pos = {graphics->position.x, graphics->position.y, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    state->view = TMMat4LookAt(pos, pos + tar, up);
    ShaderMatrix mats{};
    mats.proj = state->proj;
    mats.view = state->view;
    mats.world = TMMat4Identity();
    TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);

}

void GameRender(GameState *state) {
    TMRendererClear(state->renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

    
    TMRendererBindShader(state->renderer, state->shader);

    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            TMRendererRenderBatchAdd(state->batchRenderer,
                                     graphics->position.x, graphics->position.y, 1,
                                     graphics->size.x, graphics->size.y, 0,
                                     graphics->color.x, graphics->color.y,
                                     graphics->color.z, graphics->color.w);
        }
    }
    TMRendererRenderBatchDraw(state->batchRenderer);

    // draw debug geometry
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        if(entity->collision) {
            CollisionComponent *collision = entity->collision;
            AABB aabb = collision->aabb;
            float width = aabb.max.x - aabb.min.x;
            float height = aabb.max.y - aabb.min.y;
            float x = aabb.min.x + width*0.5f;
            float y = aabb.min.y + height*0.5f;
            TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);

            aabb.min = {aabb.min.x - 40, aabb.min.y - 60};
            aabb.max = {aabb.max.x + 40, aabb.max.y + 60};
            width = aabb.max.x - aabb.min.x;
            height = aabb.max.y - aabb.min.y;
            x = aabb.min.x + width*0.5f;
            y = aabb.min.y + height*0.5f;
            TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);


        }
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            TMDebugRendererDrawCircle(graphics->position.x, graphics->position.y, 5, 0xFF22FF22, 10);
        }
    }
    TMDebugRenderDraw();
    

    TMRendererPresent(state->renderer, 1);
}

void GameShutdown(GameState *state) {
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        EntityDestroy(entity);
    }
    EntitySystemShutdown();
    TMDebugRendererShutdown();
    TMRendererRenderBatchDestroy(state->renderer, state->batchRenderer);
    TMRendererTextureDestroy(state->renderer, state->texture);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);

}

