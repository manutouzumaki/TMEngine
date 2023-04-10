#include "entity.h"

#include <utils/tm_memory_pool.h>
#include <memory.h>
#include <assert.h>

static unsigned int entityCount;
static TMMemoryPool *entityMem;
static TMMemoryPool *graphicsComponenMem;
static TMMemoryPool *physicsComponentMem;
static TMMemoryPool *inputComponentMem;


void EntitySystemInitialize(int maxEntityCount) {
    entityCount = 0;
    entityMem = TMMemoryPoolCreate(sizeof(Entity), maxEntityCount);
    graphicsComponenMem = TMMemoryPoolCreate(sizeof(GraphicsComponent), maxEntityCount);
    physicsComponentMem = TMMemoryPoolCreate(sizeof(PhysicsComponent), maxEntityCount);
    inputComponentMem = TMMemoryPoolCreate(sizeof(InputComponent), maxEntityCount);
}

void EntitySystemShutdown() {
    TMMemoryPoolDestroy(entityMem);
    TMMemoryPoolDestroy(graphicsComponenMem);
    TMMemoryPoolDestroy(physicsComponentMem);
    TMMemoryPoolDestroy(inputComponentMem);

}

Entity *EntityCreate() {
    Entity *entity = (Entity *)TMMemoryPoolAlloc(entityMem);
    memset(entity, 0, sizeof(Entity));
    return entity;
}

void EntityAddGraphicsComponent(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color) {
    assert(entity->graphics == NULL);
    entity->graphics = (GraphicsComponent *)TMMemoryPoolAlloc(graphicsComponenMem);
    entity->graphics->position = position;
    entity->graphics->size = size;
    entity->graphics->color = color;
}

void EntityAddPhysicsComponent(Entity *entity, TMVec2 position, TMVec2 velocity, TMVec2 acceleration, float damping) {
    assert(entity->physics == NULL);
    entity->physics = (PhysicsComponent *)TMMemoryPoolAlloc(physicsComponentMem);
    entity->physics->position = position;
    entity->physics->velocity = velocity;
    entity->physics->acceleration = acceleration;
    entity->physics->damping = damping;
}

void EntityAddInputComponent(Entity *entity) {
    assert(entity->input == NULL);
    entity->input = (InputComponent *)TMMemoryPoolAlloc(inputComponentMem);
}


void EntityDestroy(Entity *entity) {
    if(entity->graphics) TMMemoryPoolFree(graphicsComponenMem, (void *)entity->graphics);
    if(entity->physics) TMMemoryPoolFree(physicsComponentMem, (void *)entity->physics);
    if(entity->input) TMMemoryPoolFree(inputComponentMem, (void *)entity->input);
    TMMemoryPoolFree(entityMem, (void *)entity);
    memset(entity, 0, sizeof(Entity));
}

