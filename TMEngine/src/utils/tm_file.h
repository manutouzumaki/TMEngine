#ifndef _TM_FILE_
#define _TM_FILE_

#include "../tm_defines.h"
#include <stddef.h>

struct TMFile {
    void *data;
    size_t size;
};

TM_EXPORT TMFile TMFileOpen(const char *filepath);
TM_EXPORT void TMFileClose(TMFile *file);

// TODO: add write files, binary and text formats
TM_EXPORT bool TMFileWriteText(const char *filepath, void *data, size_t size);

#endif
