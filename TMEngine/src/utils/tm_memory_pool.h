#ifndef _TM_MEMORY_POOL_
#define _TM_MEMORY_POOL_

#define HEADER_SIZE sizeof(unsigned char *)

struct TMMemoryPool {
    unsigned char **blockArray;
    unsigned char *head;
    unsigned int chunkSize;
    unsigned int numChunk;
    unsigned int blockCount;
};

TMMemoryPool *TMMemoryPoolCreate(unsigned int chunkSize, unsigned int numChunk);
void TMMemoryPoolDestroy(TMMemoryPool *memoryPool);
void *TMMemoryPoolAlloc(TMMemoryPool *memoryPool);
void TMMemoryPoolFree(TMMemoryPool *memoryPool, void *mem);

#endif