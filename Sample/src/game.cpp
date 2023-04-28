#include "game.h"

#include <utils/tm_darray.h>
#include <tm_debug_renderer.h>
#include "message.h"
#include "systems/physics_sys.h"
#include "systems/collision_sys.h"
#include "systems/graphics_sys.h"
#include "systems/animation_sys.h"

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

    

    const char *images[] = {
        "../../assets/images/moon.png",
        "../../assets/images/paddle_1.png",
        "../../assets/images/characters_packed.png",
        "../../assets/images/clone.png",
        "../../assets/images/player.png",
        "../../assets/images/paddle_2.png"
    };

    // TODO Shitty code DELETE this before i cry please ....
    state->texture = TMRendererTextureCreate(state->renderer,
                                             "../../assets/images/player.png");
    state->relUVs = TMGenerateUVs(state->texture, 16, 16);
    TMRendererTextureDestroy(state->renderer, state->texture);
    state->texture = NULL;

    state->absUVs = TMHashmapCreate(sizeof(TMVec4));
    state->texture = TMRendererTextureCreateAtlas(state->renderer, images, ARRAY_LENGTH(images), 1024*2, 1024*2, state->absUVs);


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
    AnimationSystemInitialize();


    EntitySystemInitialize(100);
    
    AABB aabb{};

    // create the cealing
    Entity *ceal = EntityCreate();
    EntityAddGraphicsComponentSolidColor(ceal, {0, 1.9}, {4, 1}, {0, 0.2, 0.4, 1});
    aabb.min = {0 - 2, 1.9 - 0.5};
    aabb.max = {0 + 2, 1.9 + 0.5};
    EntityAddCollisionComponent(ceal, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, ceal, Entity *);
    
    // create the cealing
    Entity *ceal1 = EntityCreate();
    EntityAddGraphicsComponentSolidColor(ceal1, {-4, 1.9}, {4, 1}, {0.5, 0.2, 0.4, 1});
    aabb.min = {-4 - 2, 1.9 - 0.5};
    aabb.max = {-4 + 2, 1.9 + 0.5};
    EntityAddCollisionComponent(ceal1, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, ceal1, Entity *);

    Entity *player2 = EntityCreate();
    EntityAddGraphicsComponentSolidColor(player2, {1.3, -0.2}, {0.8, 1}, {1, 0.8, 0.2, 1});
    aabb.min = {1.3 - 0.4, -0.2 - 0.5};
    aabb.max = {1.3 + 0.4, -0.2 + 0.5};
    EntityAddCollisionComponent(player2, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player2, Entity *);

    Entity *player3 = EntityCreate();
    EntityAddGraphicsComponentSolidColor(player3, {-2.3, 0.6}, {0.8, 1}, {1, 0.2, 0.5, 1});
    aabb.min = {-2.3 - 0.4, 0.6 - 0.5};
    aabb.max = {-2.3 + 0.4, 0.6 + 0.5};
    EntityAddCollisionComponent(player3, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player3, Entity *);

    Entity *player4 = EntityCreate();
    EntityAddGraphicsComponentSolidColor(player4, {4.5, 0.0}, {0.8, 1}, {1, 0.2, 0.5, 1});
    aabb.min = {4.5 - 0.4, 0.0 - 0.5};
    aabb.max = {4.5 + 0.4, 0.0 + 0.5};
    EntityAddCollisionComponent(player4, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player4, Entity *);

    Entity *player5 = EntityCreate();
    EntityAddGraphicsComponentSolidColor(player5, {4.5 + 0.8, 0.0}, {0.8, 1}, {1, 0.2, 0.5, 1});
    aabb.min = {4.5 + 0.8 - 0.4, 0.0 - 0.5};
    aabb.max = {4.5 + 0.8 + 0.4, 0.0 + 0.5};
    EntityAddCollisionComponent(player5, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, player5, Entity *);

    // create the floor
    Entity *floor = EntityCreate();
    EntityAddGraphicsComponentSprite(floor, {0, -1.9}, {8, 1}, (float *)TMHashmapGet(state->absUVs, "../../assets/images/paddle_1.png"));
    aabb.min = {0 - 4, -1.9 - 0.5};
    aabb.max = {0 + 4, -1.9 + 0.5};
    EntityAddCollisionComponent(floor, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, floor, Entity *);


    Entity *floor2 = EntityCreate();
    EntityAddGraphicsComponentSprite(floor2, {-8, -1.9}, {8, 1}, (float *)TMHashmapGet(state->absUVs, "../../assets/images/paddle_2.png"));
    aabb.min = {-8 - 4, -1.9 - 0.5};
    aabb.max = {-8 + 4, -1.9 + 0.5};
    EntityAddCollisionComponent(floor2, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, floor2, Entity *);

    Entity *floor3 = EntityCreate();
    EntityAddGraphicsComponentSprite(floor3, {-8, -0.5f}, {2, 2}, (float *)TMHashmapGet(state->absUVs, "../../assets/images/moon.png"));
    aabb.min = {-8 - 1, -0.5f - 1};
    aabb.max = {-8 + 1, -0.5f + 1};
    EntityAddCollisionComponent(floor3, COLLISION_TYPE_AABB, aabb);
    TMDarrayPush(state->entities, floor3, Entity *);

    // create the player
    Entity *player = EntityCreate();
    state->player = player;
    EntityAddInputComponent(player);
    TMVec4 *uvs = (TMVec4 *)TMHashmapGet(state->absUVs, "../../assets/images/player.png");
    EntityAddGraphicsComponentSubSprite(player, {-5, 0}, {1.2, 1.2}, {1, 0, 0, 1}, *uvs, 0, state->relUVs);
    EntityAddPhysicsComponent(player, {-5, 0}, {0, 0}, {0, 0}, 0.01f);
    Capsule capsule;
    capsule.r = 0.4;
    capsule.a = {-5.0, 0.2};
    capsule.b = {-5.0, -0.2};
    EntityAddCollisionComponent(player, COLLISION_TYPE_CAPSULE, capsule);

    AnimationState walkLeft;
    walkLeft.frames[0] = 0;
    walkLeft.frames[1] = 1;
    walkLeft.frames[2] = 2;
    walkLeft.frames[3] = 3;
    walkLeft.frameCount = 4;
    walkLeft.speed = 15.0f;

    AnimationState walkRight;
    walkRight.frames[0] = 4;
    walkRight.frames[1] = 5;
    walkRight.frames[2] = 6;
    walkRight.frames[3] = 7;
    walkRight.frameCount = 4;
    walkRight.speed = 15.0f;

    AnimationState idleLeft;
    idleLeft.frames[0] = 0;
    idleLeft.frames[1] = 3;
    idleLeft.frameCount = 2;
    idleLeft.speed = 7.0f;

    AnimationState idleRight;
    idleRight.frames[0] = 4;
    idleRight.frames[1] = 7;
    idleRight.frameCount = 2;
    idleRight.speed = 7.0f;

    EntityAddAnimationComponet(player);
    AnimationSystemAddState(player, walkLeft);
    AnimationSystemAddState(player, walkRight);
    AnimationSystemAddState(player, idleLeft);
    AnimationSystemAddState(player, idleRight);

    TMDarrayPush(state->entities, player, Entity *);


    // create the Enemy
    Entity *enemy = EntityCreate();
    EntityAddGraphicsComponentSubSprite(enemy, {0, 10}, {1.2, 1.2}, {1, 0, 0, 0}, *uvs, 0, state->relUVs);
    EntityAddPhysicsComponent(enemy, {0, 10}, {0, 0}, {0, 0}, 0.01f);
    capsule.r = 0.4;
    capsule.a = {0.0, 10.0f + 0.2};
    capsule.b = {0.0, 10.0f + -0.2};
    EntityAddCollisionComponent(enemy, COLLISION_TYPE_CAPSULE, capsule);
    EntityAddAnimationComponet(enemy);
    AnimationSystemAddState(enemy, idleLeft);
    TMDarrayPush(state->entities, enemy, Entity *);

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
    TMRendererClear(state->renderer, 0.6, 0.6, 0.9, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);


    TMRendererBindShader(state->renderer, state->shader);
    GraphicsSystemDraw(state->batchRenderer, state->entities);
    TMDebugRenderDraw();

    TMRendererPresent(state->renderer, 1);
}

void GameShutdown(GameState *state) {
    // TODO: this should be handle by the entity system not the game directly
    TMHashmapDestroy(state->absUVs);
    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities[i];
        EntityDestroy(entity);
    }
    EntitySystemShutdown();
    free(state->relUVs);
    GraphicsSystemShutdown();
    CollisionSystemShutdown();
    PhysicSystemShutdown();
    AnimationSystemShutdown(state->entities);
    MessageSystemShoutdown();
    TMDebugRendererShutdown();
    TMRendererRenderBatchDestroy(state->renderer, state->batchRenderer);
    TMRendererTextureDestroy(state->renderer, state->texture);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);

}

