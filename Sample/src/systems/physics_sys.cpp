#include "../entity.h"
#include <utils/tm_darray.h>
#include "../message.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

static void IntegrationStep(PhysicsComponent *physics, float dt) {
    physics->lastPosition = physics->position;
    physics->velocity = physics->velocity + physics->acceleration * dt;
    physics->potetialPosition = physics->position + physics->velocity * dt;

    if(physics->grounded) {
        float damping = powf(0.001f, dt);
        physics->velocity = physics->velocity * damping;
    }else {
        float damping = powf(0.5f, dt);
        physics->velocity = physics->velocity * damping;
    }
}


static void CollisionResolution(Entity *entity, TMVec2 normal, TMVec2 hitP, TMVec2 offset, float t, float dt, int *count, Entity **entities) {

    PhysicsComponent *physics = entity->physics;
    while(*count && physics->iterations < 20) {
        
        if(TMVec2LenSq(offset)) {
            physics->position = (hitP + (normal * 0.005f)) - offset;
        }
        else {
            physics->position = hitP + (normal * 0.005f);
        }
        physics->velocity = physics->velocity - TMVec2Project(physics->velocity, normal)  ;
        TMVec2 scaleVelocity = physics->velocity * (1.0f - t );
        physics->potetialPosition = physics->position + scaleVelocity * dt;

        Message message{};
        message.ptr[0] = (void *)entities;
        message.f32[2] = dt;
        physics->iterations++;
        //printf("Iterations: %d\n", physics->iterations);
        MessageFireFirstHit(MESSAGE_TYPE_COLLISION_DETECTION, (void *)entity, message);

    }


}

static void PhysicsClearForces(Entity **entities) {
    for(int i = 0; i < TMDarraySize(entities); ++i){
        PhysicsComponent *physics = entities[i]->physics;
        if(physics) physics->acceleration = {};
    }
}

static void PhysicsAddForce(Entity *entity, TMVec2 force) {
    PhysicsComponent *physics = entity->physics;
    physics->acceleration = physics->acceleration + force;
}

static void PhysicsAddImpulse(Entity *entity, TMVec2 impulse) {
    PhysicsComponent *physics = entity->physics;
    physics->velocity = physics->velocity + impulse;
}


void PhysicSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {
        case MESSAGE_TYPE_COLLISION_RESOLUTION: {
            Entity *entity = (Entity *)sender;
            TMVec2 normal = message.v2[0];
            TMVec2 hitP = message.v2[1];
            TMVec2 offset = message.v2[2];
            int *count = (int *)message.ptr[3];
            Entity **entities = (Entity **)message.ptr[4];
            float t = message.f32[10];
            float dt = message.f32[11];
            CollisionResolution(entity, normal, hitP, offset, t, dt, count, entities);
            entity->physics->position = entity->physics->potetialPosition;
        } break;
        case MESSAGE_TYPE_PHYSICS_ADD_FORCE: {
            Entity *entity = (Entity *)sender;
            TMVec2 force = message.v2[0];
            PhysicsAddForce(entity, force);
        } break;
        case MESSAGE_TYPE_PHYSICS_ADD_IMPULSE: {
            Entity *entity = (Entity *)sender;
            TMVec2 impulse = message.v2[0];
            PhysicsAddImpulse(entity, impulse);

        } break;
        case MESSAGE_TYPE_PHYSICS_CLEAR_FORCES: {
            Entity **entities = (Entity **)sender;
            PhysicsClearForces(entities);

        } break;
    }

}

void PhysicSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_COLLISION_RESOLUTION, NULL, PhysicSystemOnMessage);
    MessageRegister(MESSAGE_TYPE_PHYSICS_ADD_FORCE, NULL, PhysicSystemOnMessage);
    MessageRegister(MESSAGE_TYPE_PHYSICS_ADD_IMPULSE, NULL, PhysicSystemOnMessage);
    MessageRegister(MESSAGE_TYPE_PHYSICS_CLEAR_FORCES, NULL, PhysicSystemOnMessage);
}


void PhysicSystemShutdown() {

}

void PhysicSystemUpdate(Entity **entities, float dt) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->physics) {


            // NOTE: update the ray for ground check ...
            CollisionComponent *collision = entity->collision;
            PhysicsComponent *physics = entity->physics;
            switch(collision->type) {
                case COLLISION_TYPE_AABB: {
                    AABB *aabb = &collision->aabb;
                    float height = aabb->max.y - aabb->min.y;
                    physics->down.o = physics->position;
                    physics->down.d = {0, -(height*0.5f)*1.05f};

                }break;
                case COLLISION_TYPE_CIRCLE: {
                    Circle *circle = &collision->circle;
                    physics->down.o = physics->position;
                    physics->down.d = {0, -circle->r*1.05f};
                }break;
                case COLLISION_TYPE_CAPSULE: {
                    Capsule *capsule = &collision->capsule;
                    physics->down.o = capsule->b;
                    physics->down.d = {0, -capsule->r*1.05f};
                }break;           
            }

            if(!entity->physics->grounded) {
                TMVec2 gravity = {0, -30.0f};
                PhysicsAddForce(entity, gravity);
            }
            //printf("grounded: %d\n", entity->physics->grounded);
        }
    }

}

void PhysicSystemFixUpdate(Entity **entities, float dt) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->physics) {
            PhysicsComponent *physics = entity->physics;
            IntegrationStep(physics, dt);
            Message message{};
            message.ptr[0] = (void *)entities;
            message.f32[2] = dt;
            entity->physics->iterations = 0;
            MessageFireFirstHit(MESSAGE_TYPE_COLLISION_GROUNDED, (void *)entity, message);
            MessageFireFirstHit(MESSAGE_TYPE_COLLISION_DETECTION, (void *)entity, message);
        }
    }

}

void PhysicSystemPostUpdate(Entity **entities, float t) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->physics && entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            PhysicsComponent *physics = entity->physics;
            TMVec2 position = physics->position * t + physics->lastPosition * (1.0f - t);
            Message message{};
            message.v2[0] = position;
            MessageFireFirstHit(MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS, (void *)entity, message);
        }
    }

}


