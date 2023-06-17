#version 410

uniform mtx {
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (location = 3) out vec2 Tex0;
layout (location = 4) out vec4 Color;

void main() {
    Tex0 = inUV;
    Color = inColor;
    gl_Position = uProj * uView * vec4(inPosition, 1.0);
}
