#ifdef TM_MACOS
#define TM_EXPORT __attribute__((visibility("default")))
#elif TM_WIN32
#define TM_EXPORT __declspec(dllexport)
#endif

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

#define TM_RENDERER_MEMORY_BLOCK_SIZE 100
