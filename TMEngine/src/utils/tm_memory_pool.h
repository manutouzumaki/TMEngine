#ifndef _TM_MEMORY_POOL_
#define _TM_MEMORY_POOL_

#include "../tm_defines.h"

#define HEADER_SIZE sizeof(unsigned char *)

struct TMMemoryPool {
    unsigned char **blockArray;
    unsigned char *head;
    unsigned int chunkSize;
    unsigned int numChunk;
    unsigned int blockCount;
};

TM_EXPORT TMMemoryPool *TMMemoryPoolCreate(unsigned int chunkSize, unsigned int numChunk);
TM_EXPORT void TMMemoryPoolDestroy(TMMemoryPool *memoryPool);
TM_EXPORT void *TMMemoryPoolAlloc(TMMemoryPool *memoryPool);
TM_EXPORT void TMMemoryPoolFree(TMMemoryPool *memoryPool, void *mem);

#endif