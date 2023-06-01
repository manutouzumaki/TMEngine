#include "aabb_sys.h"
#include "../entity.h"
#include "../collision.h"

#include <utils/tm_darray.h>
#include <stdio.h>

void PlayerMoveEnemy(Entity *player, Entity *other) {
    printf("Player Hit Move Enemy\n");
    player->graphics->color = {1, 0.2, 0.2, 1}; 

}

void PlayerBullet(Entity *player, Entity *other) {
    printf("Player Hit Bullet\n");
    player->graphics->color = {1, 0.2, 0.2, 1}; 

}

static void UpdateAABB(AABBComponent *aabb, TMVec2 position) {
    float halfSizeX = (aabb->aabb.max.x - aabb->aabb.min.x)*0.5f;
    float halfSizeY = (aabb->aabb.max.y - aabb->aabb.min.y)*0.5f;
    aabb->aabb.min.x = position.x - halfSizeX;
    aabb->aabb.min.y = position.y - halfSizeY;
    aabb->aabb.max.x = position.x + halfSizeX;
    aabb->aabb.max.y = position.y + halfSizeY;
}

void AABBSystemUpdate(Entity *player, Entity **entities) {


    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->aabb && entity->graphics)
            UpdateAABB(entity->aabb, entity->graphics->position);
    }

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        
        Entity *other = entities[i];
        
        if(player != other && other->aabb) {
            if(TestAABBAABB(player->aabb->aabb, other->aabb->aabb)) {
                if(other->aabb->onHit) other->aabb->onHit(player, other);
            }
        }
            
    }


}
