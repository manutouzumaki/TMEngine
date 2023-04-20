#include "../entity.h"
#include <utils/tm_darray.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "../message.h"
#include <tm_debug_renderer.h>

static TMVec2 closentPoint;

static void UpdateCollider(Entity *entity) {

    if(entity->physics) {
        CollisionComponent *collision = entity->collision;
        PhysicsComponent *physics = entity->physics;
        switch(collision->type) {
                case COLLISION_TYPE_AABB: {
                    AABB *aabb = &collision->aabb;
                    float width = aabb->max.x - aabb->min.x;
                    float height = aabb->max.y - aabb->min.y;
                    float x = aabb->min.x + width*0.5f;
                    float y = aabb->min.y + height*0.5f;
                    aabb->min = {physics->position.x - width*0.5f, physics->position.y - height*0.5f};
                    aabb->max = {physics->position.x + width*0.5f, physics->position.y + height*0.5f}; 
                }break;
                case COLLISION_TYPE_CIRCLE: {
                    Circle *circle = &collision->circle;
                    circle->c = physics->position;

                }break;
                case COLLISION_TYPE_OBB: {

                }break;
                case COLLISION_TYPE_CAPSULE: {
                    Capsule *capsule = &collision->capsule;
                    TMVec2 ab = capsule->b - capsule->a;
                    float halfHeight = TMVec2Len(ab)*0.5f; 
                    TMVec2 up = {0, 1};
                    capsule->a = physics->position + up * halfHeight;
                    capsule->b = physics->position - up * halfHeight;
                }break;           
        }
    }
}

static void AABBAABBCollisionDetection(Entity *entity, Entity *other, float dt, int &count, CollisionInfo *collisionInfo) {
    PhysicsComponent *physics = entity->physics;
    UpdateCollider(entity);
    
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
    if(RayAABB(r.o, r.d, expand, t, p) && t*t < TMVec2LenSq(d)) {
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
        
        collisionInfo[count].normal = normal;
        collisionInfo[count].hitP = hitP;
        collisionInfo[count].offset = {};
        collisionInfo[count].t = t;
        count++;
        
    }  
}



static void CircleAABBCollisionDetection(Entity *entity, Entity *other, float dt, int &count, CollisionInfo *collisionInfo) {
    PhysicsComponent *physics = entity->physics;
    UpdateCollider(entity);
    
    TMVec2 d = physics->potetialPosition - physics->position;

    Circle circle;
    circle.c = physics->position;
    circle.r = entity->collision->circle.r;
    AABB aabb  = other->collision->aabb;
    
    float t = 0.0f;
    if(IntersectMovingCircleAABB(circle, d, aabb, t)) {
        TMVec2 hitP = physics->position + d * t;
        TMVec2 closestP;
        ClosestPtPointAABB(hitP, aabb, closestP);
        closentPoint = closestP;
        TMVec2 normal = TMVec2Normalized(hitP - closestP);
        TMDebugRendererDrawLine(closestP.x, closestP.y, 
                                hitP.x, hitP.y,
                                0xFF0000FF);
    
        collisionInfo[count].normal = normal;
        collisionInfo[count].hitP = hitP;
        collisionInfo[count].offset = {};
        collisionInfo[count].t = t;
        count++;
    }
}


static void CapsuleAABBCollisionDetection(Entity *entity, Entity *other, float dt, int &count, CollisionInfo *collisionInfo) {
    PhysicsComponent *physics = entity->physics;
    UpdateCollider(entity);
    
    AABB aabb  = other->collision->aabb;
    Capsule capsule = entity->collision->capsule;

    TMVec2 closest;
    ClosestPtPointAABB(physics->position, aabb, closest);
    TMVec2 capsulePosition;
    float hitCapsule;
    ClosestPtPointSegement(closest, capsule.a, capsule.b, hitCapsule, capsulePosition);
    TMVec2 offset = capsulePosition - physics->position;

    TMVec2 potetialCapsulePosition = physics->potetialPosition + offset;

    Circle circle;
    circle.c = capsulePosition;
    circle.r = capsule.r;
    TMVec2 d = potetialCapsulePosition - capsulePosition;;
    
    float t = 0.0f;
    if(IntersectMovingCircleAABB(circle, d, aabb, t)) {
        TMVec2 hitP = capsulePosition + d * t;
        TMVec2 closestP;
        ClosestPtPointAABB(hitP, aabb, closestP);
        TMVec2 normal = TMVec2Normalized(hitP - closestP);
        
        closentPoint = closestP;
        TMDebugRendererDrawLine(closestP.x, closestP.y, 
                                hitP.x, hitP.y,
                                0xFF0000FF);

        collisionInfo[count].normal = normal;
        collisionInfo[count].hitP = hitP;
        collisionInfo[count].offset = offset;
        collisionInfo[count].t = t;
        count++;

    }
}

#if 0
static void SortHitEntities(Entity **entities, float *hitT, int count) {
    for(int j = 1; j < count; ++j) {
        Entity *entityKey = entities[j];
        float key = hitT[j];
        int i = j - 1;
        
        while(i >= 0 && hitT[i] > key) {
            entities[i + 1] = entities[i];
            hitT[i + 1] = hitT[i];
            --i;
        }

        entities[i + 1] = entityKey;
        hitT[i + 1] = key;
    }
}
#endif


void CollisionSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {
        case MESSAGE_TYPE_COLLISION_DETECTION: {
            Entity *entity    = (Entity *)sender;
            Entity **entities = (Entity **)message.ptr[0];
            float dt          = message.f32[2];
            if(!entity->collision) return;

            int collisionCount = 0;
            CollisionInfo collisionInfo[10] = {};
 
            for(int j = 0; j < TMDarraySize(entities); ++j) {
                Entity *other = entities[j];
                if(other != entity && other->collision) {
                    CollisionType a = entity->collision->type;
                    CollisionType b = other->collision->type;
                    if(a == COLLISION_TYPE_AABB && b == COLLISION_TYPE_AABB) {
                        AABBAABBCollisionDetection(entity, other, dt, collisionCount, collisionInfo);
                    }
                    if(a == COLLISION_TYPE_CIRCLE && b == COLLISION_TYPE_AABB) {
                        CircleAABBCollisionDetection(entity, other, dt, collisionCount, collisionInfo);
                    }
                    if(a == COLLISION_TYPE_CAPSULE && b == COLLISION_TYPE_AABB) {
                        CapsuleAABBCollisionDetection(entity, other, dt, collisionCount, collisionInfo);
                    }
                    // TODO: others collision
                }
            }
            entity->collision->count = collisionCount;

            // NOTE: this is important nor remove please!!!!
            float minor = FLT_MAX;
            int index = 0;
            for(int j = 0; j < collisionCount; ++j) {
                if(collisionInfo[j].t < minor) {
                    minor = collisionInfo[j].t;
                    index = j;
                }
            } 

            // NOTE: send message to collision resolution
            Message message{};  // NOTE: this must be zero initialized
            message.v2[0] = collisionInfo[index].normal;
            message.v2[1] = collisionInfo[index].hitP;
            message.v2[2] = collisionInfo[index].offset;
            message.ptr[3] = &entity->collision->count;
            message.ptr[4] = (void *)entities;
            message.f32[10] = collisionInfo[index].t;
            message.f32[11] = dt;
            MessageFireFirstHit(MESSAGE_TYPE_COLLISION_RESOLUTION, (void *)entity, message);


        }break;
    }
}

void CollisionSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_COLLISION_DETECTION, NULL, CollisionSystemOnMessage);
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

    TMDebugRendererDrawCircle(closentPoint.x, closentPoint.y, 0.05, 0xFFFFFF00, 10);


}


