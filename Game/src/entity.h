#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <utils/tm_math.h>


struct GraphicsComponent {
    TMVec2 position;
    TMVec2 size;
    TMVec4 color;
};

struct PhysicsComponent {
    TMVec2 position;
    TMVec2 lastPosition;
    TMVec2 velocity;
    TMVec2 acceleration;
    float damping;

};

struct InputComponent {
    
};

struct Entity {
    unsigned int id;
    GraphicsComponent *graphics;
    PhysicsComponent *physics;
    InputComponent *input;
};


void EntitySystemInitialize(int maxEntityCount);
void EntitySystemShutdown();

Entity *EntityCreate();
void EntityDestroy(Entity *entity);

void EntityAddGraphicsComponent(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color);
void EntityAddPhysicsComponent(Entity *entity, TMVec2 position, TMVec2 velocity, TMVec2 acceleration, float damping);
void EntityAddInputComponent(Entity *entity);



#endif
