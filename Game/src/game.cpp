#include "game.h"

#include <utils/tm_darray.h>
#include <tm_input.h>

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

    TMRendererFaceCulling(state->renderer, false, 0);
    TMRendererDepthTestDisable(state->renderer);


    EntitySystemInitialize(100);
    

    // create the floor
    Entity *floor = EntityCreate();
    EntityAddGraphicsComponent(floor, {0, -199}, {800, 100}, {1, 1, 0, 1});
    TMDarrayPush(state->entities, floor, Entity *);

    // create the cealing
    Entity *ceal = EntityCreate();
    EntityAddGraphicsComponent(ceal, {0, 199}, {800, 100}, {1, 1, 0, 1});
    TMDarrayPush(state->entities, ceal, Entity *);

    // create the player
    Entity *player = EntityCreate();
    EntityAddGraphicsComponent(player, {0, 0}, {80, 120}, {1, 0, 0, 1});
    EntityAddPhysicsComponent(player, {0, 0}, {0, 0});
    EntityAddInputComponent(player);
    TMDarrayPush(state->entities, player, Entity *);



}

void GameUpdate(GameState *state, float dt) {
    int stopHere = 0;
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];


        if(entity->input && entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            if(TMInputKeyboardKeyIsDown('D')) {
                graphics->position.x += 100.0f * dt;
            }
            if(TMInputKeyboardKeyIsDown('A')) {
                graphics->position.x -= 100.0f * dt;
            }
            if(TMInputKeyboardKeyIsDown('W')) {
                graphics->position.y += 100.0f * dt;
            }
            if(TMInputKeyboardKeyIsDown('S')) {
                graphics->position.y -= 100.0f * dt;
            }
        }


    }
}

void GameFixUpdate(GameState *state, float dt) {

}

void GamePostUpdate(GameState *state, float t) {
}

void GameRender(GameState *state) {
    TMRendererClear(state->renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT);

    
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
    

    TMRendererPresent(state->renderer);
}

void GameShutdown(GameState *state) {
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        EntityDestroy(entity);
    }
    EntitySystemShutdown();
    TMRendererRenderBatchDestroy(state->renderer, state->batchRenderer);
    TMRendererTextureDestroy(state->renderer, state->texture);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);

}

