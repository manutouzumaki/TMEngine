#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <tm_renderer.h>
#include <utils/tm_math.h>
#include <stdlib.h>
#include "collision.h"

struct GraphicsComponent {

    TMShader *shader;
    TMTexture *texture;

    TMVec4 relUVs;
    TMVec4 absUVs;
    TMVec4 color;

    TMVec2 position;
    TMVec2 size;
    
    int index;
    int zIndex;  // TODO: this probably should be a float
    
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
    int index;
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

struct EnemyMovementComponent {
    bool facingLeft;
    Ray left;
    Ray right;
    Ray downLeft;
    Ray downRight;
};

struct Entity {
    unsigned int id;
    TMVec4 uvs;
    GraphicsComponent *graphics;
    PhysicsComponent *physics;
    InputComponent *input;
    CollisionComponent *collision;
    AnimationComponet *animation;
    EnemyMovementComponent *enemyMovement;
};


void EntitySystemInitialize(int maxEntityCount);
void EntitySystemShutdown();

Entity *EntityCreate();
void EntityDestroy(Entity *entity);


void EntityAddGraphicsComponent(Entity *entity,TMVec2 position, TMVec2 size, TMVec4 color,
                                TMVec4 absUVs, TMVec4 relUVs, int zIndex, TMShader *shader, TMTexture *texture);

void EntityAddPhysicsComponent(Entity *entity, TMVec2 position, TMVec2 velocity, TMVec2 acceleration, float damping);
void EntityAddInputComponent(Entity *entity);

void EntityAddCollisionComponent(Entity *entity, CollisionType type, AABB aabb, bool solid = true);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, OBB obb, bool solid = true);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, Circle circle, bool solid = true);
void EntityAddCollisionComponent(Entity *entity, CollisionType type, Capsule capsule, bool solid = true);

void EntityAddAnimationComponet(Entity *entity);


void EntityAddEnemyMovementComponent(Entity *entity, CollisionComponent *collision, PhysicsComponent *physics);


void InputSystemUpdate(Entity **entities, float dt);

struct CollisionInfo {
    TMVec2 normal;
    TMVec2 hitP;
    TMVec2 offset;
    float t;
};






#endif
