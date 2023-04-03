#include <tm_window.h>
#include <tm_timer.h>
#include "game.h"
#include <stdio.h>


// TODO: delta time

int main() {
    TMWindow *window = TMWindowCreate(800, 600, "TM Engine");
    GameState gameState{};

    TMTimer timer{};
    TMTimerStart(&timer);

    GameInitialize(&gameState, window);
    
    // TODO: add delta time and fix update functions ...
    // TODO: fix update ...
    
    float accumulator = 0.0;
    float dt = 1.0f/60.0f;

    double lastTime = TMTimerGetCurrentTime(&timer);
    while(!TMWindowShouldClose(window)) {
        
        double currentTime = TMTimerGetCurrentTime(&timer);
        float deltaTime = (float)(currentTime - lastTime);

        TMWindowFlushEventQueue(window);


        // Update
        GameUpdate(&gameState, deltaTime);
    
        // Fix Update
        accumulator += deltaTime;
        while(accumulator >= dt) {
            GameFixUpdate(&gameState, dt);
            accumulator -= dt;
        }
        float t = accumulator / dt;
        GamePostUpdate(&gameState, t); // NOTE: this is use for position interpolation before rendering

        GameRender(&gameState);

        TMWindowPresent(window);
        
        lastTime = currentTime;

    }

    GameShutdown(&gameState);
    TMWindowDestroy(window);
    printf("shuting down!!!\n");

    return 0;
}
