
#ifndef _GAME_H_
#define _GAME_H_

#include <tm_renderer.h>

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

struct Entity {
    const char *name;
    TMVec2 position;
    TMVec2 velocity;
    TMVec2 size;
    TMTexture *texture;
};

struct GameState {
    TMRenderer *renderer;
    TMShader *shader;
    TMShader *gltfShader;

    TMBuffer *buffer;
    TMBuffer *cubeBuffer;
    TMBuffer *gltfBuffer;

    TMTexture *donutTexture;
    TMTexture *backgroundTexture;
    TMTexture *paddle1Texture;
    TMTexture *paddle2Texture;
    TMTexture *moonTexture;
    TMTexture *cloneTexture;

    TMMat4 perspective;
    TMMat4 orthographic;
    TMMat4 view;

    TMVec2 player1Position;
    TMVec2 player1Size;
    TMVec2 player2Position;
    TMVec2 player2Size;
    TMVec2 ballPosition;
    TMVec2 ballVelocity;
    TMVec2 ballSize;

    Entity *entities;
    unsigned int entityCount;

};

void GameInitialize(GameState *state);
void GameUpdate(GameState *state);
void GameRender(GameState *state);
void GameShutdown(GameState *state);

#endif
