#ifndef _GAME_H_
#define _GAME_H_

#include <tm_renderer.h>
#include <utils/tm_math.h>

struct Entity;

struct GameState {
    
    TMRenderer *renderer;

    TMShader       *colorShader;
    TMShader       *spriteShader;

    TMTexture     **levelTextures;

    TMMat4 view;
    TMMat4 proj;

    Entity **entities;
    Entity *player;

    Entity *enemy;

};

void GameInitialize(GameState *state, TMWindow *window);
void GameUpdate(GameState *state, float dt);
void GameFixUpdate(GameState *state, float dt);
void GamePostUpdate(GameState *state, float t); // NOTE: this is use for position interpolation before rendering
void GameRender(GameState *state);
void GameShutdown(GameState *state);

#endif
