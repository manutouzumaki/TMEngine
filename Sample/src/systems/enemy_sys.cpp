#include "enemy_sys.h"

#include "../message.h"
#include "../entity.h"
#include <utils/tm_darray.h>


void EnemyUpdateMovementComponent(Entity *entity) {

    float width = 0;

    CollisionComponent *collision = entity->collision;
    PhysicsComponent *physics = entity->physics;

    switch(collision->type) {
        case COLLISION_TYPE_AABB: {
            width = collision->aabb.max.x - collision->aabb.min.x;
        } break;
        case COLLISION_TYPE_CAPSULE: {
            width = collision->capsule.r*2.0f;
        } break;
        case COLLISION_TYPE_CIRCLE: {
            width = collision->circle.r*2.0f;
        } break;

    }

    entity->enemyMovement->downLeft  = physics->down;
    entity->enemyMovement->downLeft.o.x -= width * 0.5f;

    entity->enemyMovement->downRight = physics->down;
    entity->enemyMovement->downRight.o.x += width * 0.5f;

    entity->enemyMovement->left  = {physics->down.o, {(-width*0.5f)-0.08f, 0}}; 
    entity->enemyMovement->right = {physics->down.o, { (width*0.5f)+0.08f, 0}};

}

static bool RayEntitities(Entity *entity, Entity **entities, Ray srcRay) {

    bool flag = false;
    for(int j = 0; j < TMDarraySize(entities); ++j) {
        Entity *other = entities[j];
        if(other != entity && other->collision) {
            CollisionType type = other->collision->type;
            if(type == COLLISION_TYPE_AABB) {
                AABB aabb = other->collision->aabb;
                Ray ray = srcRay;
                TMVec2 d = TMVec2Normalized(ray.d);
                TMVec2 q;
                float t;
                if(RayAABB(ray.o, d, aabb, t, q) && t*t < TMVec2LenSq(ray.d)) {
                    flag = true;
                }
            }
        }
    }
    return flag;
}

void EnemyProccesMovementComponent(Entity *entity, Entity **entities) {

    entity->physics->velocity.x = 2.0f;
    if(entity->enemyMovement->facingLeft) {
        entity->physics->velocity.x = -2.0f;
    }

    if(entity->physics->grounded) {
        if(!RayEntitities(entity, entities, entity->enemyMovement->downRight)){
            entity->enemyMovement->facingLeft = true;
        }
        if(!RayEntitities(entity, entities, entity->enemyMovement->downLeft)){
            entity->enemyMovement->facingLeft = false;
        }
        if(RayEntitities(entity, entities, entity->enemyMovement->right)) {
            entity->enemyMovement->facingLeft = true;
        }
        if(RayEntitities(entity, entities, entity->enemyMovement->left)) {
            entity->enemyMovement->facingLeft = false;
        }
    }

    Message message;
    if (entity->enemyMovement->facingLeft){
        message.i32[0] = 0;
        MessageFireFirstHit(MESSAGE_TYPE_ANIMATION_SET_STATE, (void *)entity, message);
    }
    else {
        message.i32[0] = 1;
        MessageFireFirstHit(MESSAGE_TYPE_ANIMATION_SET_STATE, (void *)entity, message);
    }

}

void EnemySystemUpdate(Entity **entities) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];

        
        if(entity->enemyMovement) {
            EnemyUpdateMovementComponent(entity);
            EnemyProccesMovementComponent(entity, entities);
        }
    }

}
