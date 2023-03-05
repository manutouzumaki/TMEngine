#version 410 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;

out vec2 fragUV;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uWorld;

void main() {
   fragUV = inUV;
   gl_Position = uProj * uView * uWorld * vec4(inPosition, 1.0);
}
