#include "tm_darray.h"
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#define TM_DARRAY_RAW_DATA(array) ((unsigned int *)(array) - 2)
#define TM_DARRAY_GET_CAPACITY(array) (TM_DARRAY_RAW_DATA(array)[0])
#define TM_DARRAY_GET_SIZE(array) (TM_DARRAY_RAW_DATA(array)[1])


void *TMDarrayCreate_(void *array, unsigned int elementSize, unsigned int elementCount) {
    if(array == NULL) {
        unsigned int rawSize = (sizeof(unsigned int) * 2) + (elementSize * elementCount);
        unsigned int *base = (unsigned int *)malloc(rawSize);
        base[0] = elementCount;
        base[1] = elementCount;
        return (void *)(base + 2);

    }
    else if((TM_DARRAY_GET_SIZE(array) + elementCount) <= TM_DARRAY_GET_CAPACITY(array)) {
        TM_DARRAY_GET_SIZE(array) += elementCount;
        return array;
    }
    else {
        unsigned int neededSize = TM_DARRAY_GET_SIZE(array) + elementCount;
        unsigned int newCapacity = TM_DARRAY_GET_CAPACITY(array) * 2;
        unsigned int capacity = neededSize > newCapacity ? neededSize : newCapacity;
        unsigned int size = neededSize;
        unsigned int rawSize = (sizeof(unsigned int) * 2) + (elementSize * capacity);
        unsigned int *base = (unsigned int *)realloc(TM_DARRAY_RAW_DATA(array), rawSize);
        base[0] = capacity;
        base[1] = size;
        return (void *)(base + 2);

    }
}

void TMDarrayDestroy_(void *array) {
    assert(array != NULL);
    void *rawData = TM_DARRAY_RAW_DATA(array);
    free(rawData);
}

unsigned int TMDarraySize_(void *array) {
    assert(array != NULL);
    return TM_DARRAY_GET_SIZE(array);
    
}

unsigned int TMDarrayCapacity_(void *array) {
    assert(array != NULL);
    return TM_DARRAY_GET_CAPACITY(array);
}
