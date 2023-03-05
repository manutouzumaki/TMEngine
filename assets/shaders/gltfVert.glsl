#version 410 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

out vec2 fragUV;
out vec3 fragNorm;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uWorld;

void main() {
   fragUV = inUV;
   fragNorm = mat3(transpose(inverse(uWorld))) * inNormal; 
   gl_Position = uProj * uView * uWorld * vec4(inPosition, 1.0);
}
