#include "entity.h"

#include <utils/tm_memory_pool.h>
#include <memory.h>
#include <assert.h>


// TODO: create and entity manager for all this shit!!!
// TODO: change this for slotmaps for linear processing
static unsigned int gEntityCount;
static TMMemoryPool *entityMem;
static TMMemoryPool *graphicsComponenMem;
static TMMemoryPool *physicsComponentMem;
static TMMemoryPool *inputComponentMem;
static TMMemoryPool *collisionComponentMem;
static TMMemoryPool *animationComponentMem;


void EntitySystemInitialize(int maxEntityCount) {
    gEntityCount = 0;
    entityMem = TMMemoryPoolCreate(sizeof(Entity), maxEntityCount);
    graphicsComponenMem = TMMemoryPoolCreate(sizeof(GraphicsComponent), maxEntityCount);
    physicsComponentMem = TMMemoryPoolCreate(sizeof(PhysicsComponent), maxEntityCount);
    inputComponentMem = TMMemoryPoolCreate(sizeof(InputComponent), maxEntityCount);
    collisionComponentMem = TMMemoryPoolCreate(sizeof(CollisionComponent), maxEntityCount);
    animationComponentMem = TMMemoryPoolCreate(sizeof(AnimationComponet), maxEntityCount);
}

void EntitySystemShutdown() {
    TMMemoryPoolDestroy(entityMem);
    TMMemoryPoolDestroy(graphicsComponenMem);
    TMMemoryPoolDestroy(physicsComponentMem);
    TMMemoryPoolDestroy(inputComponentMem);
    TMMemoryPoolDestroy(collisionComponentMem);
    TMMemoryPoolDestroy(animationComponentMem);

}

Entity *EntityCreate() {
    Entity *entity = (Entity *)TMMemoryPoolAlloc(entityMem);
    memset(entity, 0, sizeof(Entity));
    entity->id = gEntityCount++;
    return entity;
}

void EntityAddGraphicsComponent(Entity *entity, GraphicsComponentType type,
                                TMVec2 position, TMVec2 size, TMVec4 color,
                                TMVec4 absUVs, TMVec4 relUVs, int zIndex, TMShader *shader, TMTexture *texture) {

    assert(entity->graphics == NULL);
    entity->graphics = (GraphicsComponent *)TMMemoryPoolAlloc(graphicsComponenMem);
    memset(entity->graphics, 0, sizeof(GraphicsComponent));

    entity->graphics->type = type;
    entity->graphics->shader = shader;
    entity->graphics->relUVs = relUVs;
    entity->graphics->absUVs = absUVs;
    entity->graphics->color = color;
    entity->graphics->position = position;
    entity->graphics->size = size;
    entity->graphics->index = 0;
    entity->graphics->zIndex = zIndex;  // TODO: this probably should be a float
    entity->graphics->texture = texture;

}

void EntityAddGraphicsComponentSolidColor(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color, TMShader *shader) {
    assert(entity->graphics == NULL);
    entity->graphics = (GraphicsComponent *)TMMemoryPoolAlloc(graphicsComponenMem);
    memset(entity->graphics, 0, sizeof(GraphicsComponent));
    entity->graphics->type = GRAPHICS_TYPE_SOLID_COLOR;
    entity->graphics->position = position;
    entity->graphics->size = size;
    entity->graphics->color = color;
    entity->graphics->shader = shader;
    entity->graphics->texture = 0;
}

#if 0
void EntityAddGraphicsComponentSprite(Entity *entity, TMVec2 position, TMVec2 size, float *uvs, TMShader *shader) {
    assert(entity->graphics == NULL);
    entity->graphics = (GraphicsComponent *)TMMemoryPoolAlloc(graphicsComponenMem);
    memset(entity->graphics, 0, sizeof(GraphicsComponent));
    entity->graphics->type = GRAPHICS_TYPE_SPRITE;
    entity->graphics->position = position;
    entity->graphics->size = size;
    entity->graphics->absUVs = {0, 0, 1, 1};
    entity->graphics->relUVs = uvs;
    entity->graphics->shader = shader;
    entity->graphics->color = {1, 1, 1, 1};
}

void EntityAddGraphicsComponentSubSprite(Entity *entity, TMVec2 position, TMVec2 size,
                                         TMVec4 absUVs, int index, float *uvs, TMShader *shader) {
    assert(entity->graphics == NULL);
    entity->graphics = (GraphicsComponent *)TMMemoryPoolAlloc(graphicsComponenMem);
    memset(entity->graphics, 0, sizeof(GraphicsComponent));
    entity->graphics->type = GRAPHICS_TYPE_SUBSPRITE;
    entity->graphics->position = position;
    entity->graphics->size = size;
    entity->graphics->absUVs = absUVs;
    entity->graphics->relUVs = uvs;
    entity->graphics->index = index;
    entity->graphics->shader = shader;
    entity->graphics->color = {1, 1, 1, 1};
}
#endif

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

void EntityAddCollisionComponent(Entity *entity, CollisionType type, AABB aabb, bool solid) {
    assert(entity->collision == NULL);
    entity->collision = (CollisionComponent *)TMMemoryPoolAlloc(collisionComponentMem);
    entity->collision->type = type;
    entity->collision->aabb = aabb;
    entity->collision->solid = solid;
}

void EntityAddCollisionComponent(Entity *entity, CollisionType type, OBB obb, bool solid) {
    assert(entity->collision == NULL);
    entity->collision = (CollisionComponent *)TMMemoryPoolAlloc(collisionComponentMem);
    entity->collision->type = type;
    entity->collision->obb = obb;
    entity->collision->solid = solid;
}

void EntityAddCollisionComponent(Entity *entity, CollisionType type, Circle circle, bool solid) {
    assert(entity->collision == NULL);
    entity->collision = (CollisionComponent *)TMMemoryPoolAlloc(collisionComponentMem);
    entity->collision->type = type;
    entity->collision->circle = circle;
    entity->collision->solid = solid;
}

void EntityAddCollisionComponent(Entity *entity, CollisionType type, Capsule capsule, bool solid) {
    assert(entity->collision == NULL);
    entity->collision = (CollisionComponent *)TMMemoryPoolAlloc(collisionComponentMem);
    entity->collision->type = type;
    entity->collision->capsule = capsule;
    entity->collision->solid = solid;
}

void EntityAddAnimationComponet(Entity *entity) {
    assert(entity->animation == NULL);
    entity->animation = (AnimationComponet *)TMMemoryPoolAlloc(animationComponentMem);
    entity->animation->states = NULL;
    entity->animation->statesCount = 0;
    entity->animation->current = NULL;
    entity->animation->timer = 0.0f;
    
}

void EntityDestroy(Entity *entity) {
    if(entity->graphics) TMMemoryPoolFree(graphicsComponenMem, (void *)entity->graphics);
    if(entity->physics) TMMemoryPoolFree(physicsComponentMem, (void *)entity->physics);
    if(entity->input) TMMemoryPoolFree(inputComponentMem, (void *)entity->input);
    if(entity->collision) TMMemoryPoolFree(collisionComponentMem, (void *)entity->collision);
    TMMemoryPoolFree(entityMem, (void *)entity);
    memset(entity, 0, sizeof(Entity));
}

