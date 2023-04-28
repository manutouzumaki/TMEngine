#include "tm_hashmap.h"

#include "..\tm_defines.h"

#include <memory.h>

#define SEED 123

static size_t StringLength(const char *string) {
    size_t counter = 0;
    char *byte = (char *)string;
    while(*byte++ != '\0') counter++;
    return counter;
}

static unsigned int MurMur2(const void *key, int len, unsigned int seed) {
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    unsigned int h = seed ^ len;
    const unsigned char *data = (const unsigned char *)key;
    while(len >= 4) {
        unsigned int k = *(unsigned int *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }
    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    }
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

static void HashmapGrow(TMHashmap *hashmap) {
    
    unsigned int oldCapacity = hashmap->capacity;
    hashmap->capacity *= 2;
    hashmap->mask = hashmap->capacity - 1;
    HashElement *newElements = (HashElement *)malloc(sizeof(HashElement) * hashmap->capacity);
    memset(newElements, 0, sizeof(HashElement) * hashmap->capacity);

    for(unsigned int i = 0; i < oldCapacity; ++i) {
        HashElement element = hashmap->elements[i];
        if(element.id != 0) {
            unsigned int hashIndex = (unsigned int)(element.id & hashmap->mask);
            if(newElements[hashIndex].id == 0) {
                newElements[hashIndex] = element;
            }
            else {
                
                unsigned int nextIndex = hashIndex + 1;
                while(hashmap->elements[nextIndex].id != 0 || nextIndex >= hashmap->capacity) {
                    nextIndex = (nextIndex + 1) % hashmap->capacity;
                }
                newElements[nextIndex] = element;

            }
        }
    }
    free(hashmap->elements);
    hashmap->elements = newElements;

}

TMHashmap *TMHashmapCreate(size_t elementSize) {
    TMHashmap *hashmap = (TMHashmap *)malloc(sizeof(TMHashmap));
    memset(hashmap, 0, sizeof(TMHashmap));
    hashmap->capacity = 2;
    hashmap->occupied = 0;
    hashmap->mask = hashmap->capacity - 1;
    hashmap->elements = (HashElement *)malloc(sizeof(HashElement) * hashmap->capacity);
    hashmap->elementSize = elementSize;
    memset(hashmap->elements, 0, sizeof(HashElement) * hashmap->capacity);
    return hashmap;
}

void TMHashmapDestroy(TMHashmap *hashmap) {

    for(int i = 0; i < hashmap->capacity; ++i) {
        HashElement *element = hashmap->elements + i;
        if(element->data) free(element->data);
    }
    if(hashmap->elements) free(hashmap->elements);
    if(hashmap) free(hashmap);

}

void TMHashmapAdd(TMHashmap *hashmap, const char *string, void *data) {

    unsigned int elementId = MurMur2(string, StringLength(string), SEED);
    unsigned int hashIndex = (unsigned int)(elementId & hashmap->mask);

    if(hashmap->elements[hashIndex].id == 0) {
        HashElement *element = hashmap->elements + hashIndex;
        element->id = elementId;
        element->data = malloc(hashmap->elementSize);
        memcpy(element->data, data, hashmap->elementSize);
        hashmap->occupied += 1;
    }
    else {
        unsigned int nextIndex = hashIndex + 1;
        while(hashmap->elements[nextIndex].id != 0 || nextIndex >= hashmap->capacity) {
            nextIndex = (nextIndex + 1) % hashmap->capacity;
        }
        HashElement *element = hashmap->elements + nextIndex;
        element->id = elementId;
        element->data = malloc(hashmap->elementSize);
        memcpy(element->data, data, hashmap->elementSize);
        hashmap->occupied += 1; 
    }

    if(((float)hashmap->occupied / (float)hashmap->capacity) >= 0.7f) {
        HashmapGrow(hashmap);
    }

}

void *TMHashmapGet(TMHashmap *hashmap, const char *string) {
    unsigned int elementId = MurMur2(string, StringLength(string), SEED);
    unsigned int hashIndex = (unsigned int)(elementId & hashmap->mask);
    unsigned int counter = 0;
    while(((hashmap->elements[hashIndex].id != 0 && hashmap->elements[hashIndex].id != elementId) ||
           hashmap->elements[hashIndex].id == 0) && counter < hashmap->capacity) {
        hashIndex = (hashIndex + 1) % hashmap->capacity;
        ++counter;
    }
    if(hashmap->elements[hashIndex].id != 0 && counter < hashmap->capacity) {
        return hashmap->elements[hashIndex].data;
    }
    return NULL; 
}
