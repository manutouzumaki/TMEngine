#include "../entity.h"
#include <utils/tm_darray.h>
#include "../message.h"

#include <math.h>

static void IntegrationStep(PhysicsComponent *physics, float dt) {
    physics->lastPosition = physics->position;
    
    physics->velocity = physics->velocity + physics->acceleration * dt;
    physics->potetialPosition = physics->position + physics->velocity * dt;

    float damping = powf(physics->damping, dt);
    physics->velocity = physics->velocity * damping;
}

static void CollisionResolution(Entity *entity, TMVec2 normal, TMVec2 hitP, float t, float dt) {
    PhysicsComponent *physics = entity->physics;
    physics->velocity = physics->velocity - TMVec2Project(physics->velocity, normal);
    TMVec2 scaleVelocity = physics->velocity * (1.0f - t );
    physics->potetialPosition = hitP + (normal * 0.002f);
    physics->potetialPosition = physics->potetialPosition + scaleVelocity * dt;
}




void PhysicSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {
        case MESSAGE_TYPE_COLLISION_RESOLUTION: {
            Entity *entity = (Entity *)sender;
            TMVec2 normal = message.v2[0];
            TMVec2 hitP = message.v2[1];
            float t = message.f32[4];
            float dt = message.f32[5];
            CollisionResolution(entity, normal, hitP, t, dt);
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
            MessageFireFirstHit(MESSAGE_TYPE_COLLISION_AABBAABB, (void *)entity, message);
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

