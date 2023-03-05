#ifndef _TM_FILE_
#define _TM_FILE_

#include <stddef.h>

struct TMFile {
    void *data;
    size_t size;
};

TMFile TMFileOpen(const char *filepath);
void TMFileClose(TMFile *file);

#endif