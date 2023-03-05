
#include "game.h"
#include <tm_input.h>
#include <utils/tm_darray.h>
#include <utils/tm_json.h>

#include <stdio.h>

#include "models.h"

#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string.h>

static void UpdateProjectionsMatrices(GameState *state) {
    // manuel: create the projection and view matrix
    int width = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);
    state->orthographic = TMMat4Ortho(-width/2, width/2, -height/2, height/2, 0, 100);
    state->perspective = TMMat4Perspective(60.0f, (float)width/(float)height, 0.01f, 100.0f);
}

static void UpdateViewMatrix(GameState *state) {
    TMVec3 position{0, 0, 10};
    TMVec3 target{0, 0, 0};
    TMVec3 up{0, 1, 0};
    state->view = TMMat4LookAt(position, target, up);
    TMRendererShaderUpdate(state->shader, "uView", state->view);
    TMRendererShaderUpdate(state->gltfShader, "uView", state->view);
}

static void InitializeEntities(GameState *state) {
    // manuel: init player1
    state->player1Size.x = 400/4;
    state->player1Size.y = 100/4;
    state->player1Position.x = -200/4;
    state->player1Position.y = 800/4;
    // manuel: init player2
    state->player2Size.x = 400/4;
    state->player2Size.y = 100/4;
    state->player2Position.x = 400/4;
    state->player2Position.y = -800/4;
    // manuel: init ball
    state->ballPosition.x = 0;
    state->ballPosition.y = 0;
    state->ballVelocity.x = rand() / 2.0f;
    state->ballVelocity.y = rand();
    TMVec2Normalize(&state->ballVelocity);
    state->ballVelocity = state->ballVelocity * 40/4;
    state->ballSize.x = 200/4;
    state->ballSize.y = 200/4;
}

static void CollisionDetectionAndResolution(GameState *state, int width, int height) {
    TMVec2 halfBallSize = state->ballSize*0.5f;
    TMVec2 halfPlayerSize = state->player1Size*0.5f;
    TMVec2 player1 = state->player1Position;
    TMVec2 player2 = state->player2Position;
    TMVec2 ball = state->ballPosition;
    if(ball.x - halfBallSize.x <= player1.x + halfPlayerSize.x &&
       ball.x + halfBallSize.x >= player1.x - halfPlayerSize.x &&
       ball.y - halfBallSize.y <= player1.y + halfPlayerSize.y &&
       ball.y + halfBallSize.y >= player1.y - halfPlayerSize.y) {
        if(state->ballVelocity.y > 0.0f) {
            state->ballVelocity.y = -state->ballVelocity.y;
        }
    }
    if(ball.x - halfBallSize.x <= player2.x + halfPlayerSize.x &&
       ball.x + halfBallSize.x >= player2.x - halfPlayerSize.x &&
       ball.y - halfBallSize.y <= player2.y + halfPlayerSize.y &&
       ball.y + halfBallSize.y >= player2.y - halfPlayerSize.y) {
        if(state->ballVelocity.y < 0.0f) {
            state->ballVelocity.y = -state->ballVelocity.y;
        }    }
    if(state->ballPosition.x <= -width/2) {
        state->ballPosition.x = (-width/2) + 1;
        state->ballVelocity.x = -state->ballVelocity.x;
    }
    if(state->ballPosition.x >= width/2) {
        state->ballPosition.x = (width/2) - 1;
        state->ballVelocity.x = -state->ballVelocity.x;
    }
    if(state->ballPosition.y <= -height/2) {
        state->ballPosition.y = (-height/2) + 1;
        state->ballVelocity.y = -state->ballVelocity.y;
    }
    if(state->ballPosition.y >= height/2) {
        state->ballPosition.y = (height/2) - 1;
        state->ballVelocity.y = -state->ballVelocity.y;
    }
}

static void PrintVec2(TMVec2 v) {
    printf("x: %f y: %f\n", v.x, v.y);
}

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

enum glTFtype {
    GLTF_SIGNED_BYTE    = 5120,
    GLTF_UNSIGNED_BYTE  = 5121,
    GLTF_SIGNED_SHORT   = 5122,
    GLTF_UNSIGNED_SHORT = 5123,
    GLTF_UNSIGNED_INT   = 5125,
    GLTF_FLOAT          = 5126
};


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

void GameInitialize(GameState *state) {
    state->renderer = TMRendererCreate();
    
    state->shader = TMRendererShaderCreate(state->renderer,
                                           "../../assets/shaders/vert.glsl",
                                           "../../assets/shaders/frag.glsl");
    state->gltfShader = TMRendererShaderCreate(state->renderer,
                                           "../../assets/shaders/gltfVert.glsl",
                                           "../../assets/shaders/gltfFrag.glsl");


    state->buffer = TMRendererBufferCreate(state->renderer,
                                           vertices, ARRAY_LENGTH(vertices),
                                           indices,  ARRAY_LENGTH(indices));
    state->cubeBuffer = TMRendererBufferCreate(state->renderer, cubeVertices, ARRAY_LENGTH(cubeVertices));

    state->donutTexture = TMRendererTextureCreate(state->renderer,      "../../assets/images/donut.png");
    state->backgroundTexture = TMRendererTextureCreate(state->renderer, "../../assets/images/back.png");
    state->paddle1Texture = TMRendererTextureCreate(state->renderer,    "../../assets/images/paddle_1.png");
    state->paddle2Texture = TMRendererTextureCreate(state->renderer,    "../../assets/images/paddle_2.png");
    state->moonTexture = TMRendererTextureCreate(state->renderer,       "../../assets/images/moon.png");
    state->cloneTexture = TMRendererTextureCreate(state->renderer,      "../../assets/images/clone.png");

    UpdateProjectionsMatrices(state);
    UpdateViewMatrix(state);
    TMRendererFaceCulling(false, 0);

    // manuel: Initializes random number generator
    time_t t;
    srand((unsigned) time(&t));

    InitializeEntities(state);

    TMJson *json = TMJsonOpen("../../assets/json/test.json");

    printf("Json Opened!!\n");
    TMJsonObject *jsonEntities = json->root.childs;
    unsigned int entityCount = (unsigned int)jsonEntities->valuesCount;
    
    state->entities = (Entity *)malloc(sizeof(Entity) * entityCount);
    memset(state->entities, 0, sizeof(Entity) * entityCount);
 
    for(int i = 0; i < entityCount; ++i) {
        TMJsonObject *jsonEntity = jsonEntities->objects + i;
        Entity *entity = state->entities + i;

        TMJsonObject *position = jsonEntity->childs + 0;
        TMJsonObject *xPos = position->childs + 0;
        TMJsonObject *yPos = position->childs + 1;
        entity->position.x = StringToFloat(xPos->values[0].value, xPos->values[0].size);
        entity->position.y = StringToFloat(yPos->values[0].value, yPos->values[0].size);
                       
        TMJsonObject *velocity = jsonEntity->childs + 1;
        TMJsonObject *xVel = velocity->childs + 0;
        TMJsonObject *yVel = velocity->childs + 1;
        entity->velocity.x = StringToFloat(xVel->values[0].value, xVel->values[0].size);
        entity->velocity.y = StringToFloat(yVel->values[0].value, yVel->values[0].size);

        TMJsonObject *size = jsonEntity->childs + 2;
        TMJsonObject *w = size->childs + 0;
        TMJsonObject *h = size->childs + 1;
        entity->size.x = StringToFloat(w->values[0].value, w->values[0].size);
        entity->size.y = StringToFloat(h->values[0].value, h->values[0].size);

        TMJsonObject *texture  = jsonEntity->childs + 3;
        const char *textureFilepath = StringToNullTerString(texture->values[0].value, texture->values[0].size);
        entity->texture = TMRendererTextureCreate(state->renderer, textureFilepath);
        free((void *)textureFilepath);
        state->entityCount++;
    }
    
    for(int i = 0; i < state->entityCount; ++i) {
        Entity *entity = state->entities + i;
        printf("Entity:\n");
        printf("Position: ");
        PrintVec2(entity->position);
        printf("Velocity: ");
        PrintVec2(entity->velocity); 
        printf("Size: ");
        PrintVec2(entity->size); 
    }
    
    TMJsonClose(json);

#if 1 
 
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

    state->gltfBuffer = TMRendererBufferCreate(state->renderer,
                                               pos, posLength/sizeof(TMVec3),
                                               uvs, uvsLength/sizeof(TMVec2),
                                               nor, norLength/sizeof(TMVec3),
                                               ind, indLength/sizeof(unsigned short));
    free(pos);
    free(uvs);
    free(nor);
    free(ind);

    TMFileClose(&cloneData);
    TMJsonClose(clone);

#else

    TMJson *cube = TMJsonOpen("../../assets/gltf/cube/cube.gltf");
    TMFile cubeData = TMFileOpen("../../assets/gltf/cube/cube.bin");

    // TODO(manuel): load the model ....
    TMJsonObject *root = &cube->root;

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
     
    GetAttributeAtIndex(root, posIndex, (void **)&pos, &posLength, cubeData.data);
    GetAttributeAtIndex(root, uvsIndex, (void **)&uvs, &uvsLength, cubeData.data);
    GetAttributeAtIndex(root, norIndex, (void **)&nor, &norLength, cubeData.data);
    GetAttributeAtIndex(root, indIndex, (void **)&ind, &indLength, cubeData.data);

    state->gltfBuffer = TMRendererBufferCreate(state->renderer,
                                               pos, posLength/sizeof(TMVec3),
                                               uvs, uvsLength/sizeof(TMVec2),
                                               nor, norLength/sizeof(TMVec3),
                                               ind, indLength/sizeof(unsigned short));
    free(pos);
    free(uvs);
    free(nor);
    free(ind);

    TMFileClose(&cubeData);
    TMJsonClose(cube);
#endif
}

void GameUpdate(GameState *state) {
    int width = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);

    int mouseX = TMInputMousePositionX();

    if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {
        state->player1Position.x = mouseX - width/2;
    }
    if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_RIGHT)) {
        state->player2Position.x = mouseX - width/2;
    }

    CollisionDetectionAndResolution(state, width, height);

    state->ballPosition = state->ballPosition + state->ballVelocity;
}

void DrawEntity(Entity *entity, TMShader *shader, TMBuffer *buffer) {
    TMMat4 scale = TMMat4Scale(entity->size.x, entity->size.y, 1);
    TMMat4 trans = TMMat4Translate(entity->position.x, entity->position.y, 0);
    TMMat4 world = trans * scale;
    TMRendererShaderUpdate(shader, "uWorld", world);
    // manuel: set the background texture
    TMRendererTextureBind(entity->texture, shader, "uTexture", 0);
    // manuel: draw the background
    TMRendererDrawBufferElements(buffer);
}

void GameRender(GameState *state) {

    if(TMRendererUpdateRenderArea(state->renderer)) {
        UpdateProjectionsMatrices(state);
    }

    TMRendererClear(0.1f, 0.5f, 0.1f, 1.0f, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

    // manuel: set the shader
    TMRendererBindShader(state->shader);
    
    
    TMRendererShaderUpdate(state->shader, "uProj", state->orthographic);

    // manuel: render the background
    int width = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);

     static float angle = 0.0f;

    TMMat4 scale = TMMat4Identity();
    TMMat4 trans = TMMat4Identity();
    TMMat4 rotat = TMMat4Identity();
    TMMat4 world = TMMat4Identity();   

    scale = TMMat4Scale(width, height, 1);
    world = scale;
    TMRendererShaderUpdate(state->shader, "uWorld", world);
    // manuel: set the background texture
    TMRendererTextureBind(state->backgroundTexture, state->shader, "uTexture", 0);
    // manuel: draw the background
    TMRendererDrawBufferElements(state->buffer);
    // manuel: draw player one
    scale = TMMat4Scale(state->player1Size.x, state->player1Size.y, 1);
    trans = TMMat4Translate(state->player1Position.x, state->player1Position.y, 0);
    world = trans * scale;
    TMRendererShaderUpdate(state->shader, "uWorld", world);
    TMRendererTextureBind(state->paddle1Texture, state->shader, "uTexture", 0);
    TMRendererDrawBufferElements(state->buffer);
    // manuel: draw player two
    scale = TMMat4Scale(state->player2Size.x, state->player2Size.y, 1);
    trans = TMMat4Translate(state->player2Position.x, state->player2Position.y, 0);
    world = trans * scale;
    TMRendererShaderUpdate(state->shader, "uWorld", world);
    TMRendererTextureBind(state->paddle2Texture, state->shader, "uTexture", 0);
    TMRendererDrawBufferElements(state->buffer);
    // manuel: draw donut
    scale = TMMat4Scale(state->ballSize.x, state->ballSize.y, 1);
    trans = TMMat4Translate(state->ballPosition.x, state->ballPosition.y, 0);
    rotat = TMMat4RotateZ(angle);
    world = trans *  rotat * scale;
    TMRendererShaderUpdate(state->shader, "uWorld", world);
    TMRendererTextureBind(state->donutTexture, state->shader, "uTexture", 0);
    TMRendererDrawBufferElements(state->buffer);

    for(int i = 0; i < state->entityCount; ++i) {
        DrawEntity(state->entities + i, state->shader, state->buffer);
    }

    // manuel: draw the 3d cube
    TMRendererShaderUpdate(state->shader, "uProj", state->perspective);
    TMRendererShaderUpdate(state->gltfShader, "uProj", state->perspective);
    TMRendererDepthTestEnable();

    trans = TMMat4Translate(2, 4, 0);
    rotat = TMMat4RotateY(angle) * TMMat4RotateX(angle);
    world = trans * rotat;
    TMRendererShaderUpdate(state->shader, "uWorld", world);
    TMRendererTextureBind(state->moonTexture, state->shader, "uTexture", 0);
    TMRendererDrawBufferArray(state->cubeBuffer);

    //TMRendererBindShader(state->gltfShader);
    
    trans = TMMat4Translate(-2, 0, 0);
    rotat = TMMat4RotateY(angle);
    scale = TMMat4Scale(1, 1, 1);
    world = trans * rotat * scale;
    TMRendererShaderUpdate(state->gltfShader, "uWorld", world);
    TMRendererTextureBind(state->cloneTexture, state->gltfShader, "uTexture", 0);
    TMRendererDrawBufferElements(state->gltfBuffer);

    TMRendererDepthTestDisable();

    angle += 0.02f;

    //TMRendererPresent(state->renderer);
}

void GameShutdown(GameState *state) {
    TMRendererTextureDestroy(state->renderer, state->cloneTexture);
    TMRendererTextureDestroy(state->renderer, state->moonTexture);
    TMRendererTextureDestroy(state->renderer, state->donutTexture);
    TMRendererTextureDestroy(state->renderer, state->backgroundTexture);
    TMRendererTextureDestroy(state->renderer, state->paddle1Texture);
    TMRendererTextureDestroy(state->renderer, state->paddle2Texture);
    TMRendererBufferDestroy(state->renderer, state->cubeBuffer);
    TMRendererBufferDestroy(state->renderer, state->buffer);
    TMRendererShaderDestroy(state->renderer, state->shader);
    TMRendererDestroy(state->renderer);
}
