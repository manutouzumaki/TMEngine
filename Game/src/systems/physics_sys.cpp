#include "../game.h"
#include "../entity.h"
#include <utils/tm_darray.h>

#include <math.h>

void IntegrationStep(PhysicsComponent *physics, float dt) {
    physics->lastPosition = physics->position;
    
    physics->velocity = physics->velocity + physics->acceleration * dt;
    physics->potetialPosition = physics->position + physics->velocity * dt;

    float damping = powf(physics->damping, dt);
    physics->velocity = physics->velocity * damping;
}

void CollisionDetectionAndResolution(Entity *entity, Entity **entities, float dt) {
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

                physics->velocity = physics->velocity - TMVec2Project(physics->velocity, normal);
                TMVec2 scaleVelocity = physics->velocity * (1.0f - t );
                physics->potetialPosition = hitP + (normal * 0.002f);
                physics->potetialPosition = physics->potetialPosition + scaleVelocity * dt;
            }  
        }
    }
}

void PhysicSystemUpdate(GameState *state, Entity *entity, float dt) {
    if(entity->physics) {
        PhysicsComponent *physics = entity->physics;
        IntegrationStep(physics, dt);
        CollisionDetectionAndResolution(entity, state->entities, dt);
        physics->position = physics->potetialPosition;
    }
}

void PhysicSystemPostUpdate(Entity *entity, float t) {
    if(entity->graphics && entity->physics) {
        GraphicsComponent *graphics = entity->graphics;
        PhysicsComponent *physics = entity->physics;
        TMVec2 position = physics->position * t + physics->lastPosition * (1.0f - t);
        graphics->position = position;
    }
}


