#ifndef _AABB_SYS_H_
#define _AABB_SYS_H_

struct Entity;

void PlayerMoveEnemy(Entity *entity, Entity *other);
void PlayerBullet(Entity *entity, Entity *other);


void AABBSystemUpdate(Entity *player, Entity **entities);

#endif
