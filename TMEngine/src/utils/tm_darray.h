#ifndef _TM_DARRAY_H__
#define _TM_DARRAY_H__

#define TMDarrayPush(array, value, type)                             \
    do {                                                             \
        (array) = (type *)TMDarrayCreate_((array), sizeof(type), 1); \
        (array)[TMDarraySize(array) - 1] = (value);                  \
    }while(0);

#define TMDarrayDestroy(array) TMDarrayDestroy_((void *)(array))
#define TMDarraySize(array) TMDarraySize_((void *)(array))
#define TMDarrayCapacity(array) TMDarrayCapacity_((void *)(array))

void *TMDarrayCreate_(void *array, unsigned int elementSize, unsigned int elementCount);
void TMDarrayDestroy_(void *array);
unsigned int TMDarraySize_(void *array);
unsigned int TMDarrayCapacity_(void *array);




#endif
