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

static float MetersToPixel = 100;

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


    EntitySystemInitialize(100);
    
    AABB aabb{};

    // create the floor
    Entity *floor = EntityCreate();
    EntityAddGraphicsComponent(floor, {0, -1.9}, {8, 1}, {0, 0.2, 0.4, 1});
    aabb.min = {0 - 4, -1.9 - 0.5};
    aabb.max = {0 + 4, -1.9 + 0.5};
    EntityAddCollisionComponent(floor, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, floor, Entity *);

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

    // create the player
    Entity *player = EntityCreate();
    state->player = player;
    EntityAddGraphicsComponent(player, {0, 0}, {0.8, 1.2}, {1, 0, 0, 1});
    EntityAddPhysicsComponent(player, {0, 0}, {0, 0}, {0, 0}, 0.01f);
    EntityAddInputComponent(player);
    aabb.min = {0 - 0.4, 0 - 0.6};
    aabb.max = {0 + 0.4, 0 + 0.6};
    EntityAddCollisionComponent(player, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player, Entity *);

    Entity *player2 = EntityCreate();
    EntityAddGraphicsComponent(player2, {0.9, 0.4}, {0.8, 1}, {1, 0.2, 0.5, 1});
    EntityAddPhysicsComponent(player2, {0.9, 0.4}, {0, 0}, {0, 0}, 0.0001f);
    aabb.min = {0.9 - 0.4, 0.4 - 0.5};
    aabb.max = {0.9 + 0.4, 0.4 + 0.5};
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
              physics->acceleration = TMVec2Normalized(acceleration) * 20.0f;
            }
            else {
                physics->acceleration = acceleration;
            }


            printf("player x: %f, player y: %f\n", physics->position.x, physics->position.y);
        
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

    int width = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);
    GraphicsComponent *graphics = state->player->graphics;
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
            TMDebugRendererDrawCircle(aabb.min.x, aabb.min.y, 5.0f/MetersToPixel, 0xFFFFFF00, 10);
            TMDebugRendererDrawCircle(aabb.max.x, aabb.max.y, 5.0f/MetersToPixel, 0xFF00FFFF, 10);

            aabb.min = {aabb.min.x - 0.4f, aabb.min.y - 0.6f};
            aabb.max = {aabb.max.x + 0.4f, aabb.max.y + 0.6f};
            width = aabb.max.x - aabb.min.x;
            height = aabb.max.y - aabb.min.y;
            x = aabb.min.x + width*0.5f;
            y = aabb.min.y + height*0.5f;
            TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);


        }
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            TMDebugRendererDrawCircle(graphics->position.x, graphics->position.y, 5.0f/MetersToPixel, 0xFF22FF22, 10);
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

