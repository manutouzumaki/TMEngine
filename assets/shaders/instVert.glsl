#version 410
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in mat4 inWorld;
layout (location = 6) in vec4 inColor;

layout (location = 7) out vec2 fragUV;
layout (location = 8) out vec4 fragColor;

uniform mtx {
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
};

void main() {
    fragUV = inUV;
    fragColor = inColor;
    gl_Position = uProj * uView * inWorld * vec4(inPosition, 1.0);
}
