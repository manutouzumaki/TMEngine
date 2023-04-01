#version 410

uniform sampler2D uTexture;

layout (location = 7) in vec2 fragUV;
layout (location = 8) in vec4 fragColor;

layout (location = 0) out vec4 outColor;

void main() {
    //vec4 textureColor = texture(uTexture, fragUV);
    outColor = fragColor;
}
