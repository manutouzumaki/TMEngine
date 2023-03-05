#include "tm_memory_pool.h"
#include <stdlib.h>
#include <memory.h>

#define TM_EXPORT __attribute__((visibility("default")))

TM_EXPORT TMMemoryPool *TMMemoryPoolCreate(unsigned int chunkSize, unsigned int numChunk) {
    TMMemoryPool *memoryPool = (TMMemoryPool *)malloc(sizeof(TMMemoryPool));
    memset(memoryPool, 0, sizeof(TMMemoryPool));

    memoryPool->chunkSize = chunkSize;
    memoryPool->numChunk = numChunk;

    unsigned int blockArraySize = sizeof(unsigned char *) * (memoryPool->blockCount + 1);
    unsigned char **newBlockArray = (unsigned char **)malloc(blockArraySize);

    // alloc the memory block
    unsigned int trueChunkSize = memoryPool->chunkSize + HEADER_SIZE;
    unsigned int blockSize = trueChunkSize * memoryPool->numChunk;
    unsigned char *newBlock = (unsigned char *)malloc(blockSize);
    newBlockArray[memoryPool->blockCount++] = newBlock;

    // initialize the free list
    unsigned char *current = newBlock;
    unsigned char *end = newBlock + blockSize;
    while(current != end) {
        unsigned char *next = current + trueChunkSize;
        unsigned char **header = (unsigned char **)current;
        *header = (next < end) ? next : NULL;
        current = next;
    }
    memoryPool->head = newBlock;
    memoryPool->blockArray = newBlockArray;

    return memoryPool;
}

TM_EXPORT void TMMemoryPoolDestroy(TMMemoryPool *memoryPool) {
    for(int i = 0; i < memoryPool->blockCount; ++i) {
        free(memoryPool->blockArray[i]);
    }
    free(memoryPool->blockArray);
    free(memoryPool);
}

TM_EXPORT void *TMMemoryPoolAlloc(TMMemoryPool *memoryPool) {
    if(memoryPool->head) {
        unsigned char *chunk = memoryPool->head;
        unsigned char **next = (unsigned char **)chunk;
        memoryPool->head = *next;
        unsigned char *data = chunk + HEADER_SIZE;
        return (void *)data;        
    }

    // grow the blockArray
    unsigned int blockArraySize = sizeof(unsigned char *) * (memoryPool->blockCount + 1);
    unsigned char **newBlockArray = (unsigned char **)malloc(blockArraySize);

    // copy any existent block
    for(int i = 0; i < memoryPool->blockCount; ++i) {
        newBlockArray[i] = memoryPool->blockArray[i];
    }
    free(memoryPool->blockArray);

    // alloc the new memory block
    unsigned int trueChunkSize = memoryPool->chunkSize + HEADER_SIZE;
    unsigned int blockSize = trueChunkSize * memoryPool->numChunk;
    unsigned char *newBlock = (unsigned char *)malloc(blockSize);
    newBlockArray[memoryPool->blockCount++] = newBlock;

    // initialize the free list
    unsigned char *current = newBlock;
    unsigned char *end = newBlock + blockSize;
    while(current != end) {
        unsigned char *next = current + trueChunkSize;
        unsigned char **header = (unsigned char **)current;
        *header = (next < end) ? next : NULL;
        current = next;
    }
    memoryPool->head = newBlock;
    memoryPool->blockArray = newBlockArray;

    unsigned char *chunk = memoryPool->head;
    unsigned char **next = (unsigned char **)chunk;
    memoryPool->head = *next;
    unsigned char *data = chunk + HEADER_SIZE;
    return data;
}

TM_EXPORT void TMMemoryPoolFree(TMMemoryPool *memoryPool, void *mem) {
    unsigned char *chunk = ((unsigned char *)mem - HEADER_SIZE);
    unsigned char **next = (unsigned char **)chunk;
    *next = memoryPool->head;
    memoryPool->head = chunk;
}