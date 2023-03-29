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

struct TMRenderer {
    int width;
    int height;
    TMMemoryPool *buffersMemory;
    TMMemoryPool *texturesMemory;
    TMMemoryPool *shadersMemory;
    TMMemoryPool *framebufferMemory;
    TMMemoryPool* shaderBuffersMemory;
};


 TMRenderer *TMRendererCreate(TMWindow *window) {
    TMRenderer *renderer = (TMRenderer *)malloc(sizeof(TMRenderer));
    renderer->buffersMemory = TMMemoryPoolCreate(sizeof(TMBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->texturesMemory = TMMemoryPoolCreate(sizeof(TMTexture), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shadersMemory = TMMemoryPoolCreate(sizeof(TMShader), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->framebufferMemory = TMMemoryPoolCreate(sizeof(TMFramebuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shaderBuffersMemory = TMMemoryPoolCreate(sizeof(TMShaderBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    
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
 bool TMRendererUpdateRenderArea(TMRenderer *renderer) {
    return false;
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
 void TMRendererPresent(TMRenderer *renderer) {
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
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED:\n %s\n", infoLog);
    }
    
    // compile and create the fragment shader
    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:\n %s\n", infoLog);
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s\n", infoLog);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     
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
