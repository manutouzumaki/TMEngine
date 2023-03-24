
#include "game.h"
#include "models.h"

#include <tm_input.h>
#include <utils/tm_darray.h>
#include <utils/tm_json.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

struct Matrices {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
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
    TMJsonObject *buffers     = TMJsonFindChildByName(root, "buffers");

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

    state->shader = TMRendererShaderCreate(state->renderer,
                                           "../../assets/shaders/vert.hlsl",
                                           "../../assets/shaders/frag.hlsl");
    state->cloneShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/gltfVert.hlsl",
                                                "../../assets/shaders/gltfFrag.hlsl");

    state->texture = TMRendererTextureCreate(state->renderer,
                                             "../../assets/images/back.png");
    state->cubeTexture = TMRendererTextureCreate(state->renderer,
                                                 "../../assets/images/moon.png");
    state->cloneTexture = TMRendererTextureCreate(state->renderer,
                                                  "../../assets/images/clone.png");

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
    state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &mats, sizeof(Matrices));
}

void GameUpdate(GameState *state) {

}

void GameRender(GameState *state) {
    TMRendererClear(state->renderer, 1.0f, 0.0f, 0.0f, 1.0f, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);
    
    float width = (float)TMRendererGetWidth(state->renderer);
    float height = (float)TMRendererGetHeight(state->renderer);

    // TODO: improve the shaderBuffer upgrate API ...

    TMRendererBindShader(state->renderer, state->shader);
    TMVec3 pos = {0, 0, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    Matrices mats{};
    mats.view = TMMat4LookAt(pos, pos + tar, up);
    
    // Render Background
    TMRendererTextureBind(state->renderer, state->texture, state->shader, "back", 0);
    mats.proj = TMMat4Ortho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, 0.0f, 100.0f);
    mats.world = TMMat4Scale(width, height, 1.0f); 
    TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);
    TMRendererDrawBufferElements(state->renderer, state->buffer);

   
    // 3D Rendering Here !!!
    TMRendererBindShader(state->renderer, state->cloneShader);
    TMRendererDepthTestEnable(state->renderer);

    // Render Clone
    static float angle = 0.0f;
    TMRendererTextureBind(state->renderer, state->cloneTexture, state->cloneShader, "moon", 0);
    mats.proj = TMMat4Perspective(60.0f, width/height, 0.01f, 100.0f);
    mats.world = TMMat4Translate(0.0f, -2.0f, 5.0f) * TMMat4RotateY(angle) * TMMat4Scale(1.0f, 1.0f, 1.0f); 
    TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &mats);
    TMRendererDrawBufferElements(state->renderer, state->cloneBuffer);
    angle += 0.02f;

    TMRendererDepthTestDisable(state->renderer);


    TMRendererPresent(state->renderer);
}

void GameShutdown(GameState *state) {
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMRendererBufferDestroy(state->renderer, state->cloneBuffer);
    TMRendererBufferDestroy(state->renderer, state->cubeBuffer);
    TMRendererBufferDestroy(state->renderer, state->buffer);
    TMRendererTextureDestroy(state->renderer, state->cloneTexture);
    TMRendererTextureDestroy(state->renderer, state->cubeTexture);
    TMRendererTextureDestroy(state->renderer, state->texture);
    TMRendererShaderDestroy(state->renderer, state->cloneShader);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);
}
