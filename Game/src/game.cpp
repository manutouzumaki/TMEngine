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

static TMVec2 closestPoint;

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
    
    AABB *aabb = state->colliders;

    // create the floor
    Entity *floor = EntityCreate();
    EntityAddGraphicsComponent(floor, {0, -199}, {800, 100}, {0, 0.2, 0.4, 1});
    TMDarrayPush(state->entities, floor, Entity *);
    aabb->min = {0 - 400, -199 - 50};
    aabb->max = {0 + 400, -199 + 50};
    aabb++;

    // create the cealing
    Entity *ceal = EntityCreate();
    EntityAddGraphicsComponent(ceal, {0, 199}, {400, 100}, {0, 0.2, 0.4, 1});
    TMDarrayPush(state->entities, ceal, Entity *);
    aabb->min = {0 - 400, 199 - 50};
    aabb->max = {0 + 400, 199 + 50};
    aabb++;

    // create the cealing
    Entity *ceal1 = EntityCreate();
    EntityAddGraphicsComponent(ceal1, {-400, 199}, {400, 100}, {0.5, 0.2, 0.4, 1});
    TMDarrayPush(state->entities, ceal1, Entity *);
    aabb->min = {-400 - 400, 199 - 50};
    aabb->max = {-400 + 400, 199 + 50};
    aabb++;

    // create the player
    Entity *player = EntityCreate();
    EntityAddGraphicsComponent(player, {0, 0}, {80, 120}, {1, 0, 0, 1});
    EntityAddPhysicsComponent(player, {0, 0}, {0, 0}, {0, 0}, 0.01f);
    EntityAddInputComponent(player);
    TMDarrayPush(state->entities, player, Entity *);
    aabb->min = {0 - 40, 0 - 60};
    aabb->max = {0 + 40, 0 + 60};


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
        if(entity->physics) {
            PhysicsComponent *physics = entity->physics;

            physics->lastPosition = physics->position;
            
            physics->velocity = physics->velocity + physics->acceleration * dt;
            physics->potetialPosition = physics->position + physics->velocity * dt;

            float damping = powf(physics->damping, dt);
            physics->velocity = physics->velocity * damping;

            for(int j = 0; j < TMDarraySize(state->entities); ++j) {
                Entity *other = state->entities[j];
                if(other != entity) {
                    AABB entityAABB = state->colliders[i];
                    AABB otherAABB  = state->colliders[j];

                    TMVec2 d = physics->potetialPosition - physics->position;
                    Ray r{};
                    r.o = physics->position;
                    r.d = TMVec2Normalized(d);
                    float t = -1.0f;
                    TMVec2 p;


                    float width = entityAABB.max.x - entityAABB.min.x;
                    float height = entityAABB.max.y - entityAABB.min.y;

                    AABB expand;
                    expand.min = {otherAABB.min.x - width*0.5f, otherAABB.min.y - height*0.5f};
                    expand.max = {otherAABB.max.x + width*0.5f, otherAABB.max.y + height*0.5f};
                    if(RayAAABB(r.o, r.d, expand, t, p) && t*t < TMVec2LenSq(d)) {
                        t /= TMVec2Len(d); 
                        TMVec2 hitP = r.o + d * t;
                        TMVec2 closestP;
                        ClosestPtPointAABB(hitP, otherAABB, closestP);
                        closestPoint = closestP;
                        TMVec2 normal = TMVec2Normalized(hitP - closestP);
                        if(otherAABB.max.x <= entityAABB.min.x) {
                            normal = {1.0f, 0.0f};
                        }
                        if(otherAABB.min.x >= entityAABB.max.x) {
                            normal = {-1.0f, 0.0f};
                        }
                        if(otherAABB.max.y <= entityAABB.min.y) {
                            normal = {0.0f, 1.0f};
                        }
                        if(otherAABB.min.y >= entityAABB.max.y) {
                            normal = {0.0f, -1.0f};
                        }
                        

                        physics->velocity = physics->velocity - TMVec2Project(physics->velocity, normal);

                        TMVec2 scaleVelocity = physics->velocity * (1.0f - t );

                        physics->potetialPosition = hitP + (normal * 0.002f);
                        

                        physics->potetialPosition = physics->potetialPosition + scaleVelocity * dt;
                    }  


                }
            }

            physics->position = physics->potetialPosition;

        }


    }
}

void GamePostUpdate(GameState *state, float t) {
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        if(entity->graphics && entity->physics) {

            GraphicsComponent *graphics = entity->graphics;
            PhysicsComponent *physics = entity->physics;
            TMVec2 position = physics->position * t + physics->lastPosition * (1.0f - t);
            graphics->position = position;


            TMVec3 pos = {graphics->position.x, graphics->position.y, 0};
            TMVec3 tar = {0, 0, 1};
            TMVec3 up  = {0, 1, 0};
            state->view = TMMat4LookAt(pos, pos + tar, up);
            ShaderMatrix mats{};
            mats.proj = state->proj;
            mats.view = state->view;
            mats.world = TMMat4Identity();
            state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &mats, sizeof(ShaderMatrix), 0);
        }
    }

    Entity **entity = state->entities;
    AABB *aabb = state->colliders;
    GraphicsComponent *graphics = entity[0]->graphics;
    aabb->min = {graphics->position.x - 400, graphics->position.y - 50};
    aabb->max = {graphics->position.x + 400, graphics->position.y + 50};
    aabb++;
    graphics = entity[1]->graphics;
    aabb->min = {graphics->position.x - 200, graphics->position.y - 50};
    aabb->max = {graphics->position.x + 200, graphics->position.y + 50};
    aabb++;
    graphics = entity[2]->graphics;
    aabb->min = {graphics->position.x - 200, graphics->position.y - 50};
    aabb->max = {graphics->position.x + 200, graphics->position.y + 50};
    aabb++;
    graphics = entity[3]->graphics;
    aabb->min = {graphics->position.x - 40, graphics->position.y - 60};
    aabb->max = {graphics->position.x + 40, graphics->position.y + 60};
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

            TMDebugRendererDrawCircle(graphics->position.x, graphics->position.y, 5, 0xFF22FF22, 10);
        }
    }
    TMRendererRenderBatchDraw(state->batchRenderer);

    for(int i = 0; i < 4; ++i) {
        AABB aabb = state->colliders[i];
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

    TMDebugRendererDrawCircle(closestPoint.x, closestPoint.y, 5, 0xFF2FFFF2, 10);
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

