#include "../tm_renderer.h"
#include "../utils/tm_memory_pool.h"
#include "../utils/tm_file.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include <glext.h>
#include <stdlib.h>
#include <stdio.h>


// TODO: try to remove this ...
struct TMWindow {
    GLFWwindow *glfwWindow;
    int width;
    int height;
    bool updateRenderArea;
};

enum TM_EBO_TYPE {
    TM_EBO_UNSIGNED_UNDEFINE,
    TM_EBO_UNSIGNED_SHORT,
    TM_EBO_UNSIGNED_INT,
};

struct TMBuffer {
    unsigned int id;
    unsigned int vbo0;
    unsigned int vbo1;
    unsigned int vbo2;
    unsigned int ebo;
    unsigned int verticesCount;
    unsigned int indicesCount;
    TM_EBO_TYPE eboType;
};

struct TMShader {
    unsigned int id;
};

struct TMShaderBuffer {
    unsigned int id;
    unsigned int uboSize;
    unsigned int uboIndex;
};

struct TMTexture {
    unsigned int id;
    int width;
    int height;
};

struct TMFramebuffer {
    unsigned int id;
};

struct TMRenderBatch {
    TMRenderer *renderer;
    TMShader *shader;
    TMTexture *texture;

    unsigned int vao;
    unsigned int vbo;
    TMBatchVertex *batchBuffer;
    unsigned int bufferSizeInBytes;
    unsigned int size;
    unsigned int used;
};

struct TMInstanceRenderer {
    unsigned int vao;
    unsigned int vertBuffer;
    unsigned int instBuffer;
    unsigned int instCount;
    unsigned int instSize;
};

struct TMRenderer {
    int width;
    int height;
    TMWindow *window;
    TMMemoryPool *buffersMemory;
    TMMemoryPool *texturesMemory;
    TMMemoryPool *shadersMemory;
    TMMemoryPool *framebufferMemory;
    TMMemoryPool* shaderBuffersMemory;
    TMMemoryPool* renderBatchsMemory;
    TMMemoryPool* instanceRendererMemory;
};


 TMRenderer *TMRendererCreate(TMWindow *window) {
    TMRenderer *renderer = (TMRenderer *)malloc(sizeof(TMRenderer));
    renderer->buffersMemory = TMMemoryPoolCreate(sizeof(TMBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->texturesMemory = TMMemoryPoolCreate(sizeof(TMTexture), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shadersMemory = TMMemoryPoolCreate(sizeof(TMShader), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->framebufferMemory = TMMemoryPoolCreate(sizeof(TMFramebuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shaderBuffersMemory = TMMemoryPoolCreate(sizeof(TMShaderBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->renderBatchsMemory = TMMemoryPoolCreate(sizeof(TMRenderBatch), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->instanceRendererMemory = TMMemoryPoolCreate(sizeof(TMInstanceRenderer), TM_RENDERER_MEMORY_BLOCK_SIZE);


    renderer->window = window; 
    renderer->width = window->width;
    renderer->height = window->height;

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return renderer;
}

 void TMRendererDestroy(TMRenderer *renderer) {
    TMMemoryPoolDestroy(renderer->buffersMemory);
    TMMemoryPoolDestroy(renderer->texturesMemory);
    TMMemoryPoolDestroy(renderer->shadersMemory);
    TMMemoryPoolDestroy(renderer->framebufferMemory);
    TMMemoryPoolDestroy(renderer->shaderBuffersMemory);
    TMMemoryPoolDestroy(renderer->renderBatchsMemory);
    TMMemoryPoolDestroy(renderer->instanceRendererMemory);
    free(renderer);
}

 void TMRendererDepthTestEnable(TMRenderer *renderer) {
    glEnable(GL_DEPTH_TEST);
}

 void TMRendererDepthTestDisable(TMRenderer *renderer) {
    glDisable(GL_DEPTH_TEST);
}

 int TMRendererGetWidth(TMRenderer *renderer) {
    return renderer->width;
}

 int TMRendererGetHeight(TMRenderer *renderer) {
    return renderer->height;
}


// TODO(manuel): implement this function
 bool TMRendererUpdateRenderArea(TMRenderer *renderer, int *outWidth, int *outHeight) {
    *outWidth = renderer->window->width;
    *outHeight = renderer->window->height;
    // TODO: finish this function ...
    renderer->width = renderer->window->width;
    renderer->height = renderer->window->height;
    return renderer->window->updateRenderArea;
}

 void TMRendererFaceCulling(TMRenderer *renderer, bool value,  unsigned int flags) {
    if(value) {
        glEnable(GL_CULL_FACE);
        if (flags == TM_CULL_BACK) {
            glCullFace(GL_BACK);
            return;
        }
        if (flags == TM_CULL_FRONT) {
            glCullFace(GL_FRONT);
            return;
        }
        if (flags == (TM_CULL_BACK | TM_CULL_FRONT)) {
            glCullFace(GL_FRONT_AND_BACK);
            return;
        }
    } else {
        glDisable(GL_CULL_FACE);
    }
}

 void TMRendererClear(TMRenderer *renderer, float r, float g, float b, float a, unsigned  int flags) {
        glClearColor(r, g, b, a);
        unsigned int mask = 0;
        if(flags & TM_COLOR_BUFFER_BIT) mask |= GL_COLOR_BUFFER_BIT;
        if(flags & TM_DEPTH_BUFFER_BIT) mask |= GL_DEPTH_BUFFER_BIT;
        if(flags & TM_STENCIL_BUFFER_BIT) mask |= GL_STENCIL_BUFFER_BIT;
        glClear(mask);
}

// TODO(manuel): see where this functio should be implemented
 void TMRendererPresent(TMRenderer *renderer, int vsync) {
    // TODO ...
}

 TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                  TMVertex *vertices,
                                  unsigned int verticesCount,
                                  TMShader *shader) {
    TMBuffer *buffer = (TMBuffer *)TMMemoryPoolAlloc(renderer->buffersMemory);

    unsigned int VAO, VBO;
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMVertex) * verticesCount, vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    buffer->id = VAO;
    buffer->vbo0 = VBO;
    buffer->verticesCount = verticesCount;

    return buffer;
}

 TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                  TMVertex *vertices,      unsigned int verticesCount,
                                  unsigned int *indices, unsigned int indicesCount,
                                  TMShader *shader) {
    TMBuffer *buffer = (TMBuffer *)TMMemoryPoolAlloc(renderer->buffersMemory);

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMVertex) * verticesCount, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    buffer->id = VAO;
    buffer->vbo0 = VBO;
    buffer->ebo = EBO;
    buffer->eboType = TM_EBO_UNSIGNED_INT;
    buffer->verticesCount = verticesCount;
    buffer->indicesCount = indicesCount;

    return buffer;

}

 TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                      float *vertices,        unsigned int verticesCount,
                                      float *uvs,              unsigned int uvsCount,
                                      float *normals,          unsigned int normalsCount,
                                      unsigned short *indices, unsigned int indicesCount,
                                      TMShader *shader) {
    TMBuffer *buffer = (TMBuffer *)TMMemoryPoolAlloc(renderer->buffersMemory);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBOs[3];
    glGenBuffers(3, VBOs);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMVec3) * verticesCount, vertices, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMVec2) * uvsCount, uvs, GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMVec3) * normalsCount, normals, GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    unsigned int EBO;

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indicesCount, indices, GL_STATIC_DRAW);

    buffer->vbo0 = VBOs[0];
    buffer->vbo1 = VBOs[1];
    buffer->vbo2 = VBOs[2];
    buffer->ebo = EBO;
    buffer->eboType = TM_EBO_UNSIGNED_SHORT;
    buffer->id = VAO;

    buffer->verticesCount = verticesCount;
    buffer->indicesCount = indicesCount;

    return buffer;

}

 void TMRendererBufferDestroy(TMRenderer *renderer, TMBuffer *buffer) {
    if(glIsBuffer(buffer->vbo0))     glDeleteBuffers(1, &buffer->vbo0);
    if(glIsBuffer(buffer->vbo1))     glDeleteBuffers(1, &buffer->vbo1);
    if(glIsBuffer(buffer->vbo2))     glDeleteBuffers(1, &buffer->vbo2);
    if(glIsBuffer(buffer->ebo))     glDeleteBuffers(1, &buffer->ebo);
    if(glIsVertexArray(buffer->id)) glDeleteVertexArrays(1, &buffer->id);
    TMMemoryPoolFree(renderer->buffersMemory, (void *)buffer);
}

void TMRendererDrawBufferElements(TMRenderer *renderer, TMBuffer *buffer) {
    glBindVertexArray(buffer->id);
    switch(buffer->eboType) {
        case TM_EBO_UNSIGNED_SHORT: glDrawElements(GL_TRIANGLES, buffer->indicesCount, GL_UNSIGNED_SHORT, 0); break;
        case TM_EBO_UNSIGNED_INT: glDrawElements(GL_TRIANGLES, buffer->indicesCount, GL_UNSIGNED_INT, 0); break;
        case TM_EBO_UNSIGNED_UNDEFINE:
        default: printf("Error: your buffer dont have indices, try use TMRendererDrawBufferArray\n"); break;
    }
}

void TMRendererDrawBufferArray(TMRenderer *renderer, TMBuffer *buffer) {
    glBindVertexArray(buffer->id);
    glDrawArrays(GL_TRIANGLES, 0, buffer->verticesCount);
}

TMShader *TMRendererShaderCreate(TMRenderer *renderer, const char *vertPath, const char *fragPath) {
    TMShader *shader = (TMShader *)TMMemoryPoolAlloc(renderer->shadersMemory);

    // read the vertex and fragment files
    TMFile vertFile = TMFileOpen(vertPath);
    TMFile fragFile = TMFileOpen(fragPath);
    const char *vertSource = (const char *)vertFile.data;
    const char *fragSource = (const char *)fragFile.data;

    int success;
    char infoLog[512];

    // compile and create the vertex shader
    unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n %s\n", vertPath, infoLog);
    }
    
    // compile and create the fragment shader
    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n %s\n", fragPath, infoLog);
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED: %s\n%s\n %s\n",vertPath, fragPath, infoLog);
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    
    shader->id = program;

    // free the memory use by TMFile
    TMFileClose(&vertFile);
    TMFileClose(&fragFile);

    return shader;
}

 void TMRendererShaderDestroy(TMRenderer *renderer, TMShader *shader) {
    glDeleteProgram(shader->id);
    TMMemoryPoolFree(renderer->shadersMemory, (void *)shader);
}

 void TMRendererBindShader(TMRenderer *renderer, TMShader *shader) {
    glUseProgram(shader->id);
}

 void TMRendererUnbindShader(TMRenderer *renderer, TMShader *shader) {
    glUseProgram(0);
}

TMShaderBuffer* TMRendererShaderBufferCreate(TMRenderer* renderer, void *bufferData, size_t bufferSize, unsigned int index) {
    TMShaderBuffer *shaderBuffer = (TMShaderBuffer *)TMMemoryPoolAlloc(renderer->shaderBuffersMemory);

    shaderBuffer->uboSize = bufferSize;
    shaderBuffer->uboIndex = index;

    glGenBuffers(1, &shaderBuffer->id);
    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer->id);
    glBufferData(GL_UNIFORM_BUFFER, shaderBuffer->uboSize, bufferData, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, shaderBuffer->uboIndex, shaderBuffer->id);

    return shaderBuffer;
}

void TMRendererShaderBufferDestroy(TMRenderer* renderer, TMShaderBuffer* shaderBuffer) {
    TMMemoryPoolFree(renderer->shaderBuffersMemory, (void *)shaderBuffer);
}

void TMRendererShaderBufferUpdate(TMRenderer* renderer, TMShaderBuffer* shaderBuffer, void* bufferData) {
    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer->id);
    glBufferData(GL_UNIFORM_BUFFER, shaderBuffer->uboSize, bufferData, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, shaderBuffer->uboIndex, shaderBuffer->id);
}

 TMTexture *TMRendererTextureCreate(TMRenderer *renderer, const char *filepath) {
    TMTexture *texture = (TMTexture *)TMMemoryPoolAlloc(renderer->texturesMemory);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if(!data) {
        printf("ERROR: loading png %s falied\n", filepath);
        return NULL;
    }

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     
    unsigned int format;
    switch(nrChannels) {
        case 3: {
            format = GL_RGB;
        } break;
        case 4: {
            format = GL_RGBA;
        } break;
        default: {
            printf("ERROR: Image format not supported\n");
            return NULL;
        } break;
    }

        // manuel: Load the texture into VRAM
    glTexImage2D(
            GL_TEXTURE_2D, // target
            0, // mip level
            GL_RGBA, // internal format, often advisable to use BGR
            width, // width of the texture
            height, // height of the texture
            0, // border (always 0)
            format, // format
            GL_UNSIGNED_BYTE, // type
            data // Data to upload
    );

    glGenerateMipmap(GL_TEXTURE_2D);


    stbi_image_free(data);

    texture->id = textureId;
    texture->width = width;
    texture->height = height;

    return texture;
}

void TMRendererTextureBind(TMRenderer *renderer, TMTexture *texture, TMShader *shader, const char *varName, int textureIndex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(renderer, shader);
    glUniform1i(varLoc, textureIndex);

}

 void TMRendererTextureUnbind(TMRenderer *renderer, TMTexture *texture, int textureIndex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

 void TMRendererTextureDestroy(TMRenderer *renderer, TMTexture *texture) {
    glDeleteTextures(1, &texture->id);    
    TMMemoryPoolFree(renderer->texturesMemory, (void *)texture);
}

 TMFramebuffer *TMRendererFramebufferCreate(TMRenderer *renderer) {
    TMFramebuffer *framebuffer = (TMFramebuffer *)TMMemoryPoolAlloc(renderer->framebufferMemory);
    return framebuffer;
}

 void TMRendererFramebufferDestroy(TMRenderer *renderer, TMFramebuffer *framebuffer) {
    TMMemoryPoolFree(renderer->framebufferMemory, (void *)framebuffer);
}

TMRenderBatch *TMRendererRenderBatchCreate(TMRenderer *renderer, TMShader *shader, TMTexture *texture, size_t size) {
    TMRenderBatch *renderBatch = (TMRenderBatch *)TMMemoryPoolAlloc(renderer->renderBatchsMemory);

    renderBatch->renderer = renderer;
    renderBatch->shader = shader;
    renderBatch->texture = texture;
    renderBatch->size = size;
    renderBatch->used = 0;
    renderBatch->bufferSizeInBytes = sizeof(TMBatchVertex)*6*size;

    unsigned int VAO, VBO;
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, renderBatch->bufferSizeInBytes, NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMBatchVertex), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TMBatchVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    renderBatch->vao = VAO;
    renderBatch->vbo = VBO;

    renderBatch->batchBuffer = (TMBatchVertex *)malloc(renderBatch->bufferSizeInBytes);
    memset(renderBatch->batchBuffer, 0, renderBatch->bufferSizeInBytes);

    return renderBatch;
}

static void BatchQuadLocalToWorld(TMBatchVertex *quad, float x, float y, float z, float w, float h, float angle) {
    // TODO: test the performance of this matrix multiplycation, maybe if fast to do it directly ...
    TMMat4 world = TMMat4Translate(x, y, z) * TMMat4RotateZ(angle) * TMMat4Scale(w, h, 1);
    for(int i = 0; i < 6; ++i) {
        quad[i].position = TMMat4TransformPoint(world, quad[i].position);
    }
}

static void BatchQuadHandleUVs(TMBatchVertex *quad, int sprite, float *uvs) {
   int uvsIndex = sprite * 4;
   float u0 = uvs[uvsIndex + 0]; 
   float v0 = uvs[uvsIndex + 1];
   float u1 = uvs[uvsIndex + 2];
   float v1 = uvs[uvsIndex + 3];
   quad[0].uvs = {u0, v0};
   quad[1].uvs = {u1, v0};
   quad[2].uvs = {u0, v1};
   quad[3].uvs = {u0, v1};
   quad[4].uvs = {u1, v0};
   quad[5].uvs = {u1, v1};
}

static void AddQuadToBatchBuffer(TMRenderBatch *renderBatch, TMBatchVertex *quad) {
    assert(renderBatch->used < renderBatch->size);
    TMBatchVertex *vertex = renderBatch->batchBuffer + (renderBatch->used*6);
    memcpy(vertex, quad, sizeof(TMBatchVertex)*6);
    ++renderBatch->used;
}

void TMRendererRenderBatchAdd(TMRenderBatch *renderBatch, float x, float y, float z,
                                                          float w, float h, float angle) {
    TMBatchVertex quad[] = {
        TMBatchVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}}, // 1
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}}, // 0
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}}, // 2
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}}, // 2
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}}, // 0
        TMBatchVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}}  // 3
    };
    BatchQuadLocalToWorld(quad, x, y, z, w, h, angle);

    if(renderBatch->used + 1 >= renderBatch->size) {
        TMRendererRenderBatchDraw(renderBatch);
    }
    AddQuadToBatchBuffer(renderBatch, quad);
}

void TMRendererRenderBatchAdd(TMRenderBatch *renderBatch, float x, float y, float z,
                                                          float w, float h, float angle,
                                                          float r, float g, float b, float a) {
    TMBatchVertex quad[] = {
        TMBatchVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec4{r, g, b, a}}, // 1
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec4{r, g, b, a}}, // 0
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec4{r, g, b, a}}, // 2
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec4{r, g, b, a}}, // 2
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec4{r, g, b, a}}, // 0
        TMBatchVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec4{r, g, b, a}}  // 3
    };
    BatchQuadLocalToWorld(quad, x, y, z, w, h, angle);

    if(renderBatch->used + 1 >= renderBatch->size) {
        TMRendererRenderBatchDraw(renderBatch);
    }
    AddQuadToBatchBuffer(renderBatch, quad);
}

void TMRendererRenderBatchAdd(TMRenderBatch *renderBatch, float x, float y, float z,
                                                          float w, float h, float angle,
                                                          int sprite, float *uvs) {
    TMBatchVertex quad[] = {
        TMBatchVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}}, // 1
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}}, // 0
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}}, // 2
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}}, // 2
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}}, // 0
        TMBatchVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}}  // 3
    };
    BatchQuadLocalToWorld(quad, x, y, z, w, h, angle);
    BatchQuadHandleUVs(quad, sprite, uvs);

    if(renderBatch->used + 1 >= renderBatch->size) {
        TMRendererRenderBatchDraw(renderBatch);
    }
    AddQuadToBatchBuffer(renderBatch, quad);
}

void TMRendererRenderBatchDraw(TMRenderBatch *renderBatch) {
    TMRenderer *renderer = renderBatch->renderer;
    
    glBindVertexArray(renderBatch->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderBatch->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderBatch->used*sizeof(TMBatchVertex)*6, renderBatch->batchBuffer);    


    TMRendererBindShader(renderer, renderBatch->shader);
    TMRendererTextureBind(renderer, renderBatch->texture, renderBatch->shader, "uTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, renderBatch->used*6);


    renderBatch->used = 0;

}

void TMRendererRenderBatchDestroy(TMRenderer *renderer, TMRenderBatch *renderBatch) {
    if(renderBatch->batchBuffer) free(renderBatch->batchBuffer);
    if(glIsBuffer(renderBatch->vbo))     glDeleteBuffers(1, &renderBatch->vbo);
    if(glIsVertexArray(renderBatch->vao)) glDeleteVertexArrays(1, &renderBatch->vao);
    TMMemoryPoolFree(renderer->renderBatchsMemory, (void *)renderBatch);
}

float *TMGenerateUVs(TMTexture *texture, int tileWidth, int tileHeight, int *count) {
    float width = (float)tileWidth / (float)texture->width;
    float height = (float)tileHeight / (float)texture->height;
    int cols = texture->width / tileWidth;
    int rows = texture->height / tileHeight;
    float *uvs = (float *)malloc(cols * rows * 4 * sizeof(float));
    *count = cols * rows;
    
    float ux = 0.0f;
    float uy = 0.0f;
    float vx = width;
    float vy = height;
    
    float *uvsPtr = uvs;
    for(int j = 0; j < rows; ++j) {
        for(int i = 0; i < cols; ++i) {
            *uvsPtr++ = ux;
            *uvsPtr++ = uy;
            *uvsPtr++ = vx;
            *uvsPtr++ = vy;

            ux += width;
            vx += width;
        }
        ux = 0;
        vx = width;
        uy += height;
        vy += height;
    }
    return uvs;
}

float *TMGenerateUVs(int texWidth, int texHeight, int tileWidth, int tileHeight, int *count) {
    float width = (float)tileWidth / (float)texWidth;
    float height = (float)tileHeight / (float)texHeight;
    int cols = texWidth / tileWidth;
    int rows = texHeight / tileHeight;
    float *uvs = (float *)malloc(cols * rows * 4 * sizeof(float));
    *count = cols * rows;
    
    float ux = 0.0f;
    float uy = 0.0f;
    float vx = width;
    float vy = height;
    
    float *uvsPtr = uvs;
    for(int j = 0; j < rows; ++j) {
        for(int i = 0; i < cols; ++i) {
            *uvsPtr++ = ux;
            *uvsPtr++ = uy;
            *uvsPtr++ = vx;
            *uvsPtr++ = vy;

            ux += width;
            vx += width;
        }
        ux = 0;
        vx = width;
        uy += height;
        vy += height;
    }
    return uvs;
}


TMInstanceRenderer *TMRendererInstanceRendererCreate(TMRenderer *renderer, TMShader *shader, unsigned int instCount, unsigned int instSize) {
    TMInstanceRenderer *instanceRenderer = (TMInstanceRenderer *)TMMemoryPoolAlloc(renderer->instanceRendererMemory);

    // create vertex buffer
    TMBatchVertex quad[] = {
        TMBatchVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}}, // 1
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}}, // 0
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}}, // 2
        TMBatchVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}}, // 2
        TMBatchVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}}, // 0
        TMBatchVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}}  // 3
    };

    unsigned int VAO, vertVBO, instVBO;
    


    // create instance buffer
    glGenBuffers(1, &instVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instVBO);
    glBufferData(GL_ARRAY_BUFFER, instCount*instSize, NULL, GL_STATIC_DRAW);



    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // create vertex buffer
    glGenBuffers(1, &vertVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMBatchVertex)*6, quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMBatchVertex), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TMBatchVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, instVBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, instSize, (void *)0);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, instSize, (void *)(4 * sizeof(float)));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, instSize, (void *)(8 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, instSize, (void *)(12 * sizeof(float)));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, instSize, (void *)(16 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    instanceRenderer->vao = VAO;
    instanceRenderer->vertBuffer = vertVBO;
    instanceRenderer->instBuffer = instVBO;
    instanceRenderer->instCount = instCount;
    instanceRenderer->instSize = instSize;

    return instanceRenderer;
}

void TMRendererInstanceRendererDraw(TMRenderer *renderer, TMInstanceRenderer *instanceRenderer, void *buffer) {
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceRenderer->instBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceRenderer->instCount*instanceRenderer->instSize, buffer);    

    glBindVertexArray(instanceRenderer->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceRenderer->instCount);  


}

void TMRendererInstanceRendererDestroy(TMRenderer *renderer, TMInstanceRenderer *instanceRenderer) {
    if(glIsBuffer(instanceRenderer->vertBuffer)) glDeleteBuffers(1, &instanceRenderer->vertBuffer);
    if(glIsBuffer(instanceRenderer->instBuffer)) glDeleteBuffers(1, &instanceRenderer->instBuffer);
    if(glIsVertexArray(instanceRenderer->vao)) glDeleteVertexArrays(1, &instanceRenderer->vao);
    TMMemoryPoolFree(renderer->instanceRendererMemory, (void *)instanceRenderer);
}
