#include "../entity.h"
#include <utils/tm_darray.h>
#include "../message.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

static void IntegrationStep(PhysicsComponent *physics, float dt) {
    physics->lastlastPosition = physics->lastPosition;
    physics->lastPosition = physics->position;
    physics->velocity = physics->velocity + physics->acceleration * dt;
    physics->potetialPosition = physics->position + physics->velocity * dt;
    float damping = powf(physics->damping, dt);
    physics->velocity = physics->velocity * damping;
}


static void CollisionResolution(Entity *entity, TMVec2 normal, TMVec2 hitP, TMVec2 offset, float t, float dt, int count) {

    PhysicsComponent *physics = entity->physics;
    if(TMVec2LenSq(offset)) {
        physics->potetialPosition = hitP + (normal * 0.002f) + (physics->potetialPosition - offset);
    }
    else {
        physics->potetialPosition = hitP + (normal * 0.002f);
    }
    physics->velocity = physics->velocity - TMVec2Project(physics->velocity, normal)  ;
    TMVec2 scaleVelocity = physics->velocity * (1.0f - t );
    physics->potetialPosition = physics->potetialPosition + scaleVelocity * dt;

    if(count) {
        physics->potetialPosition = physics->lastlastPosition;
        physics->velocity = {};
        physics->acceleration = {};
    }
}


void PhysicSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {
        case MESSAGE_TYPE_COLLISION_RESOLUTION: {
            Entity *entity = (Entity *)sender;
            TMVec2 normal = message.v2[0];
            TMVec2 hitP = message.v2[1];
            TMVec2 offset = message.v2[2];
            float t = message.f32[6];
            float dt = message.f32[7];
            int count = message.i32[8];
            CollisionResolution(entity, normal, hitP, offset, t, dt, count);
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
            // TODO: try other solution for this
            // TODO: this is not a full solution
            MessageFireFirstHit(MESSAGE_TYPE_COLLISION_DETECTION, (void *)entity, message);
            physics->position = physics->potetialPosition;
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


