
#ifndef _GAME_H_
#define _GAME_H_

#include <tm_renderer.h>

struct GameState {
    TMRenderer *renderer;
    
    TMShader *shader;
    TMShader *cloneShader;
    TMShader *batchShader;
    TMShader *instShader;

    TMBuffer *buffer;
    TMBuffer *cubeBuffer;
    TMBuffer *cloneBuffer;
    
    TMTexture *texture;
    TMTexture *cubeTexture;
    TMTexture *cloneTexture;
    TMTexture *charactersTexture;
    
    TMShaderBuffer *shaderBuffer;
    
    TMRenderBatch *renderBatch;
    float *uvs;

    TMInstanceRenderer *instanceRenderer;
};

void GameInitialize(GameState *state, TMWindow *window);
void GameUpdate(GameState *state);
void GameRender(GameState *state);
void GameShutdown(GameState *state);

#endif
