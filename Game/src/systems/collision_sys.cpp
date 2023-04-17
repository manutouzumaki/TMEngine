#include "../entity.h"
#include <utils/tm_darray.h>
#include <stdlib.h>
#include <stdio.h>
#include "../message.h"
#include <tm_debug_renderer.h>

static TMVec2 closentPoint;

static void UpdateCollider(Entity *entity) {

    if(entity->graphics) {
        GraphicsComponent *graphics = entity->graphics;
        CollisionComponent *collision = entity->collision;
        switch(collision->type) {
                case COLLISION_TYPE_AABB: {
                    AABB *aabb = &collision->aabb;
                    float width = aabb->max.x - aabb->min.x;
                    float height = aabb->max.y - aabb->min.y;
                    float x = aabb->min.x + width*0.5f;
                    float y = aabb->min.y + height*0.5f;
                    aabb->min = {graphics->position.x - width*0.5f, graphics->position.y - height*0.5f};
                    aabb->max = {graphics->position.x + width*0.5f, graphics->position.y + height*0.5f}; 
                }break;
                case COLLISION_TYPE_CIRCLE: {
                    Circle *circle = &collision->circle;
                    circle->c = graphics->position;

                }break;
                case COLLISION_TYPE_OBB: {

                }break;
                case COLLISION_TYPE_CAPSULE: {

                }break;           
        }
    }
}

static void AABBAABBCollisionDetection(Entity *entity, Entity *other, float dt) {
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

        Message message{};
        message.v2[0] = normal;
        message.v2[1] = hitP;
        message.f32[4] = t;
        message.f32[5] = dt;
        MessageFireFirstHit(MESSAGE_TYPE_COLLISION_RESOLUTION, (void *)entity, message);
    }  
}



static void CircleAABBCollisionDetection(Entity *entity, Entity *other, float dt) {
    PhysicsComponent *physics = entity->physics;
    
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

        Message message{};
        message.v2[0] = normal;
        message.v2[1] = hitP;
        message.f32[4] = t;
        message.f32[5] = dt;
        MessageFireFirstHit(MESSAGE_TYPE_COLLISION_RESOLUTION, (void *)entity, message);
    }
}

void CollisionSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {
        case MESSAGE_TYPE_COLLISION_DETECTION: {
            Entity *entity    = (Entity *)sender;
            Entity **entities = (Entity **)message.ptr[0];
            float dt          = message.f32[2];
            if(!entity->collision) return;
            for(int j = 0; j < TMDarraySize(entities); ++j) {
                Entity *other = entities[j];
                if(other != entity && other->collision) {
                    CollisionType a = entity->collision->type;
                    CollisionType b = other->collision->type;
                    if(a == COLLISION_TYPE_AABB && b == COLLISION_TYPE_AABB) {
                        AABBAABBCollisionDetection(entity, other, dt);
                    }
                    if(a == COLLISION_TYPE_CIRCLE && b == COLLISION_TYPE_AABB) {
                        CircleAABBCollisionDetection(entity, other, dt);
                    }
                    // TODO: others collision
                }
            }
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


