#include <tm_window.h>
#include "game.h"
#include <stdio.h>

int main() {
    TMWindow *window = TMWindowCreate(800, 600, "TM Engine");
    GameState gameState{};

    GameInitialize(&gameState, window);

    while(!TMWindowShouldClose(window)) {
        TMWindowFlushEventQueue(window);
        
        GameUpdate(&gameState);
        GameRender(&gameState);

        TMWindowPresent(window);
    } 

    GameShutdown(&gameState);
    TMWindowDestroy(window);
    printf("shuting down!!!\n");

    return 0;
}
