#include "player_sys.h"
#include "../entity.h"
#include <stdio.h>
#include <math.h>

static float MinF32(float a, float b) {
    if(a < b) return a;
    return b;
}

static float MaxF32(float a, float b) {
    if(a > b) return a;
    return b;
}

static int MinI32(int a, int b) {
    if(a < b) return a;
    return b;
}

static int MaxI32(int a, int b) {
    if(a > b) return a;
    return b;
}


void PlayerWin(Entity *entity, Entity *other) {
    printf("you win mother fucker!!!\n");

}

void PlayerGameOver(Entity *entity, Entity *other) {
    printf("you lost mother fucker!!!\n");

}

void PlayerHitEnemy(Entity *entity, Entity *other) {
    
    PlayerComponent  *player =  entity->player;
    PhysicsComponent *physics =  entity->physics;

    if(player->invencibleTimer <= 0.0f) {
        player->invencibleTimer = player->invencibleMaxTime;
        player->life = MaxI32(player->life - 1, 0);
        physics->velocity.x = 0;
        physics->acceleration.x = 0;
    }

}


void PlayerComponentUpdate(Entity *entity, float dt) {

    PlayerComponent *player =  entity->player;
    GraphicsComponent *graphics = entity->graphics;

    player->invencibleTimer = MaxF32(player->invencibleTimer - dt, 0.0f);

    graphics->color = {1, 1, 1, 1};
    if(player->invencibleTimer > 0.0f) {
        float animation = (sinf(player->invencibleTimer * 20.0f) + 1.0f) * 0.5f; 
        graphics->color = {1.0f, animation, animation, 1.0f};
    }

}

bool PlayerWin(Entity *entity) {
    PlayerComponent *player =  entity->player;
    return player->winCondition;
}

bool PlayerAlive(Entity *entity) {
    PlayerComponent *player =  entity->player;
    return (bool)player->life;
}
