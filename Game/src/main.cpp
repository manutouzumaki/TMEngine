#include <tm_window.h>
#include "game.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h> 

int main() {
    TMWindow *window = TMWindowCreate(1080/4, 1920/4, "TM Engine");
    GameState gameState{};

#ifdef TM_DEBUG
    // print the current directory
    char buffer[MAXPATHLEN];
    getcwd(buffer, MAXPATHLEN);
    printf("current working directory: %s\n", buffer);
#endif

    GameInitialize(&gameState);

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
