#include "../tm_renderer.h"
#include "../utils/tm_memory_pool.h"
#include "../utils/tm_file.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdlib.h>
#include <stdio.h>

#include <d3d11.h>
#include <d3dcompiler.h>

// TODO: try to remove this ...
struct TMWindow {
    HWND hwndWindow;
    int width;
    int height;
};

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
};

struct TMTexture {
    int width;
    int height;
    ID3D11ShaderResourceView* colorMap;
    ID3D11SamplerState* colorMapSampler;
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

void InitD3D11(TMRenderer* renderer, TMWindow *window) {
    int clientWidth = window->width;
    int clientHeight = window->height;
    // - 1: Define the device types and feature level we want to check for.
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_SOFTWARE
    };
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    int driverTypesCount = ARRAY_LENGTH(driverTypes);
    int featureLevelsCount = ARRAY_LENGTH(featureLevels);
    // - 2: create the d3d11 device, rendering context, and swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window->hwndWindow;
    swapChainDesc.Windowed = true;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel;
    D3D_DRIVER_TYPE driverType;
    HRESULT result;
    for (unsigned int driver = 0; driver < driverTypesCount; ++driver) {
        result = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[driver], NULL, 0, featureLevels, featureLevelsCount, D3D11_SDK_VERSION, &swapChainDesc,
            &renderer->swapChain, &renderer->device, &featureLevel, &renderer->deviceContext);
        if (SUCCEEDED(result)) {
            driverType = driverTypes[driver];
            break;
        }
    }

    // - 3: Create render target view.
    ID3D11Texture2D* backBufferTexture = NULL;
    result = renderer->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
    result = renderer->device->CreateRenderTargetView(backBufferTexture, 0, &renderer->renderTargetView);
    if (backBufferTexture) {
        backBufferTexture->Release();
    }

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = clientWidth;
    viewport.Height = clientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    renderer->deviceContext->RSSetViewports(1, &viewport);

    // create the depth stencil texture
    ID3D11Texture2D* depthStencilTexture = 0;
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
    ZeroMemory(&depthStencilTextureDesc, sizeof(depthStencilTextureDesc));
    depthStencilTextureDesc.Width = clientWidth;
    depthStencilTextureDesc.Height = clientHeight;
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.ArraySize = 1;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilTextureDesc.SampleDesc.Count = 1;
    depthStencilTextureDesc.SampleDesc.Quality = 0;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilTextureDesc.CPUAccessFlags = 0;
    depthStencilTextureDesc.MiscFlags = 0;
    // create depth stencil states
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    // Depth test parameters
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    // Stencil test parameters
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    renderer->device->CreateDepthStencilState(&depthStencilDesc, &renderer->depthStencilOn);
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.StencilEnable = false;
    renderer->device->CreateDepthStencilState(&depthStencilDesc, &renderer->depthStencilOff);

    // create the depth stencil view
    result = renderer->device->CreateTexture2D(&depthStencilTextureDesc, NULL, &depthStencilTexture);
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    result = renderer->device->CreateDepthStencilView(depthStencilTexture, &descDSV, &renderer->depthStencilView);
    if (depthStencilTexture)
    {
        depthStencilTexture->Release();
    }

    // Alpha blending
    D3D11_BLEND_DESC blendStateDesc = {};
    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    renderer->device->CreateBlendState(&blendStateDesc, &renderer->alphaBlendEnable);

    blendStateDesc.RenderTarget[0].BlendEnable = false;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    renderer->device->CreateBlendState(&blendStateDesc, &renderer->alphaBlendDisable);

    // Create Rasterizers Types
    D3D11_RASTERIZER_DESC fillRasterizerFrontDesc = {};
    fillRasterizerFrontDesc.FillMode = D3D11_FILL_SOLID;
    fillRasterizerFrontDesc.CullMode = D3D11_CULL_FRONT;
    fillRasterizerFrontDesc.DepthClipEnable = true;
    renderer->device->CreateRasterizerState(&fillRasterizerFrontDesc, &renderer->fillRasterizerCullFront);

    D3D11_RASTERIZER_DESC fillRasterizerBackDesc = {};
    fillRasterizerBackDesc.FillMode = D3D11_FILL_SOLID;
    fillRasterizerBackDesc.CullMode = D3D11_CULL_BACK;
    fillRasterizerBackDesc.DepthClipEnable = true;
    renderer->device->CreateRasterizerState(&fillRasterizerBackDesc, &renderer->fillRasterizerCullBack);

    D3D11_RASTERIZER_DESC fillRasterizerNoneDesc = {};
    fillRasterizerNoneDesc.FillMode = D3D11_FILL_SOLID;
    fillRasterizerNoneDesc.CullMode = D3D11_CULL_NONE;
    fillRasterizerNoneDesc.DepthClipEnable = true;
    renderer->device->CreateRasterizerState(&fillRasterizerNoneDesc, &renderer->fillRasterizerCullNone);

    D3D11_RASTERIZER_DESC wireFrameRasterizerDesc = {};
    wireFrameRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    wireFrameRasterizerDesc.CullMode = D3D11_CULL_NONE;
    wireFrameRasterizerDesc.DepthClipEnable = true;
    renderer->device->CreateRasterizerState(&wireFrameRasterizerDesc, &renderer->wireFrameRasterizer);

    printf("D3D11 Initialized\n");

}

TMRenderer* TMRendererCreate(TMWindow *window) {
    TMRenderer* renderer = (TMRenderer*)malloc(sizeof(TMRenderer));

    renderer->width = window->width;
    renderer->height = window->height;
    InitD3D11(renderer, window);

    renderer->buffersMemory = TMMemoryPoolCreate(sizeof(TMBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->texturesMemory = TMMemoryPoolCreate(sizeof(TMTexture), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shadersMemory = TMMemoryPoolCreate(sizeof(TMShader), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->framebufferMemory = TMMemoryPoolCreate(sizeof(TMFramebuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shaderBuffersMemory = TMMemoryPoolCreate(sizeof(TMShaderBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);

    renderer->deviceContext->OMSetRenderTargets(1, &renderer->renderTargetView, renderer->depthStencilView);
    renderer->deviceContext->OMSetDepthStencilState(renderer->depthStencilOn, 1);
    renderer->deviceContext->OMSetBlendState(renderer->alphaBlendEnable, 0, 0xffffffff);
    renderer->deviceContext->RSSetState(renderer->fillRasterizerCullBack);

    ShowWindow(window->hwndWindow, SW_SHOW);
    UpdateWindow(window->hwndWindow);

    return renderer;
}

void TMRendererDestroy(TMRenderer* renderer) {
    if (renderer->alphaBlendDisable) renderer->alphaBlendDisable->Release();
    if (renderer->alphaBlendEnable) renderer->alphaBlendEnable->Release();
    if (renderer->depthStencilOff) renderer->depthStencilOff->Release();
    if (renderer->depthStencilOn) renderer->depthStencilOn->Release();
    if (renderer->fillRasterizerCullNone) renderer->fillRasterizerCullNone->Release();
    if (renderer->fillRasterizerCullFront) renderer->fillRasterizerCullFront->Release();
    if (renderer->fillRasterizerCullBack) renderer->fillRasterizerCullBack->Release();
    if (renderer->wireFrameRasterizer) renderer->wireFrameRasterizer->Release();
    if (renderer->depthStencilView) renderer->depthStencilView->Release();
    if (renderer->renderTargetView) renderer->renderTargetView->Release();
    if (renderer->swapChain) renderer->swapChain->Release();
    if (renderer->deviceContext) renderer->deviceContext->Release();
    if (renderer->device) renderer->device->Release();
    TMMemoryPoolDestroy(renderer->buffersMemory);
    TMMemoryPoolDestroy(renderer->texturesMemory);
    TMMemoryPoolDestroy(renderer->shadersMemory);
    TMMemoryPoolDestroy(renderer->framebufferMemory);
    TMMemoryPoolDestroy(renderer->shaderBuffersMemory);
    free(renderer);
}

void TMRendererDepthTestEnable(TMRenderer *renderer) {
    renderer->deviceContext->OMSetDepthStencilState(renderer->depthStencilOn, 1);
}

void TMRendererDepthTestDisable(TMRenderer *renderer) {
    renderer->deviceContext->OMSetDepthStencilState(renderer->depthStencilOff, 1);
}

void TMRendererFaceCulling(TMRenderer *renderer, bool value, unsigned int flags) {
    if (value) {
        if (flags == TM_CULL_BACK) {
            renderer->deviceContext->RSSetState(renderer->fillRasterizerCullBack);
            return;
        }
        if (flags == TM_CULL_FRONT) {
            renderer->deviceContext->RSSetState(renderer->fillRasterizerCullFront);
            return;
        }
        if (flags == (TM_CULL_BACK | TM_CULL_FRONT)) {
            // TODO: ...
            return;
        }
    }
    else {
        renderer->deviceContext->RSSetState(renderer->fillRasterizerCullNone);
    }
}

int TMRendererGetWidth(TMRenderer* renderer) {
    return renderer->width;
}

int TMRendererGetHeight(TMRenderer* renderer) {
    return renderer->height;
}

bool TMRendererUpdateRenderArea(TMRenderer* renderer) {
    // TODO: ...
    return false;
}

void TMRendererClear(TMRenderer *renderer, float r, float g, float b, float a, unsigned  int flags) {
    if (flags & TM_COLOR_BUFFER_BIT) {
        float clearColor[] = { r, g, b, a };
        renderer->deviceContext->ClearRenderTargetView(renderer->renderTargetView, clearColor);
    }
    if (flags & TM_DEPTH_BUFFER_BIT) {
        renderer->deviceContext->ClearDepthStencilView(renderer->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    if (flags & TM_STENCIL_BUFFER_BIT) {
        renderer->deviceContext->ClearDepthStencilView(renderer->depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void TMRendererPresent(TMRenderer* renderer) {
    renderer->swapChain->Present(1, 0);
}


TMBuffer* TMRendererBufferCreate(TMRenderer* renderer,
    TMVertex* vertices, unsigned int verticesCount,
    TMShader *shader) {
   
    TMBuffer* buffer = (TMBuffer*)TMMemoryPoolAlloc(renderer->buffersMemory);

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));

    D3D11_BUFFER_DESC vertexDesc;
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(TMVertex) * verticesCount;
    resourceData.pSysMem = vertices;
    HRESULT result = renderer->device->CreateBuffer(&vertexDesc, &resourceData, &buffer->vertices[0]);
    if (FAILED(result)) {
        printf("Error: failed vertex buffer creation ...\n");
        return NULL;
    }

    buffer->verticesCount = verticesCount;
    buffer->numBuffers = 1;
    buffer->stride[0] = sizeof(TMVertex);
    buffer->offset[0] = 0;

    // create input layout.
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    int totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = renderer->device->CreateInputLayout(inputLayoutDesc,
        totalLayoutElements,
        shader->vertexShaderCompiled->GetBufferPointer(),
        shader->vertexShaderCompiled->GetBufferSize(),
        &buffer->layout);

    return buffer;
  
    return buffer;
}

TMBuffer* TMRendererBufferCreate(TMRenderer* renderer,
    TMVertex* vertices, unsigned int verticesCount,
    unsigned int* indices, unsigned int indicesCount,
    TMShader *shader) {

    TMBuffer* buffer = (TMBuffer*)TMMemoryPoolAlloc(renderer->buffersMemory);

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));

    D3D11_BUFFER_DESC vertexDesc;
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(TMVertex) * verticesCount;
    resourceData.pSysMem = vertices;
    HRESULT result = renderer->device->CreateBuffer(&vertexDesc, &resourceData, &buffer->vertices[0]);
    if (FAILED(result)) {
        printf("Error: failed vertex buffer creation ...\n");
        return NULL;
    }

    D3D11_BUFFER_DESC indexDesc;
    ZeroMemory(&indexDesc, sizeof(indexDesc));
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.ByteWidth = sizeof(unsigned int) * indicesCount;
    resourceData.pSysMem = indices;
    result = renderer->device->CreateBuffer(&indexDesc, &resourceData, &buffer->indices);
    if (FAILED(result)) {
        printf("Error: failed index buffer creation ...\n");
        return NULL;
    }
 
    buffer->verticesCount = verticesCount;
    buffer->indicesCount = indicesCount;
    buffer->numBuffers = 1;
    buffer->stride[0] = sizeof(TMVertex);
    buffer->offset[0] = 0;
    buffer->indexBufferFormat = DXGI_FORMAT_R32_UINT;

    // create input layout.
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    int totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = renderer->device->CreateInputLayout(inputLayoutDesc,
        totalLayoutElements,
        shader->vertexShaderCompiled->GetBufferPointer(),
        shader->vertexShaderCompiled->GetBufferSize(),
        &buffer->layout);

    return buffer;

}

TMBuffer* TMRendererBufferCreate(TMRenderer* renderer,
    float* vertices, unsigned int verticesCount,
    float* uvs, unsigned int uvsCount,
    float* normals, unsigned int normalsCount,
    unsigned short* indices, unsigned int indicesCount,
    TMShader *shader) {
    
    TMBuffer* buffer = (TMBuffer*)TMMemoryPoolAlloc(renderer->buffersMemory);
    
    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));

    D3D11_BUFFER_DESC vertexDesc;
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(TMVec3) * verticesCount;
    resourceData.pSysMem = vertices;
    HRESULT result = renderer->device->CreateBuffer(&vertexDesc, &resourceData, &buffer->vertices[0]);
    if (FAILED(result)) {
        printf("Error: failed vertex buffer creation ...\n");
        return NULL;
    }
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(TMVec2) * uvsCount;
    resourceData.pSysMem = uvs;
    result = renderer->device->CreateBuffer(&vertexDesc, &resourceData, &buffer->vertices[1]);
    if (FAILED(result)) {
        printf("Error: failed vertex buffer creation ...\n");
        return NULL;
    }
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(TMVec3) * normalsCount;
    resourceData.pSysMem = normals;
    result = renderer->device->CreateBuffer(&vertexDesc, &resourceData, &buffer->vertices[2]);
    if (FAILED(result)) {
        printf("Error: failed vertex buffer creation ...\n");
        return NULL;
    }

    D3D11_BUFFER_DESC indexDesc;
    ZeroMemory(&indexDesc, sizeof(indexDesc));
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.ByteWidth = sizeof(unsigned short) * indicesCount;
    resourceData.pSysMem = indices;
    result = renderer->device->CreateBuffer(&indexDesc, &resourceData, &buffer->indices);
    if (FAILED(result)) {
        printf("Error: failed index buffer creation ...\n");
        return NULL;
    }
 
    buffer->verticesCount = verticesCount;
    buffer->indicesCount = indicesCount;
    buffer->numBuffers = 3;
    buffer->stride[0] = sizeof(TMVec3);
    buffer->stride[1] = sizeof(TMVec2);
    buffer->stride[2] = sizeof(TMVec3);
    buffer->offset[0] = 0;
    buffer->offset[1] = 0;
    buffer->offset[2] = 0;
    buffer->indexBufferFormat = DXGI_FORMAT_R16_UINT;

    // create input layout.
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
        1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT,
        2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    int totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = renderer->device->CreateInputLayout(inputLayoutDesc,
        totalLayoutElements,
        shader->vertexShaderCompiled->GetBufferPointer(),
        shader->vertexShaderCompiled->GetBufferSize(),
        &buffer->layout);

    return buffer;

}

void TMRendererBufferDestroy(TMRenderer* renderer, TMBuffer* buffer) {
    if (buffer->layout) buffer->layout->Release();
    if (buffer->vertices[0]) buffer->vertices[0]->Release();
    if (buffer->vertices[1]) buffer->vertices[1]->Release();
    if (buffer->vertices[2]) buffer->vertices[2]->Release();
    if (buffer->indices) buffer->indices->Release();
    TMMemoryPoolFree(renderer->buffersMemory, (void*)buffer);
}

void TMRendererDrawBufferElements(TMRenderer *renderer, TMBuffer* buffer) {
    renderer->deviceContext->IASetInputLayout(buffer->layout);
    renderer->deviceContext->IASetVertexBuffers(0, buffer->numBuffers, buffer->vertices, buffer->stride, buffer->offset);
    renderer->deviceContext->IASetIndexBuffer(buffer->indices, buffer->indexBufferFormat, 0);
    renderer->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    renderer->deviceContext->DrawIndexed(buffer->indicesCount, 0, 0);
}

void TMRendererDrawBufferArray(TMRenderer* renderer, TMBuffer* buffer) {
    renderer->deviceContext->IASetInputLayout(buffer->layout);
    renderer->deviceContext->IASetVertexBuffers(0, buffer->numBuffers, buffer->vertices, buffer->stride, buffer->offset);
    renderer->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    renderer->deviceContext->Draw(buffer->verticesCount, 0);
}


TMShader *TMRendererShaderCreate(TMRenderer *renderer, const char *vertPath, const char *fragPath) {
    TMShader* shader = (TMShader*)TMMemoryPoolAlloc(renderer->shadersMemory);

    // read the vertex and fragment files
    TMFile vertFile = TMFileOpen(vertPath);
    TMFile fragFile = TMFileOpen(fragPath);
    const char* vertSource = (const char*)vertFile.data;
    const char* fragSource = (const char*)fragFile.data;

    HRESULT result = {};
    ID3DBlob* errorVertexShader = 0;
    result = D3DCompile((void*)vertSource, vertFile.size,
        0, 0, 0, "VS_Main", "vs_4_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0,
        &shader->vertexShaderCompiled, &errorVertexShader);
    if (errorVertexShader != 0) {
        const char* errorString = (const char*)errorVertexShader->GetBufferPointer();
        printf("ERROR VERTEX SHADER: %s\n", errorString);
        errorVertexShader->Release();

        return NULL;
    }

    ID3DBlob* errorFragmentShader = 0;
    result = D3DCompile((void*)fragSource, fragFile.size,
        0, 0, 0, "PS_Main", "ps_4_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0,
        &shader->fragmentShaderCompile, &errorFragmentShader);
    if (errorFragmentShader != 0) {
        const char* errorString = (const char*)errorFragmentShader->GetBufferPointer();
        printf("ERROR PIXEL SHADER: %s\n", errorString);
        errorFragmentShader->Release();
    }

    // create vertex and fragment shaders
    result = renderer->device->CreateVertexShader(
        shader->vertexShaderCompiled->GetBufferPointer(),
        shader->vertexShaderCompiled->GetBufferSize(), 0,
        &shader->vertex);
    result = renderer->device->CreatePixelShader(
        shader->fragmentShaderCompile->GetBufferPointer(),
        shader->fragmentShaderCompile->GetBufferSize(), 0,
        &shader->pixel);

    return shader;
}

void TMRendererShaderDestroy(TMRenderer* renderer, TMShader* shader) {
    if(shader->vertexShaderCompiled) shader->vertexShaderCompiled->Release();
    if(shader->fragmentShaderCompile) shader->fragmentShaderCompile->Release();
    if (shader->vertex) shader->vertex->Release();
    if (shader->pixel)  shader->pixel->Release();
    TMMemoryPoolFree(renderer->shadersMemory, (void*)shader);
}

void TMRendererBindShader(TMRenderer *renderer, TMShader* shader) {
    renderer->deviceContext->VSSetShader(shader->vertex, 0, 0);
    renderer->deviceContext->PSSetShader(shader->pixel, 0, 0);
}


TMShaderBuffer* TMRendererShaderBufferCreate(TMRenderer* renderer, void *bufferData, size_t bufferSize, unsigned int index) {
    TMShaderBuffer* shaderBuffer = (TMShaderBuffer*)TMMemoryPoolAlloc(renderer->shaderBuffersMemory);

    // Initialize constant buffer
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = bufferSize;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    HRESULT result = renderer->device->CreateBuffer(&constantBufferDesc, 0, &shaderBuffer->buffer);
    if (FAILED(result)) {
        printf("Error: failed creating shader buffer\n");
    }
    renderer->deviceContext->UpdateSubresource(shaderBuffer->buffer, 0, 0, bufferData, 0, 0);
    renderer->deviceContext->VSSetConstantBuffers(0, 1, &shaderBuffer->buffer);
    return shaderBuffer;

}

void TMRendererShaderBufferDestroy(TMRenderer* renderer, TMShaderBuffer* shaderBuffer) {
    if (shaderBuffer->buffer) shaderBuffer->buffer->Release();
    TMMemoryPoolFree(renderer->shaderBuffersMemory, (void*)shaderBuffer);
}

void TMRendererShaderBufferUpdate(TMRenderer* renderer, TMShaderBuffer* shaderBuffer, void* bufferData) {
    renderer->deviceContext->UpdateSubresource(shaderBuffer->buffer, 0, 0, bufferData, 0, 0);
    renderer->deviceContext->VSSetConstantBuffers(0, 1, &shaderBuffer->buffer);
}

TMTexture* TMRendererTextureCreate(TMRenderer* renderer, const char* filepath) {
    TMTexture* texture = (TMTexture*)TMMemoryPoolAlloc(renderer->texturesMemory);

    int width, height, nrChannels;
    unsigned char* pixels = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (!pixels) {
        printf("ERROR: loading png %s falied\n", filepath);
        return NULL;
    }

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = pixels;
    data.SysMemPitch = width * sizeof(unsigned int);
    data.SysMemSlicePitch = 0;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 0; // use 0 to generate a full set of subtextures (mipmaps)
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    ID3D11Texture2D* tempTexture;
    HRESULT result = renderer->device->CreateTexture2D(&textureDesc, 0, &tempTexture);
    if (FAILED(result))
    {
        printf("Error: falied Creating texture\n");
        return NULL;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
    shaderResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// DXGI_FORMAT_R8G8B8A8_UNORM;
    shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceDesc.Texture2D.MipLevels = -1;
    shaderResourceDesc.Texture2D.MostDetailedMip = 0;
    result = renderer->device->CreateShaderResourceView(tempTexture, &shaderResourceDesc, &texture->colorMap);
    if (FAILED(result))
    {
        printf("Error: falied Creating Shader resource view\n");
    }
    renderer->deviceContext->UpdateSubresource(tempTexture, 0, 0, data.pSysMem, data.SysMemPitch, 0);
    renderer->deviceContext->GenerateMips(texture->colorMap);

    tempTexture->Release();

    D3D11_SAMPLER_DESC colorMapDesc = {};
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//D3D11_TEXTURE_ADDRESS_CLAMP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//D3D11_TEXTURE_ADDRESS_CLAMP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//D3D11_TEXTURE_ADDRESS_CLAMP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //D3D11_FILTER_MIN_MAG_MIP_LINEAR | D3D11_FILTER_MIN_MAG_MIP_POINT
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = renderer->device->CreateSamplerState(&colorMapDesc, &texture->colorMapSampler);
    if (FAILED(result))
    {
        printf("Error: Failed Creating sampler state\n");
    }

    texture->width = width;
    texture->height = height;
    
    stbi_image_free(pixels);

    return texture;
}

void TMRendererTextureDestroy(TMRenderer* renderer, TMTexture* texture) {
    if (texture->colorMapSampler) texture->colorMapSampler->Release();
    if (texture->colorMap) texture->colorMap->Release();
    TMMemoryPoolFree(renderer->texturesMemory, (void*)texture);
}

void TMRendererTextureBind(TMRenderer *renderer, TMTexture* texture, TMShader* shader, const char* varName, int textureIndex) {
    renderer->deviceContext->PSSetShaderResources(0, 1, &texture->colorMap);
    renderer->deviceContext->PSSetSamplers(0, 1, &texture->colorMapSampler);
}

void TMRendererTextureUnbind(TMRenderer *renderer, TMTexture* texture, int textureIndex) {
    renderer->deviceContext->PSSetShaderResources(0, 1, NULL);
    renderer->deviceContext->PSSetSamplers(0, 1, NULL);
}


TMFramebuffer* TMRendererFramebufferCreate(TMRenderer* renderer) {
    return NULL;
}

void TMRendererFramebufferDestroy(TMRenderer* renderer, TMFramebuffer* framebuffer) {

}
