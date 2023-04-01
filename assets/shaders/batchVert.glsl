#version 410
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;

layout (location = 2) out vec2 fragUV;

uniform mtx {
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
};

void main() {
    fragUV = inUV;
    gl_Position = uProj * uView * vec4(inPosition, 1.0);
}
