#ifndef _GAME_H_
#define _GAME_H_

#include <tm_renderer.h>
#include <utils/tm_math.h>
#include <utils/tm_hashmap.h>

#include "entity.h"
#include "collision.h"

struct GameState {
    
    TMRenderer *renderer;

    TMShader       *colorShader;
    TMShader       *spriteShader;

    TMMat4 view;
    TMMat4 proj;

    Entity **entities;
    Entity *player;

    // TODO: this should be a hashmap or somthing like that
    //TMVec4 *absUVs;
    //TMHashmap *absUVs;
    //float  *relUVs;
    //int relUVsCount;
};

void GameInitialize(GameState *state, TMWindow *window);
void GameUpdate(GameState *state, float dt);
void GameFixUpdate(GameState *state, float dt);
void GamePostUpdate(GameState *state, float t); // NOTE: this is use for position interpolation before rendering
void GameRender(GameState *state);
void GameShutdown(GameState *state);

#endif
