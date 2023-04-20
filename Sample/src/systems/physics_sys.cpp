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
    float damping = powf(physics->damping, dt);
    physics->velocity = physics->velocity * damping;
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
        MessageFireFirstHit(MESSAGE_TYPE_COLLISION_DETECTION, (void *)entity, message);

    }


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
    }

}

void PhysicSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_COLLISION_RESOLUTION, NULL, PhysicSystemOnMessage);
}


void PhysicSystemShutdown() {

}

void PhysicSystemUpdate(Entity **entities, float dt) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->physics) {
            PhysicsComponent *physics = entity->physics;
            IntegrationStep(physics, dt);
            Message message{};
            message.ptr[0] = (void *)entities;
            message.f32[2] = dt;
            entity->physics->iterations = 0;
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


