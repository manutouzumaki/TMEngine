#include "entity.h"

#include <utils/tm_memory_pool.h>
#include <utils/tm_darray.h>
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
static TMMemoryPool *enemyMovementComponentMem;
static TMMemoryPool *enemyShotComponentMem;


void EntitySystemInitialize(int maxEntityCount) {
    gEntityCount = 0;
    entityMem = TMMemoryPoolCreate(sizeof(Entity), maxEntityCount);
    graphicsComponenMem = TMMemoryPoolCreate(sizeof(GraphicsComponent), maxEntityCount);
    physicsComponentMem = TMMemoryPoolCreate(sizeof(PhysicsComponent), maxEntityCount);
    inputComponentMem = TMMemoryPoolCreate(sizeof(InputComponent), maxEntityCount);
    collisionComponentMem = TMMemoryPoolCreate(sizeof(CollisionComponent), maxEntityCount);
    animationComponentMem = TMMemoryPoolCreate(sizeof(AnimationComponet), maxEntityCount);
    enemyMovementComponentMem = TMMemoryPoolCreate(sizeof(EnemyMovementComponent), maxEntityCount);
    enemyShotComponentMem = TMMemoryPoolCreate(sizeof(EnemyShotComponent), maxEntityCount);
}

void EntitySystemShutdown() {

    TMMemoryPoolDestroy(entityMem);
    TMMemoryPoolDestroy(graphicsComponenMem);
    TMMemoryPoolDestroy(physicsComponentMem);
    TMMemoryPoolDestroy(inputComponentMem);
    TMMemoryPoolDestroy(collisionComponentMem);
    TMMemoryPoolDestroy(animationComponentMem);
    TMMemoryPoolDestroy(enemyMovementComponentMem);
    TMMemoryPoolDestroy(enemyShotComponentMem);
}

Entity *EntityCreate() {
    Entity *entity = (Entity *)TMMemoryPoolAlloc(entityMem);
    memset(entity, 0, sizeof(Entity));
    entity->id = gEntityCount++;
    return entity;
}

void EntityAddGraphicsComponent(Entity *entity, TMVec2 position, TMVec2 size, TMVec4 color,
                                TMVec4 absUVs, TMVec4 relUVs, int zIndex, TMShader *shader, TMTexture *texture) {

    assert(entity->graphics == NULL);
    entity->graphics = (GraphicsComponent *)TMMemoryPoolAlloc(graphicsComponenMem);
    memset(entity->graphics, 0, sizeof(GraphicsComponent));

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

void EntityAddEnemyMovementComponent(Entity *entity,
                                     CollisionComponent *collision, PhysicsComponent *physics) {
    assert(entity->enemyMovement == NULL);
    entity->enemyMovement = (EnemyMovementComponent *)TMMemoryPoolAlloc(enemyMovementComponentMem);

    float width = 0;

    switch(collision->type) {
        case COLLISION_TYPE_AABB: {
            width = collision->aabb.max.x - collision->aabb.min.x;
        } break;
        case COLLISION_TYPE_CAPSULE: {
            width = collision->capsule.r*2.0f;
        } break;
        case COLLISION_TYPE_CIRCLE: {
            width = collision->circle.r*2.0f;
        } break;

    }
    entity->enemyMovement->facingLeft = false;
    entity->enemyMovement->downLeft  = physics->down;
    entity->enemyMovement->downLeft.o.x -= width * 0.5f;

    entity->enemyMovement->downRight = physics->down;
    entity->enemyMovement->downRight.o.x += width * 0.5f;

    entity->enemyMovement->left  = {physics->down.o, {(-width*0.5f)-0.08f, 0}}; 
    entity->enemyMovement->right = {physics->down.o, { (width*0.5f)+0.08f, 0}};
}


void EntityAddEnemyShotComponent(Entity ***entities, Entity *entity, GraphicsComponent *graphics, TMShader *shader, TMTexture *texture) {

    assert(entity->enemyShot == NULL);

    entity->enemyShot = (EnemyShotComponent *)TMMemoryPoolAlloc(enemyShotComponentMem);

    Entity *bullet = EntityCreate();
    EntityAddGraphicsComponent(bullet, graphics->position, {0.2, 0.2}, {1, 1, 0, 1}, {}, {}, 3, shader, texture);
    
    float halfSizeX = bullet->graphics->size.x * 0.5f;
    float halfSizeY = bullet->graphics->size.y * 0.5f;
    AABB aabb;
    aabb.min.x = bullet->graphics->position.x - halfSizeX;
    aabb.min.y = bullet->graphics->position.y - halfSizeY;
    aabb.max.x = bullet->graphics->position.x + halfSizeX;
    aabb.max.y = bullet->graphics->position.y + halfSizeY;

    EntityAddCollisionComponent(bullet, COLLISION_TYPE_AABB, aabb, false);

    entity->enemyShot->facingLeft = true;
    entity->enemyShot->range = 6;
    entity->enemyShot->speed = 3;
    entity->enemyShot->bullet = bullet;

    TMDarrayPush(*entities, bullet, Entity *);

}

void EntityDestroy(Entity *entity) {
    if(entity->graphics) TMMemoryPoolFree(graphicsComponenMem, (void *)entity->graphics);
    if(entity->physics) TMMemoryPoolFree(physicsComponentMem, (void *)entity->physics);
    if(entity->input) TMMemoryPoolFree(inputComponentMem, (void *)entity->input);
    if(entity->collision) TMMemoryPoolFree(collisionComponentMem, (void *)entity->collision);
    if(entity->animation) TMMemoryPoolFree(animationComponentMem, (void *)entity->animation);
    if(entity->enemyMovement) TMMemoryPoolFree(enemyMovementComponentMem, (void *)entity->enemyMovement);
    if(entity->enemyShot) TMMemoryPoolFree(enemyShotComponentMem, (void *)entity->enemyShot);

    TMMemoryPoolFree(entityMem, (void *)entity);
    memset(entity, 0, sizeof(Entity));
}

