#ifndef _TM_RENDERER_WIN32_H_
#define _TM_RENDERER_WIN32_H_

#include <d3d11.h>
#include <d3dcompiler.h>

struct TMMemoryPool;
struct TMBatchVertex;


struct TMBuffer {
    ID3D11Buffer* vertices[3];
    ID3D11Buffer* indices;
    ID3D11InputLayout* layout;
    unsigned int verticesCount;
    unsigned int indicesCount;
    unsigned int numBuffers;
    unsigned int stride[3];
    unsigned int offset[3];
    DXGI_FORMAT indexBufferFormat;

};

struct TMShader {
    ID3D11VertexShader* vertex;
    ID3D11PixelShader* pixel;
    ID3DBlob* vertexShaderCompiled;
    ID3DBlob* fragmentShaderCompile;
};

struct TMShaderBuffer {
    ID3D11Buffer *buffer;
    unsigned int index;
};

struct TMTexture {
    int width;
    int height;
    ID3D11ShaderResourceView* colorMap;
    ID3D11SamplerState* colorMapSampler;
};

struct TMRenderBatch {
    TMRenderer *renderer;
    TMShader *shader;
    TMTexture *texture;

    ID3D11Buffer *buffer;
    ID3D11InputLayout *layout;
    TMBatchVertex *batchBuffer;
    unsigned int bufferSizeInBytes;
    unsigned int size;
    unsigned int used;
};

struct TMInstanceRenderer {
    ID3D11InputLayout *layout;
    ID3D11Buffer *vertBuffer;
    ID3D11Buffer *instBuffer;
    unsigned int instCount;
    unsigned int instSize;
};

struct TMFramebuffer {
    unsigned int id;
};

struct TMRenderer {
    int width;
    int height;
    TMMemoryPool* buffersMemory;
    TMMemoryPool* texturesMemory;
    TMMemoryPool* shadersMemory;
    TMMemoryPool* framebufferMemory;
    TMMemoryPool* shaderBuffersMemory;
    TMMemoryPool* renderBatchsMemory;
    TMMemoryPool* instanceRendererMemory;

    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    ID3D11DepthStencilView* depthStencilView;
    ID3D11RasterizerState* wireFrameRasterizer;
    ID3D11RasterizerState* fillRasterizerCullBack;
    ID3D11RasterizerState* fillRasterizerCullFront;
    ID3D11RasterizerState* fillRasterizerCullNone;
    ID3D11DepthStencilState* depthStencilOn;
    ID3D11DepthStencilState* depthStencilOff;
    ID3D11BlendState* alphaBlendEnable;
    ID3D11BlendState* alphaBlendDisable;
};

#endif
