#include "tm_renderer.h"
#include "utils/tm_memory_pool.h"
#include "utils/tm_file.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

// TODO: memset zero all the return structs


#define TM_EXPORT __attribute__((visibility("default")))
#define TM_RENDERER_MEMORY_BLOCK_SIZE 100

struct TMBuffer {
    unsigned int id;
    unsigned int vbo0;
    unsigned int vbo1;
    unsigned int vbo2;
    unsigned int ebo;
    unsigned int verticesCount;
    unsigned int indicesCount;
};

struct TMShader {
    unsigned int id;
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
};

TM_EXPORT TMRenderer *TMRendererCreate() {
    TMRenderer *renderer = (TMRenderer *)malloc(sizeof(TMRenderer));
    renderer->buffersMemory = TMMemoryPoolCreate(sizeof(TMBuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->texturesMemory = TMMemoryPoolCreate(sizeof(TMTexture), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->shadersMemory = TMMemoryPoolCreate(sizeof(TMShader), TM_RENDERER_MEMORY_BLOCK_SIZE);
    renderer->framebufferMemory = TMMemoryPoolCreate(sizeof(TMFramebuffer), TM_RENDERER_MEMORY_BLOCK_SIZE);
    
    renderer->width = 1080/4;
    renderer->height = 1920/4;

    //renderer->width = 800;
    //renderer->height = 600;

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return renderer;
}

TM_EXPORT void TMRendererDestroy(TMRenderer *renderer) {
    TMMemoryPoolDestroy(renderer->buffersMemory);
    TMMemoryPoolDestroy(renderer->texturesMemory);
    TMMemoryPoolDestroy(renderer->shadersMemory);
    TMMemoryPoolDestroy(renderer->framebufferMemory);
    free(renderer);
}

TM_EXPORT void TMRendererDepthTestEnable() {
    glEnable(GL_DEPTH_TEST);
}

TM_EXPORT void TMRendererDepthTestDisable() {
    glDisable(GL_DEPTH_TEST);
}

// TODO(manuel): set up the renderer->width and renderer->height fields
TM_EXPORT int TMRendererGetWidth(TMRenderer *renderer) {
    return renderer->width;
}

TM_EXPORT int TMRendererGetHeight(TMRenderer *renderer) {
    return renderer->height;
}


// TODO(manuel): implement this function
TM_EXPORT bool TMRendererUpdateRenderArea(TMRenderer *renderer) {
    return false;
}

TM_EXPORT void TMRendererFaceCulling(bool value,  unsigned int flags) {
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

TM_EXPORT void TMRendererClear(float r, float g, float b, float a, unsigned  int flags) {
        glClearColor(r, g, b, a);
        unsigned int mask = 0;
        if(flags & TM_COLOR_BUFFER_BIT) mask |= GL_COLOR_BUFFER_BIT;
        if(flags & TM_DEPTH_BUFFER_BIT) mask |= GL_DEPTH_BUFFER_BIT;
        if(flags & TM_STENCIL_BUFFER_BIT) mask |= GL_STENCIL_BUFFER_BIT;
        glClear(mask);
}

// TODO(manuel): see where this functio should be implemented
TM_EXPORT void TMRendererPresent(TMRenderer *renderer) {
    // TODO ...
}

TM_EXPORT TMBuffer *TMRendererBufferCreate(TMRenderer *renderer, TMVertex *vertices, unsigned int verticesCount) {
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

    buffer->id = VAO;
    buffer->vbo0 = VBO;
    buffer->verticesCount = verticesCount;

    return buffer;
}

TM_EXPORT TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                           TMVertex *vertices,      unsigned int verticesCount,
                                           unsigned short *indices, unsigned int indicesCount) {
    TMBuffer *buffer = (TMBuffer *)TMMemoryPoolAlloc(renderer->buffersMemory);

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TMVertex) * verticesCount, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indicesCount, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TMVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    buffer->id = VAO;
    buffer->vbo0 = VBO;
    buffer->ebo = EBO;
    buffer->verticesCount = verticesCount;
    buffer->indicesCount = indicesCount;

    return buffer;

}

TM_EXPORT TMBuffer *TMRendererBufferCreate(TMRenderer *renderer,
                                      float *vertices,        unsigned int verticesCount,
                                      float *uvs,              unsigned int uvsCount,
                                      float *normals,          unsigned int normalsCount,
                                      unsigned short *indices, unsigned int indicesCount) {
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
    buffer->id = VAO;

    buffer->verticesCount = verticesCount;
    buffer->indicesCount = indicesCount;

    return buffer;

}

TM_EXPORT void TMRendererBufferDestroy(TMRenderer *renderer, TMBuffer *buffer) {
    if(glIsBuffer(buffer->vbo0))     glDeleteBuffers(1, &buffer->vbo0);
    if(glIsBuffer(buffer->vbo1))     glDeleteBuffers(1, &buffer->vbo1);
    if(glIsBuffer(buffer->vbo2))     glDeleteBuffers(1, &buffer->vbo2);
    if(glIsBuffer(buffer->ebo))     glDeleteBuffers(1, &buffer->ebo);
    if(glIsVertexArray(buffer->id)) glDeleteVertexArrays(1, &buffer->id);
    TMMemoryPoolFree(renderer->buffersMemory, (void *)buffer);
}

TM_EXPORT void TMRendererDrawBufferElements(TMBuffer *buffer) {
    glBindVertexArray(buffer->id);
    glDrawElements(GL_TRIANGLES, buffer->indicesCount, GL_UNSIGNED_SHORT, 0);
}

TM_EXPORT void TMRendererDrawBufferArray(TMBuffer *buffer) {
    glBindVertexArray(buffer->id);
    glDrawArrays(GL_TRIANGLES, 0, buffer->verticesCount);
}

TM_EXPORT TMShader *TMRendererShaderCreate(TMRenderer *renderer, const char *vertPath, const char *fragPath) {
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

TM_EXPORT void TMRendererShaderDestroy(TMRenderer *renderer, TMShader *shader) {
    glDeleteProgram(shader->id);
    TMMemoryPoolFree(renderer->shadersMemory, (void *)shader);
}

TM_EXPORT void TMRendererBindShader(TMShader *shader) {
    glUseProgram(shader->id);
}

TM_EXPORT void TMRendererUnbindShader(TMShader *shader) {
    glUseProgram(0);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, float value) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniform1f(varLoc, value);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, int value) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniform1i(varLoc, value);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, TMVec3 value) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniform3fv(varLoc, 1, value.v);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, TMVec4 value) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniform4fv(varLoc, 1, value.v);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, TMMat4 value) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniformMatrix4fv(varLoc, 1, false, value.v);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, int size, int *array) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniform1iv(varLoc, size, array);
}

TM_EXPORT void TMRendererShaderUpdate(TMShader *shader, const char *varName, int size, TMMat4 *array) {
    int varLoc = glGetUniformLocation(shader->id, varName);
    TMRendererBindShader(shader);
    glUniformMatrix4fv(varLoc, size, false, (float *)array);
}

TM_EXPORT TMTexture *TMRendererTextureCreate(TMRenderer *renderer, const char *filepath) {
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

TM_EXPORT void TMRendererTextureBind(TMTexture *texture, TMShader *shader, const char *varName, int textureIndex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    TMRendererShaderUpdate(shader, varName, textureIndex);
}

TM_EXPORT void TMRendererTextureUnbind(TMTexture *texture, int textureIndex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

TM_EXPORT void TMRendererTextureDestroy(TMRenderer *renderer, TMTexture *texture) {
    glDeleteTextures(1, &texture->id);    
    TMMemoryPoolFree(renderer->texturesMemory, (void *)texture);
}

TM_EXPORT TMFramebuffer *TMRendererFramebufferCreate(TMRenderer *renderer) {
    TMFramebuffer *framebuffer = (TMFramebuffer *)TMMemoryPoolAlloc(renderer->framebufferMemory);
    return framebuffer;
}

TM_EXPORT void TMRendererFramebufferDestroy(TMRenderer *renderer, TMFramebuffer *framebuffer) {
    TMMemoryPoolFree(renderer->framebufferMemory, (void *)framebuffer);
}
