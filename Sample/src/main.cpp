#include <tm_window.h>
#include <tm_timer.h>
#include "game.h"
#include <stdio.h>
#include <math.h>

int main() {
    TMWindow *window = TMWindowCreate(800, 600, "Platformer Demo");
    GameState gameState{};

    TMTimer timer{};
    TMTimerStart(&timer);

    GameInitialize(&gameState, window);
    
    float accumulator = 0.0f;
    // NOTE: try to set this at 1.0f/120.0f to improve collision resolution
    float dt = 1.0f/60.0f;

    float targetFrameTime = 1.0f/60.0f;

    unsigned int framesCounter = 0;
    float time = 0.0f;

    unsigned long long lastTime = TMTimerGetCurrentTime(&timer);
    while(!TMWindowShouldClose(window)) {

        unsigned long long currentTime = TMTimerGetCurrentTime(&timer);
        float frameTime =  (float)(currentTime - lastTime)/(float)timer.frequency;
        while(frameTime < targetFrameTime) {
            float millisecondsToSleep = targetFrameTime - frameTime;
            TMSleep(millisecondsToSleep);
            currentTime = TMTimerGetCurrentTime(&timer);
            frameTime = (float)(currentTime - lastTime)/(float)timer.frequency;
        }

        float deltaTime = (float)(currentTime - lastTime)/(float)timer.frequency;
        lastTime = currentTime;
        
        TMWindowFlushEventQueue(window);

        // Update
        GameUpdate(&gameState, deltaTime);

        // Fix Update
        accumulator += deltaTime;
        int counter = 0;
        while(accumulator >= dt) {
            counter++;
            GameFixUpdate(&gameState, dt);
            accumulator -= dt;
        }

        float t = accumulator / dt;
        GamePostUpdate(&gameState, t); // NOTE: this is use for position interpolation before rendering

        GameRender(&gameState);

        TMWindowPresent(window);

        if(time >= 1.0f) {
            printf("FPS: %d\n", framesCounter);
            framesCounter = 0;
            time = 0.0f;
        }
        framesCounter++;
        time += deltaTime;
    }

    GameShutdown(&gameState);
    TMWindowDestroy(window);
    printf("shuting down!!!\n");

    return 0;
}
