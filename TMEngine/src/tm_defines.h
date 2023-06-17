#ifndef _TM_DEFINE_H_
#define _TM_DEFINE_H_

#ifdef TM_MACOS
#define TM_EXPORT __attribute__((visibility("default")))
#elif TM_WIN32
#define TM_EXPORT __declspec(dllexport)
#endif

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

#define TM_RENDERER_MEMORY_BLOCK_SIZE 100
#define TM_PI 3.14159265359


#endif

