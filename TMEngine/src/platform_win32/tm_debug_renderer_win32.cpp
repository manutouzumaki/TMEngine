#include "../tm_debug_renderer.h"
#include "../utils/tm_math.h"
#include "../utils/tm_darray.h"

#include "../tm_renderer.h"
#include "tm_renderer_win32.h"

#include <assert.h>
#include <math.h>



static const char *gVertSource =
"cbuffer CBuffer : register(b0)\n"
"{\n"
"    matrix proj;\n"
"    matrix view;\n"
"    matrix world;\n"
"}\n"
"struct VS_Input\n"
"{\n"
"    float3 pos : POSITION;\n"
"    float4 col : TEXCOORD0;\n"
"};\n"
"struct PS_Input\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 col : TEXCOORD0;\n"
"};\n"
"PS_Input VS_Main(VS_Input vertex)\n"
"{\n"
"    PS_Input vsOut = (PS_Input)0;\n"
"    float4 viewPos = mul(float4(vertex.pos, 1.0f), view);\n"
"    vsOut.pos = mul(viewPos, proj);\n"
"    vsOut.col = vertex.col;\n"
"    return vsOut;\n"
"}\0";

static const char *gFragSource =
"struct PS_Input {\n"
"    float4 pos : SV_POSITION;\n"
"    float4 col : COLOR;\n"
"};\n"
"float4 PS_Main(PS_Input frag) : SV_TARGET {\n"
"    return frag.col;\n"
"}\0";

struct DebugVertex {
    TMVec3 position;
    TMVec4 color;
};

static TMRenderer *gRenderer;

static ID3D11Buffer      *gGPUBuffer;
static ID3D11InputLayout *gGPULayout;
static DebugVertex       *gCPUBuffer;
static size_t             gBufferSize;
static size_t             gBufferSizeInBytes;
static size_t             gBufferUsed;
static TMShader          *gShader;


static size_t StringLength(const char *string) {
    size_t counter = 0;
    char *byte = (char *)string;
    while(*byte++ != '\0') counter++;
    return counter;
}

void TMDebugRendererInitialize_(TMRenderer *renderer, size_t bufferSize) {
    gRenderer = renderer;

    gBufferSize = bufferSize;
    gBufferUsed = 0;
    gShader = TMRendererShaderCreateFromString(renderer, gVertSource, StringLength(gVertSource),
                                                         gFragSource, StringLength(gFragSource));

    size_t bufferSizeInBytes = gBufferSize*sizeof(DebugVertex);
    // create gGPUBuffer
    D3D11_BUFFER_DESC vertexDesc;
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexDesc.ByteWidth = bufferSizeInBytes;
    HRESULT result = gRenderer->device->CreateBuffer(&vertexDesc, NULL, &gGPUBuffer);

    // create gGPULayout
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    int totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = renderer->device->CreateInputLayout(inputLayoutDesc,
        totalLayoutElements,
        gShader->vertexShaderCompiled->GetBufferPointer(),
        gShader->vertexShaderCompiled->GetBufferSize(),
        &gGPULayout);

    // create gCPUBuffer
    gCPUBuffer = (DebugVertex *)malloc(bufferSizeInBytes);
    memset(gCPUBuffer, 0, bufferSizeInBytes);

}


void TMDebugRendererShutdown_() {
    if(gRenderer) {
        TMRendererShaderDestroy(gRenderer, gShader);
    }
    if(gGPUBuffer) gGPUBuffer->Release();
    if(gGPULayout) gGPULayout->Release();
    if(gCPUBuffer) free(gCPUBuffer);
}

static void LocalToWorld(DebugVertex *quad, unsigned int count, float x, float y, float z, float w, float h, float angle) {
    TMMat4 world = TMMat4Translate(x, y, z) * TMMat4RotateZ(angle) * TMMat4Scale(w, h, 1);
    for(int i = 0; i < count; ++i) {
        quad[i].position = TMMat4TransformPoint(world, quad[i].position);
    }
}

static void LocalToWorld(DebugVertex *circle, unsigned int count, float x, float y, float z, float radio) {
    TMMat4 world = TMMat4Translate(x, y, z) * TMMat4Scale(radio, radio, 1);
    for(int i = 0; i < count; ++i) {
        circle[i].position = TMMat4TransformPoint(world, circle[i].position);
    }
}

static void LocalToWorld(DebugVertex *capsule, unsigned int count, float x, float y, float z, float radio, float angle) {
    TMMat4 world = TMMat4Translate(x, y, z) * TMMat4RotateZ(angle) * TMMat4Scale(radio, radio, 1);
    for(int i = 0; i < count; ++i) {
        capsule[i].position = TMMat4TransformPoint(world, capsule[i].position);
    }
}

static void SetColor(DebugVertex *quad, unsigned int count, unsigned int color) {
    float a = (float)((color >> 24) & 0xFF) / 255.0f;
    float r = (float)((color >> 16) & 0xFF) / 255.0f;
    float g = (float)((color >>  8) & 0xFF) / 255.0f;
    float b = (float)((color >>  0) & 0xFF) / 255.0f;
    for(int i = 0; i < count; ++i) {
        quad[i].color = {r, g, b, a};
    }
}


static void AddFigureToBuffer(DebugVertex *quad, unsigned int count) {
    assert(gBufferUsed < gBufferSize);
    assert(count < gBufferSize);
    DebugVertex *vertex = gCPUBuffer + gBufferUsed;
    memcpy(vertex, quad, sizeof(DebugVertex)*count);
    gBufferUsed += count;
}

void TMDebugRendererDrawQuad_(float x, float y, float w, float h, float angle, unsigned int color) {

    DebugVertex quad[] = {
        {TMVec3{-0.5f,   0.5f, 1}, TMVec4{0, 1, 0, 1}}, 
        {TMVec3{ 0.5f,   0.5f, 1}, TMVec4{0, 1, 0, 1}}, 
        
        {TMVec3{ 0.5f,   0.5f, 1}, TMVec4{0, 1, 0, 1}}, 
        {TMVec3{ 0.5f,  -0.5f, 1}, TMVec4{0, 1, 0, 1}},

        {TMVec3{ 0.5f,  -0.5f, 1}, TMVec4{0, 1, 0, 1}},
        {TMVec3{-0.5f,  -0.5f, 1}, TMVec4{0, 1, 0, 1}},
        
        {TMVec3{-0.5f,  -0.5f, 1}, TMVec4{0, 1, 0, 1}},
        {TMVec3{-0.5f,   0.5f, 1}, TMVec4{0, 1, 0, 1}} 
    };
    LocalToWorld(quad, 8, x, y, 0, w, h, angle);
    SetColor(quad, 8, color);


    if(gBufferUsed >= gBufferSize) {
        TMDebugRenderDraw_();
    }
    AddFigureToBuffer(quad, 8);
}


void TMDebugRendererDrawCircle_(float x, float y, float radio, unsigned int color, unsigned int vertNum) {

    DebugVertex *circle = NULL;
    float angle = 0; 
    float angleIncrement = (2*TM_PI)/vertNum;
    DebugVertex lastVertex{};

    for(int i = 0; i < vertNum + 1; ++i) {
        
        DebugVertex currentVertex = {
            {cosf(angle), sinf(angle), 1},
            {0, 0, 0, 1}
        };

        if(i >= 1) {
            TMDarrayPush(circle, lastVertex, DebugVertex);
            TMDarrayPush(circle, currentVertex, DebugVertex);
        }
        angle += angleIncrement;
        lastVertex = currentVertex;
    }

    LocalToWorld(circle, TMDarraySize(circle), x, y, 0, radio);
    SetColor(circle, TMDarraySize(circle), color);

    if(gBufferUsed >= gBufferSize) {
        TMDebugRenderDraw_();
    }
    AddFigureToBuffer(circle, TMDarraySize(circle));
    
    TMDarrayDestroy(circle);
}

void TMDebugRendererDrawCapsule(float x, float y, float radio, float halfHeight, float rotation,
                                unsigned int color, unsigned int vertNum) {

    DebugVertex *capsule = NULL;

    float angle = 0; 
    float angleIncrement = (2*TM_PI)/vertNum;
    float offset = halfHeight/radio;
    
    DebugVertex lastVertex{};

    for(int i = 0; i < (vertNum/2) + 1; ++i) {
        DebugVertex currentVertex = {
            {cosf(angle), sinf(angle)+offset, 1},
            {0, 0, 0, 1}
        };

        if(i >= 1) {
            TMDarrayPush(capsule, lastVertex, DebugVertex);
            TMDarrayPush(capsule, currentVertex, DebugVertex);
        }

        if(i < vertNum/2) angle += angleIncrement;
        lastVertex = currentVertex;
    }
        
    TMDarrayPush(capsule, lastVertex, DebugVertex);
    lastVertex.position.y -= offset*2;
    TMDarrayPush(capsule, lastVertex, DebugVertex);

    for(int i = 0; i < (vertNum/2) + 1; ++i) {
        
        DebugVertex currentVertex = {
            {cosf(angle), sinf(angle)-offset, 1},
            {0, 0, 0, 1}
        };

        if(i >= 1) {
            TMDarrayPush(capsule, lastVertex, DebugVertex);
            TMDarrayPush(capsule, currentVertex, DebugVertex);
        }
        angle += angleIncrement;
        lastVertex = currentVertex;
    }

    TMDarrayPush(capsule, lastVertex, DebugVertex);
    lastVertex.position.y += offset*2;
    TMDarrayPush(capsule, lastVertex, DebugVertex);


    LocalToWorld(capsule, TMDarraySize(capsule), x, y, 0, radio, rotation);
    SetColor(capsule, TMDarraySize(capsule), color);

    if(gBufferUsed >= gBufferSize) {
        TMDebugRenderDraw_();
    }
    AddFigureToBuffer(capsule, TMDarraySize(capsule));


    TMDarrayDestroy(capsule);

}

void TMDebugRenderDraw_() {
    
    D3D11_MAPPED_SUBRESOURCE bufferData;
    ZeroMemory(&bufferData, sizeof(bufferData));
    gRenderer->deviceContext->Map(gGPUBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
    memcpy(bufferData.pData, gCPUBuffer, sizeof(DebugVertex)*gBufferUsed);
    gRenderer->deviceContext->Unmap(gGPUBuffer, 0);

    unsigned int stride = sizeof(DebugVertex);
    unsigned int offset = 0;
    gRenderer->deviceContext->IASetInputLayout(gGPULayout);
    TMRendererBindShader(gRenderer, gShader);
    gRenderer->deviceContext->IASetVertexBuffers(0, 1, &gGPUBuffer, &stride, &offset);

    gRenderer->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    gRenderer->deviceContext->Draw(gBufferUsed, 0);

    gBufferUsed = 0;

}


