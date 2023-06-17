#version 410

uniform sampler2D uTexture;

layout (location = 3) in vec2 Tex0;
layout (location = 4) in vec4 Color;

layout (location = 0) out vec4 outColor;

void main() {
    vec4 textureColor = texture(uTexture, Tex0);
    float t = (Color.w != 0) ? 1 : 0;
    outColor = (1 - t) * textureColor + t * Color;
}
