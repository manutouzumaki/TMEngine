#ifndef _TM_RENDERER_H_
#define _TM_RENDERER_H_

#include "tm_defines.h"
#include <stdlib.h>

#define TM_CULL_BACK (1 << 0)
#define TM_CULL_FRONT (1 << 1)

#define TM_COLOR_BUFFER_BIT   (1 << 0)
#define TM_DEPTH_BUFFER_BIT   (1 << 1)
#define TM_STENCIL_BUFFER_BIT (1 << 2)

#include "utils/tm_math.h"

struct TMWindow;
struct TMRenderer;
struct TMBuffer;
struct TMShader;
struct TMShaderBuffer;
struct TMTexture;
struct TMShaderStruct;
struct TMFramebuffer;
struct TMRenderBatch;
struct TMInstanceRenderer;

struct TMVertex {
    TMVec3 position;
    TMVec2 uv;
    TMVec3 normal;
};

struct TMBatchVertex {
    TMVec3 position;
    TMVec2 uvs;
    TMVec4 color;
};

TM_EXPORT TMRenderer *TMRendererCreate(TMWindow *window);
TM_EXPORT void TMRendererDestroy(TMRenderer *renderer);
TM_EXPORT void TMRendererDepthTestEnable(TMRenderer* renderer);
TM_EXPORT void TMRendererDepthTestDisable(TMRenderer* renderer);
TM_EXPORT void TMRendererFaceCulling(TMRenderer* renderer, bool value,  unsigned int flags);
TM_EXPORT int TMRendererGetWidth(TMRenderer *renderer);
TM_EXPORT int TMRendererGetHeight(TMRenderer *renderer);
TM_EXPORT bool TMRendererUpdateRenderArea(TMRenderer *renderer);
TM_EXPORT void TMRendererClear(TMRenderer* renderer, float r, float g, float b, float a, unsigned  int flags);
TM_EXPORT void TMRendererPresent(TMRenderer *renderer);


TM_EXPORT TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                 TMVertex *vertices, unsigned int verticesCount,
                                 TMShader *shader);
TM_EXPORT TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                 TMVertex *vertices, unsigned int verticesCount,
                                 unsigned int *indices, unsigned int indicesCount,
                                 TMShader *shader);
TM_EXPORT TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                 float *vertices, unsigned int verticesCount,
                                 float *uvs, unsigned int uvsCount,
                                 float *normals, unsigned int normalsCount,
                                 unsigned short *indices, unsigned int indicesCount,
                                 TMShader *shader);

TM_EXPORT void TMRendererBufferDestroy(TMRenderer *renderer, TMBuffer *buffer);
TM_EXPORT void TMRendererDrawBufferElements(TMRenderer* renderer, TMBuffer *buffer);
TM_EXPORT void TMRendererDrawBufferArray(TMRenderer* renderer, TMBuffer *buffer);

TM_EXPORT TMShader *TMRendererShaderCreate(TMRenderer *renderer, const char *vertPath, const char *fragPath);
TM_EXPORT TMShader *TMRendererShaderCreateFromString(TMRenderer *renderer, const char *vertSource, size_t vertSize,
                                                                           const char *fragSource, size_t fragSize);
TM_EXPORT void TMRendererShaderDestroy(TMRenderer *renderer, TMShader *shader);
TM_EXPORT void TMRendererBindShader(TMRenderer* renderer, TMShader *shader);

TM_EXPORT TMShaderBuffer* TMRendererShaderBufferCreate(TMRenderer* renderer, void *bufferData, size_t bufferSize, unsigned int index);
TM_EXPORT void TMRendererShaderBufferDestroy(TMRenderer* renderer, TMShaderBuffer* shaderBuffer);
TM_EXPORT void TMRendererShaderBufferUpdate(TMRenderer* renderer, TMShaderBuffer* shaderBuffer, void* bufferData);

TM_EXPORT TMTexture *TMRendererTextureCreate(TMRenderer *renderer, const char *filepath);
TM_EXPORT void TMRendererTextureDestroy(TMRenderer *renderer, TMTexture *texture);
TM_EXPORT void TMRendererTextureBind(TMRenderer* renderer, TMTexture *texture, TMShader *shader, const char *varName, int textureIndex);
TM_EXPORT void TMRendererTextureUnbind(TMRenderer* renderer, TMTexture *texture, int textureIndex);

TM_EXPORT TMFramebuffer *TMRendererFramebufferCreate(TMRenderer *renderer);
TM_EXPORT void TMRendererFramebufferDestroy(TMRenderer *renderer, TMFramebuffer *framebuffer);

TM_EXPORT TMRenderBatch *TMRendererRenderBatchCreate(TMRenderer *renderer, TMShader *shader, TMTexture *texture, size_t size);
TM_EXPORT void TMRendererRenderBatchAdd(TMRenderBatch *renderBatch, float x, float y, float z, float w, float h, float angle);
TM_EXPORT void TMRendererRenderBatchAdd(TMRenderBatch *renderBatch, float x, float y, float z,
                                                                    float w, float h, float angle,
                                                                    float r, float g, float b, float a);
TM_EXPORT void TMRendererRenderBatchAdd(TMRenderBatch *renderBatch, float x, float y, float z, float w, float h, float angle, int sprite, float *uvs);
TM_EXPORT void TMRendererRenderBatchDraw(TMRenderBatch *renderBatch);
TM_EXPORT void TMRendererRenderBatchDestroy(TMRenderer *renderer, TMRenderBatch *renderBatch);


TM_EXPORT TMInstanceRenderer *TMRendererInstanceRendererCreate(TMRenderer *renderer, TMShader *shader, unsigned int instCount, unsigned int instSize);
TM_EXPORT void TMRendererInstanceRendererDraw(TMRenderer *renderer, TMInstanceRenderer *instanceRenderer, void *buffer);
TM_EXPORT void TMRendererInstanceRendererDestroy(TMRenderer *renderer, TMInstanceRenderer *instanceRenderer);

// this uvs should be free calling free(uvs)
TM_EXPORT float *TMGenerateUVs(TMTexture *texture, int tileWidth, int tileHeight);




#endif
