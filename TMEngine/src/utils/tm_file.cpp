#include "tm_file.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

extern int errno;

TMFile TMFileOpen(const char *filepath) {
    TMFile result{};
    FILE *file = fopen(filepath, "rb");
    if(!file) {
        int errorCode = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error opening file: %s\n", strerror( errorCode ));
        printf("Error loading TMFile: %s\n", filepath);
        assert(!"Error loading TMFile");
        return result;
    }
    // go to the end of the file
    fseek(file, 0, SEEK_END);
    // get the size of the file to alloc the memory we need
    long int fileSize = ftell(file);
    // go back to the start of the file
    fseek(file, 0, SEEK_SET);
    // alloc the memory
    result.data = malloc(fileSize + 1);
    memset(result.data, 0, fileSize + 1);
    result.size = fileSize;
    // store the content of the file
    fread(result.data, fileSize, 1, file);
    char *buffer = (char *)result.data;
    buffer[fileSize] = '\0'; // null terminating string...
    fclose(file);
    return result;
}

void TMFileClose(TMFile *file) {
    if(file->data) free(file->data);
    file->data = NULL;
    file->size = 0;
}

bool TMFileWriteText(const char *filepath, void *data, size_t size) {
    FILE *file = fopen(filepath, "w");
    if(!file) {
        printf("Error writing TMFile %s\n", filepath);
        return false;
    }
    size_t itemsWriten = fwrite(data, size, 1, file);
    assert(itemsWriten >= 1);
    fclose(file);
    return true;
}
