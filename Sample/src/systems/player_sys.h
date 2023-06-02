#ifndef _PLAYER_SYS_H_
#define _PLAYER_SYS_H_

struct Entity;

void PlayerWin(Entity *entity, Entity *other);
void PlayerGameOver(Entity *entity, Entity *other);
void PlayerHitEnemy(Entity *entity, Entity *other);

void PlayerComponentUpdate(Entity *entity, float dt);
bool PlayerAlive(Entity *entity);


#endif
