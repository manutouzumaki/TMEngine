#include "../entity.h"
#include <utils/tm_darray.h>
#include <stdlib.h>
#include "../message.h"

static void UpdateCollider(Entity *entity) {

    if(entity->graphics) {
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

static void AABBAABBCollisionDetection(Entity *entity, Entity **entities, float dt) {
    for(int j = 0; j < TMDarraySize(entities); ++j) {
        Entity *other = entities[j];
        if(other != entity && other->collision && entity->collision) {
            PhysicsComponent *physics = entity->physics;
            
            AABB entityAABB = entity->collision->aabb;
            AABB otherAABB  = other->collision->aabb;

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

                Message message{};
                message.v2[0] = normal;
                message.v2[1] = hitP;
                message.f32[4] = t;
                message.f32[5] = dt;
                MessageFireFirstHit(MESSAGE_TYPE_COLLISION_RESOLUTION, (void *)entity, message);
            }  
        }
    }
}

void CollisionSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {
        case MESSAGE_TYPE_COLLISION_AABBAABB: {
            Entity *entity   = (Entity *)sender;
            Entity **entities = (Entity **)message.ptr[0];
            float dt         = message.f32[2];
            AABBAABBCollisionDetection(entity, entities, dt);
        }break;
    }
}

void CollisionSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_COLLISION_AABBAABB, NULL, CollisionSystemOnMessage);
}

void CollisionSystemShutdown() {

}

void CollisionSystemUpdate(Entity **entities) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->collision) {
            UpdateCollider(entity);
        }

    }

}


