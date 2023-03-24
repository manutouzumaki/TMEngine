#version 410
// #extension GL_ARB_separate_shader_objects : enable

uniform sampler2D uTexture;

layout (location = 3) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

void main() {
   outColor = texture(uTexture, fragUV);
}
