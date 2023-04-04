
#include "game.h"
#include "models.h"

#include <tm_input.h>
#include <utils/tm_darray.h>
#include <utils/tm_json.h>
#include <tm_debug_renderer.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <math.h>

struct Matrices {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
};

struct WorldColorInstance {
    TMMat4 world;
    TMVec4 color;
    TMVec4 uvs;
};

static float StringToFloat(const char *c, size_t size) {
    assert(size < 32);
    static char buffer[32];
    memcpy((void *)buffer, (void *)c, size);
    buffer[size] = '\0';
    return (float)atof(buffer);
}

static unsigned int StringToInt(const char *c, size_t size) {
    assert(size < 32);
    static char buffer[32];
    memcpy((void *)buffer, (void *)c, size);
    buffer[size] = '\0';
    return (int)atoi(buffer);
}

static unsigned int StringToUnsignedInt(const char *c, size_t size) {
    assert(size < 32);
    static char buffer[32];
    memcpy((void *)buffer, (void *)c, size);
    buffer[size] = '\0';
    return (unsigned int)atoi(buffer);
}

static const char *StringToNullTerString(const char *c, size_t size) {
    char *nullTerString = (char *)malloc(size + 1);
    memcpy((void *)nullTerString, (void *)c, size);
    nullTerString[size] = '\0';
    return (const char *)nullTerString;
}

void GetAttributeAtIndex(TMJsonObject *root, unsigned int i, void **data, size_t *dataSize, void *srcData) {
    TMJsonObject *accessors   = TMJsonFindChildByName(root, "accessors");
    TMJsonObject *bufferViews = TMJsonFindChildByName(root, "bufferViews");
    // TMJsonObject *buffers     = TMJsonFindChildByName(root, "buffers");

    TMJsonObject *accessor = accessors->objects + i;

    // manuel: get the accesor information
    TMJsonObject *bufferViewIndex = TMJsonFindChildByName(accessor, "bufferView");
    TMJsonObject *componentType   = TMJsonFindChildByName(accessor, "componentType");
    TMJsonObject *count           = TMJsonFindChildByName(accessor, "count");
    TMJsonObject *type            = TMJsonFindChildByName(accessor, "type");

    // get the buffer for this assesor
    unsigned int index = StringToUnsignedInt(bufferViewIndex->values[0].value, bufferViewIndex->values[0].size);

    TMJsonObject *bufferView = &bufferViews->objects[index];
    
    TMJsonObject *bufferIndexJson = TMJsonFindChildByName(bufferView, "buffer");
    TMJsonObject *byteLengthJson  = TMJsonFindChildByName(bufferView, "byteLength");
    TMJsonObject *byteOffsetJson  = TMJsonFindChildByName(bufferView, "byteOffset");

    unsigned int bufferIndex = StringToUnsignedInt(bufferIndexJson->values[0].value, bufferIndexJson->values[0].size);
    unsigned int byteLength  = StringToUnsignedInt(byteLengthJson->values[0].value, byteLengthJson->values[0].size);
    unsigned int byteOffset  = StringToUnsignedInt(byteOffsetJson->values[0].value, byteOffsetJson->values[0].size);

    char *buffer = (char *)srcData;
    char *dataBuffer = buffer + byteOffset;
     
    *data = malloc(byteLength);
    memcpy(*data, dataBuffer, byteLength);
    *dataSize = byteLength;
}

void GameInitialize(GameState *state, TMWindow *window) {
    state->renderer = TMRendererCreate(window);


#ifdef TM_MACOS
    state->shader = TMRendererShaderCreate(state->renderer,
                                           "../../assets/shaders/vert.glsl",
                                           "../../assets/shaders/frag.glsl");
    state->cloneShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/gltfVert.glsl",
                                                "../../assets/shaders/gltfFrag.glsl");
    state->batchShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/batchVert.glsl",
                                                "../../assets/shaders/batchFrag.glsl");
    state->instShader = TMRendererShaderCreate(state->renderer,
                                               "../../assets/shaders/instVert.glsl",
                                               "../../assets/shaders/instFrag.glsl");
#elif TM_WIN32
    state->shader = TMRendererShaderCreate(state->renderer,
                                           "../../assets/shaders/vert.hlsl",
                                           "../../assets/shaders/frag.hlsl");
    state->cloneShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/gltfVert.hlsl",
                                                "../../assets/shaders/gltfFrag.hlsl");
    state->batchShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/batchVert.hlsl",
                                                "../../assets/shaders/batchFrag.hlsl");
    state->instShader = TMRendererShaderCreate(state->renderer,
                                               "../../assets/shaders/instVert.hlsl",
                                               "../../assets/shaders/instFrag.hlsl");
#endif

    state->texture = TMRendererTextureCreate(state->renderer,
                                             "../../assets/images/back.png");
    state->cubeTexture = TMRendererTextureCreate(state->renderer,
                                                 "../../assets/images/moon.png");
    state->cloneTexture = TMRendererTextureCreate(state->renderer,
                                                  "../../assets/images/clone.png");
    state->charactersTexture = TMRendererTextureCreate(state->renderer,
                                                       "../../assets/images/characters_packed.png");

    state->buffer = TMRendererBufferCreate(state->renderer,
                                           vertices, ARRAY_LENGTH(vertices),
                                           indices, ARRAY_LENGTH(indices),
                                           state->shader);

    state->cubeBuffer = TMRendererBufferCreate(state->renderer,
                                               cubeVertices, ARRAY_LENGTH(cubeVertices),
                                               state->shader);


    TMJson *clone = TMJsonOpen("../../assets/gltf/clone/clone.gltf");
    TMFile cloneData = TMFileOpen("../../assets/gltf/clone/clone.bin");
    TMJsonObject *root = &clone->root;
    TMJsonObject *primitivesJson = TMJsonFindChildByName(root, "primitives");
    TMJsonObject *posIndexJson = TMJsonFindChildByName(primitivesJson, "POSITION");
    TMJsonObject *norIndexJson = TMJsonFindChildByName(primitivesJson, "NORMAL");
    TMJsonObject *uvsIndexJson = TMJsonFindChildByName(primitivesJson, "TEXCOORD_0");
    TMJsonObject *indIndexJson = TMJsonFindChildByName(primitivesJson, "indices");
    unsigned int posIndex = StringToUnsignedInt(posIndexJson->values[0].value, posIndexJson->values[0].size);
    unsigned int norIndex = StringToUnsignedInt(norIndexJson->values[0].value, norIndexJson->values[0].size);
    unsigned int uvsIndex = StringToUnsignedInt(uvsIndexJson->values[0].value, uvsIndexJson->values[0].size);
    unsigned int indIndex = StringToUnsignedInt(indIndexJson->values[0].value, indIndexJson->values[0].size);
    
    float *pos = NULL;
    size_t posLength = 0;
    float *uvs = NULL;
    size_t uvsLength = 0;
    float *nor = NULL;
    size_t norLength = 0; 
    unsigned short *ind = NULL;
    size_t indLength = 0;

    GetAttributeAtIndex(root, posIndex, (void **)&pos, &posLength, cloneData.data);
    GetAttributeAtIndex(root, uvsIndex, (void **)&uvs, &uvsLength, cloneData.data);
    GetAttributeAtIndex(root, norIndex, (void **)&nor, &norLength, cloneData.data);
    GetAttributeAtIndex(root, indIndex, (void **)&ind, &indLength, cloneData.data);
    
    state->cloneBuffer = TMRendererBufferCreate(state->renderer,
                                                pos, posLength/sizeof(TMVec3),
                                                uvs, uvsLength/sizeof(TMVec2),
                                                nor, norLength/sizeof(TMVec3),
                                                ind, indLength/sizeof(unsigned short),
                                                state->cloneShader);

    free(pos);
    free(uvs);
    free(nor);
    free(ind);

    TMFileClose(&cloneData);
    TMJsonClose(clone);

    TMRendererFaceCulling(state->renderer, false, TM_CULL_BACK);
   
    Matrices mats{};
    state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &mats, sizeof(Matrices), 0);


    state->renderBatch = TMRendererRenderBatchCreate(state->renderer, state->batchShader, state->charactersTexture, 100);
    state->uvs = TMGenerateUVs(state->charactersTexture, 24, 24);

    state->instanceRenderer = TMRendererInstanceRendererCreate(state->renderer, state->instShader, 4, sizeof(WorldColorInstance));

    TMDebugRendererInitialize(state->renderer, 200);


}

void GameUpdate(GameState *state, float dt) {

}

void GameFixUpdate(GameState *state, float dt) {

}

void GamePostUpdate(GameState *state, float t) {
}

void GameRender(GameState *state) {

    TMRendererClear(state->renderer, 1.0f, 0.0f, 0.0f, 1.0f, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);
    
    float width = (float)TMRendererGetWidth(state->renderer);
    float height = (float)TMRendererGetHeight(state->renderer);
    static float angle = 0.0f;
    angle += 0.02f;

    // TODO: improve the shaderBuffer upgrate API ...

    // Render Background
    TMRendererDepthTestDisable(state->renderer);
    
    TMRendererBindShader(state->renderer, state->shader);
    TMRendererTextureBind(state->renderer, state->texture, state->shader, "back", 0);
    TMVec3 pos = {0, 0, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    Matrices mats{};
    mats.view = TMMat4LookAt(pos, pos + tar, up);
    mats.proj = TMMat4Ortho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, 0.0f, 100.0f);
    mats.world = TMMat4Scale(width, height, 1.0f); 
    TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);
    TMRendererDrawBufferElements(state->renderer, state->buffer);

    // 3D Rendering Here !!!
    TMRendererBindShader(state->renderer, state->cloneShader);
    TMRendererDepthTestEnable(state->renderer);

    // Render Clone
    TMRendererTextureBind(state->renderer, state->cloneTexture, state->cloneShader, "moon", 0);
    mats.proj = TMMat4Perspective(60.0f, width/height, 0.01f, 100.0f);
    mats.world = TMMat4Translate(0.0f, -2.0f, 5.0f) *TMMat4RotateY(angle) * TMMat4Scale(1.0f, 1.0f, 1.0f); 
    TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);
    TMRendererDrawBufferElements(state->renderer, state->cloneBuffer);

    TMRendererDepthTestDisable(state->renderer);

    // batch rendering test
    mats.world = TMMat4Identity(); 
    mats.proj = TMMat4Ortho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, 0.0f, 100.0f);
    TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);
    TMRendererRenderBatchAdd(state->renderBatch, 0, sinf(angle)*100.0f, 1, 100, 100, 0, 13, state->uvs);
    TMRendererRenderBatchAdd(state->renderBatch, 200, 0, 1, 100, 100, 0, 24, state->uvs);
    
    TMRendererRenderBatchDraw(state->renderBatch);

    // instance rendering test
    WorldColorInstance instBuffer[4] = {};
    instBuffer[0].world = TMMat4Translate(sinf(angle)*300, 0, 0) * TMMat4Scale(100, 50, 1);
    instBuffer[0].color = {1, 0.8, 0.8, 1};
    instBuffer[0].uvs = {state->uvs[0], state->uvs[1], state->uvs[2], state->uvs[3]};
    instBuffer[1].world = TMMat4Translate(-200, 200, 0) * TMMat4Scale(200, 200, 1);
    instBuffer[1].color = {1, 1, 1, 1};
    instBuffer[1].uvs = {state->uvs[0+4*2], state->uvs[1+4*2], state->uvs[2+4*2], state->uvs[3+4*2]};
    instBuffer[2].world = TMMat4Translate(0, -200, 0) * TMMat4Scale(100, 50, 1);
    instBuffer[2].color = {1, 1, 1, 1};
    instBuffer[2].uvs = {state->uvs[0+4*4], state->uvs[1+4*4], state->uvs[2+4*4], state->uvs[3+4*4]};
    instBuffer[3].world = TMMat4Translate(width*0.5f, 0, 0) * TMMat4Scale(100, 300, 1);
    instBuffer[3].color = {1, 1, 1, 1};
    instBuffer[3].uvs = {state->uvs[0+4*6], state->uvs[1+4*6], state->uvs[2+4*6], state->uvs[3+4*6]};

    TMRendererBindShader(state->renderer, state->instShader);
    TMRendererInstanceRendererDraw(state->renderer, state->instanceRenderer, instBuffer);

    TMDebugRendererDrawQuad(0, 0, 200, 200, 0, 0xFF00FF00);
    TMDebugRendererDrawQuad(300, 0, 200, 200, angle, 0xFFFF00FF);
    TMDebugRendererDrawCircle(0, 0, 100, 0xFFFF0000, 20);
    TMDebugRendererDrawCapsule(300, 0, 50, 50, angle, 0xFFFFFF00, 20);
    TMDebugRenderDraw();

    TMRendererPresent(state->renderer);
}

void GameShutdown(GameState *state) {
    TMDebugRendererShutdown();
    TMRendererInstanceRendererDestroy(state->renderer, state->instanceRenderer);
    if(state->uvs) free(state->uvs);
    TMRendererRenderBatchDestroy(state->renderer, state->renderBatch);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMRendererBufferDestroy(state->renderer, state->cloneBuffer);
    TMRendererBufferDestroy(state->renderer, state->cubeBuffer);
    TMRendererBufferDestroy(state->renderer, state->buffer);
    TMRendererTextureDestroy(state->renderer, state->charactersTexture);
    TMRendererTextureDestroy(state->renderer, state->cloneTexture);
    TMRendererTextureDestroy(state->renderer, state->cubeTexture);
    TMRendererTextureDestroy(state->renderer, state->texture);
    TMRendererShaderDestroy(state->renderer, state->instShader);
    TMRendererShaderDestroy(state->renderer, state->batchShader);
    TMRendererShaderDestroy(state->renderer, state->cloneShader);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);
}
