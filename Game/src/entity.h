#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <utils/tm_math.h>
#include "collision.h"


struct GraphicsComponent {
    TMVec2 position;
    TMVec2 size;
    TMVec4 color;
};

struct PhysicsComponent {
    TMVec2 potetialPosition;
    TMVec2 position;
    TMVec2 lastPosition;
    TMVec2 velocity;
    TMVec2 acceleration;
    float damping;
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
};

struct Entity {
    unsigned int id;
    GraphicsComponent *graphics;
    PhysicsComponent *physics;
    InputComponent *input;
    CollisionComponent *collision;
};


void EntitySystemInitialize(int maxEntityCount);
void EntitySystemShutdown();

Entity *EntityCreate();
void EntityDestroy(Entity *entity);

void EntityAddGraphicsComponent(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color);
void EntityAddPhysicsComponent(Entity *entity, TMVec2 position, TMVec2 velocity, TMVec2 acceleration, float damping);
void EntityAddInputComponent(Entity *entity);

void EntityAddCollisionComponent(Entity *entity, CollisionType type, AABB aabb);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, OBB obb);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, Circle circle);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, Capsule capsule);

void InputSystemUpdate(Entity **entities);




#endif
