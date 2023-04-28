#ifndef _TM_HASHMAP_H_
#define _TM_HASHMAP_H_

#include "../tm_defines.h"
#include <stdlib.h>

struct HashElement {
    unsigned int id;
    void *data;
};

struct TMHashmap {
    unsigned int occupied;
    unsigned int capacity;
    unsigned int mask;
    HashElement *elements;
    size_t elementSize;
};

TM_EXPORT TMHashmap *TMHashmapCreate(size_t elementSize);
TM_EXPORT void TMHashmapDestroy(TMHashmap *hashmap);
TM_EXPORT void TMHashmapAdd(TMHashmap *hashmap, const char *string, void *data);
TM_EXPORT void *TMHashmapGet(TMHashmap *hashmap, const char *string);

#endif
