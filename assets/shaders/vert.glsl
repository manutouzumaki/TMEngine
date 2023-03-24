#version 410
// #extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 3) out vec2 fragUV;

layout (std140) uniform mtx {
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
};

void main() {
   fragUV = inUV;
   gl_Position = uProj * uView * uWorld * vec4(inPosition, 1.0);
}
