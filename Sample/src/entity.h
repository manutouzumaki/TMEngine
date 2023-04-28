#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <utils/tm_math.h>
#include <stdlib.h>
#include "collision.h"


enum GraphicsComponentType {
    GRAPHICS_TYPE_SOLID_COLOR,
    GRAPHICS_TYPE_SPRITE,
    GRAPHICS_TYPE_SUBSPRITE
};

struct GraphicsComponent {
    TMVec2 position;
    TMVec2 size;
    TMVec4 color;
    TMVec4 absUVs;
    float *relUVs;
    int index;
    GraphicsComponentType type;
};

struct AnimationState {
    int frames[32];
    int frameCount;
    float speed;
};

struct AnimationComponet {
    AnimationState *states;
    AnimationState *current;
    int statesCount;
    float timer;
};

struct PhysicsComponent {
    TMVec2 potetialPosition;
    TMVec2 position;
    TMVec2 lastPosition;
    TMVec2 velocity;
    TMVec2 acceleration;
    Ray down;
    float damping;
    int iterations;
    bool grounded;
};

struct InputComponent {
};

enum CollisionType {
    COLLISION_TYPE_AABB,
    COLLISION_TYPE_OBB,
    COLLISION_TYPE_CIRCLE,
    COLLISION_TYPE_CAPSULE
};

struct CollisionComponent {
    CollisionType type;
    union {
        AABB aabb;
        OBB obb;
        Circle circle;
        Capsule capsule;
    };
    int count;
    bool solid;
};

struct Entity {
    unsigned int id;
    GraphicsComponent *graphics;
    PhysicsComponent *physics;
    InputComponent *input;
    CollisionComponent *collision;
    AnimationComponet *animation;
};


void EntitySystemInitialize(int maxEntityCount);
void EntitySystemShutdown();

Entity *EntityCreate();
void EntityDestroy(Entity *entity);


void EntityAddGraphicsComponentSolidColor(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color);
void EntityAddGraphicsComponentSprite(Entity *entity, TMVec2 position, TMVec2 size, float *uvs);
void EntityAddGraphicsComponentSubSprite(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color,
                                         TMVec4 absUVs, int index, float *uvs);


void EntityAddPhysicsComponent(Entity *entity, TMVec2 position, TMVec2 velocity, TMVec2 acceleration, float damping);
void EntityAddInputComponent(Entity *entity);

void EntityAddCollisionComponent(Entity *entity, CollisionType type, AABB aabb, bool solid = true);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, OBB obb, bool solid = true);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, Circle circle, bool solid = true);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, Capsule capsule, bool solid = true);

void EntityAddAnimationComponet(Entity *entity);

void InputSystemUpdate(Entity **entities, float dt);

struct CollisionInfo {
    TMVec2 normal;
    TMVec2 hitP;
    TMVec2 offset;
    float t;
};






#endif
