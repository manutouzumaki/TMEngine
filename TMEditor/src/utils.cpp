#include "utils.h"
#include "editor.h"
#include <tm_window.h>
#include <utils/tm_darray.h>

int MinI32(int a, int b) {
    if(a < b) return a;
    return b;
}

int MaxI32(int a, int b) {
    if(a > b) return a;
    return b;
}

int StringLength(char *string) {
    int counter = 0;
    char *letter = string; 
    while(*letter++ != L'\0') counter++;
    return counter;
}

void InsertionSortEntities(Entity *entities, int length)
{
    for(int j = 1; j < length; ++j)
    {
        Entity key = entities[j];
        int i = j - 1;
        while(i > -1 && entities[i].zIndex < key.zIndex)
        {
            entities[i + 1] = entities[i];
            --i;
        }
        entities[i + 1] = key;
    }
}

void LoadFileNamesFromDirectory(char *path, char ***filesNames) {
    TMGetFileNamesInDirectory(path, filesNames);
    if(*filesNames) {
        for (int i = 0; i < TMDarraySize(*filesNames); ++i) {
            printf("%s\n", (*filesNames)[i]);
        }
    }
}

void FreeFileNames(char ***filesNames) {
    if(*filesNames) {
        for (int i = 0; i < TMDarraySize(*filesNames); ++i) {
            if((*filesNames)[i]) {
                printf("deleted %s\n", (*filesNames)[i]);
                free((void *)(*filesNames)[i]);
            }
        }
        TMDarrayDestroy(*filesNames);
        *filesNames  = NULL;
    }
}
