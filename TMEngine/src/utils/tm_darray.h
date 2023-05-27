#ifndef _TM_DARRAY_H__
#define _TM_DARRAY_H__

#include "../tm_defines.h"

#define TMDarrayPush(array, value, type)                             \
    do {                                                             \
        (array) = (type *)TMDarrayCreate_((array), sizeof(type), 1); \
        (array)[TMDarraySize(array) - 1] = (value);                  \
    }while(0);

#define TMDarrayDestroy(array) TMDarrayDestroy_((void *)(array))
#define TMDarraySize(array) TMDarraySize_((void *)(array))
#define TMDarrayCapacity(array) TMDarrayCapacity_((void *)(array))

TM_EXPORT void *TMDarrayCreate_(void *array, unsigned int elementSize, unsigned int elementCount);
TM_EXPORT void TMDarrayDestroy_(void *array);
TM_EXPORT unsigned int TMDarraySize_(void *array);
TM_EXPORT unsigned int TMDarrayCapacity_(void *array);


TM_EXPORT void TMDarrayModifySize(void *array, unsigned int size);




#endif
